#!/usr/bin/env bash
set -ex

ROOTDIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd )"


function build() {
    #NICE="nice -n20"
    NICE="chrt -i 0"

    ORIGINDIR=$(pwd)

    COMPILER=$1
    CXX_STANDARD=$2
    SANITIZER=$3
    CXXFLAGS=$4

    DIRNAME=${COMPILER}_cxx${CXX_STANDARD}_sanitizer${SANITIZER}_${CXXFLAGS}
    
    rm -Rf ${DIRNAME}
    mkdir -p ${DIRNAME}
    cd ${DIRNAME}

    CXX=$(which ${COMPILER}) cmake -G Ninja -DCMAKE_CXX_FLAGS=${CXXFLAGS} -DCMAKE_BUILD_TYPE=Release -DNB_cxx_standard=${CXX_STANDARD} -DNB_sanitizer=${SANITIZER} ${ROOTDIR}
    ${NICE} cmake --build .
    rm -f ubsan.log*

    UBSAN_OPTIONS=print_stacktrace=1:log_path=ubsan.log:suppressions=${ROOTDIR}/ubsan.supp ./nb
    if ls ubsan.log* 1> /dev/null 2>&1; then
        cat ubsan.log*
        exit 1
    fi
 
    cd ${ORIGINDIR}
}


# put fi down to skip
#if false; then
#fi

## DON'T MODIFY PAST HERE! Just copy&past above this line to test it before other stuff.

build "clang++" "11" "OFF"
build "clang++" "11" "OFF" "-m32"
build "clang++" "11" "ON"
#build "clang++" "11" "ON" "-m32" # linker error in chrono

build "clang++" "14" "OFF"
build "clang++" "14" "OFF" "-m32"
build "clang++" "14" "ON"
#build "clang++" "14" "ON" "-m32" # linker error in chrono

build "clang++" "17" "OFF"
build "clang++" "17" "OFF" "-m32"
build "clang++" "17" "ON"
#build "clang++" "17" "ON" "-m32" # linker error in chrono

build "clang++" "20" "OFF"
build "clang++" "20" "OFF" "-m32"
build "clang++" "20" "ON"
#build "clang++" "20" "ON" "-m32" # linker error in chrono

build "clang++-6" "11" "OFF"
build "clang++-6" "11" "OFF" "-m32"
#build "clang++-6" "11" "ON" # can't find asan lib
#build "clang++-6" "11" "ON" "-m32" # can't find asan lib

build "clang++-6" "14" "OFF"
build "clang++-6" "14" "OFF" "-m32"
#build "clang++-6" "14" "ON" # can't find asan lib
#build "clang++-6" "14" "ON" "-m32" # can't find asan lib

build "clang++-6" "17" "OFF"
build "clang++-6" "17" "OFF" "-m32"
#build "clang++-6" "17" "ON" # can't find asan lib
#build "clang++-6" "17" "ON" "-m32" # can't find asan lib

#build "clang++-6" "20" "OFF"
#build "clang++-6" "20" "OFF" "-m32"
#build "clang++-6" "20" "ON" # can't find asan lib
#build "clang++-6" "20" "ON" "-m32" # can't find asan lib

build "g++" "11" "OFF"
build "g++" "11" "OFF" "-m32"
build "g++" "11" "ON"
build "g++" "11" "ON" "-m32"

build "g++" "14" "OFF"
build "g++" "14" "OFF" "-m32"
build "g++" "14" "ON"
build "g++" "14" "ON" "-m32"

build "g++" "17" "OFF"
build "g++" "17" "OFF" "-m32"
build "g++" "17" "ON"
build "g++" "17" "ON" "-m32"

build "g++" "20" "OFF"
build "g++" "20" "OFF" "-m32"
build "g++" "20" "ON"
build "g++" "20" "ON" "-m32"

build "g++-4.8" "11" "OFF"
#build "g++-4.8" "11" "OFF" "-m32" # skipping incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/4.8.5/libstdc++.so when searching for -lstdc++
#build "g++-4.8" "11" "ON" # no -fsanitize=undefined,float-divide-by-zero,float-cast-overflow
#build "g++-4.8" "11" "ON" "-m32" # skipping incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/4.8.5/libstdc++.so when searching for -lstdc++

build "g++-4.9" "11" "OFF"
build "g++-4.9" "11" "OFF" "-m32"
#build "g++-4.9" "11" "ON" # unrecognized fsanitize options
#build "g++-4.9" "11" "ON" "-m32"  # unrecognized fsanitize options

build "g++-4.9" "14" "OFF"
build "g++-4.9" "14" "OFF" "-m32"
#build "g++-4.9" "14" "ON" # unrecognized fsanitize options
#build "g++-4.9" "14" "ON" "-m32" # unrecognized fsanitize options

build "g++-5" "11" "OFF"
#build "g++-5" "11" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-5" "11" "ON" # unrecognized fsanitize options
#build "g++-5" "11" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-5" "14" "OFF"
#build "g++-5" "14" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-5" "14" "ON" # unrecognized fsanitize options
#build "g++-5" "14" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-5" "17" "OFF"
#build "g++-5" "17" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-5" "17" "ON" # unrecognized fsanitize options
#build "g++-5" "17" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-6" "11" "OFF"
#build "g++-6" "11" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-6" "11" "ON" # unrecognized fsanitize options
#build "g++-6" "11" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-6" "14" "OFF"
#build "g++-6" "14" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-6" "14" "ON" # unrecognized fsanitize options
#build "g++-6" "14" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-6" "17" "OFF"
#build "g++-6" "17" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-6" "17" "ON" # unrecognized fsanitize options
#build "g++-6" "17" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-7" "11" "OFF"
#build "g++-7" "11" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-7" "11" "ON" # unrecognized fsanitize options
#build "g++-7" "11" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-7" "14" "OFF"
#build "g++-7" "14" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-7" "14" "ON" # unrecognized fsanitize options
#build "g++-7" "14" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so

build "g++-7" "17" "OFF"
#build "g++-7" "17" "OFF" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
#build "g++-7" "17" "ON" # unrecognized fsanitize options
#build "g++-7" "17" "ON" "-m32" # incompatible /usr/lib/gcc/x86_64-pc-linux-gnu/5.5.0/libstdc++.so
