#!/usr/bin/env bash
# Gate B — SDK / wire-protocol compatibility (plan §6 Gate B).
#   python/test_pysdk (313), go/tests (375), HTTP (--http) — all **unmodified SDKs**.
#
#   ./scripts/run_gate_b.sh
#
# Preconditions: the toolchains below must be present. The script installs
# nothing; it reports clearly what is missing so the gate result is auditable.
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
port="${INFINITY_PORT:-23817}"

need() { command -v "$1" >/dev/null 2>&1 || { echo "MISSING: $1 ($2)"; return 1; }; }

echo "== Gate B =="
status=0
need python3 "python SDK tests" || status=1
need go      "go SDK tests"      || status=1

# 1. build and start the Rust server (Thrift on $port)
cd "$here/.." || exit 1
cargo build -q --release -p infinity-server || exit 1
# the server binary name follows the crate's bin target
srv_bin="$(ls target/release/infinity-server 2>/dev/null || true)"
if [[ -z "$srv_bin" ]]; then echo "no infinity-server binary built"; exit 1; fi

tmp="$(mktemp -d)"
"$srv_bin" "$tmp" >"$tmp/server.log" 2>&1 &
srv=$!
trap 'kill $srv 2>/dev/null; rm -rf "$tmp"' EXIT
sleep 2

# 2. python SDK tests
if [[ -d "$root/python/test_pysdk" ]]; then
  ( cd "$root/python" && python3 -m pytest -q test_pysdk ) \
    | tee "$tmp/pysdk.log" || status=1
else
  echo "SKIP: $root/python/test_pysdk not present"
fi

# 3. go SDK tests
if [[ -d "$root/go/tests" ]]; then
  ( cd "$root/go" && go test ./tests/... ) | tee "$tmp/go.log" || status=1
else
  echo "SKIP: $root/go/tests not present"
fi

# 4. HTTP API tests
if [[ -f "$root/tools/run_http_api.py" ]]; then
  python3 "$root/tools/run_http_api.py" | tee "$tmp/http.log" || status=1
else
  echo "SKIP: $root/tools/run_http_api.py not present"
fi

echo "== Gate B result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
