// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#include <sys/select.h>
#include <signal.h>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <string>

#include <unixsignal/signalfd.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;

int main(int, char const* [])
{
    unixsignal::signalfd<SIGINT, SIGTERM> sig;
    int const fd = sig.fd();

    cout << "Type to watch stdin activity. Send signals to watch the program react. Use ^D to exit" << endl << "# " << flush;
    while (std::cin)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(fd, &rfds);

        int const s = select(fd + 1, &rfds, 0, 0, 0);
        if (s < 0)
        {
            if (EINTR != errno)
                cerr << "select(): " << strerror(errno) << endl;
            continue;
        }
        if (FD_ISSET(0, &rfds))
        {
            std::string s;
            std::getline(cin, s);
            cout << "activity on stdin: " << s << endl;
        }
        else if (FD_ISSET(fd, &rfds))
        {
            cout << "received signal";
            siginfo_t siginfo;
            int const s = read(fd, &siginfo, sizeof siginfo);
            if (s < 0)
                cerr << "\nCannot read from fd: read(): " << strerror(errno) << endl;
            else if (static_cast<size_t>(s) < sizeof siginfo)
                cerr << "\nCannot read the whole siginfo_t struct. read " << s << " bytes" << endl;
            else
                cout << " #" << siginfo.si_signo << endl;
        }
        cout << "# " << flush;
    }
    cout << "\nstdin closed. Bye" << endl;
}

