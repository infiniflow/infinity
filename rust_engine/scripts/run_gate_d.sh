#!/usr/bin/env bash
# Gate D — concurrency & stability (plan §6 Gate D).
#   python/parallel_test; bounded loom models (capacity=1, early stop, error,
#   cancel); miri on FFI-free core tests; native TSAN/ASAN where FFI/SIMD exist.
#
#   ./scripts/run_gate_d.sh
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
status=0

echo "== Gate D =="
cd "$here/.." || exit 1

# 1. loom: core scheduler/channel state machines
RUSTFLAGS="--cfg loom" cargo test -q -p infinity-txn --lib loom || status=1

# 2. miri on the FFI-free core (parser/expression/planner are pure Rust)
if rustup component list --installed 2>/dev/null | grep -q '^miri'; then
  cargo +nightly miri test -p infinity-expression -p infinity-planner -p infinity-parser \
    || status=1
else
  echo "SKIP: miri not installed (rustup component add miri --toolchain nightly)"
fi

# 3. parallel stress (needs a running server + python SDK)
if [[ -d "$root/python/parallel_test" ]]; then
  ( cd "$root/python" && python3 -m pytest -q parallel_test ) || status=1
else
  echo "SKIP: python/parallel_test not present"
fi

echo "== Gate D result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
