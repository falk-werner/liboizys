#ifndef COM_CONTEXT_HPP
#define COM_CONTEXT_HPP

#include <com/context_i.hpp>
#include <com/api.hpp>

namespace com
{

COM_API std::unique_ptr<context_i> context_new();

}


#endif

