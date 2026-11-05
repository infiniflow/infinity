#!/usr/bin/env python3
"""End-to-end SDK→engine round-trip: build a Thrift `CreateTable` CALL with the
authoritative Python codec, run it through the Rust `handle-create-table` binary,
and decode the `CommonResponse` REPLY. Run (after `cargo build`):
    python3 py_call_create_table.py
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


def encode_create_table():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("CreateTable", TMessageType.CALL, 1)

    req = T.CreateTableRequest()
    req.db_name = "default_db"
    req.table_name = "t"
    cd0 = T.ColumnDef(id=0, name="id", data_type=T.DataType(logic_type=5))
    cd1 = T.ColumnDef(id=1, name="name", data_type=T.DataType(logic_type=2))
    req.column_defs = [cd0, cd1]
    req.session_id = 1

    args = S.CreateTable_args()
    args.request = req
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-create-table")
    call = encode_create_table()
    proc = subprocess.run([binary], input=call, capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)

    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    name, _mtype, _seq = proto.readMessageBegin()
    res = S.CreateTable_result()
    res.read(proto)
    proto.readMessageEnd()
    ok = name == "CreateTable" and res.success is not None and res.success.error_code == 0
    if res.success is not None:
        print(f"reply method={name} error_code={res.success.error_code} msg={res.success.error_msg}")
    print("PY_CREATE_TABLE_OK" if ok else "PY_CREATE_TABLE_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
