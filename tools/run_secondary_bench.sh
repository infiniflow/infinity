#!/usr/bin/env bash
# Secondary-index benchmark (build, lookups, DML, maintenance).
#
#   tools/run_secondary_bench.sh lookup|maint [rows]
#
# A thin alias for `tools/run_index_bench.sh secondary-<lookup|maint>`, kept
# because the reports name it. See docs/INDEX_PERF.md.
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec "$here/run_index_bench.sh" "secondary-${1:-lookup}" "${2:-100000}"
