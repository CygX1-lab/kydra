#!/bin/bash

# Kydra .deb Package Builder Script
# This script builds a .deb installer for Kydra with KDE menu integration

set -e

# Configuration
PACKAGE_NAME="kydra"
VERSION="1.0.0"
ARCHITECTURE="amd64"
MAINTAINER="CygX1 <cygnx1@gmail.com>"
DESCRIPTION="KDE-native package manager for Debian systems"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Kydra .deb Package Builder ===${NC}"

# Check if we're in the right directory
if [ ! -f "../CMakeLists.txt" ] || [ ! -d "../src" ]; then
    echo -e "${RED}Error: Please run this script from the Kydra scripts directory${NC}"
    exit 1
fi

# Change to project root directory
cd ..

# Check for required tools
echo -e "${YELLOW}Checking build dependencies...${NC}"
for tool in cmake dpkg-deb fakeroot; do
    if ! command -v $tool &> /dev/null; then
        echo -e "${RED}Error: $tool is not installed${NC}"
        echo "Please install: sudo apt-get install cmake dpkg-dev fakeroot"
        exit 1
    fi
done

# Clean previous builds
echo -e "${YELLOW}Cleaning previous builds...${NC}"
rm -rf build-deb
mkdir -p build-deb
cd build-deb

# Configure with CMake
echo -e "${YELLOW}Configuring build...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DKDE_INSTALL_USE_QT_SYS_PATHS=ON \
    -DBUILD_TESTING=OFF

# Build the project
echo -e "${YELLOW}Building Kydra...${NC}"
make -j$(nproc)

# Create debian package structure
echo -e "${YELLOW}Creating Debian package structure...${NC}"
mkdir -p debian/DEBIAN
mkdir -p debian/usr
mkdir -p debian/usr/bin
mkdir -p debian/usr/share/applications
mkdir -p debian/usr/share/pixmaps
mkdir -p debian/usr/share/doc/kydra

# Install files to package structure
echo -e "${YELLOW}Installing files to package structure...${NC}"
make install DESTDIR=$(pwd)/debian

# Copy desktop file to applications directory (already installed by CMake)
# This is redundant as CMake already installs it, but keep for safety
if [ -f "../data/org.kydra.app.desktop" ]; then
    cp ../data/org.kydra.app.desktop debian/usr/share/applications/
fi

# Create control file
echo -e "${YELLOW}Creating control file...${NC}"
cat > debian/DEBIAN/control << EOF
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Section: admin
Priority: optional
Architecture: ${ARCHITECTURE}
Maintainer: ${MAINTAINER}
Depends: libqapt3, libqt5core5a, libqt5gui5, libqt5widgets5, libkf5configcore5, libkf5xmlgui5, libkf5i18n5, libkf5widgetsaddons5, libkf5auth5, libkf5coreaddons5, libkf5dbusaddons5, libkf5iconthemes5, libkf5service5, libqt5svg5
Recommends: plasma-desktop
Suggests: software-properties-kde
Description: ${DESCRIPTION}
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

# Create postinst script for KDE menu integration
echo -e "${YELLOW}Creating postinst script...${NC}"
cat > debian/DEBIAN/postinst << 'EOF'
#!/bin/bash
set -e

# Update KDE menu cache
if command -v kbuildsycoca5 >/dev/null 2>&1; then
    kbuildsycoca5 --noincremental
fi

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications
fi

# Update icon cache
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f /usr/share/icons/hicolor
fi

echo "Kydra has been successfully installed!"
echo "You can find it in the KDE menu under System > Package Manager"
echo "Or run it from the terminal with: kydra"
EOF

chmod 755 debian/DEBIAN/postinst

# Create prerm script
echo -e "${YELLOW}Creating prerm script...${NC}"
cat > debian/DEBIAN/prerm << 'EOF'
#!/bin/bash
set -e

# Clean up KDE menu cache
if command -v kbuildsycoca5 >/dev/null 2>&1; then
    kbuildsycoca5 --noincremental
fi

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications
fi
EOF

chmod 755 debian/DEBIAN/prerm

# Create copyright file
echo -e "${YELLOW}Creating copyright file...${NC}"
cat > debian/usr/share/doc/kydra/copyright << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: kydra
Upstream-Contact: CygX1 <cygnx1@gmail.com>
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
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 .
 On Debian systems, the complete text of the GNU General Public
 License version 2 can be found in "/usr/share/common-licenses/GPL-2".
EOF

# Create changelog file
echo -e "${YELLOW}Creating changelog file...${NC}"
cat > debian/usr/share/doc/kydra/changelog << EOF
kydra (${VERSION}-1) unstable; urgency=medium

  * Repository configuration page with add/remove/edit capabilities
  * Local .deb folder support for offline package installation
  * Modern KDE/Qt6 interface with Breeze styling
  * Enhanced package search and filtering
  * Improved dependency visualization
  * Plasma desktop integration

 -- ${MAINTAINER}  $(date -R)
EOF

gzip -9 debian/usr/share/doc/kydra/changelog

# Set proper permissions
echo -e "${YELLOW}Setting proper permissions...${NC}"
find debian -type f -exec chmod 644 {} \;
find debian -type d -exec chmod 755 {} \;
find debian/usr/bin -type f -exec chmod 755 {} \;
chmod 755 debian/DEBIAN/postinst
chmod 755 debian/DEBIAN/prerm

# Create releases directory if it doesn't exist
mkdir -p ../releases

# Build the .deb package
echo -e "${YELLOW}Building .deb package...${NC}"
fakeroot dpkg-deb --build debian ${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb

# Move the package to the releases directory
mv ${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb ../releases/

echo -e "${GREEN}=== Build Complete! ===${NC}"
echo -e "${GREEN}Package created: ../releases/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb${NC}"
echo -e "${YELLOW}To install: sudo dpkg -i ../releases/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb${NC}"
echo -e "${YELLOW}To remove: sudo apt-get remove ${PACKAGE_NAME}${NC}"

# Return to scripts directory (we're in the project root after cd .. from build-deb)
cd scripts 2>/dev/null || true

echo -e "${GREEN}Kydra .deb package has been successfully built!${NC}"
echo -e "${YELLOW}The package includes KDE menu integration and will appear in System > Package Manager${NC}"