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

add_library(DataManagement SHARED)
target_sources(DataManagement 
    PRIVATE 
        "${CMAKE_SOURCE_DIR}/src/DataTable.cpp"
        "${CMAKE_SOURCE_DIR}/src/Configuration.cpp"
        "${CMAKE_SOURCE_DIR}/src/SQLite3.cpp"
        "${CMAKE_SOURCE_DIR}/lib/sqlite3/sqlite3.c"
    PUBLIC
        FILE_SET publicheaders 
        TYPE HEADERS
        FILES
            "${CMAKE_SOURCE_DIR}/include/Configuration.hpp"
            "${CMAKE_SOURCE_DIR}/include/DataManagement.hpp"
            "${CMAKE_SOURCE_DIR}/include/DataTable.hpp"
            "${CMAKE_SOURCE_DIR}/include/Reader.hpp"
            "${CMAKE_SOURCE_DIR}/include/SQLite3.hpp"
            "${CMAKE_SOURCE_DIR}/include/Writer.hpp"
)


add_library(DataManagement::DataManagement ALIAS DataManagement)

target_include_directories(DataManagement 
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/lib/sqlite3" 
        ${Boost_INCLUDE_DIRS})
target_link_libraries(DataManagement Boost::filesystem)

option(BUILD_DATA_TESTS "enable Data tests" OFF)

if(BUILD_DATA_TESTS STREQUAL ON)
    add_subdirectory(test)
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} 
    FILE_SET publicheaders
    CONFIGURATIONS "DataManagementConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "DataManagementConfigVersion.cmake"
    VERSION ${DataManagement_VERSION}
    COMPATIBILITY AnyNewerVersion)

# install(TARGETS DataManagement 
#     EXPORT DataManagementTargets
#     FILE_SET HEADERS
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION include)

# install(EXPORT DataManagementTargets
#     FILE DataManagementTargets.cmake
#     NAMESPACE DataManagement::
#     DESTINATION lib/cmake/DataManagement)

# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     "DataManagementConfigVersion.cmake"
#     VERSION ${DataManagement_VERSION}
#     COMPATIBILITY AnyNewerVersion
# )

# install(FILES "DataManagementConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake"
#     DESTINATION lib/cmake/DataManagement)
