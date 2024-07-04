---
sidebar_position: 2
slug: /build_from_source
---

# Build from Source

Infinity can only be compiled natively on Linux. If your operating system is not Linux, build it using Docker.

- [Build from source on Linux using Docker](#build-from-source-on-linux-using-docker)
- [Build from source on Ubuntu 22.04](#build-from-source-on-ubuntu-2204)
- [Build from source on Ubuntu 23.10](#build-from-source-on-ubuntu-2310)


## Prerequisites

Build the whole project, especially the link stage, requires much RAM. The host may become unresponsive due to the very slow kernel oom killer. To lessen the chances of it happening, it is recommended to install [earlyoom](https://github.com/rfjakob/earlyoom). Furthermore, you need to tell cmake to limit the concurrent link process:

`-DCMAKE_JOB_POOL_LINK:STRING=link_pool -DCMAKE_JOB_POOLS:STRING=link_pool=2`

The recommended `link_pool` size is:

- 1 for 6GB
- 2 for 16GB
- 3 for 32GB

The `CMAKE_BUILD_TYPE` (cmake build type) can be one of:

- `Debug`: no inline, with symbol info, with address sanitize, normally ~10x slower than `RelWithDebInfo` and `Release`. This aims daily development.
- `RelWithDebInfo`: optimize with `-O2`, with symbol info. This aims performance analysis.
- `Release`: optimize with `-O3`, without symbol info. The executables built out are much smaller than `RelWithDebInfo`. This aims [project releases](https://github.com/infiniflow/infinity/releases).

Following procedures use `Debug`. Change it as you need.

## Build from source on Linux using Docker

This section provides instructions on building Infinity from source on Linux using Docker.

### Step1 Download the source code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step2 Build the source code using Docker

```shell
cd infinity && mkdir cmake-build-debug
TZ=$(readlink -f /etc/localtime | awk -F '/zoneinfo/' '{print $2}')
docker run -d --privileged --name infinity_build -e TZ=$TZ -v $PWD:/infinity -v /boot:/boot infiniflow/infinity_builder:centos7_clang18
docker exec infinity_build bash -c "cd /infinity/cmake-build-debug && cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON .. && cmake --build ."
```

### Step3 Start up the Infinity server

```shell
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
ulimit -n 500000
./cmake-build-debug/src/infinity
```

## Build from source on Ubuntu 22.04

This section provides instructions on building Infinity from source on Ubuntu 22.04.

### Step1 Install necessary dependencies

```shell
sudo apt update && sudo apt install git wget unzip software-properties-common
wget https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.tar.gz
tar zxvf cmake-3.29.0-linux-x86_64.tar.gz
sudo cp -rf cmake-3.29.0-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.29.0-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.29.0-linux-x86_64
wget https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-linux.zip
unzip ninja-linux.zip && sudo cp ninja /usr/local/bin && rm ninja
wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 18 && rm llvm.sh
sudo add-apt-repository -P ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -P ppa:mhier/libboost-latest
sudo apt update && sudo apt install g++-13 clang-tools-18 flex libboost1.81-dev liblz4-dev zlib1g-dev libevent-dev libjemalloc-dev python3-dev
ln -s /usr/lib/llvm-18/bin/clang-scan-deps /usr/bin/clang-scan-deps
ln -s /usr/bin/clang-format-18 /usr/bin/clang-format
ln -s /usr/bin/clang-tidy-18 /usr/bin/clang-tidy
ln -s /usr/bin/llvm-symbolizer-18 /usr/bin/llvm-symbolizer
```

### Step2 Download the source code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step3 Build the source code

you need first install `simde` if on arm, note we need `v0.7.4+` version for simde

```
sudo apt install libsimde-dev
```
If the installed version is under v0.7.4, you can download the include files directly from github and replace it.


```shell
git config --global --add safe.directory infinity
cd infinity && mkdir cmake-build-debug && cd cmake-build-debug
export CC=/usr/bin/clang-18
export CXX=/usr/bin/clang++-18
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build .
```

### Step4 Start up the Infinity server

```shell
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
ulimit -n 500000
./cmake-build-debug/src/infinity
```


## Build from source on Ubuntu 23.10

This section provides instructions on building Infinity from source on Ubuntu 23.10.

### Step1 Install dependencies

```shell
sudo apt update && sudo apt install -y git wget
wget https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.tar.gz
tar zxvf cmake-3.29.0-linux-x86_64.tar.gz
sudo cp -rf cmake-3.29.0-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.29.0-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.29.0-linux-x86_64
wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 18 && rm llvm.sh
sudo apt install -y ninja-build clang-tools-18 flex libboost1.81-dev liblz4-dev zlib1g-dev libevent-dev libjemalloc-dev python3-dev
sudo ln -s /usr/lib/llvm-18/bin/clang-scan-deps /usr/bin/clang-scan-deps
sudo ln -s /usr/bin/clang-format-18 /usr/bin/clang-format
sudo ln -s /usr/bin/clang-tidy-18 /usr/bin/clang-tidy
sudo ln -s /usr/bin/llvm-symbolizer-18 /usr/bin/llvm-symbolizer
```

### Step2 Download Source Code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step3 Build source code

```shell
git config --global --add safe.directory infinity
cd infinity && mkdir cmake-build-debug && cd cmake-build-debug
export CC=/usr/bin/clang-18
export CXX=/usr/bin/clang++-18
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build .
```

### Step4 Start up Infinity server

```shell
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
ulimit -n 500000
./cmake-build-debug/src/infinity
```
