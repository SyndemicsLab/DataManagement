cmake_minimum_required(VERSION 3.20)

project(
    DataManagement
    LANGUAGES CXX
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS STREQUAL ON)
    add_subdirectory(test)
endif()

if (DEFINED DataManagement_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${DataManagement_SHARED_LIBS}")
endif ()

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(SQLite3 REQUIRED)

add_library(DataManagement SHARED
    "src/DataTable.cpp"
    "src/Configuration.cpp"
    "src/SQLite3.cpp"
    "include/Configuration.hpp"
    "include/DataManagement.hpp"
    "include/DataTable.hpp"
    "include/Reader.hpp"
    "include/Writer.hpp"
    "include/SQLite3.hpp"
)

add_library(DataManagement::DataManagement ALIAS DataManagement)

set_target_properties(DataManagement PROPERTIES
                        VERSION ${DataManagement_VERSION}
                        SOVERSION ${DataManagement_VERSION_MAJOR})

target_link_libraries(DataManagement PRIVATE Boost::filesystem sqlite3)

# Add definitions for targets
# Values:
#   * Debug: -DFOO_BAR_DEBUG=1
#   * Release: -DFOO_BAR_DEBUG=0
#   * other: -DFOO_BAR_DEBUG=0
target_compile_definitions(DataManagement PUBLIC "DATAMANAGEMENT_DEBUG=$<CONFIG:Debug>")

# Global includes. Used by all targets
# Note:
#   * header location in project: Foo/Source/foo/Bar.hpp
#   * header can be included by C++ code `#include <foo/Bar.hpp>`
#   * header location in project: ${CMAKE_CURRENT_BINARY_DIR}/generated_headers/foo/BAR_EXPORT.h
#   * header can be included by: `#include <BAR_EXPORT.h>`
target_include_directories(DataManagement 
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        ${SQLite3_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)

target_compile_features(DataManagement PUBLIC cxx_std_17)

# https://cmake.org/cmake/help/v3.9/module/GenerateExportHeader.html
# include(GenerateExportHeader)
# generate_export_header(DataManagement EXPORT_FILE_NAME include/DataManagement/export.h)
target_compile_definitions(
    DataManagement PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:DATAMANAGEMENT_STATIC_DEFINE>")

target_include_directories(
    DataManagement PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
option(DataManagement_INCLUDE_PACKAGING "Include packaging rules for DataManagement" "${is_top_level}")
if (DataManagement_INCLUDE_PACKAGING)
    add_subdirectory(packaging)
endif ()
