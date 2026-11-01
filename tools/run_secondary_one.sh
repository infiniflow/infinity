#!/usr/bin/env bash
# One engine, one benchmark run: start it, wait for its port, run the driver,
# stop it. Invoked *inside* its own network namespace by
# `run_secondary_bench.sh`, so 23817 is private to the run and a server another
# session started cannot answer for it.
#
#   SEC_DRIVER=tools/bench_secondary_lookup.py run_secondary_one.sh cpp
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$here"
engine=$1
root="$PWD/.scratch/sec-$engine"
rm -rf "$root"
mkdir -p "$root/log" "$root/tmp"
persistence=""
[[ "$engine" == "rust" ]] && persistence="$root/persistence"
# The reference takes data_dir/catalog_dir/wal_dir and an empty persistence_dir;
# the port derives its root from the *parent* of persistence_dir.
cat > "$root/engine.toml" <<EOF
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
log_dir = "$root/log"
log_to_stdout = false
log_level = "info"

[storage]
data_dir = "$root/data"
catalog_dir = "$root/catalog"
persistence_dir = "$persistence"
# Every background task off: the measurement is the statement path, not a
# worker thread that happens to run mid-script.
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"
mem_index_capacity = 65536

[buffer]
buffer_manager_size = "2GB"
temp_dir = "$root/tmp"
result_cache = "off"
memindex_memory_quota = "2GB"

[wal]
wal_dir = "$root/wal"
checkpoint_interval = "0s"

[resource]
EOF
if [[ "$engine" == "rust" ]]; then
  bin="$here/rust_engine/target/release/infinity"
else
  bin="$here/cmake-build-reldeb/src/infinity"
fi
echo "== $engine (${SEC_ROWS:-100000} rows, $(basename "${SEC_DRIVER:-}" .py))"
"$bin" --config="$root/engine.toml" > "$root/stdout.log" 2>&1 &
srv=$!
for _ in $(seq 1 120); do
  if ! kill -0 "$srv" 2>/dev/null; then
    echo "engine exited early:"; tail -5 "$root/stdout.log"; exit 1
  fi
  if "$here/.venv/bin/python" - <<PY 2>/dev/null
import socket, sys
s = socket.socket(); s.settimeout(0.2)
try:
    s.connect(("127.0.0.1", 23817)); sys.exit(0)
except Exception:
    sys.exit(1)
PY
  then break; fi
  sleep 0.5
done
echo "serving: $(readlink -f /proc/$(ss -ltnp 2>/dev/null | sed -n "s/.*pid=\([0-9]*\).*/\1/p" | head -1)/exe 2>/dev/null)"
"$here/.venv/bin/python" "${SEC_DRIVER:-tools/bench_secondary_lookup.py}"
code=$?
kill "$srv" 2>/dev/null
wait "$srv" 2>/dev/null
exit $code