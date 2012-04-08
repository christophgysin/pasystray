bin_PROGRAMS=pasystray

pasystray_SOURCES=pasystray.c menu_info.c systray.c pulseaudio.c \
	pulseaudio_info.c pulseaudio_action.c options.c notify.c \
	menu_info.h systray.h pulseaudio.h pulseaudio_info.h pulseaudio_action.h \
	options.h notify.h

pasystray_LDADD=$(AM_LDADD) $(GUILIBS_LIBS) $(PULSEAUDIO_LIBS) $(NOTIFY_LIBS)
pasystray_CFLAGS=$(AM_CFLAGS) $(GUILIBS_CFLAGS) $(PULSEAUDIO_CFLAGS) $(NOTIFY_CFLAGS)
