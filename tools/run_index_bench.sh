#!/usr/bin/env bash
# Per-index benchmarks: build and query cost of each index kind, reference vs
# port, one machine, the same client and statements.
#
#   tools/run_index_bench.sh <workload> [rows] [queries]
#
# Workloads (docs/INDEX_PERF.md says what each one answers):
#   index-types        fulltext / hnsw / secondary / sparse: build + query
#   multivector        EMVB / PLAID / SMVE on a tensor column
#   secondary-lookup   secondary index: build, point/range lookups, DML
#   secondary-maint    secondary index: cost of keeping it current per row
#   sdk                aggregate insert / select / point (the Gate E driver)
#
# Each engine runs in its own network namespace (`bwrap --unshare-net`), so
# 23817 is private to the run and a server another session started cannot
# answer for it.
#
# Env: BENCH_ROWS (default 100000), BENCH_QUERIES (default 50), SEC_ROWS
# (the secondary workloads read it; defaults to <rows>).
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/.." && pwd)"
cd "$root"

workload=${1:-index-types}
rows=${2:-${BENCH_ROWS:-100000}}
queries=${3:-${BENCH_QUERIES:-50}}
export SEC_ROWS=${SEC_ROWS:-$rows}

case "$workload" in
  index-types)      driver=tools/bench_index_types.py;       argrow=("127.0.0.1" 23817 "$rows" "$queries") ;;
  multivector)      driver=tools/bench_multivector.py;       argrow=("127.0.0.1" 23817 "$rows" "$queries") ;;
  secondary-lookup) driver=tools/bench_secondary_lookup.py;  argrow=() ;;
  secondary-maint)  driver=tools/bench_secondary_maint.py;   argrow=() ;;
  sdk)              driver=tools/bench_sdk_driver.py;        argrow=("127.0.0.1" 23817 "$rows") ;;
  *) echo "usage: $0 <index-types|multivector|secondary-lookup|secondary-maint|sdk> [rows] [queries]"; exit 2 ;;
esac

status=0
for engine in cpp rust; do
  if ! bwrap --dev-bind / / --unshare-net -- bash "$here/run_index_bench_one.sh" \
      "$engine" "$workload" "$rows" "$driver" "${argrow[@]}"; then
    echo "FAILED: $engine"
    status=1
  fi
done
exit $status
