#include "asio_listener.hpp"
#include <gtest/gtest.h>
#include <unistd.h>

TEST(asio_listener, construct)
{
    boost::asio::io_context context;
    std::string const sock_name = "/tmp/com_test_ep.sock";
    unlink(sock_name.c_str());

    com::asio_listener listener(context, sock_name, [&](auto){});
}

TEST(asio_listener, accept)
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

    bool connected = false;
    bool connect_called = false;
    bool accept_called = false;

    com::asio_listener listener(context, sock_name, [&](auto){
        accept_called = true;
    });

    listener.start();

    boost::asio::local::stream_protocol::socket sock(context);
    boost::asio::local::stream_protocol::endpoint ep(sock_name);
    sock.async_connect(ep, [&](auto err) {
        connected = !err;
        connect_called = true;
    });

    while ((!timeout) && (!(connect_called && accept_called)))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(connected);
    ASSERT_TRUE(accept_called);
}

TEST(asio_listener, accept_multiple)
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

    int accept_count = 0;
    com::asio_listener listener(context, sock_name, [&](auto){
        accept_count++;
    });

    listener.start();

    int connect_count = 0;
    bool connect_err = false;

    boost::asio::local::stream_protocol::endpoint ep(sock_name);

    boost::asio::local::stream_protocol::socket sock1(context);
    sock1.async_connect(ep, [&](auto err) {
        if (err) { connect_err = true; }
        connect_count = 3;
    });

    boost::asio::local::stream_protocol::socket sock2(context);
    sock2.async_connect(ep, [&](auto err) {
        if (err) { connect_err = true; }
        connect_count = 3;
    });

    boost::asio::local::stream_protocol::socket sock3(context);
    sock3.async_connect(ep, [&](auto err) {
        if (err) { connect_err = true; }
        connect_count = 3;
    });

    while ((!timeout) && ((connect_count != 3) || (accept_count != 3)))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_FALSE(connect_err);
    ASSERT_EQ(3, accept_count);
    ASSERT_EQ(3, connect_count);
}

TEST(asio_listener, accept_double_start)
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

    bool connected = false;
    bool connect_called = false;
    bool accept_called = false;

    com::asio_listener listener(context, sock_name, [&](auto){
        accept_called = true;
    });

    listener.start();
    listener.start();

    boost::asio::local::stream_protocol::socket sock(context);
    boost::asio::local::stream_protocol::endpoint ep(sock_name);
    sock.async_connect(ep, [&](auto err) {
        connected = !err;
        connect_called = true;
    });

    while ((!timeout) && (!(connect_called && accept_called)))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(connected);
    ASSERT_TRUE(accept_called);
}

TEST(asio_listener, stop_listening)
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

    bool connected = false;
    bool connect_called = false;
    bool accept_called = false;

    com::asio_listener listener(context, sock_name, [&](auto){
        accept_called = true;
        listener.stop();
    });

    listener.start();

    boost::asio::local::stream_protocol::socket sock(context);
    boost::asio::local::stream_protocol::endpoint ep(sock_name);
    sock.async_connect(ep, [&](auto err) {
        connected = !err;
        connect_called = true;
    });

    while ((!timeout) && (!(connect_called && accept_called)))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(connected);
    ASSERT_TRUE(accept_called);
}

TEST(asio_listener, stop_listening_immediatly)
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

    bool connected = false;
    bool connect_called = false;
    bool accept_called = false;

    com::asio_listener listener(context, sock_name, [&](auto){
        accept_called = true;
    });

    listener.start();

    boost::asio::local::stream_protocol::socket sock(context);
    boost::asio::local::stream_protocol::endpoint ep(sock_name);
    sock.async_connect(ep, [&](auto err) {
        connected = !err;
        std::cout << "connect " << connected << std::endl;
        connect_called = true;
        listener.stop();

        boost::asio::local::stream_protocol::socket sock2(context);
        sock2.async_connect(ep, [&](auto) {
            std::cout << "connect " << std::endl;
            listener.start();
        });
    });


    while ((!timeout) && (!(connect_called && accept_called)))
    {
        context.run_one();
    }

    ASSERT_FALSE(timeout);
    ASSERT_TRUE(connected);
    ASSERT_TRUE(accept_called);
}
