// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_SIGNAL_HANDLER_HPP
#define UNIXSIGNAL_SIGNAL_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read.hpp>
#include <unixsignal/signalfd.hpp>

namespace unixsignal {

template <int Signo>
class signal_handler
{
public:
    explicit signal_handler(boost::asio::io_service& ios)
        : m_sd(ios, m_sigfd.fd())
    {}

    template <typename Handler>
    void async_wait(Handler h)
    {
        boost::asio::async_read(m_sd, boost::asio::buffer(&m_buf, sizeof m_buf), h);
    }

private:
    signalfd<Signo> m_sigfd;
    boost::asio::posix::stream_descriptor m_sd;
    char m_buf;
};

} // namespace unixsignal

#endif

