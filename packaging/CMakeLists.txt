include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED DataManagement_INSTALL_CMAKEDIR)
   set(DataManagement_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/DataManagement"
       CACHE STRING "Path to DataManagement CMake files")
endif ()

install(TARGETS DataManagement EXPORT DataManagement_Targets
    RUNTIME COMPONENT DataManagement_Runtime
    LIBRARY COMPONENT DataManagement_Runtime
    NAMELINK_COMPONENT DataManagement_Development
    ARCHIVE COMPONENT DataManagement_Development
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${DataManagement_SOURCE_DIR}/include/" "${DataManagement_BINARY_DIR}/include/"
    TYPE INCLUDE
    COMPONENT DataManagement_Development)

if(BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif()

install(EXPORT DataManagement_Targets
    DESTINATION "${DataManagement_INSTALL_CMAKEDIR}"
    NAMESPACE DataManagement::
    FILE DataManagement-${type}-targets.cmake
    COMPONENT DataManagement_Development)

write_basic_package_version_file(
    DataManagementConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/DataManagementConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DataManagementConfigVersion.cmake"
    DESTINATION "${DataManagement_INSTALL_CMAKEDIR}"
    COMPONENT DataManagement_Development)