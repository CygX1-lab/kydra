# Contributing to Kydra

Thank you for your interest in contributing to Kydra! This document provides guidelines and information for contributors.

## Code of Conduct

By participating in this project, you agree to abide by the KDE Code of Conduct. Please read it at https://kde.org/code-of-conduct/

## Getting Started

### Development Setup

1. **Fork the repository** on KDE Invent or GitHub
2. **Clone your fork**:
   ```bash
   git clone https://invent.kde.org/[your-username]/kydra.git
   cd kydra
   ```
3. **Install dependencies** (Debian/Ubuntu):
   ```bash
   sudo apt install build-essential cmake extra-cmake-modules \
       qtbase5-dev libkf5config-dev libkf5xmlgui-dev libkf5i18n-dev \
       libkf5widgetsaddons-dev libqt5svg5-dev libqapt-dev pkg-kde-tools
   ```
4. **Build the project**:
   ```bash
   ./scripts/build-debug.sh
   ```

### Development Workflow

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```
2. **Make your changes** and test thoroughly
3. **Run tests** if available:
   ```bash
   cd build-debug && make test
   ```
4. **Commit your changes** with a clear commit message
5. **Push to your fork** and create a Pull Request

## Contribution Guidelines

### What We're Looking For

- **Bug fixes** - Help us squash bugs
- **Feature implementations** - New functionality
- **Documentation improvements** - Better docs and examples
- **Translations** - Help make Kydra available in more languages
- **Testing** - Report bugs and test new features
- **Performance improvements** - Make Kydra faster and more efficient

### Code Style

Please follow the KDE coding style:

- **C++ Code Style**: Follow the [KDE C++ Coding Style](https://community.kde.org/Policies/Frameworks_Coding_Style)
- **Qt Best Practices**: Use Qt idioms and patterns
- **Modern C++**: Use C++17 features appropriately
- **Variable naming**: Use camelCase for variables and functions
- **Class naming**: Use CamelCase for classes
- **Constants**: Use UPPER_CASE for constants

### Commit Messages

Write clear, concise commit messages:

```
Component: Brief description of change

Longer explanation of the change, why it's needed,
and what it does. Wrap at 72 characters.

Fixes #123
```

Example:
```
PackageModel: Improve search performance

Refactors the search algorithm to use parallel processing
for faster package discovery. Reduces search time by ~40%
for large repositories.

Fixes #45
```

## Areas of Contribution

### 1. User Interface

**Files**: `src/*.cpp`, `src/*Widget*.cpp`, `src/*Tab*.cpp`

- Improve UI/UX design
- Add new interface elements
- Enhance accessibility
- Fix UI bugs and glitches

### 2. Package Management Backend

**Files**: `src/muonapt/*.cpp`, `src/PackageModel/*.cpp`

- Improve APT integration
- Enhance package search
- Optimize dependency resolution
- Add new package operations

### 3. Filtering and Search

**Files**: `src/FilterWidget/*.cpp`

- Improve search algorithms
- Add new filter criteria
- Enhance search performance
- Better search result presentation

### 4. Configuration and Settings

**Files**: `src/config/*.cpp`, `data/kydra.schemas`

- Add new configuration options
- Improve settings UI
- Enhance KConfig integration
- Better default configurations

### 5. Documentation

**Files**: `docs/*.md`

- Improve user documentation
- Add developer guides
- Update API documentation
- Create tutorials and examples

### 6. Translations

**Files**: `po/*.po`, `data/*.desktop`, `data/*.metainfo.xml`

- Translate to new languages
- Update existing translations
- Improve translation coverage
- Fix translation issues

## Testing

### Manual Testing

1. **Test basic functionality**:
   - Package search and filtering
   - Installation and removal
   - Update management
   - Repository handling

2. **Test edge cases**:
   - Large package sets
   - Broken dependencies
   - Network issues
   - Permission problems

3. **Test UI/UX**:
   - Different screen sizes
   - Various KDE themes
   - Accessibility features
   - Keyboard navigation

### Automated Testing

Run existing tests:
```bash
cd build-debug
make test
ctest --verbose
```

Add new tests for your features in the `tests/` directory.

## Reporting Issues

### Bug Reports

When reporting bugs, please include:

1. **Kydra version** and **system information**
2. **Steps to reproduce** the issue
3. **Expected behavior** vs **actual behavior**
4. **Error messages** or **screenshots**
5. **System logs** if relevant

Use the KDE Bug Tracker: https://bugs.kde.org/enter_bug.cgi?format=guided&product=Kydra

### Feature Requests

For feature requests, please:

1. **Check existing issues** first
2. **Describe the feature** clearly
3. **Explain the use case**
4. **Consider implementation complexity**

## Pull Request Process

1. **Update documentation** if needed
2. **Add tests** for new features
3. **Ensure code compiles** without warnings
4. **Follow the coding style** guidelines
5. **Write clear commit messages**
6. **Reference related issues** in the PR description

### PR Review Process

- All PRs require **review by maintainers**
- **CI checks** must pass
- **Documentation** must be updated
- **Tests** should be included for new features
- **Translation strings** should be marked properly

## Development Resources

### Documentation

- [KDE Developer Documentation](https://develop.kde.org/)
- [Qt Documentation](https://doc.qt.io/)
- [CMake Documentation](https://cmake.org/documentation/)
- [KDE Frameworks Documentation](https://api.kde.org/frameworks/)

### Communication

- **IRC**: #kde-devel on Libera.Chat
- **Matrix**: #kde-devel:kde.org
- **Forums**: https://forum.kde.org/
- **Mailing List**: kde-devel@kde.org

## Recognition

Contributors will be recognized in:

- The project's AUTHORS file
- Release announcements
- The about dialog in the application

## Questions?

If you have questions about contributing:

1. Check this document first
2. Search existing issues and discussions
3. Ask in the development channels
4. Contact the maintainers

Thank you for contributing to Kydra! Your help makes KDE better for everyone.

---

**Happy coding!** 🐉