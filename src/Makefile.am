#  This file is part of PaSystray
#
#  Copyright (C) 2011-2016  Christoph Gysin
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
gladefile = pasystray.gtk$(GTK_VERSION_MAJOR).glade
glade_DATA = $(gladefile)
EXTRA_DIST = $(glade_DATA)

bin_PROGRAMS = pasystray

pasystray_SOURCES = \
    avahi.c \
    avahi.h \
    menu_info.c \
    menu_info.h \
    notify.c \
    notify.h \
    options.c \
    options.h \
    pasystray.c \
    pasystray.h \
    pulseaudio.c \
    pulseaudio.h \
    pulseaudio_action.c \
    pulseaudio_action.h \
    pulseaudio_info.c \
    pulseaudio_info.h \
    systray.c \
    systray.h \
    systray_impl.c \
    systray_impl.h \
    ui.c \
    ui.h \
    x11-property.c \
    x11-property.h

if HAVE_X11
pasystray_SOURCES += \
    x11-key-grabber.c \
    x11-key-grabber.h
endif

pasystray_LDADD = \
    $(AM_LDADD) \
    $(GTK_LIBS) \
    $(PULSEAUDIO_LIBS) \
    $(NOTIFY_LIBS) \
    $(AVAHI_LIBS) \
    $(APPINDICATOR_LIBS) \
    $(AYATANA_APPINDICATOR_LIBS) \
    $(X11_LIBS)

pasystray_CFLAGS = \
    $(AM_CFLAGS) \
    $(GTK_CFLAGS) \
    $(PULSEAUDIO_CFLAGS) \
    $(NOTIFY_CFLAGS) \
    $(AVAHI_CFLAGS) \
    $(APPINDICATOR_CFLAGS) \
    $(AYATANA_APPINDICATOR_CFLAGS) \
    $(X11_CFLAGS) \
    -DG_LOG_DOMAIN=\"pasystray\" \
    -DGLADE_FILE=\"$(gladedir)/$(gladefile)\"
