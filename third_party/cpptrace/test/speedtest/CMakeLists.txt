cmake_minimum_required(VERSION 3.8)

project(cpptrace-speedtest CXX)

option(SPEEDTEST_DWARF4 "" OFF)
option(SPEEDTEST_DWARF5 "" OFF)

include(CheckCXXCompilerFlag)

if(SPEEDTEST_DWARF4)
  check_cxx_compiler_flag("-gdwarf-4" HAS_DWARF4)
  if(HAS_DWARF4)
    add_compile_options("$<$<CONFIG:Debug>:-gdwarf-4>")
    #target_compile_options(speedtest PRIVATE "$<$<CONFIG:Debug>:-gdwarf-4>")
    #target_compile_options(googletest INTERFACE "$<$<CONFIG:Debug>:-gdwarf-4>")
  endif()
endif()
if(SPEEDTEST_DWARF5)
  check_cxx_compiler_flag("-gdwarf-5" HAS_DWARF5)
  if(HAS_DWARF5)
    add_compile_options("$<$<CONFIG:Debug>:-gdwarf-5>")
    #target_compile_options(speedtest PRIVATE "$<$<CONFIG:Debug>:-gdwarf-4>")
    #target_compile_options(googletest INTERFACE "$<$<CONFIG:Debug>:-gdwarf-4>")
  endif()
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP On
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(cpptrace_DIR "../../build/foo/lib/cmake/cpptrace")
set(libdwarf_DIR "../../build/foo/lib/cmake/libdwarf")
set(zstd_DIR "../../build/foo/lib/cmake/zstd")
find_package(cpptrace REQUIRED)

add_executable(speedtest speedtest.cpp)
target_compile_features(speedtest PRIVATE cxx_std_11)
target_link_libraries(
  speedtest
  PRIVATE
  GTest::gtest_main
  cpptrace::cpptrace
)

if(WIN32)
  add_custom_command(
      TARGET speedtest POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:speedtest> $<TARGET_FILE_DIR:speedtest>
      COMMAND_EXPAND_LISTS
  )
endif()
