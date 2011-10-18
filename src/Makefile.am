bin_PROGRAMS=pasystray

pasystray_SOURCES=pasystray.c menu_info.c systray.c pulseaudio.c pulseaudio_info.c pulseaudio_action.c \
				  menu_info.h systray.h pulseaudio.h pulseaudio_info.h pulseaudio_action.h

pasystray_LDADD=$(AM_LDADD) $(GUILIBS_LIBS) $(PULSE_LIBS)
pasystray_CFLAGS=$(AM_CFLAGS) $(GUILIBS_CFLAGS) $(PULSE_CFLAGS)
