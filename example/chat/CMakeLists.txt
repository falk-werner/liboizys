# SPDX-License-Identifier: MPL-2.0
# SPDX-FileCopyrightText: Copyright 2024 Falk Werner

find_package(Protobuf REQUIRED)
add_library(chat_proto STATIC messages.proto)
target_include_directories(chat_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(chat_proto PUBLIC ${Protobuf_LIBRARIES})
protobuf_generate(TARGET chat_proto)

add_executable(chat-server chat_server.cpp)
target_link_libraries(chat-server PRIVATE chat_proto oizys)

add_executable(chat-client chat_client.cpp)
target_link_libraries(chat-client PRIVATE chat_proto oizys)
