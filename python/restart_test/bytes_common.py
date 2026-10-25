"""The two engines must write the same bytes for the same data.

`test_cross_engine_compat.py` proves the two engines can *read* each other's
directories. This test proves the stronger claim: given the same input, they
*write* the same files. It is the format-equivalence check the behavioural
test cannot make — a chunk that both engines happen to interpret correctly is
still a format difference if their bytes differ.

The comparison is per file class, because the files are not all equally
determined. Which class a file falls into was **measured**, not assumed: the
reference was run several times on the same input and the distinct byte
patterns counted (see `test_the_reference_is_deterministic_where_the_test_assumes_it`).

* **Deterministic** — the block columns (`.col`), the variable-length outline
  (`col_*_out`), the full-text chunk (`.dic`/`.pos`/`.len`), the sparse (BMP)
  index chunk and every secondary index chunk (integral, fixed and floating
  point keys). The reference produces the same bytes on every run, so the two
  engines must too: **byte for byte identical**. This is the strong signal.
  The data set is `ROWS` rows (200) with one block: the reference's index chunk
  *boundaries* are a scheduling outcome (its dump runs on a worker thread), so
  they can only be compared where they are stable - one block. The block layer
  is compared at its real size in `test_the_blocks_match_across_many_blocks`
  (20000 rows, three blocks).
  The sparse chunk is in this class because the order of a term's rows inside a
  block is defined: the reference sorts the block's `(term, offset, value)`
  triples with a **stable** sort on the term id (`ToBlockFwd`,
  `bmp_fwd.cppm:312`), so a term's rows stay in document order. The secondary
  chunks are here because the reference's PGM fit is reproducible: pure
  `__int128` arithmetic over an integral key (`piecewise_linear_model.hpp:249`)
  and pure `double` arithmetic over a floating-point one (`LargeSigned<T>` is
  `double` for a floating `T`, `piecewise_linear_model.hpp:40`), with a
  well-defined guard key at the top of every level (`pgm_index.hpp:104-130`).
* **Deterministic** — the block `version` file: its `create_ts` values are the
  commit ts of the append that filled each block and its marked slots carry the
  ts of the delete that emptied them. The two engines allocate the same
  sequence for the same statements (bootstrap, initial checkpoint, every DDL,
  every append, every delete — `rust_engine/docs/FORMAT_EQUIVALENCE_TESTS.md`
  §6.6), so the ts, and with it the whole file, must match.
* **Not byte comparable — compared by content** — the HNSW chunk's **graph**:
  the reference builds it with a `thread_local std::mt19937`
  (`hnsw_alg.cppm:107`), so which thread inserts which vector — and therefore
  which node gets which level — is up to the scheduler. Measured: five runs of
  the reference on 5000 rows produced **five different** HNSW chunks, while
  every other file was byte-stable.

  What must match is the **postings** — the vectors and labels — because that is
  what the index *says*. The postings are compared by decoding both chunks.
* **Not comparable at all** — the RocksDB `catalog/` directory and the `wal/`
  directory. Both carry session ids, timestamps and LSNs; the reference is
  not even byte-reproducible against *itself* here.

The test therefore asserts:
1. every deterministic file is byte identical — including the sparse index
   chunk and every secondary index chunk;
2. the `version` file is byte identical: its `create_ts` values and its
   deleted-slot ts are the commit ts of the appends and deletes that wrote
   them, and the two engines allocate the same sequence (`_config` explains
   the one scheduling-dependent dump the test switches off);
3. the index chunk that carries a build-dependent model — the HNSW graph —
   still has identical **postings** (decoded), and the rest of the HNSW chunk
   (header, vectors, labels) is byte identical;
4. the catalog's *logical* content (the key/value pairs the reference sees
   through `SHOW CATALOG 'all'`) is identical, keys and bare-ts values
   included — with every background task off, both engines dump at the same
   points, so even `idx_chunk|…|{chunk_id}` names the same transaction;
5. the indexes are **logically** equivalent: the reference reads each
   engine's directory and must answer the same queries the same way. This
   covers everything the byte comparison cannot see.

The cases themselves are split by layer — what the comparison is *about* —
so a failure names the level it belongs to and a layer can be run on its own
(`pytest python/restart_test/test_cross_engine_bytes_30_segments.py`):

| 文件 | 层 | 对照物 |
|---|---|---|
| `test_cross_engine_bytes_00_premises.py` | 前提 | 每个引擎自己是否确定（不比较两个引擎） |
| `test_cross_engine_bytes_10_encoding.py` | L0 编码原语 | 一个文件、一条记录 |
| `test_cross_engine_bytes_20_table.py` | L1 单表 | 一张表的目录 + catalog 逻辑内容 |
| `test_cross_engine_bytes_30_segments.py` | L2 段演化 | 段号、块归属、压缩与 cleanup |
| `test_cross_engine_bytes_40_wal.py` | L3 WAL | 帧（载荷、txn_id、CRC） |
| `test_cross_engine_bytes_50_mutations.py` | L4 变更与恢复 | 一轮写入历史之后的结果 |
| `test_cross_engine_bytes_60_storage.py` | L5 外部存储 | 对象目录与 S3 |

`test_cross_engine_compat.py` 是 L6 交接语义（另一引擎读回、答案一致），
它有自己的 harness，单独一个文件。

This module holds the harness every layer shares: the config template, the
engine launcher (including the crash/kill phases), the file walker, the
per-class comparators, and the catalog/SELECT readers. It defines no test.
"""

import hashlib
import json
import os
import re
import shutil
import signal
import socket
import struct
import subprocess
import time

import infinity
from infinity import index
from infinity.common import ConflictType, NetworkAddress

CPP_BIN = os.environ.get("INFINITY_CPP_BIN", "cmake-build-reldeb/src/infinity")
RUST_BIN = os.environ.get("INFINITY_RUST_BIN", "rust_engine/scripts/infinity_rust_shim.sh")
# The Rust CLI binary itself. `RUST_BIN` is the harness shim, which only
# forwards `--config=…` and otherwise forces `serve`, so a subcommand has to be
# sent to the binary directly.
RUST_CLI = os.environ.get("INFINITY_RUST_CLI", "rust_engine/target/release/infinity")
URI = NetworkAddress("127.0.0.1", 23817)

TABLE = "test_cross_engine_bytes"
COLUMNS = 6  # the schema in `_seed`
# The byte comparison of the **index chunks** runs at 200 rows, where the
# reference is reproducible: its chunk boundaries are a scheduling outcome
# (`test_the_chunk_boundaries_are_timing_dependent`), so at 20000 rows the two
# engines can split the same postings differently. The **block layer** is
# compared at 20000 rows (`test_the_blocks_match_across_many_blocks`), where it
# must be byte identical: 3 blocks of 8192/8192/3616 rows.
ROWS = int(os.environ.get("INFINITY_BYTES_ROWS", "200"))
BLOCK_ROWS = int(os.environ.get("INFINITY_BYTES_BLOCK_ROWS", "20000"))
DET_ROWS = int(os.environ.get("INFINITY_BYTES_DET_ROWS", "200"))

# `idx_<n>` follows the order `_seed` creates the indexes in. The value is how
# the index's chunk is compared (see the docstring at the top of the file).
INDEX_KINDS = {
    1: "fulltext",
    2: "hnsw",
    3: "secondary-u64",
    4: "secondary-f32",
    5: "bmp",
}

# The engine's root: the C++ engine reads `data_dir`/`catalog_dir`/`wal_dir`
# from its config, the port derives its root from the config's
# `persistence_dir` (the CLI takes the parent). Both are pointed at a
# per-engine directory so the two runs cannot see each other's files.
ROOT = os.environ.get("INFINITY_BYTES_ROOT", "/var/infinity/bytes")

# Object storage (`storage_type = "minio"`). Unset means the round-trip test is
# skipped: it needs a live S3 endpoint, which the byte gates do not.
MINIO_URL = os.environ.get("INFINITY_MINIO_URL", "")
MINIO_BUCKET = os.environ.get("INFINITY_MINIO_BUCKET_PREFIX", "infinity-bytes")
MINIO_ACCESS_KEY = os.environ.get("INFINITY_MINIO_ACCESS_KEY", "")
MINIO_SECRET_KEY = os.environ.get("INFINITY_MINIO_SECRET_KEY", "")


def _config(root, engine, pg_port, quota="2GB"):
    """A config for one engine, rooted at `root`, with every asynchronous task off.

    The two engines read the root differently: the reference takes
    `data_dir`/`catalog_dir`/`wal_dir` and uses `persistence_dir` only as a
    "use the object store" switch (it must stay empty), while the port
    derives its root from the *parent* of `persistence_dir`
    (`serve_from_config`, `crates/infinity-cli/src/main.rs:98`).

    **Determinism is the input of this test, so no background task may write
    while the script runs.** Every interval the reference accepts is set to
    "0s", which its parser turns into 0 and its triggers read as "disabled"
    (`MIN_*_INTERVAL_SEC = 0`, "0 means disable the function",
    `src/common/default_values.cppm:98-123`):

    * `[storage] optimize_interval` - merges index chunks from a background
      task (`OptimizeIndexProcessor`), which rewrites the very chunk files the
      test compares;
    * `[storage] cleanup_interval` - deletes the chunks a previous optimize
      merged away;
    * `[storage] compact_interval` - merges segments on its own schedule, so
      the segment/block layout would depend on wall-clock time;
    * `[wal] checkpoint_interval` - writes a checkpoint from the WAL thread,
      which spends a commit ts and rewrites the C++ mirror (the reference's
      default is 30 s, so without this a slow run would checkpoint mid-script).

    A memory index has **two** dump triggers, and only one of them is
    deterministic:

    * **row capacity** - `[storage] mem_index_capacity` (default 65536 = 8
      blocks). When a memory index holds that many rows it is dumped
      (`new_txn_index_impl.cpp:1381`). The count is a property of the script, so
      the trigger is reproducible and stays **on**;
    * **memory usage** - `[buffer] memindex_memory_quota`. The tracer
      (`BGMemIndexTracer::IncreaseMemoryUsage`,
      `src/storage/tracer/memindex_tracer.cppm:102`) dumps from a **worker
      thread** as soon as the memory index crosses the quota, and that dump is a
      write transaction whose commit ts is allocated whenever the worker happens
      to run - it lands between whatever statements the client sent next, so the
      chunk boundaries *and* the ts sequence become a scheduling outcome. The
      quota is therefore set beyond anything this test can allocate (`"2GB"`, the
      buffer size; the reference's byte-size parser rejects `"0"` - "Invalid
      byte size: 0"), which leaves the explicit `DumpIndex` calls and the
      row-capacity trigger as the only dumps.
    """
    persistence = "" if engine == "cpp" else f"{root}/persistence"
    return f"""
[general]
version = "0.7.4"
time_zone = "utc-8"

[network]
server_address = "0.0.0.0"
http_port = 23821
peer_ip = "0.0.0.0"
peer_port = 23851
postgres_port = {pg_port}

[log]
log_dir = "{root}/log"
log_to_stdout = true
log_level = "info"

[storage]
data_dir = "{root}/data"
catalog_dir = "{root}/catalog"
persistence_dir = "{persistence}"
# Every background task is off: the mirror, the segment layout and the ts
# sequence must not depend on when a worker thread happens to run.
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"
# The *row-count* trigger stays on (the reference default, 8 blocks): it fires
# when a memory index holds this many rows, which is a property of the script,
# not of the clock (`new_txn_index_impl.cpp:1381`). Only the *memory-usage*
# trigger (`BGMemIndexTracer`, a worker thread) is disabled, by the quota below.
mem_index_capacity = 65536

[buffer]
buffer_manager_size = "2GB"
temp_dir = "{root}/tmp"
result_cache = "off"
memindex_memory_quota = "{quota}"

[wal]
wal_dir = "{root}/wal"
checkpoint_interval = "0s"

[resource]
"""


def _dump_indexes(t, names):
    """Dump each index's memory tail explicitly, in creation order.

    The reference's dump is otherwise driven by its memory quota and runs on a
    worker thread: the first crossing proposes the largest index, whose size is
    then added to `proposed_dump_size_`, and with several large indexes the
    quota is never crossed again - measured on the six-index table, only one
    index (the whole 200 rows in it) ever landed and the other five never
    appeared, 60 s after the insert. `DumpIndex` is the synchronous command
    (`Infinity::DumpIndex` -> `NewTxn::DumpMemIndex`), so the chunks the test
    compares are the dumps it asked for rather than a scheduling outcome.
    """
    for name in names:
        t.dump_index(name)


def _seed(conn, row_count=ROWS):
    """The same table, indexes and rows for both engines."""
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
            "score": {"type": "float"},
        },
        ConflictType.Error,
    )
    t.create_index("ft", index.IndexInfo("body", index.IndexType.FullText))
    t.create_index(
        "vec_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "l2"}
        ),
    )
    t.create_index("sec", index.IndexInfo("title", index.IndexType.Secondary))
    t.create_index("score_idx", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    rows = []
    for i in range(row_count):
        rows.append(
            {
                "id": i,
                "title": f"doc-{i}",
                # A long value: it goes to the outline file, so the `.col` slot
                # holds only its length and its offset.
                "body": f"alpha beta doc{i}",
                "vec": [float(i % 10) / 10.0, 0.1, 0.2, 0.3],
                "sp": {0: 0.5, i % 5 + 1: 1.0},
                "score": i * 0.5,
            }
        )
    # The engine refuses a batch of more than 8192 rows
    # (`LogicalPlanner::insert`, "Insert batch row limit shouldn't more than
    # 8192"), so a multi-block data set arrives as several batches - which is
    # also how it would arrive in practice.
    for start in range(0, len(rows), 8192):
        t.insert(rows[start : start + 8192])
    conn.flush_data()
    _dump_indexes(t, ["ft", "vec_idx", "sec", "score_idx", "sp_idx"])

    time.sleep(2)


def _run_engine(engine, root, pg_port, row_count=ROWS, seed=None, quota="2GB"):
    """Seed one engine's directory, retrying a boot that dies during init.

    The Thrift server accepts connections before storage finishes initialising,
    and a DDL in that window aborts the process (the client sees "TSocket read 0
    bytes"). That is a startup race, not a format difference, so a boot that
    dies before the seed completes is retried once on a clean directory; a real
    crash fails both attempts and is reported.
    """
    last = None
    for _ in range(2):
        try:
            return _boot_and_seed(engine, root, pg_port, row_count, seed, quota)
        except AssertionError as err:
            last = err
            _wait_port_free()
            time.sleep(1)
    raise last


def _boot_and_seed(engine, root, pg_port, row_count, seed, quota):
    # The engines bind a fixed port without `SO_REUSEADDR`; a previous engine
    # (or anything else on the machine) still holding it makes this one abort
    # with "Could not bind: Address already in use".
    _wait_port_free()
    shutil.rmtree(root, ignore_errors=True)
    _write_config(engine, root, pg_port, quota)
    _spawn_and_run(engine, root, seed or (lambda c: _seed(c, row_count)))


def _write_config(engine, root, pg_port, quota="2GB"):
    for sub in ("log", "tmp"):
        os.makedirs(os.path.join(root, sub), exist_ok=True)
    with open(os.path.join(root, "engine.toml"), "w") as handle:
        handle.write(_config(root, engine, pg_port, quota))


def _run_phases(engine, root, pg_port, phases, quota="2GB"):
    """Run each phase in its own engine process, over the same directory.

    A phase is just a callable taking the connection; whether it ends with
    `flush_data()` decides whether the rows it wrote reach the block files or
    only the WAL - which is what the crash test relies on.

    `quota` is passed to `_config`; see there for why a ts-comparing test
    disables the background dump.
    """
    _wait_port_free()
    shutil.rmtree(root, ignore_errors=True)
    _write_config(engine, root, pg_port, quota)
    for seed in phases:
        _spawn_and_run(engine, root, seed)
    return root


def _spawn_and_run(engine, root, seed):
    config = os.path.join(root, "engine.toml")
    if engine == "rust":
        # The port's CLI takes the root as the parent of `persistence_dir`, so
        # the config already points it at `root`.
        argv = [RUST_BIN, f"--config={config}"]
    else:
        argv = [CPP_BIN, f"--config={config}"]
    # The shim creates `$INFINITY_DATA` before it starts the engine; point it
    # at this run's root so it does not need `/var/infinity` to be writable.
    env = dict(os.environ, INFINITY_DATA=root)
    proc = subprocess.Popen(
        argv,
        stdout=open(os.path.join(root, "engine.log"), "a"),
        stderr=subprocess.STDOUT,
        env=env,
        # Its own process group, so the whole group can be killed below.
        start_new_session=True,
    )
    conn = None
    try:
        for _ in range(120):
            if proc.poll() is not None:
                raise AssertionError(f"{engine} engine exited:\n{_log_tail(root)}")
            try:
                conn = infinity.connect(URI)
                # A successful TCP connect is not readiness: the Thrift server
                # accepts the socket before storage finishes initialising, and a
                # request in that window is reset ("TSocket read 0 bytes"). Make
                # a real round-trip before seeding.
                conn.get_database("default_db").list_tables()
                break
            except Exception:
                if conn is not None:
                    try:
                        conn.disconnect()
                    except Exception:
                        pass
                    conn = None
                time.sleep(0.5)
        else:
            raise AssertionError(f"{engine} engine did not come up")
        if seed is not None:
            seed(conn)
    finally:
        if conn is not None:
            try:
                conn.disconnect()
            except Exception:
                pass
        # The whole process group: the shim runs the engine as a *child*, so
        # killing only the shim would orphan the engine, which would keep
        # holding the port and make the next engine abort on startup.
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        except (ProcessLookupError, PermissionError):
            pass
        try:
            proc.wait(timeout=30)
        except subprocess.TimeoutExpired:
            pass
        time.sleep(1)
        # The next engine must be able to bind the port: the reference does
        # not set `SO_REUSEADDR`, so a socket in `TIME_WAIT` would make it
        # abort on startup.
        _wait_port_free()
    return root


def _log_tail(root, lines=20):
    path = os.path.join(root, "engine.log")
    if not os.path.exists(path):
        return "(no engine log)"
    with open(path, errors="replace") as handle:
        return "".join(handle.readlines()[-lines:])


def _port_is_free():
    """Whether a listener could bind 0.0.0.0:23817 without SO_REUSEADDR.

    The reference binds the wildcard address and does not set `SO_REUSEADDR`
    (`TServerSocket::listen`), so this is the exact condition it needs.
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        try:
            sock.bind(("0.0.0.0", URI.port))
            return True
        except OSError:
            return False


def _wait_port_free(timeout=120.0):
    """Wait until the engine's port can be bound again.

    The reference binds the wildcard address without `SO_REUSEADDR`, so a
    socket left in `TIME_WAIT` on 23817 keeps the next engine from binding -
    and it aborts with an unhandled exception when it cannot.
    """
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if _port_is_free():
            return
        time.sleep(0.2)
    raise AssertionError(f"port {URI.port} was still in use after {timeout:.0f}s")


def _files(root):
    """Every file under the engine's `data/` directory, by relative path."""
    out = {}
    base = os.path.join(root, "data")
    for dirpath, _dirnames, filenames in os.walk(base):
        for name in filenames:
            path = os.path.join(dirpath, name)
            rel = os.path.relpath(path, base)
            with open(path, "rb") as handle:
                out[rel] = handle.read()
    return out


def _hnsw_regions(chunk):
    """The HNSW chunk's header, vector region and labels.

    The layout (`knn_format::encode_hnsw_chunk_ext`): `[M][ef_construction]
    [cur_vec_num][store_meta][Mmax0][Mmax][max_layer][enterpoint]`, then the
    vector region, the graph region and the labels.

    The *header* is everything up to `max_layer`: it is derived from the
    index's parameters alone, so it must match. **`max_layer` and the
    enterpoint are properties of the built graph** - how tall it came out and
    which node it hangs from - so they belong with the graph (at 500 rows the
    two engines happen to agree on both; at 8192 they already do not). The
    vector region and the labels are derived from the input and must match.
    """
    _m, _ef, cur = struct.unpack_from("<3Q", chunk, 0)
    # The store meta is 8 bytes here (the dim), then the graph parameters.
    meta_len = 8
    mmax0, mmax = struct.unpack_from("<2Q", chunk, 24 + meta_len)
    _max_layer, _enterpoint = struct.unpack_from("<2i", chunk, 40 + meta_len)
    header_end = 40 + meta_len  # up to (not including) `max_layer`
    data_start = 48 + meta_len  # past the `enterpoint`
    # The vector region: `cur` vectors of `dim` f32. The dim is the first u32
    # of the store meta.
    dim = struct.unpack_from("<I", chunk, 24)[0]
    vector_len = cur * dim * 4
    labels_len = cur * 4
    graph_end = len(chunk) - labels_len
    return {
        "header": chunk[:header_end],
        "vectors": chunk[data_start : data_start + vector_len],
        "labels": chunk[graph_end:],
        "graph": chunk[data_start + vector_len : graph_end],
    }


def _secondary_chunk_rows(chunk, key_width, floating_width=4):
    """The chunk's row count, taken from its PGM model and validated exactly.

    Layout (`SecondaryIndexDataHighCardinalityT::SaveIndexInner`,
    `secondary_index_data_impl.cpp:223`): `n * key_width` keys, `n * u32`
    offsets, then the model (`PGMWithExtraFunction::Save`,
    `secondary_index_pgm.cppm:129`): `u32 n`, `K first_key`, `u32
    segment_count`, the segments, `u32 levels_offsets_count`, then `size_t`
    offsets. A `Segment` is `pack(1) { K key; Floating slope; u32 intercept; }`
    and the model ends at EOF, so exactly one `n` reproduces the file length.

    Deriving `n` rather than assuming a chunk capacity keeps the comparison
    honest when the multi-chunk layout is what is being tested.
    """
    segment_size = key_width + floating_width + 4
    for n in range(1, len(chunk) // (key_width + 4) + 1):
        pos = n * (key_width + 4)
        if pos + 4 + key_width + 4 > len(chunk):
            break
        if struct.unpack_from("<I", chunk, pos)[0] != n:
            continue
        segment_count = struct.unpack_from("<I", chunk, pos + 4 + key_width)[0]
        after = pos + 4 + key_width + 4 + segment_count * segment_size
        if after + 4 > len(chunk):
            continue
        levels = struct.unpack_from("<I", chunk, after)[0]
        if after + 4 + levels * 8 == len(chunk):
            return n
    raise AssertionError(f"no PGM model found in a {len(chunk)}-byte chunk")


def _wait_for_index_chunks(root, indexes, timeout=60.0):
    """Wait until every index has written at least one chunk under `root`.

    The dumps run on a worker pool, so a chunk can still be missing right after
    `flush_data()` returns - a table with six indexes needs longer than one with
    two.
    """
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        present = {
            int(parts[0])
            for parts in (
                name.split("/idx_")[1].split("/")
                for name in _files(root)
                if "/idx_" in name
            )
        }
        if set(range(1, indexes + 1)) <= present:
            return
        time.sleep(1)
    raise AssertionError(f"{root}: index chunks missing after {timeout:.0f}s")


def _chunks(files, index_id):
    """One index's files, keyed by their path after `idx_{id}/`."""
    marker = f"idx_{index_id}/"
    return {
        name.split(marker, 1)[1]: blob for name, blob in files.items() if marker in name
    }


def _check_index(cpp, rust, index_id, kind, failures):
    """Compare one index's files according to its class."""
    cpp_files_for_index = _chunks(cpp, index_id)
    rust_files_for_index = _chunks(rust, index_id)
    if set(cpp_files_for_index) != set(rust_files_for_index):
        # Different chunk boundaries: the reference's are a scheduling outcome,
        # so fall back to what the index says (see `_index_content`).
        a = _index_content(cpp_files_for_index, kind)
        b = _index_content(rust_files_for_index, kind)
        if a is None or b is None:
            return
        if a != b:
            failures.append(
                f"idx_{index_id}: the content differs "
                f"({len(set(cpp_files_for_index))} vs {len(set(rust_files_for_index))} chunks)"
            )
        return
    for name in sorted(cpp_files_for_index):
        a, b = cpp_files_for_index[name], rust_files_for_index[name]
        if kind in ("fulltext", "secondary-u64", "bmp"):
            # Deterministic: the layout is derived from the rows alone.
            if a != b:
                failures.append(
                    f"idx_{index_id}/{name}: "
                    f"cpp {hashlib.md5(a).hexdigest()} ({len(a)} B) != "
                    f"rust {hashlib.md5(b).hexdigest()} ({len(b)} B)"
                )
        elif kind == "ivf":
            # The centroids are a build artifact (see `_ivf_content`), so the
            # header's *counts* and the (offset, vector) pairs are compared and
            # the centroids are not.
            a_c, b_c = _ivf_content(a), _ivf_content(b)
            if a_c is None or b_c is None:
                failures.append(f"idx_{index_id}/{name}: not a plain IVF chunk")
            elif a_c[:3] != b_c[:3]:
                failures.append(f"idx_{index_id}/{name}: header {a_c[:3]} != {b_c[:3]}")
            elif a_c[3] != b_c[3]:
                failures.append(
                    f"idx_{index_id}/{name}: the vectors differ "
                    f"({len(a_c[3])} vs {len(b_c[3])} pairs)"
                )
        elif kind == "hnsw":
            # The header, the vectors and the labels are derived from the
            # input; the graph is built, and the reference does not reproduce
            # it (see the module docstring), so it is compared by content via
            # `_index_answers`.
            regions_a, regions_b = _hnsw_regions(a), _hnsw_regions(b)
            for part in ("header", "vectors", "labels"):
                if regions_a[part] != regions_b[part]:
                    failures.append(f"idx_{index_id}/{name}: {part} differ")
        elif kind in ("secondary-f32", "secondary-f64"):
            # Deterministic: the raw keys, the offsets and the PGM fit are all
            # derived from the rows (`PgmFloat` mirrors `PGMIndex<K, 64, 4,
            # Floating>` in `double`).
            if a != b:
                failures.append(
                    f"idx_{index_id}/{name}: "
                    f"cpp {hashlib.md5(a).hexdigest()} ({len(a)} B) != "
                    f"rust {hashlib.md5(b).hexdigest()} ({len(b)} B)"
                )
        elif kind in ("secondary-u32", "secondary-i64"):
            key_width = {"secondary-u32": 4, "secondary-i64": 8}[kind]
            floating_width = 4
            n_a = _secondary_chunk_rows(a, key_width, floating_width)
            n_b = _secondary_chunk_rows(b, key_width, floating_width)
            if n_a != n_b:
                failures.append(f"idx_{index_id}/{name}: {n_a} rows != {n_b}")
                continue
            postings = n_a * (key_width + 4)
            # The keys and the offsets are what the index says; the model
            # behind them is a build artifact (module docstring).
            if a[:postings] != b[:postings]:
                failures.append(f"idx_{index_id}/{name}: the postings differ")
        else:  # pragma: no cover - a typo in INDEX_KINDS
            raise AssertionError(f"unknown index kind {kind}")


def _bmp_doc_ids(blob):
    """A BMP chunk's document ids.

    The file ends with the `docs` section, `u64 doc_num` then `u32[doc_num]`
    (`BMPAlg::SaveToPtr`), so the count is the `u64` that sits exactly
    `8 + 4 * n` bytes before EOF. Returns `None` if nothing matches.
    """
    for n in range(BLOCK_CAPACITY + 1):
        at = len(blob) - 8 - 4 * n
        if at < 0:
            return None
        if struct.unpack_from("<Q", blob, at)[0] == n:
            return list(struct.unpack_from(f"<{n}I", blob, at + 8))
    return None


def _ivf_content(blob):
    """What a plain IVF chunk says: the header's counts and every
    `(segment_offset, vector)` pair.

    `[row_count][embedding_count][dimension][centroid_count]` then
    `centroid_count * dimension` `f32` centroids, then each part as
    `u32 n`, `u32[n]` offsets and `n * dimension` `f32` vectors
    (`knn_format::encode_ivf_plain_chunk`).

    The centroids are **not** part of the content: the reference samples its
    training set with `std::random_device` + `std::mt19937` and then runs
    k-means (`ivf_index_data_impl.cpp:212-216`), so three runs on one input
    give three different chunks. Measured, not assumed - see
    `test_the_reference_is_deterministic_where_the_test_assumes_it`.
    """
    if len(blob) < 16:
        return None
    row_count, embedding_count, dimension, centroid_count = struct.unpack_from(
        "<4I", blob, 0
    )
    at = 16 + centroid_count * dimension * 4
    if at > len(blob):
        return None
    pairs = []
    for _ in range(centroid_count):
        if at + 4 > len(blob):
            return None
        (n,) = struct.unpack_from("<I", blob, at)
        at += 4
        width = dimension * 4
        if at + n * 4 + n * width > len(blob):
            return None
        offsets = struct.unpack_from(f"<{n}I", blob, at)
        at += n * 4
        for i in range(n):
            pairs.append((offsets[i], blob[at + i * width : at + (i + 1) * width]))
        at += n * width
    if at != len(blob):
        return None
    return (row_count, embedding_count, dimension, sorted(pairs))


def _index_content(files, kind):
    """What an index says, independent of how it was split into chunks.

    The reference's chunk boundaries are a scheduling outcome (see
    `test_the_chunk_boundaries_are_timing_dependent`), so at scale the two
    engines can split the same postings differently. The content - the ordered
    keys and row offsets, the per-row term counts, the vectors and labels - is
    the same either way.
    """
    if kind.startswith("secondary-"):
        key_width = {
            "secondary-f32": 4,
            "secondary-u32": 4,
            "secondary-i64": 8,
            "secondary-f64": 8,
            "secondary-u64": 8,
        }[kind]
        # The PGM's `Floating` is `LargeSigned<K>`: 4 bytes for a `float` key,
        # 8 for a `double` one (`secondary_segment.rs` writes `{f64,f64,u32}`).
        floating_width = 8 if kind == "secondary-f64" else 4
        postings = []
        for name in sorted(files):
            blob = files[name]
            n = _secondary_chunk_rows(blob, key_width, floating_width)
            for i in range(n):
                postings.append(
                    blob[i * key_width : (i + 1) * key_width]
                    + blob[n * key_width + i * 4 : n * key_width + i * 4 + 4]
                )
        return sorted(postings)
    if kind == "fulltext":
        # `.len` is `u32[row_cnt]` per chunk and the chunks cover the rows in
        # base-name order, so concatenating them gives one entry per row.
        blob = b""
        for name in sorted(n for n in files if n.endswith(".len")):
            blob += files[name]
        return blob
    if kind == "hnsw":
        # (label, vector) for every row, sorted by label.
        rows = []
        for name in sorted(files):
            regions = _hnsw_regions(files[name])
            labels = regions["labels"]
            columns = len(labels) // 4
            if not columns:
                continue
            width = len(regions["vectors"]) // columns
            for i in range(columns):
                label = labels[i * 4 : (i + 1) * 4]
                vector = regions["vectors"][i * width : (i + 1) * width]
                rows.append(label + vector)
        return sorted(rows)
    if kind == "bmp":
        # The document ids every chunk covers (the postings themselves are only
        # decoded when the chunk boundaries agree).
        docs = []
        for name in sorted(files):
            ids = _bmp_doc_ids(files[name])
            if ids is None:
                return None
            docs.extend(ids)
        return sorted(docs)
    if kind == "ivf":
        # The union of every chunk's (offset, vector) pairs; the counts come
        # from the first chunk and are the same in every chunk of an index.
        merged = None
        for name in sorted(files):
            content = _ivf_content(files[name])
            if content is None:
                return None
            counts, pairs = content[:3], content[3]
            if merged is None:
                merged = (counts, list(pairs))
            else:
                if merged[0][:2] != counts[:2]:
                    return None
                merged[1].extend(pairs)
        return None if merged is None else (merged[0], sorted(merged[1]))
    return None


def _compare_data_files(cpp, rust, index_kinds):
    """The files of one table (relative to its directory), by class."""
    failures = []

    def blocks(files):
        return {
            name: blob
            for name, blob in files.items()
            if not name.split("/")[0].startswith("idx_")
        }

    cpp_blocks, rust_blocks = blocks(cpp), blocks(rust)
    if set(cpp_blocks) != set(rust_blocks):
        failures.append(
            f"file sets differ: only in cpp {sorted(set(cpp_blocks) - set(rust_blocks))}, "
            f"only in rust {sorted(set(rust_blocks) - set(cpp_blocks))}"
        )
        return failures
    for name in sorted(cpp_blocks):
        if name.endswith("/version"):
            # The `create_ts` of every append and the ts of every delete are
            # the transactions' own commit ts, and the two engines now allocate
            # the same sequence for the same statements, so this file is
            # compared like every other one.
            if cpp[name] != rust[name]:
                failures.append(f"{name}: the version file differs")
        else:
            # Block columns and the outline: laid out from the row values. A
            # `.col` file's NULL slots are filler (see `_mask_null_slots`).
            a, b = cpp_blocks[name], rust_blocks[name]
            if name.endswith(".col"):
                a, b = _mask_null_slots(a), _mask_null_slots(b)
            if a != b:
                failures.append(
                    f"{name}: cpp {hashlib.md5(cpp_blocks[name]).hexdigest()} "
                    f"({len(cpp_blocks[name])} B) != rust "
                    f"{hashlib.md5(rust_blocks[name]).hexdigest()} ({len(rust_blocks[name])} B)"
                )
    # The indexes: their chunk count is a scheduling outcome (see
    # `test_the_chunk_boundaries_are_timing_dependent`), so a differing set of
    # chunk files is compared by content rather than reported.
    index_ids = sorted(
        {
            int(name.split("/")[0][len("idx_") :])
            for name in list(cpp) + list(rust)
            if name.split("/")[0].startswith("idx_")
        }
    )
    for index_id in index_ids:
        kind = index_kinds.get(index_id)
        if kind is None:
            failures.append(f"idx_{index_id}: no comparison class registered")
            continue
        _check_index(cpp, rust, index_id, kind, failures)
    return failures


def _own_table_dir(files, columns):
    """The `db_{db}/tbl_{table}` directory of the table this test created.

    The comparison is scoped to that table rather than to "every file under
    `data/`": the engines listen on a fixed Thrift port, so anything else on the
    machine that connects to it while the test runs can leave a table of its own
    behind. Such a table is not this test's input, and its presence must not turn
    a format difference into a false positive (or the other way round).

    The table is recognised by its column count - the sixth column's `5.col` is
    only written for this schema.
    """
    marker = f"{columns - 1}.col"
    dirs = {
        "/".join(parts[:2])
        for parts in (name.split("/") for name in files)
        if len(parts) == 5 and parts[4] == marker
    }
    assert len(dirs) <= 1, f"more than one table has {columns} columns: {sorted(dirs)}"
    return dirs.pop() if dirs else None


def _under(files, table_dir):
    """The files of `table_dir`, relative to it."""
    if table_dir is None:
        return {}
    return {
        name[len(table_dir) + 1 :]: blob
        for name, blob in files.items()
        if name.startswith(table_dir + "/")
    }


BLOCK_CAPACITY = 8192


def _mask_null_slots(blob):
    """Zero the bytes of a `.col` file that no reader looks at.

    Layout (`CPP_FORMAT_SPEC.md §8.3`): `[buffer header 16][data
    8192 * slot][null bitmap 1024][checksum 8]`, where a set bit means
    **non-NULL** (`cpp_format.rs`).

    Two kinds of byte are left over rather than written:

    * a NULL row's slot - the reference appends values into the block buffer
      and leaves a NULL's slot untouched, so it keeps the *previous* row's
      bytes;
    * the padding of a `Varchar` slot (`[u24 length][5-byte prefix][u64
      offset]`, `src/parser/type/complex/varchar.h`): an *inlined* value fills
      `[3, 3 + length)` and leaves the rest of the 13-byte inline area, a
      **non-inlined** one writes `[3, 8)` (the prefix) only - measured, two
      runs on the same 20000-row file differ in both places (`42 cd cc cc 3d`
      vs another leftover). Nothing reads them: a query reads the outline, and
      `Varchar::ToString` - the only user of the prefix - is a test helper.

    Both are a don't-care to a reader, so the comparison zeroes them rather
    than making "which leftover a slot keeps" part of the format.

    A layout whose data region is not a whole number of bytes per row (the
    bit-packed boolean) is returned unchanged.
    """
    bitmap_len = BLOCK_CAPACITY // 8
    body = len(blob) - 24 - bitmap_len
    if body <= 0 or body % BLOCK_CAPACITY:
        return blob
    width = body // BLOCK_CAPACITY
    base = 16 + body
    bitmap = blob[base : base + bitmap_len]
    out = bytearray(blob)
    for row in range(BLOCK_CAPACITY):
        at = 16 + row * width
        if not (bitmap[row // 8] >> (row % 8) & 1):
            out[at : at + width] = b"\0" * width
            continue
        if width == 16:
            length = int.from_bytes(blob[at : at + 3], "little")
            if length > 13:
                out[at + 3 : at + 8] = b"\0" * 5
            elif length <= width - 3:
                out[at + 3 + length : at + width] = b"\0" * (width - 3 - length)
    return bytes(out)



def _reader_config(root, pg_port):
    """A config for *reading* an engine's directory with the reference.

    The reference treats a non-empty `persistence_dir` as "the data lives in
    an object store" (`Config::UseVFS`), and then it looks for the block
    files in the store instead of on disk. Reading a directory the port
    wrote must therefore use an empty `persistence_dir`, like the port's own
    config does.
    """
    return f"""
[general]
version = "0.7.4"
time_zone = "utc-8"

[network]
server_address = "0.0.0.0"
http_port = 23822
peer_ip = "0.0.0.0"
peer_port = 23852
postgres_port = {pg_port}

[log]
log_dir = "{root}/log"
log_to_stdout = true
log_level = "info"

[storage]
data_dir = "{root}/data"
catalog_dir = "{root}/catalog"
persistence_dir = ""
# Every background task is off: the mirror, the segment layout and the ts
# sequence must not depend on when a worker thread happens to run.
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"
mem_index_capacity = 65536

[buffer]
buffer_manager_size = "2GB"
temp_dir = "{root}/tmp"
result_cache = "off"
# Opening a directory to read it must not start a background dump.
memindex_memory_quota = "2GB"

[wal]
wal_dir = "{root}/wal"
checkpoint_interval = "0s"

[resource]
"""


def _catalog_pairs(root, pg_port):
    """The catalog's key/value pairs, as the reference sees them.

    `SHOW CATALOG 'all'` lists what the engine actually holds, which is the
    logical content of the catalog. Every record is returned as the reference
    reports it - including the values that are only a commit ts - so the caller
    compares both engines' text as it stands.

    The reference is started on the directory to read it, so the port's
    catalog must be in the C++ key space - which is the only catalog the port
    keeps. Without it the reference opens an empty catalog and the comparison
    is meaningless, so the caller skips it.
    """
    _wait_port_free()
    config = os.path.join(root, "reader.toml")
    with open(config, "w") as handle:
        handle.write(_reader_config(root, pg_port))
    proc = subprocess.Popen(
        [CPP_BIN, f"--config={config}"],
        stdout=open(os.path.join(root, "catalog.log"), "w"),
        stderr=subprocess.STDOUT,
        start_new_session=True,
    )
    try:
        for _ in range(60):
            if proc.poll() is not None:
                raise AssertionError(f"engine exited:\n{_log_tail(root)}")
            break
            time.sleep(0.5)
        time.sleep(3)
        out = subprocess.run(
            [
                "psql",
                "-h",
                "127.0.0.1",
                "-p",
                str(pg_port),
                "-U",
                "admin",
                "-d",
                "default_db",
                "-c",
                "show catalog 'all'",
            ],
            capture_output=True,
            text=True,
        ).stdout
    finally:
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        except (ProcessLookupError, PermissionError):
            pass
        try:
            proc.wait(timeout=30)
        except subprocess.TimeoutExpired:
            pass
        time.sleep(1)
        _wait_port_free()
    pairs = {}
    for line in out.splitlines():
        match = re.match(r"\s*(\S.*?)\s*\|\s(.*)$", line)
        if not match:
            continue
        key, value = match.group(1), match.group(2)
        if key.startswith(("(", "-")):
            continue
        pairs[key] = value
    return pairs


def _index_answers(root, pg_port):
    """What the reference answers when it reads the index chunks in `root`.

    The index chunks that are not byte comparable (the HNSW graph) and the
    ones that are comparable but still differ in their *content* (the
    secondary and sparse chunks) are checked here instead: the reference is
    started on the directory and every index is queried. Two directories whose
    indexes answer the same are logically equivalent, whatever their bytes.

    The queries are chosen to exercise the index rather than the table: a
    full-text match, a vector search, a sparse search, an equality and a
    range on a secondary index.
    """
    _wait_port_free()
    config = os.path.join(root, "reader.toml")
    with open(config, "w") as handle:
        handle.write(_reader_config(root, pg_port))
    proc = subprocess.Popen(
        [CPP_BIN, f"--config={config}"],
        stdout=open(os.path.join(root, "answers.log"), "w"),
        stderr=subprocess.STDOUT,
        start_new_session=True,
    )
    conn = None
    try:
        for _ in range(120):
            if proc.poll() is not None:
                raise AssertionError(f"engine exited:\n{_log_tail(root)}")
            try:
                conn = infinity.connect(URI)
                # Readiness needs a round-trip, not just a TCP connect (see
                # `_run_engine`).
                conn.get_database("default_db").list_tables()
                break
            except Exception:
                time.sleep(0.5)
        else:
            raise AssertionError(f"the engine did not come up on {root}")
        table = conn.get_database("default_db").get_table(TABLE)

        def ids(result):
            data, _, _ = result
            key = next(k for k in data if k != "SCORE" and k != "DISTANCE")
            return sorted(data[key])

        return {
            # The full-text index.
            "fulltext": ids(table.output(["id"]).match_text("body", "alpha", 10).to_result()),
            # The HNSW index: the graph is built, so only the *answer* is
            # comparable - and it must be the same one.
            "hnsw": ids(
                table.output(["id"])
                .match_dense("vec", [0.0, 0.1, 0.2, 0.3], "float", "l2", 5)
                .to_result()
            ),
            # The sparse (BMP) index.
            "sparse": ids(
                table.output(["id"]).match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5).to_result()
            ),
            # The secondary index on a varchar column (an equality) and on a
            # float column (an equality and a range).
            "secondary_eq": ids(table.output(["id"]).filter("title = 'doc-7'").to_result()),
            "secondary_float_eq": ids(
                table.output(["id"]).filter("score = 2.5").to_result()
            ),
            "secondary_float_range": ids(
                table.output(["id"]).filter("score >= 1.5 and score <= 3.5").to_result()
            ),
        }
    finally:
        if conn is not None:
            try:
                conn.disconnect()
            except Exception:
                pass
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        except (ProcessLookupError, PermissionError):
            pass
        try:
            proc.wait(timeout=30)
        except subprocess.TimeoutExpired:
            pass
        time.sleep(1)
        _wait_port_free()


def _table_records(pairs, table_dir, table):
    """Only the catalog records that belong to `table_dir`'s table.

    Another process can connect to the engine's fixed Thrift port and create a
    table of its own (see `_own_table_dir`); its records are not this test's
    input. The table's id comes from the directory the engine wrote, so the
    filter is exact rather than name-based. `db|{n}|next_table_id` is dropped as
    well: it counts every table in the database, not just this one.
    """
    db_id, table_id = table_dir.split("/")
    db_id = db_id[len("db_") :]
    table_id = table_id[len("tbl_") :]
    out = {}
    for key, value in pairs.items():
        parts = key.split("|")
        head = parts[0]
        if head == "catalog" and len(parts) > 1:
            kind = parts[1]
            if kind == "tbl":
                # `catalog|tbl|{db}|{name}|{ts}`
                if len(parts) > 3 and parts[3] == table:
                    out[key] = value
            elif kind in ("blk", "idx", "seg", "col"):
                # `catalog|{blk|idx|seg|col}|{db}|{table}|...`
                if len(parts) > 3 and parts[2] == db_id and parts[3] == table_id:
                    out[key] = value
            elif kind == "db":
                out[key] = value
            continue
        if head == "idx_chunk":
            # `idx_chunk|{db}|{table}|...`
            if len(parts) > 2 and parts[1] == db_id and parts[2] == table_id:
                out[key] = value
        elif head == "seg":
            # `seg|{db}|{table}|{segment}|{tag}`: `first_delete_ts` is the
            # watermark a reader uses to decide whether to look at the block's
            # delete flags at all (`SegmentMeta::CommitFirstDeleteTS`).
            if len(parts) > 3 and parts[1] == db_id and parts[2] == table_id:
                out[key] = value
        elif head.startswith("table_index_tag:"):
            # `table_index_tag: idx|{db}|{table}|...`
            if len(parts) > 2 and parts[1] == db_id and parts[2] == table_id:
                out[key] = value
        elif head == "drop":
            # `drop|seg|{db}/{table}/{segment}` and
            # `drop|idx_seg|{db}/{table}/{index}/{segment}`: a compaction's
            # deprecation marks, keyed by ids rather than by name.
            if len(parts) > 2 and parts[2].startswith(f"{db_id}/{table_id}/") or table in key:
                out[key] = value
    return out


def _normalize_catalog(pairs, zero_size_indexes=(), drop_chunk_info=False):
    """The catalog's records, keys compared verbatim.

    The keys of the ts-stamped records (`catalog|tbl|{db}|{name}|{ts}`,
    `catalog|idx|{db}|{tbl}|{name}|{ts}`, `tbl|col|{db}|{tbl}|{name}|{ts}`)
    embed the commit ts of the transaction that wrote them, and the two
    engines produce the same ts for the same statements, so the key itself is
    compared. Every bare-ts value is compared too, including
    `idx_chunk|…|{chunk_id}`: with the background tasks off both engines dump
    at the same points, so the chunk records name the same transactions.

    `index_size` is also taken out of the chunk records of the indexes in
    `zero_size_indexes`: the reference stores `GetSizeInBytes()` of the
    in-memory index it dumps, which for HNSW sums the *graph's* neighbour
    counts (`GraphStoreInner::GetSizeInBytes`, `graph_store.cppm:221`) and for
    BMP the in-memory `BMPAlg`'s vector capacities. Both are properties of the
    built structure, not of the format - they differ per build just like the
    graph itself - and a reader treats zero as "use the file's own size"
    (`HnswFileWorker::HnswFileWorker`, `hnsw_file_worker_impl.cpp:55`), so the
    port writes zero.

    `drop_chunk_info` removes the `idx_chunk|…` records: a chunk's
    `base_row_id`/`row_count` are the *boundaries* the dump happened to land
    on, and those are a scheduling outcome (see
    `test_the_chunk_boundaries_are_timing_dependent`). How *many* chunks a
    segment ends up with follows from the same boundaries - the reference
    dumps only the rows added since its last dump, the port rewrites the whole
    segment - so the per-chunk ts records go with them. A test that wants the
    chunks checked must assert the tiling invariant itself rather than compare
    these records to the reference's (`_assert_chunk_tiling`).
    """
    out = {}
    for key, value in pairs.items():
        if drop_chunk_info and key.startswith("idx_chunk|"):
            continue
        if zero_size_indexes and key.endswith("|chunk_info"):
            parts = key.split("|")
            # `idx_chunk|{db}|{table}|{index}|{segment}|{chunk}|chunk_info`
            if len(parts) > 3 and parts[3].isdigit():
                if int(parts[3]) in zero_size_indexes:
                    try:
                        record = json.loads(value)
                    except ValueError:
                        record = None
                    if record is not None and "index_size" in record:
                        record["index_size"] = 0
                        value = json.dumps(record, sort_keys=True, separators=(",", ":"))
        out[key] = value
    return out


def _assert_chunk_tiling(pairs, index_ids, expected_rows, engine):
    """Every index's chunk infos tile `[0, expected_rows)` exactly once.

    Chunk *boundaries* are a scheduling outcome (see
    `test_the_chunk_boundaries_are_timing_dependent`), so two engines may split
    the same rows 100+100+10 and 210. What both must satisfy is the invariant a
    reader relies on: chunk `k`'s `base_row_id` equals the end of chunk `k-1`,
    starting at zero, and the last one ends at the table's row count.
    """
    coverage = {}
    for key, value in pairs.items():
        if not key.startswith("idx_chunk|") or not key.endswith("|chunk_info"):
            continue
        parts = key.split("|")
        # `idx_chunk|{db}|{table}|{index}|{segment}|{chunk}|chunk_info`
        record = json.loads(value)
        coverage.setdefault(int(parts[3]), []).append(
            (record["base_row_id"], record["row_count"])
        )
    assert set(coverage) == set(index_ids), (
        f"{engine}: indexes with chunks {sorted(coverage)}, expected {sorted(index_ids)}"
    )
    for index_id, spans in coverage.items():
        spans.sort()
        covered = 0
        for base, rows in spans:
            assert base == covered, f"{engine} idx_{index_id}: gap at {covered}: {spans}"
            covered += rows
        assert covered == expected_rows, (
            f"{engine} idx_{index_id}: covers {covered} of {expected_rows} rows"
        )


def _answers(root, pg_port, table, queries):
    """Run `queries` against `table` with the reference reading `root`.

    Same contract as `_index_answers` - what the chunks *say* must not depend
    on which engine wrote them - but parameterised by table and query set so
    the vector and double-key cases below can use it.
    """
    _wait_port_free()
    config = os.path.join(root, "reader.toml")
    with open(config, "w") as handle:
        handle.write(_reader_config(root, pg_port))
    proc = subprocess.Popen(
        [CPP_BIN, f"--config={config}"],
        stdout=open(os.path.join(root, "answers.log"), "w"),
        stderr=subprocess.STDOUT,
        start_new_session=True,
    )
    conn = None
    try:
        for _ in range(120):
            if proc.poll() is not None:
                raise AssertionError(f"engine exited:\n{_log_tail(root)}")
            try:
                conn = infinity.connect(URI)
                conn.get_database("default_db").list_tables()
                break
            except Exception:
                if conn is not None:
                    try:
                        conn.disconnect()
                    except Exception:
                        pass
                    conn = None
                time.sleep(0.5)
        else:
            raise AssertionError(f"the engine did not come up on {root}")

        handle = conn.get_database("default_db").get_table(table)

        def ids(result):
            data, _, _ = result
            key = next(k for k in data if k not in ("SCORE", "DISTANCE"))
            return sorted(data[key])

        return {name: ids(query(handle)) for name, query in queries.items()}
    finally:
        if conn is not None:
            try:
                conn.disconnect()
            except Exception:
                pass
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        except (ProcessLookupError, PermissionError):
            pass
        try:
            proc.wait(timeout=30)
        except subprocess.TimeoutExpired:
            pass
        time.sleep(1)
        _wait_port_free()


CRASH_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}}


SEG_TABLE = "test_cross_engine_segments"
SEG_ROWS = (100, 60, 40)
SEG_STARTS = tuple(sum(SEG_ROWS[:i]) for i in range(len(SEG_ROWS)))
def _seg_words(k):
    """Repeated short English words: a tiny term dictionary at two segments.

    The vocabulary is 5 words plus one `doc{k}` token per row, so both
    segments' dictionaries fit in memory and the cross-segment merge of a term
    that appears in both is what the test observes.
    """
    words = ["the", "quick", "brown", "fox", "jumps"]
    return " ".join(words[k % 5 :] + words[: k % 5]) + f" doc{k}"


def _seg_csv(path, base, count):
    """`id,title,body,vec,sp,score` — the schema `_seg_phase` creates."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as handle:
        for i in range(count):
            k = base + i
            handle.write(
                f"{k},title-{k},{_seg_words(k)},"
                f'"[{k / 100.0}, 0.1, 0.2, 0.3]",'
                f'"[0:0.5,{k % 5 + 1}:1.0]",'
                f"{k * 0.5}\n"
            )


def _seg_phase(conn, directory):
    db = conn.get_database("default_db")
    db.drop_table(SEG_TABLE, ConflictType.Ignore)
    t = db.create_table(
        SEG_TABLE,
        {
            "id": {"type": "int"},
            "title": {"type": "varchar"},
            "body": {"type": "varchar"},
            "vec": {"type": "vector,4,float"},
            "sp": {"type": "sparse,8,float,int"},
            "score": {"type": "float"},
        },
        ConflictType.Error,
    )
    t.create_index("ft", index.IndexInfo("body", index.IndexType.FullText))
    t.create_index(
        "vec_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "l2"}
        ),
    )
    t.create_index("ivf_idx", index.IndexInfo("vec", index.IndexType.IVF, {"metric": "l2"}))
    t.create_index("sec", index.IndexInfo("title", index.IndexType.Secondary))
    t.create_index("score_idx", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    t.create_index("id_sec", index.IndexInfo("id", index.IndexType.Secondary))
    # One import per segment (three here). `COPY` is the reference's import path, so the
    # indexes are populated inside each import transaction (`PopulateIndex`)
    # rather than by a dump: no background worker takes part, and the chunks
    # exist as soon as `flush_data()` returns.
    #
    # One thread, one file at a time: the rows and the chunk boundaries are a
    # function of this script alone, which is what the byte comparison needs
    # (concurrent `INSERT`s would interleave the segments by commit order).
    for name in ("a.csv", "b.csv", "c.csv"):
        t.import_data(os.path.join(directory, name))
    conn.flush_data()
    time.sleep(2)


BLOCK_SEG_TABLE = "test_cross_engine_block_segment"
BLOCK_SEG_ROWS = 20000
BLOCK_SEG_BLOCKS = 3
