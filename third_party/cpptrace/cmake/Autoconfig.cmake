# ================================================== Platform Support ==================================================
function(check_support var source includes libraries definitions)
  set(CMAKE_REQUIRED_INCLUDES "${includes}")
  list(APPEND CMAKE_REQUIRED_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  set(CMAKE_REQUIRED_LIBRARIES "${libraries}")
  set(CMAKE_REQUIRED_DEFINITIONS "${definitions}")
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  string(CONCAT full_source "#include \"${source}\"" ${nonce})
  check_cxx_source_compiles(${full_source} ${var})
  set(${var} ${${var}} PARENT_SCOPE)
endfunction()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  check_support(HAS_CXXABI has_cxxabi.cpp "" "" "")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  check_support(HAS_ATTRIBUTE_PACKED has_attribute_packed.cpp "" "" "")
endif()

if(NOT WIN32)
  check_support(HAS_UNWIND has_unwind.cpp "" "" "")
  check_support(HAS_EXECINFO has_execinfo.cpp "" "" "")
else()
  check_support(HAS_STACKWALK has_stackwalk.cpp "" "dbghelp" "")
endif()

if(NOT WIN32 OR MINGW)
  check_support(HAS_BACKTRACE has_backtrace.cpp "" "backtrace" "${CPPTRACE_BACKTRACE_PATH_DEFINITION}")
  set(STACKTRACE_LINK_LIB "stdc++_libbacktrace")
  check_support(HAS_CXX_EXCEPTION_TYPE has_cxx_exception_type.cpp "" "" "")
endif()

if(UNIX AND NOT APPLE)
  check_support(HAS_DL_FIND_OBJECT has_dl_find_object.cpp "" "dl" "")
  if(NOT HAS_DL_FIND_OBJECT)
    check_support(HAS_DLADDR1 has_dladdr1.cpp "" "dl" "")
  endif()
endif()

if(APPLE)
  check_support(HAS_MACH_VM has_mach_vm.cpp "" "" "")
endif()

# ================================================ Autoconfig unwinding ================================================
# Unwind back-ends
if(
  NOT (
    CPPTRACE_UNWIND_WITH_UNWIND OR
    CPPTRACE_UNWIND_WITH_LIBUNWIND OR
    CPPTRACE_UNWIND_WITH_EXECINFO OR
    CPPTRACE_UNWIND_WITH_WINAPI OR
    CPPTRACE_UNWIND_WITH_DBGHELP OR
    CPPTRACE_UNWIND_WITH_NOTHING
  )
)
  # Attempt to auto-config
  if(APPLE AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"))
    if(HAS_EXECINFO)
      set(CPPTRACE_UNWIND_WITH_EXECINFO On)
      message(STATUS "Cpptrace auto config: Using execinfo.h for unwinding")
    else()
      set(CPPTRACE_UNWIND_WITH_NOTHING On)
      message(FATAL_ERROR "Cpptrace auto config: No unwinding back-end seems to be supported, stack tracing will not work. To compile anyway set CPPTRACE_UNWIND_WITH_NOTHING.")
    endif()
  elseif(UNIX)
    if(HAS_UNWIND)
      set(CPPTRACE_UNWIND_WITH_UNWIND On)
      message(STATUS "Cpptrace auto config: Using libgcc unwind for unwinding")
    elseif(HAS_EXECINFO)
      set(CPPTRACE_UNWIND_WITH_EXECINFO On)
      message(STATUS "Cpptrace auto config: Using execinfo.h for unwinding")
    else()
      set(CPPTRACE_UNWIND_WITH_NOTHING On)
      message(FATAL_ERROR "Cpptrace auto config: No unwinding back-end seems to be supported, stack tracing will not work. To compile anyway set CPPTRACE_UNWIND_WITH_NOTHING.")
    endif()
  elseif(MINGW OR WIN32)
    if(HAS_STACKWALK)
      set(CPPTRACE_UNWIND_WITH_DBGHELP On)
      message(STATUS "Cpptrace auto config: Using dbghelp for unwinding")
    else()
      set(CPPTRACE_UNWIND_WITH_WINAPI On)
      message(STATUS "Cpptrace auto config: Using winapi for unwinding")
    endif()
  endif()
else()
  #message(STATUS "MANUAL CONFIG SPECIFIED")
endif()

# ================================================= Autoconfig symbols =================================================
if(
  NOT (
    CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE OR
    CPPTRACE_GET_SYMBOLS_WITH_LIBDL OR
    CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE OR
    CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF OR
    CPPTRACE_GET_SYMBOLS_WITH_DBGHELP OR
    CPPTRACE_GET_SYMBOLS_WITH_NOTHING
  )
)
  if(UNIX)
    message(STATUS "Cpptrace auto config: Using libdwarf for symbols")
    set(CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF On)
  elseif(MINGW)
    message(STATUS "Cpptrace auto config: Using libdwarf + dbghelp for symbols")
    # Use both dbghelp and libdwarf under mingw: Some files may use pdb symbols, e.g. system dlls like KERNEL32.dll and
    # ntdll.dll at the very least, but also other libraries linked with may have pdb symbols.
    set(CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF On)
    set(CPPTRACE_GET_SYMBOLS_WITH_DBGHELP On)
  else()
    message(STATUS "Cpptrace auto config: Using dbghelp for symbols")
    set(CPPTRACE_GET_SYMBOLS_WITH_DBGHELP On)
  endif()
endif()

# =============================================== Autoconfig demangling ================================================
# Handle demangle configuration
if(
  NOT (
    CPPTRACE_DEMANGLE_WITH_CXXABI OR
    CPPTRACE_DEMANGLE_WITH_WINAPI OR
    CPPTRACE_DEMANGLE_WITH_NOTHING
  )
)
  if(HAS_CXXABI)
    message(STATUS "Cpptrace auto config: Using cxxabi for demangling")
    set(CPPTRACE_DEMANGLE_WITH_CXXABI On)
  elseif(WIN32 AND NOT MINGW)
    message(STATUS "Cpptrace auto config: Using dbghelp for demangling")
    set(CPPTRACE_DEMANGLE_WITH_WINAPI On)
  else()
    set(CPPTRACE_DEMANGLE_WITH_NOTHING On)
  endif()
else()
  #message(STATUS "Manual demangling back-end specified")
endif()
