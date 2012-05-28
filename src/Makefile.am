#  This file is part of PaSystray
#
#  Copyright (C) 2011, 2012 Christoph Gysin
#
#  PaSystray is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation; either version 2.1 of the
#  License, or (at your option) any later version.
#
#  PaSystray is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with PaSystray; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA.

gladedir = $(pkgdatadir)

bin_PROGRAMS=pasystray
glade_DATA=pasystray.glade

pasystray_SOURCES=pasystray.c menu_info.c systray.c pulseaudio.c \
	pulseaudio_info.c pulseaudio_action.c options.c notify.c ui.c avahi.c \
	x11-property.c \
	menu_info.h systray.h pulseaudio.h pulseaudio_info.h pulseaudio_action.h \
	options.h notify.h ui.h avahi.h x11-property.h pasystray.h

pasystray_LDADD=$(AM_LDADD) $(GTK_LIBS) $(PULSEAUDIO_LIBS) \
				$(NOTIFY_LIBS) $(AVAHI_LIBS) $(X11_LIBS)
pasystray_CFLAGS=$(AM_CFLAGS) $(GTK_CFLAGS) $(PULSEAUDIO_CFLAGS) \
				$(NOTIFY_CFLAGS) $(AVAHI_CFLAGS) $(X11_CFLAGS) \
				-DGLADE_FILE=\"$(gladedir)/pasystray.glade\"

EXTRA_DIST = $(glade_DATA)
