EXTRA_PROGRAMS = sigselect sighandler
sigselect_SOURCES = sigselect.cpp
sighandler_SOURCES = sighandler.cpp
INCLUDES = -I$(top_srcdir)/include

sample: sigselect$(EXEEXT) sighandler$(EXEEXT)
	./sigselect$(EXEEXT)

.PHONY: sample

