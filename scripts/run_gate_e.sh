#!/usr/bin/env bash
# Gate E: performance regression check (plan §6).
#
# Runs the same SDK workload twice against the Rust server and compares the two
# logs with the CI checker (`tools/ci_tools/check_benchmark_result.py`, <7%).
# When a golden log produced by the C++ engine is available, pass it as
# GOLDEN_LOG to compare against that instead:
#
#   GOLDEN_LOG=/path/to/cpp_golden.log scripts/run_gate_e.sh
#
# The driver (`tools/bench_sdk_driver.py`) prints `Total cost : <t> s` lines, so
# the same script produces the golden when pointed at a C++ server.
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

ROWS=${ROWS:-20000}
PORT=${PORT:-23917}
WORK=${WORK:-/tmp/gate_e}
BIN="$ROOT/rust_engine/target/release/infinity"
PY=${PY:-$ROOT/.venv/bin/python}

mkdir -p "$WORK"
rm -rf "$WORK/data" "$WORK/baseline.log" "$WORK/latest.log"

run_once() {
    local out=$1
    rm -rf "$WORK/data"
    INFINITY_BIND="127.0.0.1:$PORT" "$BIN" --config="$WORK/infinity.toml" \
        > "$WORK/server.log" 2>&1 &
    local pid=$!
    local started=0
    for _ in $(seq 1 60); do
        if (echo > "/dev/tcp/127.0.0.1/$PORT") 2>/dev/null; then started=1; break; fi
        sleep 1
    done
    if [[ $started -ne 1 ]]; then
        echo "server did not start on port $PORT" >&2
        cat "$WORK/server.log" >&2 || true
        kill -9 "$pid" 2>/dev/null || true
        return 1
    fi
    "$PY" tools/bench_sdk_driver.py 127.0.0.1 "$PORT" "$ROWS" > "$out"
    kill -9 "$pid" 2>/dev/null || true
    wait "$pid" 2>/dev/null || true
}

cat > "$WORK/infinity.toml" <<EOF
[general]
version = "0.7.4"
[storage]
persistence_dir = "$WORK/data/persistence"
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"
[wal]
checkpoint_interval = "0s"
[resource]
EOF

echo "== Gate E: $ROWS rows, driver runs =="
run_once "$WORK/baseline.log"
echo "baseline:"; cat "$WORK/baseline.log"
run_once "$WORK/latest.log"
echo "latest:"; cat "$WORK/latest.log"

if [[ -n "${GOLDEN_LOG:-}" ]]; then
    echo "-- comparing against C++ golden: $GOLDEN_LOG"
    "$PY" tools/ci_tools/check_benchmark_result.py "$GOLDEN_LOG" "$WORK/latest.log"
else
    echo "-- no GOLDEN_LOG set: self-baseline (two runs of the same engine)"
    "$PY" tools/ci_tools/check_benchmark_result.py "$WORK/baseline.log" "$WORK/latest.log"
fi
