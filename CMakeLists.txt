cmake_minimum_required(VERSION 3.20)
project(asio-example)

option(WITHOUT_TEST "Disable unit test" OFF)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -extra-arg=-std=c++17 
    -extra-arg=-I -extra-arg=/usr/include/c++/11
    -extra-arg=-I -extra-arg=/usr/include/x86_64-linux-gnu/c++/11
    -checks=-*,modernize-*,readability-*,-modernize-use-trailing-return-type,-clang-diagnostic-error)

find_package(Protobuf REQUIRED)
add_library(proto STATIC proto/messages.proto)
target_include_directories(proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(proto PUBLIC ${Protobuf_LIBRARIES})
set_target_properties(proto PROPERTIES CXX_CLANG_TIDY "")
protobuf_generate(TARGET proto)


add_library(com STATIC 
    src/com/context.cpp
    src/com/asio_context.cpp
    src/com/asio_borrowed_context.cpp
    src/com/asio_session.cpp
    src/com/asio_listener.cpp
    src/com/com_message.cpp)
target_include_directories(com PUBLIC inc)

add_executable(chat_client src/client/main.cpp)
target_link_libraries(chat_client PRIVATE proto)
set_target_properties(chat_client PROPERTIES CXX_CLANG_TIDY "")

add_executable(chat_server
    src/server/main.cpp
    src/server/chat_server.cpp
    src/server/chat_session.cpp)
target_link_libraries(chat_server PRIVATE proto)
set_target_properties(chat_server PROPERTIES CXX_CLANG_TIDY "")

add_executable(test_client src/main.cpp)
set_target_properties(test_client PROPERTIES CXX_CLANG_TIDY "")

if(NOT(WITHOUT_TEST))

enable_testing()
include(CTest)

find_package(GTest REQUIRED)
include(GoogleTest)

include(cmake/coverage.cmake)


add_executable(alltests 
    test-src/com/test_com_message.cpp
    test-src/com/test_asio_listener.cpp
    test-src/com/test_context.cpp
    test-src/com/test_unstable_context.cpp
    test-src/com/test_asio_borrowed_context.cpp
    test-src/com/test_asio_context.cpp
    test-src/com/test_asio_session.cpp
    test-src/com/test_asio_session_read.cpp)
target_include_directories(alltests PRIVATE src/com)

# disable clang-tidy for unit test executables
set_target_properties(alltests PROPERTIES CXX_CLANG_TIDY "")

target_link_libraries(alltests PRIVATE com GTest::gtest GTest::gtest_main)
gtest_discover_tests(alltests EXTRA_ARGS "--gtest_output=xml:test-results.xml")

add_custom_target(coverage
    mkdir -p coverage
	COMMAND lcov --initial --capture --rc lcov_branch_coverage=1 --directory . --output-file coverage/lcov.info
	COMMAND ./alltests
	COMMAND lcov --capture --rc lcov_branch_coverage=1 --directory . --output-file coverage/lcov.info
	COMMAND lcov --rc lcov_branch_coverage=1 --remove coverage/lcov.info '/usr/*' --output-file coverage/lcov.info
	COMMAND lcov --rc lcov_branch_coverage=1 --remove coverage/lcov.info '*/test-src/*' --output-file coverage/lcov.info
)
add_dependencies(coverage alltests)

add_custom_target(coverage-report
	COMMAND genhtml -branch-coverage --highlight --legend coverage/lcov.info --output-directory coverage/report
)
add_dependencies(coverage-report coverage)

endif()