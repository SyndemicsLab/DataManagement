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

enable_testing()

add_executable(${PROJECT_NAME} 
        src/test_config.cpp
        src/test_csv_source.cpp
        src/test_db_source.cpp
        src/test_model_data.cpp
)

target_link_libraries(dataTests PRIVATE datamanagement)
target_link_libraries(dataTests PRIVATE GTest::gtest)
target_link_libraries(dataTests PRIVATE GTest::gtest_main)

target_include_directories(dataTests 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/../include/
)
