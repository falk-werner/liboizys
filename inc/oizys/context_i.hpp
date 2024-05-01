#ifndef OIZYS_CONTEXT_I_HPP
#define OIZYS_CONTEXT_I_HPP

#include <oizys/accept_handler.hpp>
#include <oizys/listener_i.hpp>

#include <memory>
#include <string>

namespace oizys
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
