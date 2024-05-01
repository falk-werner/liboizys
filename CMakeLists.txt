cmake_minimum_required(VERSION 3.20)
project(liboizys
    VERSION 0.1.0
    DESCRIPTION "Interpocess communication library based on boost asio"
    HOMEPAGE_URL "https://github.com/falk-werner/liboizys"
    LANGUAGES CXX)

option(WITHOUT_TEST "Disable unit test" OFF)
option(WITHOUT_EXAMPLES "Disable examples" OFF)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -extra-arg=-std=c++17 
    -checks=-*,modernize-*,readability-*,-modernize-use-trailing-return-type,-clang-diagnostic-error)

add_library(oizys_obj OBJECT
    src/context.cpp
    src/asio_context.cpp
    src/asio_borrowed_context.cpp
    src/asio_session.cpp
    src/asio_listener.cpp
    src/com_message.cpp
    src/version.cpp)
set_target_properties(oizys_obj PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1)
target_include_directories(oizys_obj PRIVATE inc)
target_compile_definitions(oizys_obj PRIVATE OIZYS_EXPORT)

add_library(oizys_static STATIC $<TARGET_OBJECTS:oizys_obj>)
target_include_directories(oizys_static PUBLIC inc)

add_library(oizys SHARED $<TARGET_OBJECTS:oizys_obj>)
target_include_directories(oizys PUBLIC inc)

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
    test-src/test_com_message.cpp
    test-src/test_asio_listener.cpp
    test-src/test_context.cpp
    test-src/test_unstable_context.cpp
    test-src/test_asio_borrowed_context.cpp
    test-src/test_asio_context.cpp
    test-src/test_asio_session.cpp
    test-src/test_asio_session_read.cpp
    test-src/test_asio_session_write.cpp
    test-src/test_version.cpp)
target_include_directories(alltests PRIVATE src)

# disable clang-tidy for unit test executables
set_target_properties(alltests PROPERTIES CXX_CLANG_TIDY "")

target_link_libraries(alltests PRIVATE oizys_static GTest::gtest GTest::gtest_main)
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