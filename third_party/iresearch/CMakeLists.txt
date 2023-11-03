project(iresearch)

# attach additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  option(IRESEARCH_DEBUG "Enable IRS_ASSERT and other checks" ON)
else ()
  option(IRESEARCH_DEBUG "Enable IRS_ASSERT and other checks" OFF)
endif ()


if (USE_VALGRIND)
  add_definitions(-DIRESEARCH_VALGRIND)
endif ()

################################################################################
### setup clang-tidy
################################################################################

unset(CLANG_TIDY_EXE CACHE)

if (USE_CLANG_TIDY)
  find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
  )

  if (CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  else ()
    message(STATUS "clang-tidy not found.")
  endif ()
endif ()

################################################################################
### find 3rd party libraries
################################################################################

# find liburing
if (USE_URING AND NOT MSVC)
  find_package(Uring)

  if (uring_FOUND)
    add_definitions(-DIRESEARCH_URING)
    set(LIBURING uring::uring)
  endif ()
endif ()

# set pthread library
if (NOT MSVC)
  set(PTHREAD_LIBRARY pthread)
endif ()

# set external dirs
set(EXTERNAL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/external
  CACHE INTERNAL
  ""
  )

set(IResearch_INCLUDE_DIR
  "${PROJECT_SOURCE_DIR}/core"
  CACHE INTERNAL
  ""
  )

add_subdirectory(external)
add_subdirectory(core)
