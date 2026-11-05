#!/usr/bin/env python3
"""Decodes a `list<ColumnField>` (TBinaryProtocol) from stdin using the
authoritative generated Python codec. Run:
    cargo run -q -p infinity-server --bin emit-select-columns | python3 py_decode_columns.py
"""
import os
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.append(os.path.join(REPO, "python", "infinity_sdk", "infinity", "remote_thrift"))

from thrift.transport import TTransport  # noqa: E402
from thrift.protocol import TBinaryProtocol  # noqa: E402
from infinity_thrift_rpc import ttypes  # noqa: E402


def main():
    data = sys.stdin.buffer.read()
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(data))
    _etype, size = proto.readListBegin()
    fields = []
    for _ in range(size):
        cf = ttypes.ColumnField()
        cf.read(proto)
        fields.append(cf)
    proto.readListEnd()
    for cf in fields:
        print(f"name={cf.column_name} type={cf.column_type} vecs={len(cf.column_vectors)} masks={cf.bitmasks}")
    ok = len(fields) == 2 and fields[0].column_name == "id" and fields[1].bitmasks == [True, False]
    print("PY_COLUMNS_OK" if ok else "PY_COLUMNS_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
