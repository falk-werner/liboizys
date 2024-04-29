#ifndef COM_UNSTABLE_COM_HPP
#define COM_UNSTABLE_COM_HPP

#include <com/context_i.hpp>
#include <com/api.hpp>
#include <boost/asio.hpp>

namespace com
{

COM_API std::unique_ptr<context_i> context_from_asio(boost::asio::io_context& context);

}

#endif
