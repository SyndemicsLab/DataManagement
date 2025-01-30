cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(utils)
datamanagement_extract_version()

project(
  datamanagement
  VERSION ${DATAMANAGEMENT_VERSION}
  DESCRIPTION "A Data Management Library Used By Syndemics Lab Simulation Models"
  LANGUAGES CXX
)

message(STATUS "Build data management: ${DATAMANAGEMENT_VERSION}")

include(GNUInstallDirs)

# ---------------------------------------------------------------------------------------
# Set default build to release
# ---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# ---------------------------------------------------------------------------------------
# Compiler config
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------------------
# Windows CXX Extensions
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

# ---------------------------------------------------------------------------------------
# Set  DATAMANAGEMENT_MASTER_PROJECT to ON if we are building synmodels directly
# ---------------------------------------------------------------------------------------
if(NOT DEFINED  DATAMANAGEMENT_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set( DATAMANAGEMENT_MASTER_PROJECT ON)
    else()
        set( DATAMANAGEMENT_MASTER_PROJECT OFF)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# Set all default options
# ---------------------------------------------------------------------------------------
include(options)

# ---------------------------------------------------------------------------------------
# Set position independent code
# ---------------------------------------------------------------------------------------
if(DATAMANAGEMENT_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# ---------------------------------------------------------------------------------------
# Build the library
# ---------------------------------------------------------------------------------------
add_library(datamanagement INTERFACE)
add_library(datamanagement::datamanagement ALIAS datamanagement)

option(BUILD_TESTS "enable datamanagement unit tests" OFF)
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ---------------------------------------------------------------------------------------
# Link Dependencies
# ---------------------------------------------------------------------------------------
include(LoadSqlite)
include(LoadBoost)
include(LoadEigen)
include(LoadSpdlog)
include(LoadGlaze)

target_link_libraries(${PROJECT_NAME} 
    INTERFACE 
        SQLite::SQLite3
        Boost::boost
        spdlog::spdlog
        Eigen3::Eigen
        glaze::glaze
)

set_target_properties(datamanagement PROPERTIES VERSION ${DATAMANAGEMENT_VERSION} SOVERSION ${DATAMANAGEMENT_VERSION_MAJOR}.${DATAMANAGEMENT_VERSION_MINOR})

# ---------------------------------------------------------------------------------------
# Build Tests
# ---------------------------------------------------------------------------------------
if(DATAMANAGEMENT_BUILD_TESTS)
    message(STATUS "Generating tests")
    enable_testing()
    add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
if(DATAMANAGEMENT_INSTALL)
    include(InstallDatamanagement)
endif()