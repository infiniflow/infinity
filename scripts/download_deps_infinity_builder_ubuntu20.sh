#!/usr/bin/env bash

download()
{
    echo "download $@"
    if [ $# -ge 2 ]; then
        # Get the filename from the second argument
        fn="$2"
    else
        # Get the filename from the URL
        # https://stackoverflow.com/questions/3162385/how-to-split-a-string-in-shell-and-get-the-last-field
        fn=${1##*/}
    fi
    if [ ! -f $fn ] || [ ! -s $fn ] ; then
        curl --insecure --location --output "$fn" "$1"
    fi
}

# https://stackoverflow.com/questions/24628076/convert-multiline-string-to-array
names="https://github.com/Kitware/CMake/releases/download/v3.31.7/cmake-3.31.7-linux-x86_64.tar.gz
https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-linux.zip
https://www.python.org/ftp/python/3.10.17/Python-3.10.17.tar.xz
https://github.com/gcc-mirror/gcc/archive/refs/tags/releases/gcc-15.1.0.tar.gz
https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
https://github.com/lz4/lz4/releases/download/v1.10.0/lz4-1.10.0.tar.gz
https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz
https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz
https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz
https://github.com/google/brotli/archive/refs/tags/v1.1.0.tar.gz
https://github.com/jemalloc/jemalloc/releases/download/5.3.0/jemalloc-5.3.0.tar.bz2
https://github.com/gperftools/gperftools/releases/download/gperftools-2.15/gperftools-2.15.tar.gz
https://github.com/openssl/openssl/releases/download/OpenSSL_1_1_1w/openssl-1.1.1w.tar.gz
https://github.com/openssl/openssl/releases/download/openssl-3.0.15/openssl-3.0.15.tar.gz
https://github.com/apache/thrift/archive/refs/tags/v0.22.0.tar.gz   thrift-0.22.0.tar.gz
https://github.com/risinglightdb/sqllogictest-rs/releases/download/v0.28.2/sqllogictest-bin-v0.28.2-x86_64-unknown-linux-musl.tar.gz
https://github.com/adrian-thurston/colm/archive/bd19b1ab7c0bcdba2772061bc38f6bca5cbba977.zip   colm-bd19b1ab7c0bcdba2772061bc38f6bca5cbba977.zip
https://github.com/adrian-thurston/ragel/archive/refs/heads/master.zip   ragel-master.zip
https://github.com/VectorCamp/vectorscan/archive/refs/heads/master.zip   vectorscan-master.zip
https://github.com/rui314/mold/releases/download/v2.40.3/mold-2.40.3-x86_64-linux.tar.gz
https://mirrors.edge.kernel.org/pub/linux/utils/net/iproute2/iproute2-6.9.0.tar.gz
https://download.docker.com/linux/static/stable/x86_64/docker-27.4.1.tgz"

SAVEIFS=$IFS   # Save current IFS (Internal Field Separator)
IFS=$'\n'      # Change IFS to newline char
names=($names) # split the `names` string into an array by the same name
IFS=$SAVEIFS   # Restore original IFS

# https://stackoverflow.com/questions/15466808/shell-iterate-over-array
for ((i=0; i<${#names[@]}; i+=1)); do
    url="${names[$i]}"
    download $url
    if [ $? != 0 ]; then
	exit -1
    fi
done

# Download vcpkg dependencies
echo "Setting up vcpkg and downloading dependencies..."

# Clone vcpkg if not exists
if [ ! -d "vcpkg" ]; then
    git clone https://github.com/microsoft/vcpkg.git vcpkg
fi

cd vcpkg

# Bootstrap vcpkg if not already done
if [ ! -f "vcpkg" ]; then
    ./bootstrap-vcpkg.sh --disableMetrics
fi

# Copy vcpkg.json to current directory for dependency resolution
if [ -f "../vcpkg.json" ]; then
    cp ../vcpkg.json .
elif [ -f "../../vcpkg.json" ]; then
    cp ../../vcpkg.json .
else
    echo "Error: vcpkg.json not found. Please ensure vcpkg.json is in the project root."
    exit 1
fi

# Download all vcpkg dependencies without building
echo "Downloading vcpkg dependencies..."
./vcpkg install --only-downloads

echo "vcpkg dependencies downloaded successfully."
cd ..
