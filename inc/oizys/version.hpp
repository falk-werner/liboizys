// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_VERSION_HPP
#define OIZYS_VERSION_HPP

#include <oizys/api.hpp>

namespace oizys
{

/// @brief Major version of liboizys.
constexpr int const major_version = 0;

/// @brief Minor version of liboizys.
constexpr int const minor_version = 1;

/// @brief Patch version of liboizys.
constexpr int const patch_version = 0;

/// @brief Returns the major version of liboizys.
/// @return major version if liboizys 
OIZYS_API int get_major_version();

/// @brief Returns the minor version of liboizys.
/// @return minor version if liboizys 
OIZYS_API int get_minor_version();

/// @brief Returns the patch version of liboizys.
/// @return patch version if liboizys 
OIZYS_API int get_patch_version();

}

#endif
