#ifndef COM_ASIO_CONTEXT_HPP
#define COM_ASIO_CONTEXT_HPP

#include "com/context_i.hpp"
#include <boost/asio.hpp>

namespace com
{

class asio_context: public context_i
{
public:
    asio_context();
    virtual ~asio_context() override = default;
    virtual void run() override;
    virtual std::unique_ptr<listener_i> new_listener(std::string const & endpoint, accept_handler handler) override;
    virtual void connect_to(std::string const & endpoint, accept_handler handler) override;
private:
    boost::asio::io_context context;
};

}

#endif
