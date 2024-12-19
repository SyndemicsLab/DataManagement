message(STATUS "Installing datamanagement")
set(project_config_in "${CMAKE_CURRENT_LIST_DIR}/datamanagementConfig.cmake.in")
set(project_config_out "${CMAKE_CURRENT_BINARY_DIR}/datamanagementConfig.cmake")
set(config_targets_file "datamanagementConfigTargets.cmake")
set(version_config_file "${CMAKE_CURRENT_BINARY_DIR}/datamanagementConfigVersion.cmake")
set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/cmake/datamanagement")
set(pkgconfig_install_dir "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
set(pkg_config "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc")

# ---------------------------------------------------------------------------------------
# Include files
# ---------------------------------------------------------------------------------------
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(TARGETS datamanagement EXPORT datamanagement)

# ---------------------------------------------------------------------------------------
# Install pkg-config file
# ---------------------------------------------------------------------------------------
if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    set(PKG_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
else()
    set(PKG_CONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()
if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(PKG_CONFIG_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
else()
    set(PKG_CONFIG_LIBDIR "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()
get_target_property(PKG_CONFIG_DEFINES datamanagement INTERFACE_COMPILE_DEFINITIONS)
string(REPLACE ";" " -D" PKG_CONFIG_DEFINES "${PKG_CONFIG_DEFINES}")
string(CONCAT PKG_CONFIG_DEFINES "-D" "${PKG_CONFIG_DEFINES}")
configure_file("cmake/${PROJECT_NAME}.pc.in" "${pkg_config}" @ONLY)
install(FILES "${pkg_config}" DESTINATION "${pkgconfig_install_dir}")

# ---------------------------------------------------------------------------------------
# Install CMake config files
# ---------------------------------------------------------------------------------------
export(TARGETS datamanagement NAMESPACE datamanagement::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${config_targets_file}")
install(EXPORT datamanagement DESTINATION ${export_dest_dir} NAMESPACE datamanagement:: FILE ${config_targets_file})

include(CMakePackageConfigHelpers)
configure_package_config_file("${project_config_in}" "${project_config_out}" INSTALL_DESTINATION ${export_dest_dir})

write_basic_package_version_file("${version_config_file}" COMPATIBILITY SameMajorVersion)
install(FILES "${project_config_out}" "${version_config_file}" DESTINATION "${export_dest_dir}")

# ---------------------------------------------------------------------------------------
# Support creation of installable packages
# ---------------------------------------------------------------------------------------
include(cmake/datamanagementCPack.cmake)