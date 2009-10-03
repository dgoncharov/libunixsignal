EXTRA_PROGRAMS = sigselect sighandler msigselect
sigselect_SOURCES = sigselect.cpp
sighandler_SOURCES = sighandler.cpp
msigselect_SOURCES = msigselect.cpp
INCLUDES = -I$(top_srcdir)/include

sample: sigselect$(EXEEXT) sighandler$(EXEEXT) msigselect$(EXEEXT)
	./sigselect$(EXEEXT)

.PHONY: sample

