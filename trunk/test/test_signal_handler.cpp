// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#include <unixsignal/signal_handler.hpp> // make sure the header is self contained
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <iostream>
#include <boost/current_function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scope_exit.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace
{
using std::cout;
using std::endl;

void test_signal_handler()
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, siginfo_t const& siginfo, siginfo_t* arrived)
        {
//            cout << "arrived signal #" << siginfo.si_signo << endl;
            BOOST_CHECK(!error);
            *arrived = siginfo;
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;

    boost::asio::io_service ios;

    unixsignal::signal_handler<SIGINT> sigint(ios);
    unixsignal::signal_handler<SIGTERM> sigterm(ios);

    {
        ios.reset();
        siginfo_t siginfo1;
        siginfo_t siginfo2;
        std::memset(&siginfo1, 0, sizeof siginfo1);
        std::memset(&siginfo2, 0, sizeof siginfo2);

        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));
        sigterm.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo2));

//        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        ios.run_one();

        BOOST_CHECK(SIGINT == siginfo1.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);

        ios.reset();
        std::memset(&siginfo1, 0, sizeof siginfo1);
        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));

//        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
        ios.run_one();

        BOOST_CHECK(0 == siginfo1.si_signo);
        BOOST_CHECK(SIGTERM == siginfo2.si_signo);

        ios.reset();
        std::memset(&siginfo2, 0, sizeof siginfo2);
        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));

//        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        ios.run_one();

        BOOST_CHECK(SIGINT == siginfo1.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);
    }
}

void test_multisignal_handler(int n)
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, siginfo_t const& siginfo, siginfo_t* arrived)
        {
//            cout << "arrived signal #" << siginfo.si_signo << endl;
            BOOST_CHECK(!error);
            *arrived = siginfo;
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;

    boost::asio::io_service ios;

    unixsignal::signal_handler<SIGINT, SIGTERM> sig(ios);

    for (int i = 0; i < n; ++i)
    {
        ios.reset();
        siginfo_t siginfo;
        siginfo_t siginfo2;
        std::memset(&siginfo, 0, sizeof siginfo);
        std::memset(&siginfo2, 0, sizeof siginfo2);

        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));

//        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        ios.run_one();
        BOOST_CHECK(SIGINT == siginfo.si_signo);

        ios.reset();
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));

//        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
        ios.run_one();
        BOOST_CHECK(SIGTERM == siginfo.si_signo);

        ios.reset();
        std::memset(&siginfo, 0, sizeof siginfo);
        std::memset(&siginfo2, 0, sizeof siginfo2);
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo2));

//        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
//        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        ios.run();
        BOOST_CHECK(SIGTERM == siginfo.si_signo);
        BOOST_CHECK(SIGINT == siginfo2.si_signo);
    }
}

void test_multisignal_handler2(int n)
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, siginfo_t const& siginfo, siginfo_t* arrived)
        {
//            cout << "arrived signal #" << siginfo.si_signo << endl;
            BOOST_CHECK(!error);
            *arrived = siginfo;
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;

    boost::asio::io_service ios;

    unixsignal::signal_handler<SIGINT, SIGTERM> sig(ios);

    for (int i = 0; i < n; ++i)
    {
        ios.reset();
        siginfo_t siginfo;
        siginfo_t siginfo2;
        std::memset(&siginfo, 0, sizeof siginfo);
        std::memset(&siginfo2, 0, sizeof siginfo2);
    
        ios.reset();
//        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
//        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        std::memset(&siginfo, 0, sizeof siginfo);
        std::memset(&siginfo2, 0, sizeof siginfo2);
    
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo2));
    
    
        BOOST_CHECK(0 == siginfo.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);
    
//        cout << "running once" << endl;
        ios.run_one();
        BOOST_CHECK(SIGTERM == siginfo.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);
    
//        cout << "running twice" << endl;
        ios.run_one();
        BOOST_CHECK(SIGINT == siginfo2.si_signo);
    }
}

void test_signal_handler_no_spurious_invocation()
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, siginfo_t const& siginfo, int* arrived)
        {
            cout << "arrived signal #" << siginfo.si_signo << endl;
            BOOST_CHECK(!error);
            *arrived = 1;
        }
        
        static void on_timeout(boost::system::error_code const& error, int* expired)
        {
           BOOST_CHECK(!error);
           *expired = 1;
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;

    sigset_t sigset, oldsigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    int const s = pthread_sigmask(SIG_BLOCK, &sigset, &oldsigset);
    BOOST_CHECK(0 == s);
    BOOST_SCOPE_EXIT((&oldsigset))
    {
        int const s1 = pthread_sigmask(SIG_SETMASK, &oldsigset, 0);
        BOOST_CHECK(0 == s1);
    }
    BOOST_SCOPE_EXIT_END

    boost::asio::io_service ios;
    unixsignal::signal_handler<SIGINT> sigint(ios);

    ios.reset();

    siginfo_t siginfo;
    std::memset(&siginfo, 0, sizeof siginfo);
    int arrived = 0;
    sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &arrived));

    int expired = 0;
    boost::asio::deadline_timer timer(ios);
    timer.expires_from_now(boost::posix_time::seconds(5));
    timer.async_wait(boost::bind(&local::on_timeout, _1, &expired));

    ios.run_one();
    BOOST_CHECK(expired);
    BOOST_CHECK(!arrived);
}

} //namespace

void init_test_signal_handler()
{
    cout << "using ";
#if defined(BOOST_ASIO_HAS_IOCP)
    cout << "iocp";
#elif defined(BOOST_ASIO_HAS_EPOLL)
    cout << "epoll";
#elif defined(BOOST_ASIO_HAS_KQUEUE)
    cout << "kqueue";
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
    cout << "devpoll";
#else
    cout << "select";
#endif
    cout << " multiplexing mechanism" << endl;

    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signal_handler));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler, 1)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler, 1000)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler2, 1)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler2, 1000)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signal_handler_no_spurious_invocation));
}

