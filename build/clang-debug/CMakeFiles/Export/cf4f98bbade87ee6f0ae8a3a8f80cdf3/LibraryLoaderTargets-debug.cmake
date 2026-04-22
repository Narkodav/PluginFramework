#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PluginFramework::PluginFramework" for configuration "Debug"
set_property(TARGET PluginFramework::PluginFramework APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PluginFramework::PluginFramework PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libPluginFramework.a"
  )

list(APPEND _cmake_import_check_targets PluginFramework::PluginFramework )
list(APPEND _cmake_import_check_files_for_PluginFramework::PluginFramework "${_IMPORT_PREFIX}/lib/libPluginFramework.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
