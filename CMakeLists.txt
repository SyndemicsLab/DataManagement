cmake_minimum_required(VERSION 3.20)

project(
    DataManagement
    LANGUAGES CXX
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(SQLite3 REQUIRED)

add_library(DataManagement SHARED)
target_sources(DataManagement 
    PRIVATE 
        "${CMAKE_SOURCE_DIR}/src/DataTable.cpp"
        "${CMAKE_SOURCE_DIR}/src/Configuration.cpp"
        "${CMAKE_SOURCE_DIR}/src/SQLite3.cpp"
    PUBLIC
        FILE_SET publicheaders 
        TYPE HEADERS
        FILES
            "${CMAKE_SOURCE_DIR}/include/Configuration.hpp"
            "${CMAKE_SOURCE_DIR}/include/DataManagement.hpp"
            "${CMAKE_SOURCE_DIR}/include/DataTable.hpp"
            "${CMAKE_SOURCE_DIR}/include/Reader.hpp"
            "${CMAKE_SOURCE_DIR}/include/Writer.hpp"
            "${CMAKE_SOURCE_DIR}/include/SQLite3.hpp"
)

target_include_directories(DataManagement 
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        ${SQLite3_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})
target_link_libraries(DataManagement Boost::filesystem sqlite3)

add_library(DataManagement::DataManagement ALIAS DataManagement)

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS STREQUAL ON)
    add_subdirectory(test)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "DataManagementConfigVersion.cmake"
    VERSION ${DataManagement_VERSION}
    COMPATIBILITY AnyNewerVersion)

set(INSTALL_DIR "lib/DataManagement/cmake")
set(INCLUDE_DIR "${INSTALL_DIR}/include")

configure_package_config_file(
    DataManagementConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
    INSTALL_DESTINATION ${INSTALL_DIR}
    PATH_VARS INCLUDE_DIR
)

include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME} 
    FILE_SET publicheaders
    DESTINATION ${INSTALL_DIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfig.cmake
    DESTINATION ${INSTALL_DIR}
)
