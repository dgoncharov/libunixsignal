check_PROGRAMS = unittest unittest_select
INCLUDES = -I$(top_srcdir)/include

unittest_SOURCES = unittest.cpp test_signalfd.cpp test_signal_handler.cpp
unittest_LDADD = -lboost_unit_test_framework

unittest_select_SOURCES = unittest.cpp test_signalfd.cpp test_signal_handler.cpp
unittest_select_LDADD = -lboost_unit_test_framework
unittest_select_CPPFLAGS = -DBOOST_ASIO_DISABLE_IOCP -DBOOST_ASIO_DISABLE_EPOLL -DBOOST_ASIO_DISABLE_KQUEUE -DBOOST_ASIO_DISABLE_DEV_POLL

EXTRA_DIST = ut.sh uts.sh
TESTS = ut.sh uts.sh

