// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_VERSION_HPP
#define OIZYS_VERSION_HPP

#include <oizys/api.hpp>

namespace oizys
{

constexpr int const major_version = 0;
constexpr int const minor_version = 1;
constexpr int const patch_version = 0;

OIZYS_API int get_major_version();
OIZYS_API int get_minor_version();
OIZYS_API int get_patch_version();


}

#endif
