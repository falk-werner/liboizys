// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_SESSION_HPP
#define OIZYS_SESSION_HPP

#include <oizys/session_i.hpp>
#include <oizys/api.hpp>

#include <boost/asio.hpp>
#include <memory>

namespace oizys
{

/// @brief Creates a new session.
/// @param sock An already connected socket that will be associated with the session.
/// @return A newly created session.
OIZYS_API std::shared_ptr<session_i> create_session(boost::asio::local::stream_protocol::socket sock);

}

#endif
