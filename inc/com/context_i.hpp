#ifndef COM_CONTEXT_I_HPP
#define COM_CONTEXT_I_HPP

#include <com/accept_handler.hpp>
#include <com/listener_i.hpp>

#include <memory>
#include <string>

namespace com
{

class context_i
{
public:
    virtual ~context_i() = default;
    virtual void run() = 0;
    virtual std::unique_ptr<listener_i> new_listener(std::string const & endpoint, accept_handler handler) = 0;
    virtual void connect_to(std::string const & endpoint, accept_handler handler) = 0;
};


}

#endif
