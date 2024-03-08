#!/usr/bin/env bash

set -e -o pipefail

echo
echo "running script $0"
echo "current working directory : $PWD"

echo
echo 'step [1/5] : sudo apt update && sudo apt install -y git wget'
echo
sudo apt update && sudo apt install -y git wget

echo
echo 'step [2/5] : download cmake-3.28.1'
echo 'command: wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz'
echo
wget https://cmake.org/files/v3.28/cmake-3.28.1-linux-x86_64.tar.gz

echo
echo 'step [3/5] : install cmake-3.28.1 into /usr/local/bin and /usr/local/share'
echo 'command: tar zxvf cmake-3.28.1-linux-x86_64.tar.gz'
echo 'command: sudo cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64'
echo
tar zxvf cmake-3.28.1-linux-x86_64.tar.gz
sudo cp -rf cmake-3.28.1-linux-x86_64/bin/* /usr/local/bin && sudo cp -rf cmake-3.28.1-linux-x86_64/share/* /usr/local/share && rm -rf cmake-3.28.1-linux-x86_64

echo
echo 'step [4/5] : sudo apt install -y ninja-build clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev'
echo
sudo apt install -y ninja-build clang-17 clang-tools-17 flex libboost1.81-dev liblz4-dev libevent-dev liburing-dev libthrift-dev

echo
echo 'step [5/5] : create a symbolic link for clang-scan-deps-17'
echo 'command: sudo ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps'
echo
sudo ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps

echo
echo "script $0 finished"
echo
