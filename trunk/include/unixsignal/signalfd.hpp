// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_SIGNALFD_HPP
#define UNIXSIGNAL_SIGNALFD_HPP

#include <signal.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace unixsignal {

template <int Signo>
class signalfd
{
public:
    signalfd()
    {
        int s = pipe(m_pipe);
        if (s < 0)
        {
            throw std::runtime_error(strerror(errno));
        }
        struct sigaction act;
        std::memset(&act, 0, sizeof act);
        std::memset(&m_oldact, 0, sizeof m_oldact);
        act.sa_handler = signalfd<Signo>::on_signal;

        s = sigaction(Signo, &act, &m_oldact);
        if (s < 0)
        {
            throw std::runtime_error(strerror(errno));
        }
    }

    ~signalfd()
    {
        // If this destructor invokes anything that can throw an exception
        // embrace it with a try-catch block.
        sigaction(Signo, &m_oldact, 0);
        close(m_pipe[0]);
        close(m_pipe[1]);
    }

    /// \brief The application should not close the file descriptor
    /// which this functions returns.
    int fd() const
    {
        return m_pipe[0];
    }

private:
    // Copy constructor and copy assignment are disabled
    // because the destructor closes the file descriptors.
    // The user can copy around the file descriptor which signalfd::fd() returns.
    signalfd(signalfd<Signo> const&);
    signalfd<Signo>& operator=(signalfd<Signo> const&);

private:
    static void on_signal(int)
    {
        // If this callback invokes anything that can throw an exception
        // embrace it with a try-catch block.
        char const c = 0;
        // The return value is ignored, because
        // if write() fails there is, probably, nothing useful that can be done.
        while (write(m_pipe[1], &c, sizeof c) < 0 && EINTR == errno);
    }
    static int m_pipe[2];

private:
    struct sigaction m_oldact;
};

template <int Signo>
int signalfd<Signo>::m_pipe[2];

}

#endif // UNIXSIGNAL_SIGNALFD_HPP

