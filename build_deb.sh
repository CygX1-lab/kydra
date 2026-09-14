#!/usr/bin/env bash
# Build a .deb for Kydra.
#
# Kydra is compiled C++ against Qt5, KF5 and QApt, so unlike the pure-Python
# projects here a package is only ever good for one architecture: the one it
# was compiled on. There is no `Architecture: all` package, and no link into
# another machine's folder - to get an amd64 package, run this on an amd64
# machine.
#
# Nothing about the dependencies is written down twice. dpkg-shlibdeps reads
# them back out of the finished binary, so a library added to
# target_link_libraries() in src/CMakeLists.txt cannot quietly go missing from
# Depends the way the hand-written list in scripts/build-deb.sh could. Only
# what the program loads without linking - the QApt worker, the Kirigami QML
# modules - is named by hand below, with the reason it is there.
#
# Produces: $DEB_DIR/<arch>/kydra_<version>_<arch>.deb
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build-deb"
STAGING_DIR="$BUILD_DIR/staging"

# Where the finished packages go. Not in the checkout: a built package is not
# source, it is a quarter of a megabyte again on every version, and it belongs
# with the other software kept for installing rather than with the code it was
# made from. (releases/ in this repo is the old habit; RELEASES_COPY=1 keeps
# feeding it.) The build tree stays under build-deb/ - that is scaffolding,
# thrown away and rebuilt every run. Override for a one-off:
#   DEB_DIR=/tmp/debs ./build_deb.sh
DEB_DIR="${DEB_DIR:-$HOME/Documents/Software/deb}"
RELEASES_COPY="${RELEASES_COPY:-0}"

# One folder per machine to install on. A compiled package can only ever be
# stamped with the architecture it was built for, so it goes in that folder
# and nowhere else - the trick the Python projects use, of building one
# `Architecture: all` package and linking it into every folder, is not
# available to a program made of machine code.
HOST_ARCH="$(dpkg --print-architecture)"
PKG_DIR="$DEB_DIR/$HOST_ARCH"

APP_NAME="kydra"
MAINTAINER="CygX1 <cygnx1@gmail.com>"
DESCRIPTION="KDE-native package manager for Debian systems"

# Read from CMakeLists.txt rather than repeated here: the version the package
# claims and the version the program was compiled with are then the same
# number by construction, not by someone remembering to edit both.
VERSION="$(sed -n \
    's/^[[:space:]]*project([[:space:]]*kydra[[:space:]]\+VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' \
    "$PROJECT_DIR/CMakeLists.txt" | head -1)"
if [[ -z "$VERSION" ]]; then
    echo "ERROR: no project(kydra VERSION ...) line in CMakeLists.txt" >&2
    exit 1
fi

# Needed at runtime but not linked, so dpkg-shlibdeps cannot see them.
# libqapt3-runtime holds the QApt worker: every install and removal is handed
# to it over D-Bus behind polkit, and without it Kydra can list packages and
# change nothing.
EXTRA_DEPENDS="libqapt3-runtime"
RECOMMENDS="plasma-desktop"
SUGGESTS="software-properties-kde"

echo "==> Building $APP_NAME $VERSION for $HOST_ARCH"

# ---------------------------------------------------------------------------
# Before spending ten minutes compiling
# ---------------------------------------------------------------------------
echo "==> Checking the toolchain..."
MISSING_TOOLS=()
# No fakeroot: dpkg-deb --root-owner-group stamps every file root:root on its
# own, which is the only thing fakeroot was ever doing here.
for tool in cmake make dpkg-deb dpkg-shlibdeps; do
    command -v "$tool" >/dev/null 2>&1 || MISSING_TOOLS+=("$tool")
done
if [[ ${#MISSING_TOOLS[@]} -gt 0 ]]; then
    echo "ERROR: missing build tools: ${MISSING_TOOLS[*]}" >&2
    echo "  sudo apt install cmake build-essential dpkg-dev" >&2
    exit 1
fi
echo "✓ cmake, make and dpkg-dev are present"

# A source file that is tracked but not in the working tree compiles into
# nothing and is packaged as nothing. cmake fails on it if it is named in
# src/CMakeLists.txt, but an icon or a .qml file just goes missing quietly, so
# say it here where it can be read.
DELETED="$(cd "$PROJECT_DIR" && git ls-files --deleted 2>/dev/null | wc -l)"
if [[ "$DELETED" -gt 0 ]]; then
    echo "WARNING: $DELETED tracked file(s) are missing from the working tree." >&2
    echo "         Whatever they held will not be in the package:" >&2
    (cd "$PROJECT_DIR" && git ls-files --deleted | sed 's/^/           /' | head -10) >&2
    [[ "$DELETED" -gt 10 ]] && echo "           ... and $((DELETED - 10)) more" >&2
    echo "         Restore them with: git checkout -- ." >&2
fi

# The about box reads its number from KydraVersion.h.in, which hardcodes a
# string instead of taking @PROJECT_VERSION@ from cmake. When the two drift,
# the package says one version and the program says another.
HDR_VERSION="$(sed -n 's/.*KYDRA_VERSION_STRING[[:space:]]\+"\([0-9][0-9.]*\)".*/\1/p' \
    "$PROJECT_DIR/KydraVersion.h.in" | head -1)"
if [[ -n "$HDR_VERSION" && "$HDR_VERSION" != "$VERSION" ]]; then
    echo "WARNING: KydraVersion.h.in says $HDR_VERSION, CMakeLists.txt says $VERSION." >&2
    echo "         The package is $VERSION; Help > About will say $HDR_VERSION." >&2
fi

# ---------------------------------------------------------------------------
# Compile
# ---------------------------------------------------------------------------
echo "==> Configuring..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DKDE_INSTALL_USE_QT_SYS_PATHS=ON \
    -DBUILD_TESTING=OFF \
    >"$BUILD_DIR/cmake.log" 2>&1 \
    || { echo "ERROR: cmake failed - $BUILD_DIR/cmake.log" >&2
         tail -20 "$BUILD_DIR/cmake.log" >&2
         echo "  build deps: sudo apt install extra-cmake-modules qtbase5-dev \\" >&2
         echo "      qtdeclarative5-dev libkf5kio-dev libkf5dbusaddons-dev \\" >&2
         echo "      libkf5i18n-dev libkf5iconthemes-dev libkf5xmlgui-dev libqapt-dev" >&2
         echo "  optional, where the release still packages it: libkf5kirigami2-dev" >&2
         exit 1; }

# Kirigami is optional at compile time - cmake carries on without it and turns
# the QML interface off. That is a different package, so it is worth knowing
# which one was built rather than discovering it in the finished .deb.
WITH_KIRIGAMI=0
if grep -q "^KF5Kirigami2_DIR:PATH=\(.*\)$" "$BUILD_DIR/CMakeCache.txt" \
   && ! grep -q "^KF5Kirigami2_DIR:PATH=.*NOTFOUND" "$BUILD_DIR/CMakeCache.txt"; then
    WITH_KIRIGAMI=1
    RECOMMENDS="$RECOMMENDS, qml-module-org-kde-kirigami2"
    echo "✓ Kirigami found - the QML interface is in this build"
else
    echo "WARNING: Kirigami not found - the QML interface is compiled out." >&2
    echo "         Install libkf5kirigami2-dev and build again, if this release" >&2
    echo "         still packages it (Ubuntu 26.04 no longer does)." >&2
fi

echo "==> Compiling..."
cmake --build "$BUILD_DIR" -j"$(nproc)" >"$BUILD_DIR/build.log" 2>&1 \
    || { echo "ERROR: build failed - $BUILD_DIR/build.log" >&2
         grep -i "error" "$BUILD_DIR/build.log" | head -20 >&2
         exit 1; }
echo "✓ built $(du -h "$BUILD_DIR/src/$APP_NAME" | cut -f1) binary"

# ---------------------------------------------------------------------------
# Stage
# ---------------------------------------------------------------------------
echo "==> Staging package tree..."
rm -rf "$STAGING_DIR"
mkdir -p "$STAGING_DIR/DEBIAN" "$STAGING_DIR/usr/share/doc/$APP_NAME"
DESTDIR="$STAGING_DIR" cmake --install "$BUILD_DIR" >"$BUILD_DIR/install.log" 2>&1 \
    || { echo "ERROR: install failed - $BUILD_DIR/install.log" >&2
         tail -20 "$BUILD_DIR/install.log" >&2; exit 1; }

# Everything cmake was asked to install, checked for on the way out. Each of
# these is a file the program looks for by absolute path at runtime and does
# without silently: no kydraui.rc and the menus and toolbar are empty, no
# muon.kcfg and every setting is back to its default, no .desktop file and
# nothing appears in the menu.
echo "==> Verifying the staged tree..."
REQUIRED=(
    "usr/bin/$APP_NAME"
    "usr/share/applications/org.kydra.app.desktop"
    "usr/share/metainfo/org.kydra.app.metainfo.xml"
    "usr/share/kxmlgui5/$APP_NAME/kydraui.rc"
    "usr/share/config.kcfg/muon.kcfg"
    "usr/share/icons/hicolor/scalable/apps/org.kydra.app.svg"
)
missing=0
for f in "${REQUIRED[@]}"; do
    if [[ ! -s "$STAGING_DIR/$f" ]]; then
        echo "ERROR: /$f is missing from the package" >&2
        missing=1
    fi
done
[[ $missing -eq 0 ]] || exit 1

# The QML lives under the multiarch qt5/qml path, so it is found rather than
# named. Installing only the executable once shipped a Kirigami build whose
# interface could not load a single page.
if [[ $WITH_KIRIGAMI -eq 1 ]]; then
    for qml in KydraKirigami.qml PackageDetailsPanel.qml; do
        found="$(find "$STAGING_DIR" -path "*/org/kde/$APP_NAME/$qml" -size +0c -print -quit)"
        if [[ -z "$found" ]]; then
            echo "ERROR: $qml was built with Kirigami but not packaged" >&2
            exit 1
        fi
    done
fi
echo "✓ $(find "$STAGING_DIR/usr" -type f | wc -l) file(s) staged, all of them expected"

# Every library the binary asks for has to be resolvable on the machine that
# built it, or the Depends worked out below are a list of guesses.
BINARY="$STAGING_DIR/usr/bin/$APP_NAME"
if ldd "$BINARY" 2>/dev/null | grep -q "not found"; then
    echo "ERROR: the binary links libraries that are not installed:" >&2
    ldd "$BINARY" | grep "not found" | sed 's/^/  /' >&2
    exit 1
fi
echo "✓ every linked library resolves"

# Cheapest possible does-it-run check. Only a warning: --version still opens a
# QApt backend, and a build machine with no D-Bus session is a fine machine to
# build a package on.
if timeout 30 env QT_QPA_PLATFORM=offscreen "$BINARY" --version >/dev/null 2>&1; then
    echo "✓ the binary starts and answers --version"
else
    echo "WARNING: $APP_NAME --version did not answer here. That is expected on a" >&2
    echo "         headless or session-less build machine, but check it if not." >&2
fi

if command -v desktop-file-validate >/dev/null 2>&1; then
    desktop-file-validate "$STAGING_DIR/usr/share/applications/org.kydra.app.desktop" \
        | sed 's/^/    /' || true
fi
if command -v appstreamcli >/dev/null 2>&1; then
    appstreamcli validate --no-net \
        "$STAGING_DIR/usr/share/metainfo/org.kydra.app.metainfo.xml" \
        >/dev/null 2>&1 || echo "    note: appstream metadata has validation complaints"
fi

# ---------------------------------------------------------------------------
# Depends, read out of the binary
# ---------------------------------------------------------------------------
echo "==> Working out dependencies..."
SHLIBDEPS_DIR="$BUILD_DIR/shlibdeps"
mkdir -p "$SHLIBDEPS_DIR/debian"
# dpkg-shlibdeps insists on a debian/control next to where it runs. It only
# reads the package name out of it, so this is a stub, not a second place that
# says what Kydra depends on.
cat > "$SHLIBDEPS_DIR/debian/control" <<EOF
Source: $APP_NAME

Package: $APP_NAME
Architecture: $HOST_ARCH
EOF
: > "$SHLIBDEPS_DIR/debian/substvars"
# A library with no owning package fails here rather than at install time on
# someone else's machine. If that library is deliberately hand-built, say so:
#   SHLIBDEPS_FLAGS=--ignore-missing-info ./build_deb.sh
SHLIB_DEPENDS="$(cd "$SHLIBDEPS_DIR" \
    && dpkg-shlibdeps -O ${SHLIBDEPS_FLAGS:-} "$BINARY" 2>"$BUILD_DIR/shlibdeps.log" \
    | sed 's/^shlibs:Depends=//')" \
    || { echo "ERROR: dpkg-shlibdeps could not name a package for every library:" >&2
         cat "$BUILD_DIR/shlibdeps.log" >&2; exit 1; }
if [[ -z "$SHLIB_DEPENDS" ]]; then
    echo "ERROR: dpkg-shlibdeps returned no dependencies at all" >&2
    exit 1
fi
DEB_DEPENDS="$SHLIB_DEPENDS, $EXTRA_DEPENDS"

# The hand-written names are the ones that can be wrong - a package that does
# not exist installs as an unmet dependency and nothing else. Checked here
# when apt has lists to check against, and passed over quietly when it does not.
for pkg in $(echo "$EXTRA_DEPENDS, $RECOMMENDS" | tr ',' ' '); do
    if apt-cache show "$pkg" >/dev/null 2>&1; then :
    elif apt-cache policy >/dev/null 2>&1 && [[ -n "$(apt-cache pkgnames 2>/dev/null | head -1)" ]]; then
        echo "WARNING: apt has never heard of '$pkg' - check the name" >&2
    fi
done
echo "✓ Depends: $DEB_DEPENDS"

# ---------------------------------------------------------------------------
# Control
# ---------------------------------------------------------------------------
INSTALLED_SIZE="$(du -sk "$STAGING_DIR/usr" | cut -f1)"
cat > "$STAGING_DIR/DEBIAN/control" <<EOF
Package: $APP_NAME
Version: $VERSION
Section: admin
Priority: optional
Architecture: $HOST_ARCH
Installed-Size: $INSTALLED_SIZE
Depends: $DEB_DEPENDS
Recommends: $RECOMMENDS
Suggests: $SUGGESTS
Maintainer: $MAINTAINER
Description: $DESCRIPTION
 Kydra is a KDE/Qt application for managing Debian packages, providing
 a powerful, fast, and Plasma-integrated interface built on top of Muon's
 architecture.
 .
 Kydra brings a refreshed interface, streamlined workflows, and enhanced
 integration with KDE technologies. With its multi-headed architecture,
 it offers fast search, clear dependency insights, repository control,
 and update management for Debian-based systems.
 .
 Features:
  - Modern KDE/Qt UI with Breeze styling
  - Fast package search and filtering
  - Repository configuration management
  - Local .deb folder support
  - Dependency visualization
  - Update management
  - Plasma desktop integration
EOF

cat > "$STAGING_DIR/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
# A KDE application that is not in the menu until the next login looks like
# one that did not install.
if command -v kbuildsycoca5 >/dev/null 2>&1; then
    kbuildsycoca5 --noincremental >/dev/null 2>&1 || true
fi
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q /usr/share/applications || true
fi
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor 2>/dev/null || true
fi
echo ""
echo "Kydra installed. Find it in the menu under System > Package Manager,"
echo "or run:"
echo "  kydra"
echo ""
echo "Installing and removing packages is done by the QApt worker over polkit,"
echo "so Kydra itself is never run as root."
echo ""
exit 0
EOF
chmod 755 "$STAGING_DIR/DEBIAN/postinst"

cat > "$STAGING_DIR/DEBIAN/postrm" <<'EOF'
#!/bin/sh
set -e
if command -v kbuildsycoca5 >/dev/null 2>&1; then
    kbuildsycoca5 --noincremental >/dev/null 2>&1 || true
fi
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q /usr/share/applications || true
fi
exit 0
EOF
chmod 755 "$STAGING_DIR/DEBIAN/postrm"

# ---------------------------------------------------------------------------
# Documentation dpkg expects to find
# ---------------------------------------------------------------------------
cat > "$STAGING_DIR/usr/share/doc/$APP_NAME/copyright" <<EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: $APP_NAME
Upstream-Contact: $MAINTAINER
Source: https://github.com/CygX1/kydra

Files: *
Copyright: 2025 CygX1 <cygnx1@gmail.com>
License: GPL-2+
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 .
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 On Debian systems, the complete text of the GNU General Public
 License version 2 can be found in "/usr/share/common-licenses/GPL-2".
EOF

# The release notes are already written in docs/ChangeLog; retyping them into
# the package is how the two come to disagree. The section for this version is
# lifted out if it is there, and a bare entry stands in if it is not.
CHANGELOG="$STAGING_DIR/usr/share/doc/$APP_NAME/changelog"
{
    echo "$APP_NAME ($VERSION-1) unstable; urgency=medium"
    echo ""
    body="$(awk -v v="$VERSION" '
        $0 ~ "^Kydra Version " v "$" {inside=1; next}
        inside && /^Kydra Version /                {exit}
        inside && !/^=+$/                          {print}
    ' "$PROJECT_DIR/docs/ChangeLog" 2>/dev/null | sed 's/[[:space:]]*$//')"
    if [[ -n "$(echo "$body" | tr -d '[:space:]')" ]]; then
        echo "$body" | sed '/^$/d; s/^[-*][[:space:]]*/  * /; s/^\([^ ]\)/  * \1/; s/^\([[:space:]]\{2,\}\)\([^*]\)/    \2/'
    else
        echo "  * Kydra $VERSION. See docs/ChangeLog for the full release notes."
    fi
    echo ""
    echo " -- ${MAINTAINER}  $(date -R)"
} > "$CHANGELOG"
gzip -9n "$CHANGELOG"

find "$STAGING_DIR/usr" -type f -exec chmod 644 {} +
find "$STAGING_DIR" -type d -exec chmod 755 {} +
chmod 755 "$BINARY"

# ---------------------------------------------------------------------------
# Build
# ---------------------------------------------------------------------------
echo "==> Building .deb into $PKG_DIR..."
if ! mkdir -p "$PKG_DIR"; then
    echo "ERROR: $DEB_DIR cannot be created - set DEB_DIR somewhere it can" >&2
    exit 1
fi
DEB_OUT="$PKG_DIR/${APP_NAME}_${VERSION}_${HOST_ARCH}.deb"
rm -f "$DEB_OUT"
dpkg-deb --build --root-owner-group "$STAGING_DIR" "$DEB_OUT" >/dev/null

# What was asked for is what came out. A package stamped with an architecture
# nobody meant is one apt refuses on the machine it was built for.
STAMPED="$(dpkg-deb -f "$DEB_OUT" Architecture)"
if [[ "$STAMPED" != "$HOST_ARCH" ]]; then
    echo "ERROR: $DEB_OUT says Architecture: $STAMPED, not $HOST_ARCH" >&2
    exit 1
fi

if [[ "$RELEASES_COPY" == "1" ]]; then
    mkdir -p "$PROJECT_DIR/releases"
    cp "$DEB_OUT" "$PROJECT_DIR/releases/"
    echo "    copied to releases/$(basename "$DEB_OUT")"
fi

if command -v lintian >/dev/null 2>&1; then
    echo "==> lintian:"
    lintian "$DEB_OUT" 2>/dev/null | head -12 || true
fi

echo ""
echo "✓ Built $APP_NAME $VERSION ($(du -h "$DEB_OUT" | cut -f1)):"
echo "    $DEB_OUT"
echo ""
echo "Install with:  sudo apt install $DEB_OUT"
echo "Remove with:   sudo apt remove $APP_NAME"
echo ""
echo "This package runs on $HOST_ARCH only - it is compiled code. For another"
echo "architecture, run this script on a machine of that architecture."
