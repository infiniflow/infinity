#!/usr/bin/env python3
"""Decodes a Thrift `Connect` CALL message (TBinaryProtocol) from stdin using the
authoritative generated Python codec, proving the Rust wire bytes are readable by
the SDK. Run:
    cargo run -q -p infinity-server --bin emit-connect-call | python3 py_decode.py
"""
import os
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
RPC_PARENT = os.path.join(REPO, "python", "infinity_sdk", "infinity", "remote_thrift")
sys.path.append(RPC_PARENT)

from thrift.transport import TTransport  # noqa: E402
from thrift.protocol import TBinaryProtocol  # noqa: E402
from infinity_thrift_rpc import InfinityService as S  # noqa: E402


def main():
    data = sys.stdin.buffer.read()
    proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(data))
    name, mtype, seqid = proto.readMessageBegin()
    args = S.Connect_args()
    args.read(proto)
    proto.readMessageEnd()
    cv = args.request.client_version
    print(f"method={name} type={mtype} seq={seqid} client_version={cv}")
    expected = 37
    if name == "Connect" and cv == expected:
        print("PY_DECODE_OK")
        sys.exit(0)
    print(f"PY_DECODE_FAIL (expected Connect/{expected})")
    sys.exit(1)


if __name__ == "__main__":
    main()
