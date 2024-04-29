#include "asio_borrowed_context.hpp"
#include <gtest/gtest.h>
#include <unistd.h>

TEST(asio_borrowed_context, all)
{
    boost::asio::io_context context;
    com::asio_borrowed_context com_context(context);

    // add time to ensure the test finishes on failure
    boost::asio::deadline_timer timer(context);
    timer.expires_from_now(boost::posix_time::seconds(60));
    bool timeout = false;
    timer.async_wait([&](auto){
        timeout = true;
        context.stop();
    });

    std::string const sock_name("/tmp/com_test.sock");
    unlink(sock_name.c_str());
    bool accept_called = false;
    auto listener = com_context.new_listener(sock_name, [&](auto){
        accept_called = true;
    });
    listener->start();

    bool connected = false;
    bool connect_called = false;
    com_context.connect_to(sock_name, [&](auto session){
        connected = static_cast<bool>(session);
        connect_called = true;
    });

    while ((!timeout) && (!(connect_called && accept_called)))
    {
        com_context.run();
    }

    ASSERT_TRUE(accept_called);
    ASSERT_TRUE(connect_called);
    ASSERT_TRUE(connected);
    ASSERT_FALSE(timeout);
}
