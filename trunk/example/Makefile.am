EXTRA_PROGRAMS = sigselect sighandler
sigselect_SOURCES = sigselect.cpp
sighandler_SOURCES = sighandler.cpp
INCLUDES = -I$(top_srcdir)/include
sigselect_LDADD = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ @BOOST_SYSTEM_LIB@ @PTHREAD_LIB@
sighandler_LDADD = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ @BOOST_SYSTEM_LIB@ @PTHREAD_LIB@

sample: sigselect$(EXEEXT) sighandler$(EXEEXT)
	./sigselect$(EXEEXT)

.PHONY: sample

