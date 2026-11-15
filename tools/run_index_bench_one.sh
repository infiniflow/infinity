#!/usr/bin/env bash
# One engine, one benchmark run: start it in this (private) network namespace,
# wait for 23817, run the driver, stop it. Invoked by run_index_bench.sh.
#
#   run_index_bench_one.sh <cpp|rust> <workload> <rows> <driver> [driver args…]
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/.." && pwd)"
cd "$root"
engine=$1; workload=$2; rows=$3; driver=$4
shift 4
# A unique dir per run: two runs of the same workload must not clobber each
# other (an `rm -rf` mid-run made the other one fail to start).
dir="$root/.scratch/bench-$workload-$engine-$$"
if [[ "$engine" == "rust" ]]; then
  bin="$root/rust_engine/target/release/infinity"
else
  bin="$root/cmake-build-reldeb/src/infinity"
fi
if [[ ! -x "$bin" ]]; then
  echo "SKIP $engine: $bin is not executable"; exit 0
fi
rm -rf "$dir"; mkdir -p "$dir/log" "$dir/tmp"
persistence=""
[[ "$engine" == "rust" ]] && persistence="$dir/persistence"
# The reference takes data_dir/catalog_dir/wal_dir and an empty
# persistence_dir; the port derives its root from the parent of
# persistence_dir. Every background task is off: the measurement is the
# statement path, not a worker thread that happens to run mid-script.
cat > "$dir/engine.toml" <<EOF
[general]
version = "0.7.4"
time_zone = "utc-8"

[network]
server_address = "0.0.0.0"
http_port = 23821
peer_ip = "0.0.0.0"
peer_port = 23851
postgres_port = 5432

[log]
log_dir = "$dir/log"
log_to_stdout = false
log_level = "info"

[storage]
data_dir = "$dir/data"
catalog_dir = "$dir/catalog"
persistence_dir = "$persistence"
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"
mem_index_capacity = 65536

[buffer]
buffer_manager_size = "2GB"
temp_dir = "$dir/tmp"
result_cache = "off"
memindex_memory_quota = "2GB"

[wal]
wal_dir = "$dir/wal"
checkpoint_interval = "0s"

[resource]
EOF
echo "== $engine ($workload, $rows rows)"
INFINITY_DATA="$dir" "$bin" --config="$dir/engine.toml" > "$dir/stdout.log" 2>&1 &
srv=$!
up=0
for _ in $(seq 1 120); do
  if ! kill -0 "$srv" 2>/dev/null; then
    echo "engine exited early:"; tail -5 "$dir/stdout.log"; exit 1
  fi
  if "$root/.venv/bin/python" - <<PY 2>/dev/null
import socket, sys
s = socket.socket(); s.settimeout(0.2)
try:
    s.connect(("127.0.0.1", 23817)); sys.exit(0)
except Exception:
    sys.exit(1)
PY
  then up=1; break; fi
  sleep 0.5
done
if [[ $up -eq 0 ]]; then
  echo "engine did not come up"; kill "$srv" 2>/dev/null; exit 1
fi
"$root/.venv/bin/python" "$driver" "$@"
code=$?
kill "$srv" 2>/dev/null
wait "$srv" 2>/dev/null
exit $code
