// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_session.hpp"

#include <boost/asio.hpp>
#include <gtest/gtest.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <stdexcept>

TEST(asio_session, fail_to_set_close_handler_twice)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    session->set_on_close([](){});
    ASSERT_ANY_THROW({
        session->set_on_close([](){});
    });
}

TEST(asio_session, throw_on_close_for_closed_session)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    session->close();
    session->set_on_close([](){
        throw std::runtime_error("fail");
    });
}

TEST(asio_session, fail_to_set_message_handler_twice)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    session->set_on_message([](auto){});
    ASSERT_ANY_THROW({
        session->set_on_message([](auto){});
    });
}

TEST(asio_session, destruct_closed_socket_without_registered_on_message_handler)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(sock));

    session->set_on_close([](){});
}

TEST(asio_session, destruct_closed_socket_without_registered_on_close_handler)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(sock));
}
