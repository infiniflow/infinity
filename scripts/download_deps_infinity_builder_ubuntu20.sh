#!/usr/bin/env bash

# This script should be run from the infinity project root directory
# Usage: ./scripts/download_deps_infinity_builder_ubuntu20.sh

# Check if we're in the correct directory
if [ ! -d ".git" ] || [ ! -d "scripts" ]; then
    echo "Error: This script must be run from the infinity project root directory"
    echo "Usage: ./scripts/download_deps_infinity_builder_ubuntu20.sh"
    exit 1
fi

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
names="https://github.com/Kitware/CMake/releases/download/v4.1.0/cmake-4.1.0-linux-x86_64.tar.gz
https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-linux.zip
https://www.python.org/ftp/python/3.10.18/Python-3.10.18.tar.xz
https://github.com/gcc-mirror/gcc/archive/refs/tags/releases/gcc-15.2.0.tar.gz
https://github.com/jemalloc/jemalloc/releases/download/5.3.0/jemalloc-5.3.0.tar.bz2
https://github.com/gperftools/gperftools/releases/download/gperftools-2.17/gperftools-2.17.tar.gz
https://github.com/risinglightdb/sqllogictest-rs/releases/download/v0.28.3/sqllogictest-bin-v0.28.3-x86_64-unknown-linux-musl.tar.gz
https://github.com/rui314/mold/releases/download/v2.40.3/mold-2.40.3-x86_64-linux.tar.gz
https://download.docker.com/linux/static/stable/x86_64/docker-28.3.3.tgz"

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
