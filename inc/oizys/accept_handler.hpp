#ifndef OIZYS_ACCEPT_HANDLER_HPP
#define OIZYS_ACCEPT_HANDLER_HPP

#include <oizys/session_i.hpp>

#include <functional>
#include <memory>

namespace oizys
{

using accept_handler = std::function<void(std::shared_ptr<session_i>)>;

}

#endif
