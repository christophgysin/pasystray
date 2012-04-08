AC_PREREQ([2.68])

AC_INIT([pasystray], [0.1.3], [christoph.gysin@gmail.com])
AC_CONFIG_SRCDIR([src/pasystray.c])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE([foreign 1.9 -Wall])
AC_PROG_CC

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
    [PKG_CHECK_MODULES(AVAHI, [ avahi-client >= 0.6 ], HAVE_AVAHI=1,
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
################################################################################

if test "x$GCC" = xyes -o "x$CLANG" = xyes; then
    CFLAGS="$CFLAGS -pipe -Wall -W -Wno-unused-parameter"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
