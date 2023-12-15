## Prerequisites

-  Operating system: Ubuntu 22.04,  openSUSE Tumbleweed 20230828 or higher
-  GCC 13 / Clang-18 or higher to support C++23
-  CMake 3.10 or higher

## Step1 Download Source Code

```shell
$ git clone https://github.com/infiniflow/infinity.git
```

## Step2 Install dependencies

Taking Ubuntu 22.4+ as an example:

```shell
# Clang 17 or 18 is required. GCC is not supported.
$ apt install clang-*-17
$ ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps

# CMake 3.28+ is requrired.
$ wget https://github.com/Kitware/CMake/releases/download/v3.28.0-rc5/cmake-3.28.0-rc5-linux-x86_64.tar.gz
$ tar xzvf cmake-3.28.0-rc5-linux-x86_64.tar.gz
$ sudo cp -r cmake-3.28.0-rc5-linux-x86_64/bin/* /usr/local/bin/

$ sudo apt install make libevent-dev ninja-build bison flex libomp-17-dev libblas-dev liblapack-dev libboost1.81-dev liburing-dev libgflags-dev libleveldb-dev

# iresearch requires lz4
$ git clone https://github.com/lz4/lz4.git
$ cd lz4
$ make
$ sudo make install
$ export LZ4_ROOT=/usr/local

```

## Step3 Build source code

```shell
$ git config --global --add safe.directory infinity
$ cd infinity && mkdir build && cd build
$ export CC=/usr/bin/clang-18
$ export CXX=/usr/bin/clang++-18
$ cmake -G Ninja ..
$ ninja -j 12
```

### Step4 Start up Infinity server

```shell
$ ./src/infinity_main
```

## SDK develop

### Generate thrift rpc code

```shell
$ apt install thrift-compiler 
$ cd tools && python generate_rpc_code.py
```