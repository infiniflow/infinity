#!/usr/bin/env bash
# Gate A — SQL conformance over the reference's .slt suite (plan §6 Gate A).
#
#   ./scripts/run_gate_a.sh [slt_dir|file]   # default: <repo>/test/sql
#
# The suite is parsed, executed and validated by sqllogictest-rs, the crate the
# reference's own runner is built from, so the format has one implementation.
# conformance prints the failing files and exits non-zero; the gate is that
# exit status (any failing file fails Gate A).
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
slt="${1:-$root/test/sql}"

cd "$here/.."
cargo run -q --release -p cli -- conformance "$slt"
status=$?

echo "== Gate A: $([[ $status -eq 0 ]] && echo PASS || echo FAIL) =="
exit $status
