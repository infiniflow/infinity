project(curlpp) 


# In response to CMake 3.0 generating warnings regarding policy CMP0042,
# the OSX RPATH settings have been updated per recommendations found
# in the CMake Wiki:
#  http://www.cmake.org/Wiki/CMake_RPATH_handling#Mac_OS_X_and_the_RPATH
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # MACOSX_RPATH
    set(CMAKE_MACOSX_RPATH TRUE)
endif()

# for unix platform, define install directories.
include(GNUInstallDirs)

if(WIN32)
    # cmake 3.4 is required for CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    cmake_minimum_required(VERSION 3.4)

    # c++ 11 support from cmake 3.4 or newer
    set(CMAKE_CXX_STANDARD 11) # C++11...
    set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
    set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  else()
    if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_LESS 3.1)
        cmake_minimum_required(VERSION 2.8)
        # c++11 support for cmake 2.8.12 - 3.0.x
        #
        # for non-windows platform we try to keep cmake 2.8 support
        # since entreprise distribution tends to have 2.8 version.
        add_compile_options(-std=c++11)
    else()
        # c++ 11 support from cmake 3.1 or newer
        set(CMAKE_CXX_STANDARD 11) # C++11...
        set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
        set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
    endif()
endif()


# Conan.io integration
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

# extra (pkg-config-related files)
#add_subdirectory(extras)

#########################################################################################
# Look for dependencies

# Documented at https://cmake.org/cmake/help/v3.0/module/FindCURL.html?highlight=curlpp
# Seems simple.

#message(STATUS "Looking for CURL")
#include(FindCURL)
#find_package(CURL REQUIRED)
#
#if(CURL_FOUND)
#  message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
#  message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
#  message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
#else()
#  message(FATAL_ERROR "Could not find CURL")
#endif()

# All following targets should search these directories for headers
include_directories( 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CURL_INCLUDE_DIRS}
)

#########################################################################################
# Define Targets

# If building on windows, install path will be in build/winbuild
file(GLOB_RECURSE HeaderFileList "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
file(GLOB_RECURSE SourceFileList "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
add_library(${PROJECT_NAME} SHARED ${HeaderFileList} ${SourceFileList})
message(status "*********** ${PROJECT_NAME}")
target_link_libraries(${PROJECT_NAME} libcurl_static)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1 VERSION 1.0.0)

add_library(${PROJECT_NAME}_static STATIC ${HeaderFileList} ${SourceFileList})
target_include_directories(${PROJECT_NAME}_static PUBLIC "${CMAKE_SOURCE_DIR}/third_party/curl/include")
# Make sure that on unix-platforms shared and static libraries have
# the same root name, but different suffixes.
#
#  (solution taken from https://cmake.org/Wiki/CMake_FAQ#How_do_I_make_my_shared_and_static_libraries_have_the_same_root_name.2C_but_different_suffixes.3F)
# 
# Making shared and static libraries have the same root name, but different suffixes
SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
# Now the library target "curlpp_static" will be named "curlpp.lib" with MS tools.
# This conflicts with the "curlpp.lib" import library corresponding to "curlpp.dll",
# so we add a "lib" prefix (which is default on other platforms anyway):
SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES PREFIX "lib")
#target_link_libraries(${PROJECT_NAME}_static ${CURL_LIBRARIES} ${CONAN_LIBS})

# install headers
install(DIRECTORY include/utilspp/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/utilspp")
install(DIRECTORY include/curlpp/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/curlpp")

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

