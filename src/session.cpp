#include "oizys/session.hpp"
#include "asio_session.hpp"

namespace oizys
{

std::shared_ptr<session_i> create_session(boost::asio::local::stream_protocol::socket sock)
{
    return std::make_shared<asio_session>(std::move(sock));
}

}