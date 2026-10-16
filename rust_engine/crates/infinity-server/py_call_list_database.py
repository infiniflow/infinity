#!/usr/bin/env python3
"""SDK→server `ListDatabase` round-trip. Run after `cargo build`:
    python3 py_call_list_database.py
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
    proto.writeMessageBegin("ListDatabase", TMessageType.CALL, 1)
    args = S.ListDatabase_args()
    args.request = T.ListDatabaseRequest(session_id=1)
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-list-database")
    proc = subprocess.run([binary], input=encode(), capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    _n, _mt, _sq = proto.readMessageBegin()
    res = S.ListDatabase_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    print(f"error_code={r.error_code} db_names={r.db_names}")
    ok = r.error_code == 0 and list(r.db_names) == ["default_db"]
    print("PY_LIST_DATABASE_OK" if ok else "PY_LIST_DATABASE_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
