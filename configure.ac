AC_PREREQ([2.68])
AC_INIT([pasystray], [0.1.3], [christoph.gysin@gmail.com])
AC_CONFIG_SRCDIR([src/pasystray.c])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE([foreign 1.9 -Wall])
AC_PROG_CC
AM_PROG_CC_C_O
PKG_CHECK_MODULES(GUILIBS, [ gtk+-3.0 ])
AC_SUBST(GUILIBS_CFLAGS)
AC_SUBST(GUILIBS_LIBS)
PKG_CHECK_MODULES(PULSEAUDIO, [ libpulse >= 1.0 libpulse-mainloop-glib >= 1.0 ])
AC_SUBST(PULSEAUDIO_CFLAGS)
AC_SUBST(PULSEAUDIO_LIBS)
PKG_CHECK_MODULES(AVAHI, [ avahi-client >= 0.6 ])
AC_SUBST(AVAHI_CFLAGS)
AC_SUBST(AVAHI_LIBS)
if test "x$GCC" = "xyes" -o "x$CLANG" = "xyes"; then
    CFLAGS="$CFLAGS -pipe -Wall -W -Wno-unused-parameter"
fi
AC_FUNC_MALLOC
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
