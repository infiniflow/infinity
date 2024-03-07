# zsv+lib: the world's fastest (simd) CSV parser, with an extensible CLI

lib + CLI:
[![ci](https://github.com/liquidaty/zsv/actions/workflows/ci.yml/badge.svg)](https://github.com/liquidaty/zsv/actions/workflows/ci.yml)
![GitHub pre-release)](https://img.shields.io/github/v/release/liquidaty/zsv?include_prereleases&label=pre-release&logo=github&style=flat-square)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/liquidaty/zsv?logo=github&style=flat-square)
![GitHub all releases (downloads)](https://img.shields.io/github/downloads/liquidaty/zsv/total?logo=github&style=flat-square)
[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](https://github.com/liquidaty/zsv/blob/master/LICENSE)

lib only:
[![NPM Version][npm-version-image]][npm-url]
[![NPM Install Size][npm-install-size-image]][npm-install-size-url]

[npm-install-size-image]: https://badgen.net/packagephobia/install/zsv-lib
[npm-install-size-url]: https://packagephobia.com/result?p=zsv-lib
[npm-url]: https://npmjs.org/package/zsv-lib
[npm-version-image]: https://badgen.net/npm/v/zsv-lib


zsv+lib is a fast CSV parser library and extensible command-line utility.
It achieves high performance using SIMD operations,
[efficient memory use](docs/memory.md) and other optimization techniques, and
can also parse generic-delimited and fixed-width formats, as well as multi-row-span headers

The ZSV CLI can be compiled to virtually any target, including [web assembly](examples/js), and offers features including `select`, `count`, direct CSV `sql`, `flatten`, `serialize`, `2json` conversion, `2db` sqlite3 conversion, `stack`, `pretty`, `2tsv`, `compare` and more.

Pre-built CLI packages are available via brew and nuget


A pre-built library package is available for Node (`npm install zsv-lib`). Please note, this package
is still in alpha and currently only exposes a small subset of the zsv library capabilities. More
to come

If you like zsv+lib, do not forget to give it a star! 🌟

## Performance

Preliminary performance results compare favorably vs other CSV utilities (`xsv`,
`tsv-utils`, `csvkit`, `mlr` (miller) etc). Below were results on a pre-M1 macOS
MBA; on most platforms zsvlib was 2x faster, though in some cases the advantage
was smaller e.g. 15-25%) (below, mlr not shown as it was about 25x slower):

<img src="https://user-images.githubusercontent.com/26302468/146497899-48174114-3b18-49b0-97da-35754ab56e48.png" alt="count speed" height="150px"><img src="https://user-images.githubusercontent.com/26302468/146498211-afc77ce6-4229-4599-bf33-81bf00c725a8.png" alt="select speed" height="150px">

** See 12/19 update re M1 processor at
https://github.com/liquidaty/zsv/blob/main/app/benchmark/README.md

#### Which "CSV"

"CSV" is an ambiguous term. This library uses the same definition as Excel. In addition, it provides a *row-level* (as well as cell-level) API and provides "normalized" CSV output (e.g. input of `this"iscell1,"thisis,"cell2` becomes `"this""iscell1","thisis,cell2"`). Each of these three objectives (Excel compatibility, row-level API and normalized output) has a measurable performance impact; conversely, it is possible to achieve-- which a number of other CSV parsers do-- much faster parsing speeds if any of these requirements (especially Excel compatibility) are dropped.

## Built-in and extensible features

`zsv` is an extensible CSV utility, which uses zsvlib, for tasks such as slicing
and dicing, querying with SQL, combining, serializing, flattening,
[converting between CSV/JSON/sqlite3](docs/csv_json_sqlite.md) and more.

`zsv` is streamlined for easy development of custom dynamic extensions.

zsvlib and `zsv` are written in C, but since zsvlib is a library, and `zsv`
extensions are just shared libraries, you can extend `zsv` with your own code in
any programming language, so long as it has been compiled into a shared library
that implements the expected
[interface](./include/zsv/ext/implementation_private.h).

## Key highlights

* Available as BOTH a library and an application
* Open-source, permissively licensed
* Handles real-world CSV the same way that spreadsheet programs do (*including
  edge cases*). Gracefully handles (and can "clean") real-world data that may be
  "dirty"
* Runs on macOS (tested on clang/gcc), Linux (gcc), Windows (mingw),
  BSD (gcc-only) and in-browser (emscripten/wasm)
* Fastest (at least, vs all alternatives and on all platforms we've benchmarked where
  256-bit SIMD operations are available). See
  [app/benchmark/README.md](app/benchmark/README.md)
* Low memory usage (regardless of how big your data is) and size footprint for
  both lib (~20k) and CLI executable (< 1MB)
* Handles general delimited data (e.g. pipe-delimited) and fixed-with input
  (with specified widths or auto-detected widths)
* Handles multi-row headers
* Handles input from any stream, including caller-defined streams accessed via
  a single caller-defined `fread`-like function
* Easy to use as a library in a few lines of code, via either pull or push parsing
* Includes the `zsv` CLI with the following built-in commands:
  * `select`, `count`, `sql` query, `desc`ribe, `flatten`, `serialize`, `2json`,
    `2db`, `stack`, `pretty`, `2tsv`, `compare`, `jq`, `prop`, `rm`
  * easily [convert between CSV/JSON/sqlite3](docs/csv_json_sqlite.md)
  * [compare multiple files](docs/compare.md)

* CLI is easy to extend/customize with a few lines of code via modular plug-in framework.
  Just write a few custom functions and compile into a distributable DLL that any existing zsv
  installation can use
* zsvlib and `zsv` are permissively licensed

## Installing

### Packages

Download pre-built binaries and packages for macOS, Windows, Linux and BSD from
the [Releases](https://github.com/liquidaty/zsv/releases) page.

You can also download pre-built binaries and packages from
[Actions](https://github.com/liquidaty/zsv/actions) for the latest commits and
PRs but these are retained only for limited days.

#### macOS

...via Homebrew:

```shell
brew tap liquidaty/zsv
brew install zsv
```

...via MacPorts:

```shell
sudo port install zsv
```

#### Linux

For Linux (Debian/Ubuntu - `*.deb`):

```shell
# Install
sudo apt install ./zsv-amd64-linux-gcc.deb

# Uninstall
sudo apt remove zsv
```

For Linux (RHEL/CentOS - `*.rpm`):

```shell
# Install
sudo yum install ./zsv-amd64-linux-gcc.rpm

# Uninstall
sudo yum remove zsv
```

#### Windows

For Windows (`*.nupkg`), install with `nuget.exe`:

```shell
# Install via nuget custom feed (requires absolutes paths)
md nuget-feed
nuget.exe add zsv .\<path>\zsv-amd64-windows-mingw.nupkg -source <path>/nuget-feed
nuget.exe install zsv -version <version> -source <path>/nuget-feed

# Uninstall
nuget.exe delete zsv <version> -source <path>/nuget-feed
```

For Windows (`*.nupkg`), install with `choco.exe`:

```shell
# Install
choco.exe install zsv --pre -source .\zsv-amd64-windows-mingw.nupkg

# Uninstall
choco.exe uninstall zsv
```

#### Node

The zsv parser library is available for node:
```
npm install zsv-lib
```

Please note:
* this pacakge is still in alpha and currently only exposes a small subset of the zsv library capabilities.
  More to come!
* The CLI is not yet available as a Node package
* If you'd like to use additional parser features, or use the CLI as a Node package,
  please feel free to post a request in an issue here

### From source

See [BUILD.md](BUILD.md) for more details.

## Why another CSV parser/utility?

Our objectives, which we were unable to find in a pre-existing project, are:

* Reasonably high performance
* Runs on any platform, including web assembly
* Available as both a library and a standalone executable / command-line
  interface utility (CLI)
* Memory-efficient, configurable resource limits
* Handles real-world CSV cases the same way that Excel does, including all edge
  cases (quote handling, newline handling (either `\n` or `\r`), embedded
  newlines, abnormal quoting (e.g. aaa"aaa,bbb...)
* Handles other "dirty" data issues:
  * Assumes valid UTF8, but does not misbehave if input contains bad UTF8
  * Option to specify multi-row headers
  * Does not assume or stop working in case of inconsistent numbers of columns
* Easy to use library or extend/customize CLI

There are several excellent tools that achieve high performance. Among those we
considered were xsv and tsv-utils. While they met our performance objective,
both were designed primarily as a utility and not a library, and were not easy
enough, for our needs, to customize and/or to support modular customizations
that could be maintained (or licensed) independently of the related project (in
addition to the fact that they were written in Rust and D, respectively, which
happen to be languages with which we lacked deep experience, especially for web
assembly targeting).

Others we considered were Miller (mlr), csvkit and Go (csv module), which did
not meet our performance objective. We also considered various other libraries
using SIMD for CSV parsing, but none that we tried met the "real-world CSV"
objective.

Hence zsv was created as a library and a versatile application, both optimized
for speed and ease of development for extending and/or customizing to your needs

## Batteries included

`zsv` comes with several built-in commands:

* `echo`: read CSV from stdin and write it back out to stdout. This is mostly
  useful for demonstrating how to use the API and also how to create a plug-in,
  and has some limited utility beyond that e.g. for adding/removing the UTF8
  BOM, or cleaning up bad UTF8
* `select`: re-shape CSV by skipping leading garbage, combining header rows into
  a single header, selecting or excluding specified columns, removing duplicate
  columns, sampling, searching and more
* `sql`: run ad-hoc SQL query on a CSV file
* `desc`: provide a quick description of your table data
* `pretty`: format for console (fixed-width) display, or convert to markdown
  format
* `2json`: convert CSV to JSON. Optionally, output in [database schema](docs/db.schema.json)
* `2tsv`: convert CSV to TSV
* `compare`: compare two or more tables of data and output the differences
* `serialize` (inverse of flatten): convert an NxM table to a single 3x (Nx(M-1))
  table with columns: Row, Column Name, Column Value
* `flatten` (inverse of serialize): flatten a table by combining rows that share
  a common value in a specified identifier column
* `stack`: merge CSV files vertically
* `jq`: run a jq filter
* `2db`: [convert from JSON to sqlite3 db](docs/csv_json_sqlite.md)
* `prop`: view or save parsing options associated with a file, such as initial
          rows to ignore, or header row span. Saved options are be applied by
          default when processing that file

Each of these can also be built as an independent executable named `zsv_xxx`
where `xxx` is the command name.

## Running the CLI

After installing, run `zsv help` to see usage details. The typical syntax is
`zsv <command> <parameters>` e.g.

```shell
zsv sql my_population_data.csv "select * from data where population > 100000"
```

### Using the API

Full application code examples can be found at [examples/lib/README.md](examples/lib/README.md).

An example of using the API, compiled to wasm and called via Javascript,
is in [examples/js/README.md](examples/js/README.md).

For more sophisticated (but at this time, only sporadically commented/documented) use cases,
see the various CLI C source files in the app/ directory such as app/serialize.c


### Creating your own extension

You can extend `zsv` by providing a pre-compiled shared or static library that
defines the functions specified in `extension_template.h` and which `zsv` loads
in one of three ways:

* as a static library that is statically linked at compile time
* as a dynamic library that is linked at compile time and located in any library
  search path
* as a dynamic library that is located in the same folder as the `zsv`
  executable and loaded at runtime if/as/when the custom mode is invoked

#### Example and template

You can build and run a sample extension by running `make test` from
`app/ext_example`.

The easiest way to implement your own extension is to copy and customize the
template files in [app/ext_template](app/ext_template/README.md)

## Current release limitations

This release does not yet implement the full range of core features that are
planned for implementation prior to beta release. If you are interested in
helping, please post an issue.

### Possible enhancements and related developments

* online "playground" (soon to be released)
* optimize search; add search with hyperscan or re2 regex matching, possibly
  parallelize?
* optional openmp or other multi-threading for row processing
* auto-generated documentation, and better documentation in general
* Additional benchmarking. Would be great to use
  https://bitbucket.org/ewanhiggs/csv-game/src/master/ as a springboard to
  benchmarking a number of various tasks
* encoding conversion e.g. UTF16 to UTF8

## Contribute

* [Fork](https://github.com/liquidaty/zsv/fork) the project.
* Check out the latest [`main`](https://github.com/liquidaty/zsv/tree/main)
  branch.
* Create a feature or bugfix branch from `main`.
* Commit and push your changes.
* Submit the PR.

## License

[MIT](https://github.com/liquidaty/zsv/blob/master/LICENSE)
