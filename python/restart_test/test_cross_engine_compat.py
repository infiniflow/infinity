import os
import time

import pytest

import infinity
from infinity.common import ConflictType, NetworkAddress
from infinity import index
from infinity_runner import InfinityRunner

# Cross-engine compatibility (plan §3.1 disk-format compatibility + Gate C):
# one engine creates the table, several index types and a batch of rows, stops,
# and the *other* engine has to read the same data directory: the queries must
# return the same rows and new inserts must keep working.
CPP_BIN = os.environ.get("INFINITY_CPP_BIN", "cmake-build-reldeb/src/infinity")
RUST_BIN = os.environ.get("INFINITY_RUST_BIN", "rust_engine/scripts/infinity_rust_shim.sh")
CONFIG = "test/data/config/restart_test/test_memidx/1.toml"
URI = NetworkAddress("127.0.0.1", 23817)

TABLE = "test_cross_engine"
ROWS = 200


def _seed(conn):
    db = conn.get_database("default_db")
    db.drop_table(TABLE, ConflictType.Ignore)
    t = db.create_table(
        TABLE,
        {
            "id": {"type": "int"},
            "title": {"type": "varchar"},
            "body": {"type": "varchar"},
            "vec": {"type": "vector,4,float"},
            "sp": {"type": "sparse,8,float,int"},
        },
        ConflictType.Error,
    )
    t.create_index("ft", index.IndexInfo("body", index.IndexType.FullText))
    t.create_index(
        "vec_idx",
        index.IndexInfo(
            "vec",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "l2"},
        ),
    )
    t.create_index("sec", index.IndexInfo("title", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    rows = []
    for i in range(ROWS):
        body = f"alpha beta doc{i}" if i % 2 == 0 else f"gamma delta doc{i}"
        rows.append(
            {
                "id": i,
                "title": f"doc-{i}",
                "body": body,
                "vec": [float(i % 10) / 10.0, 0.1, 0.2, 0.3],
                "sp": {0: 0.5, i % 5 + 1: 1.0},
            }
        )
    t.insert(rows)
    return t


def _ids(result):
    data, _, _ = result
    key = next(k for k in data if k != "SCORE" and k != "DISTANCE")
    return sorted(data[key])


def _snapshot(conn):
    """Everything both engines must agree on, in a score-independent shape."""
    t = conn.get_database("default_db").get_table(TABLE)
    out = {}
    data, _, _ = t.output(["count(*)"]).to_result()
    out["count"] = data["count(star)"][0]
    out["filter"] = _ids(t.output(["id"]).filter("id < 5").to_result())
    out["fulltext"] = _ids(t.output(["id"]).match_text("body", "alpha", 10).to_result())
    out["hnsw"] = _ids(
        t.output(["id"])
        .match_dense("vec", [0.0, 0.1, 0.2, 0.3], "float", "l2", 5)
        .to_result()
    )
    out["sparse"] = _ids(
        t.output(["id"]).match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5).to_result()
    )
    out["secondary"] = _ids(t.output(["id"]).filter("title = 'doc-7'").to_result())
    return out


def _run(engine_path, body):
    runner = InfinityRunner(engine_path, CONFIG)
    runner.init(CONFIG)
    try:
        # `init()` only spawns the process; `connect()` is the harness's retry
        # loop (the engine answers Thrift a moment later).
        conn = runner.connect(URI)
        try:
            return body(conn)
        finally:
            conn.disconnect()
    finally:
        runner.uninit(kill=True)


@pytest.mark.parametrize(
    "first,second",
    [
        pytest.param(CPP_BIN, RUST_BIN, id="cpp-then-rust"),
        pytest.param(RUST_BIN, CPP_BIN, id="rust-then-cpp"),
    ],
)
def test_indices_and_rows_survive_the_other_engine(first, second):
    if not os.access(first, os.X_OK):
        pytest.skip(f"{first} is not executable")
    if not os.access(second, os.X_OK):
        pytest.skip(f"{second} is not executable")

    InfinityRunner(first, CONFIG).clear()

    def phase1(conn):
        _seed(conn)
        expected = _snapshot(conn)
        # A checkpoint writes the C++-format block/index mirror the other
        # engine reads; the SDK call is the same the reference tests use.
        conn.flush_data()
        time.sleep(1)
        return expected

    before = _run(first, phase1)
    assert before["count"] == ROWS, before
    assert before["filter"], before
    assert before["fulltext"], before
    assert before["secondary"] == [7], before

    def phase2(conn):
        after = _snapshot(conn)
        # Insert through the *second* engine and read it back through it.
        t = conn.get_database("default_db").get_table(TABLE)
        t.insert(
            [
                {
                    "id": ROWS + i,
                    "title": f"new-{i}",
                    "body": "alpha omega inserted",
                    "vec": [0.0, 0.1, 0.2, 0.3],
                    "sp": {0: 0.5, 1: 1.0},
                }
                for i in range(3)
            ]
        )
        conn.flush_data()
        time.sleep(1)
        grown, _, _ = t.output(["count(*)"]).to_result()
        fresh = _ids(t.output(["id"]).match_text("body", "omega", 5).to_result())
        return after, grown["count(star)"][0], fresh

    after, count, fresh = _run(second, phase2)
    assert after == before, f"{first} -> {second}: {after} != {before}"
    assert count == ROWS + 3, count
    assert sorted(fresh) == [ROWS, ROWS + 1, ROWS + 2], fresh
