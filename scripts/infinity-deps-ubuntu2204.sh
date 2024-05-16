#!/usr/bin/env bash

set -e -o pipefail

echo
echo "running script $0"
echo "current working directory : $PWD"

echo
echo 'step [1/9] : sudo apt update && sudo apt install -y git wget unzip software-properties-common'
echo
sudo apt update && sudo apt install -y git wget unzip software-properties-common

echo
echo 'step [2/9] : download cmake-3.29.0'
echo 'command: wget https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.tar.gz'
echo
wget https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.tar.gz

echo
echo 'step [3/9] : install cmake-3.29.0 into /usr/local/bin and /usr/local/share'
echo 'command: tar zxvf cmake-3.29.0-linux-x86_64.tar.gz'
echo 'command: sudo cp -rf cmake-3.29.0-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.29.0-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.29.0-linux-x86_64'
echo
tar zxvf cmake-3.29.0-linux-x86_64.tar.gz
sudo cp -rf cmake-3.29.0-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.29.0-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.29.0-linux-x86_64

echo
echo 'step [4/9] : download ninja-1.11.1'
echo 'command: wget https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip'
echo
wget https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip

echo
echo 'step [5/9] : install ninja-1.11.1 into /usr/local/bin'
echo 'command: unzip ninja-linux.zip && sudo cp ninja /usr/local/bin && rm ninja'
echo
unzip ninja-linux.zip && sudo cp ninja /usr/local/bin && rm ninja

echo
echo 'step [6/9] : add apt source for llvm-17'
echo 'command: echo "deb https://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main" | sudo tee /etc/apt/sources.list.d/llvm17.list'
echo 'command: wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc'
echo
echo "deb https://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main" | sudo tee /etc/apt/sources.list.d/llvm17.list
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc

echo
echo 'step [7/9] : add apt source for g++-13 gcc-13 libstdc++-13-dev libboost1.81-dev'
echo 'command: sudo add-apt-repository -y -P ppa:ubuntu-toolchain-r/test'
echo 'command: sudo add-apt-repository -y -P ppa:mhier/libboost-latest'
echo
sudo add-apt-repository -y -P ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -y -P ppa:mhier/libboost-latest

echo
echo "step [8/9] : sudo apt update && sudo apt install -y g++-13 clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev"
echo
sudo apt update && sudo apt install -y g++-13 clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev

echo
echo 'step [9/9] : create a symbolic link for clang-scan-deps-17'
echo 'command: sudo ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps'
echo
sudo ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps

echo
echo "script $0 finished"
echo
