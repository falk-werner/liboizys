cmake_minimum_required(VERSION 3.20)
project(asio-example)

option(WITHOUT_TEST "Disable unit test" OFF)
option(WITHOUT_EXAMPLES "Disable examples" OFF)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -extra-arg=-std=c++17 
    -checks=-*,modernize-*,readability-*,-modernize-use-trailing-return-type,-clang-diagnostic-error)

add_library(com_obj OBJECT
    src/com/context.cpp
    src/com/asio_context.cpp
    src/com/asio_borrowed_context.cpp
    src/com/asio_session.cpp
    src/com/asio_listener.cpp
    src/com/com_message.cpp)
set_target_properties(com_obj PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1)
target_include_directories(com_obj PRIVATE inc)
target_compile_definitions(com_obj PRIVATE COM_EXPORT)

add_library(com STATIC $<TARGET_OBJECTS:com_obj>)
target_include_directories(com PUBLIC inc)

add_library(com_shared SHARED $<TARGET_OBJECTS:com_obj>)
target_include_directories(com_shared PUBLIC inc)

if(NOT(WITHOUT_EXAMPLES))

add_subdirectory(example/echo)
add_subdirectory(example/chat)

endif()

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
    test-src/com/test_asio_session_read.cpp
    test-src/com/test_asio_session_write.cpp)
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