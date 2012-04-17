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

AC_PREREQ([2.68])

AC_INIT([pasystray], [0.2.1], [christoph.gysin@gmail.com], [pasystray],
        [http://github.com/christophgysin/pasystray])
AC_CONFIG_SRCDIR([src/pasystray.c])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE([foreign 1.9 -Wall])
AC_PROG_CC
AM_PROG_CC_C_O

### check for gtk3 ############################################################
PKG_CHECK_MODULES(GUILIBS, [ gtk+-3.0 ])
AC_SUBST(GUILIBS_CFLAGS)
AC_SUBST(GUILIBS_LIBS)

### check for pulseaudio libs #################################################
PKG_CHECK_MODULES(PULSEAUDIO, [ libpulse >= 1.0 libpulse-mainloop-glib >= 1.0 ])
AC_SUBST(PULSEAUDIO_CFLAGS)
AC_SUBST(PULSEAUDIO_LIBS)

### optional avahi-client support #############################################
AC_ARG_ENABLE([avahi],
    AS_HELP_STRING([--disable-avahi], [Disable optional avahi support]))

AS_IF([test "x$enable_avahi" != xno],
    [PKG_CHECK_MODULES(AVAHI, [ avahi-client >= 0.6 avahi-glib >= 0.6 ], HAVE_AVAHI=1,
                       HAVE_AVAHI=0)], HAVE_AVAHI=0)

AS_IF([test "x$enable_avahi" = xyes && test "x$HAVE_AVAHI" = x0],
    [AC_MSG_ERROR([*** avahi-client not found])])

AC_SUBST(AVAHI_CFLAGS)
AC_SUBST(AVAHI_LIBS)
AC_SUBST(HAVE_AVAHI)
AM_CONDITIONAL([HAVE_AVAHI], [test "x$HAVE_AVAHI" = x1])
AS_IF([test "x$HAVE_AVAHI" = x1], AC_DEFINE([HAVE_AVAHI], 1,
      [Have avahi-client?]))

### optional libnotify support ################################################
AC_ARG_ENABLE([notify],
    AS_HELP_STRING([--disable-notify], [Disable optional libnotify support]))

AS_IF([test "x$enable_notify" != xno],
    [PKG_CHECK_MODULES(NOTIFY, [ libnotify >= 0.7 ], HAVE_NOTIFY=1,
                       HAVE_NOTIFY=0)], HAVE_NOTIFY=0)

AS_IF([test "x$enable_notify" = xyes && test "x$HAVE_NOTIFY" = x0],
    [AC_MSG_ERROR([*** libnotify not found])])

AC_SUBST(NOTIFY_CFLAGS)
AC_SUBST(NOTIFY_LIBS)
AC_SUBST(HAVE_NOTIFY)
AM_CONDITIONAL([HAVE_NOTIFY], [test "x$HAVE_NOTIFY" = x1])
AS_IF([test "x$HAVE_NOTIFY" = x1], AC_DEFINE([HAVE_NOTIFY], 1,
      [Have libnotify?]))

### optional Xlib support ######################################################
AC_ARG_ENABLE([x11],
    AS_HELP_STRING([--disable-x11], [Disable optional X11 support]))

AS_IF([test "x$enable_x11" != xno],
    [PKG_CHECK_MODULES(X11, [ x11 >= 1.4 ], HAVE_X11=1,
                       HAVE_X11=0)], HAVE_X11=0)

AS_IF([test "x$enable_x11" = xyes && test "x$HAVE_X11" = x0],
    [AC_MSG_ERROR([*** libX11 not found])])

AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)
AC_SUBST(HAVE_X11)
AM_CONDITIONAL([HAVE_X11], [test "x$HAVE_X11" = x1])
AS_IF([test "x$HAVE_X11" = x1], AC_DEFINE([HAVE_X11], 1,
      [Have X11?]))
################################################################################

if test "x$GCC" = xyes -o "x$CLANG" = xyes; then
    CFLAGS="$CFLAGS -pipe -Wall -W -Wno-unused-parameter"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
