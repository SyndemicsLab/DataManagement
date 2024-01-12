cmake_minimum_required(VERSION 3.20)

project(DataManagementTests)

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

target_include_directories(dataTests PUBLIC 
    ${CMAKE_SOURCE_DIR}include/
    ${CMAKE_SOURCE_DIR}lib/sqlite3/
    ${CMAKE_SOURCE_DIR}lib/boost/include/boost/)

target_link_libraries(dataTests DataManagement Boost::filesystem gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(dataTests)