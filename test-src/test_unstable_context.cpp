#include "oizys/unstable/context.hpp"
#include <gtest/gtest.h>

TEST(unstable_context, create_new)
{
    boost::asio::io_context context;
    auto com_context = oizys::context_from_asio(context);
}
