if(${CMAKE_C_COMPILER_ID} MATCHES "Intel") # icc / icpc
  # prevent shared libraries from depending on Intel provided libraries
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-intel")
endif()


# we default on a shared library.
if(ROARING_BUILD_STATIC)
  set(ROARING_LIB_TYPE STATIC)
  MESSAGE( STATUS "Building a static library." )
else()
  MESSAGE( STATUS "Building a dynamic library." )
  set(ROARING_LIB_TYPE SHARED)
endif()

MESSAGE( STATUS "ROARING_LIB_TYPE: " ${ROARING_LIB_TYPE})
set(ROARING_SRC
    array_util.c
    bitset_util.c
    containers/array.c
    containers/bitset.c
    containers/containers.c
    containers/convert.c
    containers/mixed_intersection.c
    containers/mixed_union.c
    containers/mixed_equal.c
    containers/mixed_subset.c
    containers/mixed_negation.c
    containers/mixed_xor.c
    containers/mixed_andnot.c
    containers/run.c
    memory.c
    roaring.c
    roaring_priority_queue.c
    roaring_array.c)

if(ROARING_BUILD_C_AS_CPP)  # more checks and tools, e.g. <type_traits> analysis 
  SET_SOURCE_FILES_PROPERTIES(${ROARING_SRC} PROPERTIES LANGUAGE CXX)
endif()

add_library(${ROARING_LIB_NAME} ${ROARING_LIB_TYPE} ${ROARING_SRC})
target_include_directories(${ROARING_LIB_NAME}
  PUBLIC
   $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${ROARING_LIB_NAME} PUBLIC roaring-headers)
target_link_libraries(${ROARING_LIB_NAME} PUBLIC roaring-headers-cpp)
#
#install(TARGETS ${ROARING_LIB_NAME} DESTINATION lib)
#
install(TARGETS ${ROARING_LIB_NAME} 
   EXPORT ${ROARING_LIB_NAME}-config
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   INCLUDES DESTINATION ${CMAKE_INSTALL_INCDIR}
)
install(EXPORT ${ROARING_LIB_NAME}-config
   FILE ${ROARING_LIB_NAME}-config.cmake
   NAMESPACE ${ROARING_LIB_NAME}::
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${ROARING_LIB_NAME}
 )

if(NOT MSVC)
## We output the library at the root of the current directory where cmake is invoked
## This is handy but Visual Studio will happily ignore us
set_target_properties(${ROARING_LIB_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  VERSION ${ROARING_LIB_VERSION}
  SOVERSION ${ROARING_LIB_SOVERSION})
MESSAGE( STATUS "Library output directory (does not apply to Visual Studio): " ${CMAKE_BINARY_DIR})
endif()

if(MSVC AND (ROARING_LIB_TYPE STREQUAL "SHARED"))
  if (CMAKE_VERSION VERSION_LESS 3.4)
    MESSAGE( STATUS "To build  a Windows DLL using Visual Studio, you may need cmake 3.4 or better." )
  endif()
  MESSAGE( STATUS "Building a Windows DLL using Visual Studio, exporting all symbols automatically." )
 set_target_properties(${ROARING_LIB_NAME}
    PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif()
