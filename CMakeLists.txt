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
    add_subdirectory(test)
endif()

include(GNUInstallDirs)

install(TARGETS dataManagement
    EXPORT dataManagementTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/DataManagement
    RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/DataManagement
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement)

list(APPEND LIB_INCLUDE_FILES 
    "include/Configuration.hpp"
    "include/DataManagement.hpp"
    "include/DataTable.hpp"
    "include/Reader.hpp"
    "include/SQLite3.hpp"
    "include/Writer.hpp")

install(
    FILES
        ${LIB_INCLUDE_FILES}
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/DataManagement
    COMPONENT
        Devel
)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/DataManagement
    CACHE PATH "Location of header files")
set(SYSCONFIG_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR}/DataManagement
    CACHE PATH "Location of configuration files")

include(CMakePackageConfigHelpers)
configure_package_config_file(DataManagementConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement
  PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
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
