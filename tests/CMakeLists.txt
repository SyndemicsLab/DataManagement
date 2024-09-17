cmake_minimum_required(VERSION 3.20)
project(dataTests CXX)

find_package(GTest REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME} 
        src/TEST_DataManager.cpp
)

target_link_libraries(dataTests PRIVATE DataManagement)
target_link_libraries(dataTests PRIVATE GTest::gtest)
target_link_libraries(dataTests PRIVATE GTest::gtest_main)

target_include_directories(dataTests 
    PRIVATE 
        "mocks/"
        ${PROJECT_SOURCE_DIR}/../include/
)
