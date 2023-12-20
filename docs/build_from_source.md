# Build from Source

- [Build from source on Ubuntu 22.04](#build-from-source-on-ubuntu-2204)
- [Build from source on Ubuntu 23.10](#build-from-source-on-ubuntu-2310)

## Build from source on Ubuntu 22.04

This section provides instructions on building Infinity from source on Ubuntu 22.04.

### Step1 Download the source code

```shell
$ git clone https://github.com/infiniflow/infinity.git
```

### Step2 Install necessary dependencies

```bash
wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz
tar zxvf cmake-3.28.1-linux-x86_64.tar.gz
cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64
wget https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip
unzip ninja-linux.zip && cp ninja /usr/local/bin && rm ninja ninja-linux.zip
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
sudo add-apt-repository -P ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -P ppa:mhier/libboost-latest
sudo apt update && sudo apt install git wget unzip software-properties-common g++-13 clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev
ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps
```

### Step3 Build the source code

```shell
git config --global --add safe.directory infinity
cd infinity && mkdir build && cd build
export CC=/usr/bin/clang-18
export CXX=/usr/bin/clang++-18
cmake -G Ninja ..
ninja -j 12
```

### Step4 Start up the Infinity server

```shell
.build/src/infinity_main
```


## Build from source on Ubuntu 23.10

This section provides instructions on building Infinity from source on Ubuntu 23.10.


### Step1 Download Source Code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step2 Install dependencies

```bash
sudo apt update && sudo apt install -y git wget
wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz
tar zxvf cmake-3.28.1-linux-x86_64.tar.gz
cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64
sudo apt install -y ninja-build clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev
ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps
```

### Step3 Build source code

```shell
git config --global --add safe.directory infinity
cd infinity && mkdir build && cd build
export CC=/usr/bin/clang-18
export CXX=/usr/bin/clang++-18
cmake -G Ninja ..
ninja -j 12
```

### Step4 Start up Infinity server

```shell
.build/src/infinity_main
```