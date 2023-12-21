

- [Build from source on Ubuntu 22.04](#build-from-source-on-ubuntu-2204)
- [Build from source on Ubuntu 23.10](#build-from-source-on-ubuntu-2310)

## Build from source on Ubuntu 22.04

This section provides instructions on building Infinity from source on Ubuntu 22.04.

### Step1 Download source code

```shell
$ git clone https://github.com/infiniflow/infinity.git
```

### Step2 Install necessary dependencies

#### Step [1/9]: sudo apt update && sudo apt install -y git wget unzip software-properties-common

```bash
sudo apt update && sudo apt install -y git wget unzip software-properties-common
```

#### Step [2/9]: Download cmake-3.28.1

```bash
wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz
```

### Step [3/9]: Install cmake-3.28.1

```bash
tar zxvf cmake-3.28.1-linux-x86_64.tar.gz
cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64
```

### 


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



## Build from source on Ubuntu 23.10

```shell
$ git clone https://github.com/infiniflow/infinity.git
```


## Step1 Download Source Code

```shell
$ git clone https://github.com/infiniflow/infinity.git
```

## Step2 Install dependencies






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