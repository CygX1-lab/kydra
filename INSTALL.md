# Kydra Installation Guide

## Precompiled Packages

Precompiled .deb packages are available in the `releases/` folder. Simply download the latest package and install it using:

```bash
sudo dpkg -i releases/kydra_0.9.1_amd64.deb
```

## Building and Installing Kydra .deb Package

If you prefer to build from source, this guide explains how to build a .deb installer for Kydra and install it on your Debian/Ubuntu/Kubuntu system with KDE menu integration.

### Prerequisites

Before building the package, ensure you have the following dependencies installed:

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    extra-cmake-modules \
    qtbase5-dev \
    libkf5config-dev \
    libkf5xmlgui-dev \
    libkf5i18n-dev \
    libkf5widgetsaddons-dev \
    libkf5auth-dev \
    libkf5bookmarks-dev \
    libkf5codecs-dev \
    libkf5configwidgets-dev \
    libkf5coreaddons-dev \
    libkf5dbusaddons-dev \
    libkf5iconthemes-dev \
    libkf5service-dev \
    libkf5solid-dev \
    libkf5windowsystem-dev \
    libkf5xmlgui-dev \
    libqt5svg5-dev \
    libqapt-dev \
    pkg-kde-tools \
    dpkg-dev \
    fakeroot
```

### Quick Build and Install

1. **Clone or download the Kydra source code**
2. **Run the build script:**
   ```bash
   cd scripts
   ./build-deb.sh
   ```
3. **Install the generated package:**
   ```bash
   sudo dpkg -i ../releases/kydra_0.9.1_amd64.deb
   ```

### Using the Build Script

The `scripts/build-deb.sh` script provides an automated way to build a .deb package with the following features:

- **Automated dependency checking** - Verifies all required build tools are installed
- **Clean build environment** - Creates isolated build directory
- **Proper Debian packaging** - Follows Debian packaging standards
- **KDE menu integration** - Automatically installs desktop file and updates menu cache
- **Post-installation scripts** - Handles KDE menu cache updates and desktop database refresh

**Script Features:**
- Builds Kydra with Release configuration
- Creates proper Debian package structure
- Installs KDE desktop file for menu integration
- Generates control file with dependencies
- Creates postinst/prerm scripts for proper system integration
- Builds final .deb package with proper permissions

**Usage:**
```bash
cd /path/to/kydra/source/scripts
./build-deb.sh
```

The script will output the final .deb package in the `releases/` directory.

### Manual Build Process

If you prefer to build manually:

1. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Configure with CMake:**
   ```bash
   cmake .. \
       -DCMAKE_BUILD_TYPE=Release \
       -DCMAKE_INSTALL_PREFIX=/usr \
       -DKDE_INSTALL_USE_QT_SYS_PATHS=ON \
       -DBUILD_TESTING=OFF
   ```

3. **Build the project:**
   ```bash
   make -j$(nproc)
   ```

4. **Create Debian package structure:**
   ```bash
   mkdir -p debian/DEBIAN
   mkdir -p debian/usr
   ```

5. **Install files:**
   ```bash
   make install DESTDIR=$(pwd)/debian
   ```

6. **Create control file and package:**
   ```bash
   # Create control file (see build-deb.sh for details)
   fakeroot dpkg-deb --build debian kydra_0.9.1_amd64.deb
   ```

### Installation

After building the package:

```bash
# Install the package
sudo dpkg -i kydra_0.9.1_amd64.deb

# If there are dependency issues, fix them with:
sudo apt-get install -f
```

### KDE Menu Integration

The package automatically:

- **Creates a KDE menu entry** under `System > Package Manager`
- **Installs a desktop file** at `/usr/share/applications/org.kydra.app.desktop`
- **Updates KDE menu cache** automatically after installation
- **Provides application icons** and proper categorization

### Finding Kydra After Installation

1. **KDE Menu:** Look under `System > Package Manager > Kydra`
2. **Application Launcher:** Search for "Kydra" or "Package Manager"
3. **Terminal:** Run `kydra` directly
4. **KRunner:** Press `Alt+F2` and type "kydra"

### Desktop File Details

The installed desktop file (`/usr/share/applications/org.kydra.app.desktop`) includes:

- **Name:** Kydra
- **Generic Name:** Package Manager
- **Categories:** Qt;KDE;System;
- **Keywords:** program,software,repository,archive,package,install,remove,update,deb
- **Icon:** org.kydra.app
- **Multi-language support** for name and descriptions

### Uninstallation

To remove Kydra:

```bash
sudo apt-get remove kydra
```

To remove configuration files as well:

```bash
sudo apt-get purge kydra
```

### Troubleshooting

#### Missing Dependencies
If installation fails due to missing dependencies:
```bash
sudo apt-get install -f
```

#### KDE Menu Not Showing
If the menu entry doesn't appear:
```bash
kbuildsycoca5 --noincremental
```

#### Icon Issues
If icons are missing:
```bash
gtk-update-icon-cache -f /usr/share/icons/hicolor
```

### Features Available After Installation

- **Repository Configuration:** Settings → Configure Repositories
- **Local .deb Folder Support:** Configure local package directories
- **Package Management:** Full package installation, removal, and updates
- **Dependency Visualization:** Clear dependency tree views
- **Search and Filtering:** Fast package search with multiple filters
- **KDE Integration:** Native Plasma desktop integration

### Building for Different Architectures

To build for different architectures (e.g., i386, arm64), you may need:

1. **Cross-compilation tools**
2. **Architecture-specific dependencies**
3. **QEMU for emulation** (if building on different architecture)

Example for i386:
```bash
# Set architecture in build-deb.sh
ARCHITECTURE="i386"
# Install i386 dependencies
sudo dpkg --add-architecture i386
sudo apt-get update
```

### Development and Testing

For development builds without packaging:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./src/kydra
```

This guide provides everything needed to build, install, and use Kydra with full KDE menu integration on Debian-based systems.