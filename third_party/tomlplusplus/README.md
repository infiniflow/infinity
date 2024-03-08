[![banner](docs/images/banner.png)][homepage]
[![Releases](https://img.shields.io/github/v/release/marzer/tomlplusplus?style=flat-square)](https://github.com/marzer/tomlplusplus/releases)
[![C++17](docs/images/badge-C++17.svg)][cpp_compilers]
[![TOML](docs/images/badge-TOML.svg)][v1.0.0]
[![MIT license](docs/images/badge-license-MIT.svg)](./LICENSE)
[![ci](https://github.com/marzer/tomlplusplus/actions/workflows/ci.yaml/badge.svg?branch=master)](https://github.com/marzer/tomlplusplus/actions/workflows/ci.yaml)
[![Mentioned in Awesome C++](docs/images/badge-awesome.svg)](https://github.com/fffaraz/awesome-cpp)
[![Sponsor](docs/images/badge-sponsor.svg)](https://github.com/sponsors/marzer)
[![Gitter](docs/images/badge-gitter.svg)](https://gitter.im/marzer/tomlplusplus)
====

## toml++ homepage

<p align="center">
	<strong>✨&#xFE0F; This README is fine, but the <a href="https://marzer.github.io/tomlplusplus/">toml++ homepage</a> is better. ✨&#xFE0F;</strong>
</p>

<br>

## Library features

-   Header-only (optional!)
-   Supports the latest [TOML] release ([v1.0.0]), plus optional support for some unreleased TOML features
-   Passes all tests in the [toml-test](https://github.com/BurntSushi/toml-test) suite
-   Supports serializing to JSON and YAML
-   Proper UTF-8 handling (incl. BOM)
-   C++17 (plus some C++20 features where available, e.g. experimental support for [char8_t] strings)
-   Doesn't require RTTI
-   Works with or without exceptions
-   Tested on Clang (6+), GCC (7+) and MSVC (VS2019)
-   Tested on x64, x86 and ARM

<br>

## Basic usage

> ℹ&#xFE0F; _The following example favours brevity. If you'd prefer full API documentation and lots of specific code
> snippets instead, visit the project [homepage]_

Given a [TOML] file `configuration.toml` containing the following:

```toml
[library]
name = "toml++"
authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]

[dependencies]
cpp = 17
```

Reading it in C++ is easy with toml++:

```cpp
#include <toml.hpp>

auto config = toml::parse_file( "configuration.toml" );

// get key-value pairs
std::string_view library_name = config["library"]["name"].value_or(""sv);
std::string_view library_author = config["library"]["authors"][0].value_or(""sv);
int64_t depends_on_cpp_version = config["dependencies"]["cpp"].value_or(0);

// modify the data
config.insert_or_assign("alternatives", toml::array{
    "cpptoml",
    "toml11",
    "Boost.TOML"
});

// use a visitor to iterate over heterogenous data
config.for_each([](auto& key, auto& value)
{
    std::cout << value << "\n";
    if constexpr (toml::is_string<decltype(value)>)
        do_something_with_string_values(value);
});

// you can also iterate more 'traditionally' using a ranged-for
for (auto&& [k, v] : config)
{
    // ...
}

// re-serialize as TOML
std::cout << config << "\n";

// re-serialize as JSON
std::cout << toml::json_formatter{ config } << "\n";

// re-serialize as YAML
std::cout << toml::yaml_formatter{ config } << "\n";

```

You'll find some more code examples in the `examples` directory, and plenty more as part of the [API documentation].

<br>

## Adding toml++ to your project

`toml++` comes in two flavours: Single-header and Regular. The API is the same for both.

### 🍦&#xFE0F; Single-header flavour

1. Drop [`toml.hpp`] wherever you like in your source tree
2. There is no step two

### 🍨&#xFE0F; Regular flavour

1. Clone the repository
2. Add `tomlplusplus/include` to your include paths
3. `#include <toml++/toml.hpp>`

### Conan

Add `tomlplusplus/3.3.0` to your conanfile.

### DDS

Add `tomlpp` to your `package.json5`, e.g.:

```plaintext
depends: [
    'tomlpp^3.3.0',
]
```

> ℹ&#xFE0F; _[What is DDS?](https://dds.pizza/)_

### Tipi.build

`tomlplusplus` can be easily used in [tipi.build](https://tipi.build) projects by adding the following entry to your `.tipi/deps`:

```json
{
	"marzer/tomlplusplus": {}
}
```

### Vcpkg

```plaintext
vcpkg install tomlplusplus
```

### Meson

You can install the wrap with:

```plaintext
meson wrap install tomlplusplus
```

After that, you can use it like a regular dependency:

```meson
tomlplusplus_dep = dependency('tomlplusplus')
```

You can also add it as a subproject directly.

### CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.3.0
)
FetchContent_MakeAvailable(tomlplusplus)
```

> ℹ&#xFE0F; _[What is FetchContent?](https://cmake.org/cmake/help/latest/module/FetchContent.html)_

### Git submodules

```plaintext
git submodule add --depth 1 https://github.com/marzer/tomlplusplus.git tomlplusplus
```

### Other environments and package managers

The C++ tooling ecosystem is a fractal nightmare of unbridled chaos so naturally I'm not up-to-speed with all of the
available packaging and integration options. I'm always happy to see new ones supported, though! If there's some
integration you'd like to see and have the technical know-how to make it happen, feel free to
[make a pull request](./CONTRIBUTING.md).

### What about dependencies?

If you just want to consume `toml++` as a regular library then you don't have any dependencies to worry about.
There's a few test-related dependencies to be aware of if you're working on the library, though.
See [CONTRIBUTING] for information.

<br>

## Configuration

A number of configurable options are exposed in the form of preprocessor `#defines` Most likely you
won't need to mess with these at all, but if you do, set them before including toml++.

| Option                            |      Type      | Description                                                                                               | Default               |
| --------------------------------- | :------------: | --------------------------------------------------------------------------------------------------------- | --------------------- |
| `TOML_ASSERT(expr)`               | function macro | Sets the assert function used by the library.                                                             | `assert()`            |
| `TOML_CALLCONV`                   |     define     | Calling convention to apply to exported free/static functions.                                            | undefined             |
| `TOML_CONFIG_HEADER`              | string literal | Includes the given header file before the rest of the library.                                            | undefined             |
| `TOML_ENABLE_FORMATTERS`          |    boolean     | Enables the formatters. Set to `0` if you don't need them to improve compile times and binary size.       | `1`                   |
| `TOML_ENABLE_FLOAT16`             |    boolean     | Enables support for the built-in `_Float16` type.                                                         | per compiler settings |
| `TOML_ENABLE_PARSER`              |    boolean     | Enables the parser. Set to `0` if you don't need it to improve compile times and binary size.             | `1`                   |
| `TOML_ENABLE_UNRELEASED_FEATURES` |    boolean     | Enables support for [unreleased TOML language features].                                                  | `0`                   |
| `TOML_ENABLE_WINDOWS_COMPAT`      |    boolean     | Enables support for transparent conversion between wide and narrow strings.                               | `1` on Windows        |
| `TOML_EXCEPTIONS`                 |    boolean     | Sets whether the library uses exceptions.                                                                 | per compiler settings |
| `TOML_EXPORTED_CLASS`             |     define     | API export annotation to add to classes.                                                                  | undefined             |
| `TOML_EXPORTED_MEMBER_FUNCTION`   |     define     | API export annotation to add to non-static class member functions.                                        | undefined             |
| `TOML_EXPORTED_FREE_FUNCTION`     |     define     | API export annotation to add to free functions.                                                           | undefined             |
| `TOML_EXPORTED_STATIC_FUNCTION`   |     define     | API export annotation to add to static functions.                                                         | undefined             |
| `TOML_HEADER_ONLY`                |    boolean     | Disable this to explicitly control where toml++'s implementation is compiled (e.g. as part of a library). | `1`                   |
| `TOML_IMPLEMENTATION`             |     define     | Define this to enable compilation of the library's implementation when `TOML_HEADER_ONLY` == `0`.         | undefined             |
| `TOML_OPTIONAL_TYPE`              |   type name    | Overrides the `optional<T>` type used by the library if you need [something better than std::optional].   | undefined             |
| `TOML_SMALL_FLOAT_TYPE`           |   type name    | If your codebase has a custom 'small float' type (e.g. half-precision), this tells toml++ about it.       | undefined             |
| `TOML_SMALL_INT_TYPE`             |   type name    | If your codebase has a custom 'small integer' type (e.g. 24-bits), this tells toml++ about it.            | undefined             |

> ℹ&#xFE0F; _A number of these have ABI implications; the library uses inline namespaces to prevent you from accidentally
> linking incompatible combinations together._

<br>

## TOML Language Support

At any given time the library aims to support whatever the [most recently-released version] of TOML is, with opt-in
support for a number of unreleased features from the [TOML master] and some sane cherry-picks from the
[TOML issues list] where the discussion strongly indicates inclusion in a near-future release.

The library advertises the most recent numbered language version it fully supports via the preprocessor
defines `TOML_LANG_MAJOR`, `TOML_LANG_MINOR` and `TOML_LANG_PATCH`.

### **Unreleased language features:**

-   [#516]: Allow newlines and trailing commas in inline tables
-   [#562]: Allow hex floating-point values
-   [#644]: Support `+` in key names
-   [#671]: Local time of day format should support `09:30` as opposed to `09:30:00`
-   [#687]: Relax bare key restrictions to allow additional unicode characters
-   [#790]: Include an `\e` escape code sequence (shorthand for `\u001B`)
-   [#796]: Include an `\xHH` escape code sequence
-   [#891]: Allow non-English scripts for unquoted keys

> ℹ&#xFE0F; _`#define TOML_ENABLE_UNRELEASED_FEATURES 1` to enable these features (see [Configuration](#Configuration))._

### 🔹&#xFE0F; **TOML v1.0.0:**

All features supported, including:

-   [#356]: Allow leading zeros in the exponent part of a float
-   [#567]: Control characters are not permitted in comments
-   [#571]: Allow raw tabs inside strings
-   [#665]: Make arrays heterogeneous
-   [#766]: Allow comments before commas in arrays

### 🔹&#xFE0F; **TOML v0.5.0:**

All features supported.

<br>

## Contributing

Contributions are very welcome! Either by [reporting issues] or submitting pull requests.
If you wish to submit a pull request, please see [CONTRIBUTING] for all the details you need to get going.

<br>

## License and Attribution

toml++ is licensed under the terms of the MIT license - see [LICENSE].

UTF-8 decoding is performed using a state machine based on Bjoern Hoehrmann's '[Flexible and Economical UTF-8 Decoder]'.

### With thanks to:

-   **[@beastle9end](https://github.com/beastle9end)** - Made Windows.h include bypass
-   **[@bjadamson](https://github.com/bjadamson)** - Reported some bugs and helped design a new feature
-   **[@bobfang1992](https://github.com/bobfang1992)** - Reported a bug and created a [wrapper in python](https://github.com/bobfang1992/pytomlpp)
-   **[@GiulioRomualdi](https://github.com/GiulioRomualdi)** - Added cmake+meson support
-   **[@jonestristand](https://github.com/jonestristand)** - Designed and implemented the `toml::path`s feature
-   **[@kcsaul](https://github.com/kcsaul)** - Fixed a bug
-   **[@levicki](https://github.com/levicki)** - Helped design some new features
-   **[@moorereason](https://github.com/moorereason)** - Reported a whole bunch of bugs
-   **[@mosra](https://github.com/mosra)** - Created the awesome [m.css] used to generate the API docs
-   **[@ned14](https://github.com/ned14)** - Reported a bunch of bugs and helped design some new features
-   **[@okureta](https://github.com/okureta)** - Reported a bug
-   **[@prince-chrismc](https://github.com/prince-chrismc)** - Added toml++ to ConanCenter, and fixed some typos
-   **[@rbrugo](https://github.com/rbrugo)** - Helped design a new feature
-   **[@Reedbeta](https://github.com/Reedbeta)** - Fixed a bug and added additional Visual Studio debugger native visualizers
-   **[@Ryan-rsm-McKenzie](https://github.com/Ryan-rsm-McKenzie)** - Add natvis file to cmake install script
-   **[@shdnx](https://github.com/shdnx)** - Fixed a bug on GCC 8.2.0 and some meson config issues
-   **[@sneves](https://github.com/sneves)** - Helped fix a number of parser bugs
-   **[@sobczyk](https://github.com/sobczyk)** - Reported some bugs
-   **[@std-any-emplace](https://github.com/std-any-emplace)** - Reported some bugs
-   **[@Tachi107](https://github.com/Tachi107)** - Made some tweaks to meson.build, added compile_library build option
-   **[@traversaro](https://github.com/traversaro)** - Added vcpkg support and reported a bunch of bugs
-   **[@whiterabbit963](https://github.com/whiterabbit963)** - Fixed a bug with value_or conversions
-   **[@ximion](https://github.com/ximion)** - Added support for installation with meson
-   **[@a-is](https://github.com/a-is)** - Fixed a bug

<br>

## Contact

For bug reports and feature requests please consider using the [issues] system here on GitHub. For anything else
though you're welcome to reach out via other means. In order of likely response time:

-   Gitter: [marzer/tomlplusplus](https://gitter.im/marzer/tomlplusplus) ("Discord for repos")
-   Twitter: [marzer8789](https://twitter.com/marzer8789)
-   Email: [mark.gillard@outlook.com.au](mailto:mark.gillard@outlook.com.au)
-   Facebook: [marzer](https://www.facebook.com/marzer)
-   LinkedIn: [marzer](https://www.linkedin.com/in/marzer/)

[API documentation]: https://marzer.github.io/tomlplusplus/
[homepage]: https://marzer.github.io/tomlplusplus/
[unreleased TOML language features]: #unreleased-language-features
[most recently-released version]: https://github.com/toml-lang/toml/releases
[char8_t]: https://en.cppreference.com/w/cpp/keyword/char8_t
[TOML]: https://toml.io/
[TOML master]: https://github.com/toml-lang/toml/blob/master/README.md
[TOML issues list]: https://github.com/toml-lang/toml/issues
[v1.0.0]: https://toml.io/en/v1.0.0
[CONTRIBUTING]: ./CONTRIBUTING.md
[LICENSE]: ./LICENSE
[Flexible and Economical UTF-8 Decoder]: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
[cpp_compilers]: https://en.cppreference.com/w/cpp/compiler_support
[reporting issues]: https://github.com/marzer/tomlplusplus/issues/new/choose
[issues]: https://github.com/marzer/tomlplusplus/issues
[#356]: https://github.com/toml-lang/toml/issues/356
[#516]: https://github.com/toml-lang/toml/issues/516
[#562]: https://github.com/toml-lang/toml/issues/562
[#567]: https://github.com/toml-lang/toml/issues/567
[#571]: https://github.com/toml-lang/toml/issues/571
[#644]: https://github.com/toml-lang/toml/issues/644
[#665]: https://github.com/toml-lang/toml/issues/665
[#671]: https://github.com/toml-lang/toml/issues/671
[#687]: https://github.com/toml-lang/toml/issues/687
[#766]: https://github.com/toml-lang/toml/issues/766
[#790]: https://github.com/toml-lang/toml/pull/790
[#796]: https://github.com/toml-lang/toml/pull/796
[#891]: https://github.com/toml-lang/toml/pull/891
[something better than std::optional]: https://github.com/TartanLlama/optional
[m.css]: https://mcss.mosra.cz/documentation/doxygen
[`toml.hpp`]: https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp
