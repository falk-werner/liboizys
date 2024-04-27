#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "chat_session.hpp"

#include <boost/asio.hpp>
#include <string>
#include <unordered_map>

namespace chat
{

class chat_server
{
public:
    chat_server(boost::asio::io_context & context, std::string const & filename);
    ~chat_server() = default;    
private:
    void do_accept();
    uint32_t next_id();
    boost::asio::local::stream_protocol::acceptor acceptor;
    uint32_t id;
    std::unordered_map<uint32_t, chat_session> sessions;
};

}

#endif
