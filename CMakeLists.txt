cmake_minimum_required(VERSION 3.20)

set (CMAKE_CXX_STANDARD 17)

project(
    DataManagement
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

find_package(Boost 
REQUIRED COMPONENTS filesystem 
    PATHS libs/)

add_library(data
    src/DataTable.cpp
    src/Configuration.cpp
    src/SQLite3.cpp
    libs/sqlite3/sqlite3.c
)

set(DM_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
set(SQL_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/libs/sqlite3")

target_include_directories(data PUBLIC ${DM_INCLUDES} ${SQL_INCLUDES})

target_link_libraries(data
    Boost::filesystem
)

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS)
    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(dataTests test/main.cpp test/DataTableTest.cpp test/ConfigurationTest.cpp)
    target_link_libraries(dataTests data Boost::filesystem gtest gtest_main)
    include(GoogleTest)
    gtest_discover_tests(dataTests)
endif()
