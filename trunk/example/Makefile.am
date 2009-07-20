bin_PROGRAMS = sigselect sighandler
sigselect_SOURCES = sigselect.cpp
sighandler_SOURCES = sighandler.cpp
INCLUDES = -I$(top_builddir)/include
#AM_CFLAGS = -O0
#sigselect_LDFLAGS =
sigselect_LDADD = $(top_builddir)/src/libunixsignal.la
sighandler_LDADD = $(top_builddir)/src/libunixsignal.la

