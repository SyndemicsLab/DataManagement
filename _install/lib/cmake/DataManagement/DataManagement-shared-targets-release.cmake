#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DataManagement::DataManagement" for configuration "Release"
set_property(TARGET DataManagement::DataManagement APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(DataManagement::DataManagement PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libDataManagement.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libDataManagement.so.1"
  )

list(APPEND _cmake_import_check_targets DataManagement::DataManagement )
list(APPEND _cmake_import_check_files_for_DataManagement::DataManagement "${_IMPORT_PREFIX}/lib/libDataManagement.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
