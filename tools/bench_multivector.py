#!/usr/bin/env python3
"""Gate E, multivector: build and query latency for each multi-vector index.

The per-index driver (`bench_index_types.py`) covers fulltext, hnsw, secondary
and sparse; a `multivector(float,dim)` column is a different animal (a row is
*several* vectors, scored by MaxSim) and none of those indexes touch it. This
driver measures the same two phases for the three indexes the engine offers a
multi-vector column:

    INDEX <kind> build <seconds>
    INDEX <kind> query <seconds>

Every engine gets the same rows, the same queries and the same statements, so
the C++ golden and the Rust candidate come from one driver.

Usage: bench_multivector.py [host] [port] [rows] [queries]
"""

import random
import sys
import time

import infinity
from infinity.common import ConflictType, NetworkAddress
from infinity import index

DIMS = 8
VECTORS_PER_ROW = 4


def connect_deadline(host, port, seconds=180):
    deadline = time.time() + seconds
    while True:
        try:
            return infinity.connect(NetworkAddress(host, port))
        except Exception:
            if time.time() > deadline:
                raise
            time.sleep(1)


def row(i, rng):
    """A row's vectors: a centre plus jitter, so near neighbours are real."""
    cx = (i % 97) / 97.0
    cy = (i % 13) / 13.0
    return {
        "id": i,
        "mv": [
            [cx + rng.uniform(-0.01, 0.01), cy + rng.uniform(-0.01, 0.01), 0.25, 0.75]
            + [(i % 7) * 0.01] * (DIMS - 4)
            for _ in range(VECTORS_PER_ROW)
        ],
    }


INDEXES = [
    ("emvb", index.IndexType.EMVB, {"pq_subspace_num": "4", "pq_subspace_bits": "8"}),
    ("plaid", index.IndexType.PLAID, {"nbits": "4"}),
    ("smve", index.IndexType.SMVE, {}),
]

QUERY = [
    [0.5 + j * 0.001, 0.5, 0.25, 0.75] + [0.02] * (DIMS - 4)
    for j in range(VECTORS_PER_ROW)
]


def main() -> int:
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 23917
    rows = int(sys.argv[3]) if len(sys.argv) > 3 else 20000
    queries = int(sys.argv[4]) if len(sys.argv) > 4 else 50

    rng = random.Random(7)
    batch = [row(i, rng) for i in range(rows)]

    obj = connect_deadline(host, port)
    db = obj.get_database("default_db")

    for kind, kind_type, opts in INDEXES:
        table = f"bench_mv_{kind}"
        db.drop_table(table, ConflictType.Ignore)
        t = db.create_table(
            table,
            {
                "id": {"type": "int"},
                # EMVB, PLAID and SMVE all validate against
                # `LogicalType::kTensor` (`index_{emvb,plaid,smve}_impl.cpp`), not
                # `kMultiVector`: a multi-vector column for these indexes is
                # declared `tensor(float, dim)` and holds `[[...], [...]]`.
                "mv": {"type": f"tensor,{DIMS},float"},
            },
            ConflictType.Error,
        )
        for start in range(0, rows, 1000):
            t.insert(batch[start : start + 1000])
        obj.flush_data()
        time.sleep(1)

        start = time.perf_counter()
        try:
            t.create_index(f"idx_{kind}", index.IndexInfo("mv", kind_type, opts))
        except Exception as error:
            print(f"INDEX {kind} build ERROR {error}", flush=True)
            db.drop_table(table, ConflictType.Ignore)
            continue
        build = time.perf_counter() - start
        print(f"INDEX {kind} build {build:.4f}", flush=True)

        # Warm the first query (plan/cache effects), then time the steady state.
        t.output(["id"]).match_tensor("mv", QUERY, "float", 10).to_result()
        start = time.perf_counter()
        for _ in range(queries):
            t.output(["id"]).match_tensor("mv", QUERY, "float", 10).to_result()
        elapsed = time.perf_counter() - start
        print(f"INDEX {kind} query {elapsed / queries:.6f}", flush=True)
        print(f"INDEX {kind} query_total {elapsed:.4f}", flush=True)

        db.drop_table(table, ConflictType.Ignore)

    obj.disconnect()
    return 0


if __name__ == "__main__":
    sys.exit(main())
