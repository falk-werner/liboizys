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

    session->set_onclose([](auto){});
    ASSERT_ANY_THROW({
        session->set_onclose([](auto){});
    });
}

TEST(asio_session, throw_onclose_for_closed_session)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    session->close();
    session->set_onclose([](auto){
        throw std::runtime_error("fail");
    });
}

TEST(asio_session, fail_to_set_message_handler_twice)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    session->set_onmessage([](auto){});
    ASSERT_ANY_THROW({
        session->set_onmessage([](auto){});
    });
}

TEST(asio_session, destruct_closed_socket_without_registered_onmessage_handler)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(sock));

    session->set_onclose([](auto){});
}

TEST(asio_session, destruct_closed_socket_without_registered_onclose_handler)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(sock));
}
