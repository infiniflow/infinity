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

# 2. python restart_test (needs a running server + the python SDK)
if [[ -d "$root/python/restart_test" ]]; then
  ( cd "$root/python" && python3 -m pytest -q restart_test ) || status=1
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
