cmake_minimum_required(VERSION 3.20)

project(DataManagementTests)

find_package(Boost REQUIRED COMPONENTS filesystem)

find_package(GTest REQUIRED)

enable_testing()

add_executable(dataTests main.cpp DataTableTest.cpp ConfigurationTest.cpp)

target_link_libraries(dataTests PUBLIC DataManagement) 
target_link_libraries(dataTests PUBLIC Boost::filesystem)
target_link_libraries(dataTests PUBLIC GTest::gtest)
target_link_libraries(dataTests PUBLIC GTest::gtest_main)

target_include_directories(dataTests PUBLIC "../include/")

gtest_discover_tests(dataTests)