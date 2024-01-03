# Build from Source

- [Build from source on Linux using Docker](#build-from-source-on-linux-using-docker)
- [Build from source on Ubuntu 22.04](#build-from-source-on-ubuntu-2204)
- [Build from source on Ubuntu 23.10](#build-from-source-on-ubuntu-2310)

## Build from source on Linux using Docker

This section provides instructions on building Infinity from source on Linux using Docker.

### Step1 Download the source code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step2 Build the source code using Docker

```shell
cd infinity && mkdir build
TZ=$(readlink -f /etc/localtime | awk -F '/zoneinfo/' '{print $2}')
docker run -d --name infinity_build --network=host -e TZ=$TZ -v $PWD:/infinity infiniflow/infinity_builder:centos7
docker exec infinity_build bash -c "cd /infinity/build && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja -j 12"
```

### Step3 Start up the Infinity server

```shell
./build/src/infinity
```

## Build from source on Ubuntu 22.04

This section provides instructions on building Infinity from source on Ubuntu 22.04.

### Step1 Install necessary dependencies

```shell
sudo apt update && sudo apt install git wget unzip software-properties-common
wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz
tar zxvf cmake-3.28.1-linux-x86_64.tar.gz
sudo cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64
wget https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip
unzip ninja-linux.zip && sudo cp ninja /usr/local/bin && rm ninja
echo 'deb https://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main' | sudo tee /etc/apt/sources.list.d/llvm17.list
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
sudo add-apt-repository -P ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -P ppa:mhier/libboost-latest
sudo apt update && sudo apt install g++-13 clang-18 clang-tools-18 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev
ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps
```

### Step2 Download the source code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step3 Build the source code

```shell
git config --global --add safe.directory infinity
cd infinity && mkdir build && cd build
export CC=/usr/bin/clang-17
export CXX=/usr/bin/clang++-17
cmake -G Ninja ..
ninja -j 12
```

### Step4 Start up the Infinity server

```shell
./src/infinity
```


## Build from source on Ubuntu 23.10

This section provides instructions on building Infinity from source on Ubuntu 23.10.

### Step1 Install dependencies

```shell
sudo apt update && sudo apt install -y git wget
wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz
tar zxvf cmake-3.28.1-linux-x86_64.tar.gz
sudo cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64
<<<<<<< HEAD
sudo apt install -y ninja-build clang-18 clang-tools-18 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev
=======
sudo apt install ninja-build clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev
>>>>>>> up_stream/main
ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps
```

### Step2 Download Source Code

```shell
git clone https://github.com/infiniflow/infinity.git
```

### Step3 Build source code

```shell
git config --global --add safe.directory infinity
cd infinity && mkdir build && cd build
export CC=/usr/bin/clang-17
export CXX=/usr/bin/clang++-17
cmake -G Ninja ..
ninja -j 12
```

### Step4 Start up Infinity server

```shell
.build/src/infinity
```
