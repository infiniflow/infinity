#!/usr/bin/env python3
"""Gate E driver: the sift-style insert/select workload over the wire.

Runs against whichever server is listening on `URI`, so the identical script
can produce a golden log from the C++ engine and a candidate log from the Rust
engine. Each measured phase prints a line in the format the CI checker expects:

    Total cost : <seconds> s

Usage: bench_sdk_driver.py <host> <port> [rows]
"""

import sys
import time

import infinity
from infinity.common import ConflictType, NetworkAddress


def main() -> int:
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 23917
    rows = int(sys.argv[3]) if len(sys.argv) > 3 else 20000

    # The reference server answers 2008 (Infinity is initing) until recovery
    # finishes, so connect lazily with the same retry the test harness uses.
    deadline = time.time() + 120
    while True:
        try:
            obj = infinity.connect(NetworkAddress(host, port))
            db = obj.get_database("default_db")
            break
        except Exception:
            if time.time() > deadline:
                raise
            time.sleep(1)
    db.drop_table("bench_t", ConflictType.Ignore)
    t = db.create_table(
        "bench_t", {"id": {"type": "int"}, "name": {"type": "varchar"}}
    )

    batch = 1000
    batches = [
        [{"id": i, "name": f"row-{i}"} for i in range(b, min(b + batch, rows))]
        for b in range(0, rows, batch)
    ]

    start = time.perf_counter()
    for chunk in batches:
        t.insert(chunk)
    print(f"Total cost : {time.perf_counter() - start:.6f} s", flush=True)

    start = time.perf_counter()
    data, _, _ = t.output(["id", "name"]).to_result()
    print(f"Total cost : {time.perf_counter() - start:.6f} s", flush=True)
    if len(data["id"]) != rows:
        print(f"row count mismatch: {len(data['id'])} != {rows}", file=sys.stderr)
        return 1

    start = time.perf_counter()
    t.output(["id"]).filter(f"id = {rows - 1}").to_result()
    print(f"Total cost : {time.perf_counter() - start:.6f} s", flush=True)

    obj.disconnect()
    return 0


if __name__ == "__main__":
    sys.exit(main())
