#include "oizys/session.hpp"
#include <gtest/gtest.h>

TEST(session, create)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket sock(context);

    auto session = oizys::create_session(std::move(sock));
}