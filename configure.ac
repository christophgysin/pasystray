#  This file is part of PaSystray
#
#  Copyright (C) 2011-2023  Christoph Gysin
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

AC_INIT([pasystray], [0.8.2], [christoph.gysin@gmail.com], [pasystray],
        [http://github.com/christophgysin/pasystray])
AC_CONFIG_SRCDIR([src/pasystray.c])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE([foreign 1.9 -Wall])
AC_PROG_CC
AM_PROG_CC_C_O

### check for gtk #############################################################
AC_ARG_WITH(gtk, AC_HELP_STRING([--with-gtk=2|3], [choose GTK version (default: 3)]),, with_gtk=3)
AC_MSG_CHECKING([GTK version to use])
case ${with_gtk} in
    2) GTK_VERSION=gtk+-2.0
       GTK_VERSION_MAJOR=2
       ;;
    3) GTK_VERSION=gtk+-3.0
       GTK_VERSION_MAJOR=3
       ;;
    *) AC_MSG_ERROR([unknown GTK version!]);;
esac
AC_MSG_RESULT($GTK_VERSION)

PKG_CHECK_MODULES(GTK, [ $GTK_VERSION ])
AC_DEFINE_UNQUOTED(GTK_VERSION_MAJOR, $with_gtk, [Have GTK version?])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_VERSION_MAJOR)

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

### optional gtk stausicon support ###########################################
AC_ARG_ENABLE([statusicon],
    AS_HELP_STRING([--disable-statusicon], [Disable optional statusicon support]))

AS_IF([test "x$enable_statusicon" != xno],
    HAVE_STATUSICON=1, HAVE_STATUSICON=0)

AC_SUBST(HAVE_STATUSICON)
AM_CONDITIONAL([HAVE_STATUSICON], [test "x$HAVE_STATUSICON" = x1])
AS_IF([test "x$HAVE_STATUSICON" = x1], AC_DEFINE([HAVE_STATUSICON], 1,
      [Have StatusIcon?]))

### optional libappindicator support ###########################################
AC_ARG_ENABLE([appindicator],
    AS_HELP_STRING([--disable-appindicator], [Disable optional appindicator support]))

case ${with_gtk} in
    2) APPINDICATOR_VERSION=appindicator-0.1;;
    3) APPINDICATOR_VERSION=appindicator3-0.1;;
esac
AS_IF([test "x$enable_appindicator" != xno],
    [PKG_CHECK_MODULES(APPINDICATOR, [ $APPINDICATOR_VERSION ], HAVE_APPINDICATOR=1,
                       HAVE_APPINDICATOR=0)], HAVE_APPINDICATOR=0)

AS_IF([test "x$enable_appindicator" = xyes && test "x$HAVE_APPINDICATOR" = x0],
    [AC_MSG_ERROR([*** libappindicator not found])])

AC_SUBST(APPINDICATOR_CFLAGS)
AC_SUBST(APPINDICATOR_LIBS)
AC_SUBST(HAVE_APPINDICATOR)
AM_CONDITIONAL([HAVE_APPINDICATOR], [test "x$HAVE_APPINDICATOR" = x1])
AS_IF([test "x$HAVE_APPINDICATOR" = x1], AC_DEFINE([HAVE_APPINDICATOR], 1,
      [Have AppIndicator?]))
### optional libayatana-appindicator support ###################################
AC_ARG_ENABLE([ayatana-appindicator],
    AS_HELP_STRING([--disable-ayatana-appindicator], [Disable optional ayatana-appindicator support]))

case ${with_gtk} in
    2) AYATANA_APPINDICATOR_VERSION=ayatana-appindicator-0.1;;
    3) AYATANA_APPINDICATOR_VERSION=ayatana-appindicator3-0.1;;
esac
AS_IF([test "x$enable_ayatana_appindicator" != xno],
    [PKG_CHECK_MODULES(AYATANA_APPINDICATOR, [ $AYATANA_APPINDICATOR_VERSION ], HAVE_AYATANA_APPINDICATOR=1,
                       HAVE_AYATANA_APPINDICATOR=0)], HAVE_AYATANA_APPINDICATOR=0)

AS_IF([test "x$enable_ayatana_appindicator" = xyes && test "x$HAVE_APPINDICATOR" = x0],
    [AC_MSG_ERROR([*** libappindicator not found])])

AC_SUBST(AYATANA_APPINDICATOR_CFLAGS)
AC_SUBST(AYATANA_APPINDICATOR_LIBS)
AC_SUBST(HAVE_AYATANA_APPINDICATOR)
AM_CONDITIONAL([HAVE_AYATANA_APPINDICATOR], [test "x$HAVE_AYATANA_APPINDICATOR" = x1])
AS_IF([test "x$HAVE_AYATANA_APPINDICATOR" = x1], AC_DEFINE([HAVE_AYATANA_APPINDICATOR], 1,
      [Have AyatanaAppIndicator?]))
################################################################################

if test "x$GCC" = xyes -o "x$CLANG" = xyes; then
    CFLAGS="$CFLAGS -pipe -Wall -W -Wno-unused-parameter"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
