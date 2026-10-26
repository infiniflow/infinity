"""CI benchmark (tests.yml: reldeb_amd64_benchmark) reproduced over SQL.

The CI job runs `knn_import_benchmark` / `knn_query_benchmark` against the SIFT
1M dataset with an HNSW index (`M=16`, `ef_construction=200`, `metric=l2`,
`encode=lvq`), then queries with `ef=200`, `topk=100`. This driver issues the
same statements to an engine over the PG wire so the reference and the rewrite
can be compared on the same machine.

Usage: perf.py <cpp|rust> [--rows N] [--queries N] [--ef N] [--times N]
"""

import argparse
import json
import os
import shutil
import signal
import struct
import subprocess
import sys
import time

sys.path.insert(0, "python")
from restart_test import bytes_common as bc  # noqa: E402

SIFT = "/home/zhichyu/benchmark/sift1M"
DIMS = 128


def read_fvecs(path, limit=None):
    out = []
    with open(path, "rb") as f:
        while True:
            head = f.read(4)
            if len(head) < 4:
                break
            dim = struct.unpack("<i", head)[0]
            body = f.read(4 * dim)
            if len(body) < 4 * dim:
                break
            out.append(struct.unpack(f"<{dim}f", body))
            if limit and len(out) >= limit:
                break
    return out


def read_ivecs(path, limit=None):
    out = []
    with open(path, "rb") as f:
        while True:
            head = f.read(4)
            if len(head) < 4:
                break
            dim = struct.unpack("<i", head)[0]
            body = f.read(4 * dim)
            if len(body) < 4 * dim:
                break
            out.append(struct.unpack(f"<{dim}i", body))
            if limit and len(out) >= limit:
                break
    return out


def subset_fvecs(src, dst, rows):
    """The first `rows` records of `src` as a standalone fvecs file."""
    if rows <= 0 or os.path.exists(dst) and os.path.getsize(dst) == rows * (4 + 4 * DIMS):
        return dst
    with open(src, "rb") as f, open(dst, "wb") as g:
        for _ in range(rows):
            rec = f.read(4 + 4 * DIMS)
            if len(rec) < 4 + 4 * DIMS:
                break
            g.write(rec)
    return dst


def sql(port, stmt, timeout=None):
    return subprocess.run(
        [
            "psql", "-h", "127.0.0.1", "-p", str(port), "-U", "admin",
            "-d", "default_db", "-A", "-t", "-c", stmt,
        ],
        capture_output=True,
        text=True,
        timeout=timeout,
    )


def wait_ready(port, proc, log, seconds=300):
    deadline = time.time() + seconds
    while time.time() < deadline:
        if proc.poll() is not None:
            raise SystemExit(f"engine exited:\n{open(log).read()[-3000:]}")
        if sql(port, "show databases;").stdout.strip():
            return
        time.sleep(0.5)
    raise SystemExit("engine did not come up")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("engine", choices=["cpp", "rust"])
    ap.add_argument("--rows", type=int, default=0, help="0 = the whole base")
    ap.add_argument("--queries", type=int, default=100)
    ap.add_argument("--ef", type=int, default=200)
    ap.add_argument("--times", type=int, default=1)
    ap.add_argument("--port", type=int, default=0)
    args = ap.parse_args()

    port = args.port or (5432 if args.engine == "cpp" else 23817)
    root = f"/tmp/perf-{args.engine}"
    shutil.rmtree(root, ignore_errors=True)
    os.makedirs(root, exist_ok=True)
    bc._write_config(args.engine, root, port)
    binary = bc.CPP_BIN if args.engine == "cpp" else "rust_engine/scripts/infinity_rust_shim.sh"
    log = os.path.join(root, "engine.log")
    proc = subprocess.Popen(
        [binary, f"--config={root}/engine.toml"],
        stdout=open(log, "w"),
        stderr=subprocess.STDOUT,
        env=dict(os.environ, INFINITY_DATA=root),
        start_new_session=True,
    )

    base = os.path.join(SIFT, "sift_base.fvecs")
    if args.rows:
        base = subset_fvecs(
            os.path.join(SIFT, "sift_base.fvecs"),
            f"/tmp/sift_base_{args.rows}.fvecs",
            args.rows,
        )

    result = {"engine": args.engine, "rows": args.rows or 1_000_000, "ef": args.ef,
              "queries": args.queries, "times": args.times}
    try:
        wait_ready(port, proc, log)
        time.sleep(1)

        sql(port, "DROP TABLE IF EXISTS sift;")
        r = sql(port, "CREATE TABLE sift (col1 EMBEDDING(FLOAT,128));")
        if r.returncode != 0:
            raise SystemExit(f"create table: {r.stderr}")

        t0 = time.time()
        r = sql(port, f"COPY sift FROM '{base}' WITH (DELIMITER ',', FORMAT fvecs);", timeout=3600)
        result["import_s"] = round(time.time() - t0, 3)
        if r.returncode != 0:
            raise SystemExit(f"copy: {r.stderr[-2000:]}")
        result["imported_rows"] = sql(port, "SELECT count(*) FROM sift;").stdout.strip().splitlines()[-1]

        t0 = time.time()
        r = sql(
            port,
            "CREATE INDEX hnsw_index ON sift (col1) USING Hnsw "
            "WITH (M = 16, ef_construction = 200, metric = l2, encode = lvq);",
            timeout=7200,
        )
        result["index_s"] = round(time.time() - t0, 3)
        if r.returncode != 0:
            raise SystemExit(f"create index: {r.stderr[-2000:]}")

        queries = read_fvecs(os.path.join(SIFT, "sift_query.fvecs"), args.queries)
        truth = read_ivecs(os.path.join(SIFT, "sift_groundtruth.ivecs"), args.queries)

        latencies = []
        hits = 0
        total = 0
        warmup = queries[: min(5, len(queries))]
        for q in warmup:
            vec = "[" + ",".join(f"{x:.6f}" for x in q) + "]"
            sql(
                port,
                f"SELECT ROW_ID() FROM sift SEARCH MATCH VECTOR (col1, {vec}, 'float', 'l2', 100) "
                f"WITH (ef = {args.ef});",
            )
        for _ in range(args.times):
            for qi, q in enumerate(queries):
                vec = "[" + ",".join(f"{x:.6f}" for x in q) + "]"
                t0 = time.time()
                r = sql(
                    port,
                    f"SELECT ROW_ID() FROM sift SEARCH MATCH VECTOR (col1, {vec}, 'float', 'l2', 100) "
                    f"WITH (ef = {args.ef});",
                )
                latencies.append(time.time() - t0)
                if r.returncode != 0:
                    raise SystemExit(f"query: {r.stderr[-1000:]}")
                got = set()
                for line in r.stdout.splitlines():
                    line = line.strip()
                    if line:
                        try:
                            got.add(int(line))
                        except ValueError:
                            pass
                total += len(truth[qi])
                hits += len(got & set(truth[qi]))

        latencies.sort()
        result["qps"] = round(len(latencies) / sum(latencies), 2) if latencies else 0
        result["lat_p50_ms"] = round(latencies[len(latencies) // 2] * 1000, 3) if latencies else 0
        result["lat_p95_ms"] = round(latencies[int(len(latencies) * 0.95)] * 1000, 3) if latencies else 0
        result["recall_at_100"] = round(hits / total, 4) if total else 0
    finally:
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        except Exception:
            pass
        proc.wait(timeout=60)
    print(json.dumps(result))


if __name__ == "__main__":
    main()
