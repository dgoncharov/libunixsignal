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

int main(int argc, char const* argv[])
{
    unixsignal::signalfd<SIGINT> sigint;
    int const intfd = sigint.fd();

    unixsignal::signalfd<SIGTERM> sigterm;
    int const termfd = sigterm.fd();

    cout << "Type to watch stdin activity. Send signals to watch the program react. Use ^D to exit" << endl << "# " << flush;
    while (std::cin)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(intfd, &rfds);
        FD_SET(termfd, &rfds);

        int const s = select(std::max(intfd, termfd) + 1, &rfds, 0, 0, 0);
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
        else if (FD_ISSET(intfd, &rfds))
        {
            cout << "sigint received" << endl;
            char c;
            int const s = read(intfd, &c, sizeof c);
            if (s < 0)
                cerr << "Cannot read from intfd: read(): " << strerror(errno) << endl;
        }
        else if (FD_ISSET(termfd, &rfds))
        {
            cout << "sigterm received" << endl;
            char c;
            int const s = read(termfd, &c, sizeof c);
            if (s < 0)
                cerr << "Cannot read from termfd: read(): " << strerror(errno) << endl;
        }
        cout << "# " << flush;
    }
    cout << "\nstdin closed. Bye" << endl;
}

