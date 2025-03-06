cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(utils)
datamanagement_extract_version()

project(datamanagement VERSION ${DATAMANAGEMENT_VERSION} LANGUAGES CXX)

message(STATUS "Build datamanagement: ${DATAMANAGEMENT_VERSION}")

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
# Set DATAMANAGEMENT_MASTER_PROJECT to ON if we are building datamanagement directly
# ---------------------------------------------------------------------------------------
if(NOT DEFINED DATAMANAGEMENT_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(DATAMANAGEMENT_MASTER_PROJECT ON)
    else()
        set(DATAMANAGEMENT_MASTER_PROJECT OFF)
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
# Building the Library
# ---------------------------------------------------------------------------------------
set(DATAMANAGEMENT_SRCS 
    src/Configuration.cpp 
    src/DataTable.cpp
)

if(BUILD_SHARED_LIBS)
    if(WIN32)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
        list(APPEND DATAMANAGEMENT_SRCS ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
    add_library(datamanagement SHARED ${DATAMANAGEMENT_SRCS} ${DATAMANAGEMENT_ALL_HEADERS})
    target_compile_definitions(datamanagement PUBLIC DATAMANAGEMENT_SHARED_LIB)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(datamanagement PUBLIC $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/wd4251
                                             /wd4275>)
    endif()
else()
    add_library(datamanagement STATIC ${DATAMANAGEMENT_SRCS} ${DATAMANAGEMENT_ALL_HEADERS})
endif()

# ---------------------------------------------------------------------------------------
# Alias for importing
# ---------------------------------------------------------------------------------------
add_library(datamanagement::datamanagement ALIAS datamanagement)

set(DATAMANAGEMENT_INCLUDES_LEVEL "")
if(DATAMANAGEMENT_SYSTEM_INCLUDES)
    set(DATAMANAGEMENT_INCLUDES_LEVEL "SYSTEM")
endif()

target_compile_definitions(datamanagement PUBLIC DATAMANAGEMENT_COMPILED_LIB)
target_include_directories(datamanagement ${DATAMANAGEMENT_INCLUDES_LEVEL} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                                                                  PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>")

include(MakeDependenciesAvailable)
                                                                  
target_link_libraries(datamanagement
    PRIVATE
        Boost::boost
)

set_target_properties(datamanagement PROPERTIES VERSION ${DATAMANAGEMENT_VERSION} SOVERSION ${DATAMANAGEMENT_VERSION_MAJOR}.${DATAMANAGEMENT_VERSION_MINOR})

include(BuildBinaries)

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
if(DATAMANAGEMENT_INSTALL)
    include(InstallDatamanagement)
endif()
