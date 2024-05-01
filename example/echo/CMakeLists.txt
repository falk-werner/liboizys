add_executable(echo-server echo_server.cpp)
target_link_libraries(echo-server PRIVATE oizys)

add_executable(echo-client echo_client.cpp)
target_link_libraries(echo-client PRIVATE oizys)
