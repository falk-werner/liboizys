// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "oizys/version.hpp"
#include <gtest/gtest.h>

TEST(version, get_major_version)
{
    ASSERT_EQ(oizys::major_version, oizys::get_major_version());
}

TEST(version, get_minor_version)
{
    ASSERT_EQ(oizys::minor_version, oizys::get_minor_version());
}

TEST(version, get_patch_version)
{
    ASSERT_EQ(oizys::patch_version, oizys::get_patch_version());
}
