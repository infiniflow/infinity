#!/usr/bin/env python3
"""SDK→engine→SDK `Delete` round-trip. Run after `cargo build`:
    python3 py_call_delete.py
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
    proto.writeMessageBegin("Delete", TMessageType.CALL, 1)
    where = T.ParsedExpr(
        type=T.ParsedExprType(
            function_expr=T.FunctionExpr(
                function_name=">=",
                arguments=[
                    T.ParsedExpr(type=T.ParsedExprType(column_expr=T.ColumnExpr(column_name=["id"]))),
                    T.ParsedExpr(type=T.ParsedExprType(constant_expr=T.ConstantExpr(literal_type=T.LiteralType.Int64, i64_value=2))),
                ],
            )
        )
    )
    args = S.Delete_args()
    args.request = T.DeleteRequest(db_name="default_db", table_name="t", where_expr=where, session_id=1)
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-delete")
    proc = subprocess.run([binary], input=encode(), capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    _n, _mt, _sq = proto.readMessageBegin()
    res = S.Delete_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    print(f"error_code={r.error_code} deleted_rows={r.deleted_rows}")
    ok = r.error_code == 0 and r.deleted_rows == 2
    print("PY_DELETE_OK" if ok else "PY_DELETE_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
