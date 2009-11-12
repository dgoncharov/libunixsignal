// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#ifndef UNIXSIGNAL_SIGNAL_HANDLER_HPP
#define UNIXSIGNAL_SIGNAL_HANDLER_HPP

#include <signal.h> // For siginfo_t.
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/basic_stream_descriptor.hpp>
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
private:
    typedef signal_handler<
        S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
        S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
        S21, S22, S23, S24, S25, S26, S27, S28, S29, S30> this_type;

    typedef boost::function<void (boost::system::error_code const&, siginfo_t const&)> cb_t;

public:
    explicit signal_handler(boost::asio::io_service& ios)
        : m_sd(ios, m_sigfd.fd())
    {}

    template <typename Handler>
    void async_wait(Handler h)
    {
        cb_t f(h);
        m_sd.async_read_some(boost::asio::null_buffers(), boost::bind(&this_type::on_sig, this, _1, f));
    }

private:
    void on_sig(boost::system::error_code const& error, cb_t const& f)
    {
        if (error)
        {
            f(error, siginfo_t());
            return;
        }
        boost::asio::posix::descriptor_base::bytes_readable cmd;
        m_sd.io_control(cmd);
        std::size_t const nbytes = cmd.get();
        if (nbytes >= sizeof(siginfo_t))
        {
            siginfo_t siginfo;
            std::size_t const n = boost::asio::read(m_sd, boost::asio::buffer(&siginfo, sizeof siginfo));
            if (sizeof siginfo == n)
                f(error, siginfo);
        }
    }

private:
    signalfd<
        S1, S2, S3, S4, S5, S6, S7, S8, S9, S10,
        S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
        S21, S22, S23, S24, S25, S26, S27, S28, S29, S30> m_sigfd;
    boost::asio::posix::stream_descriptor m_sd;
};

} // namespace unixsignal

#endif

