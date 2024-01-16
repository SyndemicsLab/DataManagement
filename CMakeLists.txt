cmake_minimum_required(VERSION 3.20)

project(
    DataManagement
    LANGUAGES CXX
    VERSION 1.0.0
    DESCRIPTION "A Data Management Library Used By RESPOND and HEP-CE Models"
)

# Debug Information Format:
# * https://docs.microsoft.com/en-us/cpp/build/reference/z7-zi-zi-debug-information-format
#
# Notes:
#
# * /Z7 still produce PDB file for DLL and without the PDB file installed
#   you can't debug DLL
#
# * /Z7 for static library doesn't produce PDB. It's the best option if you
#   want debug library without changing internal CMake code.
#   Toolchain example: https://github.com/ruslo/polly/blob/master/vs-15-2017-win64-z7.cmake
#
# * /Zi option is default (produce separate PDB files)
#
# * TARGET_PDB_FILE generator expression doesn't support static libraries.
#   See https://gitlab.kitware.com/cmake/cmake/issues/16932
#   (that's why it's not used here)
#
# * This code can be implemented as a 'PDB DESTINATION' feature.
#   See https://gitlab.kitware.com/cmake/cmake/issues/16935#note_275180
#
# * By default only Debug/RelWithDebInfo produce debug information,
#   Release/MinSizeRel do not.
#
# * Generated PDB for static libraries doesn't respect CMAKE_<CONFIG>_POSTFIX
#   variable. It means if you specify Debug and RelWithDebInfo then generated
#   PDB files for both will be "md5.pdb". When PDB files installed one will
#   overwrite another making it unusable. Release + Debug configurations will
#   work fine because Release doesn't produce PDB files.
#
# * All PDB files will be installed, including PDB for targets that will not
#   be installed themselves.

if(MSVC)
  set(pdb_output_dir "${CMAKE_CURRENT_BINARY_DIR}/pdb-files")

  set(CMAKE_PDB_OUTPUT_DIRECTORY "${pdb_output_dir}")
  set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${pdb_output_dir}")

  get_cmake_property(is_multi GENERATOR_IS_MULTI_CONFIG)
  if(is_multi)
    set(config_suffix "$<CONFIG>")
  else()
    set(config_suffix "")
  endif()

  # Introduce variables:
  # * CMAKE_INSTALL_LIBDIR
  # * CMAKE_INSTALL_BINDIR
  include(GNUInstallDirs)

  if(BUILD_SHARED_LIBS)
    set(pdb_dst ${CMAKE_INSTALL_BINDIR})
  else()
    set(pdb_dst ${CMAKE_INSTALL_LIBDIR})
  endif()

  install(
      DIRECTORY "${pdb_output_dir}/${config_suffix}/"
      DESTINATION ${pdb_dst}
  )
endif()

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS STREQUAL ON)
    add_subdirectory(test)
endif()

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
# target_sources(DataManagement 
#     PRIVATE 
#         "${CMAKE_SOURCE_DIR}/src/DataTable.cpp"
#         "${CMAKE_SOURCE_DIR}/src/Configuration.cpp"
#         "${CMAKE_SOURCE_DIR}/src/SQLite3.cpp"
#     PUBLIC
#         FILE_SET publicheaders 
#         TYPE HEADERS
#         FILES
#             "${CMAKE_SOURCE_DIR}/include/Configuration.hpp"
#             "${CMAKE_SOURCE_DIR}/include/DataManagement.hpp"
#             "${CMAKE_SOURCE_DIR}/include/DataTable.hpp"
#             "${CMAKE_SOURCE_DIR}/include/Reader.hpp"
#             "${CMAKE_SOURCE_DIR}/include/Writer.hpp"
#             "${CMAKE_SOURCE_DIR}/include/SQLite3.hpp"
# )

target_compile_definitions(DataManagement PUBLIC "DATAMANAGEMENT_DEBUG=$<CONFIG:Debug>")

set(generated_headers "${CMAKE_CURRENT_BINARY_DIR}/generated_headers")
set(datamanagement_export "${generated_headers}/DataManagement/DATAMANAGEMENT_EXPORT.h")

include(GenerateExportHeader)
generate_export_header(DataManagement EXPORT_FILE_NAME ${datamanagement_export})

target_include_directories(DataManagement 
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<BUILD_INTERFACE:${generated_headers}>"
    PRIVATE
        ${SQLite3_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(DataManagement PUBLIC Boost::filesystem sqlite3)

# Installation (https://github.com/forexample/package-example)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)


# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * <prefix>/lib/libDataManagement.so
#   * header location after install: <prefix>/include/DataManagement/DataManagement.hpp
#   * headers can be included by C++ code `#include <DataManagement/DataManagement.hpp>`
install(
    TARGETS DataManagement
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Headers:
#   * include/DataManagement.hpp -> <prefix>/include/DataManagement/include/DataManagement.hpp
install(
    FILES 
        "include/Configuration.hpp"
        "include/DataManagement.hpp"
        "include/DataTable.hpp"
        "include/Reader.hpp"
        "include/Writer.hpp"
        "include/SQLite3.hpp"
        "${datamanagement_export}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/DataManagement"
)

# Config
#   * <prefix>/lib/cmake/Foo/FooConfig.cmake
#   * <prefix>/lib/cmake/Foo/FooConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/Foo/FooTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
