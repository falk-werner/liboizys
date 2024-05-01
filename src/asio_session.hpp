#ifndef OIZYS_ASIO_SESSION_HPP
#define OIZYS_ASIO_SESSION_HPP

#include "oizys/session_i.hpp"
#include "oizys/accept_handler.hpp"

#include "com_message.hpp"

#include <boost/asio.hpp>

#include <queue>
#include <memory>

namespace oizys
{

class asio_session
: public session_i 
, public std::enable_shared_from_this<asio_session>
{
    asio_session(asio_session const &) = delete;
    asio_session operator=(asio_session const &) = delete;
public:
    asio_session(boost::asio::local::stream_protocol::socket sock);
    virtual ~asio_session() override;
    virtual void send(std::string const & message) override;
    virtual void set_on_close(close_handler handler) override;
    virtual void set_on_message(message_handler handler) override;
    virtual void close() override;

    void start();
    void connect_to(std::string const & endpoint, accept_handler handler);

private:
    void read_header();
    void read_payload(size_t length);
    void write_header();
    void write_payload();

    boost::asio::local::stream_protocol::socket socket_;
    com_message message_to_read;
    std::queue<com_message> write_queue;
    message_handler on_message;
    close_handler on_close;
};

}

#endif
