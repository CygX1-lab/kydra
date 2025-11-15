# Kydra

A modern, KDE-native package manager forked from Muon.

## Overview

Kydra brings a refreshed, Plasma-integrated interface to Debian package management. It combines the solid backend of Muon with a redesigned UI, faster search, and clearer workflows built on KDE Frameworks.

## Features

- **Clean, Breeze-styled interface** - Native KDE look and feel
- **Fast search** - Parallel metadata requests for instant results
- **Improved repository management** - Easy source control and management
- **Rich package information** - Detailed dependency trees and version history
- **Streamlined update workflow** - Clear, intuitive update management
- **Deep KDE integration** - Notifications, theming, KConfig support

## Screenshots

*Browse installed, available, and upgradable packages with a clean KDE interface.*

*Detailed package info with dependency trees and version history.*

*Manage repositories and sources effortlessly.*

*Fast, parallelized search delivers results instantly.*

## Installation

### From Source

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

### Debian/Ubuntu Packages

Download the latest `.deb` package from the releases page and install:

```bash
sudo dpkg -i kydra_1.0.0-1_amd64.deb
sudo apt-get install -f  # Install dependencies if needed
```

### Flatpak (Coming Soon)

```bash
flatpak install flathub org.kde.kydra
```

## Building

### Requirements

- CMake 3.16+
- Qt5 (>= 5.15)
- KDE Frameworks 5 (>= 5.80)
- QApt library
- C++17 compatible compiler

### Build Dependencies (Debian/Ubuntu)

```bash
sudo apt install build-essential cmake extra-cmake-modules \
    qtbase5-dev libkf5config-dev libkf5xmlgui-dev libkf5i18n-dev \
    libkf5widgetsaddons-dev libqt5svg5-dev libqapt-dev pkg-kde-tools
```

### Development Build

```bash
# Use the provided development script
./scripts/build-debug.sh

# Or manually
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
make -j$(nproc)
```

## Usage

### Basic Operations

- **Search**: Type in the search bar to find packages
- **Install**: Right-click → Install, or use the toolbar button
- **Remove**: Right-click → Remove, or use the toolbar button
- **Update**: Click the update button or use System → Check for Updates

### Advanced Features

- **Repository Management**: Settings → Repositories
- **Dependency Visualization**: Package details → Dependencies tab
- **Package History**: View installation/upgrade history
- **Filtering**: Use the filter panel to narrow results by status, origin, etc.

## Configuration

Kydra stores its configuration in `~/.config/kydrarc`. You can modify settings through the GUI or edit the file directly.

Key settings include:
- Window geometry and state
- Update checking frequency
- Repository sources
- Display preferences

## Development

### Project Structure

```
src/                 # Source code
├── muonapt/        # APT backend integration
├── PackageModel/   # Package data models
├── FilterWidget/   # Search and filtering
├── DetailsTabs/    # Package detail views
└── config/         # Configuration handling

data/               # Desktop files and metadata
packaging/          # Debian packaging files
docs/               # Documentation
scripts/            # Build and utility scripts
```

### Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Translation

Help translate Kydra into your language:

```bash
# Update translation files
./scripts/update-translations.sh

# Edit translation files in po/ directory
# Submit your translations via Pull Request
```

## Roadmap

### Version 1.1
- [ ] Enhanced dependency graphing
- [ ] Package history improvements
- [ ] Performance optimizations

### Version 1.2
- [ ] Flatpak integration
- [ ] Snap package support
- [ ] Plugin architecture

### Version 1.3
- [ ] Qt6/KF6 support
- [ ] Advanced filtering options
- [ ] Custom actions and scripts

## Troubleshooting

### Common Issues

**Kydra won't start**
- Check that all dependencies are installed
- Try running from terminal: `kydra` to see error messages
- Check KDE session is properly initialized

**Package search is slow**
- Ensure QApt is properly configured
- Try rebuilding the package cache
- Check available system resources

**Updates not showing**
- Verify repository sources are enabled
- Check network connectivity
- Try refreshing the package cache

### Getting Help

- **Bug Reports**: [KDE Bug Tracker](https://bugs.kde.org/enter_bug.cgi?format=guided&product=Kydra)
- **Discussions**: [KDE Community Forums](https://forum.kde.org/)
- **Chat**: #kde-devel on Libera.Chat

## License

Kydra is licensed under the GNU General Public License v3.0 or later. See [COPYING](COPYING) for details.

## Acknowledgments

- **Muon Team** - For the solid backend foundation
- **KDE Community** - For frameworks, support, and testing
- **QApt Developers** - For the APT integration library
- **All Contributors** - Thank you for your contributions!

---

**Kydra** - *Multi-headed package mastery for KDE*