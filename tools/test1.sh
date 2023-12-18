#!/bin/bash

GITHUB_WORKSPACE=/data01/zhichyu

echo "#    - name: start builder container"
sudo docker rm -f infinity_build
sudo docker run -d --name infinity_build --network=host -v $GITHUB_WORKSPACE/infinity:/infinity infiniflow/infinity_build:0.1
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: Build debug version"
sudo docker exec infinity_build bash -c "cd /infinity && rm -fr cmake-build-debug && mkdir -p cmake-build-debug && cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S /infinity -B /infinity/cmake-build-debug && cmake --build /infinity/cmake-build-debug"
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: Build release version"
sudo docker exec infinity_build bash -c "cd /infinity && rm -fr cmake-build-release && mkdir -p cmake-build-release && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S /infinity -B /infinity/cmake-build-release && cmake --build /infinity/cmake-build-release"
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi
