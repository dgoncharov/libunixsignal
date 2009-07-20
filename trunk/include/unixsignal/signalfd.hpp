// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_SIGNALFD_HPP
#define UNIXSIGNAL_SIGNALFD_HPP

#include <signal.h>
#include <unistd.h>

#include <cerrno>
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
        sigaction(Signo, &m_oldact, 0);
        close(m_pipe[0]);
        close(m_pipe[1]);
    }

    //TODO: copy constructor and copy assignment?
    int fd() const
    {
        return m_pipe[0]; //TODO: dup()?
    }

private:
    static void on_signal(int signo)
    {
        try
        {
            char const c = 0;
            // The return value is ignored, because
            // if write() fails there is, probably, nothing useful that can be done.
            while (write(m_pipe[1], &c, sizeof c) < 0 && EINTR == errno);
        }
        catch (...) {}
    }
    static int m_pipe[2];

private:
    struct sigaction m_oldact;
};

template<int Signo>
int signalfd<Signo>::m_pipe[2];

}

#endif // UNIXSIGNAL_SIGNALFD_HPP

