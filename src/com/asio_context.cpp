#include "asio_context.hpp"
#include "asio_listener.hpp"
#include "asio_session.hpp"

#include <boost/asio.hpp>

#include <stdexcept>

namespace com
{

void asio_context::run()
{
    context.run_one();
}

std::unique_ptr<listener_i> asio_context::new_listener(std::string const & endpoint, accept_handler handler)
{
    auto * listener = new asio_listener(context, endpoint, handler);
    return std::unique_ptr<listener_i>(listener);
}

void asio_context::connect_to(std::string const & endpoint, accept_handler handler)
{
    boost::asio::local::stream_protocol::socket sock(context);
    auto session = std::make_shared<asio_session>(std::move(sock));
    session->connect_to(endpoint, handler);
}


}