cmake_minimum_required(VERSION 3.27)
project(datamanagement_tests VERSION ${DATAMANAGEMENT_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT DATAMANAGEMENT_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but DATAMANAGEMENT_BUILD_TESTS is OFF!")
    return()
endif()

enable_testing()

add_executable(${PROJECT_NAME} 
        src/test_config.cpp
        src/test_csv_source.cpp
        src/test_db_source.cpp
        src/test_model_data.cpp
)

target_link_libraries(datamanagement_tests 
    PUBLIC 
        datamanagement
        GTest::gtest
        GTest::gtest_main
        spdlog::spdlog
)

gtest_discover_tests(${PROJECT_NAME})
