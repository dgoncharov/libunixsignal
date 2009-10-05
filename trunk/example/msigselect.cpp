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
    unixsignal::multisignalfd<SIGINT> sigfd(SIGTERM, SIGQUIT, 0);
    unixsignal::multisignalfd<SIGHUP> sigfd2;
    unixsignal::multisignalfd<SIGUSR1> sigfd3(0);
    int const fd = sigfd.fd();
    int const fd2 = sigfd2.fd();
    int const fd3 = sigfd3.fd();

    cout << "Type to watch stdin activity. Send signals to watch the program react. Use ^D to exit" << endl << "# " << flush;
    while (std::cin)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(fd, &rfds);
        FD_SET(fd2, &rfds);
        FD_SET(fd3, &rfds);

        int const s = select(std::max(std::max(fd, fd2), fd3) + 1, &rfds, 0, 0, 0);
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
            cout << "activity on multisignalfd #" << fd;
            int signo;
            //TODO: read in a loop
            int const s = read(fd, &signo, sizeof signo);
            if (s < 0)
                cerr << "\nCannot read from fd: read(): " << strerror(errno) << endl;
            cout << ":fd signal #" << signo << " received" << endl;
        }
        else if (FD_ISSET(fd2, &rfds))
        {
            cout << "activity on multisignalfd #" << fd2;
            int signo;
            //TODO: read in a loop
            int const s = read(fd2, &signo, sizeof signo);
            if (s < 0)
                cerr << "\nCannot read from fd: read(): " << strerror(errno) << endl;
            cout << ":fd2 signal #" << signo << " received" << endl;
        }
        else if (FD_ISSET(fd3, &rfds))
        {
            cout << "activity on multisignalfd #" << fd3;
            int signo;
            //TODO: read in a loop
            int const s = read(fd3, &signo, sizeof signo);
            if (s < 0)
                cerr << "\nCannot read from fd: read(): " << strerror(errno) << endl;
            cout << ":fd3 signal #" << signo << " received" << endl;
        }
        cout << "# " << flush;
    }
    cout << "\nstdin closed. Bye" << endl;
}

