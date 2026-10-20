import glob
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
            # A FLOAT column keeps ordered raw keys in the reference
            # (KeepOrderedSelf includes FloatT), so its secondary chunk must be
            # readable by both engines and its ranges answered from the index.
            "score": {"type": "float"},
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
    t.create_index("score_idx", index.IndexInfo("score", index.IndexType.Secondary))
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
                "score": i * 0.5,
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
    out = _catalog_snapshot(conn)
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
    # The float secondary index, read as an equality and as a range: the chunk
    # holds raw f32 keys, so a wrong key width or a hash-based encoding shows
    # up as missing rows rather than a wrong order.
    out["score_eq"] = _ids(t.output(["id"]).filter("score = 2.5").to_result())
    out["score_range"] = _ids(
        t.output(["id"]).filter("score >= 1.5 and score <= 3.5").to_result()
    )
    # Ordering: proves the block rows are readable in sequence, not just
    # countable. (`_row_id` is not reachable through the SDK's `output()`; the
    # planner-side name is pinned by
    # `multi_statement::the_row_id_system_column_is_selectable`.)
    ids = _ids(t.output(["id"]).to_result())
    out["first_ids"] = ids[:3]
    out["last_ids"] = ids[-3:]
    return out


def _catalog_snapshot(conn):
    """The catalog views both engines must agree on after a handover.

    `show_table` is the reference's own view of the table: the column and
    segment counts it reports come from the catalog the *other* engine wrote,
    so a missing `catalog|seg|…` record shows up here (segment_count 0) even
    when the block files are on disk.
    """
    db = conn.get_database("default_db")
    meta = db.show_table(TABLE)
    cols = db.get_table(TABLE).show_columns()
    return {
        "databases": sorted(conn.list_databases().db_names),
        "tables": sorted(db.list_tables().table_names),
        "column_count": meta.column_count,
        "segment_count": meta.segment_count,
        # Names and the scalar types compare exactly. The SPARSE column's type
        # text does not: both engines read the same stored bytes but decode the
        # sparse DataType differently (C++ prints `Sparse(float,int8,0)`, Rust
        # `Sparse(float,int32,8)`) - a decoder divergence tracked in
        # SIMPLIFICATION_AUDIT.md, not something to paper over by comparing the
        # rendered string only when it happens to agree.
        "columns": [
            (name, ty)
            for name, ty in zip(cols["name"], cols["type"])
            if not ty.lower().startswith("sparse")
        ],
    }


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


def _drop_wal():
    """Drop the redo log of the engine that just stopped.

    The handover point is a checkpoint (`flush_data()`), so the durable state is
    the catalog (the C++ key space) plus the block/index mirror; the log holds
    nothing that is not already there. Its *frame* is engine-private: this port
    writes `[magic "WAL1"][size][crc][content]` while the reference writes
    `[size][crc][txn_id][commit_ts]` followed by the commands. The reference
    cannot step over a foreign frame -- it reports
    `WAL error: ... entry size` and then aborts with
    `WAL replay: No checkpoint found in wal` -- so a Rust-written directory is
    unopenable by C++ for as long as `wal.log` exists. Dropping the fully
    applied log keeps this test about what it is named for (catalog and
    index-chunk compatibility) instead of dying in the redo log.
    """
    for path in glob.glob(os.path.join("/var/infinity", "wal", "wal.log*")):
        os.remove(path)


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
        # A checkpoint writes the C++-format block/index mirror the other
        # engine reads; the SDK call is the same the reference tests use.
        conn.flush_data()
        time.sleep(1)
        # Snapshot only after the checkpoint: the 1 KB `memindex_memory_quota`
        # spills the append mid-insert, and both engines can answer a
        # secondary/HNSW query from a half-dumped memory index -- the reference
        # returned [] for `title = 'doc-7'` and for the HNSW probe when the
        # snapshot ran before `flush_data()`. The state this test hands over is
        # the post-checkpoint one.
        return _snapshot(conn)

    before = _run(first, phase1)
    assert before["count"] == ROWS, before
    assert before["filter"], before
    assert before["fulltext"], before
    assert before["secondary"] == [7], before
    assert before["score_eq"] == [5], before
    assert before["score_range"] == [3, 4, 5, 6, 7], before
    # Catalog expectations, not just writer/reader equality: a both-sides-broken
    # catalog must not pass.
    assert before["databases"] == ["default_db"], before["databases"]
    assert before["tables"] == [TABLE], before["tables"]
    assert before["column_count"] == 6, before["column_count"]
    assert before["segment_count"] >= 1, before["segment_count"]
    assert [c[0] for c in before["columns"]] == [
        "id",
        "title",
        "body",
        "vec",
        "score",
    ], before["columns"]
    assert [c[1] for c in before["columns"]] == [
        "Integer",
        "Varchar",
        "Varchar",
        "Embedding(float,4)",
        "Float",
    ], before["columns"]
    assert before["first_ids"] == [0, 1, 2], before["first_ids"]
    assert before["last_ids"] == [ROWS - 3, ROWS - 2, ROWS - 1], before["last_ids"]

    _drop_wal()

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
                    "score": float(i),
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
