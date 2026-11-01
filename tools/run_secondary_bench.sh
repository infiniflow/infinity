#!/usr/bin/env bash
# Secondary-index benchmark, reference vs port: both engines, one machine, the
# same client and SQL, every background task off. Each engine runs in its own
# network namespace so port 23817 is private to the run.
#
#   tools/run_secondary_bench.sh lookup   # build + lookups + one maintenance pass
#   tools/run_secondary_bench.sh maint    # maintenance cost per live index
#
# Env: SEC_ROWS (default 100000), SEC_POINTS, SEC_RANGES.
# Results and how to read them: docs/SECONDARY_INDEX_BENCH.md.
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$here"
export SEC_ROWS=${SEC_ROWS:-100000}
case "${1:-lookup}" in
  lookup) export SEC_DRIVER=tools/bench_secondary_lookup.py ;;
  maint) export SEC_DRIVER=tools/bench_secondary_maint.py ;;
  *) echo "usage: $0 [lookup|maint]"; exit 2 ;;
esac
for engine in cpp rust; do
  bwrap --dev-bind / / --unshare-net -- bash tools/run_secondary_one.sh "$engine" \
    || echo "FAILED: $engine"
done