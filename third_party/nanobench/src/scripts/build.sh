#!/usr/bin/env bash

ROOT_SRC_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd )"
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ${ROOT_SRC_DIR}
