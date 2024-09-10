#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::curlpp::curlpp" for configuration "Debug"
set_property(TARGET unofficial::curlpp::curlpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::curlpp::curlpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "CURL::libcurl"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libcurlpp.a"
  )

list(APPEND _cmake_import_check_targets unofficial::curlpp::curlpp )
list(APPEND _cmake_import_check_files_for_unofficial::curlpp::curlpp "${_IMPORT_PREFIX}/debug/lib/libcurlpp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
