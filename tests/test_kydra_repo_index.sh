#!/usr/bin/env bash
# Integration test for kydra-repo-index. Builds a handful of tiny packages in
# a temporary folder, indexes them in both layouts the helper knows, and
# checks what apt is told - finally by pointing a private, unprivileged apt at
# the result. Nothing outside the temporary folder is read or written.
#
#   tests/test_kydra_repo_index.sh [path/to/kydra-repo-index]
set -euo pipefail

HELPER="$(readlink -f "${1:-$(dirname "$0")/../src/kydra-repo-index}")"
# Resolved, because the helper reports the folders it indexed that way.
WORK="$(readlink -f "$(mktemp -d)")"
trap 'rm -rf "$WORK"' EXIT

HOST_ARCH="$(dpkg --print-architecture)"
if [[ "$HOST_ARCH" == amd64 ]]; then OTHER_ARCH=arm64; else OTHER_ARCH=amd64; fi

FAILURES=0
check() {
    local what="$1"
    shift
    if "$@"; then
        echo "ok   - $what"
    else
        echo "FAIL - $what"
        FAILURES=$((FAILURES + 1))
    fi
}

# make_deb <name> <version> <arch> <folder> [<file name>] [<extra control lines>]
make_deb() {
    local name="$1" version="$2" arch="$3" dest="$4"
    local file="${5:-${name}_${version}_${arch}.deb}" extra="${6:-}"
    local root="$WORK/build/$name-$version-$arch"
    mkdir -p "$root/DEBIAN"
    {
        echo "Package: $name"
        echo "Version: $version"
        echo "Architecture: $arch"
        echo "Maintainer: Test <test@example.invalid>"
        [[ -n "$extra" ]] && echo "$extra"
        echo "Description: test package"
    } > "$root/DEBIAN/control"
    dpkg-deb --build --root-owner-group "$root" "$dest/$file" >/dev/null
}

versions_of() { # <Packages file> <package>
    awk -v p="$2" '/^Package:/{n=$2} /^Version:/{if (n==p) print $2}' "$1" | sort -V | xargs
}

hashes_match() { # <folder>
    local dir="$1" file sha
    while read -r file sha; do
        [[ "$(sha256sum < "$dir/$file" | cut -d' ' -f1)" == "$sha" ]] || return 1
    done < <(awk '/^Filename:/{f=$2} /^SHA256:/{print f, $2}' "$dir/Packages")
}

release_lists_packages() { # <folder>
    local sha
    sha="$(sha256sum < "$1/Packages" | cut -d' ' -f1)"
    grep -q "^ $sha [0-9]* Packages$" "$1/Release"
}

release_dated_a_while_back_in_english() { # <folder>
    local stamp
    stamp="$(sed -n 's/^Date: //p' "$1/Release")"
    [[ "$stamp" =~ ^[A-Z][a-z]{2},\ [0-9]{2}\ [A-Z][a-z]{2}\ [0-9]{4}\ [0-9:]{8}\ UTC$ ]] \
        || return 1
    (( $(LC_ALL=C date -u -d "$stamp" +%s) < $(date -u +%s) - 3600 ))
}

# --- one folder per architecture ---------------------------------------------
STORE="$WORK/store"
HOST="$STORE/$HOST_ARCH"
OTHER="$STORE/$OTHER_ARCH"
mkdir -p "$HOST" "$OTHER"
make_deb hello 1.0 all "$HOST"
make_deb hello 1.1 all "$HOST"
make_deb tool 2.0 "$HOST_ARCH" "$HOST"
make_deb tool 2.0 "$OTHER_ARCH" "$HOST"
make_deb spaced 1.0 all "$HOST" "spaced name_1.0_all.deb"
echo "not a package" > "$HOST/broken_1.0_all.deb"
make_deb tool 2.0 "$OTHER_ARCH" "$OTHER"
ln -s "../$HOST_ARCH/hello_1.1_all.deb" "$OTHER/"

# In a German session, as the Release file's date must be English regardless.
OUT="$(LC_TIME=de_AT.UTF-8 "$HELPER" "$STORE" 2>&1)" && status=0 || status=$?
check "should_succeed_when_some_files_are_not_packages" test "$status" -eq 0
check "should_list_every_version_when_several_are_present" \
    test "$(versions_of "$HOST/Packages" hello)" == "1.0 1.1"
check "should_leave_out_packages_for_another_machine" \
    bash -c "! grep -q '^Architecture: $OTHER_ARCH' '$HOST/Packages'"
check "should_skip_files_that_are_not_packages" \
    bash -c "! grep -q broken '$HOST/Packages'"
check "should_say_which_files_it_skipped" \
    grep -q 'broken_1.0_all.deb - not a readable package' <<<"$OUT"
check "should_skip_file_names_with_unusual_characters" \
    bash -c "! grep -q '^Package: spaced' '$HOST/Packages'"
OTHER_INDEX="$(versions_of "$OTHER/Packages" tool) / $(versions_of "$OTHER/Packages" hello)"
check "should_index_each_architecture_folder_when_the_folder_has_them" \
    test "$OTHER_INDEX" == "2.0 / 1.1"
check "should_record_hashes_that_match_the_files" hashes_match "$HOST"
check "should_record_hashes_that_match_the_files_behind_links" hashes_match "$OTHER"
check "should_list_the_index_in_Release_with_its_hash" release_lists_packages "$HOST"
check "should_date_the_Release_a_while_back_in_english" \
    release_dated_a_while_back_in_english "$HOST"
check "should_write_Packages_gz_with_the_same_content" \
    cmp -s <(gzip -dc "$HOST/Packages.gz") "$HOST/Packages"
check "should_leave_no_temporary_files" \
    test -z "$(find "$STORE" -name '*.tmp')"

# --- what a second run reads again ---------------------------------------------
OUT="$("$HELPER" "$STORE" 2>&1)"
check "should_skip_a_folder_when_nothing_changed" \
    test "$(grep -c 'up to date' <<<"$OUT")" -eq 2

make_deb hello 1.2 all "$HOST"
OUT="$("$HELPER" "$STORE" 2>&1)"
check "should_hash_only_the_new_package_when_one_is_added" \
    grep -q "^$HOST: 4 package version(s) indexed, 1 newly hashed$" <<<"$OUT"
check "should_skip_the_other_folder_when_only_one_changed" \
    grep -q "^$OTHER: up to date$" <<<"$OUT"

OUT="$("$HELPER" --force "$STORE" 2>&1)"
check "should_hash_everything_again_when_forced" \
    grep -q "^$HOST: 4 package version(s) indexed, 4 newly hashed$" <<<"$OUT"

# --- a package that names its own location and hash ----------------------------
FORGED="$WORK/forged"
mkdir -p "$FORGED"
if make_deb forged 1.0 all "$FORGED" "" "Filename: ../../../etc/shadow"; then
    "$HELPER" "$FORGED" >/dev/null 2>&1
    check "should_ignore_a_Filename_the_package_declares_itself" \
        test "$(grep '^Filename:' "$FORGED/Packages")" == "Filename: ./forged_1.0_all.deb"
else
    echo "skip - should_ignore_a_Filename_the_package_declares_itself (dpkg-deb refused it)"
fi

# --- a single folder -----------------------------------------------------------
FLAT="$WORK/flat"
mkdir -p "$FLAT"
make_deb tool 3.0 "$HOST_ARCH" "$FLAT"
make_deb tool 3.0 "$OTHER_ARCH" "$FLAT"
"$HELPER" "$FLAT" >/dev/null
check "should_index_the_folder_for_this_machine_when_it_has_no_arch_folders" \
    test "$(grep -c '^Package: tool' "$FLAT/Packages") $(grep '^Architecture:' "$FLAT/Packages")" \
         == "1 Architecture: $HOST_ARCH"

# --- what apt makes of it ------------------------------------------------------
if command -v apt-get >/dev/null 2>&1; then
    APT="$WORK/apt"
    mkdir -p "$APT/parts" "$APT/lists/partial" "$APT/cache/archives/partial"
    printf 'Types: deb\nURIs: file://%s\nSuites: ./\nTrusted: yes\n' "$HOST" \
        > "$APT/parts/local.sources"
    APT_OPTS=(-o Dir::Etc::SourceList=/dev/null -o Dir::Etc::SourceParts="$APT/parts"
              -o Dir::State::Lists="$APT/lists" -o Dir::Cache="$APT/cache"
              -o Debug::NoLocking=1)
    UPDATE="$(apt-get "${APT_OPTS[@]}" update 2>&1)" && status=0 || status=$?
    check "should_be_accepted_by_apt_update" \
        bash -c "[[ $status -eq 0 ]] && ! grep -q '^[WE]:' <<<\"\$0\"" "$UPDATE"
    [[ $status -eq 0 ]] || sed 's/^/     /' <<<"$UPDATE"
    check "should_make_apt_offer_the_newest_version" \
        grep -q "Candidate: 1.2" <<<"$(apt-cache "${APT_OPTS[@]}" policy hello 2>&1)"
else
    echo "skip - apt checks (no apt-get here)"
fi

if [[ $FAILURES -gt 0 ]]; then
    echo "$FAILURES check(s) failed"
    exit 1
fi
echo "all checks passed"
