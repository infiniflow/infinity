#!/usr/bin/env python3
"""How much does each secondary index cost to keep current?

A 100k-row table is updated (5k rows per step) and deleted from with 0, 1, 2
and 3 secondary indexes live, so the difference between the steps is one
index: the per-row cost of removing and re-inserting keys. Same script for both
engines; the runner decides which binary listens.
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
BATCH = 2000
ROWS_PER_STEP = 5000
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


def timed(label, fn):
    start = time.perf_counter()
    fn()
    elapsed = time.perf_counter() - start
    print(f"MAINT {label} {elapsed:.6f}  # {elapsed / ROWS_PER_STEP * 1e6:.1f} us/row", flush=True)


def main():
    obj = connect()
    db = obj.get_database("default_db")
    db.drop_table("sec_maint", ConflictType.Ignore)
    t = db.create_table(
        "sec_maint",
        {
            "id": {"type": "int"},
            "name": {"type": "varchar"},
            "score": {"type": "double"},
        },
        ConflictType.Error,
    )
    for base in range(0, ROWS, BATCH):
        end = min(base + BATCH, ROWS)
        t.insert(
            [{"id": i, "name": f"n-{i}", "score": float(i % 1000)} for i in range(base, end)]
        )
    obj.flush_data()
    time.sleep(1)

    steps = [
        ("none", []),
        ("score", [("idx_score", "score")]),
        ("score+name", [("idx_name", "name")]),
        ("score+name+id", [("idx_id", "id")]),
    ]
    marker = 5000.0
    for step, (label, _new) in enumerate(steps):
        if label != "none":
            name, column = _new[0]
            t.create_index(name, index.IndexInfo(column, index.IndexType.Secondary))
        lo = step * ROWS_PER_STEP
        hi = lo + ROWS_PER_STEP
        value = marker + step
        timed(
            f"update_{label}",
            lambda: t.update(f"id >= {lo} and id < {hi}", {"score": value}),
        )
        timed(f"delete_{label}", lambda: t.delete(f"score = {value}"))

    obj.disconnect()
    return 0


if __name__ == "__main__":
    sys.exit(main())