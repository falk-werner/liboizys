#include "asio_session.hpp"

#include <boost/asio.hpp>
#include <gtest/gtest.h>
#include <unistd.h>

#include <memory>
#include <string>

TEST(asio_session, fail_to_set_close_handler_twice)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<com::asio_session>(std::move(read_sock));

    session->set_on_close([](){});
    ASSERT_ANY_THROW({
        session->set_on_close([](){});
    });
}

TEST(asio_session, fail_to_set_message_handler_twice)
{
    boost::asio::io_context context;
    boost::asio::local::stream_protocol::socket read_sock(context);
    auto session = std::make_shared<com::asio_session>(std::move(read_sock));

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
    auto session = std::make_shared<com::asio_session>(std::move(read_sock));

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

