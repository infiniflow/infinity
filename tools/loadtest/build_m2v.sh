#!/usr/bin/env bash
# Build the model2vec C ABI shared library (cdylib) that the Go load test links against.
#
# The Go program (package loadtest) is the whole harness: it reads the wiki
# extract, cleans it, embeds through this library, creates tables, imports, and
# drives the query mix. Nothing else has to be run first except this build.
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$here/m2v"

cargo build --release

lib="$here/m2v/target/release/libm2v.so"
if [[ ! -f "$lib" ]]; then
  echo "expected $lib to exist after cargo build" >&2
  exit 1
fi

echo "built $lib"
echo "run: go test -count=1 -v -run TestLoadTest ./..."
