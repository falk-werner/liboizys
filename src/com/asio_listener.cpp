#include "asio_listener.hpp"
#include "asio_session.hpp"

namespace com
{

asio_listener::asio_listener(boost::asio::io_context & context, std::string const & endpoint,
    accept_handler handler)
: acceptor(context, boost::asio::local::stream_protocol::endpoint(endpoint))
, on_accept(handler)
, is_listening(false)
{

}

void asio_listener::start()
{
    if (!is_listening)
    {
        is_listening = true;
        do_accept();
    }
}

void asio_listener::stop()
{
    is_listening = false;
}

void asio_listener::do_accept()
{
    acceptor.async_accept([this](auto ec, auto sock) {
        if ((ec) || (!is_listening))
        { 
            return; 
        }

        auto session = std::make_shared<asio_session>(std::move(sock));
        on_accept(session);

        if (is_listening)
        {
            do_accept();
        }
    });
}


}