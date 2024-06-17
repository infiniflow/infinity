cmake_minimum_required(VERSION 3.15...3.27)
project(nanobind LANGUAGES NONE)

# ---------------------------------------------------------------------------
# Only build tests by default if this is the top-level CMake project
# ---------------------------------------------------------------------------

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(NB_MASTER_PROJECT ON)
else()
  set(NB_MASTER_PROJECT OFF)
endif()

option(NB_CREATE_INSTALL_RULES "Create installation rules" ${NB_MASTER_PROJECT})
option(NB_USE_SUBMODULE_DEPS   "Use the nanobind dependencies shipped as a git submodule of this repository" ON)

option(NB_TEST              "Compile nanobind tests?" ${NB_MASTER_PROJECT})
option(NB_TEST_STABLE_ABI   "Test the stable ABI interface?" OFF)
option(NB_TEST_SHARED_BUILD "Build a shared nanobind library for the test suite?" OFF)

if (NOT MSVC)
  option(NB_TEST_SANITZE    "Build tests with address and undefined behavior sanitizers?" OFF)
endif()

# ---------------------------------------------------------------------------
# Do a release build if nothing was specified
# ---------------------------------------------------------------------------

if (NB_MASTER_PROJECT AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "nanobind: setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# ---------------------------------------------------------------------------
# Check whether all dependencies are present
# ---------------------------------------------------------------------------

if (NB_USE_SUBMODULE_DEPS AND NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/robin_map/include")
  message(FATAL_ERROR "The nanobind dependencies are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by invoking\n$ git submodule update --init --recursive")
endif()

# ---------------------------------------------------------------------------
# Installation rules
# ---------------------------------------------------------------------------

if(NB_CREATE_INSTALL_RULES AND NOT CMAKE_SKIP_INSTALL_RULES)
  # Silence warning in GNUInstallDirs due to no enabled languages
  set(CMAKE_INSTALL_LIBDIR "")
  include(GNUInstallDirs)
  set(NB_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}/nanobind"
    CACHE PATH "Installation path for read-only architecture-independent nanobind data files")

  # Normally these would be configurable by the user, but we can't allow that
  # because the lookup paths are hard-coded in 'cmake/nanobind-config.cmake'
  set(NB_INSTALL_INC_DIR "${NB_INSTALL_DATADIR}/include")
  set(NB_INSTALL_SRC_DIR "${NB_INSTALL_DATADIR}/src")
  set(NB_INSTALL_EXT_DIR "${NB_INSTALL_DATADIR}/ext")
  set(NB_INSTALL_MODULE_DIR "${NB_INSTALL_DATADIR}")
  set(NB_INSTALL_CMAKE_DIR "${NB_INSTALL_DATADIR}/cmake")

  install(
    DIRECTORY include/
    DESTINATION "${NB_INSTALL_INC_DIR}"
  )

  install(
    DIRECTORY src/
    DESTINATION "${NB_INSTALL_SRC_DIR}"
    PATTERN "*.py" EXCLUDE
  )

  install(
    DIRECTORY src/
    DESTINATION "${NB_INSTALL_MODULE_DIR}"
    FILES_MATCHING PATTERN "*\.py"
  )

  if(NB_USE_SUBMODULE_DEPS)
    install(
      DIRECTORY ext/robin_map/include/
      DESTINATION "${NB_INSTALL_EXT_DIR}/robin_map/include"
    )
    install(
      FILES ext/robin_map/CMakeLists.txt
      DESTINATION "${NB_INSTALL_EXT_DIR}/robin_map"
    )
  endif()

  install(
    DIRECTORY cmake/
    DESTINATION "${NB_INSTALL_CMAKE_DIR}"
  )
endif()

# Return early to skip finding needless dependencies if the user only wishes to
# install nanobind
if (NB_MASTER_PROJECT AND NOT NB_TEST)
  return()
else()
  enable_language(CXX)
endif()

# ---------------------------------------------------------------------------
# Compile with a few more compiler warnings turned on
# ---------------------------------------------------------------------------

if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    add_compile_options(/W4)
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wno-unused-local-typedefs)
endif()

# ---------------------------------------------------------------------------
# Find the Python interpreter and development libraries
# ---------------------------------------------------------------------------

if (NOT TARGET Python::Module OR NOT TARGET Python::Interpreter)
  set(Python_FIND_FRAMEWORK LAST) # Prefer Brew/Conda to Apple framework python

  if (CMAKE_VERSION VERSION_LESS 3.18)
    set(NB_PYTHON_DEV_MODULE Development)
  else()
    set(NB_PYTHON_DEV_MODULE Development.Module)
  endif()

  find_package(Python 3.8
    REQUIRED COMPONENTS Interpreter ${NB_PYTHON_DEV_MODULE}
    OPTIONAL_COMPONENTS Development.SABIModule)
endif()

# ---------------------------------------------------------------------------
# Include nanobind cmake functionality
# ---------------------------------------------------------------------------

find_package(nanobind
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  NO_DEFAULT_PATH)

if (NB_TEST)
  add_subdirectory(tests)
endif()
