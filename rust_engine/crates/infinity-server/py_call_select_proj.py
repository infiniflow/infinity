#!/usr/bin/env python3
"""SDK→engine→SDK `Select` with projection: build a `Select` CALL selecting only
`name`, run it through `handle-select`, and decode the columnar result. Run after
`cargo build`:
    python3 py_call_select_proj.py
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


def encode_select():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("Select", TMessageType.CALL, 1)
    proj = T.ParsedExpr(type=T.ParsedExprType(column_expr=T.ColumnExpr(column_name=["name"])))
    req = T.SelectRequest(session_id=1, db_name="default_db", table_name="t", select_list=[proj])
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
    _name, _mt, _sq = proto.readMessageBegin()
    res = S.Select_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    defs = [d.name for d in r.column_defs]
    cols = {f.column_name: [bytes(v) for v in f.column_vectors] for f in r.column_fields}
    print(f"defs={defs} name={cols.get('name')}")
    ok = r.error_code == 0 and defs == ["name"] and cols.get("name") == [b"a", b"b"]
    print("PY_SELECT_PROJ_OK" if ok else "PY_SELECT_PROJ_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
