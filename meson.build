project('pasystray', 'c',
        version: '0.7.1',
        default_options: ['warning_level=2'],
        license: 'LGPL2.1+',
        meson_version: '>=0.47.0')

datadir = join_paths(get_option('prefix'), get_option('datadir'))
pkgdatadir = join_paths(datadir, meson.project_name())
sysconfdir = join_paths(get_option('prefix'), get_option('sysconfdir'))

pulse = dependency('libpulse', version: '>=1.0', required: true)
pulse_glib = dependency('libpulse-mainloop-glib', version: '>=1.0', required: true)

if get_option('gtk') == '2'
  gtk = dependency('gtk+-2.0', required: true)
  appindicator = dependency('appindicator-0.1', required: get_option('appindicator'))
else
  gtk = dependency('gtk+-3.0', required: true)
  appindicator = dependency('appindicator3-0.1', required: get_option('appindicator'))
endif

avahi_client = dependency('avahi-client', version: '>=0.6', required: get_option('avahi'))
avahi_glib = dependency('avahi-glib', version: '>=0.6', required: get_option('avahi'))
notify = dependency('libnotify', version: '>=0.7', required: get_option('notify'))
x11 = dependency('x11', version: '>=1.4', required: get_option('x11'))

subdir('data')
subdir('man')
subdir('src')

run_target('doc',
           command: ['markdown_py', '-f', 'README.html', files('README.md')])
