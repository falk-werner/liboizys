#ifndef COM_ACCEPT_HANDLER_HPP
#define COM_ACCEPT_HANDLER_HPP

#include <com/session_i.hpp>

#include <functional>
#include <memory>

namespace com
{

using accept_handler = std::function<void(std::shared_ptr<session_i>)>;

}

#endif
