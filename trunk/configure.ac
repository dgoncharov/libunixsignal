#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([], [], [dgoncharov@users.sourceforge.net])
AM_INIT_AUTOMAKE([libunixsignal], ["0.0.0"])
AC_CONFIG_SRCDIR([include/unixsignal/signalfd.hpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

AC_LANG_CPLUSPLUS

AC_HEADER_STDBOOL

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
AC_CONFIG_MACRO_DIR([m4])

# Checks for header files.

AC_CHECK_HEADERS([\
    pthread.h],,
    AC_MSG_ERROR([You need to have pthread.h installed]))

AC_CHECK_HEADERS([\
    cstdio cstdlib cstring cerrno stdexcept iostream iomanip string],,
    AC_MSG_ERROR([You need to have the C++ standard headers installed]))

AC_HEADER_STDBOOL

AC_ARG_WITH(boost,
AC_HELP_STRING(
    [--with-boost=prefix],
    [boost install directory]),
    boost_pathset=yes,
    boost_pathset=no)

if test $boost_pathset = yes ; then
    CPPFLAGS="$CPPFLAGS -I$with_boost/include"
    LDFLAGS="$LDFLAGS -L$with_boost/lib"
fi

BOOST_NOT_FOUND_ERRMSG="You need to install the boost libraries. The boost libraries can be obtained from boost.org"

AC_CHECK_HEADERS([ \
    boost/bind.hpp \
    boost/thread.hpp \
    boost/asio/io_service.hpp \
    boost/asio/posix/stream_descriptor.hpp \
    boost/asio/read.hpp],,
    AC_MSG_ERROR([You need to have the boost headers installed]))

# Checks for library functions.
save_LIBS="$LIBS"
LIBS="$LIBS -lboost_system"
AC_MSG_CHECKING([for boost.system library])
AC_LINK_IFELSE([
#include <boost/system/error_code.hpp>
int main() {boost::system::error_code c; return 0;}
],
    [AC_MSG_RESULT([-lboost_system])],
    [AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Can't find the boost_system library. $BOOST_NOT_FOUND_ERRMSG])]
)
LIBS="$save_LIBS -lboost_system"

save_LIBS="$LIBS"
LIBS="$LIBS -lboost_thread"
AC_MSG_CHECKING([for boost.thread library])
AC_LINK_IFELSE([
#include <boost/thread.hpp>
int main() {boost::thread t; return 0;}
],
    [AC_MSG_RESULT([-lboost_thread])],
    [AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Can't find the boost_thread library. $BOOST_NOT_FOUND_ERRMSG])]
)
LIBS="$save_LIBS -lboost_thread"

AH_TOP([
#ifndef CONFIG_H
#define CONFIG_H])
AH_BOTTOM([#endif /* CONFIG_H */])
CXXFLAGS="$CXXFLAGS -include config.h"

AC_MSG_CHECKING(whether to build with debug information)
debuger=no
AC_ARG_ENABLE(
    debug,
    AC_HELP_STRING([--enable-debug],[enable debugging @<:@default=no@:>@]),
    debuger="$enableval")
    AC_MSG_RESULT($debuger)

if test x"$debuger" = x"yes"; then
    AC_DEFINE(_DEBUG,, [Enable debugging])
    CXXFLAGS=`echo $CXXFLAGS | sed -e "s/-O@<:@0-3@:>@//g; s/-Os//g; s/-fomit-frame-pointer//g"`
    CXXFLAGS="$CXXFLAGS -O0 -g -Wall -Werror"
else
    AC_DEFINE(NDEBUG,, [Disable debugging])
    CXXFLAGS="$CXXFLAGS -Wall -Werror"
fi

AC_CONFIG_FILES([Makefile src/Makefile include/Makefile include/unixsignal/Makefile example/Makefile])

AC_OUTPUT

