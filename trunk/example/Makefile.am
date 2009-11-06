EXTRA_PROGRAMS = sigselect sighandler msigselect multisigselect msighandler
sigselect_SOURCES = sigselect.cpp
sighandler_SOURCES = sighandler.cpp
msigselect_SOURCES = msigselect.cpp
multisigselect_SOURCES = multisigselect.cpp
msighandler_SOURCES = msighandler.cpp
INCLUDES = -I$(top_srcdir)/include

sample: sigselect$(EXEEXT) sighandler$(EXEEXT) msigselect$(EXEEXT) multisigselect$(EXEEXT) msighandler$(EXEEXT)
	./sigselect$(EXEEXT)

.PHONY: sample

