#!/bin/bash

# Find the current directory
current_dir=$(dirname "$0")
SRC_DIR=$(cd "$current_dir"; pwd)
echo "SRC_DIR : $SRC_DIR"
CPP_DST_DIR=$(cd "$current_dir/.."; pwd)"/src/network"
echo "CPP_DST_DIR : $CPP_DST_DIR"
PYTHON_DST_DIR=$(cd "$current_dir/.."; pwd)"/python/infinity/remote/infinity_grpc"
echo "PYTHON_DST_DIR : $PYTHON_DST_DIR"
GRPC_CPP_PLUGIN_DIR=$(which grpc_cpp_plugin)
/usr/bin/protoc -I=$SRC_DIR --cpp_out=$CPP_DST_DIR  $SRC_DIR/infinity.proto
echo "Generating proto for cpp step 1"
/usr/bin/protoc -I=$SRC_DIR --grpc_out=$CPP_DST_DIR --plugin=protoc-gen-grpc=$GRPC_CPP_PLUGIN_DIR $SRC_DIR/infinity.proto
echo "Generating proto for cpp step 2"
python3 -m grpc_tools.protoc -I=$SRC_DIR --python_out=$PYTHON_DST_DIR --grpc_python_out=$PYTHON_DST_DIR $SRC_DIR/infinity.proto
echo "Generating proto for python done"

#python -m grpc_tools.protoc -I=$SRC_DIR --cpp_out=$CPP_DST_DIR  --grpc_cpp_out=$CPP_DST_DIR --plugin=grpc_cpp_plugin $SRC_DIR/infinity.proto