#!/usr/bin/env python3
"""SDK→engine→SDK `Insert` round-trip: build an `Insert` CALL with the
authoritative Python codec (ConstantExpr literals), run it through the Rust
`handle-insert` binary, and decode the `CommonResponse`. Run after `cargo build`:
    python3 py_call_insert.py
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


def lit_int(v):
    return T.ParsedExpr(type=T.ParsedExprType(constant_expr=T.ConstantExpr(literal_type=T.LiteralType.Int64, i64_value=v)))


def lit_str(v):
    return T.ParsedExpr(type=T.ParsedExprType(constant_expr=T.ConstantExpr(literal_type=T.LiteralType.String, str_value=v)))


def encode_insert():
    buf = TTransport.TMemoryBuffer()
    proto = TBinaryProtocol.TBinaryProtocol(buf)
    proto.writeMessageBegin("Insert", TMessageType.CALL, 1)
    field = T.Field(column_names=["id", "name"], parse_exprs=[lit_int(7), lit_str("z")])
    req = T.InsertRequest(db_name="default_db", table_name="t", fields=[field], session_id=1)
    args = S.Insert_args()
    args.request = req
    args.write(proto)
    proto.writeMessageEnd()
    return buf.getvalue()


def main():
    binary = os.path.join(REPO, "rust_engine", "target", "debug", "handle-insert")
    proc = subprocess.run([binary], input=encode_insert(), capture_output=True)
    if proc.returncode != 0:
        print("binary failed:", proc.stderr.decode(errors="replace"))
        sys.exit(2)
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(proc.stdout))
    name, _mtype, _seq = proto.readMessageBegin()
    res = S.Insert_result()
    res.read(proto)
    proto.readMessageEnd()
    ok = name == "Insert" and res.success is not None and res.success.error_code == 0
    if res.success is not None:
        print(f"reply method={name} error_code={res.success.error_code} msg={res.success.error_msg}")
    print("PY_INSERT_OK" if ok else "PY_INSERT_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
