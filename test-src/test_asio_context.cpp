// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_context.hpp"
#include <gtest/gtest.h>
#include <unistd.h>

TEST(asio_context, all)
{
    oizys::asio_context context;

    std::string const sock_name("/tmp/com_test.sock");
    unlink(sock_name.c_str());
    bool accept_called = false;
    auto listener = context.new_listener(sock_name, [&](auto){
        accept_called = true;
    });
    listener->start();

    bool connected = false;
    bool connect_called = false;
    context.connect_to(sock_name, [&](auto session){
        connected = static_cast<bool>(session);
        connect_called = true;
    });

    while (!(connect_called && accept_called))
    {
        context.run();
    }

    ASSERT_TRUE(accept_called);
    ASSERT_TRUE(connect_called);
    ASSERT_TRUE(connected);
}
