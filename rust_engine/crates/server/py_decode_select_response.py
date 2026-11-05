#!/usr/bin/env python3
"""Decodes a `Select` REPLY (`SelectResponse`) from stdin using the authoritative
generated Python codec. Run:
    cargo run -q -p infinity-server --bin emit-select-response | python3 py_decode_select_response.py
"""
import os
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.append(os.path.join(REPO, "python", "infinity_sdk", "infinity", "remote_thrift"))

from thrift.transport import TTransport  # noqa: E402
from thrift.protocol import TBinaryProtocol  # noqa: E402
from infinity_thrift_rpc import InfinityService as S  # noqa: E402


def main():
    data = sys.stdin.buffer.read()
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(data))
    name, _mtype, _seq = proto.readMessageBegin()
    res = S.Select_result()
    res.read(proto)
    proto.readMessageEnd()
    r = res.success
    defs = [d.name for d in r.column_defs]
    fields = [(f.column_name, list(f.bitmasks)) for f in r.column_fields]
    print(f"method={name} err={r.error_code} defs={defs} fields={fields}")
    ok = (
        name == "Select"
        and r.error_code == 0
        and defs == ["id", "name"]
        and [f[0] for f in fields] == ["id", "name"]
        and fields[1][1] == [True, False]
    )
    print("PY_SELECT_OK" if ok else "PY_SELECT_FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
