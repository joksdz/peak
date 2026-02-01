# Peak

**Peak** is a lightweight, high-performance application launcher built with C, GTK3, and GTK Layer Shell. Designed for speed and simplicity, it provides an instant search interface for your installed applications without the visual clutter of full desktop environments.

## Features

- **Icon Support:** Automatically loads and scales application icons from system themes or file paths.
- **Keyboard Centric:** Fully navigable interface (Arrows to select, Enter to launch, Esc to quit).
- **Wayland Native:** Built using `gtk-layer-shell` for seamless integration with compositors like Hyprland and Sway.
 ## **Upcoming:**
-  #### **Wallpaper Mode** — A dedicated feature to browse and set wallpapers directly from the interface and
-   #### **x11 support**

## Dependencies

To build Peak, you need a C compiler and the development headers for GTK3 and Layer Shell.

- `gtk+-3.0`
- `gtk-layer-shell`

**Arch Linux:**
```bash
sudo pacman -S gtk3 gtk-layer-shell
```
**Fedora:**
```bash

sudo dnf install gtk3-devel gtk-layer-shell-devel
```
**Debian/Ubuntu:**
```bash

sudo apt install libgtk-3-dev libgtk-layer-shell-dev
```

## Installation

Peak comes with a Makefile that handles compilation and resource installation (CSS styles).

   Clone the repository:
  ```Bash

git clone [https://github.com/yourusername/peak.git](https://github.com/joksdz/peak.git)
cd peak
  ```
Build and Install: Use sudo to install the binary to /usr/local/bin and assets to /usr/local/share.
```Bash

    sudo make install
```
### Uninstallation
(this will make me sad ;( )
If you wish to remove Peak from your system:
```Bash

sudo make uninstall
```
## Customization

Peak's appearance is controlled by a CSS file. After installation, you can modify the styles (colors, fonts, borders) by editing:
```Bash

/usr/local/share/peak/style.css
```
the default is nice tho : 
<img width="1629" height="899" alt="image" src="https://github.com/user-attachments/assets/39e650ba-6546-4bc4-982b-1a122ea199cf" />

