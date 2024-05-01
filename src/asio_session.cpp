// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "asio_session.hpp"
#include <stdexcept>

namespace oizys
{

asio_session::asio_session(boost::asio::local::stream_protocol::socket sock)
: socket_(std::move(sock))
{

}

asio_session::~asio_session()
{
    asio_session::close();
}

void asio_session::send(std::string const & message)
{
    write_queue.emplace(message);
    if (write_queue.size() == 1)
    {
        write_header();
    }
}

void asio_session::set_on_close(close_handler handler)
{
    if (on_close)
    {
        throw std::runtime_error("handler already set");
    }

    on_close = std::move(handler);

    if(!socket_.is_open())
    {
        try
        {
            on_close();
        }
        catch (...)
        {
            // swallow
        }
    }
}

void asio_session::set_on_message(message_handler handler)
{
    if (on_message)
    {
        throw std::runtime_error("handler already set");        
    }

    on_message = std::move(handler);
    read_header();
}

void asio_session::close()
{
    if (socket_.is_open())
    {
        socket_.close();
        if (on_close)
        {
            try
            {
                on_close();
            }
            catch(...)
            {
                // swallow
            }

            on_close = [](){};
        }
    
        if (on_message)
        {
            on_message = [](auto){};            
        }
    }
}

void asio_session::read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_, boost::asio::buffer(message_to_read.header, 4),
        [this, self](auto err, auto) {
            if (err)
            {
                close();
                return;
            }

            size_t length;
            if (!message_to_read.parse_header(length))
            {
                close();
                return;
            }
            
            read_payload(length);
        });

}

void asio_session::read_payload(size_t length)
{
    message_to_read.payload.resize(length);

    auto self(shared_from_this());
    boost::asio::async_read(socket_, boost::asio::buffer(message_to_read.payload, length),
        [this, self](auto err, auto) {
            if (err)
            {
                close();
                return;
            }

            if (on_message)
            {
                try
                {
                    on_message(message_to_read.payload);
                }
                catch (...)
                {
                    // swallow
                }
            }

            read_header();
    });
}

void asio_session::write_header()
{
    if (write_queue.empty()) { return; }

    auto & message = write_queue.front();

    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(message.header, 4),
        [this, self](auto err, auto){
            if (err)
            {
                close();
                return;
            }

            write_payload();
    });
}

void asio_session::write_payload()
{
    if (write_queue.empty()) { return; }

    auto & message = write_queue.front();
    
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(message.payload, message.payload.size()),
        [this, self](auto err, auto){
            if (err)
            {
                close();
                return;
            }

            write_queue.pop();
            write_header();
    });
}



}