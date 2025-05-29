cmake_minimum_required(VERSION 3.14)

include(cmake/PreventInSourceBuilds.cmake)

# ---- Initialize Project ----

# Used to support find_package
set(package_name "cpptrace")

project(
  cpptrace
  VERSION 0.8.3
  DESCRIPTION "Simple, portable, and self-contained stacktrace library for C++11 and newer "
  HOMEPAGE_URL "https://github.com/jeremy-rifkin/cpptrace"
  LANGUAGES C CXX
)

# Don't change include order, OptionVariables checks if project is top level
include(cmake/ProjectIsTopLevel.cmake)
include(cmake/OptionVariables.cmake)

include(GNUInstallDirs)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)

if(PROJECT_IS_TOP_LEVEL)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  endif()
endif()

if(PROJECT_IS_TOP_LEVEL)
  if(CMAKE_GENERATOR STREQUAL "Ninja")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      include(CheckCXXCompilerFlag)
      check_cxx_compiler_flag(-fdiagnostics-color=always HAS_CXX_FDIAGNOSTICS_COLOR)
      if(HAS_CXX_FDIAGNOSTICS_COLOR)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
      endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
      include(CheckCCompilerFlag)
      check_c_compiler_flag(-fdiagnostics-color=always HAS_C_FDIAGNOSTICS_COLOR)
      if(HAS_C_FDIAGNOSTICS_COLOR)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
      endif()
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
    endif()
  endif()
endif()

if(CPPTRACE_SANITIZER_BUILD)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

if(NOT "${CPPTRACE_BACKTRACE_PATH}" STREQUAL "")
  # quotes used over <> because of a macro substitution issue where
  # </usr/lib/gcc/x86_64-linux-gnu/12/include/backtrace.h>
  # is expanded to
  # </usr/lib/gcc/x86_64-1-gnu/12/include/backtrace.h>
  string(CONCAT CPPTRACE_BACKTRACE_PATH "\"" ${CPPTRACE_BACKTRACE_PATH})
  string(CONCAT CPPTRACE_BACKTRACE_PATH ${CPPTRACE_BACKTRACE_PATH} "\"")
  #message(STATUS ${CPPTRACE_BACKTRACE_PATH})
  string(CONCAT CPPTRACE_BACKTRACE_PATH_DEFINITION "-DCPPTRACE_BACKTRACE_PATH=" ${CPPTRACE_BACKTRACE_PATH})
  #message(STATUS ${CPPTRACE_BACKTRACE_PATH_DEFINITION})
else()
  set(CPPTRACE_BACKTRACE_PATH_DEFINITION "")
endif()

# ========================================== Platform Support and Auto-config ==========================================
include(cmake/Autoconfig.cmake)

# =================================================== Library Setup ====================================================

if(NOT CPPTRACE_BUILD_NO_SYMBOLS)
  set(
    debug
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-g>
    $<$<CXX_COMPILER_ID:MSVC>:/DEBUG>
  )
else()
  add_compile_options($<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-g0>)
  set(
    debug
  )
endif()

# Target that we can modify (can't modify ALIAS targets)
# Target name should not be the same as ${PROJECT_NAME}, causes add_subdirectory issues
set(target_name "cpptrace-lib")
add_library(${target_name} ${build_type})

# Alias to cause error at configuration time instead of link time if target is missing
add_library(cpptrace::cpptrace ALIAS ${target_name})

# Add /include files to target
# This is solely for IDE benefit, doesn't affect building
target_sources(
    ${target_name} PRIVATE
    include/cpptrace/cpptrace.hpp
    include/ctrace/ctrace.h
)

# add /src files to target
target_sources(
    ${target_name} PRIVATE
    # src
    src/binary/elf.cpp
    src/binary/mach-o.cpp
    src/binary/module_base.cpp
    src/binary/object.cpp
    src/binary/pe.cpp
    src/binary/safe_dl.cpp
    src/cpptrace.cpp
    src/ctrace.cpp
    src/exceptions.cpp
    src/from_current.cpp
    src/formatting.cpp
    src/options.cpp
    src/utils.cpp
    src/demangle/demangle_with_cxxabi.cpp
    src/demangle/demangle_with_nothing.cpp
    src/demangle/demangle_with_winapi.cpp
    src/jit/jit_objects.cpp
    src/snippets/snippet.cpp
    src/symbols/dwarf/debug_map_resolver.cpp
    src/symbols/dwarf/dwarf_options.cpp
    src/symbols/dwarf/dwarf_resolver.cpp
    src/symbols/symbols_core.cpp
    src/symbols/symbols_with_addr2line.cpp
    src/symbols/symbols_with_dbghelp.cpp
    src/symbols/symbols_with_dl.cpp
    src/symbols/symbols_with_libbacktrace.cpp
    src/symbols/symbols_with_libdwarf.cpp
    src/symbols/symbols_with_nothing.cpp
    src/unwind/unwind_with_dbghelp.cpp
    src/unwind/unwind_with_execinfo.cpp
    src/unwind/unwind_with_libunwind.cpp
    src/unwind/unwind_with_nothing.cpp
    src/unwind/unwind_with_unwind.cpp
    src/unwind/unwind_with_winapi.cpp
    src/utils/io/file.cpp
    src/utils/io/memory_file_view.cpp
    src/utils/error.cpp
    src/utils/microfmt.cpp
    src/utils/string_view.cpp
    src/utils/utils.cpp
    src/platform/dbghelp_utils.cpp
)

target_include_directories(
  ${target_name}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cpptrace/>
)

target_include_directories(
  ${target_name}
  PRIVATE
  src
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
  set(IS_CLANG_CL 1)
else()
  set(IS_CLANG_CL 0)
endif()

set(
  warning_options
  $<$<NOT:$<OR:$<CXX_COMPILER_ID:MSVC>,$<STREQUAL:${IS_CLANG_CL},1>>>:-Wall -Wextra -Werror=return-type -Wundef>
  $<$<CXX_COMPILER_ID:GNU>:-Wuseless-cast -Wmaybe-uninitialized>
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

if(CPPTRACE_WERROR_BUILD)
  set(
    warning_options
    ${warning_options}
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
  )
endif()

target_compile_options(
  ${target_name}
  PRIVATE
  ${warning_options}
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
  # https://godbolt.org/z/qYh89E6rq
  target_compile_options(${target_name} PRIVATE -Wno-missing-field-initializers)
endif()

set(CPPTRACE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPPTRACE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPPTRACE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
configure_file("${PROJECT_SOURCE_DIR}/cmake/in/version-hpp.in" "${PROJECT_BINARY_DIR}/include/cpptrace/version.hpp")

# ---- Generate Build Info Headers ----

if(build_type STREQUAL "STATIC")
  target_compile_definitions(${target_name} PUBLIC CPPTRACE_STATIC_DEFINE)
  set(CPPTRACE_STATIC_DEFINE TRUE)
endif()

# ---- Library Properties ----

# Hide all symbols by default
# Use SameMajorVersion versioning for shared library runtime linker lookup
set_target_properties(
    ${target_name} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME "cpptrace"
    OUTPUT_NAME "cpptrace"
    POSITION_INDEPENDENT_CODE ${CPPTRACE_POSITION_INDEPENDENT_CODE}
)

# Header files generated by CMake
target_include_directories(
    ${target_name} SYSTEM PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
)

# Header files from /include
target_include_directories(
    ${target_name} ${warning_guard} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

# Require C++11 support
target_compile_features(
    ${target_name}
    PRIVATE cxx_std_11
)

target_compile_definitions(${target_name} PRIVATE NOMINMAX)

if(HAS_ATTRIBUTE_PACKED)
  target_compile_definitions(${target_name} PRIVATE HAS_ATTRIBUTE_PACKED)
endif()

if(NOT CPPTRACE_STD_FORMAT)
  target_compile_definitions(${target_name} PUBLIC CPPTRACE_NO_STD_FORMAT)
endif()

if(CPPTRACE_UNPREFIXED_TRY_CATCH)
  target_compile_definitions(${target_name} PUBLIC CPPTRACE_UNPREFIXED_TRY_CATCH)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
  SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

# =================================================== Back-end setup ===================================================

if(HAS_CXX_EXCEPTION_TYPE)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_HAS_CXX_EXCEPTION_TYPE)
endif()

if(HAS_DL_FIND_OBJECT)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_HAS_DL_FIND_OBJECT)
endif()

if(HAS_DLADDR1)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_HAS_DLADDR1)
endif()

if(HAS_MACH_VM)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_HAS_MACH_VM)
endif()

# Symbols
if(CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE)
  if(NOT HAS_BACKTRACE)
    if(NOT "${CPPTRACE_BACKTRACE_PATH}" STREQUAL "")
      message(WARNING "Cpptrace: Using libbacktrace for symbols but libbacktrace doesn't appear installed or configured properly.")
    else()
      message(WARNING "Cpptrace: Using libbacktrace for symbols but libbacktrace doesn't appear installed or configured properly. You may need to specify CPPTRACE_BACKTRACE_PATH.")
    endif()
  endif()
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE)
  target_link_libraries(${target_name} PRIVATE backtrace ${CMAKE_DL_LIBS})
endif()

if(CPPTRACE_GET_SYMBOLS_WITH_LIBDL)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_GET_SYMBOLS_WITH_LIBDL)
  target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS})
endif()

if(CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE)
  # set(CPPTRACE_ADDR2LINE_PATH "" CACHE STRING "Absolute path to the addr2line executable you want to use.")
  # option(CPPTRACE_ADDR2LINE_SEARCH_SYSTEM_PATH "" OFF)
  if(CPPTRACE_ADDR2LINE_SEARCH_SYSTEM_PATH)
    target_compile_definitions(${target_name} PRIVATE CPPTRACE_ADDR2LINE_SEARCH_SYSTEM_PATH)
  else()
    if("${CPPTRACE_ADDR2LINE_PATH}" STREQUAL "")
      if(APPLE)
        find_program(CPPTRACE_ADDR2LINE_PATH_FINAL atos PATHS ENV PATH REQUIRED)
      else()
        find_program(CPPTRACE_ADDR2LINE_PATH_FINAL addr2line PATHS ENV PATH REQUIRED)
      endif()
    else()
      set(CPPTRACE_ADDR2LINE_PATH_FINAL "${CPPTRACE_ADDR2LINE_PATH}")
    endif()
    message(STATUS "Cpptrace: Using ${CPPTRACE_ADDR2LINE_PATH_FINAL} for addr2line path")
    target_compile_definitions(${target_name} PRIVATE CPPTRACE_ADDR2LINE_PATH="${CPPTRACE_ADDR2LINE_PATH_FINAL}")
  endif()
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE)
  if(UNIX)
    target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS})
  endif()
endif()

if(CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF)
  if(CPPTRACE_USE_EXTERNAL_LIBDWARF)
    if(NOT CPPTRACE_FIND_LIBDWARF_WITH_PKGCONFIG)
      find_package(libdwarf REQUIRED)
    else()
      find_package(PkgConfig)
      pkg_check_modules(LIBDWARF REQUIRED libdwarf)
    endif()
  else()
    include(FetchContent)
    # First, dependencies: Zstd and zlib (currently relying on system zlib)
    if(CPPTRACE_USE_EXTERNAL_ZSTD)
      find_package(zstd)
    else()
      cmake_policy(SET CMP0074 NEW)
      set(ZSTD_BUILD_PROGRAMS OFF)
      set(ZSTD_BUILD_CONTRIB OFF)
      set(ZSTD_BUILD_TESTS OFF)
      set(ZSTD_BUILD_STATIC ON)
      set(ZSTD_BUILD_SHARED OFF)
      set(ZSTD_LEGACY_SUPPORT OFF)
      FetchContent_Declare(
          zstd
          SOURCE_SUBDIR build/cmake
          DOWNLOAD_EXTRACT_TIMESTAMP TRUE
          URL "${CPPTRACE_ZSTD_URL}"
      )
      FetchContent_MakeAvailable(zstd)
    endif()
    # Libdwarf itself
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(PIC_ALWAYS TRUE)
    set(BUILD_DWARFDUMP FALSE)
    FetchContent_Declare(
      libdwarf
      GIT_REPOSITORY ${CPPTRACE_LIBDWARF_REPO}
      GIT_TAG ${CPPTRACE_LIBDWARF_TAG}
      GIT_SHALLOW ${CPPTRACE_LIBDWARF_SHALLOW}
    )
    FetchContent_MakeAvailable(libdwarf)
    target_include_directories(
      dwarf
      PRIVATE
      ${zstd_SOURCE_DIR}/lib
    )
    if(CPPTRACE_PROVIDE_EXPORT_SET_FOR_LIBDWARF)
      export(
        TARGETS dwarf
        NAMESPACE libdwarf::
        FILE "${PROJECT_BINARY_DIR}/libdwarf-targets.cmake"
      )
    endif()
  endif()
  if(CPPTRACE_CONAN)
    set(dwarf_lib libdwarf::libdwarf)
    target_link_libraries(${target_name} PRIVATE libdwarf::libdwarf)
  elseif(CPPTRACE_VCPKG)
    set(dwarf_lib libdwarf::dwarf)
    target_link_libraries(${target_name} PRIVATE libdwarf::dwarf)
  elseif(CPPTRACE_USE_EXTERNAL_LIBDWARF)
    if(DEFINED LIBDWARF_LIBRARIES)
      set(dwarf_lib ${LIBDWARF_LIBRARIES})
      target_link_libraries(${target_name} PRIVATE ${LIBDWARF_LIBRARIES})
    else()
      # if LIBDWARF_LIBRARIES wasn't set by find_package, try looking for libdwarf::dwarf-static,
      # libdwarf::dwarf-shared, libdwarf::dwarf, then libdwarf
      # libdwarf v0.8.0 installs with the target libdwarf::dwarf somehow, despite creating libdwarf::dwarf-static or
      # libdwarf::dwarf-shared under fetchcontent
      if(TARGET libdwarf::dwarf-static)
        set(LIBDWARF_LIBRARIES libdwarf::dwarf-static)
      elseif(TARGET libdwarf::dwarf-shared)
        set(LIBDWARF_LIBRARIES libdwarf::dwarf-shared)
      elseif(TARGET libdwarf::dwarf)
        set(LIBDWARF_LIBRARIES libdwarf::dwarf)
      elseif(TARGET libdwarf)
        set(LIBDWARF_LIBRARIES libdwarf)
      else()
        message(FATAL_ERROR "Couldn't find libdwarf target name to link against")
      endif()
      set(dwarf_lib ${LIBDWARF_LIBRARIES})
      target_link_libraries(${target_name} PRIVATE ${LIBDWARF_LIBRARIES})
    endif()
    # There seems to be no consistency at all about where libdwarf decides to place its headers........ Figure out if
    # it's libdwarf/libdwarf.h and libdwarf/dwarf.h or just libdwarf.h and dwarf.h
    include(CheckIncludeFileCXX)
    # libdwarf's cmake doesn't properly set variables to indicate where its libraries live
    if(NOT CPPTRACE_FIND_LIBDWARF_WITH_PKGCONFIG)
      get_target_property(LIBDWARF_INCLUDE_DIRS ${LIBDWARF_LIBRARIES} INTERFACE_INCLUDE_DIRECTORIES)
    else()
      target_include_directories(${target_name} PRIVATE ${LIBDWARF_INCLUDE_DIRS})
    endif()
    set(CMAKE_REQUIRED_INCLUDES ${LIBDWARF_INCLUDE_DIRS})
    CHECK_INCLUDE_FILE_CXX("libdwarf/libdwarf.h" LIBDWARF_IS_NESTED)
    CHECK_INCLUDE_FILE_CXX("libdwarf.h" LIBDWARF_IS_NOT_NESTED)
    # check_include_file("libdwarf/libdwarf.h" LIBDWARF_IS_NESTED)
    # check_support(LIBDWARF_IS_NESTED nested_libdwarf_include.cpp "" "" "")
    if(${LIBDWARF_IS_NESTED})
      target_compile_definitions(${target_name} PRIVATE CPPTRACE_USE_NESTED_LIBDWARF_HEADER_PATH)
    elseif(NOT LIBDWARF_IS_NOT_NESTED)
      message(FATAL_ERROR "Couldn't find libdwarf.h")
    endif()
  else()
    set(dwarf_lib libdwarf::dwarf-static)
    target_link_libraries(${target_name} PRIVATE libdwarf::dwarf-static)
  endif()
  if(UNIX)
    target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS})
  endif()
endif()

if(CPPTRACE_GET_SYMBOLS_WITH_DBGHELP)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_GET_SYMBOLS_WITH_DBGHELP)
  target_link_libraries(${target_name} PRIVATE dbghelp)
endif()

if(CPPTRACE_GET_SYMBOLS_WITH_NOTHING)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_GET_SYMBOLS_WITH_NOTHING)
endif()

# Unwinding
if(CPPTRACE_UNWIND_WITH_UNWIND)
  if(NOT HAS_UNWIND)
    message(WARNING "Cpptrace: CPPTRACE_UNWIND_WITH_UNWIND specified but libgcc unwind doesn't seem to be available.")
  endif()
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_UNWIND_WITH_UNWIND)
endif()

if(CPPTRACE_UNWIND_WITH_LIBUNWIND)
  find_package(PkgConfig)
  if(PkgConfig_FOUND)
    pkg_check_modules(LIBUNWIND QUIET libunwind)
    if(libunwind_FOUND)
      target_compile_options(${target_name} PRIVATE ${LIBUNWIND_CFLAGS_OTHER})
      target_include_directories(${target_name} PRIVATE ${LIBUNWIND_INCLUDE_DIRS})
      target_link_libraries(${target_name} PRIVATE ${LIBUNWIND_LDFLAGS})
    endif()
  endif()
  if(NOT libunwind_FOUND)
    if (NOT APPLE)
      # set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
      # set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS ON)
      find_path(LIBUNWIND_INCLUDE_DIRS NAMES "libunwind.h")
      find_library(LIBUNWIND NAMES unwind libunwind libunwind8 libunwind.so.8 REQUIRED PATHS "/usr/lib/x86_64-linux-gnu/")
      if(LIBUNWIND)
        set(libunwind_FOUND TRUE)
      endif()
      if(NOT libunwind_FOUND)
        # message(FATAL_ERROR "Unable to locate libunwind")
        # Try to link with it if it's where it should be
        # This path can be entered if libunwind was installed via the system package manager, sometimes. I probably messed
        # up the find_library above.
        set(LIBUNWIND_LDFLAGS "-lunwind")
      endif()
      if(NOT LIBUNWIND_LDFLAGS)
        set(LIBUNWIND_LDFLAGS "${LIBUNWIND}")
      endif()
      target_compile_options(${target_name} PRIVATE ${LIBUNWIND_CFLAGS_OTHER})
      target_include_directories(${target_name} PRIVATE ${LIBUNWIND_INCLUDE_DIRS})
      target_link_libraries(${target_name} PRIVATE ${LIBUNWIND_LDFLAGS})
    endif()
    target_compile_definitions(${target_name} PRIVATE CPPTRACE_UNWIND_WITH_LIBUNWIND UNW_LOCAL_ONLY)
  endif()
endif()

if(CPPTRACE_UNWIND_WITH_EXECINFO)
  if(NOT HAS_EXECINFO)
    message(WARNING "Cpptrace: CPPTRACE_UNWIND_WITH_EXECINFO specified but execinfo.h doesn't seem to be available.")
  endif()
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_UNWIND_WITH_EXECINFO)
endif()

if(CPPTRACE_UNWIND_WITH_WINAPI)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_UNWIND_WITH_WINAPI)
endif()

if(CPPTRACE_UNWIND_WITH_DBGHELP)
  if(NOT HAS_STACKWALK)
    message(WARNING "Cpptrace: CPPTRACE_UNWIND_WITH_DBGHELP specified but dbghelp stackwalk64 doesn't seem to be available.")
  endif()
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_UNWIND_WITH_DBGHELP)
  target_link_libraries(${target_name} PRIVATE dbghelp)
endif()

if(CPPTRACE_UNWIND_WITH_NOTHING)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_UNWIND_WITH_NOTHING)
endif()

# Demangling
if(CPPTRACE_DEMANGLE_WITH_CXXABI)
  if(NOT HAS_CXXABI)
    message(WARNING "Cpptrace: CPPTRACE_DEMANGLE_WITH_CXXABI specified but cxxabi.h doesn't seem to be available.")
  endif()
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_DEMANGLE_WITH_CXXABI)
endif()

if(CPPTRACE_DEMANGLE_WITH_WINAPI)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_DEMANGLE_WITH_WINAPI)
  target_link_libraries(${target_name} PRIVATE dbghelp)
endif()

if(CPPTRACE_DEMANGLE_WITH_NOTHING)
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_DEMANGLE_WITH_NOTHING)
endif()

if(NOT "${CPPTRACE_BACKTRACE_PATH}" STREQUAL "")
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_BACKTRACE_PATH=${CPPTRACE_BACKTRACE_PATH})
endif()

if(NOT "${CPPTRACE_HARD_MAX_FRAMES}" STREQUAL "")
  target_compile_definitions(${target_name} PRIVATE CPPTRACE_HARD_MAX_FRAMES=${CPPTRACE_HARD_MAX_FRAMES})
endif()

# ====================================================== Install =======================================================

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/InstallRules.cmake)
endif()

# ================================================== Demo/test/tools ===================================================

if(CPPTRACE_BUILD_TESTING)
  if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
  endif()
  add_subdirectory(test)
endif()

if(CPPTRACE_BUILD_BENCHMARKING)
  add_subdirectory(benchmarking)
endif()

if(CPPTRACE_BUILD_TOOLS)
  add_subdirectory(tools)
endif()
