# extracting curlpp version text + version num
file (READ ${CMAKE_CURRENT_SOURCE_DIR}/../include/curlpp/cURLpp.hpp CURLPP_VERSION_H_CONTENTS)

string (REGEX MATCH "#define LIBCURLPP_VERSION \"[^\"]*"
  CURLPP_VERSION ${CURLPP_VERSION_H_CONTENTS})
string (REGEX REPLACE "[^\"]+\"" "" CURLPP_VERSION ${CURLPP_VERSION})
string (REGEX MATCH "#define LIBCURLPP_VERSION_NUM 0x[0-9a-fA-F]+"
  CURLPP_VERSION_NUM ${CURLPP_VERSION_H_CONTENTS})
string (REGEX REPLACE "[^0]+0x" "" CURLPP_VERSION_NUM ${CURLPP_VERSION_NUM})

message(STATUS "curlpp version=[${CURLPP_VERSION}]")
message(STATUS "curlpp version num=[${CURLPP_VERSION_NUM}]")


# curlpp-config needs the following options to be set.
string(REPLACE "." "" libext "${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(CC "${CMAKE_C_COMPILER}")
set(libdir "${CMAKE_INSTALL_LIBDIR}")
set(libdir_static "${CMAKE_INSTALL_LIBDIR}")
set(includedir "${CMAKE_INSTALL_INCLUDEDIR}")
set(prefix "${CMAKE_INSTALL_PREFIX}")

set(LDFLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

# Finally generate a "curl-config" matching this config
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/curlpp-config.in"
               "${CMAKE_CURRENT_BINARY_DIR}/curlpp-config" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/curlpp-config"
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE
          GROUP_READ GROUP_EXECUTE
          WORLD_READ WORLD_EXECUTE)


# Finally generate a pkg-config file matching this config
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/curlpp.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/curlpp.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/curlpp.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
