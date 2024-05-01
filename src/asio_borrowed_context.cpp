// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_borrowed_context.hpp"
#include "asio_listener.hpp"
#include "asio_session.hpp"

#include <boost/asio.hpp>

#include <stdexcept>

namespace oizys
{

asio_borrowed_context::asio_borrowed_context(boost::asio::io_context& context)
: context_(context)
{
}

void asio_borrowed_context::run()
{
    context_.run_one();
}

std::unique_ptr<listener_i> asio_borrowed_context::new_listener(std::string const & endpoint, accept_handler handler)
{
    auto * listener = new asio_listener(context_, endpoint, std::move(handler));
    return std::unique_ptr<listener_i>(listener);
}

void asio_borrowed_context::connect_to(std::string const & endpoint, accept_handler handler)
{
    boost::asio::local::stream_protocol::socket sock(context_);
    auto session = std::make_shared<asio_session>(std::move(sock));

    session->connect_to(endpoint, std::move(handler));
}


}