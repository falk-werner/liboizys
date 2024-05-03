// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#ifndef OIZYS_SESSION_I_HPP
#define OIZYS_SESSION_I_HPP

#include <functional>
#include <string>
#include <boost/asio.hpp>

namespace oizys
{

/// @brief Type of handler that is invoked when the session is closed.
/// @see session_i::set_onclose
using close_handler = std::function<void(boost::system::error_code err)>;

/// @brief Type of handler that is invoked when a message is received
/// @param message message received
/// @see session_i::set_onmessage
using message_handler = std::function<void(std::string const & message)>;

/// @brief Session interface.
class session_i
{
public:
    /// @brief Destorys the session
    virtual ~session_i() = default;

    /// @brief Sends a message asynchronously.
    ///
    /// The method adds the message to the send queue and returns immediatly.
    /// The message will be send asynchronously.
    ///
    /// @note Make sure, this method is only called from within the same
    ///       thread, that runs the associated asio::io_context.
    /// @note There is no way to detect that or when the message is received
    ///       by the peer.
    ///
    /// @param message message to send.
    /// @return Count of messages in the write queue
    virtual size_t send(std::string const & message) = 0;

    /// @brief Sets a handler that is invoked when the session is closed.
    ///
    /// The handler is invoked when the session gets closed. There is no
    /// way to prevent the session from closing. The handler may be used
    /// to clean up data assiciated with the session.
    ///
    /// @note It is not allowed to invoke this method twice on the same session.
    ///
    /// @throws std::exception When a handler is already set.
    /// @param handler Handler that is invoked when the session is closed.
    virtual void set_onclose(close_handler handler) = 0;

    /// @brief Sets a handler that is invoked whe a message is received.
    ///
    /// The handler is invoked whenever a message is received.
    ///
    /// @note It is not allowed to invoke this method twice on the same session.
    ///
    /// @throws std::exception When a handler is already set.
    /// @param handler 
    virtual void set_onmessage(message_handler handler) = 0;

    /// @brief Closes the session.
    ///
    /// If the an on close handelr is set, the handler is invoked.
    /// @note All registered handler will be removed when the session is closed.
    virtual void close() = 0;
};

}

#endif
