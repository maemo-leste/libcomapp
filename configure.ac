AC_INIT([libcomapp], [1.8.04])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
AM_CONFIG_HEADER([config.h])

AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

CFLAGS="$CFLAGS -Wall -Wmissing-prototypes -Wmissing-declarations"

PKG_CHECK_MODULES(PACKAGE, [gtk+-2.0 >= 2.0.0 glib-2.0 >= 2.0.0 gobject-2.0 \
hildon-1 dbus-glib-1 dbus-1 libosso \
hildon-fm-2 >= 0.8.1 gconf-2.0 >= 2.16.0 ])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl Unit test support
AC_ARG_ENABLE(unit-tests,
              AS_HELP_STRING([--enable-unit-tests], [enable unit tests]),
              [ want_unit_tests=$enableval ],
              [ want_unit_tests=no])

AM_CONDITIONAL([ENABLE_UNIT_TESTS], [test "x$want_unit_tests" = "xyes"])

if test "x$want_unit_tests" = "xyes"; then
  PKG_CHECK_MODULES([CHECK], [ check >= 0.9.5 ])
  AC_SUBST(CHECK_CFLAGS)
  AC_SUBST(CHECK_LIBS)
fi

PACKAGE_INC=libcomapp
AC_SUBST(PACKAGE_INC)
AC_DEFINE_UNQUOTED(PACKAGE_INC,"$PACKAGE_INC",[Name of the include directory])

dnl Set PACKAGE_LOCALE_DIR in config.h
DATADIRNAME="share"
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR], ["${ac_default_prefix}/${DATADIRNAME}/locale"],[Locale directory])
else
  AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR], ["${prefix}/${DATADIRNAME}/locale"], [Locale directory])
fi
AC_SUBST(PACKAGE_LOCALE_DIR)


AC_OUTPUT([
Makefile
src/Makefile
test/Makefile
comapp.pc
debian/libcomapp-dev.install
])

