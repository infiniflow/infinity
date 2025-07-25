# Libdwarf needs zstd, cpptrace doesn't, and libdwarf has its own Findzstd but it doesn't define zstd::libzstd_static /
# zstd::libzstd_shared targets which leads to issues, necessitating a find_dependency(zstd) in cpptrace's cmake config
# and in order to support non-cmake-module installs we need to provide a Findzstd script.
# https://github.com/jeremy-rifkin/cpptrace/issues/112

# This will define
# zstd_FOUND
# zstd_INCLUDE_DIR
# zstd_LIBRARY

find_path(zstd_INCLUDE_DIR NAMES zstd.h)

find_library(zstd_LIBRARY_DEBUG NAMES zstdd zstd_staticd)
find_library(zstd_LIBRARY_RELEASE NAMES zstd zstd_static)

include(SelectLibraryConfigurations)
SELECT_LIBRARY_CONFIGURATIONS(zstd)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  zstd DEFAULT_MSG
  zstd_LIBRARY zstd_INCLUDE_DIR
)

if(zstd_FOUND)
  message(STATUS "Found Zstd: ${zstd_LIBRARY}")
endif()

mark_as_advanced(zstd_INCLUDE_DIR zstd_LIBRARY)

if(zstd_FOUND)
  # just defining them the same... cmake will figure it out
  if(NOT TARGET zstd::libzstd_static)
    add_library(zstd::libzstd_static UNKNOWN IMPORTED)
    set_target_properties(
      zstd::libzstd_static
      PROPERTIES
      IMPORTED_LOCATION "${zstd_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${zstd_INCLUDE_DIR}"
    )
  endif()
  if(NOT TARGET zstd::libzstd_shared)
    add_library(zstd::libzstd_shared UNKNOWN IMPORTED)
    set_target_properties(
      zstd::libzstd_shared
      PROPERTIES
      IMPORTED_LOCATION "${zstd_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${zstd_INCLUDE_DIR}"
    )
  endif()
endif()
