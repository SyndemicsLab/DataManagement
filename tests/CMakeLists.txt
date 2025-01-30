cmake_minimum_required(VERSION 3.20)
project(dataTests CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME} 
        src/TEST_Config.cpp
        src/TEST_CSVDataSource.cpp
        src/TEST_DataSource.cpp
        src/TEST_DBDataSource.cpp
        src/TEST_ModelData.cpp
        src/TEST_Utils.cpp
)

target_link_libraries(dataTests PRIVATE datamanagement)
target_link_libraries(dataTests PRIVATE GTest::gtest)
target_link_libraries(dataTests PRIVATE GTest::gtest_main)

target_include_directories(dataTests 
    PRIVATE 
        "mocks/"
        ${PROJECT_SOURCE_DIR}/../include/
)
