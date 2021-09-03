wacom_settings_sources = [
  'main.c',
  'wacom_settings-window.c',
  'parser.c',
  'datatypes.c',
  'windowmanager.c',
]

wacom_settings_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk4'),
  dependency('x11'),
]

gnome = import('gnome')

wacom_settings_sources += gnome.compile_resources(
  'wacom_settings-resources',
  'wacom_settings.gresource.xml',
  c_name: 'wacom_settings'
)

executable(
  'wacom-settings',
  wacom_settings_sources,
  dependencies: wacom_settings_deps,
  install: true,
)
