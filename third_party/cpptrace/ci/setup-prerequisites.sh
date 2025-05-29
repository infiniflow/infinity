#!/bin/bash
sudo apt install libgtest-dev

mkdir zstd
cd zstd
git init
git remote add origin https://github.com/facebook/zstd.git
git fetch --depth 1 origin 794ea1b0afca0f020f4e57b6732332231fb23c70 # 1.5.6
git checkout FETCH_HEAD
make -j
sudo make install

cd ..

mkdir libdwarf
cd libdwarf
git init
git remote add origin https://github.com/jeremy-rifkin/libdwarf-lite.git
git fetch --depth 1 origin 23044a5654d4abc45f0864a06d42144365d9a425 # 2.0.0
git checkout FETCH_HEAD
mkdir build
cd build
cmake .. -DPIC_ALWAYS=TRUE -DBUILD_DWARFDUMP=FALSE
make -j
sudo make install
