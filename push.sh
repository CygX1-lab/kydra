#!/usr/bin/env bash
# Publish Kydra: push it to the NAS and to GitHub, and put this version's
# .deb packages on its GitHub release.
#
#   ./push.sh                all three steps
#   ./push.sh --no-nas       skip the NAS (likewise --no-github, --no-release)
#
# 1. NAS: every branch and tag, over SSH, to
#    /volume1/git-repository/kydra.git on 192.168.188.27 as the user pbs. The
#    bare repository is created the first time and added as the remote "nas".
# 2. GitHub: the current branch and every tag to origin
#    (https://github.com/CygX1-lab/kydra).
# 3. Release: the version in CMakeLists.txt is tagged v<version> (unless it
#    is already) and released on GitHub as "Kydra <version>", with its section
#    of docs/RELEASE_NOTES.md as the text. Every kydra_<version>_<arch>.deb
#    that build_deb.sh left in $DEB_DIR/<arch>/ is attached, each with a
#    .sha256 file. A release that is already there gets its files replaced -
#    so the amd64 machine can add its package to the release made here.
#
# Everything the release needs (gh signed in, the packages, the notes) is
# checked before anything is pushed. Nothing is ever force-pushed: a branch
# that has moved on elsewhere is refused, and the script stops there.
#
# Needs git and ssh; for the release the GitHub CLI, signed in once:
#   sudo apt install gh && gh auth login
# The NAS needs SSH on and git installed (Synology: Git Server package).
#
# Override for a one-off:
#   NAS_USER=admin NAS_PORT=2222 ./push.sh
#   DEB_DIR=/tmp/debs ./push.sh
#   NAS_GIT=/usr/local/bin/git ./push.sh     # git not on the NAS's PATH over SSH
set -euo pipefail

NAS_HOST="${NAS_HOST:-192.168.188.27}"
NAS_USER="${NAS_USER:-pbs}"
NAS_PORT="${NAS_PORT:-22}"
NAS_BASE="${NAS_BASE:-/volume1/git-repository}"
NAS_REPO="${NAS_REPO:-kydra.git}"
NAS_REMOTE="${NAS_REMOTE:-nas}"
NAS_GIT="${NAS_GIT:-git}"
GITHUB_REMOTE="${GITHUB_REMOTE:-origin}"
GITHUB_REPO="${GITHUB_REPO:-CygX1-lab/kydra}"
DEB_DIR="${DEB_DIR:-$HOME/Documents/Software/deb}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

die() { echo "ERROR: $*" >&2; exit 1; }

DO_NAS=1
DO_GITHUB=1
DO_RELEASE=1
for arg in "$@"; do
    case "$arg" in
        --no-nas) DO_NAS=0 ;;
        --no-github) DO_GITHUB=0 ;;
        --no-release) DO_RELEASE=0 ;;
        -h|--help) sed -n '2,/^set -euo/{/^set -euo/d; s/^# \{0,1\}//; p}' "$0"; exit 0 ;;
        *) die "unknown option '$arg' (see ./push.sh --help)" ;;
    esac
done

# Everything below that reaches the NAS ends up in a command line its shell
# runs, so only plain names and paths are let through: no spaces, quotes or
# shell syntax.
[[ "$NAS_HOST" =~ ^[A-Za-z0-9.-]+$ ]] || die "NAS_HOST '$NAS_HOST' is not a host name or address"
[[ "$NAS_USER" =~ ^[A-Za-z0-9._-]+$ ]] || die "NAS_USER '$NAS_USER' is not a user name"
[[ "$NAS_PORT" =~ ^[0-9]{1,5}$ ]] || die "NAS_PORT '$NAS_PORT' is not a port"
[[ "$NAS_BASE" =~ ^/[A-Za-z0-9._/-]+$ && "$NAS_BASE" != *..* ]] \
    || die "NAS_BASE '$NAS_BASE' must be an absolute path of plain names"
[[ "$NAS_REPO" =~ ^[A-Za-z0-9._-]+\.git$ && "$NAS_REPO" != *..* ]] \
    || die "NAS_REPO '$NAS_REPO' must be a plain name ending in .git"
[[ "$NAS_REMOTE" =~ ^[A-Za-z0-9._-]+$ ]] || die "NAS_REMOTE '$NAS_REMOTE' is not a remote name"
[[ "$NAS_GIT" =~ ^[A-Za-z0-9._/-]+$ ]] || die "NAS_GIT '$NAS_GIT' is not a program path"
[[ "$GITHUB_REMOTE" =~ ^[A-Za-z0-9._-]+$ ]] || die "GITHUB_REMOTE '$GITHUB_REMOTE' is not a remote name"
[[ "$GITHUB_REPO" =~ ^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$ ]] \
    || die "GITHUB_REPO '$GITHUB_REPO' must be owner/name"

command -v git >/dev/null || die "git is required"
git rev-parse --git-dir >/dev/null 2>&1 || die "$SCRIPT_DIR is not a git repository"
BRANCH="$(git symbolic-ref --short HEAD 2>/dev/null)" || die "not on a branch (detached HEAD)"

# The same rule build_deb.sh reads the version by.
VERSION="$(sed -n \
    's/^[[:space:]]*project([[:space:]]*kydra[[:space:]]\+VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' \
    CMakeLists.txt | head -1)"
[[ "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || die "no project(kydra VERSION x.y.z) in CMakeLists.txt"
TAG="v$VERSION"

WORK="$(mktemp -d)"
cleanup() {
    [[ $DO_NAS -eq 1 ]] && ssh -p "$NAS_PORT" -o "ControlPath=$WORK/%C" -O exit \
        "$NAS_USER@$NAS_HOST" >/dev/null 2>&1
    rm -rf "$WORK"
}
trap cleanup EXIT

# ---------------------------------------------------------------------------
# Before pushing anything: all the release needs
# ---------------------------------------------------------------------------
DEBS=()
if [[ $DO_RELEASE -eq 1 ]]; then
    [[ $DO_GITHUB -eq 1 ]] || git ls-remote --exit-code --tags "$GITHUB_REMOTE" "refs/tags/$TAG" \
        >/dev/null 2>&1 || die "the release needs $TAG on GitHub - run without --no-github"
    command -v gh >/dev/null \
        || die "the release needs the GitHub CLI: sudo apt install gh && gh auth login (or --no-release)"
    gh auth status --hostname github.com >/dev/null 2>&1 \
        || die "gh is not signed in to GitHub: gh auth login (or --no-release)"

    # Each package is checked for what it says it is, not only what it is called.
    declare -A SEEN=()
    shopt -s nullglob
    for deb in "$DEB_DIR"/*/kydra_"$VERSION"_*.deb; do
        real="$(readlink -f "$deb")"
        name="$(basename "$deb")"
        [[ -n "${SEEN[$name]:-}" ]] && continue
        package="$(dpkg-deb -f "$real" Package 2>/dev/null)" || die "$deb is not a readable package"
        version="$(dpkg-deb -f "$real" Version 2>/dev/null)"
        [[ "$package" == kydra && "$version" == "$VERSION" ]] \
            || die "$deb holds $package $version, not kydra $VERSION"
        SEEN[$name]=1
        DEBS+=("$real")
    done
    shopt -u nullglob
    [[ ${#DEBS[@]} -gt 0 ]] \
        || die "no kydra_${VERSION}_<arch>.deb in $DEB_DIR/<arch>/ - run ./build_deb.sh (or --no-release)"

    # This version's section of the release notes: from its heading to the
    # next version's heading or the --- between them.
    awk -v heading="## Version $VERSION" '
        /^## Version / { if (on) exit; on = ($0 == heading || index($0, heading " ") == 1); next }
        on && /^---/ { exit }
        on { print }' docs/RELEASE_NOTES.md | sed '/./,$!d' > "$WORK/notes.md"
    if [[ ! -s "$WORK/notes.md" ]]; then
        echo "note: docs/RELEASE_NOTES.md has no section for $VERSION; the release says only its name"
        echo "Kydra $VERSION" > "$WORK/notes.md"
    fi

    mkdir "$WORK/sums"
    SUMS=()
    for deb in "${DEBS[@]}"; do
        sum="$WORK/sums/$(basename "$deb").sha256"
        printf '%s  %s\n' "$(sha256sum < "$deb" | cut -d' ' -f1)" "$(basename "$deb")" > "$sum"
        SUMS+=("$sum")
    done
fi

[[ -z "$(git status --porcelain --untracked-files=no)" ]] \
    || echo "note: uncommitted changes are neither pushed nor in the release"

if [[ $DO_RELEASE -eq 1 ]]; then
    if git rev-parse -q --verify "refs/tags/$TAG" >/dev/null; then
        echo "==> $TAG is already tagged ($(git rev-parse --short "$TAG^{commit}"))"
    else
        git tag "$TAG"
        echo "==> tagged $TAG at $(git rev-parse --short HEAD)"
    fi
fi

# ---------------------------------------------------------------------------
# 1. NAS
# ---------------------------------------------------------------------------
if [[ $DO_NAS -eq 1 ]]; then
    command -v ssh >/dev/null || die "ssh is required (or --no-nas)"
    REPO_PATH="$NAS_BASE/$NAS_REPO"
    NAS_URL="ssh://$NAS_USER@$NAS_HOST:$NAS_PORT$REPO_PATH"
    # One SSH connection for the whole step, so a password is asked for once.
    SSH_OPTS=(-p "$NAS_PORT" -o ControlMaster=auto -o "ControlPath=$WORK/%C" -o ControlPersist=60)
    echo "==> NAS: $NAS_URL"
    # The values were checked above; the remote command quotes them anyway.
    ssh "${SSH_OPTS[@]}" "$NAS_USER@$NAS_HOST" \
        "if [ -d '$REPO_PATH' ]; then echo '    repository already there'; else
             mkdir -p '$NAS_BASE' && '$NAS_GIT' init --bare --quiet '$REPO_PATH' &&
             '$NAS_GIT' --git-dir='$REPO_PATH' symbolic-ref HEAD 'refs/heads/$BRANCH' &&
             echo '    repository created (default branch $BRANCH)'; fi" \
        || die "could not reach or set up $REPO_PATH on $NAS_HOST (SSH on? git installed? access?)"
    if existing="$(git remote get-url "$NAS_REMOTE" 2>/dev/null)"; then
        [[ "$existing" == "$NAS_URL" ]] \
            || die "the remote '$NAS_REMOTE' already points at $existing - set NAS_REMOTE"
    else
        git remote add "$NAS_REMOTE" "$NAS_URL"
        echo "    added the remote '$NAS_REMOTE'"
    fi
    if [[ "$NAS_GIT" != "git" ]]; then
        git config "remote.$NAS_REMOTE.receivepack" "$NAS_GIT receive-pack"
        git config "remote.$NAS_REMOTE.uploadpack" "$NAS_GIT upload-pack"
    fi
    GIT_SSH_COMMAND="ssh -o ControlMaster=auto -o ControlPath=$WORK/%C -o ControlPersist=60" \
        git push "$NAS_REMOTE" --all
    GIT_SSH_COMMAND="ssh -o ControlMaster=auto -o ControlPath=$WORK/%C -o ControlPersist=60" \
        git push "$NAS_REMOTE" --tags
    echo "✓ NAS up to date"
fi

# ---------------------------------------------------------------------------
# 2. GitHub
# ---------------------------------------------------------------------------
if [[ $DO_GITHUB -eq 1 ]]; then
    echo "==> GitHub: $(git remote get-url "$GITHUB_REMOTE")"
    # gh can answer for the password if nothing else does; nothing is changed
    # in the git configuration for that.
    CREDENTIALS=()
    command -v gh >/dev/null && CREDENTIALS=(-c "credential.helper=!gh auth git-credential")
    git "${CREDENTIALS[@]}" push "$GITHUB_REMOTE" "$BRANCH"
    git "${CREDENTIALS[@]}" push "$GITHUB_REMOTE" --tags
    echo "✓ GitHub up to date"
fi

# ---------------------------------------------------------------------------
# 3. Release
# ---------------------------------------------------------------------------
if [[ $DO_RELEASE -eq 1 ]]; then
    echo "==> Release $TAG on $GITHUB_REPO: $(printf '%s ' "${DEBS[@]##*/}")"
    if gh release view "$TAG" -R "$GITHUB_REPO" >/dev/null 2>&1; then
        gh release upload "$TAG" "${DEBS[@]}" "${SUMS[@]}" -R "$GITHUB_REPO" --clobber
        echo "✓ replaced the files of the release $TAG"
    else
        gh release create "$TAG" "${DEBS[@]}" "${SUMS[@]}" -R "$GITHUB_REPO" --verify-tag \
            --title "Kydra $VERSION" --notes-file "$WORK/notes.md"
        echo "✓ released $TAG"
    fi
    echo "  https://github.com/$GITHUB_REPO/releases/tag/$TAG"
fi
