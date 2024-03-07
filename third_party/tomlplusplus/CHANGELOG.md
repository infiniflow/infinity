# Changelog

<!--
template:

## vX.X.X

[Released](https://github.com/marzer/tomlplusplus/releases/tag/vX.X.X) YYYY-MM-DD

#### Fixes:

#### Additions:

#### Changes:

#### Removals:

#### Build system:

<br><br>

-->

## Unreleased

#### Fixes

-   fixed `value_flags` not being preserved correctly when inserting into tables and arrays (#108) (@LebJe)
-   fixed `toml::value::flags()` not being cleared when `std::move`-ing a value
-   fixed error in README (#195) (@andrewkcorcoran)
-   fixed compiler error when using NVCC (#198) (@thompsonnoahe)

#### Changes:

-   renamed header files to have `.hpp` extension (`toml.h` is still present for backwards-compatibility)

#### Build system:

-   fixed meson builds with `-Ddefault_library=static` having hidden symbols on GNU compilers (#201) (@vlad0x00)

<br><br>

## v3.3.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v3.3.0) 2023-01-29

#### Fixes:

-   fixed null pointer dereference in parser when exceptions are disabled (#169) (@ncaklovic)
-   fixed spurious warnings in MSVC 19.34
-   fixed `toml::parse_file()` on windows for non-ASCII paths
-   fixed a spurious table redefinition error (#187) (@jorisvr)
-   fixed UB edge-case in integer parsing (#188) (@jorisvr)
-   fixed some build issues with Apple-flavoured Clang (#189) (@eddelbuettel)

#### Additions:

-   added `toml::format_flags::terse_key_value_pairs`
-   added `TOML_ENABLE_FLOAT16` config (#178) (@Scrumplex)

#### Removals:

-   removed automatic detection of `_Float16` (you must explicitly set `TOML_ENABLE_FLOAT16` to enable it) (#186) (@benthetechguy)

#### Build system:

-   re-wrote the meson scripts to fix a number of issues (#185, #186) (@Tachi107, @benthetechguy)
-   increased the minimum required meson version to `0.61.0`

<br><br>

## v3.2.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v3.2.0) 2022-08-29

#### Fixes:

-   fixed `[dotted.table]` source columns sometimes being off by one (#152) (@vaartis)
-   fixed spurious `Wnull-dereference` warning on GCC (#164) (@zaporozhets)
-   fixed `print_to_stream` ambiguity for `size_t` (#167) (@acronce)

#### Additions:

-   added value type deduction to `emplace()` methods
-   added `toml::path` utility type (#153, #156, #168) (@jonestristand, @kcsaul)
-   added config option `TOML_CALLCONV`
-   added missing relational operators for `source_position`

#### Changes:

-   relaxed cvref requirements of `is_homogeneous()`, `emplace()`, `emplace_back()`, `emplace_hint()`
-   relaxed mantissa and digits10 requirements of extended float support

<br><br>

## v3.1.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v3.1.0) 2022-04-22

#### Fixes:

-   fixed potential segfault when calling `at_path()` with an empty string
-   fixed UB in internal unicode machinery (#144) (@kchalmer)
-   fixed a number of spurious warnings with Clang 10 (#145, #146) (@chronoxor)

#### Additions:

-   added `toml::array::for_each()`
-   added `toml::table::for_each()`
-   added config options `TOML_EXPORTED_CLASS`, `TOML_EXPORTED_MEMBER_FUNCTION`, `TOML_EXPORTED_STATIC_FUNCTION` &amp; `TOML_EXPORTED_FREE_FUNCTION`
-   added support for escape sequence `\e` when using `TOML_ENABLE_UNRELEASED_FEATURES` ([toml/790](https://github.com/toml-lang/toml/pull/790))
-   added support for more unicode in bare keys when using `TOML_ENABLE_UNRELEASED_FEATURES` ([toml/891](https://github.com/toml-lang/toml/pull/891))

#### Removals/Deprecations:

-   deprecated old `TOML_API` option in favour new `TOML_EXPORTED_X` options
    (it will continue to work as it did before if none of the new function export options are defined)

#### Build system:

-   meson: added `compile_library` option (@Tachi107)
-   meson: added `ubsan_tests` and `ubsan_examples` options
-   meson: use system dependencies where available when building tests (@Tachi107)

<br><br>

## v3.0.1

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v3.0.1) 2022-01-13

This is a single-bugfix release to fix an ODR issue for people using header-only mode in multiple
translation units. If you aren't seeing linker errors because of `toml::array::insert_at()`,
this release holds nothing of value over v3.0.0.

#### Fixes:

-   fixed erroneous use of `TOML_API` causing ODR issue (#136) (@Azarael)

<br><br>

## v3.0.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v3.0.0) 2022-01-11

This release will be a major version bump, so it's ABI breaks all around.
Any changes that are likely to cause migration issues (API changes, build system breakage, etc.) are indicated with ⚠&#xFE0F;.

#### Fixes:

-   ⚠&#xFE0F; fixed `toml::table` init-list constructor requiring double-brackets
-   ⚠&#xFE0F; fixed `TOML_API` + extern templates causing linker errors in some circumstances
-   ⚠&#xFE0F; fixed incorrect `noexcept` specifications on many functions
-   ⚠&#xFE0F; fixed missing `TOML_API` on some interfaces
-   fixed `toml::json_formatter` not formatting inf and nan incorrectly
-   fixed a number of spec conformance issues (#127, #128, #129, #130, #131, #132, #135) (@moorereason)
-   fixed an illegal table redefinition edge case (#112) (@python36)
-   fixed documentation issues
-   fixed GCC bug causing memory leak during parse failures (#123, #124) (@rsmmr, @ronalabraham)
-   fixed incorrect handling of vertical whitespace in keys when printing TOML to streams
-   fixed incorrect source position in redefinition error messages
-   fixed missing includes `<initializer_list>`, `<utility>`
-   fixed parser not correctly round-tripping the format of binary and octal integers in some cases
-   fixed some incorrect unicode scalar sequence transformations (#125)
-   fixed strong exception guarantee edge-cases in `toml::table` and `toml::array`

#### Additions:

-   added value flags to array + table insert methods (#44) (@levicki)
-   added support for Unicode 14.0
-   added support for ref categories and cv-qualifiers in `toml::node::ref()`
-   added magic `toml::value_flags` constant `toml::preserve_source_value_flags`
-   added clang's enum annotation attributes to all enums
-   added `TOML_ENABLE_FORMATTERS` option
-   added `toml::yaml_formatter`
-   added `toml::value` copy+move constructor overloads with flags override
-   added `toml::table::prune()`
-   added `toml::table::lower_bound()` (same semantics as `std::map::lower_bound()`)
-   added `toml::table::emplace_hint()` (same semantics as `std::map::emplace_hint()`)
-   added `toml::table::at()` (same semantics as `std::map::at()`)
-   added `toml::node_view::operator==`
-   added `toml::key` - provides a facility to access the source_regions of parsed keys (#82) (@vaartis)
-   added `toml::is_key<>` and `toml::is_key_or_convertible<>` metafunctions
-   added `toml::format_flags::relaxed_float_precision` (#89) (@vaartis)
-   added `toml::format_flags::quote_infinities_and_nans`
-   added `toml::format_flags::indent_sub_tables` (#120) (@W4RH4WK)
-   added `toml::format_flags::indent_array_elements` (#120) (@W4RH4WK)
-   added `toml::format_flags::allow_unicode_strings`
-   added `toml::format_flags::allow_real_tabs_in_strings`
-   added `toml::format_flags::allow_octal_integers`
-   added `toml::format_flags::allow_hexadecimal_integers`
-   added `toml::format_flags::allow_binary_integers`
-   added `toml::date_time` converting constructors from `toml::date` and `toml::time`
-   added `toml::at_path()`, `toml::node::at_path()` and `toml::node_view::at_path()` for qualified path-based lookups (#118) (@ben-crowhurst)
-   added `toml::array::resize()` param `default_init_flags`
-   added `toml::array::replace()` (#109) (@LebJe)
-   added `toml::array::prune()`
-   added `toml::array::at()` (same semantics as `std::vector::at()`)
-   added `parse_benchmark` example
-   added `operator->` to `toml::value` for class types

#### Changes:

-   ⚠&#xFE0F; `toml::format_flags` is now backed by `uint64_t` (was previously `uint8_t`)
-   ⚠&#xFE0F; `toml::source_index` is now an alias for `uint32_t` unconditionally (was previously dependent on `TOML_LARGE_FILES`)
-   ⚠&#xFE0F; `toml::table` now uses `toml::key` as the key type (was previously `std::string`)
-   ⚠&#xFE0F; `toml::value_flags` is now backed by `uint16_t` (was previously `uint8_t`)
-   ⚠&#xFE0F; made all overloaded operators 'hidden friends' where possible
-   ⚠&#xFE0F; renamed `toml::default_formatter` to `toml::toml_formatter` (`toml::default_formatter` is now an alias)
-   ⚠&#xFE0F; renamed `TOML_PARSER` option to `TOML_ENABLE_PARSER` (`TOML_PARSER` will continue to work but is deprecated)
-   ⚠&#xFE0F; renamed `TOML_UNRELEASED_FEATURES` to `TOML_ENABLE_UNRELEASED_FEATURES` (`TOML_UNRELEASED_FEATURES` will continue to work but is deprecated)
-   ⚠&#xFE0F; renamed `TOML_WINDOWS_COMPAT` to `TOML_ENABLE_WINDOWS_COMPAT` (`TOML_WINDOWS_COMPAT` will continue to work but is deprecated)
-   applied clang-format to all the things 🎉&#xFE0F;
-   exposed `TOML_NAMESPACE_START` and `TOML_NAMESPACE_END` macros to help with ADL specialization scenarios
-   improved performance of parser
-   made date/time constructors accept any integral types
-   moved all implementation headers to `/impl`
-   renamed all implementation headers to `.h` and 'source' headers to `.inl`
-   updated conformance tests

#### Removals:

-   ⚠&#xFE0F; removed `toml::format_flags::allow_value_format_flags`
-   ⚠&#xFE0F; removed `TOML_LARGE_FILES` (it is now default - explicitly setting `TOML_LARGE_FILES` to `0` will invoke an `#error`)
-   ⚠&#xFE0F; removed unnecessary template machinery (esp. where ostreams were involved)
-   removed unnecessary uses of `final`

#### Build system:

-   ⚠&#xFE0F; increased minimum required meson version to `0.54.0`
-   disabled 'install' path when being used as a meson subproject (#114) (@Tachi107)
-   fixed builds failing with meson 0.6.0 (#117) (@Tachi107)
-   general meson improvements and fixes (#115) (@Tachi107)
-   used `override_dependency` where supported (#116) (@Tachi107)

<br><br>

## v2.5.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v2.5.0) 2021-07-11

#### Fixes:

-   fixed linkage error with windows compat mode
-   fixed `TOML_CONSTEVAL` broken in MSVC (again)
-   fixed minor documentation bugs
-   fixed cmake project version being incorrect (#110) (@GiulioRomualdi)

#### Additions:

-   added support for lowercase 't' and 'z' in datetimes (per spec)
-   added natvis file to cmake install (#106) (@Ryan-rsm-McKenzie)
-   added VS cpp.hint file to cmake install
-   added metafunctions `is_container`, `is_chronological`, `is_value`, `is_node`, `inserted_type_of`

#### Changes:

-   improved debug code size by removing unnecessary std::forwards and std::moves
-   modernized the CMake build files (#102, #103, #105) (@friendlyanon)
-   updated conformance tests

<br><br>

## v2.4.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v2.4.0) 2021-05-19

#### Fixes:

-   fixed `node::value()` not retrieving inf and nan correctly
-   fixed dotted kvps being unable to add subtables (#61) (@Validark)
-   fixed linker error on linux ICC (#83) (@blackwer)
-   fixed segfault JSON-formatting a failed `parse_result` (#96) (@proydakov)
-   fixed spurious newline after JSON formatting a table
-   fixed VS intellisense not detecting `TOML_COMPILER_EXCEPTIONS` correctly
-   fixed crash with pathologically-nested inputs (#100) (@geeknik)
-   fixed `parse_result` natvis
-   fixed false-positive `char8_t` support detection on older compilers
-   fixed unnecessary `#include <Windows.h>` Windows builds (@BeastLe9enD)
-   fixed `TOML_CONSTEVAL` breaking on VS 16.10.0pre2
-   fixed spurious warnings with MSVC /Wall
-   fixed missing blank lines between consecutive empty tables/A-o-T
-   fixed unnecessary `TOML_API` declarations
-   fixed many small documentation issues

#### Additions:

-   added proper cmake support (#85) (@ClausKlein)
-   added cmake FetchContent information to documentation (#101) (@proydakov)

#### Removals:

-   removed explicit `#include <fstream>` requirement for `parse_file()`

<br><br>

## v2.3.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v2.3.0) 2020-12-29

#### Fixes:

-   fixed compiler errors caused by `<charconv>` with Apple-flavoured clang
-   fixed array and table iterators missing `iterator_category` (#77) (@HazardyKnusperkeks)
-   fixed `Wuseless-cast` warnings on GCC 10 (#75) (@HazardyKnusperkeks)
-   fixed formatter not correctly line wrapping in some rare circumstances (#73) (@89z)
-   fixed an unnecessary global compiler flag breaking builds when used as a meson subproject (#72) (@jamabr)
-   fixed link error caused by `<charconv>` on emscripten (#71) (@suy)
-   fixed ambiguity with the `toml::literals` inline namespace (#69) (@std-any-emplace)
-   fixed formatter emitting superfluous newlines after printing tables (#68) (@std-any-emplace)
-   fixed array and table iterators not converting between const and non-const versions of themselves (#67) (@std-any-emplace)
-   fixed some parser crashes when given pathologically-malformed UTF-8 (#65) (@sneves)

<br><br>

## v2.2.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v2.2.0) 2020-08-09

#### Fixes:

-   fixed some issues building with VS2017 (#55) (@sobczyk)
-   fixed `_Float16` erroneously detected as supported on g++ (#57) (@sobczyk)
-   fixed `<Windows.h>` causing compilation failure on mingw (#63) (@rezahousseini)
-   fixed CMake and pkg-config files not being installed into architecture-agnostic directories (#59) (@tambry)
-   fixed memory leak during parsing (#64) (@sneves)
-   fixed ambiguous `operator==` error on MSVC (#56) (@HellsingDarge)

#### Additions:

-   added additional node_view constructors
-   added ability to specify serialization format of integer values
-   added integer value serialization format round trip (e.g. hex in, hex out)

#### Changes:

-   updated conformance tests
-   TOML version bump to v1.0.0-rc.3
-   refactors and cleanups based on feedback given [here](https://medium.com/@julienjorge/code-review-of-toml-f816a6071120)

#### Build system:

-   renamed build options to `snake_case`
-   tests, examples and cmake config now explicitly disabled when used as a subproject
-   removed small_binaries (it's now implicit when building as release)
-   bumped minimum meson version to 0.53

<br><br>

## v2.1.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v2.1.0) 2020-07-11

#### Fixes:

-   fixed inconsistent emission of leading/trailing newlines when writing a table to an ostream (#48) (@levicki)
-   fixed `Wcast-align` warning spam on ARM
-   fixed `array::insert` not working correctly in some cases
-   fixed `node::value_or()` not having the same semantics as `node::value()` (#50) (@whiterabbit963)
-   fixed 'misleading assignment' of rvalue node_views (#52) (@Reedbeta)
-   fixed some issues handling infinities and NaNs (#51) (@Reedbeta)
-   fixed some minor documentation issues

#### Additions:

-   added support for `__fp16`, `_Float16`, `__float128`, `__int128_t` and `__uint128_t`
-   added copy construction/assignment for arrays, tables and values
-   added insert, emplace, push_back etc. compatibility with node_views
-   added `node::is_homogenous`
-   added `table::is_homogenous`
-   added `value::is_homogenous` (just for generic code's sake)
-   added `is_homogenous` overload for identifying failure-causing element
-   added implicit conversion operator from `node` to `node_view` (#52) (@Reedbeta)

#### Changes:

-   renamed `TOML_ALL_INLINE` to `TOML_HEADER_ONLY` (the old name will still work, but is no longer documented)
-   general cleanup

<br><br>

## v2.0.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v2.0.0) 2020-07-20

This release contains a fairly significant number of 'quality of life' improvements, yay! But also necessitates an ABI
break (hence the version number bump). Changes that might block a migration are annotated with ⚠&#xFE0F;.

#### Fixes:

-   fixed infinity and NaN-related code breaking when using `-ffast-math` and friends
-   fixed narrowing conversion warnings when constructing int values from unsigned
-   fixed Visual Studio debugger native visualizations for `date`, `time`, `time_offset`, `date_time`
-   fixed some static assert messages being badly formatted on clang
-   fixed internal macro `assert_or_assume` leaking out of `toml_parser.hpp`

#### Additions:

-   added additional types allowed in `node::value()` and `node::value_or()` ([see `value()` dox for examples](https://marzer.github.io/tomlplusplus/classtoml_1_1node.html#ab144c1ae90338b6b03f6af0574c87993))
-   added additional types allowed in `node_view::value()` and `node_view::value_or()`
-   added `node::value_exact()` and `node_view::value_exact()`
-   added support for interop with wide strings on Windows:
    -   added wide-string path arg overloads of `parse()` and `parse_file()`
    -   added wide-string support to all relevant `table` and `array` ops
    -   added wide-string support to `node::value(), node::value_or()`
    -   added wide-string support to `node_view::value(), node_view::value_or()`
    -   added wide-string support to `value<string>` constructor
    -   added wide-string overloads of `node_view::operator[]`
    -   added `source_region::wide_path()`
    -   added `TOML_WINDOWS_COMPAT` switch for explicitly enabling/disabling this stuff
-   added emission of 'literal' strings to the TOML serializer
-   added lots of minor documentation fixes and improvements
-   added Visual Studio debugger native visualizations for `table`, `array`, `parse_result`, and `parse_error` (#46) (@Reedbeta)
-   added non-template version of `array::is_homogeneous()`
-   added explicit instantiations of more template types when `!TOML_ALL_INLINE`

#### Changes:

-   ⚠&#xFE0F; deprecated `parse_result::get()` in favour of `parse_result::table()`
-   ⚠&#xFE0F; deprecated `node_view::get()` in favour of `node_view::node()`
-   ⚠&#xFE0F; simplified internal ABI namespaces
-   improved the quality of many static_assert error messages

#### Removals:

-   ⚠&#xFE0F; renamed `date_time::time_offset` to just 'offset'
-   ⚠&#xFE0F; removed `TOML_CHAR_8_STRINGS` since it no longer makes sense

<br><br>

## v1.3.3

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.3.3) 2020-06-29

#### Fixes:

-   fixed some minor TOML spec conformance bugs
-   fixed BOM check causing EOF on very short iostream inputs
-   fixed `std::numeric_limits::max()` getting broken by macros in some environments
-   fixed 'unknown pragma' warning spam in older versions of GCC
-   fixed a few minor documentation issues

#### Additions:

-   added rvalue overload of `array::flatten`
-   added conformance tests from `BurntSushi/toml-test` and `iarna/toml-spec-tests`
-   added `toml::inserter` as a workaround for nested construction of single-element `toml::arrays` performing move-construction instead
-   added license boilerplate to test files

#### Changes:

-   refactored the parser to reduce binary size

<br><br>

## v1.3.2

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.3.2) 2020-06-19

#### Fixes:

-   fixed single-digit negative integers parsing as positive
-   fixed parse failure when parsing an empty file
-   fixed multi-line strings being allowed in keys
-   fixed overflow for very long binary integer literals

#### Changes:

-   improved the performance of toml::parse_file
-   improved the performance of printing to streams for deepy-nested TOML data

<br><br>

## v1.3.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.3.0) 2020-06-02

#### Fixes:

-   fixed `formatter::print_inline()` causing compilation failures in DLL builds
-   fixed BOMs occasionally causing overflow/crash in char8 mode
-   fixed some spurious warnings in GCC 10
-   fixed clang static analyzer warning in BOM handling code

#### Additions:

-   added `table_iterator::operator ->`
-   added `array::resize()` and `array::truncate()`
-   added `array::capacity()`, `array::shrink_to_fit()`, `array::max_size()`
-   added non-const -> const conversion for table and array iterators

#### Changes:

-   renamed table iterator proxy pair members to `first` and `second` to match STL

<br><br>

## v1.2.5

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.2.5) 2020-04-24

#### Fixes:

-   fixed some multi-line string parsing issues
-   fixed pedantic warnings on gcc 10 and clang 11
-   fixed `is_unicode_XXXXXX` functions being wrong in some cases
-   fixed `TOML_LIKELY` not being correct on older versions of gcc and clang
-   fixed minor documentation issues (#26, #38) (@prince-chrismc)

#### Additions:

-   added additional error message cases to the parser
-   added `error_printer` example
-   added `toml_generator` example

#### Changes:

-   improved unicode-related codegen

<br><br>

## v1.2.3

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.2.3) 2020-04-11

#### Fixes:

-   fixed printing of inf and nan
-   fixed parser not handling floats with leading '.' characters
-   fixed pedantic vtable warnings on clang with -Weverything
-   fixed a number of documentation bugs
-   fixed `TOML_UNRELEASED_FEATURES` default being 1 (it should have been 0)

#### Additions:

-   added `TOML_PARSER` configuration option
-   added `TOML_LIB_SINGLE_HEADER` indicator
-   added doxygen page for the configuration options
-   added SPDX-License-Identifiers around the place

#### Changes:

-   split some header files up to make future maintenance easier
-   refactored and greatly simplified parser

<br><br>

## v1.2.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.2.0) 2020-04-07

#### Fixes:

-   fixed some parsing and printing ops being locale-dependent
-   fixed some parsing errors at EOF when `TOML_EXCEPTIONS = 0`
-   fixed some unreferenced variable warnings on older compilers
-   fixed some 'maybe-uninitialized' false-positives on GCC9
-   fixed pkgconfig subdir being wrong

#### Additions:

-   added support for implementations without `<charconv>`
-   added cmake package config generator (#22) (@GiulioRomualdi)
-   added build config feature option `GENERATE_CMAKE_CONFIG`
-   added many new tests

<br><br>

## v1.1.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v1.1.0) 2020-04-03

#### Fixes:

-   fixed some parser error paths not returning early enough `TOML_EXCEPTIONS=0`
-   fixed a number of minor documentation issues

#### Additions:

-   added support for [TOML 1.0.0-rc.1](https://github.com/toml-lang/toml/releases/tag/v1.0.0-rc.1) 🎉
-   added `operator[]`, `begin()`, `end()` to `toml::parse_result` for `TOML_EXCEPTIONS=0`
-   added additional compilation speed improvements for `TOML_ALL_INLINE=0`
-   added more specific error messages for parsing errors relating to prohibited codepoints
-   added a large number of additional tests
-   added support for installation with meson (#16) (@ximion)
-   added the array and table iterators to the `toml` namespace

<br><br>

## v1.0.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/1.0.0) 2020-03-28

#### Fixes:

-   fixed minor documentation issues

#### Changes:

-   refactoring of ABI-based inline namespaces

<br><br>

## v0.6.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.6.0) 2020-03-24

#### Fixes:

-   fixed minor preprocessor/macro issues
-   fixed minor documentation issues

#### Additions:

-   added `<cassert>` include directly in 'debug' builds when `TOML_ASSERT` isn't defined
-   added Clang's `[[trivial_abi]]` attribute to `date`, `time`, `time_offset`

<br><br>

## v0.5.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.5.0) 2020-03-18

#### Fixes:

-   fixed crash when reaching EOF while parsing a string when exceptions are disabled
-   fixed some attribute warnings in GCC
-   fixed build with GCC 8.2.0 (#15) (@shdnx)
-   fixed exception mode detection sometimes being incorrect on MSVC
-   fixed compilation on older implementations without `std::launder`
-   fixed `json_formatter` type deduction on older compilers

#### Additions:

-   added support for Unicode 13.0
-   added support for `\xHH` escape sequences ([toml/pull/796](https://github.com/toml-lang/toml/pull/796))
-   added short-form license preamble to all source files
-   added build configuration option for compiling examples

<br><br>

## v0.4.3

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.4.3) 2020-03-10

#### Fixes:

-   fixed ICE in VS2019 when using `/std:c++17` instead of `/std:c++latest`

#### Additions:

-   added `#error` when `TOML_EXCEPTIONS` is set to `1` but compiler exceptions were disabled

#### Changes:

-   parsing performance improvements

<br><br>

## v0.4.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.4.0) 2020-03-05

#### Fixes:

-   fixed `parse_file()` failing to compile with plain string literals
-   fixed tests being built when used as a meson subproject (#14) (@shdnx)

#### Additions:

-   added support for compiling into DLLs on windows (`TOML_API`)
-   added support for explicitly setting the `TOML_EXCEPTION` mode
-   added `TOML_OPTIONAL_TYPE` customization point
-   added `node::ref()` and `node_view::ref()`

<br><br>

## v0.3.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.3.0) 2020-03-01

#### Fixes:

-   fixed some pedantic clang warnings
-   fixed some minor documentation errors

#### Additions:

-   added `node::value()` and `node::value_or()`
-   added `node_view::value()`
-   added relops for the date/time classes
-   added `TOML_ALL_INLINE` and `TOML_IMPLEMENTATION` options
-   added preliminary support for ICC

#### Removals:

-   removed `<cmath>` dependency

<br><br>

## v0.2.1

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.2.1) 2020-02-26

#### Fixes:

-   fixed minor printing bug in `operator<<(ostream, source_position)`
-   fixed minor documentation issues

#### Additions:

-   added `operator<<(ostream&, parse_error)`

#### Changes:

-   improved quality of error messages for boolean and inf/nan parsing

<br><br>

## v0.2.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.2.0) 2020-02-23

#### Fixes:

-   fixed truncation of floating-point values when using ostreams
-   fixed missing value deduction guides for dates and times
-   fixed potential ODR issues relating to exception mode handling etc.
-   fixed some documentation issues

#### Additions:

-   added serialization round-trip tests
-   added `node::is_number()`
-   added `node_view::is_number()`
-   added `node_view::value_or()`
-   added hexfloat parsing support for all implementations (not just `<charconv>` ones)

<br><br>

## v0.1.0

[Released](https://github.com/marzer/tomlplusplus/releases/tag/v0.1.0) 2020-02-20

-   First public release, yay! 🎉&#xFE0F;
