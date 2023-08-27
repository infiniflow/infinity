# Building and installing the library and/or CLI

## From source

GCC is the recommended compiler, but clang is also supported.

To build from source, you'll need a basic unix toolchain with `sh` and
`make`/`gmake`:

```shell
./configure && sudo ./install.sh
```

or:

```shell
./configure && sudo make install
```

To uninstall:

```shell
sudo make uninstall
```

To build the independent executables in a local build folder,
use `make install` instead of `make all`.

### Building and installing only the library

```shell
./configure && cd src && sudo make install
```

## A note on compilers

GCC 11+ is the recommended compiler. Compared with clang, gcc in some cases
seems to produce faster code for reasons we have not yet determined.

## COMING SOON! Packages

zsv will soon be available from a number of package managers:

- OSX: `brew install zsv`
- Windows: `nuget install zsv`
- Linux: `yum install zsv`
