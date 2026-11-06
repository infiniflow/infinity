#!/usr/bin/env python3
"""SDK→engine→SDK `Select` round-trip: build a `Select` CALL (empty select_list
=> `SELECT *`) with the authoritative Python codec, run it through the Rust
`handle-select` binary, and decode the columnar `SelectResponse`. Run after
`cargo build`:
    python3 py_call_select.py
"""
import os
import subprocess
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.append(os.path.join(REPO, "python", "infinity_sdk", "infinity", "remote_thrift"))

from infinity_thrift_rpc import InfinityService as S
from infinity_thrift_rpc import ttypes as T
from thrift.protocol import TBinaryProtocol
from thrift.Thrift import TMessageType
from thrift.transport import TTransport


def encode_select():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("Select", TMessageType.CALL, 1)
    req = T.SelectRequest(session_id=1, db_name="default_db", table_name="t")
    args = S.Select_args()
    args.request = req
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-select")
    proc = subprocess.run([binary], input=encode_select(), capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)

    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    name, _mtype, _seq = proto.readMessageBegin()
    res = S.Select_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    defs = [d.name for d in r.column_defs]
    cols = {}
    for f in r.column_fields:
        cols[f.column_name] = [bytes(v) for v in f.column_vectors]
    print(f"method={name} err={r.error_code} defs={defs} id={cols.get('id')} name={cols.get('name')}")
    ok = (
        name == "Select"
        and r.error_code == 0
        and defs == ["id", "name"]
        and cols.get("id") == [b"1", b"2"]
        and cols.get("name") == [b"a", b"b"]
    )
    print("PY_SELECT_ROUNDTRIP_OK" if ok else "PY_SELECT_ROUNDTRIP_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
