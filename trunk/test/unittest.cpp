#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <cerrno>
#include <iostream>
#include <boost/current_function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <unixsignal/signalfd.hpp>
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
        static void on_signal(boost::system::error_code const& error, siginfo_t const* siginfo, siginfo_t* arrived)
        {
            std::cout << "arrived signal #" << siginfo->si_signo << std::endl;
            BOOST_CHECK(!error);
            *arrived = *siginfo;
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

        raise(SIGINT);
        ios.poll();

        BOOST_CHECK(SIGINT == siginfo1.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);

        ios.reset();
        std::memset(&siginfo1, 0, sizeof siginfo1);
        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));

        raise(SIGTERM);
        ios.poll();

        BOOST_CHECK(0 == siginfo1.si_signo);
        BOOST_CHECK(SIGTERM == siginfo2.si_signo);

        ios.reset();
        std::memset(&siginfo2, 0, sizeof siginfo2);
        sigint.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo1));
        raise(SIGINT);
        ios.poll();

        BOOST_CHECK(SIGINT == siginfo1.si_signo);
        BOOST_CHECK(0 == siginfo2.si_signo);
    }
}

void test_multisignal_handler()
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, siginfo_t const* siginfo, siginfo_t* arrived)
        {
            std::cout << "arrived signal #" << siginfo->si_signo << std::endl;
            BOOST_CHECK(!error);
            *arrived = *siginfo;
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;

    boost::asio::io_service ios;

    unixsignal::signal_handler<SIGINT, SIGTERM> sig(ios);

    {
        ios.reset();
        siginfo_t siginfo;
        std::memset(&siginfo, 0, sizeof siginfo);

        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));

        cout << "raising sigint" << endl;
        raise(SIGINT);
        ios.poll_one();

        BOOST_CHECK(SIGINT == siginfo.si_signo);

        ios.reset();
        sig.async_wait(boost::bind(&local::on_signal, _1, _2, &siginfo));

        cout << "raising sigterm" << endl;
        raise(SIGTERM);
        ios.run_one();
        cout << "poll() returned" << endl;
        BOOST_CHECK(SIGTERM == siginfo.si_signo);
    }
}

bool init_unit_test()
{
//     boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signalfd));
//     boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_multisignalfd));
//     boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signal_handler));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_multisignal_handler));

    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    int const result = boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
    return result;
}

