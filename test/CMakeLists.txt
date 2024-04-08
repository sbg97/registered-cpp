cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

set(registered-cpp_TEST_SRC
	tests.cpp
)

PREPEND(registered-cpp_TEST_SRC)

find_package(Catch2 REQUIRED)

add_executable(test-registered-cpp ${registered-cpp_TEST_SRC})

include(Catch)
catch_discover_tests(test-registered-cpp)
enable_testing()

target_link_libraries(test-registered-cpp PRIVATE registered-cpp Catch2::Catch2WithMain)
