cmake_minimum_required(VERSION 3.20)

set (CMAKE_CXX_STANDARD 17)

project(
    DataManagement
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

find_package(Boost REQUIRED COMPONENTS filesystem)

add_library(DataManagement SHARED
    src/DataTable.cpp
    src/Configuration.cpp
    src/SQLite3.cpp
    lib/sqlite3/sqlite3.c
)
add_library(DataManagement::DataManagement ALIAS DataManagement)

target_include_directories(DataManagement PRIVATE 
    "include/"
    "lib/sqlite3/"
    ${Boost_INCLUDE_DIRS})

target_link_libraries(DataManagement
    Boost::filesystem
)

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS STREQUAL ON)
    add_subdirectory(test)
endif()

include(GNUInstallDirs)

install(TARGETS DataManagement DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement)

list(APPEND LIB_INCLUDE_FILES 
    "include/Configuration.hpp"
    "include/DataManagement.hpp"
    "include/DataTable.hpp"
    "include/Reader.hpp"
    "include/SQLite3.hpp"
    "include/Writer.hpp")

install(
    FILES ${LIB_INCLUDE_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/DataManagement
)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/DataManagement
    CACHE PATH "Location of header files")
set(SYSCONFIG_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR}/DataManagement
    CACHE PATH "Location of configuration files")
set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
    CACHE PATH "Location of Library")

include(CMakePackageConfigHelpers)
configure_package_config_file(DataManagementConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
  PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR LIBRARY_INSTALL_DIR
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/DataManagement
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libDataManagement.a
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
)
