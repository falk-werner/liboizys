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

size_t asio_session::send(std::string const & message)
{
    write_queue.emplace(message);
    auto const count = write_queue.size(); 
    if (count == 1)
    {
        write_header();
    }

    return count;
}

void asio_session::set_onclose(close_handler handler)
{
    if (onclose)
    {
        throw std::runtime_error("handler already set");
    }

    onclose = std::move(handler);

    if(!socket_.is_open())
    {
        try
        {
            boost::system::error_code err;
            onclose(err);
        }
        catch (...)
        {
            // swallow
        }
    }
}

void asio_session::set_onmessage(message_handler handler)
{
    if (onmessage)
    {
        throw std::runtime_error("handler already set");        
    }

    onmessage = std::move(handler);
    read_header();
}

void asio_session::close()
{
    boost::system::error_code err;
    close_intern(err);
}

void asio_session::close_intern(boost::system::error_code err)
{
    if (socket_.is_open())
    {
        socket_.close();
        if (onclose)
        {
            try
            {
                onclose(err);
            }
            catch(...)
            {
                // swallow
            }

            onclose = [](auto){};
        }
    
        if (onmessage)
        {
            onmessage = [](auto){};            
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
                close_intern(err);
                return;
            }

            size_t length;
            if (!message_to_read.parse_header(length))
            {
                close_intern({EBADMSG, boost::system::generic_category()});
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
                close_intern(err);
                return;
            }

            if (onmessage)
            {
                try
                {
                    onmessage(message_to_read.payload);
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
                close_intern(err);
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
                close_intern(err);
                return;
            }

            write_queue.pop();
            write_header();
    });
}



}