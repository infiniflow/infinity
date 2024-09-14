#!/usr/bin/env bash

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
https://github.com/Kitware/CMake/releases/download/v3.30.1/cmake-3.30.1-linux-x86_64.tar.gz
https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-linux.zip
https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.8/llvm-project-18.1.8.src.tar.xz
https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.bz2
https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz
https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
https://github.com/lz4/lz4/releases/download/v1.10.0/lz4-1.10.0.tar.gz
https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz
https://github.com/facebook/zstd/releases/download/v1.5.5/zstd-1.5.5.tar.gz
https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz
https://github.com/google/brotli/archive/refs/tags/v1.1.0.tar.gz
https://github.com/jemalloc/jemalloc/releases/download/5.3.0/jemalloc-5.3.0.tar.bz2
https://github.com/gperftools/gperftools/releases/download/gperftools-2.15/gperftools-2.15.tar.gz
https://github.com/openssl/openssl/releases/download/openssl-3.4.0-alpha1/openssl-3.4.0-alpha1.tar.gz
https://curl.se/download/curl-8.4.0.tar.gz
https://github.com/jpbarrette/curlpp/archive/refs/tags/v0.8.1.tar.gz
https://github.com/zeux/pugixml/releases/download/v1.14/pugixml-1.14.tar.gz
https://github.com/benhoyt/inih/archive/refs/tags/r58.tar.gz
https://dlcdn.apache.org/thrift/0.20.0/thrift-0.20.0.tar.gz
https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh

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
