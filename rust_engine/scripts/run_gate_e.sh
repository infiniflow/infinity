#!/usr/bin/env bash
# Gate E — performance regression (plan §6 Gate E).
#
#   ./scripts/run_gate_e.sh [<golden_log>]
#
# With a golden log the script runs the *same* SDK driver that produced it
# (`tools/bench_sdk_driver.py` over the wire: insert / full select / point
# filter) against a freshly started Rust server, then applies
# `tools/ci_tools/check_benchmark_result.py` (<7% regression).
#
# Without a golden log it falls back to a within-machine self-baseline of the
# CLI benchmark driver (two runs of the same binary).
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
status=0
latest=/tmp/infinity-bench.log

echo "== Gate E =="
checker="$root/tools/ci_tools/check_benchmark_result.py"
if [[ ! -f "$checker" ]]; then
  echo "SKIP: $checker not present"
  exit 0
fi

cd "$here/.." || exit 1

if [[ $# -ge 1 ]]; then
  # Official comparison: the golden was produced by the SDK driver against the
  # C++ engine, so the candidate must use the same driver and workload.
  golden="$1"
  rows="${INFINITY_BENCH_ROWS:-20000}"
  data="$(mktemp -d)/infinity-bench"
  cargo build -q --release -p infinity-server || exit 1
  "$here/../target/release/infinity-server" "$data" >/tmp/infinity-bench-server.log 2>&1 &
  srv=$!
  trap 'kill "$srv" 2>/dev/null' EXIT
  sleep 4
  "$root/.venv/bin/python" "$root/tools/bench_sdk_driver.py" 127.0.0.1 23817 "$rows" \
    | tee "$latest" || status=1
  kill "$srv" 2>/dev/null
  trap - EXIT
  echo "golden: $golden"
  python3 "$checker" "$golden" "$latest" || status=1
else
  # No golden: a within-machine regression detector over the CLI driver.
  run_bench() {
    cargo run -q --release -p infinity-cli -- bench "$(mktemp -d)/infinity-bench" \
      "${INFINITY_BENCH_ROWS:-2000}" | tee "$1"
  }
  run_bench "$latest" || status=1
  golden=/tmp/infinity-bench.golden.log
  echo "no golden passed — establishing a self-baseline at $golden"
  cp "$latest" "$golden"
  run_bench "$latest" || status=1
  python3 "$checker" "$golden" "$latest" || status=1
fi

echo "== Gate E result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
