# wacom-settings

GUI for calibrating the wacom tablet to a screen or window. Can adjust the width or height of the tablet to keep the aspect ratio.

Learning project for writing in C. Uses GTK4 library for the GUI and to list windows opened on the desktop. Runs cmd-line functions from xf86-input-wacom for getting details of the devices and setting the areas. Have only tested on Arch Linux running xfce4.

## Dependencies
- xf86-input-wacom
- gtk4
- meson
- ninja
- pkgconf (base-devel)

## Installation
~~~bash
git clone https://github.com/avargas05/wacom-settings.git
cd wacom-settings
meson build
ninja -C build && ninja -C build install
~~~
