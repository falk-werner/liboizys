# SPDX-License-Identifier: MPL-2.0
# SPDX-FileCopyrightText: Copyright 2024 Falk Werner

add_executable(notification-server notification_server.cpp)
target_link_libraries(notification-server PRIVATE oizys)

add_executable(notification-client notification_client.cpp)
target_link_libraries(notification-client PRIVATE oizys)
