cmake_minimum_required(VERSION 3.23)

project(
  DataManagement
  VERSION 1.0.0
  DESCRIPTION "A Data Management Library Used By Syndemics Lab Simulation Models"
)

find_package(SQLite3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

option(BUILD_TESTS "enable Data tests" OFF)
if(BUILD_TESTS STREQUAL ON)
  add_subdirectory(tests)
endif()

option(BUILD_STATIC_LIBRARY "build static library" ON)
if(BUILD_STATIC_LIBRARY STREQUAL ON)
  add_library(${PROJECT_NAME} STATIC)
  set(PRIVATE_SOURCE_LIST
    ${PROJECT_SOURCE_DIR}/src/DataManager.cpp
  )

  set(PUBLIC_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/include/DataManager.hpp
  )

  target_sources(${PROJECT_NAME}
    PUBLIC
    FILE_SET headers TYPE HEADERS FILES ${PUBLIC_HEADER_LIST}
    PRIVATE
    ${PRIVATE_SOURCE_LIST}
  )

  target_link_libraries(
    ${PROJECT_NAME}
    SQLite::SQLite3
  )

  # target_sources(${PROJECT_NAME} PUBLIC FILE_SET headers TYPE HEADERS)

  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Sources FILES ${PRIVATE_SOURCE_LIST})
  source_group(TREE ${PROJECT_SOURCE_DIR}/include/ PREFIX Interface FILES ${PUBLIC_HEADER_LIST})

  ##################################################################################

  # CMake will install by default the files C:/Program Files/MyLibrary,
  # but we don't want pollute C:/Program Files, so we'll put the files
  # somewhere in the project folders.
  if(DEFINED CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(
      STATUS
      "CMAKE_INSTALL_PREFIX is not set\n"
      "Default value: ${CMAKE_INSTALL_PREFIX}\n"
      "Will set it to ${CMAKE_SOURCE_DIR}/install"
    )
    set(CMAKE_INSTALL_PREFIX
      "${CMAKE_SOURCE_DIR}/../RESPONDSimulationv2/lib/dminstall"
      CACHE PATH "Where the library will be installed to" FORCE
    )
  else()
    message(
      STATUS
      "CMAKE_INSTALL_PREFIX was already set\n"
      "Current value: ${CMAKE_INSTALL_PREFIX}"
    )
  endif()

  include(GNUInstallDirs)  # for CMAKE_INSTALL_INCLUDEDIR definition
  set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}")

  target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
  )
else()
  add_library(${PROJECT_NAME} SHARED
    "src/DataManager.cpp"
  )
  target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
  )
  target_link_libraries(
    ${PROJECT_NAME}
    SQLite::SQLite3
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
    "include/DataManager.hpp"
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
endif()




# To install the public header files, we cannot just set the PUBLIC_HEADER property on these
# files,and expect that the install() command will do correctly the job by using the parameter
# PUBLIC_HEADER DESTINATION: the file structure will not be respected.
# So instead we iterate through public headers and install them "manually".
# Note : May be obsolete since the introduction of File sets (see target_sources())

# install(TARGETS ${PROJECT_NAME} FILE_SET HEADERS EXPORT ${PROJECT_NAME}Targets)

# Add a suffix for debug versions
# set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

# install(TARGETS ${PROJECT_NAME}
#     EXPORT "${PROJECT_NAME}Targets"
#     FILE_SET headers
#     # These get default values from GNUInstallDirs, no need to set them
#     # RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
#     # LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
#     # ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
#     # except for public headers, as we want them to be inside a library folder
#     # PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} # include/MyLibrary
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include/MyLibrary
# )

# install(EXPORT "${PROJECT_NAME}Targets"
#     FILE "${PROJECT_NAME}Targets.cmake"
#     NAMESPACE ${namespace}::
#     DESTINATION cmake)

# include(CMakePackageConfigHelpers)

# set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/dminstall/include
#     CACHE PATH "Location of header files" )
# set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/dminstall/lib
#     CACHE PATH "Location of library files" )

# configure_package_config_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
#     ${PROJECT_NAME}Config.cmake
#     INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
#     PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
# )

# write_basic_package_version_file(
#     ${PROJECT_NAME}Config-version.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY AnyNewerVersion
# )

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
#                 ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config-version.cmake
#     DESTINATION cmake)

# message(STATUS "CMAKE_CURRENT_LIST_DIR : ${CMAKE_CURRENT_LIST_DIR}")



##################################################################################
