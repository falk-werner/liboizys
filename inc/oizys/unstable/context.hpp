#ifndef OIZYS_UNSTABLE_COM_HPP
#define OIZYS_UNSTABLE_COM_HPP

#include <oizys/context_i.hpp>
#include <oizys/api.hpp>
#include <boost/asio.hpp>

namespace oizys
{

OIZYS_API std::unique_ptr<context_i> context_from_asio(boost::asio::io_context& context);

}

#endif
