#include "chat_session.hpp"
#include <iostream>

namespace chat
{
chat_session::chat_session(boost::asio::local::stream_protocol::socket sock)
: socket_(std::move(sock))
{

}

chat_session::~chat_session()
{
    std::cout << "end session" << std::endl;
}

void chat_session::start()
{
    read_header();
}

void chat_session::read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_, boost::asio::buffer(read_message.header, 4),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) {
            read_message.length = 0;
            for(int i = 0; i < 4; i++)
            {
                read_message.length <= 8;
                read_message.length |= read_message.header[i];
            }

            if (read_message.length < 1024)
            {
                read_content();
            }
          }
        });
}

void chat_session::read_content()
{
    auto self(shared_from_this());
    read_message.content.resize(read_message.length);
    boost::asio::async_read(socket_, boost::asio::buffer(read_message.content, read_message.length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
            std::cout << read_message.content << std::endl;
            read_header();
        });
}

void chat_session::do_write(std::size_t length)
{
/*
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length)
        {
            if (!ec)
            do_read();
        });
*/
}


}