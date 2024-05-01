#ifndef OIZYS_SESSION_I_HPP
#define OIZYS_SESSION_I_HPP

#include <functional>
#include <string>

namespace oizys
{

using close_handler = std::function<void(void)>;
using message_handler = std::function<void(std::string const & message)>;

class session_i
{
public:
    virtual ~session_i() = default;
    virtual void send(std::string const & message) = 0;
    virtual void set_on_close(close_handler handler) = 0;
    virtual void set_on_message(message_handler handler) = 0;
    virtual void close() = 0;
};

}

#endif
