// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_session.hpp"

#include <boost/asio.hpp>
#include <gtest/gtest.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <vector>

TEST(asio_session, receive_message)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));

    bool closed = false;
    session->set_on_close([&](){
        closed = true;
    });

    std::string message;
    session->set_on_message([&](std::string const & msg){
        message = msg;
    });    

    std::string const data("\0\0\0\x02Hi", 6);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [&](auto, auto){
            write_sock.close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_EQ("Hi", message);
}

TEST(asio_session, throw_on_message)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    
    bool closed = false;
    session->set_on_close([&](){
        closed = true;
    });

    std::string message;
    session->set_on_message([&](std::string const & msg){
        message = msg;
        throw std::runtime_error("fail");
    });

    std::string const data("\0\0\0\x02Hi", 6);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [&](auto, auto){
            write_sock.close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_EQ("Hi", message);
}

TEST(asio_session, receive_multiple_messages)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    
    bool closed = false;
    session->set_on_close([&](){
        closed = true;
    });

    std::vector<std::string> messages;
    session->set_on_message([&](std::string const & message){
        messages.push_back(message);
    });

    std::string const data("\0\0\0\x05Hello\0\0\0\x02, \0\0\0\x05World\0\0\0\x01!", 29);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [&](auto, auto){
            write_sock.close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_EQ(4, messages.size());
    ASSERT_EQ("Hello", messages[0]);
    ASSERT_EQ(", ", messages[1]);
    ASSERT_EQ("World", messages[2]);
    ASSERT_EQ("!", messages[3]);
}

TEST(asio_session, close_on_empty_message)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    
    bool closed = false;
    session->set_on_close([&](){
        closed = true;
    });

    bool message_received = false;
    session->set_on_message([&](auto){
        message_received = true;
    });

    std::string const data("\0\0\0\0", 4);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [](auto, auto){});

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_FALSE(message_received);
}

TEST(asio_session, throw_on_close)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    
    bool closed = false;
    session->set_on_close([&](){
        closed = true;
        throw std::runtime_error("fail");
    });

    bool message_received = false;
    session->set_on_message([&](auto){
        message_received = true;
    });

    std::string const data("\0\0\0\0", 4);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [](auto, auto){});

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_FALSE(message_received);
}

TEST(asio_session, close_on_incomplete_header)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    
    bool closed = false;
    session->set_on_close([&](){
        closed = true;
    });

    bool message_received = false;
    session->set_on_message([&](auto){
        message_received = true;
    });

    std::string const data("\0\0", 2);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [&](auto, auto){
            write_sock.close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_FALSE(message_received);
}

TEST(asio_session, close_on_incomplete_payload)
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
    
    auto session = std::make_shared<oizys::asio_session>(std::move(read_sock));
    
    bool closed = false;
    session->set_on_close([&](){
        closed = true;
    });

    bool message_received = false;
    session->set_on_message([&](auto){
        message_received = true;
    });

    std::string const data("\0\0\0\x05Hello", 6);
    boost::asio::async_write(write_sock, boost::asio::buffer(data, data.size()), 
        [&](auto, auto){
            write_sock.close();
    });

    while ((!timeout) && (!closed))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(closed);
    ASSERT_FALSE(message_received);
}