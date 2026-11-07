"""CI benchmark (tests.yml: reldeb_amd64_benchmark) reproduced over SQL.

The CI job runs `knn_import_benchmark` / `knn_query_benchmark` against the SIFT
or GIST 1M dataset with an HNSW index (`M=16`, `ef_construction=200`,
`metric=l2`, `encode=lvq`), then queries with `ef=200`, `topk=100`. This
driver issues the same statements to an engine over the PG wire so the
reference and the rewrite can be compared on the same machine.

`--dataset gist` switches to the 960-dimension GIST files;
`--no-index` skips `CREATE INDEX` and measures the exact full scan (brute
force); `--build-type lsg` builds the current LSP/LSG HNSW variant.

Usage: perf_sift.py <cpp|rust> [--dataset sift|gist] [--no-index]
                    [--build-type plain|lsg] [--rows N] [--queries N]
                    [--ef N] [--times N] [--port N] [--data-dir DIR]
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
from restart_test import bytes_common as bc

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATASETS = {
    "sift": {
        "dir": os.environ.get("SIFT_DIR", os.path.join(REPO_ROOT, "test/data/benchmark/sift_1m")),
        "dim": 128,
        "table": "sift",
        "prefix": "sift",
    },
    "gist": {
        "dir": os.environ.get("GIST_DIR", os.path.join(REPO_ROOT, "test/data/benchmark/gist_1m")),
        "dim": 960,
        "table": "gist",
        "prefix": "gist",
    },
}

# Everything this driver writes stays inside the checkout (`.scratch/`), so a
# run is reproducible from the tree alone and no state hides in /tmp.
SCRATCH = os.path.join(REPO_ROOT, ".scratch")


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


def subset_fvecs(src, dst, rows, dims):
    """The first `rows` records of `src` as a standalone fvecs file."""
    if rows <= 0 or os.path.exists(dst) and os.path.getsize(dst) == rows * (4 + 4 * dims):
        return dst
    with open(src, "rb") as f, open(dst, "wb") as g:
        for _ in range(rows):
            rec = f.read(4 + 4 * dims)
            if len(rec) < 4 + 4 * dims:
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


def knn_sql(table, vec, topk, ef, use_index):
    stmt = (
        f"SELECT ROW_ID() FROM {table} SEARCH MATCH VECTOR "
        f"(col1, {vec}, 'float', 'l2', {topk})"
    )
    if use_index:
        stmt += f" WITH (ef = {ef})"
    return stmt + ";"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("engine", choices=["cpp", "rust"])
    ap.add_argument("--dataset", choices=["sift", "gist"], default="sift")
    ap.add_argument("--no-index", action="store_true", help="brute force: skip CREATE INDEX")
    ap.add_argument("--build-type", choices=["plain", "lsg"], default="plain")
    ap.add_argument("--data-dir", default="", help="override the dataset directory")
    ap.add_argument("--rows", type=int, default=0, help="0 = the whole base")
    ap.add_argument("--queries", type=int, default=100)
    ap.add_argument("--ef", type=int, default=200)
    ap.add_argument("--times", type=int, default=1)
    ap.add_argument("--port", type=int, default=0)
    args = ap.parse_args()

    spec = DATASETS[args.dataset]
    dims = spec["dim"]
    table = spec["table"]
    prefix = spec["prefix"]
    data_dir = args.data_dir or spec["dir"]
    base_path = os.path.join(data_dir, f"{prefix}_base.fvecs")
    query_path = os.path.join(data_dir, f"{prefix}_query.fvecs")
    truth_path = os.path.join(data_dir, f"{prefix}_groundtruth.ivecs")
    for path in (base_path, query_path, truth_path):
        if not os.path.exists(path):
            raise SystemExit(
                f"missing {os.path.basename(path)}: {path} "
                f"(set {args.dataset.upper()}_DIR or --data-dir)"
            )

    port = args.port or (5432 if args.engine == "cpp" else 23817)
    root = os.path.join(SCRATCH, f"perf-{args.engine}")
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

    base = base_path
    if args.rows:
        base = subset_fvecs(
            base_path,
            os.path.join(SCRATCH, f"{prefix}_base_{args.rows}.fvecs"),
            args.rows,
            dims,
        )

    result = {
        "engine": args.engine,
        "dataset": args.dataset,
        "index": "none" if args.no_index else "hnsw",
        "build_type": None if args.no_index else args.build_type,
        "rows": args.rows or 1_000_000,
        "ef": args.ef,
        "queries": args.queries,
        "times": args.times,
    }
    try:
        wait_ready(port, proc, log)
        time.sleep(1)

        sql(port, f"DROP TABLE IF EXISTS {table};")
        r = sql(port, f"CREATE TABLE {table} (col1 EMBEDDING(FLOAT,{dims}));")
        if r.returncode != 0:
            raise SystemExit(f"create table: {r.stderr}")

        t0 = time.time()
        r = sql(port, f"COPY {table} FROM '{base}' WITH (DELIMITER ',', FORMAT fvecs);", timeout=3600)
        result["import_s"] = round(time.time() - t0, 3)
        if r.returncode != 0:
            raise SystemExit(f"copy: {r.stderr[-2000:]}")
        result["imported_rows"] = sql(port, f"SELECT count(*) FROM {table};").stdout.strip().splitlines()[-1]

        if args.no_index:
            result["index_s"] = 0.0
        else:
            params = "M = 16, ef_construction = 200, metric = l2, encode = lvq"
            if args.build_type == "lsg":
                params += ", build_type = 'lsg'"
            t0 = time.time()
            r = sql(
                port,
                f"CREATE INDEX hnsw_index ON {table} (col1) USING Hnsw WITH ({params});",
                timeout=7200,
            )
            result["index_s"] = round(time.time() - t0, 3)
            if r.returncode != 0:
                raise SystemExit(f"create index: {r.stderr[-2000:]}")

        queries = read_fvecs(query_path, args.queries)
        truth = read_ivecs(truth_path, args.queries)

        latencies = []
        hits = 0
        total = 0
        warmup = queries[: min(5, len(queries))]
        use_index = not args.no_index
        for q in warmup:
            vec = "[" + ",".join(f"{x:.6f}" for x in q) + "]"
            sql(port, knn_sql(table, vec, 100, args.ef, use_index))
        for _ in range(args.times):
            for qi, q in enumerate(queries):
                vec = "[" + ",".join(f"{x:.6f}" for x in q) + "]"
                t0 = time.time()
                r = sql(port, knn_sql(table, vec, 100, args.ef, use_index))
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
