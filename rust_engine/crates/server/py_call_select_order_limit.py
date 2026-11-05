#!/usr/bin/env python3
"""SDK→engine→SDK `Select` with ORDER BY + LIMIT. Run after `cargo build`:
    python3 py_call_select_order_limit.py
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


def col(name):
    return T.ParsedExpr(type=T.ParsedExprType(column_expr=T.ColumnExpr(column_name=[name])))


def i64(v):
    return T.ParsedExpr(type=T.ParsedExprType(constant_expr=T.ConstantExpr(literal_type=T.LiteralType.Int64, i64_value=v)))


def encode_select():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("Select", TMessageType.CALL, 1)
    req = T.SelectRequest(
        session_id=1,
        db_name="default_db",
        table_name="t",
        order_by_list=[T.OrderByExpr(expr=col("id"), asc=False)],
        limit_expr=i64(1),
    )
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
    _n, _mt, _sq = proto.readMessageBegin()
    res = S.Select_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    cols = {f.column_name: [bytes(v) for v in f.column_vectors] for f in r.column_fields}
    print(f"id={cols.get('id')} name={cols.get('name')}")
    ok = r.error_code == 0 and cols.get("id") == [b"2"] and cols.get("name") == [b"b"]
    print("PY_SELECT_ORDER_LIMIT_OK" if ok else "PY_SELECT_ORDER_LIMIT_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
