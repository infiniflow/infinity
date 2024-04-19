#!/bin/bash

echo "#    - name: install pysdk"
sudo docker exec infinity_build bash -c "cd /infinity/python && python3 setup.py install"
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: start infinity debug version"
sudo docker exec infinity_build bash -c "cd /infinity/ && rm -fr /var/infinity && cmake-build-debug/src/infinity > debug.log 2>&1" &
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: pysdk & sqllogictest debug version"
sudo docker exec infinity_build bash -c "cd /infinity/ && python3 tools/sqllogictest.py"
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: stop infinity debug version"
sudo kill `pidof infinity`
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: start infinity release version"
sudo docker exec infinity_build bash -c "cd /infinity/ && rm -fr /var/infinity && cmake-build-release/src/infinity > debug.log 2>&1" &
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: pysdk & sqllogictest release version"
sudo docker exec infinity_build bash -c "cd /infinity/ && python3 tools/sqllogictest.py"
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi

echo "#    - name: stop infinity release version"
sudo kill `pidof infinity`
if [ $? -ne 0 ] ; then
    echo "### failure! $? ###"
    exit 1
fi
