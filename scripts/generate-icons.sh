#!/bin/bash
# Kydra Icon Generation Script
# Generates various icon sizes from the master SVG icon

set -e

echo "Generating Kydra icons..."

# Check if required tools are available
if ! command -v rsvg-convert &> /dev/null; then
    echo "Error: rsvg-convert not found. Please install librsvg2-bin."
    exit 1
fi

# Master icon file
MASTER_ICON="kydra_master.svg"

# Check if master icon exists
if [ ! -f "$MASTER_ICON" ]; then
    echo "Error: Master icon $MASTER_ICON not found in current directory."
    exit 1
fi

# Output directory
OUTPUT_DIR="src/icons"
mkdir -p "$OUTPUT_DIR"

# Icon sizes to generate
SIZES=(16 22 24 32 48 64 128 256 512)

# Generate PNG icons for each size
for size in "${SIZES[@]}"; do
    echo "Generating ${size}x${size} icon..."
    rsvg-convert \
        --width="$size" \
        --height="$size" \
        --format=png \
        --output="$OUTPUT_DIR/kydra_icon_${size}.png" \
        "$MASTER_ICON"
done

# Copy master SVG as scalable icon
echo "Copying scalable SVG icon..."
cp "$MASTER_ICON" "$OUTPUT_DIR/org.kydra.app.svg"

# Generate symbolic icons (light and dark variants)
echo "Generating symbolic icons..."

# Create symbolic icon (simplified version)
cat > "$OUTPUT_DIR/kydra_symbolic.svg" << 'EOF'
<svg width="16" height="16" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
  <circle cx="8" cy="8" r="2" fill="currentColor"/>
  <path d="M8 2 C10 4 12 6 12 8" stroke="currentColor" stroke-width="1" stroke-linecap="round" fill="none"/>
  <path d="M2 8 C4 8 6 6 8 4" stroke="currentColor" stroke-width="1" stroke-linecap="round" fill="none"/>
  <path d="M8 14 C10 12 12 10 12 8" stroke="currentColor" stroke-width="1" stroke-linecap="round" fill="none"/>
</svg>
EOF

# Create dark and light symbolic links
ln -sf "kydra_symbolic.svg" "$OUTPUT_DIR/kydra_symbolic_dark.svg"
ln -sf "kydra_symbolic.svg" "$OUTPUT_DIR/kydra_symbolic_light.svg"

echo "Icon generation complete!"
echo "Generated icons in $OUTPUT_DIR"