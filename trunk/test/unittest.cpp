#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <cerrno>
#include <iostream>
#include <boost/current_function.hpp>
#include <boost/bind.hpp>
//#define BOOST_ASIO_DISABLE_EPOLL
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <unixsignal/signalfd.hpp> //TODO: split to two files and #include this headers first to make sure they are self contained
#include <unixsignal/signal_handler.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace
{
using namespace std;

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
        cout << "raising SIGINT" << endl;
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
		cout << "raising SIGTERM" << endl;
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

void test_signal_handler()
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, siginfo_t const& siginfo, siginfo_t* arrived)
        {
            std::cout << "arrived signal #" << siginfo.si_signo << std::endl;
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

        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        ios.run_one();

        BOOST_CHECK(SIGINT == siginfo1.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);

        ios.reset();
        std::memset(&siginfo1, 0, sizeof siginfo1);
        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));

        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
        ios.run_one();

        BOOST_CHECK(0 == siginfo1.si_signo);
        BOOST_CHECK(SIGTERM == siginfo2.si_signo);

        ios.reset();
        std::memset(&siginfo2, 0, sizeof siginfo2);
        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));

        cout << "raising SIGINT" << endl;
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
            std::cout << "arrived signal #" << siginfo.si_signo << std::endl;
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

        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        ios.run_one();
        BOOST_CHECK(SIGINT == siginfo.si_signo);

        ios.reset();
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));

        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
        ios.run_one();
        BOOST_CHECK(SIGTERM == siginfo.si_signo);

        ios.reset();
        std::memset(&siginfo, 0, sizeof siginfo);
        std::memset(&siginfo2, 0, sizeof siginfo2);
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo2));

        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
        cout << "raising SIGINT" << endl;
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
            std::cout << "arrived signal #" << siginfo.si_signo << std::endl;
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
        cout << "raising SIGTERM" << endl;
        raise(SIGTERM);
        cout << "raising SIGINT" << endl;
        raise(SIGINT);
        std::memset(&siginfo, 0, sizeof siginfo);
        std::memset(&siginfo2, 0, sizeof siginfo2);
    
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo2));
    
    
        BOOST_CHECK(0 == siginfo.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);
    
        cout << "running once" << endl;
        ios.run_one();
        BOOST_CHECK(SIGTERM == siginfo.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);
    
        cout << "running twice" << endl;
        ios.run_one();
        BOOST_CHECK(SIGINT == siginfo2.si_signo);
    }
}

/*
void test_stream_descriptor()
{
    struct local
    {
        static void on_read(boost::system::error_code const& error)
        {
            BOOST_CHECK(!error);
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;
#if defined (BOOST_ASIO_HAS_EPOLL)
    std::cout << "epoll" << std::endl;
#else
    std::cout << "select" << std::endl;
#endif

    int p[2];
    int const s = ::pipe(p);
    BOOST_CHECK(0 == s);
    
    char c2 = 0;
    {
        char const c = 7;
        int const s2 = ::write(p[1], &c, sizeof c);
        BOOST_CHECK(sizeof c == s2);
    }
    {
        char const c = 9;
        int const s3 = ::write(p[1], &c, sizeof c);
        BOOST_CHECK(sizeof c == s3);
    }
    
    boost::asio::io_service ios;
    ios.reset();

    BOOST_CHECK(0 == c2);
    boost::asio::posix::stream_descriptor sd1(ios, p[0]);
    boost::asio::async_read(sd1, boost::asio::buffer(&c2, sizeof c2), boost::bind(&local::on_read, _1));
//    boost::asio::async_read(sd1, boost::asio::buffer(&c2, sizeof c2), boost::bind(&local::on_read, _1));

    cout << "c2: " << int(c2) << endl;
    BOOST_CHECK(0 == c2);

//     ios.run_one();
//     BOOST_CHECK(7 == c2);
//
//     ios.run_one();
//     BOOST_CHECK(9 == c2);
}
*/
bool init_unit_test()
{
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signalfd));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_multisignalfd));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signal_handler));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler, 1)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler, 1000)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler2, 1)));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(test_multisignal_handler2, 1000)));
    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    int const result = boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
    return result;
}

