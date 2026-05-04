cmake_minimum_required(VERSION 3.20)
project(polycpp_cookie VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Fetch polycpp (disable its tests when building as a dependency).
# Keep the revision pinned; callers can override with FETCHCONTENT_SOURCE_DIR_POLYCPP.
set(POLYCPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    polycpp
    GIT_REPOSITORY https://github.com/enricohuang/polycpp.git
    GIT_TAG        7a076d7bbffaa16f6001e5c4fa3a0aead34ddb32
)
FetchContent_MakeAvailable(polycpp)

# Library target
add_library(polycpp_cookie
    src/cookie.cpp
)
target_link_libraries(polycpp_cookie PUBLIC polycpp)
target_include_directories(polycpp_cookie PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Alias for consistent naming
add_library(polycpp::cookie ALIAS polycpp_cookie)

option(POLYCPP_COOKIE_BUILD_EXAMPLES "Build examples" OFF)
if(POLYCPP_COOKIE_BUILD_EXAMPLES)
    add_executable(polycpp_cookie_example_parse_header examples/parse_header.cpp)
    target_link_libraries(polycpp_cookie_example_parse_header PRIVATE polycpp::cookie)
    set_target_properties(polycpp_cookie_example_parse_header PROPERTIES
        OUTPUT_NAME parse_header
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )

    add_executable(polycpp_cookie_example_session_lifecycle examples/session_lifecycle.cpp)
    target_link_libraries(polycpp_cookie_example_session_lifecycle PRIVATE polycpp::cookie)
    set_target_properties(polycpp_cookie_example_session_lifecycle PROPERTIES
        OUTPUT_NAME session_lifecycle
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
endif()

option(POLYCPP_COOKIE_BUILD_BENCHMARKS "Build benchmarks" OFF)
if(POLYCPP_COOKIE_BUILD_BENCHMARKS)
    add_executable(polycpp_cookie_benchmark benchmarks/cookie_bench.cpp)
    target_link_libraries(polycpp_cookie_benchmark PRIVATE polycpp::cookie)
    set_target_properties(polycpp_cookie_benchmark PROPERTIES
        OUTPUT_NAME cookie_bench
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks
    )
endif()

# Tests
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(POLYCPP_COOKIE_BUILD_TESTS_DEFAULT ON)
else()
    set(POLYCPP_COOKIE_BUILD_TESTS_DEFAULT OFF)
endif()
option(POLYCPP_COOKIE_BUILD_TESTS "Build tests" ${POLYCPP_COOKIE_BUILD_TESTS_DEFAULT})
if(POLYCPP_COOKIE_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(polycpp_cookie_test_cookie tests/test_cookie.cpp)
    target_link_libraries(polycpp_cookie_test_cookie PRIVATE polycpp::cookie GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(polycpp_cookie_test_cookie)
endif()
