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
        char c;
        while ((s = read(intfd, &c, sizeof c)) < 0 && EINTR == errno);
        BOOST_CHECK(1 == s);
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
        char c;
        while ((s = read(termfd, &c, sizeof c)) < 0 && EINTR == errno);
        BOOST_CHECK(1 == s);
    }
}

void test_signal_handler()
{
    struct local
    {
        static void on_signal(boost::system::error_code const& error, int signo, int* arrived)
        {
            std::cout << "arrived signal #" << signo << std::endl;
            if (!error)
                *arrived = 1;
            else
                std::cerr << error << std::endl;
        }
    };

    cout << BOOST_CURRENT_FUNCTION << endl;

    boost::asio::io_service ios;

    unixsignal::signal_handler<SIGINT> sigint(ios);
    unixsignal::signal_handler<SIGTERM> sigterm(ios);

    {
        ios.reset();
        int sigint_arrived = 0;
        int sigterm_arrived = 0;

        sigint.async_wait(boost::bind(&local::on_signal, _1, SIGINT, &sigint_arrived));
        sigterm.async_wait(boost::bind(&local::on_signal, _1, SIGTERM, &sigterm_arrived));

        raise(SIGINT);
        ios.poll();

        BOOST_CHECK(sigint_arrived);
        BOOST_CHECK(!sigterm_arrived);

        ios.reset();
        sigint_arrived = 0;
        sigint.async_wait(boost::bind(&local::on_signal, _1, SIGINT, &sigint_arrived));

        raise(SIGTERM);
        ios.poll();

        BOOST_CHECK(!sigint_arrived);
        BOOST_CHECK(sigterm_arrived);
    }
}

bool init_unit_test()
{
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signalfd));
    boost::unit_test::framework::master_test_suite().add(BOOST_TEST_CASE(test_signal_handler));

    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    int const result = boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
    return result;
}

