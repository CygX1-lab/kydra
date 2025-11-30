# Kydra

A powerful, KDE-native package manager forked from Muon.

[Kydra Main Window](docs/screenshots/main_window.png)

Kydra brings a modern, Plasma-integrated interface to Debian package management. Designed around clarity and speed, it gives users full control of repositories, updates, packages, and system components through a clean, multi-view UI.

## ✨ Features

*   **⚡ Fast & Responsive**: Multi-threaded architecture ensures the UI never freezes during package operations.
*   **🔍 Powerful Search**: Filter by status (installed, upgradeable), category, or technical details with instant results.
*   **📦 Complete Package Management**: Install, remove, purge, and upgrade packages with ease.
*   **🕸️ Dependency Visualization**: View and understand package dependencies before making changes.
*   **🔧 Repository Control**: Manage your sources, PPAs, and updates directly from the settings.
*   **📂 Local Package Support**: Seamlessly install downloaded `.deb` files.
*   **🎨 Plasma Integration**: Built with Qt and KDE Frameworks to look and feel at home on your desktop.

## 🚀 Quick Start

### Launching Kydra
You can find Kydra in your application menu under **System > Package Manager**, or run it from the terminal:

```bash
kydra
```

### Common Tasks

*   **Find a Package**: Use the search bar at the top. You can filter results using the sidebar categories.
*   **Install/Remove**: Click the checkbox next to a package name to mark it for Installation or Removal.
*   **Apply Changes**: Click the **Apply Changes** button in the toolbar to execute your queued actions.
*   **Update System**: Click **Check for Updates** to refresh package lists, then **Full Upgrade** to mark all upgrades.

## 📥 Download Kydra

You can download the latest **.deb package** here:

➡️ **[Latest Kydra Release](https://github.com/CygX1-lab/kydra/releases/)**

Once downloaded:

```bash
sudo dpkg -i kydra*.deb
sudo apt --fix-broken install
```

## 🏗️ Building from Source

For detailed instructions on building Kydra from source, including all dependencies and compilation steps, please refer to [INSTALL.md](INSTALL.md).

## 📞 Getting Help

If you encounter any issues or have questions, please contact:

*   **Email**: `cygnx1@gmail.com`

## 💖 Support Development

This project is free and open-source software released under the GNU GPLv3. If you find it useful, consider supporting development:

*   **Bitcoin (BTC)**: `bc1q4z3d06unklcp868wgy6uy8t6y424r8hvy32uvw`
*   **Ethereum (ETH)**: `0xcAFdd35c1e00e6cc142F3df0c5DA4B0D428e6bf9`
*   **Solana (SOL)**: `89xBb3fXYm68WHJsi7hpNBv4hVcaN3GJVXsLNiuYVqv1`
