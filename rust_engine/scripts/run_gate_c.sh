#!/usr/bin/env bash
# Gate C — persistence & recovery (plan §6 Gate C).
#   python/restart_test, test/sql/snapshot, WAL crash recovery, MinIO/object
#   storage (CI `minio` job), compaction-after-recovery, crash-during-recovery.
#
#   ./scripts/run_gate_c.sh
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
status=0

echo "== Gate C =="

# 1. in-repo restart / snapshot suites (work on the Rust engine today)
cd "$here/.." && cargo test --workspace -q || status=1
cargo run -q --release -p infinity-cli -- conformance "$root/test/sql/snapshot" \
  || { echo "snapshot .slt baseline not at 100%"; status=1; }

# 2. python restart_test (needs a running server + the python SDK).
#    The harness starts the engine as `<binary> --config=<toml>`; the shim maps
#    that onto the Rust `infinity-server`. Unprivileged users get a writable
#    `/var/infinity` (plus the CSV fixtures) via `bwrap` when it is available.
if [[ -d "$root/python/restart_test" ]]; then
  py="${PYTHON:-}"
  if [[ -z "$py" ]]; then
    if [[ -x "$root/.venv/bin/python" ]]; then py="$root/.venv/bin/python"; else py="python3"; fi
  fi
  shim="$here/infinity_rust_shim.sh"
  # cwd is the repository root: the suites address fixtures as
  # `test/data/...`, so running from `python/` silently imports nothing.
  run=( "$py" -m pytest -q python/restart_test --infinity_path="$shim" )
  if command -v bwrap >/dev/null 2>&1; then
    data="${INFINITY_GATE_C_DATA:-$(mktemp -d)}"
    mkdir -p "$data/tmp"
    cp "$root"/test/data/csv/* "$data/" 2>/dev/null || true
    echo "restart_test: bwrap sandbox, fixtures at $data"
    # `--unshare-pid` is required: the harness kills *every* process named
    # `infinity` before each start, which fails on an unprivileged user when a
    # foreign process matches. A private PID namespace scopes that to the run.
    run=( bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
          --bind "$data" /var/infinity/test_data --dir /var/infinity/log \
          --unshare-pid --proc /proc \
          -- "${run[@]}" )
  fi
  ( cd "$root" && "${run[@]}" ) || status=1
else
  echo "SKIP: python/restart_test not present"
fi

# 3. object storage (MinIO): requires the CI minio service
if [[ -n "${INFINITY_MINIO_ENDPOINT:-}" ]]; then
  echo "minio endpoint: $INFINITY_MINIO_ENDPOINT"
  ( cd "$root/python" && python3 -m pytest -q test_minio ) || status=1
else
  echo "SKIP: INFINITY_MINIO_ENDPOINT not set (MinIO job runs in CI)"
fi

echo "== Gate C result: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
