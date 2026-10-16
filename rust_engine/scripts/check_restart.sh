#!/usr/bin/env bash
# Gate C evidence: does committed data survive a server restart when driven by
# the *unmodified* official Python SDK?
#
#   1. start `infinity serve <dir> <addr>`
#   2. over the wire: CREATE TABLE / INSERT
#   3. kill -9 the server (no clean shutdown)
#   4. start it again on the same directory
#   5. over the wire: read the rows back
#
# Usage:
#   scripts/check_restart.sh [data_dir] [addr] [python] [sdk_pythonpath]
# Defaults: /tmp/infinity_restart_check, 127.0.0.1:23818, python3,
#           ../python/infinity_sdk
set -uo pipefail

DATA_DIR="${1:-/tmp/infinity_restart_check}"
ADDR="${2:-127.0.0.1:23818}"
PY="${3:-python3}"
SDK="${4:-$(cd "$(dirname "$0")/../.." && pwd)/python/infinity_sdk}"
BIN="$(cd "$(dirname "$0")/.." && pwd)/target/release/infinity"

PORT="${ADDR##*:}"
# The SDK itself must be importable in the chosen interpreter.
if ! PYTHONPATH="$SDK" "$PY" -c "import infinity" >/dev/null 2>&1; then
    echo "SKIP: Python SDK (and its deps) not importable with '$PY'"
    echo "      e.g. pip install thrift numpy pandas polars pyarrow requests datrie hanziconv sqlglot pydantic"
    exit 0
fi

kill_server() {
    local pid
    pid=$(ss -ltnp 2>/dev/null | grep ":${PORT}" | sed -n 's/.*pid=\([0-9]*\).*/\1/p' | head -1)
    [ -n "$pid" ] && kill -9 "$pid" 2>/dev/null
    sleep 1
}

rm -rf "$DATA_DIR" && mkdir -p "$DATA_DIR"
kill_server
"$BIN" serve "$DATA_DIR" "$ADDR" >/tmp/restart_check_server.log 2>&1 &
sleep 3

wire() {
    PYTHONPATH="$SDK" "$PY" - "$PORT" "$1" <<'PY'
import sys
import infinity
from infinity.common import NetworkAddress

port, phase = int(sys.argv[1]), sys.argv[2]
inf = infinity.connect(NetworkAddress('127.0.0.1', port))
db = inf.get_database('default_db')
if phase == 'write':
    db.drop_table('restart_probe', infinity.common.ConflictType.Ignore)
    t = db.create_table(
        'restart_probe',
        {'c1': {'type': 'int'}, 'c2': {'type': 'varchar'}},
    )
    t.insert([{'c1': 1, 'c2': 'a'}, {'c1': 2, 'c2': 'b'}, {'c1': 3, 'c2': 'c'}])
    print('wrote 3 rows')
else:
    t = db.get_table('restart_probe')
    pl = t.output(['*']).to_pl()
    rows = sorted(zip(pl['c1'].to_list(), pl['c2'].to_list()))
    print('rows after restart:', rows)
    assert rows == [(1, 'a'), (2, 'b'), (3, 'c')], f'data lost: {rows}'
    print('PASS: committed rows survived the restart')
PY
}

wire write
kill_server                        # crash, not a clean shutdown
"$BIN" serve "$DATA_DIR" "$ADDR" >>/tmp/restart_check_server.log 2>&1 &
sleep 3
if wire read; then
    echo "== Gate C restart check: PASS =="
    rc=0
else
    echo "== Gate C restart check: FAIL =="
    rc=1
fi
kill_server
exit $rc
