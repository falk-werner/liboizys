#include "com/context.hpp"
#include "com/unstable/context.hpp"

#include "asio_context.hpp"
#include "asio_borrowed_context.hpp"

namespace com
{

std::unique_ptr<context_i> context_new()
{
    auto * context = new asio_context();
    return std::unique_ptr<context_i>(context);
}

std::unique_ptr<context_i> context_from_asio(boost::asio::io_context& context)
{
    auto * ctx = new asio_borrowed_context(context);
    return std::unique_ptr<context_i>(ctx);
}

}