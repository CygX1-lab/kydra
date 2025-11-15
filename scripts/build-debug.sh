#!/bin/bash
# Kydra Debug Build Script
# Builds Kydra with debug symbols and development options

set -e

echo "Building Kydra in debug mode..."

# Create build directory
BUILD_DIR="build-debug"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with debug options
echo "Configuring with debug options..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DKDE_INSTALL_USE_QT_SYS_PATHS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_TESTING=ON

# Build
echo "Building..."
make -j$(nproc)

echo "Debug build complete!"
echo "To install: cd $BUILD_DIR && sudo make install"
echo "To run: kydra"
echo ""
echo "Development tips:"
echo "- Use 'gdb kydra' for debugging"
echo "- Check build-debug/compile_commands.json for IDE integration"
echo "- Run tests with 'make test' or 'ctest'"