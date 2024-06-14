cmake_minimum_required(VERSION 3.20)

project(
    DataManagement
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By Syndemics Lab Simulation Models"
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS filesystem)

option(BUILD_DATA_TESTS "enable Data tests" OFF)
if(BUILD_DATA_TESTS STREQUAL ON)
    add_subdirectory(test)
endif()

add_library(
    ${PROJECT_NAME} SHARED
    "src/DataTable.cpp"
    "src/Configuration.cpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::filesystem)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT DataManagementTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    FILES
        "include/Configuration.hpp"
        "include/DataManagement.hpp"
        "include/DataTable.hpp"
        "include/Reader.hpp"
        "include/Writer.hpp"
    DESTINATION include
)

install(
    EXPORT DataManagementTargets
    FILE DataManagementTargets.cmake
    NAMESPACE DataManagement::
    DESTINATION lib/cmake/DataManagement
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DataManagement
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake"
    DESTINATION lib/cmake/DataManagement
)

set(version 1.0.0)
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${version})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 1)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_DataManagement_MAJOR_VERSION 1)
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING DataManagement_MAJOR_VERISON)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)
