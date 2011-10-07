desktopdir = $(datadir)/applications

bin_PROGRAMS=pasystray

dist_desktop_DATA=pasystray.desktop

pasystray_SOURCES=pasystray.c systray.c pulseaudio.c systray.h pulseaudio.h

pasystray_LDADD=$(AM_LDADD) $(GUILIBS_LIBS) $(PULSE_LIBS)
pasystray_CFLAGS=$(AM_CFLAGS) $(GUILIBS_CFLAGS) $(PULSE_CFLAGS) 
pasystray_CFLAGS+=-DDESKTOP_FILE=\"$(desktopdir)/pasystray.desktop\" 
