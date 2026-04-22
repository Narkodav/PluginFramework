#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibraryLoader::LibraryLoader" for configuration "Debug"
set_property(TARGET LibraryLoader::LibraryLoader APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LibraryLoader::LibraryLoader PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libLibraryLoader.a"
  )

list(APPEND _cmake_import_check_targets LibraryLoader::LibraryLoader )
list(APPEND _cmake_import_check_files_for_LibraryLoader::LibraryLoader "${_IMPORT_PREFIX}/lib/libLibraryLoader.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
