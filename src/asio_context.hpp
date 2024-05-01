#ifndef OIZYS_ASIO_CONTEXT_HPP
#define OIZYS_ASIO_CONTEXT_HPP

#include "oizys/context_i.hpp"
#include <boost/asio.hpp>

namespace oizys
{

class asio_context: public context_i
{
public:
    virtual ~asio_context() override = default;
    virtual void run() override;
    virtual std::unique_ptr<listener_i> new_listener(std::string const & endpoint, accept_handler handler) override;
    virtual void connect_to(std::string const & endpoint, accept_handler handler) override;
private:
    boost::asio::io_context context;
};

}

#endif
