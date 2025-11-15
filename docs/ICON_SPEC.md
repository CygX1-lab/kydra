# Kydra Icon Specification

## Overview

This document specifies the icon design and usage guidelines for Kydra, the KDE-native package manager.

## Design Concept

The Kydra icon is based on the **hydra** motif, representing the multi-headed architecture of the package manager. The design follows KDE's Breeze design language with geometric, minimalist styling.

## Icon Specifications

### Primary Icon

- **Concept**: Three-headed hydra abstracted into branching arcs
- **Style**: Flat Breeze++ design with geometric shapes
- **Colors**: 
  - Primary: Breeze Blue (#3daee9)
  - Secondary: Breeze Dark Blue (#1b91c9)
  - Outline: Neutral Grey (#40464f)
- **Shape**: Circular motion echoing KDE Plasma's swirl

### Color Palette

```xml
<svg width="256" height="256" viewBox="0 0 256 256" fill="none" xmlns="http://www.w3.org/2000/svg">
  <circle cx="128" cy="128" r="32" fill="#3daee9"/>
  <path d="M128 32 C160 64 192 96 200 128" stroke="#3daee9" stroke-width="18" stroke-linecap="round"/>
  <path d="M32 128 C64 128 96 96 128 64" stroke="#1b91c9" stroke-width="18" stroke-linecap="round"/>
  <path d="M128 224 C160 192 192 160 200 128" stroke="#3daee9" stroke-width="18" stroke-linecap="round"/>
</svg>
```

## Icon Sizes

Required icon sizes for complete KDE integration:

- **16x16**: System tray, menus
- **22x22**: Small toolbar icons
- **24x24**: Toolbar icons
- **32x32**: List views
- **48x48**: Application launcher
- **64x64**: Large list views
- **128x128**: About dialogs
- **256x256**: High DPI displays
- **512x512**: App stores, marketing
- **Scalable SVG**: Primary format

## Symbolic Icons

Symbolic icons for dark/light themes:

- **Design**: Simplified hydra with currentColor fill
- **Usage**: System tray, status indicators
- **Variants**: Automatic dark/light theme switching

## File Naming Convention

```
org.kydra.app.svg              # Scalable application icon
kydra_icon_[size].png          # Raster icons (16, 22, 24, 32, 48, 64, 128, 256)
kydra_symbolic.svg             # Symbolic icon
kydra_symbolic_dark.svg        # Dark theme symbolic (symlink)
kydra_symbolic_light.svg       # Light theme symbolic (symlink)
```

## Installation Paths

Icons should be installed to:

```
/usr/share/icons/hicolor/[size]x[size]/apps/
/usr/share/icons/hicolor/scalable/apps/
```

## Design Guidelines

1. **Keep it simple**: Avoid excessive detail
2. **Maintain consistency**: Follow Breeze design principles
3. **Use proper colors**: Stick to the specified color palette
4. **Test at all sizes**: Ensure readability at 16x16
5. **Consider themes**: Works with both light and dark themes

## Generation Process

Use the provided `scripts/generate-icons.sh` script to automatically generate all required icon sizes from the master SVG file.

## Maintenance

- Update master SVG for design changes
- Regenerate all sizes after modifications
- Test on both light and dark themes
- Verify symbolic icon visibility