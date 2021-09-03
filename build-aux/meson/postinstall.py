#!/usr/bin/env python3

from os import environ, path
from subprocess import call

# Package managers set this so we don't need to run
if not environ.get('DESTDIR', ''):
    prefix = environ.get('MESON_INSTALL_PREFIX', '/usr')
    datadir = path.join(prefix, 'share')

    print('Updating icon cache...')
    call(['gtk-update-icon-cache',
          '-qtf',
          path.join(datadir, 'icons', 'hicolor')])

    print('Updating desktop database...')
    call(['update-desktop-database', '-q', path.join(datadir, 'applications')])

    print('Compiling GSettings schemas...')
    call(['glib-compile-schemas', path.join(datadir, 'glib-2.0', 'schemas')])


