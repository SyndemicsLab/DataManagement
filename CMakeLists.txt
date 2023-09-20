cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 17)

project(
    DataManagement
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

find_package(Boost REQUIRED COMPONENTS filesystem PATHS libs/)

add_library(data
    src/DataTable.cpp
)

set(DM_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(data PUBLIC ${DM_INCLUDES})

target_link_libraries(data
    Boost::filesystem
)

option(BUILD_TESTS "enable Data tests" ON)

if(BUILD_TESTS)

    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()

    add_executable(dataTests test/main.cpp test/DataTableTest.cpp)
    target_link_libraries(dataTests data Boost::filesystem gtest gtest_main)
    include(GoogleTest)
    gtest_discover_tests(dataTests)
endif()


