#!/bin/bash

for target in infinity-x64-arm64-builder infinity-x64-x64-v1-builder infinity-x64-x64-v2-builder infinity-x64-x64-v3-builder infinity-x64-x64-v4-builder; do
  docker build -f ./scripts/Dockerfile_infinity_builder_ubuntu22 --target "$target" -t "$target" .
done