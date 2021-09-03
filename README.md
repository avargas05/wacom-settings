# wacom-settings

GUI for calibrating the wacom tablet to a screen or window.

Learning project for writing in C. Uses GTK4 library for the GUI and to list windows opened on the desktop. Runs cmd-line functions from xf86-input-wacom for getting details of the devices and setting the areas. Have only tested on Arch Linux running xfce4.

## Usage

All of your tablet devices will appear in the Devices drop-down menu. All the monitors will appear in the monitors drop-down. All open windows including the panel and desktop space will appear in the Windows drop-down. This is done automatically. Select a device and either a monitor or window to map the tablet to. If you wish to keep the aspect ratio click the checkbox for that and it will adjust the width or height of the tablet to keep the aspect ratio.


![Alt text](https://github.com/avargas05/wacom-settings/raw/main/data/resources/screenshots/screenshot1.png "Main window")

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
