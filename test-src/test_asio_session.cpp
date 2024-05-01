// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_session.hpp"
#include "asio_listener.hpp"

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

TEST(asio_session, fail_to_connect_to_non_existing_endpoint)
{
    boost::asio::io_context context;

    // add time to ensure the test finishes on failure
    boost::asio::deadline_timer timer(context);
    timer.expires_from_now(boost::posix_time::seconds(60));
    bool timeout = false;
    timer.async_wait([&](auto){
        timeout = true;
    });

    std::string const endpoint("/tmp/com_test.sock");
    unlink(endpoint.c_str());

    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    bool connect_called = false;
    bool session_okay = false;
    session->connect_to(endpoint, [&](auto session_){
        connect_called = true;
        session_okay = static_cast<bool>(session_);
    });

    while ((!timeout) && (!connect_called))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(connect_called);
    ASSERT_FALSE(session_okay);
}

TEST(asio_session, handle_exception_on_connect)
{
    boost::asio::io_context context;

    // add time to ensure the test finishes on failure
    boost::asio::deadline_timer timer(context);
    timer.expires_from_now(boost::posix_time::seconds(60));
    bool timeout = false;
    timer.async_wait([&](auto){
        timeout = true;
    });

    std::string const sock_name = "/tmp/com_test_ep.sock";
    unlink(sock_name.c_str());

    bool connect_called = false;

    oizys::asio_listener listener(context, sock_name, [&](auto){});

    listener.start();

    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    session->connect_to(sock_name, [&](auto) {
        connect_called = true;
        throw std::runtime_error("fail");
    });


    while ((!timeout) && (!connect_called))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(connect_called);
}
