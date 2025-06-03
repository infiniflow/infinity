mkdir zstd
cd zstd
git init
git remote add origin https://github.com/facebook/zstd.git
git fetch --depth 1 origin 63779c798237346c2b245c546c40b72a5a5913fe # 1.5.5
git checkout FETCH_HEAD
cd build/cmake
mkdir build
cd build
cmake .. -DZSTD_BUILD_SHARED=On -DZSTD_BUILD_SHARED=Off -DZSTD_LEGACY_SUPPORT=Off -DZSTD_BUILD_PROGRAMS=Off -DZSTD_BUILD_CONTRIB=Off -DZSTD_BUILD_TESTS=Off -G"Unix Makefiles"
make -j
make install

cd ../../../..

mkdir libdwarf
cd libdwarf
git init
git remote add origin https://github.com/jeremy-rifkin/libdwarf-lite.git
git fetch --depth 1 origin fe09ca800b988e2ff21225ac5e7468ceade2a30e
git checkout FETCH_HEAD
mkdir build
cd build
cmake .. -DPIC_ALWAYS=TRUE -DBUILD_DWARFDUMP=FALSE -G"Unix Makefiles"
make -j
make install
