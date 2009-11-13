// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_SIGNALFD_HPP
#define UNIXSIGNAL_SIGNALFD_HPP

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

#include <cerrno>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <boost/static_assert.hpp>
#include <boost/system/system_error.hpp>

namespace unixsignal {


template <
    int S1, int S2 = 0, int S3 = 0, int S4 = 0, int S5 = 0,
    int S6 = 0, int S7 = 0, int S8 = 0, int S9 = 0, int S10 = 0,
    int S11 = 0, int S12 = 0, int S13 = 0, int S14 = 0, int S15 = 0,
    int S16 = 0, int S17 = 0, int S18 = 0, int S19 = 0, int S20 = 0,
    int S21 = 0, int S22 = 0, int S23 = 0, int S24 = 0, int S25 = 0,
    int S26 = 0, int S27 = 0, int S28 = 0, int S29 = 0, int S30 = 0>
class signalfd
{
private:
    typedef signalfd<
        S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
        S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
        S21, S22, S23, S24, S25, S26, S27, S28, S29, S30> this_type;

public:
    signalfd()
    {
        int p[2];
        int const s = pipe(p);
        if (s < 0)
            throw boost::system::system_error(
                boost::system::error_code(errno, boost::system::get_system_category()));
        m_r.fd = p[0];
        m_w.fd = p[1];
        m_wfd = m_w.fd;

        int const f = fcntl(m_w.fd, F_GETFL, 0);
        if (f < 0)
            throw boost::system::system_error(
                boost::system::error_code(errno, boost::system::get_system_category()));
        int const s1 = fcntl(m_w.fd, F_SETFL, f | O_NONBLOCK);
        if (s1 < 0)
            throw boost::system::system_error(
                boost::system::error_code(errno, boost::system::get_system_category()));

        struct sigaction act;
        std::memset(&act, 0, sizeof act);
        std::memset(&m_oldact, 0, sizeof m_oldact);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = this_type::on_signal;

        int const signals[] = {
            S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
            S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
            S21, S22, S23, S24, S25, S26, S27, S28, S29, S30};
        BOOST_STATIC_ASSERT((nsignals <= sizeof signals / sizeof signals[0]));
        for (std::size_t i = 0; i < nsignals; ++i)
        {
            assert(signals[i] > 0);
            int const s2 = sigaction(signals[i], &act, &m_oldact[i]);
            if (s2 < 0)
            {
                int const errcode = errno;
                // Ignore return values.
                // The "goes down to" operator.
                while (i --> 0)
                    sigaction(signals[i], &m_oldact[i], 0);
                throw boost::system::system_error(
                    boost::system::error_code(errcode, boost::system::get_system_category()));
            }
        }
    }

    ~signalfd()
    {
        // If this destructor invokes anything that can throw an exception
        // embrace it with a try-catch block.
        // Ignore return values.
        int const signals[] = {
            S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
            S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
            S21, S22, S23, S24, S25, S26, S27, S28, S29, S30};
        BOOST_STATIC_ASSERT((nsignals <= sizeof signals / sizeof signals[0]));
        for (std::size_t i = 0; i < nsignals; ++i)
        {
            assert(signals[i] > 0);
            sigaction(signals[i], &m_oldact[i], 0);
        }
    }

    /// \brief The user should not close the file descriptor
    /// which this function returns.
    int fd() const
    {
        return m_r.fd;
    }

private:
    // Copy constructor and copy assignment are disabled
    // because the destructor closes the file descriptors.
    // The user can copy around the file descriptor which signalfd::fd() returns.
    signalfd(this_type const&);
    this_type& operator=(this_type const&);

private:
    struct fdcloser
    {
        ~fdcloser()
        {
            close(fd);
        }
        int fd;
    };

    static void on_signal(int, siginfo_t* siginfo, void*)
    {
        // If this callback invokes anything that can throw an exception
        // embrace it with a try-catch block.
        // The return value is ignored, because
        // if write() fails there is, probably, nothing useful that can be done.
        BOOST_STATIC_ASSERT((PIPE_BUF >= sizeof(siginfo_t)));
        int s = write(m_wfd, siginfo, sizeof *siginfo);
        s = s;
    }
    fdcloser m_r;
    fdcloser m_w;
    static int m_wfd;

private:
    // 4.5 Integral promotions.
    // An rvalue of type bool can be converted to an rvalue of type int, with false becoming zero and true becoming one.
    // Plus has higher precedence than greater.
    static std::size_t const nsignals =
        (S1 > 0) + (S2 > 0) + (S3 > 0) + (S4 > 0) + (S5 > 0) +
        (S6 > 0) + (S7 > 0) + (S8 > 0) + (S9 > 0) + (S10 > 0) +
        (S11 > 0) + (S12 > 0) + (S13 > 0) + (S14 > 0) + (S15 > 0) +
        (S16 > 0) + (S17 > 0) + (S18 > 0) + (S19 > 0) + (S20 > 0) +
        (S21 > 0) + (S22 > 0) + (S23 > 0) + (S24 > 0) + (S25 > 0) +
        (S26 > 0) + (S27 > 0) + (S28 > 0) + (S29 > 0) + (S30 > 0);
    struct sigaction m_oldact[nsignals];
};

template <
    int S1, int S2, int S3, int S4, int S5,
    int S6, int S7, int S8, int S9, int S10,
    int S11, int S12, int S13, int S14, int S15,
    int S16, int S17, int S18, int S19, int S20,
    int S21, int S22, int S23, int S24, int S25,
    int S26, int S27, int S28, int S29, int S30>
int signalfd<
        S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
        S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
        S21, S22, S23, S24, S25, S26, S27, S28, S29, S30>::m_wfd;
} // namespace unixsignal

#endif // UNIXSIGNAL_SIGNALFD_HPP

