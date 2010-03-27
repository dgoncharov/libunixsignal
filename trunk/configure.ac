#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([], [], [dgoncharov@users.sf.net])
AM_INIT_AUTOMAKE([libunixsignal], ["0.0.5"])
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

BOOST_NOT_FOUND_ERRMSG="You need to install the boost libraries.\
 The boost libraries can be obtained from boost.org.\
 If you have the boost libraries installed and their file names have suffixes have a look at the --with-boost-libs-suffix option"

save_LIBS="$LIBS"
LIBS="$LIBS -lpthread"
AC_MSG_CHECKING([for pthread library])
AC_LINK_IFELSE([
#include <pthread.h>
int main() {pthread_self(); return 0;}
],
    [PTHREAD_LIB="-lpthread"
    AC_MSG_RESULT([$PTHREAD_LIB])],
    [AC_MSG_RESULT([not found])
    AC_MSG_ERROR([You need to have the pthread library installed])]
)
AC_SUBST(PTHREAD_LIB)
LIBS="$save_LIBS"

AC_ARG_WITH(boost-libs-suffix,
AC_HELP_STRING(
    [--with-boost-libs-suffix=suffix],
    [boost libraries suffix.
    If your boost libraries look like libboost_system-mt.so then the suffix is -mt.
    You need to specify a suffix if you want to force linking against a library with that suffix]))

save_LIBS="$LIBS"
LIBS="$LIBS -lboost_system$with_boost_libs_suffix"
AC_MSG_CHECKING([for Boost.System library])
AC_LINK_IFELSE([
    #include <boost/system/system_error.hpp>
    int main() {boost::system::system_error(boost::system::error_code(0, boost::system::get_system_category())); return 0;}
],
    [BOOST_SYSTEM_LIB="-lboost_system$with_boost_libs_suffix"
    AC_MSG_RESULT([$BOOST_SYSTEM_LIB])],
    [AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Can't find the Boost.System library. $BOOST_NOT_FOUND_ERRMSG])])
AC_SUBST(BOOST_SYSTEM_LIB)
LIBS="$save_LIBS"

save_LIBS="$LIBS"
LIBS="$LIBS -lboost_unit_test_framework$with_boost_libs_suffix"
AC_MSG_CHECKING([for Boost.Test library])
AC_LINK_IFELSE([
    #define BOOST_TEST_DYN_LINK
    #include <boost/test/unit_test.hpp>
    bool init_unit_test() { return true;}
    int main(int argc, char** argv) { boost::unit_test::unit_test_main(&init_unit_test, argc, argv); return 0;}
],
    [BOOST_UNIT_TEST_FRAMEWORK_LIB="-lboost_unit_test_framework$with_boost_libs_suffix"
    AC_MSG_RESULT([$BOOST_UNIT_TEST_FRAMEWORK_LIB])],
    [AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Can't find the Boost.Test library. $BOOST_NOT_FOUND_ERRMSG])])
AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB)
LIBS="$save_LIBS"

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
    CXXFLAGS="$CXXFLAGS -O0 -g"
else
    AC_DEFINE(NDEBUG,, [Disable debugging])
    CXXFLAGS="$CXXFLAGS -O2"
fi
CXXFLAGS="$CXXFLAGS -Wall -Wextra -Werror"

AC_CONFIG_FILES([Makefile include/Makefile example/Makefile test/Makefile doc/Makefile])

AC_OUTPUT

