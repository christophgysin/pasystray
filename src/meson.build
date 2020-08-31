if get_option('gtk') == '2'
  gladefile = 'pasystray.gtk2.glade'
else
  gladefile = 'pasystray.gtk3.glade'
endif

install_data(gladefile, install_dir: pkgdatadir)

conf_data = configuration_data()
conf_data.set('GTK_VERSION_MAJOR', get_option('gtk'))
conf_data.set10('HAVE_APPINDICATOR', appindicator.found())
conf_data.set10('HAVE_AVAHI', avahi_client.found() and avahi_glib.found())
conf_data.set10('HAVE_NOTIFY', notify.found())
conf_data.set10('HAVE_STATUSICON', get_option('statusicon'))
conf_data.set10('HAVE_X11', x11.found())
conf_data.set_quoted('G_LOG_DOMAIN', meson.project_name())
conf_data.set_quoted('PACKAGE_NAME', meson.project_name())
conf_data.set_quoted('PACKAGE_URL', 'http://github.com/christophgysin/pasystray')
conf_data.set_quoted('PACKAGE_VERSION', meson.project_version())
conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('GLADE_FILE', pkgdatadir + '/' + gladefile)
config_h = configure_file(input : 'config.h.meson',
                          output : 'config.h',
                          configuration : conf_data)

srcs = files('avahi.c', 'avahi.h',
             'menu_info.c', 'menu_info.h',
             'notify.c', 'notify.h',
             'options.c', 'options.h',
             'pasystray.c', 'pasystray.h',
             'pulseaudio.c', 'pulseaudio.h',
             'pulseaudio_action.c', 'pulseaudio_action.h',
             'pulseaudio_info.c', 'pulseaudio_info.h',
             'systray.c', 'systray.h',
             'systray_impl.c', 'systray_impl.h',
             'ui.c', 'ui.h',
             'x11-property.c', 'x11-property.h')

executable('pasystray', srcs, config_h,
           dependencies: [gtk, pulse, pulse_glib, appindicator, avahi_client, avahi_glib, notify, x11],
           install: true)
