SUBDIRS = 

# the library name to build
lib_LTLIBRARIES = libunixsignal.la

# the list of header files that belong to the library
libunixsignal_la_HEADERS = $(top_srcdir)/include/unixsignal/signalfd.hpp

# where to install the headers on the system
libunixsignal_ladir = $(includedir)/unixsignal

# the sources to add to the library and to add to the distribution
libunixsignal_la_SOURCES = $(libunixsignal_la_HEADERS) $(top_srcdir)/src/unixsignal.cpp
libunixsignal_la_LDFLAGS = -avoid-version -no-undefined
INCLUDES = -I$(top_srcdir)/include
libunixsignal_la_LIBADD = -lboost_system

