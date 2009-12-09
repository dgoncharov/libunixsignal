// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#include <unixsignal/signal_handler.hpp>
#include <unistd.h>
#include <cstddef>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;

namespace ba = boost::asio;
namespace bap = boost::asio::posix;
namespace bs = boost::system;

void on_signal(bs::error_code const& err, siginfo_t const& siginfo, int* invoked)
{
    if (!err)
        cout
            << "signal #" << siginfo.si_signo << " received"
            << endl << "# " << flush;
    *invoked = 1;
}

void on_stdin(
    bs::error_code const& err, char const* buf, std::size_t buflen, int* running)
{
    if (!err)
    {
        assert(buflen >= 1 && "Incorrect usage of boost::asio");
        cout 
            << "activity on stdin: " << std::string(buf, buflen - 1)
            << endl << "# " << flush;
    }
    else
    {
        if (ba::error::eof == err)
        {
            cout << "stdin closed." << flush;
            *running = 0;
        }
        else
            cout << "stdin error: " << err << endl;
    }
}

int main(int, char const* [])
{
    ba::io_service ios;

    unixsignal::signal_handler<SIGINT, SIGTERM> sigint(ios);
    unixsignal::signal_handler<SIGHUP, SIGUSR1, SIGUSR2> sighup(ios);

    bap::stream_descriptor std_in(ios, STDIN_FILENO);

    cout << "Type to watch stdin activity\n"
         << "Send the following signals to watch the program react #"
         << SIGINT << ", #" << SIGTERM << ", #" << SIGHUP
         << ", #" << SIGUSR1 << ", #" << SIGUSR2
         << "\nUse ^D to exit"
         << endl << "# " << flush;

    int running = 1;
    int intf = 1;
    int hupf = 1;
    while (running)
    {
        ios.reset();

        if (intf)
        {
            intf = 0;
            sigint.async_wait(boost::bind(on_signal, _1, _2, &intf));
        }
        if (hupf)
        {
            hupf = 0;
            sighup.async_wait(boost::bind(on_signal, _1, _2, &hupf));
        }

        char buf[1024];
        std_in.async_read_some(
            ba::buffer(buf, sizeof buf),
            boost::bind(on_stdin, _1, buf, _2, &running));

        ios.run_one();
    }
    cout << " Bye" << endl;
}

