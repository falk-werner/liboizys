// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include "oizys/version.hpp"

namespace oizys
{

int get_major_version()
{
    return major_version;
}

int get_minor_version()
{
    return minor_version;
}

int get_patch_version()
{
    return patch_version;
}

}