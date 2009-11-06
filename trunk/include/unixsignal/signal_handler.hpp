// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_SIGNAL_HANDLER_HPP
#define UNIXSIGNAL_SIGNAL_HANDLER_HPP

#include <signal.h>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read.hpp>
#include <unixsignal/signalfd.hpp>

namespace unixsignal {

template <
    int S1, int S2 = 0, int S3 = 0, int S4 = 0, int S5 = 0,
    int S6 = 0, int S7 = 0, int S8 = 0, int S9 = 0, int S10 = 0,
    int S11 = 0, int S12 = 0, int S13 = 0, int S14 = 0, int S15 = 0,
    int S16 = 0, int S17 = 0, int S18 = 0, int S19 = 0, int S20 = 0,
    int S21 = 0, int S22 = 0, int S23 = 0, int S24 = 0, int S25 = 0,
    int S26 = 0, int S27 = 0, int S28 = 0, int S29 = 0, int S30 = 0>
class signal_handler
{
public:
    explicit signal_handler(boost::asio::io_service& ios)
        : m_sd(ios, m_sigfd.fd())
    {}

    template <typename Handler>
    void async_wait(Handler h)
    {
        boost::asio::async_read(m_sd, boost::asio::buffer(&m_buf, sizeof m_buf), boost::bind(h, _1, &m_buf));
    }

private:
    signalfd<
        S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
        S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
        S21, S22, S23, S24, S25, S26, S27, S28, S29, S30> m_sigfd;
    boost::asio::posix::stream_descriptor m_sd;
    siginfo_t m_buf;
};

} // namespace unixsignal

#endif

