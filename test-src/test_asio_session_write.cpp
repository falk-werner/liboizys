// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_session.hpp"

#include <boost/asio.hpp>
#include <gtest/gtest.h>

#include <memory>

TEST(asio_session, send_message)
{
    boost::asio::io_context context;

    // add time to ensure the test finishes on failure
    boost::asio::deadline_timer timer(context);
    timer.expires_from_now(boost::posix_time::seconds(60));
    bool timeout = false;
    timer.async_wait([&](auto){
        timeout = true;
    });

    boost::asio::local::stream_protocol::socket read_sock(context);
    boost::asio::local::stream_protocol::socket write_sock(context);

    boost::asio::local::connect_pair(read_sock, write_sock);
    
    auto session = std::make_shared<oizys::asio_session>(std::move(write_sock));
    
    bool closed = false;
    session->set_onclose([&](){
        closed = true;
    });
    session->set_onmessage([](auto){});

    session->send("Hi");

    bool send_okay = false;
    std::string message(6, ' ');
    boost::asio::async_read(read_sock, boost::asio::buffer(message, message.size()),
        [&](auto err, auto) {
            send_okay = !err;
            session->close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_TRUE(send_okay);
    ASSERT_EQ(std::string("\0\0\0\2Hi",6), message);
}

TEST(asio_session, send_close_on_incomplete_header)
{
    boost::asio::io_context context;

    // add time to ensure the test finishes on failure
    boost::asio::deadline_timer timer(context);
    timer.expires_from_now(boost::posix_time::seconds(60));
    bool timeout = false;
    timer.async_wait([&](auto){
        timeout = true;
    });

    boost::asio::local::stream_protocol::socket read_sock(context);
    boost::asio::local::stream_protocol::socket write_sock(context);

    boost::asio::local::connect_pair(read_sock, write_sock);
    
    auto session = std::make_shared<oizys::asio_session>(std::move(write_sock));
    
    bool closed = false;
    session->set_onclose([&](){
        closed = true;
    });
    session->set_onmessage([](auto){});

    session->send("Hi");

    bool send_okay = false;
    std::string message(2, ' ');
    boost::asio::async_read(read_sock, boost::asio::buffer(message, message.size()),
        [&](auto err, auto) {
            send_okay = !err;
            read_sock.close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_TRUE(send_okay);
    ASSERT_EQ(std::string("\0\0",2), message);
}
