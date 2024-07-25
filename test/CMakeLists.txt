cmake_minimum_required(VERSION 3.20)
project(dataTests)

find_package(GTest REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME} main.cpp DataTableTest.cpp ConfigurationTest.cpp)

target_link_libraries(dataTests PUBLIC DataManagement)
target_link_libraries(dataTests PUBLIC GTest::gtest)
target_link_libraries(dataTests PUBLIC GTest::gtest_main)

target_include_directories(dataTests PUBLIC "../include/")

gtest_discover_tests(${PROJECT_NAME})
