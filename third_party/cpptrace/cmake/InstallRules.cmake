include(CMakePackageConfigHelpers)

# copy header files to CMAKE_INSTALL_INCLUDEDIR
# don't include third party header files
install(
  DIRECTORY
  "${PROJECT_SOURCE_DIR}/include/"     # our header files
  "${PROJECT_BINARY_DIR}/include/"     # generated header files
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  COMPONENT ${package_name}_development
  # PATTERN "**/third_party"    EXCLUDE  # skip third party directory
  # PATTERN "**/third_party/**" EXCLUDE  # skip third party files
)

# copy target build output artifacts to OS dependent locations
# (Except includes, that just sets a compiler flag with the path)
install(
  TARGETS ${target_name}
  EXPORT ${package_name}-targets
  RUNTIME #
  COMPONENT ${package_name}_runtime
  LIBRARY #
  COMPONENT ${package_name}_runtime
  NAMELINK_COMPONENT ${package_name}_development
  ARCHIVE #
  COMPONENT ${package_name}_development
  INCLUDES #
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# create config file that points to targets file
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/in/cpptrace-config-cmake.in"
  "${PROJECT_BINARY_DIR}/cmake/${package_name}-config.cmake"
  @ONLY
)

# copy config file for find_package to find
install(
  FILES "${PROJECT_BINARY_DIR}/cmake/${package_name}-config.cmake"
  DESTINATION "${CPPTRACE_INSTALL_CMAKEDIR}"
  COMPONENT ${package_name}_development
)

# create version file for consumer to check version in CMake
write_basic_package_version_file(
  "${package_name}-config-version.cmake"
  COMPATIBILITY SameMajorVersion  # a.k.a SemVer
)

# copy version file for find_package to find for version check
install(
  FILES "${PROJECT_BINARY_DIR}/${package_name}-config-version.cmake"
  DESTINATION "${CPPTRACE_INSTALL_CMAKEDIR}"
  COMPONENT ${package_name}_development
)

# create targets file included by config file with targets for consumers
install(
  EXPORT ${package_name}-targets
  NAMESPACE cpptrace::
  DESTINATION "${CPPTRACE_INSTALL_CMAKEDIR}"
  COMPONENT ${package_name}_development
)

if(CPPTRACE_PROVIDE_EXPORT_SET)
  export(
    TARGETS ${target_name}
    NAMESPACE cpptrace::
    FILE "${PROJECT_BINARY_DIR}/${package_name}-targets.cmake"
  )
endif()

# Findzstd.cmake
# vcpkg doesn't like anything being put in share/, which is where this goes apparently on their setup
if(NOT CPPTRACE_VCPKG)
  install(
    FILES "${PROJECT_SOURCE_DIR}/cmake/Findzstd.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_name}"
  )
endif()

# support packaging library
if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
