#!/usr/bin/env bash
# Integration test for push.sh. A project with two built packages is pushed
# to a "NAS" and a "GitHub" that are folders here: an ssh stand-in runs the
# NAS's side of each command locally, a local bare repository stands in for
# GitHub, and a gh stand-in records the release it is asked to make. Nothing
# outside the temporary folder is read or written, and nothing goes on the
# network.
#
#   tests/test_push.sh [path/to/push.sh]
set -euo pipefail

SCRIPT="$(readlink -f "${1:-$(dirname "$0")/../push.sh}")"
WORK="$(readlink -f "$(mktemp -d)")"
trap 'rm -rf "$WORK"' EXIT

# Commits and pushes here must not depend on the machine's git settings.
export GIT_CONFIG_GLOBAL=/dev/null GIT_CONFIG_NOSYSTEM=1
export GIT_AUTHOR_NAME=Test GIT_AUTHOR_EMAIL=test@example.invalid
export GIT_COMMITTER_NAME=Test GIT_COMMITTER_EMAIL=test@example.invalid

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

BIN="$WORK/bin"
mkdir -p "$BIN"
cat > "$BIN/ssh" <<'EOF'
#!/bin/sh
# ssh stand-in: skips the options and the host, runs the command here
while [ $# -gt 0 ]; do
  case "$1" in
    -O) exit 0 ;;
    -p|-o) shift 2 ;;
    -*) shift ;;
    *) shift; break ;;
  esac
done
echo "$*" >> "$STATE/ssh.log"
exec sh -c "$*"
EOF
cat > "$BIN/gh" <<'EOF'
#!/bin/sh
# gh stand-in: records every call; a release "exists" once $STATE/release is there
echo "gh $*" >> "$STATE/gh.log"
case "$1 $2" in
  "auth status") exit 0 ;;
  "auth git-credential") exit 0 ;;
  "release view") [ -e "$STATE/release" ] ;;
  "release create"|"release upload")
    mkdir -p "$STATE/uploaded"
    prev=""
    for a in "$@"; do
      case "$a" in *.deb|*.sha256) cp "$a" "$STATE/uploaded/" ;; esac
      [ "$prev" = "--notes-file" ] && cp "$a" "$STATE/notes.md"
      prev="$a"
    done
    touch "$STATE/release" ;;
esac
EOF
chmod 755 "$BIN/ssh" "$BIN/gh"
export PATH="$BIN:$PATH"

# make_deb <version> <arch> <file>
make_deb() {
    local root="$WORK/build/$1-$2"
    mkdir -p "$root/DEBIAN" "$(dirname "$3")"
    printf 'Package: kydra\nVersion: %s\nArchitecture: %s\nMaintainer: T <t@example.invalid>\nDescription: test\n' \
        "$1" "$2" > "$root/DEBIAN/control"
    dpkg-deb --build --root-owner-group "$root" "$3" >/dev/null
}

# A fresh project at version 9.9.9, its GitHub, an empty NAS and two packages.
fresh() {
    rm -rf "$WORK/project" "$WORK/github.git" "$WORK/volume1" "$WORK/debs" "$WORK/state"
    export STATE="$WORK/state"
    mkdir -p "$STATE"
    PROJECT="$WORK/project"
    mkdir -p "$PROJECT/docs"
    git init -q -b master "$PROJECT"
    cp "$SCRIPT" "$PROJECT/push.sh"
    echo 'project(kydra VERSION 9.9.9)' > "$PROJECT/CMakeLists.txt"
    printf '# Release Notes\n\n## Version 9.9.9 (2026-09-14)\n\n* the new thing\n\n---\n\n## Version 9.9.8\n\n* the old thing\n' \
        > "$PROJECT/docs/RELEASE_NOTES.md"
    git -C "$PROJECT" add .
    git -C "$PROJECT" commit -q -m "9.9.9"
    git init -q --bare "$WORK/github.git"
    git -C "$PROJECT" remote add origin "$WORK/github.git"
    make_deb 9.9.9 arm64 "$WORK/debs/arm64/kydra_9.9.9_arm64.deb"
    make_deb 9.9.9 amd64 "$WORK/debs/amd64/kydra_9.9.9_amd64.deb"
    make_deb 9.9.8 arm64 "$WORK/debs/arm64/kydra_9.9.8_arm64.deb"
}

push() {
    OUT="$(cd "$PROJECT" && NAS_BASE="$WORK/volume1/git-repository" NAS_USER=tester \
           DEB_DIR="$WORK/debs" bash push.sh "$@" 2>&1)" && status=0 || status=$?
}

NAS="$WORK/volume1/git-repository/kydra.git"

# --- everything --------------------------------------------------------------
fresh
push
check "should_succeed_when_everything_is_in_place" test "$status" -eq 0
[[ $status -eq 0 ]] || sed 's/^/     /' <<<"$OUT"
check "should_create_the_repository_on_the_nas_and_push_to_it" \
    test "$(git -C "$NAS" rev-parse master 2>/dev/null)" == "$(git -C "$PROJECT" rev-parse master)"
check "should_push_the_branch_to_github" \
    test "$(git -C "$WORK/github.git" rev-parse master 2>/dev/null)" == "$(git -C "$PROJECT" rev-parse master)"
check "should_tag_the_version_and_push_the_tag_everywhere" \
    bash -c "git -C '$WORK/github.git' rev-parse -q --verify refs/tags/v9.9.9 >/dev/null &&
             git -C '$NAS' rev-parse -q --verify refs/tags/v9.9.9 >/dev/null"
check "should_release_the_version_under_its_tag_and_name" \
    grep -q -- "release create v9.9.9 .*--verify-tag --title Kydra 9.9.9" "$STATE/gh.log"
check "should_attach_every_package_of_the_version" \
    test -f "$STATE/uploaded/kydra_9.9.9_arm64.deb" -a -f "$STATE/uploaded/kydra_9.9.9_amd64.deb"
check "should_leave_out_packages_of_other_versions" test ! -e "$STATE/uploaded/kydra_9.9.8_arm64.deb"
check "should_attach_checksums_that_match_the_packages" \
    bash -c "cd '$STATE/uploaded' && sha256sum --quiet -c kydra_9.9.9_arm64.deb.sha256 kydra_9.9.9_amd64.deb.sha256"
check "should_use_the_version_s_own_release_notes" \
    bash -c "grep -q 'the new thing' '$STATE/notes.md' && ! grep -q 'the old thing' '$STATE/notes.md'"

# --- a release already there: its files are replaced ---------------------------
: > "$STATE/gh.log"
push
check "should_push_again_when_everything_is_there_already" test "$status" -eq 0
check "should_replace_the_files_of_a_release_already_there" \
    bash -c "grep -q -- 'release upload v9.9.9 .*--clobber' '$STATE/gh.log' && ! grep -q 'release create' '$STATE/gh.log'"

# --- what stops it before anything is pushed -----------------------------------
fresh
rm "$WORK"/debs/*/kydra_9.9.9_*.deb
push
check "should_stop_when_the_version_has_no_package" \
    bash -c "[[ $status -ne 0 ]] && grep -q 'run ./build_deb.sh' <<<\"\$0\"" "$OUT"
check "should_push_nothing_when_it_stops_before_the_release" \
    bash -c "[[ ! -e '$STATE/ssh.log' ]] &&
             ! git -C '$WORK/github.git' rev-parse -q --verify master >/dev/null"

fresh
cp "$WORK/debs/arm64/kydra_9.9.8_arm64.deb" "$WORK/debs/arm64/kydra_9.9.9_i386.deb"
push
check "should_refuse_a_package_that_holds_another_version" \
    bash -c "[[ $status -ne 0 ]] && grep -q 'holds kydra 9.9.8, not kydra 9.9.9' <<<\"\$0\"" "$OUT"

fresh
for bad in "NAS_REPO=x.git';touch${IFS:0:1}owned;'" "NAS_BASE=/volume1/a b" "NAS_BASE=/volume1/../etc" \
           "NAS_HOST=nas;reboot" "NAS_PORT=22 -oProxyCommand=x" "GITHUB_REPO=a/b;c"; do
    OUT="$(cd "$PROJECT" && env "$bad" bash push.sh 2>&1)" && status=0 || status=$?
    check "should_refuse_${bad%%=*}_$(tr -c 'a-z0-9' _ <<<"${bad#*=}" | cut -c1-20)" test "$status" -ne 0
done
check "should_run_nothing_on_the_nas_for_values_it_refuses" test ! -e "$STATE/ssh.log"

# --- steps left out --------------------------------------------------------------
fresh
push --no-release
check "should_push_without_releasing_with_no_release" \
    bash -c "[[ $status -eq 0 && ! -e '$STATE/gh.log' ]] &&
             git -C '$WORK/github.git' rev-parse -q --verify master >/dev/null"
fresh
push --no-nas --no-release
check "should_leave_the_nas_alone_with_no_nas" \
    bash -c "[[ $status -eq 0 && ! -e '$STATE/ssh.log' && ! -d '$NAS' ]]"

check "should_never_force_a_push" \
    bash -c "! grep -n -E -- '--force|push -f|\+refs' '$SCRIPT'"

if [[ $FAILURES -gt 0 ]]; then
    echo "$FAILURES check(s) failed"
    exit 1
fi
echo "all checks passed"
