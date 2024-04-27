#ifndef COM_CONTEXT_HPP
#define COM_CONTEXT_HPP

#include <com/context_i.hpp>

namespace com
{

std::unique_ptr<context_i> context_new();

}


#endif

