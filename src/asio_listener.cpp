// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_listener.hpp"
#include "asio_session.hpp"

namespace oizys
{

asio_listener::asio_listener(boost::asio::io_context & context, std::string const & endpoint,
    accept_handler handler)
: acceptor(context, boost::asio::local::stream_protocol::endpoint(endpoint))
, on_accept(std::move(handler))
, is_listening(false)
{

}

void asio_listener::start()
{
    if (!is_listening)
    {
        is_listening = true;
        do_accept();
    }
}

void asio_listener::stop()
{
    is_listening = false;
}

void asio_listener::do_accept()
{
    acceptor.async_accept([this](auto err, auto sock) {
        if ((err) || (!is_listening))
        { 
            return; 
        }

        auto session = std::make_shared<asio_session>(std::move(sock));
        session->start();
        try
        {
            on_accept(session);
        }
        catch (...)
        {
            // swallow
        }

        if (is_listening)
        {
            do_accept();
        }
    });
}


}