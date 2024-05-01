// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_ASIO_LISTENER_HPP
#define OIZYS_ASIO_LISTENER_HPP

#include "oizys/listener_i.hpp"
#include "oizys/context_i.hpp" // accept_handler
#include <boost/asio.hpp>

namespace oizys
{

class asio_listener: public listener_i
{
public:
    asio_listener(boost::asio::io_context & context, std::string const & endpoint,
        accept_handler handler);
    virtual ~asio_listener() override = default;
    virtual void start() override;
    virtual void stop() override;
private:
    void do_accept();

    boost::asio::local::stream_protocol::acceptor acceptor;
    accept_handler on_accept;
    bool is_listening;
};

}

#endif