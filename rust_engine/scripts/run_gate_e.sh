#!/usr/bin/env bash
# Gate E — performance regression (plan §6 Gate E).
#   Runs the Rust benchmark driver and compares the mean "Total cost" against a
#   golden log with `tools/ci_tools/check_benchmark_result.py` (<7% regression).
#
#   ./scripts/run_gate_e.sh [<golden_log>]
#
# Without a golden log the script takes a *self-baseline*: it runs the driver
# twice and requires the second run to stay within 7% of the first (a within-
# machine regression detector). The C++ golden logs are produced by the C++
# benchmark harness on comparable hardware.
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
status=0
rows="${INFINITY_BENCH_ROWS:-2000}"

echo "== Gate E =="
checker="$root/tools/ci_tools/check_benchmark_result.py"
if [[ ! -f "$checker" ]]; then
  echo "SKIP: $checker not present"
  exit 0
fi

cd "$here/.." || exit 1
run_bench() {
  cargo run -q --release -p infinity-cli -- bench "$(mktemp -d)/infinity-bench" "$rows" \
    | tee "$1"
}

latest=/tmp/infinity-bench.log
run_bench "$latest" || status=1

if [[ $# -ge 1 ]]; then
  golden="$1"
  echo "golden: $golden"
else
  golden=/tmp/infinity-bench.golden.log
  echo "no golden passed — establishing a self-baseline at $golden"
  cp "$latest" "$golden"
  run_bench "$latest" || status=1
fi

python3 "$checker" "$golden" "$latest" || status=1

echo "== Gate E result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
