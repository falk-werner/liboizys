#ifndef COM_ASIO_BORROWED_CONTEXT_HPP
#define COM_ASIO_BORROWED_CONTEXT_HPP

#include "com/context_i.hpp"
#include <boost/asio.hpp>

namespace com
{

class asio_borrowed_context: public context_i
{
public:
    explicit asio_borrowed_context(boost::asio::io_context& context);
    virtual ~asio_borrowed_context() override = default;
    virtual void run() override;
    virtual std::unique_ptr<listener_i> new_listener(std::string const & endpoint, accept_handler handler) override;
    virtual void connect_to(std::string const & endpoint, accept_handler handler) override;
private:
    boost::asio::io_context& context_;
};

}

#endif
