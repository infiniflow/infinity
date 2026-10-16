#!/usr/bin/env bash
# Gate A — SQL conformance over the real `.slt` suite (plan §6 Gate A).
#
#   ./scripts/run_gate_a.sh [slt_dir]      # default: <repo>/test/sql
#
# Gate A requires 100% of `test/sql/**/*.slt`; this script prints the current
# baseline. It exits non-zero while the baseline is short of 100% so it can be
# used as a gate once the engine is complete.
set -uo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../.." && pwd)"
slt="${1:-$root/test/sql}"

cd "$here/.."
out="$(cargo run -q --release -p infinity-cli -- conformance "$slt" 2>&1)"
echo "$out"

python3 - "$out" <<'PY'
import re, sys
text = sys.argv[1]
def frac(label):
    m = re.search(rf"{label}\s*:\s*(\d+)/(\d+)", text)
    return (int(m.group(1)), int(m.group(2))) if m else (0, 0)
ok_p, ok_t = frac("statement ok")
q_p, q_t = frac("query")
print(f"Gate A: statement ok {ok_p}/{ok_t}, query {q_p}/{q_t}")
sys.exit(0 if ok_p == ok_t and q_t > 0 and q_p == q_t else 1)
PY
