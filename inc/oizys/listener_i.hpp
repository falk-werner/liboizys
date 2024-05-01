// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_LISTENER_I_HPP
#define OIZYS_LISTENER_I_HPP

namespace oizys
{

class listener_i
{
public:
    virtual ~listener_i() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};

}

#endif
