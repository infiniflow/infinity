"""Byte-for-byte parity between the rust server's hand-written thrift codec
and the volo-thrift harness in tools/volo_wire.

Why this exists: the Rust rewrite may replace the hand-written blocking
Thrift server with volo-thrift.  Before that happens the two must emit the
*identical* unframed binary reply for the same SelectResponse, otherwise the
existing go/python clients (both use TBufferedTransport, i.e. unframed) would
see a different wire format.

How it works:
  * a real infinity-server is started on a scratch data dir;
  * each case creates its table through the python SDK and inserts fixed rows;
  * the same SelectRequest bytes are then replayed, once to infinity-server and
    once to the volo harness (which ignores the request and answers with canned
    values for the case), on fresh TCP connections;
  * the two replies must be equal byte for byte.

The volo harness lives next to this file and is skipped when it has not been
built yet.
"""

import contextlib
import os
import pathlib
import shutil
import socket
import subprocess
import time

import pytest
from infinity import NetworkAddress, connect
from infinity.common import ConflictType
from infinity.remote_thrift.infinity_thrift_rpc import InfinityService, ttypes
from thrift.protocol import TBinaryProtocol
from thrift.Thrift import TMessageType
from thrift.transport import TTransport

ROOT = pathlib.Path(__file__).resolve().parents[2]
RUST_BIN = ROOT / "rust_engine" / "target" / "debug" / "infinity"
VOLO_DIR = ROOT / "tools" / "volo_wire"
VOLO_BIN = VOLO_DIR / "target" / "debug" / "volo_wire"
SCRATCH = ROOT / ".scratch" / "volo_wire_parity"

# One entry per canned response in tools/volo_wire/src/svc.rs.
CASES = [
    {
        "name": "int_varchar",
        "table": "t_sel",
        "schema": {"id": {"type": "int"}, "name": {"type": "varchar"}},
        "rows": [
            {"id": 1, "name": "abc"},
            {"id": 2, "name": "de"},
            {"id": 3, "name": None},
        ],
    },
    {
        "name": "int_varchar_no_null",
        "table": "t_sel_nn",
        "schema": {"id": {"type": "int"}, "name": {"type": "varchar"}},
        "rows": [
            {"id": 1, "name": "abc"},
            {"id": 2, "name": "de"},
            {"id": 3, "name": "fg"},
        ],
    },
    {
        "name": "float_embedding",
        "table": "t_num",
        "schema": {
            "id": {"type": "int"},
            "f": {"type": "float"},
            "v": {"type": "vector,4,float"},
        },
        "rows": [
            {"id": 1, "f": 1.5, "v": [0.25, 0.5, -0.75, 1.0]},
            {"id": 2, "f": -0.125, "v": [0.0, 0.1, 0.2, 0.3]},
            {"id": 3, "f": None, "v": None},
        ],
    },
    {
        "name": "double_timestamp",
        "table": "t_dt",
        "schema": {
            "id": {"type": "int"},
            "d": {"type": "double"},
            "ts": {"type": "timestamp"},
        },
        "rows": [
            {"id": 1, "d": 1.5, "ts": "2024-01-02 10:30:00"},
            {"id": 2, "d": -0.1, "ts": "2024-01-01 00:00:01"},
            {"id": 3, "d": None, "ts": None},
        ],
    },
    {
        "name": "date_time_json",
        "table": "t_temporal",
        "schema": {
            "id": {"type": "int"},
            "d": {"type": "date"},
            "tm": {"type": "time"},
            "dtm": {"type": "datetime"},
            "js": {"type": "json"},
        },
        "rows": [
            {
                "id": 1,
                "d": "2024-01-02",
                "tm": "10:30:00",
                "dtm": "2024-01-02 10:30:00",
                "js": '{"a":1}',
            },
            {
                "id": 2,
                "d": "1970-01-01",
                "tm": "00:00:01",
                "dtm": "2024-01-01 00:00:01",
                "js": "[1,2]",
            },
            {"id": 3, "d": None, "tm": None, "dtm": None, "js": None},
        ],
    },
    {
        "name": "array_int",
        "table": "t_array",
        "schema": {"id": {"type": "int"}, "a": {"type": "array,int"}},
        "rows": [
            {"id": 1, "a": [1, 2, 3]},
            {"id": 2, "a": [4]},
            {"id": 3, "a": None},
        ],
    },
    {
        "name": "sparse_float",
        "table": "t_sparse",
        "schema": {"id": {"type": "int"}, "s": {"type": "sparse,1000,float,int32"}},
        "rows": [
            {"id": 1, "s": {0: 2.5, 3: 1.5}},
            {"id": 2, "s": {7: -1.0}},
            {"id": 3, "s": None},
        ],
    },
    {
        "name": "tensor_float",
        "table": "t_tensor",
        "schema": {"id": {"type": "int"}, "tv": {"type": "tensor,4,float"}},
        "rows": [
            {"id": 1, "tv": [[1.0, 2.0, 3.0, 4.0]]},
            {"id": 2, "tv": [[0.5, 0.25, -0.5, -1.0]]},
            {"id": 3, "tv": None},
        ],
    },
    {
        "name": "multivector_float",
        "table": "t_mv",
        "schema": {"id": {"type": "int"}, "mv": {"type": "multivector,4,float"}},
        "rows": [
            {"id": 1, "mv": [[1.0, 2.0, 3.0, 4.0], [0.5, 0.5, 0.5, 0.5]]},
            {"id": 2, "mv": [[1.0, 0.0, 0.0, 0.0]]},
            {"id": 3, "mv": None},
        ],
    },
]


def _free_ports(n):
    """Reserve n distinct ports (all held open until the last one is picked)."""
    socks = [socket.socket() for _ in range(n)]
    try:
        for s in socks:
            s.bind(("127.0.0.1", 0))
        return [s.getsockname()[1] for s in socks]
    finally:
        for s in socks:
            s.close()


def _wait_port(port, timeout=60.0):
    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.5):
                return
        except OSError:
            time.sleep(0.05)
    raise AssertionError(f"port {port} did not open in {timeout}s")


def _write_conf(case_dir, port, http_port, pg_port):
    case_dir.mkdir(parents=True, exist_ok=True)
    conf = case_dir / "infinity_conf.toml"
    conf.write_text(
        "\n".join(
            [
                "[general]",
                'version = "0.7.5"',
                'time_zone = "utc-8"',
                "",
                "[network]",
                'server_address = "127.0.0.1"',
                f"client_port = {port}",
                f"http_port = {http_port}",
                f"postgres_port = {pg_port}",
                "",
                "[log]",
                'log_filename = "infinity.log"',
                f'log_dir = "{case_dir / "log"}"',
                "log_to_stdout = true",
                'log_level = "info"',
                "",
                "[storage]",
                f'data_dir = "{case_dir / "data"}"',
                f'persistence_dir = "{case_dir / "persistence"}"',
                f'snapshot_dir = "{case_dir / "snapshot"}"',
                'compact_interval = "0s"',
                "",
                "[buffer]",
                f'temp_dir = "{case_dir / "tmp"}"',
                "",
                "[wal]",
                f'wal_dir = "{case_dir / "wal"}"',
            ]
        )
        + "\n"
    )
    return conf


@pytest.fixture(scope="module")
def rust_server():
    if not RUST_BIN.exists():
        pytest.skip(
            f"{RUST_BIN} missing; build it with: cargo build -p cli --bin infinity"
        )

    SCRATCH.mkdir(parents=True, exist_ok=True)
    case_dir = SCRATCH / "rust"
    if case_dir.exists():
        shutil.rmtree(case_dir)
    port, http_port, pg_port = _free_ports(3)
    conf = _write_conf(case_dir, port, http_port, pg_port)
    log = open(case_dir / "stdout.log", "w")
    proc = subprocess.Popen(
        [str(RUST_BIN), "-f", str(conf)], stdout=log, stderr=subprocess.STDOUT
    )
    try:
        _wait_port(port)
        yield port
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=20)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()
        log.close()


@contextlib.contextmanager
def _volo_server(case):
    if not VOLO_BIN.exists():
        pytest.skip(
            f"{VOLO_BIN} missing; build it with: cargo build (in tools/volo_wire)"
        )

    case_dir = SCRATCH / "volo" / case["name"]
    if case_dir.exists():
        shutil.rmtree(case_dir)
    case_dir.mkdir(parents=True)
    (port,) = _free_ports(1)
    env = dict(
        os.environ, VOLO_WIRE_ADDR=f"127.0.0.1:{port}", VOLO_WIRE_CASE=case["name"]
    )
    log = open(case_dir / "stdout.log", "w")
    proc = subprocess.Popen(
        [str(VOLO_BIN)], env=env, stdout=log, stderr=subprocess.STDOUT
    )
    try:
        _wait_port(port)
        yield port
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=20)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()
        log.close()


class _Wire:
    """Minimal unframed-ish thrift client over a plain socket.

    No transport buffering: the exact bytes written and read are returned so
    the test can compare them.
    """

    def __init__(self, port):
        self.sock = socket.create_connection(("127.0.0.1", port), timeout=10)
        self.seq = 0

    def close(self):
        self.sock.close()

    def _send(self, method, args):
        self.seq += 1
        buf = TTransport.TMemoryBuffer()
        proto = TBinaryProtocol.TBinaryProtocol(buf)
        proto.writeMessageBegin(method, TMessageType.CALL, self.seq)
        args.write(proto)
        proto.writeMessageEnd()
        raw = buf.getvalue()
        self.sock.sendall(raw)
        return raw

    def call(self, method, args, result_cls):
        request = self._send(method, args)
        reply = self._recv()
        proto = TBinaryProtocol.TBinaryProtocol(TTransport.TMemoryBuffer(reply))
        name, mtype, seq = proto.readMessageBegin()
        assert mtype != TMessageType.EXCEPTION, (name, mtype)
        result = result_cls()
        result.read(proto)
        proto.readMessageEnd()
        return request, reply, result.success

    def _recv(self):
        # Unframed, one reply per call: read until the peer goes quiet.
        self.sock.settimeout(10)
        chunks = []
        first = self.sock.recv(65536)
        assert first, "connection closed before reply"
        chunks.append(first)
        self.sock.settimeout(0.3)
        while True:
            try:
                chunk = self.sock.recv(65536)
            except TimeoutError:
                break
            if not chunk:
                break
            chunks.append(chunk)
        return b"".join(chunks)


def _connect(wire):
    args = InfinityService.Connect_args()
    args.request = ttypes.ConnectRequest(client_version=1)
    _, _, res = wire.call("Connect", args, InfinityService.Connect_result)
    return res.session_id


def _select_bytes(port, table):
    wire = _Wire(port)
    try:
        session_id = _connect(wire)
        req = ttypes.SelectRequest(
            session_id=session_id,
            db_name="default_db",
            table_name=table,
            select_list=[
                ttypes.ParsedExpr(
                    type=ttypes.ParsedExprType(
                        column_expr=ttypes.ColumnExpr(star=True, column_name=[])
                    )
                )
            ],
            highlight_list=[],
            group_by_list=[],
            order_by_list=[],
        )
        args = InfinityService.Select_args()
        args.request = req
        request, reply, response = wire.call(
            "Select", args, InfinityService.Select_result
        )
        return request, reply, response
    finally:
        wire.close()


@pytest.mark.parametrize("case", CASES, ids=[c["name"] for c in CASES])
def test_select_reply_matches_volo(case, rust_server):
    db = connect(NetworkAddress("127.0.0.1", rust_server))
    try:
        database = db.get_database("default_db")
        database.drop_table(case["table"], ConflictType.Ignore)
        table = database.create_table(case["table"], case["schema"], ConflictType.Error)
        table.insert(case["rows"])
    finally:
        db.disconnect()

    request, rust_reply, rust_resp = _select_bytes(rust_server, case["table"])
    with _volo_server(case) as volo_port:
        _, volo_reply, volo_resp = _select_bytes(volo_port, case["table"])

    assert rust_resp is not None and volo_resp is not None
    assert rust_reply == volo_reply, (
        f"{case['name']}: reply differs (rust {len(rust_reply)} B, volo {len(volo_reply)} B)\n"
        f"request={request.hex()}\nrust={rust_reply.hex()}\nvolo={volo_reply.hex()}"
    )
