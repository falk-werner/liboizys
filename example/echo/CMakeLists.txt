# SPDX-License-Identifier: MPL-2.0
# SPDX-FileCopyrightText: Copyright 2024 Falk Werner

add_executable(echo-server echo_server.cpp)
target_link_libraries(echo-server PRIVATE oizys)

add_executable(echo-client echo_client.cpp)
target_link_libraries(echo-client PRIVATE oizys)
