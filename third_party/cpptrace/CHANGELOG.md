# Changelog

- [Changelog](#changelog)
- [v0.8.3](#v083)
- [v0.8.2](#v082)
- [v0.8.1](#v081)
- [v0.8.0](#v080)
- [v0.7.5](#v075)
- [v0.7.4](#v074)
- [v0.7.3](#v073)
- [v0.7.2](#v072)
- [v0.7.1](#v071)
- [v0.7.0](#v070)
- [v0.6.3](#v063)
- [v0.6.2](#v062)
- [v0.6.1](#v061)
- [v0.6.0](#v060)
- [v0.5.4](#v054)
- [v0.5.3](#v053)
- [v0.5.2](#v052)
- [v0.5.1](#v051)
- [v0.5.0](#v050)
- [v0.4.1](#v041)
- [v0.4.0](#v040)
- [v0.3.1](#v031)
- [v0.3.0](#v030)
- [v0.2.1](#v021)
- [v0.2.0](#v020)
- [v0.1.1](#v011)
- [v0.1](#v01)

# v0.8.3

Added:
- Added basic JIT support https://github.com/jeremy-rifkin/cpptrace/issues/226
- Added `cpptrace::formatter::transform` https://github.com/jeremy-rifkin/cpptrace/issues/227
- Added support for gcc 4.8.5 https://github.com/jeremy-rifkin/cpptrace/issues/220

Fixed:
- Fixed bug related to calling `dwarf_dealloc` on strings from `dwarf_formstring` and `dwarf_diename` https://github.com/davea42/libdwarf-code/issues/279
- Fixed incorrect cmake version variable https://github.com/jeremy-rifkin/cpptrace/issues/231
- Fixed `address_mode::none` not working https://github.com/jeremy-rifkin/cpptrace/issues/221
- Fixed use of `-Wall` for clang-cl

Other:
- Added ARM CI
- Miscellaneous work on supporting old compilers
- Updated cpptrace cmake target configuration to not add public compile definitions
- Internal refactoring, cleanup, and code improvements

# v0.8.2

Fixed:
- Fixed printing of internal error messages when an object file can't be loaded, mainly affecting MacOS https://github.com/jeremy-rifkin/cpptrace/issues/217

Other:
- Bumped zstd via FetchContent to 1.5.7

# v0.8.1

Fixed:
- Fixed compile error on msvc https://github.com/jeremy-rifkin/cpptrace/issues/215

Added:
- Added `cpptrace::can_get_safe_object_frame()`

Breaking changes:
- Renamed ctrace's `can_signal_safe_unwind` to `ctrace_can_signal_safe_unwind`. This was an oversight. Apologies for
  including a breaking change in a patch release. Github code search suggests this API isn't used in public code, at
  least.

Other:
- Added CI workflow to test on old msvc
- Made some internal improvements on robustness and cleanliness

# v0.8.0

Added:
- Added support for resolving symbols from elf and mach-o symbol tables, allowing function names to be resolved even in
  a build that doesn't include debug information https://github.com/jeremy-rifkin/cpptrace/issues/201
- Added a configurable stack trace formatter https://github.com/jeremy-rifkin/cpptrace/issues/164
- Added configuration options for the libdwarf back-end that can be used to lower memory usage on memory-constrained
  systems https://github.com/jeremy-rifkin/cpptrace/issues/193
- Added `cpptrace::nullable<T>::null_value`
- Made `cpptrace::nullable<T>` member functions conditionally `constexpr` where possible

Fixed:
- Fixed handling of `SymInitialize` when other code has already called `SymInitialize`. `SymInitialize` must only be
  called once per handle and cpptrace now attempts to duplicate the current process handle to avoid conflicts.
  https://github.com/jeremy-rifkin/cpptrace/issues/204
- Fixed a couple of locking edge cases surrounding dbghelp functions
- Fixed improper deallocation of `dwarf_errmsg` in the libdwarf back-end

Breaking changes:
- `cpptrace::get_snippet` previously included a newline at the end but it now does not. This also affects the behavior
  of trace formatting with snippets enabled.

Other:
- Significantly improved memory usage and performance of the libdwarf back-end
- Improved implementation and organization of internal utility types, such as `optional` and `Result`
- Improved trace printing and formatting implementation
- Added unit tests for library internal utilities
- Added logic to the cxxabi demangler to ensure external names begin with `_Z` or `__Z` before attempting to demangle
- Added various internal tools and abstractions to improve maintainability and clarity
- Various internal improvements for robustness
- Added a small handful of utility tool programs that are useful for continued development, maintenance, and debugging
- Improved library CI setup
- Marked the `CPPTRACE_BUILD_BENCHMARK` option as advanced

# v0.7.5

Fixed:
- Fixed missing `<typeinfo>` include https://github.com/jeremy-rifkin/cpptrace/pull/202
- Added `__cdecl` to a terminate handler to appease MSVC under some configurations https://github.com/jeremy-rifkin/cpptrace/issues/197
- Set C++ standard for cmake support checks https://github.com/jeremy-rifkin/cpptrace/issues/200
- Changed hyphens to underscores for cmake component names due to cpack issue https://github.com/jeremy-rifkin/cpptrace/issues/203

# v0.7.4

Added:
- Added `<cpptrace/version.hpp>` header with version macros

Fixes:
- Bumped libdwarf to 0.11.0 which fixes a number of dwarf 5 debug fission issues

Other:
- Various improvements to internal testing setup

# v0.7.3

Fixed:
- Fixed missing include affecting macos https://github.com/jeremy-rifkin/cpptrace/pull/183
- Fixed issue with cmake not using the ccache program found by `find_program` https://github.com/jeremy-rifkin/cpptrace/pull/184
- Fixed missing include and warnings affecting mingw https://github.com/jeremy-rifkin/cpptrace/pull/186
- Fixed issue with identifying inlined call frames when the `DW_TAG_inlined_subroutine` is under a `DW_TAG_lexical_block`
- Fixed a typo in the README
- Improved unittest support on various configurations
- Improved unittest robustness under LTO
- Fixed bug signal_demo in the event `fork()` fails

Added:
- Added color overload for `stacktrace_frame::to_string`
- Added CMake `export()` definition for cpptrace as well as a definition for libdwarf which currently doesn't provide one

Changed:
- Updated documentation surrounding the signal safe API

# v0.7.2

Changes:
- Better support for older CMake with using `FetchContent_Declare` from a URL https://github.com/jeremy-rifkin/cpptrace/pull/176
- Better portability for page size detection https://github.com/jeremy-rifkin/cpptrace/pull/177
- Improved compile times https://github.com/jeremy-rifkin/cpptrace/pull/172
- Split up `cpptrace.hpp` into finer-grained headers for lower compile time impact
- Some minor readme restructuring

# v0.7.1

Added
- Better support for finding libunwind on macos https://github.com/jeremy-rifkin/cpptrace/pull/162
- Support for libbacktrace under mingw https://github.com/jeremy-rifkin/cpptrace/pull/166

Fixed
- Computation of object address for safe object frames https://github.com/jeremy-rifkin/cpptrace/issues/169
- Nested microfmt in cpptrace's namespace due to an ODR problem with libassert https://github.com/jeremy-rifkin/libassert/issues/103
- Compilation on iOS https://github.com/jeremy-rifkin/cpptrace/pull/167
- Compilation on old MSVC https://github.com/jeremy-rifkin/cpptrace/pull/165
- Dbghelp use on 32 bit https://github.com/jeremy-rifkin/cpptrace/issues/170
- Warning in brand new cmake due to `FetchContent_Populate` being deprecated https://github.com/jeremy-rifkin/cpptrace/issues/171

Other changes
- Bumped the buffer size for execinfo and CaptureStackBackTrace to 400 frames
- Switched to execinfo.h for unwinding on clang/apple clang on macos due to `_Unwind` not working with `-fno-exceptions` https://github.com/jeremy-rifkin/cpptrace/issues/161

# v0.7.0

Added
- Added `cpptrace::from_current_exception()` and associated exception handler macros to allow tracing of all exceptions,
  even without cpptrace traced exception objects.

Fixes:
- Fixed issue with using `resolve_safe_object_frame` on `safe_object_frame`s with empty paths
- Fixed handling of dwarf 4 rangelist base addresses when a `DW_AT_low_pc` is not present
- Fixed use of `-g` with MSVC

Other changes:
- Bazel is now supported on linux (https://github.com/jeremy-rifkin/cpptrace/pull/153)
- More work on testing
- Some internal refactoring

# v0.6.3

Added:
- Added a flag to disable inclusion of `<format>` by cpptrace.hpp and the definition of formatter specializations

Fixes:
- Fixed use after free during cleanup of split dwarf information https://github.com/jeremy-rifkin/cpptrace/issues/141
- Fixed an issue with TCO by clang on arm interfering with unwinding skip counts for internal methods
- Fixed issue with incorrect object addresses being reported on macos when debug maps are used
- Fixed issue with handling of split dwarf emitted by clang under dwarf4 mode

Other changes:
- Added note about signal-safe tracing requiring `_dl_find_object` to documentation and fixed errors in the signal-safe
  tracing docs
- Added more configurations to unittest ci setup
- Optimized unittest ci matrix setup
- Added options for zstd and libdwarf sources if FetchContent is being used to bring the dependencies in
- Optimized includes in cpptrace.hpp

# v0.6.2

Fixes:
- Fix an issue with unwinding to collect stack traces during exception creation on arm https://github.com/jeremy-rifkin/cpptrace/issues/134
- Fix issue where `dladdr1` wasn't being used even when detected

Robustness:
- Setup more robust unit tests and added them to CI

# v0.6.1

Fixes:
- Fix for detection of `dladdr1` and `_dl_find_object` support

# v0.6.0

New:
- Added a `cpptrace::system_error` utility
- Added support for musl https://github.com/jeremy-rifkin/cpptrace/issues/128
- Added support for split dwarf / debug fission

Fixes:
- Fixed address formatting in stack traces
- Fixed frame pointer calculation for signal frames from libunwind https://github.com/jeremy-rifkin/cpptrace/issues/123
- Fixed dwarf_ranges handling of lowpc == pc causing erroneous symbol resolution
- Fixed implementation of the exception helper system/reference implementation's `lazy_trace_holder`

# v0.5.4

Fixes:
- Fixed bug with resolving object information when `dladdr` is used and an unexpected `argv[0]` is provided to the
  binary.

# v0.5.3

Fixes:
- Fixed bug with formatting of hex values on MSVC
- Fixed error handling for libbacktrace back-end when debug info is not present
- Fixed bug with cmake resolution of zstd when no zstd cmake config file is installed

Other changes:
- Added error handling for an edge case in the signal tracing demo
- Updated conan recipe to allow libunwind to be chosen
- Improved msvc support in internal formatting system
- Bumped libdwarf to 0.9.2

# v0.5.2

Fixes:
- Fixed bug with resolution of inlined calls

Other changes:
- Improved internal string formatting
- Improved internal error handling

# v0.5.1

Fixes:
- Fix MSVC warning treated as error for 32-bit windows
- Fix MSVC issue with min/max macros
- Fix potential null dereference issue identified by eyalgolan1337

# v0.5.0

New:
- Traces with source code snippets with `cpptrace::stacktrace::print_with_snippets`
- Added `cpptrace::get_snippet` utility
- Added `cpptrace::can_signal_safe_unwind` utility
- Added `stacktrace_frame::get_object_info`

Changes:
- The library is now compiled with position-independent code by default

Fixes:
- Fixed issue with `_dl_find_object` implementation

Misc:
- Various refactoring, cleanup, and improvements

# v0.4.1

Changes:
- Renamed `stacktrace_frame.address` -> `stacktrace_frame.raw_address`
- Added `stacktrace_frame.object_address`
- Fixed segfault due to an edge case with dwarf file table indices
- For the libdwarf back-end: At least show object frame information if resolution fails
- Extremely small performance improvements
- Small documentation updates
- Small fix for conan
- Updated cmake to not FetchContent zstd when using CPPTRACE_USE_EXTERNAL_LIBDWARF
- CI improvements
  - Test the default configuration first before doing the exhaustive and slow matrix of all configurations.
  - Cleanup of duplicated prerequisite installation code
  - Cleanup of built and test python scripts

# v0.4.0

What's new:
- Cpptrace now has a C API! 🎉
- Cpptrace is now able to parse macOS debug maps and resolve stack traces without dSYM files

Most notable improvements:
- Updated cpptrace exception objects to generate traces at the callsite for improved consistency with trace output. As
  part of this cpptrace exception objects have had their constructors updated.
- Improved dwarf back-end robustness
  - Fallback to the compilation-unit cache or walking compilation-units if aranges lookup fails
- Eliminated reliance on a CMake-generated export header
- Added a configuration to control resolution of inlined function calls
- Made architecture selection in Mach-O universal binaries

Other improvements:
- Improved documentation for installation and usage
- Generally improved README content and organization
- Fixed an MSVC workaround producing dozens of warnings
- Better handle compiler color diagnostic arguments if compiler families differ
- Improvements for handling libdwarf's header placement
- Fixed issue with libunwind resolution
- `-Werror` is now used in CI
- More library configurations are now tested in CI
- Updated to libdwarf 9
- Updated the library's CMake to acquire zstd through FetchContent
- Fixed minor issue with stacktrace printing always trying to enable virtual terminal processing, even when not actually
  printing to the terminal.

# v0.3.1

Tiny patch:
- Fix `CPPTRACE_EXPORT` annotations
- Add workaround for [msvc bug][msvc bug] affecting msvc 19.38.

[msvc bug]: https://developercommunity.visualstudio.com/t/MSVC-1938331290-preview-fails-to-comp/10505565

# v0.3.0

Interface Changes:
- Overhauled the API for traced `cpptrace::exception` objects
- Added `cpptrace::isatty` utility
- Added specialized `std::terminate` handler and `cpptrace::register_terminate_handler` utility
- Added `cpptrace::frame_ptr` as an alias for the appropriate type capable of representing an instruction pointer
- Added signal-safe tracing support and a guide for [how to trace safely](signal-safe-tracing.md)
- Added `cpptrace::nullable<T>` utility for better indicating when line / column information is not present
- Added `CPPTRACE_FORCE_NO_INLINE` utility macro to cpptrace.hpp
- Added `CPPTRACE_WRAP` and `CPPTRACE_WRAP_BLOCK` utilities to catch non-`cpptrace::exception`s and rethrow wrapped in a
  traced exception.
- Updated `cpptrace::stacktrace::to_string` to take a `bool color` parameter
- Eliminated uses of `std::uint_least32_t` in favor of other types
- Updated `object_frame` data member names

Other changes:
- Added object resolution with `_dl_find_object` which is much faster than `dladdr`
- Added column support for dwarf
- Added inlined call resolution
  - Added `cpptrace::stacktrace_frame::is_inline`
- Added libunwind as a back-end
- Unbundled libdwarf
- Increased hard max frame count, used by some back-end requiring fixed buffers, from 100 to 200
- Improved libgcc unwind backend
- Improved trace output when information is missing
- Added a lookup table for faster dwarf line information lookup

News:
- The library is now on conan and vcpkg

Minor changes:
- Assorted bug fixes
- Various code quality improvements
- CI improvements
- Documentation improvements
- CMake improvements
- Internal refactoring

# v0.2.1

Patches:
- Fixed uintptr_t implicit conversion issue for msvc
- Better handling for PIC and static linkage in CMake
- Added gcc 5 support
- Various warning fixes
- Added stackwalk64 support for 32-bit x86 mingw/clang and architecture detection
- Added check for stackwalk64 support and CaptureStackBacktrace as a fallback
- Various cmake cleanup and changes to use cpptrace through package managers
- Added sonarlint and implemented some sonarlint fixes

# v0.2.0

Key changes:
- Added libdwarf as a back-end so cpptrace doesn't have to rely on addr2line or libbacktrace
- Overhauled library's public-facing interface to make the library more useful
  - Added `raw_trace` interface
  - Added `object_trace` interface
  - Added `stacktrace` interface
  - Updated `generate_trace` to return a `stacktrace` rather than a vector of frames
  - Added `generate_trace` counterparts for raw and object traces
  - Added `generate_trace` overloads with max_depth
  - Added interface for internal demangling utility
  - Added cache mode configuration
  - Added option to absorb internal trace exceptions (by default it absorbs)
  - Added `cpptrace::exception`, which automatically generates and stores a stacktrace when thrown
  - Added `exception_with_message`
  - Added traced analogs for stdexcept errors: `logic_error`, `domain_error`, `invalid_argument`, `length_error`,
    `out_of_range`, `runtime_error`, `range_error`, `overflow_error`, and `underflow_error`.

Other changes:
- Bundled libdwarf with cpptrace so the library can essentially be self-contained and not have to rely on libraries that
  might not already be on a system
- Added StackWalk64 as an unwinding back-end on windows
- Added system for multiple symbol back-ends to be used, mainly for more complete stack traces on mingw
- Fixed sporadic line number reporting errors due to not adjusting the program counter from the unwinder
- Improved addr2line/atos invocation back-end on macos
- Lots of error handling improvements
- Performance improvements
- Updated default back-ends for most systems
- Removed full tracing backends
- Cleaned up library cmake
- Lots of internal cleanup and refactoring
- Improved library usage instructions in README

# v0.1.1

Fixed:
- Handle errors when object files don't exist or can't be opened for reading
- Handle paths with spaces when using addr2line on windows

# v0.1

Initial release of the library 🎉
