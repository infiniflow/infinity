#!/usr/bin/env python3
"""Gate E, per index type: build and query latency for each index kind.

`bench_sdk_driver.py` measures the aggregate insert/select workload. This driver
measures the part that workload hides: how long each index takes to build on an
existing table, and how long a typical query against that index takes. It runs
against whichever server is listening on `URI`, so the same script produces the
C++ golden and the Rust candidate.

Output (one line per measurement, `INDEX <kind> <phase> <seconds>`):

    INDEX fulltext build 1.234
    INDEX fulltext query 0.012
    ...

Usage: bench_index_types.py [host] [port] [rows] [queries]
"""

import sys
import time

import infinity
from infinity.common import ConflictType, NetworkAddress, SparseVector
from infinity import index


def connect_deadline(host, port, seconds=180):
    deadline = time.time() + seconds
    while True:
        try:
            return infinity.connect(NetworkAddress(host, port))
        except Exception:
            if time.time() > deadline:
                raise
            time.sleep(1)


def batch_insert(t, rows, batch=1000):
    for start in range(0, rows, batch):
        t.insert([row(i) for i in range(start, min(start + batch, rows))])


def row(i):
    return {
        "id": i,
        "title": f"doc-{i}",
        "body": f"alpha beta gamma doc{i}" if i % 2 == 0 else f"delta epsilon doc{i}",
        "vec": [float(i % 97) / 97.0, float(i % 13) / 13.0, 0.25, 0.75],
        "sp": SparseVector(indices=[0, i % 7 + 1], values=[0.5, 1.0]),
    }


INDEXES = [
    ("fulltext", "body", index.IndexInfo("body", index.IndexType.FullText)),
    (
        "hnsw",
        "vec",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "200", "metric": "l2"}
        ),
    ),
    ("secondary", "title", index.IndexInfo("title", index.IndexType.Secondary)),
    ("sparse", "sp", index.IndexInfo("sp", index.IndexType.BMP)),
]


def query(t, kind, i):
    if kind == "fulltext":
        return t.output(["id"]).match_text("body", "alpha", 10).to_result()
    if kind == "hnsw":
        return t.output(["id"]).match_dense("vec", [0.5, 0.5, 0.25, 0.75], "float", "l2", 10).to_result()
    if kind == "secondary":
        return t.output(["id"]).filter(f"title = 'doc-{i}'").to_result()
    if kind == "sparse":
        return t.output(["id"]).match_sparse(
            "sp", SparseVector(indices=[0, 1], values=[0.5, 1.0]), "ip", 10
        ).to_result()
    raise AssertionError(kind)


def main() -> int:
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 23917
    rows = int(sys.argv[3]) if len(sys.argv) > 3 else 20000
    queries = int(sys.argv[4]) if len(sys.argv) > 4 else 50

    obj = connect_deadline(host, port)
    db = obj.get_database("default_db")
    db.drop_table("bench_idx", ConflictType.Ignore)
    t = db.create_table(
        "bench_idx",
        {
            "id": {"type": "int"},
            "title": {"type": "varchar"},
            "body": {"type": "varchar"},
            "vec": {"type": "vector,4,float"},
            "sp": {"type": "sparse,8,float,int"},
        },
        ConflictType.Error,
    )
    batch_insert(t, rows)
    obj.flush_data()
    time.sleep(1)
    print(f"ROWS {rows}", flush=True)

    for kind, _column, info in INDEXES:
        start = time.perf_counter()
        t.create_index(f"idx_{kind}", info)
        build = time.perf_counter() - start
        print(f"INDEX {kind} build {build:.4f}", flush=True)

        # Warm the first query (plan/cache effects), then time the steady state.
        query(t, kind, 0)
        start = time.perf_counter()
        for i in range(queries):
            query(t, kind, i)
        elapsed = time.perf_counter() - start
        print(f"INDEX {kind} query {elapsed / queries:.6f}", flush=True)
        print(f"INDEX {kind} query_total {elapsed:.4f}", flush=True)

    db.drop_table("bench_idx", ConflictType.Error)
    obj.disconnect()
    return 0


if __name__ == "__main__":
    sys.exit(main())
