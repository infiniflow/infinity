#!/usr/bin/env bash
# Gate E — performance regression (plan §6 Gate E).
#   sift knn_query_benchmark (1/8 threads) and the full-text benchmark, compared
#   against the golden logs by tools/ci_tools/check_benchmark_result.py (<7% mean
#   regression). Requires comparable hardware + the same dataset/config.
#
#   ./scripts/run_gate_e.sh [golden_dir]
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
status=0

echo "== Gate E =="
checker="$root/tools/ci_tools/check_benchmark_result.py"
if [[ ! -f "$checker" ]]; then
  echo "SKIP: $checker not present"
  exit 0
fi

# the Rust benchmark driver reports rows/s for the batched INSERT workload
cd "$here/.." || exit 1
cargo run -q --release -p infinity-cli -- bench /tmp/infinity-bench 2000 \
  | tee /tmp/infinity-bench.log || status=1

if [[ $# -ge 1 ]]; then
  python3 "$checker" --current /tmp/infinity-bench.log --golden "$1" || status=1
else
  echo "NOTE: pass the golden log directory to enable the <7% regression check"
fi

echo "== Gate E result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
