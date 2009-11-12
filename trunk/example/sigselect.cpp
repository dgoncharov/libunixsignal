// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#include <unixsignal/signalfd.hpp>
#include <sys/select.h>
#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;

void readsig(int fd)
{
    cout << "received signal";
    siginfo_t siginfo;
    ssize_t const s = read(fd, &siginfo, sizeof siginfo);
    if (s < 0)
        cerr << "Cannot read: read(): " << std::strerror(errno) << endl;
    else if (static_cast<std::size_t>(s) < sizeof siginfo)
        cerr << "\nCannot read the whole siginfo_t struct. read " << s << " bytes" << endl;
    else
        cout << " #" << siginfo.si_signo << endl;
}

int main(int, char const* [])
{
    unixsignal::signalfd<SIGINT, SIGTERM> sigint;
    int const intfd = sigint.fd();

    unixsignal::signalfd<SIGHUP, SIGUSR1, SIGUSR2> sighup;
    int const hupfd = sighup.fd();

    cout << "Type to watch stdin activity\n"
         << "Send the following signals to watch the program react #"
         << SIGINT << ", #" << SIGTERM << ", #" << SIGHUP << ", #" << SIGUSR1 << ", #" << SIGUSR2
         << "\nUse ^D to exit"
         << endl << "# " << flush;
    while (std::cin)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(intfd, &rfds);
        FD_SET(hupfd, &rfds);

        int const s = select(std::max(intfd, hupfd) + 1, &rfds, 0, 0, 0);
        if (s < 0)
        {
            if (EINTR != errno)
                cerr << "select(): " << std::strerror(errno) << endl;
            continue;
        }
        if (FD_ISSET(0, &rfds))
        {
            std::string s;
            std::getline(cin, s);
            cout << "activity on stdin: " << s << endl;
        }
        else if (FD_ISSET(intfd, &rfds))
            readsig(intfd);
        else if (FD_ISSET(hupfd, &rfds))
            readsig(hupfd);
        cout << "# " << flush;
    }
    cout << "\nstdin closed. Bye" << endl;
}

