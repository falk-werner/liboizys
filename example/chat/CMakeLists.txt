find_package(Protobuf REQUIRED)
add_library(chat_proto STATIC messages.proto)
target_include_directories(chat_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(chat_proto PUBLIC ${Protobuf_LIBRARIES})
set_target_properties(chat_proto PROPERTIES CXX_CLANG_TIDY "")
protobuf_generate(TARGET chat_proto)

add_executable(chat-server chat_server.cpp)
target_link_libraries(echo-server PRIVATE chat_proto com)

add_executable(chat-client chat_client.cpp)
target_link_libraries(chat-client PRIVATE chat_proto com)
