#!/bin/bash
# Kydra Translation Update Script
# Updates translation files for the Kydra project

set -e

echo "Updating Kydra translations..."

# Check if required tools are available
if ! command -v xgettext &> /dev/null; then
    echo "Error: xgettext not found. Please install gettext tools."
    exit 1
fi

if ! command -v msgmerge &> /dev/null; then
    echo "Error: msgmerge not found. Please install gettext tools."
    exit 1
fi

# Create po directory if it doesn't exist
mkdir -p po

# Extract translatable strings from source files
echo "Extracting translatable strings..."
find src -name "*.cpp" -o -name "*.h" | xargs xgettext \
    --keyword=i18n \
    --keyword=i18nc:1c,2 \
    --keyword=i18np:1,2 \
    --keyword=i18ncp:1c,2,3 \
    --language=C++ \
    --add-comments=TRANSLATORS: \
    --sort-by-file \
    --output=po/kydra.pot

# Update existing translation files
for po_file in po/*.po; do
    if [ -f "$po_file" ]; then
        echo "Updating $po_file..."
        msgmerge --update --backup=none "$po_file" po/kydra.pot
    fi
done

echo "Translation update complete!"