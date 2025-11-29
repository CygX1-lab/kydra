#!/bin/bash

# Test if local .deb packages are properly indexed

echo "=== Testing Local Package Detection ==="
echo

# Check what's in the configured local folder
LOCAL_FOLDER=$(grep localDebFolder ~/.config/kydrarc 2>/dev/null | cut -d= -f2 | tr -d '"')

if [ -z "$LOCAL_FOLDER" ]; then
    echo "No local folder configured in settings"
    LOCAL_FOLDER="$HOME/Downloads"
    echo "Using default: $LOCAL_FOLDER"
fi

echo "Local folder: $LOCAL_FOLDER"
echo

# List .deb files
echo "Looking for .deb files in local folder:"
if [ -d "$LOCAL_FOLDER" ]; then
    DEB_COUNT=$(find "$LOCAL_FOLDER" -maxdepth 1 -name "*.deb" 2>/dev/null | wc -l)
    echo "Found $DEB_COUNT .deb files"
    
    if [ $DEB_COUNT -gt 0 ]; then
        echo
        echo "Sample .deb files:"
        find "$LOCAL_FOLDER" -maxdepth 1 -name "*.deb" 2>/dev/null | head -n 3 | while read deb; do
            echo "  - $(basename "$deb")"
            # Try to extract package name
            PKG_NAME=$(dpkg-deb --field "$deb" Package 2>/dev/null)
            if [ -n "$PKG_NAME" ]; then
                echo "    Package name: $PKG_NAME"
                # Check if this package exists in APT
                if apt-cache show "$PKG_NAME" >/dev/null 2>&1; then
                    echo "    ✓ Package exists in APT database"
                    if dpkg -l "$PKG_NAME" 2>/dev/null | grep -q "^ii"; then
                        echo "    ✓ Currently installed"
                    else
                        echo "    ✗ Not installed (should show in filter if backend knows it)"
                    fi
                else
                    echo "    ✗ Package NOT in APT database (won't show - needs virtual package support)"
                fi
            fi
            echo
        done
    fi
else
    echo "Local folder does not exist: $LOCAL_FOLDER"
fi
