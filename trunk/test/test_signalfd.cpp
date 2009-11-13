// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#include <unixsignal/signalfd.hpp> // make sure the header is self contained
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <cerrno>
#include <iostream>
#include <boost/current_function.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace
{
using std::cout;
using std::endl;

void test_signalfd()
{
    cout << BOOST_CURRENT_FUNCTION << endl;

    unixsignal::signalfd<SIGINT> sigint;
    int const intfd = sigint.fd();

    unixsignal::signalfd<SIGTERM> sigterm;
    int const termfd = sigterm.fd();

    fd_set rfds;
    {
        raise(SIGINT);

        FD_ZERO(&rfds);
        FD_SET(intfd, &rfds);
        FD_SET(termfd, &rfds);
    
        int s = select(std::max(intfd, termfd) + 1, &rfds, 0, 0, 0);
        BOOST_CHECK(1 == s || EINTR == errno);
        BOOST_CHECK(FD_ISSET(intfd, &rfds));
        BOOST_CHECK(!FD_ISSET(termfd, &rfds));
        siginfo_t siginfo;
        while ((s = read(intfd, &siginfo, sizeof siginfo)) < 0 && EINTR == errno);
        BOOST_CHECK(sizeof siginfo == s);
        BOOST_CHECK(SIGINT == siginfo.si_signo);
    }
    {
        raise(SIGTERM);

        FD_ZERO(&rfds);
        FD_SET(intfd, &rfds);
        FD_SET(termfd, &rfds);

        int s = select(std::max(intfd, termfd) + 1, &rfds, 0, 0, 0);
    
        BOOST_CHECK(1 == s || EINTR == errno);
        BOOST_CHECK(FD_ISSET(termfd, &rfds));
        BOOST_CHECK(!FD_ISSET(intfd, &rfds));
        siginfo_t siginfo;
        while ((s = read(termfd, &siginfo, sizeof siginfo)) < 0 && EINTR == errno);
        BOOST_CHECK(sizeof siginfo == s);
        BOOST_CHECK(SIGTERM == siginfo.si_signo);
    }
}

void test_multisignalfd()
{
    cout << BOOST_CURRENT_FUNCTION << endl;

    unixsignal::signalfd<SIGINT, SIGTERM> sigfd;
    int const fd = sigfd.fd();

    fd_set rfds;
    {
//        cout << "raising SIGINT" << endl;
        raise(SIGINT);

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
    
        int s = select(fd + 1, &rfds, 0, 0, 0);
        BOOST_CHECK(1 == s || EINTR == errno);
        BOOST_CHECK(FD_ISSET(fd, &rfds));
        siginfo_t siginfo;
        while ((s = read(fd, &siginfo, sizeof siginfo)) < 0 && EINTR == errno);
        BOOST_CHECK(sizeof siginfo == s);
		BOOST_CHECK(SIGINT == siginfo.si_signo);
    }
    {
//		cout << "raising SIGTERM" << endl;
        raise(SIGTERM);

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        int s = select(fd + 1, &rfds, 0, 0, 0);
    
        BOOST_CHECK(1 == s || EINTR == errno);
        BOOST_CHECK(FD_ISSET(fd, &rfds));
        siginfo_t siginfo;
        while ((s = read(fd, &siginfo, sizeof siginfo)) < 0 && EINTR == errno);
        BOOST_CHECK(sizeof siginfo == s);
        BOOST_CHECK(SIGTERM == siginfo.si_signo);
    }
}

} //namespace

void init_test_signalfd()
{
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signalfd));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_multisignalfd));
}

