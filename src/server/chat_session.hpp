#ifndef CHAT_SESSION_HPP
#define CHAT_SESSION_HPP

#include <boost/asio.hpp>
#include <queue>

namespace chat
{

struct chat_message
{
    unsigned char header[4];
    uint32_t length;
    std::string content;
};

class chat_session: public std::enable_shared_from_this<chat_session>
{
public:
    chat_session(boost::asio::local::stream_protocol::socket sock);
    ~chat_session();
    void start();
private:
    void read_header();
    void read_content();
    void do_write(std::size_t length);

    boost::asio::local::stream_protocol::socket socket_;
    chat_message read_message;
};

}

#endif
