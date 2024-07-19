cmake_minimum_required(VERSION 3.20)
project(dataTests)

set(BOOST_ROOT "$ENV{BOOST_ROOT}")
set(BOOST_INCLUDEDIR "$ENV{BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "$ENV{BOOST_ROOT}/lib")

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem)
find_package(GTest REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME} main.cpp DataTableTest.cpp ConfigurationTest.cpp)

target_link_libraries(dataTests PUBLIC DataManagement)
target_link_libraries(dataTests PUBLIC Boost::filesystem)
target_link_libraries(dataTests PUBLIC GTest::gtest)
target_link_libraries(dataTests PUBLIC GTest::gtest_main)

target_include_directories(dataTests PUBLIC "../include/")

gtest_discover_tests(${PROJECT_NAME})
