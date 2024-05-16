#!/usr/bin/env bash

# This script will download the following files:
# bison-3.8.2.tar.xz
# binutils-2.41.tar.xz
# gcc-13.2.0.tar.xz
# cmake-3.28.3-linux-x86_64.tar.gz
# ninja-linux.zip
# llvm-project-17.0.6.src.tar.xz
# boost_1_81_0.tar.bz2
# flex-2.6.4.tar.gz
# liburing-2.5.tar.gz
# libevent-2.1.12-stable.tar.gz
# lz4-1.9.4.tar.gz

download()
{
    echo "download $1"
    # https://stackoverflow.com/questions/3162385/how-to-split-a-string-in-shell-and-get-the-last-field
    fn=${1##*/}
    if [ ! -f $fn ] ; then
        wget --no-check-certificate $1
    fi
}

# https://stackoverflow.com/questions/24628076/convert-multiline-string-to-array
names="https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.xz
https://ftp.gnu.org/gnu/binutils/binutils-2.41.tar.xz
https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.xz
https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.tar.gz
https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip
https://github.com/llvm/llvm-project/releases/download/llvmorg-17.0.6/llvm-project-17.0.6.src.tar.xz
https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.bz2
https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz
https://github.com/axboe/liburing/archive/refs/tags/liburing-2.5.tar.gz
https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
https://github.com/lz4/lz4/releases/download/v1.9.4/lz4-1.9.4.tar.gz
https://github.com/jemalloc/jemalloc/releases/download/5.3.0/jemalloc-5.3.0.tar.bz2
https://github.com/gperftools/gperftools/releases/download/gperftools-2.15/gperftools-2.15.tar.gz
https://github.com/openssl/openssl/releases/download/OpenSSL_1_1_1w/openssl-1.1.1w.tar.gz
https://www.python.org/ftp/python/3.12.3/Python-3.12.3.tar.xz"

SAVEIFS=$IFS   # Save current IFS (Internal Field Separator)
IFS=$'\n'      # Change IFS to newline char
names=($names) # split the `names` string into an array by the same name
IFS=$SAVEIFS   # Restore original IFS

find . -size 0 | xargs rm -f
# https://stackoverflow.com/questions/15466808/shell-iterate-over-array
for ((i=0; i<${#names[@]}; i+=1)); do
    url="${names[$i]}"
    download $url
    if [ $? != 0 ]; then
	exit -1
    fi
done
find . -size 0 | xargs rm -f
