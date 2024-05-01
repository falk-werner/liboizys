// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_CONTEXT_HPP
#define OIZYS_CONTEXT_HPP

#include <oizys/context_i.hpp>
#include <oizys/api.hpp>

namespace oizys
{

OIZYS_API std::unique_ptr<context_i> context_new();

}


#endif

