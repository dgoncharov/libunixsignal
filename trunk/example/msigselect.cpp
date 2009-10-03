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

#include <unixsignal/multisignalfd.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;

int main(int, char const* [])
{
    unixsignal::multisignalfd sigfd(SIGINT, SIGTERM, 0);
    unixsignal::multisignalfd sigfd2(SIGQUIT, 0);
    int const fd = sigfd.fd();
    int const fd2 = sigfd2.fd();

    cout << "Type to watch stdin activity. Send signals to watch the program react. Use ^D to exit" << endl << "# " << flush;
    while (std::cin)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(fd, &rfds);
        FD_SET(fd2, &rfds);

        int const s = select(std::max(fd, fd2) + 1, &rfds, 0, 0, 0);
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
            cout << "activity on multisignalfd";
            int signo;
            //TODO: read in a loop
            int const s = read(fd, &signo, sizeof signo);
            if (s < 0)
                cerr << "\nCannot read from fd: read(): " << strerror(errno) << endl;
            cout << ":fd signal #" << signo << " received" << endl;
        }
        else if (FD_ISSET(fd2, &rfds))
        {
            cout << "activity on multisignalfd";
            int signo;
            //TODO: read in a loop
            int const s = read(fd2, &signo, sizeof signo);
            if (s < 0)
                cerr << "\nCannot read from fd: read(): " << strerror(errno) << endl;
            cout << ":fd2 signal #" << signo << " received" << endl;
        }
        cout << "# " << flush;
    }
    cout << "\nstdin closed. Bye" << endl;
}

