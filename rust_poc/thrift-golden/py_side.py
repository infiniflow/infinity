#!/usr/bin/env python3
"""PoC D (Python side): authoritative generated codec from the repo.

Encodes CommonResponse / ColumnField with the SDK's generated `ttypes`, writes
`py_*.bin`, and decodes `rust_*.bin` written by the Rust `thrift` crate.

Run after the Rust program has produced rust_*.bin:
    python3 py_side.py <workdir>
"""
import json
import os
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
RPC = os.path.join(REPO, "python", "infinity_sdk", "infinity", "remote_thrift", "infinity_thrift_rpc")
sys.path.insert(0, RPC)

from thrift.transport import TTransport  # noqa: E402
from thrift.protocol import TBinaryProtocol  # noqa: E402
import ttypes  # noqa: E402


def encode(obj):
    buf = TTransport.TMemoryBuffer()
    obj.write(TBinaryProtocol.TBinaryProtocol(buf))
    return buf.getvalue()


def decode(cls, data):
    buf = TTransport.TMemoryBuffer(data)
    obj = cls()
    obj.read(TBinaryProtocol.TBinaryProtocol(buf))
    return obj


def dump(o):
    return json.dumps(o, separators=(",", ":"))


def main():
    workdir = sys.argv[1] if len(sys.argv) > 1 else "/tmp/infinity-thrift-golden"
    os.makedirs(workdir, exist_ok=True)

    # --- encode with the authoritative codec ---
    cr = ttypes.CommonResponse(error_code=0, error_msg="ok", session_id=42)
    cr_bytes = encode(cr)
    with open(os.path.join(workdir, "py_common_response.bin"), "wb") as f:
        f.write(cr_bytes)

    vec0 = bytearray()
    for v in (7, 8):
        vec0 += int(v).to_bytes(4, "little", signed=True)
    cf = ttypes.ColumnField(column_type=1, column_vectors=[bytes(vec0)], column_name="score", bitmasks=[True, False])
    cf_bytes = encode(cf)
    with open(os.path.join(workdir, "py_column_field.bin"), "wb") as f:
        f.write(cf_bytes)

    print(f"python encoded: common_response={len(cr_bytes)} bytes, column_field={len(cf_bytes)} bytes")

    # --- decode Rust-produced bytes ---
    p = os.path.join(workdir, "rust_common_response.bin")
    if os.path.exists(p):
        o = decode(ttypes.CommonResponse, open(p, "rb").read())
        print("PY_DECODED py_common_response.bin " + dump(
            {"error_code": o.error_code, "error_msg": o.error_msg, "session_id": o.session_id}
        ))
    p = os.path.join(workdir, "rust_column_field.bin")
    if os.path.exists(p):
        o = decode(ttypes.ColumnField, open(p, "rb").read())
        ints = []
        for chunk in o.column_vectors:
            for i in range(0, len(chunk), 4):
                ints.append(int.from_bytes(chunk[i:i + 4], "little", signed=True))
        print("PY_DECODED py_column_field.bin " + dump(
            {"column_type": o.column_type, "column_name": o.column_name, "ints": ints,
             "bitmasks": [bool(b) for b in o.bitmasks]}
        ))


if __name__ == "__main__":
    main()
