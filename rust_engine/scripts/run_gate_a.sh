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
    m = re.search(rf"^{re.escape(label)}\s*:\s*(\d+)/(\d+)\s*$", text, re.M)
    return (int(m.group(1)), int(m.group(2))) if m else None
counters = ["statement ok", "statement error", "query"]
missing = [c for c in counters if frac(c) is None]
if missing:
    # A missing counter must fail the gate: defaulting it to 0/0 would make
    # the comparison below vacuously true.
    print(f"Gate A: missing counter line(s): {', '.join(missing)}")
    sys.exit(1)
ok_p, ok_t = frac("statement ok")
e_p, e_t = frac("statement error")
q_p, q_t = frac("query")
print(f"Gate A: statement ok {ok_p}/{ok_t}, statement error {e_p}/{e_t}, query {q_p}/{q_t}")
sys.exit(
    0
    if ok_t > 0
    and e_t > 0
    and q_t > 0
    and ok_p == ok_t
    and e_p == e_t
    and q_p == q_t
    else 1
)
PY
