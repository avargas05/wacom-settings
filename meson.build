project('wacom-settings', 'c',
          version: '0.2.0',
    meson_version: '>= 1.1',
  default_options: [ 'warning_level=2',
                     'c_std=gnu17',
                   ],
)

i18n = import('i18n')

config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', 'wacom-settings')
config_h.set_quoted('LOCALEDIR', join_paths('/usr', get_option('localedir')))
configure_file(
  output: 'wacom_settings-config.h',
  configuration: config_h,
)
add_project_arguments([
  '-I' + meson.project_build_root(),
], language: 'c')


subdir('data')
subdir('src')
subdir('po')

meson.add_install_script('build-aux/meson/postinstall.py')
