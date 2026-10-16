#!/usr/bin/env python3
"""SDK→engine→SDK `ShowTable` round-trip. Run after `cargo build`:
    python3 py_call_show_table.py
"""
import os
import subprocess
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.append(os.path.join(REPO, "python", "infinity_sdk", "infinity", "remote_thrift"))

from thrift.transport import TTransport  # noqa: E402
from thrift.protocol import TBinaryProtocol  # noqa: E402
from thrift.Thrift import TMessageType  # noqa: E402
from infinity_thrift_rpc import InfinityService as S  # noqa: E402
from infinity_thrift_rpc import ttypes as T  # noqa: E402


def encode():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("ShowTable", TMessageType.CALL, 1)
    args = S.ShowTable_args()
    args.request = T.ShowTableRequest(db_name="default_db", table_name="t", session_id=1)
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-show-table")
    proc = subprocess.run([binary], input=encode(), capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    _n, _mt, _sq = proto.readMessageBegin()
    res = S.ShowTable_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    print(f"db={r.database_name} table={r.table_name} cols={r.column_count} rows={r.row_count}")
    ok = r.error_code == 0 and r.column_count == 2 and r.row_count == 2
    print("PY_SHOW_TABLE_OK" if ok else "PY_SHOW_TABLE_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
