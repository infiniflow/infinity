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
#    miri needs a nightly toolchain; the repo pins `nightly-2025-04-04` for it.
miri_tc="nightly-2025-04-04"
if rustup component list --installed --toolchain "$miri_tc" 2>/dev/null | grep -q '^miri'; then
  cargo "+$miri_tc" miri test -p infinity-expression -p infinity-planner -p infinity-parser \
    || status=1
elif rustup component list --installed 2>/dev/null | grep -q '^miri'; then
  cargo +nightly miri test -p infinity-expression -p infinity-planner -p infinity-parser \
    || status=1
else
  echo "SKIP: miri not installed (rustup component add miri --toolchain $miri_tc)"
fi

# 3. Address/ThreadSanitizer on the FFI-free core crates (Gate G / G4).
#    Sanitizers are nightly rustc flags (`-Zsanitizer=…`) and need `-Zbuild-std`
#    (rust-src) so std matches the instrumented build; the pinned nightly is used
#    because the runtime must match the compiler. Only the pure-Rust crates are
#    built here: `infinity-storage`/`infinity-catalog`/`infinity-executor` pull
#    dependencies whose MSRV is above the pinned nightly.
san_tc="nightly-2025-04-04"
if rustup toolchain list 2>/dev/null | grep -q "^$san_tc"; then
  for san in address thread; do
    if RUSTFLAGS="-Zsanitizer=$san" cargo "+$san_tc" test \
        -p infinity-expression -p infinity-parser -p infinity-planner \
        --target x86_64-unknown-linux-gnu -Zbuild-std; then
      echo "sanitizer=$san: core crates clean"
    else
      status=1
    fi
  done
else
  echo "SKIP: $san_tc not installed (rustup toolchain install $san_tc && rustup component add rust-src --toolchain $san_tc)"
fi

# 4. parallel stress (needs a running server + python SDK)
if [[ -d "$root/python/parallel_test" ]]; then
  ( cd "$root/python" && python3 -m pytest -q parallel_test ) || status=1
else
  echo "SKIP: python/parallel_test not present"
fi

echo "== Gate D result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
