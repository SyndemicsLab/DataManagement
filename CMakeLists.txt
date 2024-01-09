cmake_minimum_required(VERSION 3.20)

set (CMAKE_CXX_STANDARD 17)

project(
    DataManagement
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

find_package(Boost 
REQUIRED COMPONENTS filesystem 
    PATHS lib/boost)

add_library(dataManagement
    src/DataTable.cpp
    src/Configuration.cpp
    src/SQLite3.cpp
    lib/sqlite3/sqlite3.c
)
add_library(dataManagement::dataManagement ALIAS dataManagement)

target_include_directories(dataManagement PRIVATE 
    "include/"
    "lib/sqlite3/"
    "lib/boost/include/boost/")

target_link_libraries(dataManagement
    Boost::filesystem
)

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS STREQUAL ON)
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
    target_link_libraries(dataTests dataManagement Boost::filesystem gtest gtest_main)
    include(GoogleTest)
    gtest_discover_tests(dataTests)
endif()

include(GNUInstallDirs)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/DataManagement
    CACHE PATH "Location of header files")
set(SYSCONFIG_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR}/dataManagement
    CACHE PATH "Location of configuration files")

install(TARGETS dataManagement
    EXPORT dataManagementTargets
    INCLUDES DESTINATION include
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    FILES
        "include/DataManagement.hpp"
    DESTINATION
        include
    COMPONENT
        Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
)

export(EXPORT dataManagementTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/DataManagement/DataManagementTargets.cmake"
  NAMESPACE Upstream::
)
configure_package_config_file(DataManagementConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
)
