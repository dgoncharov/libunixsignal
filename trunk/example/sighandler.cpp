// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#include <iostream>
#include <iomanip>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <unixsignal/signal_handler.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;

namespace ba = boost::asio;
namespace bap = boost::asio::posix;

void on_signal(boost::system::error_code const& error, int signo, int* invoked)
{
    if (!error)
        cout << "signal #" << signo << " received" << endl << "# " << flush;
    *invoked = 1;
}

void on_stdin(boost::system::error_code const& error, char const* buf, std::size_t buflen, int* running)
{
    if (!error)
    {
        assert(buflen >= 1 && "Incorrect usage of boost::asio");
        cout << "activity on stdin: " << std::string(buf, buflen - 1) << endl << "# " << flush;
    }
    else
    {
        if (ba::error::eof == error)
        {
            cout << "stdin closed." << flush;
            *running = 0;
        }
	else
            cout << "stdin error: " << error << endl;
   }
}

int main(int, char const* [])
{
    ba::io_service ios;

    unixsignal::signal_handler<SIGINT> sigint(ios);
    unixsignal::signal_handler<SIGTERM> sigterm(ios);

    bap::stream_descriptor std_in(ios, STDIN_FILENO);

    cout << "Type to watch stdin activity. Send signals to watch the program react. Use ^D to exit" << endl << "# " << flush;

    int running = 1;
    int sigint_restart_wait = 1;
    int sigterm_restart_wait = 1;
    while (running)
    {
        ios.reset();

        if (sigint_restart_wait)
        {
            sigint_restart_wait = 0;
            sigint.async_wait(boost::bind(on_signal, _1, SIGINT, &sigint_restart_wait));
        }
        if (sigterm_restart_wait)
        {
            sigterm_restart_wait = 0;
            sigterm.async_wait(boost::bind(on_signal, _1, SIGTERM, &sigterm_restart_wait));
        }

        char buf[1024];
        std_in.async_read_some(ba::buffer(buf, sizeof buf), boost::bind(on_stdin, _1, buf, _2, &running));

        ios.poll();
    }
    cout << " Bye" << endl;
}

