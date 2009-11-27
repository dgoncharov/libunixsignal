SUBDIRS = include example test doc

ACLOCAL_AMFLAGS = -I m4

#EXTRA_DIST = libunixsignal.pc.in
EXTRA_DIST = bootstrap.sh

#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = libunixsignal.pc

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

sample:
	cd example && $(MAKE) $(AM_MAKEFLAGS) sample
.PHONY: sample

