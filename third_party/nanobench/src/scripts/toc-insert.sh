#!/usr/bin/env bash
ROOT_DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd )"

find ${ROOT_DIR} -name "*.md" -exec ${ROOT_DIR}/src/scripts/gh-md-toc --insert \{\} \;
find ${ROOT_DIR} -name "*.md.orig.*" -exec rm \{\} \;
find ${ROOT_DIR} -name "*.md.toc.*" -exec rm \{\} \;
