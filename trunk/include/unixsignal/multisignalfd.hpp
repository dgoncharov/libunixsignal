// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_MULTISIGNALFD_HPP
#define UNIXSIGNAL_MULTISIGNALFD_HPP

#include <signal.h>
#include <unistd.h>

#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>
//#include <boost/preprocessor/slot/counter.hpp>


namespace unixsignal {


//template <int Unique = BOOST_PP_COUNTER>
class multisignalfd
{
public:
    explicit multisignalfd(int signo, ...)
    {
        int s = pipe(m_pipe);
        if (s < 0)
        {
            throw std::runtime_error(strerror(errno));
        }

        va_list ap;
        va_start(ap, signo);
        while (signo)
        {
            struct sigaction act, oldact;
            std::memset(&act, 0, sizeof act);
            std::memset(&oldact, 0, sizeof oldact);
            act.sa_handler = multisignalfd::on_signal;

            //TODO: error handling and ex safety
            s = sigaction(signo, &act, &oldact);
            m_oldacts.push_back(std::make_pair(signo, oldact));
            m_fds[signo] = m_pipe[1];
            signo = va_arg(ap, int);
        }
        va_end(ap);
    }

    ~multisignalfd()
    {
        try
        {
            for (actlist_t::const_iterator it = m_oldacts.begin(), last = m_oldacts.end(); it != last; ++it)
            {
                sigaction(it->first, &it->second, 0);
            }
        }
        catch (...) {}
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
    // The user can copy around the file descriptor which multisignalfd::fd() returns.
    multisignalfd(multisignalfd const&);
    multisignalfd& operator=(multisignalfd const&);

private:
    static void on_signal(int signo)
    {
        // If this callback invokes anything that can throw an exception
        // embrace it with a try-catch block.
        // The return value is ignored, because if write() fails,
        // there is, probably, nothing useful that can be done.
//TODO: write in a loop until success or failure, resume on EINTR
        int const s = write(m_fds[signo], &signo, sizeof signo);
        (void) s; // Suppress unused variable warning.
    }
    int m_pipe[2];
    static int m_fds[100];

private:
    typedef std::vector<std::pair<int, struct sigaction> > actlist_t;
    actlist_t m_oldacts;
};

int multisignalfd::m_fds[100];
// template <int Unique>
// int multisignalfd<Unique>::m_fds[100];
// #include BOOST_PP_UPDATE_COUNTER()
}

#endif // UNIXSIGNAL_MULTISIGNALFD_HPP

