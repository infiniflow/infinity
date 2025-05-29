include(CTest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(
  warning_options
  ${warning_options} $<$<CXX_COMPILER_ID:GNU>:-Wno-infinite-recursion>
)

macro(add_test_dependencies exec_name)
  target_compile_features(${exec_name} PRIVATE cxx_std_11)
  target_link_libraries(${exec_name} PRIVATE ${target_name})
  target_compile_options(${exec_name} PRIVATE ${warning_options})
  target_compile_options(${exec_name} PRIVATE ${debug})
  if(NOT CPPTRACE_BUILD_NO_SYMBOLS AND CPPTRACE_BUILD_TESTING_SPLIT_DWARF)
    target_compile_options(${exec_name} PRIVATE -gsplit-dwarf)
  endif()
  if(NOT CPPTRACE_BUILD_NO_SYMBOLS AND NOT (CPPTRACE_BUILD_TESTING_DWARF_VERSION STREQUAL "0"))
    target_compile_options(${exec_name} PRIVATE -gdwarf-${CPPTRACE_BUILD_TESTING_DWARF_VERSION})
  endif()
  # # Clang has been fast to adopt dwarf 5, other tools (e.g. addr2line from binutils) have not
  if(CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE AND NOT CPPTRACE_BUILD_NO_SYMBOLS)
    check_cxx_compiler_flag("-gdwarf-4" HAS_DWARF4)
    if(HAS_DWARF4)
      target_compile_options(${exec_name} PRIVATE "$<$<CONFIG:Debug>:-gdwarf-4>")
    endif()
  endif()
  # TODO: add debug info for mingw clang?
  if(CPPTRACE_BUILD_TEST_RDYNAMIC)
    set_property(TARGET ${exec_name} PROPERTY ENABLE_EXPORTS ON)
  endif()
endmacro()


add_executable(integration integration.cpp)
add_executable(demo demo.cpp)
add_executable(c_demo ctrace_demo.c)
add_executable(link_test link_test.cpp)

add_test_dependencies(integration)
add_test_dependencies(demo)
add_test_dependencies(c_demo)
add_test_dependencies(link_test)

if(HAS_CXX20_MODULES)
  add_executable(integration_modules integration.cpp)
  add_test_dependencies(integration_modules)

  add_executable(link_test_modules link_test.cpp)
  add_test_dependencies(link_test_modules)
endif()

if(UNIX)
  add_executable(signal_demo signal_demo.cpp)
  target_compile_features(signal_demo PRIVATE cxx_std_11)
  target_link_libraries(signal_demo PRIVATE ${target_name})
  target_compile_options(signal_demo PRIVATE ${debug})
  if(NOT CPPTRACE_BUILD_NO_SYMBOLS AND CPPTRACE_BUILD_TESTING_SPLIT_DWARF)
    target_compile_options(signal_demo PRIVATE -gsplit-dwarf)
  endif()
  if(NOT CPPTRACE_BUILD_NO_SYMBOLS AND NOT (CPPTRACE_BUILD_TESTING_DWARF_VERSION STREQUAL "0"))
    target_compile_options(signal_demo PRIVATE -gdwarf-${CPPTRACE_BUILD_TESTING_DWARF_VERSION})
  endif()

  add_executable(signal_tracer signal_tracer.cpp)
  target_compile_features(signal_tracer PRIVATE cxx_std_11)
  target_link_libraries(signal_tracer PRIVATE ${target_name})
  target_compile_options(signal_tracer PRIVATE ${debug})
endif()

function(test_cpptrace)
  cmake_parse_arguments(
      CPPTRACE
      ""          # No-arg options
      "TEST_NAME" # Single-arg options
      "DEFINE"    # Multi-arg options
      ${ARGN}     # Parameters to parse
  )

  add_executable(
    "${CPPTRACE_TEST_NAME}"
    unit/main.cpp
    unit/tracing/raw_trace.cpp
    unit/tracing/object_trace.cpp
    unit/tracing/stacktrace.cpp
    unit/tracing/from_current.cpp
    unit/tracing/from_current_try_catch.cpp
    unit/tracing/try_catch.cpp
    unit/tracing/traced_exception.cpp
    unit/tracing/rethrow.cpp
    unit/internals/optional.cpp
    unit/internals/lru_cache.cpp
    unit/internals/result.cpp
    unit/internals/string_utils.cpp
    unit/internals/general.cpp
    unit/internals/span.cpp
    unit/internals/string_view.cpp
    unit/lib/formatting.cpp
    unit/lib/nullable.cpp
    unit/lib/prune_symbol.cpp
  )

  target_compile_features("${CPPTRACE_TEST_NAME}" PRIVATE cxx_std_20)
  target_link_libraries("${CPPTRACE_TEST_NAME}" PRIVATE ${target_name} GTest::gtest_main GTest::gmock_main)
  target_compile_definitions("${CPPTRACE_TEST_NAME}" PRIVATE ${CPPTRACE_DEFINE})

  target_compile_options("${CPPTRACE_TEST_NAME}" PRIVATE ${warning_options} $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-pedantic -Wno-attributes>)
  target_compile_options("${CPPTRACE_TEST_NAME}" PRIVATE ${debug})
  if(NOT CPPTRACE_BUILD_NO_SYMBOLS AND CPPTRACE_BUILD_TESTING_SPLIT_DWARF)
    target_compile_options("${CPPTRACE_TEST_NAME}" PRIVATE -gsplit-dwarf)
  endif()
  if(NOT CPPTRACE_BUILD_NO_SYMBOLS AND NOT (CPPTRACE_BUILD_TESTING_DWARF_VERSION STREQUAL "0"))
    target_compile_options("${CPPTRACE_TEST_NAME}" PRIVATE -gdwarf-${CPPTRACE_BUILD_TESTING_DWARF_VERSION})
  endif()
  if(CPPTRACE_SANITIZER_BUILD)
    target_compile_definitions("${CPPTRACE_TEST_NAME}" PRIVATE CPPTRACE_SANITIZER_BUILD)
  endif()
  if(CPPTRACE_BUILD_NO_SYMBOLS)
    target_compile_definitions("${CPPTRACE_TEST_NAME}" PRIVATE CPPTRACE_BUILD_NO_SYMBOLS)
  endif()
  target_include_directories("${CPPTRACE_TEST_NAME}" PRIVATE ../src)
  add_test(NAME ${CPPTRACE_TEST_NAME} COMMAND ${CPPTRACE_TEST_NAME})
endfunction()

# primarily a workaround for github actions issue https://github.com/actions/runner-images/issues/8659
if(NOT CPPTRACE_SKIP_UNIT)
  if(CPPTRACE_USE_EXTERNAL_GTEST)
    find_package(GTest)
  else()
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY "https://github.com/google/googletest.git"
      GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()

  test_cpptrace(TEST_NAME unittest)

  if(HAS_CXX20_MODULES)
    test_cpptrace(
      TEST_NAME unittest_module
      DEFINE TEST_MODULE
    )
  endif()
endif()
