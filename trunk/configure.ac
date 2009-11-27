#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([], [], [dgoncharov@users.sourceforge.net])
AM_INIT_AUTOMAKE([libunixsignal], ["0.0.1"])
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
    pthread.h sys/time.h sys/types.h unistd.h sys/select.h signal.h fcntl.h limits.h],,
    AC_MSG_ERROR([You need to have the libc headers installed]))

AC_CHECK_HEADERS([\
    cerrno cstring cstddef cstdlib cassert iostream iomanip string],,
    AC_MSG_ERROR([You need to have the C++ standard headers installed]))

AC_HEADER_STDBOOL

AC_ARG_WITH(boost,
AC_HELP_STRING(
    [--with-boost=prefix],
    [boost install directory]),
    boost_pathset=yes,
    boost_pathset=no)

if test x"$boost_pathset" = x"yes" ; then
    CPPFLAGS="$CPPFLAGS -isystem $with_boost/include"
    LDFLAGS="$LDFLAGS -L$with_boost/lib"
fi

AC_CHECK_HEADERS([ \
    boost/bind.hpp \
    boost/static_assert.hpp \
    boost/system/system_error.hpp \
    boost/function.hpp \
    boost/system/error_code.hpp \
    boost/current_function.hpp \
    boost/asio/buffer.hpp \
    boost/asio/io_service.hpp \
    boost/asio/posix/basic_stream_descriptor.hpp \
    boost/asio/posix/stream_descriptor.hpp \
    boost/asio/read.hpp \
    boost/asio/deadline_timer.hpp \
		boost/asio/error.hpp \
    boost/date_time/posix_time/posix_time.hpp \
    boost/scope_exit.hpp \
    boost/test/unit_test.hpp],,
    AC_MSG_ERROR([You need to have the boost headers installed]))

BOOST_NOT_FOUND_ERRMSG="You need to install the boost libraries. The boost libraries can be obtained from boost.org"

save_LIBS="$LIBS"
LIBS="$LIBS -lpthread"
AC_MSG_CHECKING([for pthread library])
AC_LINK_IFELSE([
#include <pthread.h>
int main() {pthread_self(); return 0;}
],
    [AC_MSG_RESULT([-lpthread])],
    [AC_MSG_RESULT([not found])
     AC_MSG_ERROR([You need to have the pthread library installed])]
)
LIBS="$save_LIBS -lpthread"

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
LIBS="-lboost_system $save_LIBS"

AH_TOP([
#ifndef CONFIG_H
#define CONFIG_H])
AH_BOTTOM([#endif /* CONFIG_H */])
CXXFLAGS="$CXXFLAGS -include config.h"

AC_MSG_CHECKING(whether to build with debug information)
debuger=no
AC_ARG_ENABLE(
    debug,
    AC_HELP_STRING([--enable-debug],[enable debugging [[default=no]]]),
    debuger="$enableval")
    AC_MSG_RESULT($debuger)

CXXFLAGS=`echo $CXXFLAGS | sed -e "s/-O[[0-3]]//g; s/-Os//g; s/-fomit-frame-pointer//g"`
if test x"$debuger" = x"yes"; then
    AC_DEFINE(_DEBUG,, [Enable debugging])
    AC_DEFINE(_GLIBCXX_DEBUG,, [gcc debug mode])
    CXXFLAGS="$CXXFLAGS -O0 -g"
else
    AC_DEFINE(NDEBUG,, [Disable debugging])
    CXXFLAGS="$CXXFLAGS -O2"
fi
CXXFLAGS="$CXXFLAGS -Wall -Wextra -Werror"

AC_CONFIG_FILES([Makefile include/Makefile example/Makefile test/Makefile doc/Makefile])

AC_OUTPUT

