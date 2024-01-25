cmake_minimum_required(VERSION 3.20)

project(DataManagementTests)

find_package(Boost REQUIRED COMPONENTS filesystem)

include(FetchContent)

FetchContent_Declare(
googletest
URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(dataTests main.cpp DataTableTest.cpp ConfigurationTest.cpp)

target_link_libraries(dataTests PUBLIC DataManagement) 
target_link_libraries(dataTests PUBLIC Boost::filesystem)
target_link_libraries(dataTests PUBLIC gtest gtest_main)

target_include_directories(dataTests PUBLIC "../include/")

include(GoogleTest)
gtest_discover_tests(dataTests)