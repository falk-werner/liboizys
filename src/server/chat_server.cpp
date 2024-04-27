#include "chat_server.hpp"
#include "chat_session.hpp"

namespace chat
{

chat_server::chat_server(boost::asio::io_context & context, std::string const & filename)
: acceptor(context, boost::asio::local::stream_protocol::endpoint(filename))
{
    do_accept();
}

void chat_server::do_accept()
{
    acceptor.async_accept([this](auto ec, auto socket) {
        if (!ec)
        {
            std::make_shared<chat_session>(std::move(socket))->start();
        }

        do_accept();
    });
}


}