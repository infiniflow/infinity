#!/usr/bin/env python3
"""SDK→engine→SDK `Update` round-trip. Run after `cargo build`:
    python3 py_call_update.py
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


def strv(v):
    return T.ParsedExpr(type=T.ParsedExprType(constant_expr=T.ConstantExpr(literal_type=T.LiteralType.String, str_value=v)))


def encode():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("Update", TMessageType.CALL, 1)
    where = T.ParsedExpr(
        type=T.ParsedExprType(function_expr=T.FunctionExpr(function_name=">=", arguments=[col("id"), i64(2)]))
    )
    upd = T.UpdateExpr(column_name="name", value=strv("X"))
    args = S.Update_args()
    args.request = T.UpdateRequest(
        db_name="default_db", table_name="t", where_expr=where, update_expr_array=[upd], session_id=1
    )
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-update")
    proc = subprocess.run([binary], input=encode(), capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    _n, _mt, _sq = proto.readMessageBegin()
    res = S.Update_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    print(f"error_code={r.error_code} msg={r.error_msg}")
    ok = r.error_code == 0
    print("PY_UPDATE_OK" if ok else "PY_UPDATE_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
