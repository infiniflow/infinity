#!/usr/bin/env python3
"""Secondary-index benchmark: build cost, lookups, maintenance cost.

The same script runs against either engine (the runner decides which binary
listens), so the numbers are comparable: same rows, same SQL, same client, same
machine, every background task off. `*_noindex` phases repeat the maintenance
workload without any index, so the index cost is the difference.
"""

import logging
import os
import sys
import time

import infinity
from infinity import index
from infinity.common import ConflictType, NetworkAddress

logging.disable(logging.CRITICAL)

ROWS = int(os.environ.get("SEC_ROWS", "100000"))
POINTS = int(os.environ.get("SEC_POINTS", "200"))
RANGES = int(os.environ.get("SEC_RANGES", "100"))
URI = NetworkAddress("127.0.0.1", 23817)


def connect(deadline_s=180):
    deadline = time.time() + deadline_s
    while True:
        try:
            obj = infinity.connect(URI)
            obj.get_database("default_db").list_tables()
            return obj
        except Exception:
            if time.time() > deadline:
                raise
            time.sleep(1)


def insert_batches(t, start, count, batch=2000):
    for base in range(start, start + count, batch):
        end = min(base + batch, start + count)
        t.insert(
            [
                {"id": i, "name": f"n-{i}", "score": float(i % 1000)}
                for i in range(base, end)
            ]
        )


def timeit(label, fn):
    start = time.perf_counter()
    fn()
    elapsed = time.perf_counter() - start
    print(f"SEC {label} {elapsed:.6f}", flush=True)
    return elapsed


def main():
    obj = connect()
    db = obj.get_database("default_db")
    db.drop_table("sec_bench", ConflictType.Ignore)
    t = db.create_table(
        "sec_bench",
        {
            "id": {"type": "int"},
            "name": {"type": "varchar"},
            "score": {"type": "double"},
        },
        ConflictType.Error,
    )

    # 1. Rows in, then seal them: a lookup has to read sealed blocks.
    timeit("insert", lambda: insert_batches(t, 0, ROWS))
    obj.flush_data()
    time.sleep(1)

    def point():
        for i in range(POINTS):
            t.output(["id"]).filter(f"score = {float(i % 1000)}").to_result()

    def range_q():
        for i in range(RANGES):
            lo = float(i % 900)
            t.output(["id"]).filter(
                f"score >= {lo} and score < {lo + 10}"
            ).to_result()

    def varchar_point():
        for i in range(POINTS):
            t.output(["id"]).filter(f"name = 'n-{i * 7 % ROWS}'").to_result()

    # 2. The same queries with no index: the scan baseline.
    point()
    timeit("noindex_point", point)
    range_q()
    timeit("noindex_range", range_q)

    # 3. Build both index kinds over the sealed table.
    timeit(
        "build_score",
        lambda: t.create_index(
            "idx_score", index.IndexInfo("score", index.IndexType.Secondary)
        ),
    )
    timeit(
        "build_name",
        lambda: t.create_index(
            "idx_name", index.IndexInfo("name", index.IndexType.Secondary)
        ),
    )
    timeit(
        "build_id",
        lambda: t.create_index("idx_id", index.IndexInfo("id", index.IndexType.Secondary)),
    )

    def unique_point():
        for i in range(POINTS):
            t.output(["id", "name"]).filter(f"id = {i * 499}").to_result()

    unique_point()
    timeit("index_unique_point", unique_point)

    point()
    timeit("index_point", point)
    range_q()
    timeit("index_range", range_q)
    varchar_point()
    timeit("index_varchar_point", varchar_point)

    # 4. Maintenance with both indexes live.
    timeit("insert_with_index", lambda: insert_batches(t, ROWS, 20000))
    timeit(
        "update_with_index",
        lambda: t.update("id >= 0 and id < 5000", {"score": 1234.0}),
    )
    timeit("delete_with_index", lambda: t.delete("score = 1234.0"))

    # 5. The same maintenance without any index: the difference is the cost of
    #    keeping the two indexes current.
    for name in ("idx_score", "idx_name", "idx_id"):
        t.drop_index(name)
    timeit(
        "update_noindex",
        lambda: t.update("id >= 20000 and id < 25000", {"score": 777.0}),
    )
    timeit("delete_noindex", lambda: t.delete("score = 777.0"))

    obj.flush_data()
    obj.disconnect()
    return 0


if __name__ == "__main__":
    sys.exit(main())
