#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::curlpp::curlpp" for configuration "Release"
set_property(TARGET unofficial::curlpp::curlpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(unofficial::curlpp::curlpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "CURL::libcurl"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libcurlpp.a"
  )

list(APPEND _cmake_import_check_targets unofficial::curlpp::curlpp )
list(APPEND _cmake_import_check_files_for_unofficial::curlpp::curlpp "${_IMPORT_PREFIX}/lib/libcurlpp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
