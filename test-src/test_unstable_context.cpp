// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "oizys/unstable/context.hpp"
#include <gtest/gtest.h>

TEST(unstable_context, create_new)
{
    boost::asio::io_context context;
    auto com_context = oizys::context_from_asio(context);
}
