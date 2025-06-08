# Prerequisites for Windows:
#     This cmake build is for Windows 64-bit only.
#
# Prerequisites:
#     You must have at least Visual Studio 2019. Start the Developer Command Prompt window that is a part of Visual Studio installation.
#     Run the build commands from within the Developer Command Prompt window to have paths to the compiler and runtime libraries set.
#     You must have git.exe in your %PATH% environment variable.
#
# To build Rocksdb for Windows is as easy as 1-2-3-4-5:
#
# 1. Update paths to third-party libraries in thirdparty.inc file
# 2. Create a new directory for build artifacts
#        mkdir build
#        cd build
# 3. Run cmake to generate project files for Windows, add more options to enable required third-party libraries.
#    See thirdparty.inc for more information.
#        sample command: cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DWITH_GFLAGS=1 -DWITH_SNAPPY=1 -DWITH_JEMALLOC=1 -DWITH_JNI=1 ..
# 4. Then build the project in debug mode (you may want to add /m[:<N>] flag to run msbuild in <N> parallel threads
#                                          or simply /m to use all avail cores)
#        msbuild rocksdb.sln
#
#        rocksdb.sln build features exclusions of test only code in Release. If you build ALL_BUILD then everything
#        will be attempted but test only code does not build in Release mode.
#
# 5. And release mode (/m[:<N>] is also supported)
#        msbuild rocksdb.sln /p:Configuration=Release
#
# Linux:
#
# 1. Install a recent toolchain if you're on a older distro. C++17 required (GCC >= 7, Clang >= 5)
# 2. mkdir build; cd build
# 3. cmake ..
# 4. make -j

cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")
#include(ReadVersion)
#include(GoogleTest)
#get_rocksdb_version(rocksdb_VERSION)
project(rocksdb
  DESCRIPTION "An embeddable persistent key-value store for fast storage"
  HOMEPAGE_URL https://rocksdb.org/
  LANGUAGES CXX C ASM)

if(APPLE)
  # On macOS Cmake, when cross-compiling, sometimes CMAKE_SYSTEM_PROCESSOR wrongfully stays
  # the same as CMAKE_HOST_SYSTEM_PROCESSOR regardless the target CPU.
  # The manual call to  set(CMAKE_SYSTEM_PROCESSOR) has to be set after the project() call.
  # because project() might reset CMAKE_SYSTEM_PROCESSOR back to the value of CMAKE_HOST_SYSTEM_PROCESSOR.
  # Check if CMAKE_SYSTEM_PROCESSOR is not equal to CMAKE_OSX_ARCHITECTURES
  if(NOT CMAKE_OSX_ARCHITECTURES STREQUAL "")
    if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL CMAKE_OSX_ARCHITECTURES)
      # Split CMAKE_OSX_ARCHITECTURES into a list
      string(REPLACE ";" " " ARCH_LIST ${CMAKE_OSX_ARCHITECTURES})
      separate_arguments(ARCH_LIST UNIX_COMMAND ${ARCH_LIST})
      # Count the number of architectures
      list(LENGTH ARCH_LIST ARCH_COUNT)
      # Ensure that exactly one architecture is specified
      if(NOT ARCH_COUNT EQUAL 1)
          message(FATAL_ERROR "CMAKE_OSX_ARCHITECTURES must have exactly one value. Current value: ${CMAKE_OSX_ARCHITECTURES}")
      endif()
      set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_OSX_ARCHITECTURES})
      message(STATUS "CMAKE_SYSTEM_PROCESSOR is manually set to ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
  endif()
endif()

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

if(NOT CMAKE_BUILD_TYPE)
  if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
  else()
    set(default_build_type "RelWithDebInfo")
  endif()
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING
    "Default BUILD_TYPE is ${default_build_type}" FORCE)
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

option(WITH_JEMALLOC "build with JeMalloc" OFF)
option(WITH_LIBURING "build with liburing" ON)
option(WITH_SNAPPY "build with SNAPPY" OFF)
option(WITH_LZ4 "build with lz4" OFF)
option(WITH_ZLIB "build with zlib" OFF)
option(WITH_ZSTD "build with zstd" OFF)
option(WITH_WINDOWS_UTF8_FILENAMES "use UTF8 as characterset for opening files, regardles of the system code page" OFF)
if (WITH_WINDOWS_UTF8_FILENAMES)
  add_definitions(-DROCKSDB_WINDOWS_UTF8_FILENAMES)
endif()
option(ROCKSDB_BUILD_SHARED "Build shared versions of the RocksDB libraries" ON)

if( NOT DEFINED CMAKE_CXX_STANDARD )
  set(CMAKE_CXX_STANDARD 17)
endif()

include(CMakeDependentOption)

if(MSVC)
  option(WITH_GFLAGS "build with GFlags" OFF)
  option(WITH_XPRESS "build with windows built in compression" OFF)
  option(ROCKSDB_SKIP_THIRDPARTY "skip thirdparty.inc" OFF)

  if(NOT ROCKSDB_SKIP_THIRDPARTY)
    include(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty.inc)
  endif()
else()
  if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" AND NOT CMAKE_SYSTEM_NAME MATCHES "kFreeBSD")
    # FreeBSD has jemalloc as default malloc
    # but it does not have all the jemalloc files in include/...
    set(WITH_JEMALLOC ON)
  else()
    if(WITH_JEMALLOC)
      find_package(JeMalloc REQUIRED)
      add_definitions(-DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE)
      list(APPEND THIRDPARTY_LIBS JeMalloc::JeMalloc)
    endif()
  endif()

#  if(MINGW)
#    option(WITH_GFLAGS "build with GFlags" OFF)
#  else()
#    option(WITH_GFLAGS "build with GFlags" ON)
#  endif()
#  set(GFLAGS_LIB)
#  if(WITH_GFLAGS)
#    # Config with namespace available since gflags 2.2.2
#    option(GFLAGS_USE_TARGET_NAMESPACE "Use gflags import target with namespace." ON)
#    find_package(gflags CONFIG)
#    if(gflags_FOUND)
#      if(TARGET ${GFLAGS_TARGET})
#        # Config with GFLAGS_TARGET available since gflags 2.2.0
#        set(GFLAGS_LIB ${GFLAGS_TARGET})
#      else()
#        # Config with GFLAGS_LIBRARIES available since gflags 2.1.0
#        set(GFLAGS_LIB ${gflags_LIBRARIES})
#      endif()
#    else()
#      find_package(gflags REQUIRED)
#      set(GFLAGS_LIB gflags::gflags)
#    endif()
#    include_directories(${GFLAGS_INCLUDE_DIR})
#    list(APPEND THIRDPARTY_LIBS ${GFLAGS_LIB})
#    add_definitions(-DGFLAGS=1)
#  endif()

  if(WITH_SNAPPY)
    find_package(Snappy CONFIG)
    if(NOT Snappy_FOUND)
      find_package(Snappy REQUIRED)
    endif()
    add_definitions(-DSNAPPY)
    list(APPEND THIRDPARTY_LIBS Snappy::snappy)
  endif()

  if(WITH_ZLIB)
    find_package(ZLIB REQUIRED)
    add_definitions(-DZLIB)
    list(APPEND THIRDPARTY_LIBS ZLIB::ZLIB)
  endif()

  option(WITH_BZ2 "build with bzip2" OFF)
  if(WITH_BZ2)
    find_package(BZip2 REQUIRED)
    add_definitions(-DBZIP2)
    if(BZIP2_INCLUDE_DIRS)
      include_directories(${BZIP2_INCLUDE_DIRS})
    else()
      include_directories(${BZIP2_INCLUDE_DIR})
    endif()
    list(APPEND THIRDPARTY_LIBS ${BZIP2_LIBRARIES})
  endif()

  if(WITH_LZ4)
    find_package(lz4 REQUIRED)
    add_definitions(-DLZ4)
    list(APPEND THIRDPARTY_LIBS lz4::lz4)
  endif()

  if(WITH_ZSTD)
    find_package(zstd REQUIRED)
    add_definitions(-DZSTD)
    include_directories(${ZSTD_INCLUDE_DIRS})
    list(APPEND THIRDPARTY_LIBS zstd::zstd)
  endif()
endif()

option(WITH_MD_LIBRARY "build with MD" ON)
if(WIN32 AND MSVC)
  if(WITH_MD_LIBRARY)
    set(RUNTIME_LIBRARY "MD")
  else()
    set(RUNTIME_LIBRARY "MT")
  endif()
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /nologo /EHsc /GS /Gd /GR /GF /fp:precise /Zc:wchar_t /Zc:forScope /errorReport:queue")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FC /d2Zi+ /W4 /wd4127 /wd4996 /wd4100 /wd4324")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wextra -Wall -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare -Wshadow -Wno-unused-parameter -Wno-unused-variable -Woverloaded-virtual -Wnon-virtual-dtor -Wno-missing-field-initializers -Wno-strict-aliasing -Wno-invalid-offsetof")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
  endif()
  if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
    add_definitions(-D_POSIX_C_SOURCE=1)
  endif()
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-momit-leaf-frame-pointer" HAVE_OMIT_LEAF_FRAME_POINTER)
    if(HAVE_OMIT_LEAF_FRAME_POINTER)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -momit-leaf-frame-pointer")
    endif()
  endif()
endif()

include(CheckCCompilerFlag)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64")
  CHECK_C_COMPILER_FLAG("-mcpu=power9" HAS_POWER9)
  if(HAS_POWER9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=power9 -mtune=power9")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=power9 -mtune=power9")
  else()
    CHECK_C_COMPILER_FLAG("-mcpu=power8" HAS_POWER8)
    if(HAS_POWER8)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=power8 -mtune=power8")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=power8 -mtune=power8")
    endif(HAS_POWER8)
  endif(HAS_POWER9)
  CHECK_C_COMPILER_FLAG("-maltivec" HAS_ALTIVEC)
  if(HAS_ALTIVEC)
    message(STATUS " HAS_ALTIVEC yes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -maltivec")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -maltivec")
  endif(HAS_ALTIVEC)
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|AARCH64")
        CHECK_C_COMPILER_FLAG("-march=armv8-a+crc+crypto" HAS_ARMV8_CRC)
  if(HAS_ARMV8_CRC)
    message(STATUS " HAS_ARMV8_CRC yes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+crc+crypto -Wno-unused-function")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crc+crypto -Wno-unused-function")
  endif(HAS_ARMV8_CRC)
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|AARCH64")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "s390x")
  CHECK_C_COMPILER_FLAG("-march=native" HAS_S390X_MARCH_NATIVE)
  if(HAS_S390X_MARCH_NATIVE)
    message(STATUS " HAS_S390X_MARCH_NATIVE yes")
  endif(HAS_S390X_MARCH_NATIVE)
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "s390x")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "loongarch64")
  CHECK_C_COMPILER_FLAG("-march=loongarch64" HAS_LOONGARCH64)
  if(HAS_LOONGARCH64)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=loongarch64 -mtune=loongarch64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=loongarch64 -mtune=loongarch64")
  endif(HAS_LOONGARCH64)
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "loongarch64")

set(PORTABLE 0 CACHE STRING "Minimum CPU arch to support, or 0 = current CPU, 1 = baseline CPU")
if(PORTABLE MATCHES "1|ON|YES|TRUE|Y")
  # Usually nothing to do; compiler default is typically the most general
  if(NOT MSVC)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^s390x")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=z196")
    endif()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^loongarch64")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=loongarch64")
    endif()
  endif()
elseif(PORTABLE MATCHES "0|OFF|NO|FALSE|N")
  if(MSVC)
    # NOTE: No auto-detection of current CPU, but instead assume some useful
    # level of optimization is supported
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
  else()
    # Require instruction set from current CPU (with some legacy or opt-out
    # exceptions)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^s390x" AND NOT HAS_S390X_MARCH_NATIVE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=z196")
    elseif(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64" AND NOT HAS_ARMV8_CRC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
  endif()
else()
  # Name of a CPU arch spec or feature set to require
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:${PORTABLE}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${PORTABLE}")
  endif()
endif()

include(CheckCXXSourceCompiles)
set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
if(NOT MSVC)
  set(CMAKE_REQUIRED_FLAGS "-msse4.2 -mpclmul")
endif()

# Check if -latomic is required or not
if (NOT MSVC)
  set(CMAKE_REQUIRED_FLAGS "--std=c++17")
  CHECK_CXX_SOURCE_COMPILES("
#include <atomic>
std::atomic<uint64_t> x(0);
int main() {
  uint64_t i = x.load(std::memory_order_relaxed);
  bool b = x.is_lock_free();
  return 0;
}
" BUILTIN_ATOMIC)
  if (NOT BUILTIN_ATOMIC)
    #TODO: Check if -latomic exists
    list(APPEND THIRDPARTY_LIBS atomic)
  endif()
endif()

#if (WITH_LIBURING)
#  find_package(uring)
#  if (uring_FOUND)
#    add_definitions(-DROCKSDB_IOURING_PRESENT)
#    list(APPEND THIRDPARTY_LIBS uring::uring)
#  endif()
#endif()

# Reset the required flags
set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})

option(WITH_IOSTATS_CONTEXT "Enable IO stats context" ON)
if (NOT WITH_IOSTATS_CONTEXT)
  add_definitions(-DNIOSTATS_CONTEXT)
endif()

option(WITH_PERF_CONTEXT "Enable perf context" ON)
if (NOT WITH_PERF_CONTEXT)
  add_definitions(-DNPERF_CONTEXT)
endif()

option(FAIL_ON_WARNINGS "Treat compile warnings as errors" ON)
if(FAIL_ON_WARNINGS)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
  else() # assume GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
endif()

option(WITH_ASAN "build with ASAN" OFF)
if(WITH_ASAN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
  if(WITH_JEMALLOC)
    message(FATAL "ASAN does not work well with JeMalloc")
  endif()
endif()

option(WITH_TSAN "build with TSAN" OFF)
if(WITH_TSAN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread -Wl,-pie")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -fPIC")
  if(WITH_JEMALLOC)
    message(FATAL "TSAN does not work well with JeMalloc")
  endif()
endif()

option(WITH_UBSAN "build with UBSAN" OFF)
if(WITH_UBSAN)
  add_definitions(-DROCKSDB_UBSAN_RUN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
  if(WITH_JEMALLOC)
    message(FATAL "UBSAN does not work well with JeMalloc")
  endif()
endif()

option(WITH_NUMA "build with NUMA policy support" OFF)
if(WITH_NUMA)
  find_package(NUMA REQUIRED)
  add_definitions(-DNUMA)
  include_directories(${NUMA_INCLUDE_DIRS})
  list(APPEND THIRDPARTY_LIBS NUMA::NUMA)
endif()

option(WITH_TBB "build with Threading Building Blocks (TBB)" OFF)
if(WITH_TBB)
  find_package(TBB REQUIRED)
  add_definitions(-DTBB)
  list(APPEND THIRDPARTY_LIBS TBB::TBB)
endif()

# Stall notifications eat some performance from inserts
option(DISABLE_STALL_NOTIF "Build with stall notifications" OFF)
if(DISABLE_STALL_NOTIF)
  add_definitions(-DROCKSDB_DISABLE_STALL_NOTIFICATION)
endif()

option(WITH_DYNAMIC_EXTENSION "build with dynamic extension support" OFF)
if(NOT WITH_DYNAMIC_EXTENSION)
  add_definitions(-DROCKSDB_NO_DYNAMIC_EXTENSION)
endif()

option(ASSERT_STATUS_CHECKED "build with assert status checked" OFF)
if (ASSERT_STATUS_CHECKED)
  message(STATUS "Build with assert status checked")
  add_definitions(-DROCKSDB_ASSERT_STATUS_CHECKED)
endif()


## RTTI is by default AUTO which enables it in debug and disables it in release.
#set(USE_RTTI AUTO CACHE STRING "Enable RTTI in builds")
#set_property(CACHE USE_RTTI PROPERTY STRINGS AUTO ON OFF)
#if(USE_RTTI STREQUAL "AUTO")
#  message(STATUS "Enabling RTTI in Debug builds only (default)")
#  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DROCKSDB_USE_RTTI")
#  if(MSVC)
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GR-")
#  else()
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti")
#  endif()
#elseif(USE_RTTI)
#  message(STATUS "Enabling RTTI in all builds")
#  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DROCKSDB_USE_RTTI")
#  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DROCKSDB_USE_RTTI")
#else()
#  if(MSVC)
#    message(STATUS "Disabling RTTI in Release builds. Always on in Debug.")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DROCKSDB_USE_RTTI")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GR-")
#  else()
#    message(STATUS "Disabling RTTI in all builds")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-rtti")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti")
#  endif()
#endif()

# Used to run CI build and tests so we can run faster
option(OPTDBG "Build optimized debug build with MSVC" OFF)
option(WITH_RUNTIME_DEBUG "build with debug version of runtime library" ON)
if(MSVC)
  if(OPTDBG)
    message(STATUS "Debug optimization is enabled")
    set(CMAKE_CXX_FLAGS_DEBUG "/Oxt")
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /RTC1")

    # Minimal Build is deprecated after MSVC 2015
    if( MSVC_VERSION GREATER 1900 )
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gm-")
    else()
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gm")
    endif()

  endif()
  if(WITH_RUNTIME_DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /${RUNTIME_LIBRARY}d")
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /${RUNTIME_LIBRARY}")
  endif()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oxt /Zp8 /Gm- /Gy /${RUNTIME_LIBRARY}")

  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-memcmp")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Cygwin")
  add_definitions(-fno-builtin-memcmp -DCYGWIN)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DOS_MACOSX)
elseif(CMAKE_SYSTEM_NAME MATCHES "iOS")
  add_definitions(-DOS_MACOSX -DIOS_CROSS_COMPILE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-DOS_LINUX)
elseif(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  add_definitions(-DOS_SOLARIS)
elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD")
  add_definitions(-DOS_GNU_KFREEBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  add_definitions(-DOS_FREEBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  add_definitions(-DOS_NETBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
  add_definitions(-DOS_OPENBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly")
  add_definitions(-DOS_DRAGONFLYBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
  add_definitions(-DOS_ANDROID)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_definitions(-DWIN32 -DOS_WIN -D_MBCS -DWIN64 -DNOMINMAX)
  if(MINGW)
    add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
  endif()
endif()

if(NOT WIN32)
  add_definitions(-DROCKSDB_PLATFORM_POSIX -DROCKSDB_LIB_IO_POSIX)
endif()

option(WITH_FALLOCATE "build with fallocate" ON)
if(WITH_FALLOCATE)
  CHECK_CXX_SOURCE_COMPILES("
#include <fcntl.h>
#include <linux/falloc.h>
int main() {
 int fd = open(\"/dev/null\", 0);
 fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, 1024);
}
" HAVE_FALLOCATE)
  if(HAVE_FALLOCATE)
    add_definitions(-DROCKSDB_FALLOCATE_PRESENT)
  endif()
endif()

CHECK_CXX_SOURCE_COMPILES("
#include <fcntl.h>
int main() {
  int fd = open(\"/dev/null\", 0);
  sync_file_range(fd, 0, 1024, SYNC_FILE_RANGE_WRITE);
}
" HAVE_SYNC_FILE_RANGE_WRITE)
if(HAVE_SYNC_FILE_RANGE_WRITE)
  add_definitions(-DROCKSDB_RANGESYNC_PRESENT)
endif()

CHECK_CXX_SOURCE_COMPILES("
#include <pthread.h>
int main() {
  (void) PTHREAD_MUTEX_ADAPTIVE_NP;
}
" HAVE_PTHREAD_MUTEX_ADAPTIVE_NP)
if(HAVE_PTHREAD_MUTEX_ADAPTIVE_NP)
  add_definitions(-DROCKSDB_PTHREAD_ADAPTIVE_MUTEX)
endif()

include(CheckCXXSymbolExists)
if(CMAKE_SYSTEM_NAME MATCHES "^FreeBSD")
  check_cxx_symbol_exists(malloc_usable_size malloc_np.h HAVE_MALLOC_USABLE_SIZE)
else()
  check_cxx_symbol_exists(malloc_usable_size malloc.h HAVE_MALLOC_USABLE_SIZE)
endif()
if(HAVE_MALLOC_USABLE_SIZE)
  add_definitions(-DROCKSDB_MALLOC_USABLE_SIZE)
endif()

check_cxx_symbol_exists(sched_getcpu sched.h HAVE_SCHED_GETCPU)
if(HAVE_SCHED_GETCPU)
  add_definitions(-DROCKSDB_SCHED_GETCPU_PRESENT)
endif()

check_cxx_symbol_exists(getauxval "sys/auxv.h" HAVE_AUXV_GETAUXVAL)
if(HAVE_AUXV_GETAUXVAL)
  add_definitions(-DROCKSDB_AUXV_GETAUXVAL_PRESENT)
endif()

check_cxx_symbol_exists(F_FULLFSYNC "fcntl.h" HAVE_FULLFSYNC)
if(HAVE_FULLFSYNC)
  add_definitions(-DHAVE_FULLFSYNC)
endif()

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

if(USE_COROUTINES)
  if(USE_FOLLY OR USE_FOLLY_LITE)
    message(FATAL_ERROR "Please specify exactly one of USE_COROUTINES,"
    " USE_FOLLY, and USE_FOLLY_LITE")
  endif()
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines -Wno-maybe-uninitialized")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-redundant-move")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-memory-model")
  add_compile_definitions(USE_COROUTINES)
  set(USE_FOLLY 1)
endif()

# Main library source code

set(SOURCES
        cache/cache.cc
        cache/cache_entry_roles.cc
        cache/cache_key.cc
        cache/cache_helpers.cc
        cache/cache_reservation_manager.cc
        cache/charged_cache.cc
        cache/clock_cache.cc
        cache/compressed_secondary_cache.cc
        cache/lru_cache.cc
        cache/secondary_cache.cc
        cache/secondary_cache_adapter.cc
        cache/sharded_cache.cc
        cache/tiered_secondary_cache.cc
        db/arena_wrapped_db_iter.cc
        db/attribute_group_iterator_impl.cc
        db/blob/blob_contents.cc
        db/blob/blob_fetcher.cc
        db/blob/blob_file_addition.cc
        db/blob/blob_file_builder.cc
        db/blob/blob_file_cache.cc
        db/blob/blob_file_garbage.cc
        db/blob/blob_file_meta.cc
        db/blob/blob_file_reader.cc
        db/blob/blob_garbage_meter.cc
        db/blob/blob_log_format.cc
        db/blob/blob_log_sequential_reader.cc
        db/blob/blob_log_writer.cc
        db/blob/blob_source.cc
        db/blob/prefetch_buffer_collection.cc
        db/builder.cc
        db/c.cc
        db/coalescing_iterator.cc
        db/column_family.cc
        db/compaction/compaction.cc
        db/compaction/compaction_iterator.cc
        db/compaction/compaction_picker.cc
        db/compaction/compaction_job.cc
        db/compaction/compaction_picker_fifo.cc
        db/compaction/compaction_picker_level.cc
        db/compaction/compaction_picker_universal.cc
        db/compaction/compaction_service_job.cc
        db/compaction/compaction_state.cc
        db/compaction/compaction_outputs.cc
        db/compaction/sst_partitioner.cc
        db/compaction/subcompaction_state.cc
        db/convenience.cc
        db/db_filesnapshot.cc
        db/db_impl/compacted_db_impl.cc
        db/db_impl/db_impl.cc
        db/db_impl/db_impl_write.cc
        db/db_impl/db_impl_compaction_flush.cc
        db/db_impl/db_impl_files.cc
        db/db_impl/db_impl_follower.cc
        db/db_impl/db_impl_open.cc
        db/db_impl/db_impl_debug.cc
        db/db_impl/db_impl_experimental.cc
        db/db_impl/db_impl_readonly.cc
        db/db_impl/db_impl_secondary.cc
        db/db_info_dumper.cc
        db/db_iter.cc
        db/dbformat.cc
        db/error_handler.cc
        db/event_helpers.cc
        db/experimental.cc
        db/external_sst_file_ingestion_job.cc
        db/file_indexer.cc
        db/flush_job.cc
        db/flush_scheduler.cc
        db/forward_iterator.cc
        db/import_column_family_job.cc
        db/internal_stats.cc
        db/logs_with_prep_tracker.cc
        db/log_reader.cc
        db/log_writer.cc
        db/malloc_stats.cc
        db/manifest_ops.cc
        db/memtable.cc
        db/memtable_list.cc
        db/merge_helper.cc
        db/merge_operator.cc
        db/output_validator.cc
        db/periodic_task_scheduler.cc
        db/range_del_aggregator.cc
        db/range_tombstone_fragmenter.cc
        db/repair.cc
        db/seqno_to_time_mapping.cc
        db/snapshot_impl.cc
        db/table_cache.cc
        db/table_properties_collector.cc
        db/transaction_log_impl.cc
        db/trim_history_scheduler.cc
        db/version_builder.cc
        db/version_edit.cc
        db/version_edit_handler.cc
        db/version_set.cc
        db/wal_edit.cc
        db/wal_manager.cc
        db/wide/wide_column_serialization.cc
        db/wide/wide_columns.cc
        db/wide/wide_columns_helper.cc
        db/write_batch.cc
        db/write_batch_base.cc
        db/write_controller.cc
        db/write_stall_stats.cc
        db/write_thread.cc
        env/composite_env.cc
        env/env.cc
        env/env_chroot.cc
        env/env_encryption.cc
        env/file_system.cc
        env/file_system_tracer.cc
        env/fs_on_demand.cc
        env/fs_remap.cc
        env/mock_env.cc
        env/unique_id_gen.cc
        file/delete_scheduler.cc
        file/file_prefetch_buffer.cc
        file/file_util.cc
        file/filename.cc
        file/line_file_reader.cc
        file/random_access_file_reader.cc
        file/read_write_util.cc
        file/readahead_raf.cc
        file/sequence_file_reader.cc
        file/sst_file_manager_impl.cc
        file/writable_file_writer.cc
        logging/auto_roll_logger.cc
        logging/event_logger.cc
        logging/log_buffer.cc
        memory/arena.cc
        memory/concurrent_arena.cc
        memory/jemalloc_nodump_allocator.cc
        memory/memkind_kmem_allocator.cc
        memory/memory_allocator.cc
        memtable/alloc_tracker.cc
        memtable/hash_linklist_rep.cc
        memtable/hash_skiplist_rep.cc
        memtable/skiplistrep.cc
        memtable/vectorrep.cc
        memtable/wbwi_memtable.cc
        memtable/write_buffer_manager.cc
        monitoring/histogram.cc
        monitoring/histogram_windowing.cc
        monitoring/in_memory_stats_history.cc
        monitoring/instrumented_mutex.cc
        monitoring/iostats_context.cc
        monitoring/perf_context.cc
        monitoring/perf_level.cc
        monitoring/persistent_stats_history.cc
        monitoring/statistics.cc
        monitoring/thread_status_impl.cc
        monitoring/thread_status_updater.cc
        monitoring/thread_status_util.cc
        monitoring/thread_status_util_debug.cc
        options/cf_options.cc
        options/configurable.cc
        options/customizable.cc
        options/db_options.cc
        options/offpeak_time_info.cc
        options/options.cc
        options/options_helper.cc
        options/options_parser.cc
        port/mmap.cc
        port/stack_trace.cc
        table/adaptive/adaptive_table_factory.cc
        table/block_based/binary_search_index_reader.cc
        table/block_based/block.cc
        table/block_based/block_based_table_builder.cc
        table/block_based/block_based_table_factory.cc
        table/block_based/block_based_table_iterator.cc
        table/block_based/block_based_table_reader.cc
        table/block_based/block_builder.cc
        table/block_based/block_cache.cc
        table/block_based/block_prefetcher.cc
        table/block_based/block_prefix_index.cc
        table/block_based/data_block_hash_index.cc
        table/block_based/data_block_footer.cc
        table/block_based/filter_block_reader_common.cc
        table/block_based/filter_policy.cc
        table/block_based/flush_block_policy.cc
        table/block_based/full_filter_block.cc
        table/block_based/hash_index_reader.cc
        table/block_based/index_builder.cc
        table/block_based/index_reader_common.cc
        table/block_based/parsed_full_filter_block.cc
        table/block_based/partitioned_filter_block.cc
        table/block_based/partitioned_index_iterator.cc
        table/block_based/partitioned_index_reader.cc
        table/block_based/reader_common.cc
        table/block_based/uncompression_dict_reader.cc
        table/block_fetcher.cc
        table/cuckoo/cuckoo_table_builder.cc
        table/cuckoo/cuckoo_table_factory.cc
        table/cuckoo/cuckoo_table_reader.cc
        table/format.cc
        table/get_context.cc
        table/iterator.cc
        table/merging_iterator.cc
        table/compaction_merging_iterator.cc
        table/meta_blocks.cc
        table/persistent_cache_helper.cc
        table/plain/plain_table_bloom.cc
        table/plain/plain_table_builder.cc
        table/plain/plain_table_factory.cc
        table/plain/plain_table_index.cc
        table/plain/plain_table_key_coding.cc
        table/plain/plain_table_reader.cc
        table/sst_file_dumper.cc
        table/sst_file_reader.cc
        table/sst_file_writer.cc
        table/table_factory.cc
        table/table_properties.cc
        table/two_level_iterator.cc
        table/unique_id.cc
        test_util/sync_point.cc
        test_util/sync_point_impl.cc
        test_util/testutil.cc
        test_util/transaction_test_util.cc
        tools/block_cache_analyzer/block_cache_trace_analyzer.cc
        tools/dump/db_dump_tool.cc
        tools/io_tracer_parser_tool.cc
        tools/ldb_cmd.cc
        tools/ldb_tool.cc
        tools/sst_dump_tool.cc
        tools/trace_analyzer_tool.cc
        trace_replay/block_cache_tracer.cc
        trace_replay/io_tracer.cc
        trace_replay/trace_record_handler.cc
        trace_replay/trace_record_result.cc
        trace_replay/trace_record.cc
        trace_replay/trace_replay.cc
        util/async_file_reader.cc
        util/cleanable.cc
        util/coding.cc
        util/compaction_job_stats_impl.cc
        util/comparator.cc
        util/compression.cc
        util/compression_context_cache.cc
        util/concurrent_task_limiter_impl.cc
        util/crc32c.cc
        util/data_structure.cc
        util/dynamic_bloom.cc
        util/hash.cc
        util/murmurhash.cc
        util/random.cc
        util/rate_limiter.cc
        util/ribbon_config.cc
        util/slice.cc
        util/file_checksum_helper.cc
        util/status.cc
        util/stderr_logger.cc
        util/string_util.cc
        util/thread_local.cc
        util/threadpool_imp.cc
        util/udt_util.cc
        util/write_batch_util.cc
        util/xxhash.cc
        utilities/agg_merge/agg_merge.cc
        utilities/backup/backup_engine.cc
        utilities/blob_db/blob_compaction_filter.cc
        utilities/blob_db/blob_db.cc
        utilities/blob_db/blob_db_impl.cc
        utilities/blob_db/blob_db_impl_filesnapshot.cc
        utilities/blob_db/blob_dump_tool.cc
        utilities/blob_db/blob_file.cc
        utilities/cache_dump_load.cc
        utilities/cache_dump_load_impl.cc
        utilities/cassandra/cassandra_compaction_filter.cc
        utilities/cassandra/format.cc
        utilities/cassandra/merge_operator.cc
        utilities/checkpoint/checkpoint_impl.cc
        utilities/compaction_filters.cc
        utilities/compaction_filters/remove_emptyvalue_compactionfilter.cc
        utilities/counted_fs.cc
        utilities/debug.cc
        utilities/env_mirror.cc
        utilities/env_timed.cc
        utilities/fault_injection_env.cc
        utilities/fault_injection_fs.cc
        utilities/fault_injection_secondary_cache.cc
        utilities/leveldb_options/leveldb_options.cc
        utilities/memory/memory_util.cc
        utilities/merge_operators.cc
        utilities/merge_operators/bytesxor.cc
        utilities/merge_operators/max.cc
        utilities/merge_operators/put.cc
        utilities/merge_operators/sortlist.cc
        utilities/merge_operators/string_append/stringappend.cc
        utilities/merge_operators/string_append/stringappend2.cc
        utilities/merge_operators/uint64add.cc
        utilities/object_registry.cc
        utilities/option_change_migration/option_change_migration.cc
        utilities/options/options_util.cc
        utilities/persistent_cache/block_cache_tier.cc
        utilities/persistent_cache/block_cache_tier_file.cc
        utilities/persistent_cache/block_cache_tier_metadata.cc
        utilities/persistent_cache/persistent_cache_tier.cc
        utilities/persistent_cache/volatile_tier_impl.cc
        utilities/simulator_cache/cache_simulator.cc
        utilities/simulator_cache/sim_cache.cc
        utilities/table_properties_collectors/compact_for_tiering_collector.cc
        utilities/table_properties_collectors/compact_on_deletion_collector.cc
        utilities/trace/file_trace_reader_writer.cc
        utilities/trace/replayer_impl.cc
        utilities/transactions/lock/lock_manager.cc
        utilities/transactions/lock/point/point_lock_tracker.cc
        utilities/transactions/lock/point/point_lock_manager.cc
        utilities/transactions/lock/range/range_tree/range_tree_lock_manager.cc
        utilities/transactions/lock/range/range_tree/range_tree_lock_tracker.cc
        utilities/transactions/optimistic_transaction_db_impl.cc
        utilities/transactions/optimistic_transaction.cc
        utilities/transactions/pessimistic_transaction.cc
        utilities/transactions/pessimistic_transaction_db.cc
        utilities/transactions/snapshot_checker.cc
        utilities/transactions/transaction_base.cc
        utilities/transactions/transaction_db_mutex_impl.cc
        utilities/transactions/transaction_util.cc
        utilities/transactions/write_prepared_txn.cc
        utilities/transactions/write_prepared_txn_db.cc
        utilities/transactions/write_unprepared_txn.cc
        utilities/transactions/write_unprepared_txn_db.cc
        utilities/types_util.cc
        utilities/ttl/db_ttl_impl.cc
        utilities/wal_filter.cc
        utilities/write_batch_with_index/write_batch_with_index.cc
        utilities/write_batch_with_index/write_batch_with_index_internal.cc)

list(APPEND SOURCES
  utilities/transactions/lock/range/range_tree/lib/locktree/concurrent_tree.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/keyrange.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/lock_request.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/locktree.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/manager.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/range_buffer.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/treenode.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/txnid_set.cc
  utilities/transactions/lock/range/range_tree/lib/locktree/wfg.cc
  utilities/transactions/lock/range/range_tree/lib/standalone_port.cc
  utilities/transactions/lock/range/range_tree/lib/util/dbt.cc
  utilities/transactions/lock/range/range_tree/lib/util/memarena.cc)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64")
  list(APPEND SOURCES
    util/crc32c_ppc.c
    util/crc32c_ppc_asm.S)
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64")

if(HAS_ARMV8_CRC)
  list(APPEND SOURCES
    util/crc32c_arm64.cc)
endif(HAS_ARMV8_CRC)

list(APPEND SOURCES
  port/port_posix.cc
  env/env_posix.cc
  env/fs_posix.cc
  env/io_posix.cc)

set(ROCKSDB_STATIC_LIB rocksdb${ARTIFACT_SUFFIX})

set(SYSTEM_LIBS ${CMAKE_THREAD_LIBS_INIT})

string(TIMESTAMP TS "%Y-%m-%d %H:%M:%S" UTC)
set(BUILD_DATE "${TS}" CACHE STRING "the time we first built rocksdb")

find_package(Git)

if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  execute_process(WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_SHA COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD )
  execute_process(WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" RESULT_VARIABLE GIT_MOD COMMAND "${GIT_EXECUTABLE}" diff-index HEAD --quiet)
  execute_process(WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_DATE COMMAND "${GIT_EXECUTABLE}" log -1 --date=format:"%Y-%m-%d %T" --format="%ad")
  execute_process(WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_TAG RESULT_VARIABLE rv COMMAND "${GIT_EXECUTABLE}" symbolic-ref -q --short HEAD OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (rv AND NOT rv EQUAL 0)
    execute_process(WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE GIT_TAG COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
else()
  set(GIT_SHA 0)
  set(GIT_MOD 1)
endif()
string(REGEX REPLACE "[^0-9a-fA-F]+" "" GIT_SHA "${GIT_SHA}")
string(REGEX REPLACE "[^0-9: /-]+" "" GIT_DATE "${GIT_DATE}")

set(BUILD_VERSION_CC ${CMAKE_BINARY_DIR}/build_version.cc)
configure_file(util/build_version.cc.in ${BUILD_VERSION_CC} @ONLY)

add_library(${ROCKSDB_STATIC_LIB} STATIC ${SOURCES} ${BUILD_VERSION_CC})
target_include_directories(${ROCKSDB_STATIC_LIB} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_link_libraries(${ROCKSDB_STATIC_LIB} PRIVATE
  ${THIRDPARTY_LIBS} ${SYSTEM_LIBS})

set(ROCKSDB_LIB ${ROCKSDB_STATIC_LIB})

if(NOT WIN32 OR ROCKSDB_INSTALL_ON_WINDOWS)
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
      # Change default installation prefix on Linux to /usr
      set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    endif()
  endif()

  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  set(package_config_destination ${CMAKE_INSTALL_LIBDIR}/cmake/rocksdb)

  configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/RocksDBConfig.cmake.in RocksDBConfig.cmake
    INSTALL_DESTINATION ${package_config_destination}
  )

#  write_basic_package_version_file(
#    RocksDBConfigVersion.cmake
#    VERSION ${rocksdb_VERSION}
#    COMPATIBILITY SameMajorVersion
#  )

  configure_file(
    ${PROJECT_NAME}.pc.in
    ${PROJECT_NAME}.pc
    @ONLY
  )

  install(DIRECTORY include/rocksdb COMPONENT devel DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

  foreach (plugin ${PLUGINS})
    foreach (header ${${plugin}_HEADERS})
      install(FILES plugin/${plugin}/${header} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rocksdb/plugin/${plugin})
    endforeach()
  endforeach()

  install(DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/modules" COMPONENT devel DESTINATION ${package_config_destination})

  install(
    TARGETS ${ROCKSDB_STATIC_LIB}
    EXPORT RocksDBTargets
    COMPONENT devel
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )

  install(
    EXPORT RocksDBTargets
    COMPONENT devel
    DESTINATION ${package_config_destination}
    NAMESPACE RocksDB::
  )

  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/RocksDBConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/RocksDBConfigVersion.cmake
    COMPONENT devel
    DESTINATION ${package_config_destination}
  )

  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )
endif()

