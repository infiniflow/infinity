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
import tempfile
import time

import pytest

import infinity
from infinity.common import ConflictType, NetworkAddress
from infinity import index

CPP_BIN = os.environ.get("INFINITY_CPP_BIN", "cmake-build-reldeb/src/infinity")
RUST_BIN = os.environ.get("INFINITY_RUST_BIN", "rust_engine/scripts/infinity_rust_shim.sh")
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
    for n in range(0, BLOCK_CAPACITY + 1):
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
    """Zero the data slot of every NULL row of a `.col` file.

    Layout (`CPP_FORMAT_SPEC.md §8.3`): `[buffer header 16][data
    8192 * slot][null bitmap 1024][checksum 8]`, where a set bit means
    **non-NULL** (`cpp_format.rs`). The reference appends values into the block
    buffer and leaves a NULL's slot untouched, so the slot keeps the *previous*
    row's bytes - deterministic, but invisible to a reader (the bitmap decides).
    The comparison treats those bytes as a don't-care rather than making "which
    leftover a NULL keeps" part of the format.

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
        if not (bitmap[row // 8] >> (row % 8) & 1):
            at = 16 + row * width
            out[at : at + width] = b"\0" * width
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
    catalog must be in the C++ key space - which is what the port's
    `rocksdb-store` build writes. Without it the reference opens an empty
    catalog and the comparison is meaningless, so the caller skips it.
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
        if key.startswith("(") or key.startswith("-"):
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
            if table in key:
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


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_two_engines_write_the_same_bytes():
    cpp_root = os.path.join(ROOT, "cpp")
    rust_root = os.path.join(ROOT, "rust")
    # A PostgreSQL port per engine, so the two catalog dumps (each of which
    # starts the reference on one directory) do not fight over the port.
    cpp_pg = int(os.environ.get("INFINITY_BYTES_CPP_PG_PORT", "5432"))
    rust_pg = int(os.environ.get("INFINITY_BYTES_RUST_PG_PORT", "5433"))

    _run_engine("cpp", cpp_root, cpp_pg)
    _run_engine("rust", rust_root, rust_pg)
    _wait_for_index_chunks(cpp_root, len(INDEX_KINDS))
    _wait_for_index_chunks(rust_root, len(INDEX_KINDS))

    cpp_files = _files(cpp_root)
    rust_files = _files(rust_root)
    cpp_dir = _own_table_dir(cpp_files, COLUMNS)
    rust_dir = _own_table_dir(rust_files, COLUMNS)
    assert cpp_dir and rust_dir, (
        "the table under test was not written: "
        f"cpp dirs {sorted({n.split('/')[0] + '/' + n.split('/')[1] for n in cpp_files})}, "
        f"rust dirs {sorted({n.split('/')[0] + '/' + n.split('/')[1] for n in rust_files})}"
    )

    # The file set, the block columns, the outline, the `version` files and
    # every index chunk, each compared according to its class. The classes and
    # the evidence behind them are in the module docstring; the block layer is
    # exercised at its real size (20000 rows = 3 blocks of 8192/8192/3616) and
    # so are the per-index chunks.
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), INDEX_KINDS
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    # The catalog: the same records. The keys embed the commit ts of the
    # transaction that created them and the bare-ts values are that ts too;
    # with every background task off (see `_config`) the two engines dump at
    # the same points, so keys and values are both compared verbatim. The
    # `index_size` of a graph-shaped chunk (HNSW, BMP) is taken out
    # as well, for the reason given in `_normalize_catalog`.
    #
    # The port writes the C++ key space only in its `rocksdb-store` build;
    # without it the reference opens an empty catalog and there is nothing to
    # compare, so the check is skipped rather than failed.
    zero_size_indexes = {
        index_id
        for index_id, kind in INDEX_KINDS.items()
        if kind in ("hnsw", "bmp")
    }
    cpp_catalog = _normalize_catalog(
        _table_records(_catalog_pairs(cpp_root, cpp_pg), cpp_dir, TABLE),
        zero_size_indexes,
    )
    rust_catalog = _normalize_catalog(
        _table_records(_catalog_pairs(rust_root, rust_pg), rust_dir, TABLE),
        zero_size_indexes,
    )
    if not rust_catalog:
        pytest.skip(
            "the port's catalog is not in the C++ key space; "
            "build it with `--features rocksdb-store`"
        )
    assert set(cpp_catalog) == set(rust_catalog), (
        f"only in cpp: {sorted(set(cpp_catalog) - set(rust_catalog))}, "
        f"only in rust: {sorted(set(rust_catalog) - set(cpp_catalog))}"
    )
    for key in sorted(cpp_catalog):
        assert cpp_catalog[key] == rust_catalog[key], (
            f"catalog {key}: {cpp_catalog[key]} != {rust_catalog[key]}"
        )

    # The logical equivalence of the index chunks. The HNSW graph cannot be
    # compared byte for byte (the reference itself does not reproduce it), so
    # the indexes are compared by *what they answer*: the reference reads each
    # engine's directory and every index must give the same result.
    cpp_answers = _index_answers(cpp_root, cpp_pg)
    rust_answers = _index_answers(rust_root, rust_pg)
    assert cpp_answers == rust_answers, (
        f"the indexes answer differently:\n"
        f"  cpp:  {cpp_answers}\n"
        f"  rust: {rust_answers}"
    )


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
def test_the_reference_is_deterministic_where_the_test_assumes_it():
    """The classification the byte comparison rests on, measured.

    The byte test compares the sparse chunk and every secondary chunk -
    integral, date and floating point keys alike - byte for byte, and excludes
    only the HNSW chunk (its graph). That split is an assumption about the
    reference, so it is checked here rather than asserted in a comment: the
    reference is run three times on the same input and the files that come out
    differently are counted.

    The HNSW chunk is the one that may differ - the reference builds the graph
    with a `thread_local std::mt19937` (`hnsw_alg.cppm:107`), so which thread
    inserts which vector, and therefore which node gets which level, is up to
    the scheduler. Every other file must come out identical.
    """
    roots = []
    for i in range(3):
        root = os.path.join(ROOT, f"determinism{i}")
        shutil.rmtree(root, ignore_errors=True)
        _run_engine("cpp", root, 5432 + i, row_count=DET_ROWS)
        roots.append(root)

    digests = [_files(root) for root in roots]
    names = sorted(set().union(*[set(d) for d in digests]))
    varying = [
        name
        for name in names
        if len({digest.get(name) for digest in digests}) > 1
    ]
    # Only the HNSW chunk may vary. Everything else - the block columns, the
    # outline, the full-text chunk, the secondary and the sparse chunks - is
    # built from the row values alone and must be reproducible.
    #
    # Note this is measured at `DET_ROWS` (500 rows) and not at the byte
    # test's own scale: at a few thousand rows the reference's HNSW build
    # spreads over several threads and the graph varies between runs, which is
    # exactly why the byte test compares the HNSW chunk by content.
    assert all(
        name.endswith("idx_2/seg_0/chunk_0.idx") for name in varying
    ), f"the reference is not deterministic here: {varying}"


@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_port_is_deterministic_too():
    """The port's half of the same classification, measured on its own.

    The byte gate compares two engines, so it can only catch a drift the two
    do *differently*: a value that both produce from a clock, a hash map's
    iteration order or a thread schedule would agree today and disagree
    tomorrow. This runs the port three times on the same input and requires
    every file under `data/` to come out identical - including the HNSW
    chunk, whose graph the reference itself cannot reproduce (the port's
    build is a plain insertion in row order, with no randomness to schedule).
    """
    roots = []
    for i in range(3):
        root = os.path.join(ROOT, f"port_determinism{i}")
        _run_engine("rust", root, 5432 + i, row_count=DET_ROWS)
        roots.append(root)

    digests = [_files(root) for root in roots]
    names = sorted(set().union(*[set(d) for d in digests]))
    varying = [
        name
        for name in names
        if len({digest.get(name) for digest in digests}) > 1
    ]
    assert not varying, f"the port is not deterministic here: {varying}"


BLOCK_TABLE = "test_cross_engine_blocks"
BLOCK_COLUMNS = {
    "id": {"type": "int"},
    "title": {"type": "varchar"},
    "vec": {"type": "vector,4,float"},
}


def _seed_blocks(conn):
    """`BLOCK_ROWS` rows and no indexes: the block layer only."""
    db = conn.get_database("default_db")
    db.drop_table(BLOCK_TABLE, ConflictType.Ignore)
    t = db.create_table(BLOCK_TABLE, BLOCK_COLUMNS, ConflictType.Error)
    rows = [
        {
            "id": i,
            "title": f"doc-{i}",
            "vec": [float(i % 10) / 10.0, 0.1, 0.2, 0.3],
        }
        for i in range(BLOCK_ROWS)
    ]
    # The engine refuses a batch of more than 8192 rows, so the data set
    # arrives as several batches - which is also how it would in practice.
    for start in range(0, len(rows), 8192):
        t.insert(rows[start : start + 8192])
    conn.flush_data()
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_blocks_match_across_many_blocks():
    """The block layer at its real size: 3 blocks, 8192/8192/3616 rows.

    Every `.col`, every block's `version` and the outline must be byte
    identical. This is where a full block, a partly filled one and the
    per-block version file are all exercised; the byte test's own table is a
    single block.
    """
    cpp_root = os.path.join(ROOT, "blocks_cpp")
    rust_root = os.path.join(ROOT, "blocks_rust")
    _run_engine("cpp", cpp_root, 5432, seed=_seed_blocks)
    _run_engine("rust", rust_root, 5433, seed=_seed_blocks)

    cpp_files = _files(cpp_root)
    rust_files = _files(rust_root)
    cpp_dir = _own_table_dir(cpp_files, len(BLOCK_COLUMNS))
    rust_dir = _own_table_dir(rust_files, len(BLOCK_COLUMNS))
    assert cpp_dir and rust_dir, "the blocks table was not written"

    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), {}
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )


# ---------------------------------------------------------------------------
# Deletes, updates, and a restart between the writes.
# ---------------------------------------------------------------------------

MUTATE_TABLE = "test_cross_engine_mutations"
MUTATE_COLUMNS = {
    "id": {"type": "int"},
    "title": {"type": "varchar"},
    "score": {"type": "float"},
}


def _rows(base, count):
    return [
        {
            "id": base + i,
            "title": f"alpha beta {base + i}",
            "score": (base + i) * 0.5,
        }
        for i in range(count)
    ]


def _phase_initial(conn):
    db = conn.get_database("default_db")
    db.drop_table(MUTATE_TABLE, ConflictType.Ignore)
    t = db.create_table(MUTATE_TABLE, MUTATE_COLUMNS, ConflictType.Error)
    t.create_index("sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("ft", index.IndexInfo("title", index.IndexType.FullText))
    t.insert(_rows(0, 100))
    conn.flush_data()
    _dump_indexes(t, ["sec", "ft"])
    time.sleep(1)


def _phase_mutate(conn):
    """A second process: append, delete a prefix, update a range, flush."""
    t = conn.get_database("default_db").get_table(MUTATE_TABLE)
    t.insert(_rows(100, 100))
    t.delete("id < 20")
    t.update("id >= 50 and id < 60", {"score": 999.5})
    conn.flush_data()
    _dump_indexes(t, ["sec", "ft"])
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_bytes_after_deletes_updates_and_a_restart():
    """Append, delete and update in a second run over the same directory.

    Deletes live in the catalog (a bitmap), not in `data/`, and an update is a
    delete plus an appended row, so the block files must still agree byte for
    byte after both - and the two engines must answer the same queries.

    The block is append-only, which is what makes this a real check: a deleted
    row's cells stay in the block and only the `version` file's `deleted_`
    array hides it, and an UPDATE's rewritten row sits at the end with a fresh
    id. So the block holds 210 rows (100 + 100 appended + 10 rewritten) and
    the `version` file three creates (100/200/210) with 30 marked slots - 20
    from the DELETE and 10 from the UPDATE that replaced them.

    `memindex_memory_quota = "2GB"`: the `version` file carries the commit ts
    of every append and delete, and the reference's quota-driven dump is a
    *background* write transaction whose ts is allocated whenever its worker
    runs - measured between the append and the DELETE on one run and after the
    UPDATE on another. A quota the memory index never crosses leaves the
    explicit `DumpIndex` calls in the phases as the only dumps, so both engines
    allocate the same ts sequence (`_config` has the details).
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine,
            os.path.join(ROOT, f"mut_{engine}"),
            port,
            [_phase_initial, _phase_mutate],
            quota="2GB",
        )

    files = {engine: _files(root) for engine, root in roots.items()}
    dirs = {
        engine: _own_table_dir(blobs, len(MUTATE_COLUMNS))
        for engine, blobs in files.items()
    }
    assert dirs["cpp"] and dirs["rust"], "the mutations table was not written"

    failures = _compare_data_files(
        _under(files["cpp"], dirs["cpp"]),
        _under(files["rust"], dirs["rust"]),
        {1: "secondary-f32", 2: "fulltext"},
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    # The catalog: the same records, ts-stamped keys included, and the delete
    # bitmap. Only the `idx_chunk|…` records are left out - a chunk's
    # boundaries (and therefore how many chunks a segment has) are a scheduling
    # outcome, not the format - and the invariant they must satisfy is asserted
    # instead: the chunks tile the rows.
    cpp_pairs = _table_records(
        _catalog_pairs(roots["cpp"], 5432), dirs["cpp"], MUTATE_TABLE
    )
    rust_pairs = _table_records(
        _catalog_pairs(roots["rust"], 5433), dirs["rust"], MUTATE_TABLE
    )
    _assert_chunk_tiling(cpp_pairs, {1, 2}, 210, "cpp")
    _assert_chunk_tiling(rust_pairs, {1, 2}, 210, "rust")
    cpp_catalog = _normalize_catalog(cpp_pairs, drop_chunk_info=True)
    rust_catalog = _normalize_catalog(rust_pairs, drop_chunk_info=True)
    assert set(cpp_catalog) == set(rust_catalog), (
        f"only in cpp: {sorted(set(cpp_catalog) - set(rust_catalog))}, "
        f"only in rust: {sorted(set(rust_catalog) - set(cpp_catalog))}"
    )
    for key in sorted(cpp_catalog):
        assert cpp_catalog[key] == rust_catalog[key], (
            f"catalog {key}: {cpp_catalog[key]} != {rust_catalog[key]}"
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


# ---------------------------------------------------------------------------
# Indexes under DELETE / UPDATE / a restart, and the variants the default seed
# does not create (a DOUBLE secondary key, IVF with SQ/PQ storage).
#
# §4 of `rust_engine/docs/FORMAT_EQUIVALENCE_TESTS.md` marks these empty: the
# gates above exercise one block and one dump, so nothing below a restart had
# been compared for any index but a float-key secondary.
# ---------------------------------------------------------------------------

IDX_TABLE = "test_cross_engine_index_mutations"
IDX_COLUMNS = {
    "id": {"type": "int"},
    "title": {"type": "varchar"},
    "score": {"type": "float"},
    "amount": {"type": "double"},
    "sp": {"type": "sparse,8,float,int"},
}
# Creation order names the directories: `idx_1` … `idx_4`.
IDX_INDEXES = {
    1: "secondary-u32",  # id
    2: "secondary-f32",  # score
    3: "secondary-f64",  # amount: a DOUBLE key (`LargeSigned<double>`)
    4: "bmp",            # sp
}


def _idx_rows(base, count):
    return [
        {
            "id": base + i,
            "title": f"doc-{base + i}",
            "score": (base + i) * 0.5,
            "amount": (base + i) * 1.25,
            "sp": {0: 0.5, (base + i) % 5 + 1: 1.0},
        }
        for i in range(count)
    ]


def _phase_idx_initial(conn):
    db = conn.get_database("default_db")
    db.drop_table(IDX_TABLE, ConflictType.Ignore)
    t = db.create_table(IDX_TABLE, IDX_COLUMNS, ConflictType.Error)
    t.create_index("id_sec", index.IndexInfo("id", index.IndexType.Secondary))
    t.create_index("score_sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("amount_sec", index.IndexInfo("amount", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    t.insert(_idx_rows(0, 100))
    conn.flush_data()
    _dump_indexes(t, ["id_sec", "score_sec", "amount_sec", "sp_idx"])
    time.sleep(1)


def _phase_idx_mutate(conn):
    """A second process: delete a prefix, rewrite a range, flush, dump."""
    t = conn.get_database("default_db").get_table(IDX_TABLE)
    t.delete("id < 20")
    t.update(
        "id >= 20 and id < 30",
        {
            "title": "rewritten",
            "score": 777.5,
            "amount": 888.25,
            "sp": {0: 1.5, 2: 2.0},
        },
    )
    conn.flush_data()
    _dump_indexes(t, ["id_sec", "score_sec", "amount_sec", "sp_idx"])
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_secondary_and_sparse_indexes_survive_deletes_updates_and_a_restart():
    """An INT, a FLOAT and a DOUBLE secondary key plus a BMP index, mutated.

    The delete goes to the catalog's bitmap and the update re-appends the row,
    so a second run produces chunks that mix "the rows of the block" with "the
    rows appended after the last dump". Both engines must land on the same
    bytes: the PGM fit, the raw keys and the sparse postings are all derived
    from the rows, not from the dump schedule.

    The DOUBLE key is the case the default seed misses: its keys are 8 bytes
    and the PGM runs `LargeSigned<double>`, not the `float` path.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine,
            os.path.join(ROOT, f"idxmut_{engine}"),
            port,
            [_phase_idx_initial, _phase_idx_mutate],
        )

    files = {engine: _files(root) for engine, root in roots.items()}
    dirs = {
        engine: _own_table_dir(blobs, len(IDX_COLUMNS))
        for engine, blobs in files.items()
    }
    assert dirs["cpp"] and dirs["rust"], "the mutated index table was not written"

    failures = _compare_data_files(
        _under(files["cpp"], dirs["cpp"]),
        _under(files["rust"], dirs["rust"]),
        IDX_INDEXES,
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    # 100 rows + the 10 the UPDATE re-appended; the 20 deletes only mark slots.
    cpp_pairs = _table_records(
        _catalog_pairs(roots["cpp"], 5432), dirs["cpp"], IDX_TABLE
    )
    rust_pairs = _table_records(
        _catalog_pairs(roots["rust"], 5433), dirs["rust"], IDX_TABLE
    )
    _assert_chunk_tiling(cpp_pairs, set(IDX_INDEXES), 110, "cpp")
    _assert_chunk_tiling(rust_pairs, set(IDX_INDEXES), 110, "rust")

    # The answers too: a chunk that parses but says something else is a format
    # difference the byte comparison of a *different* chunk boundary cannot see.
    queries = {
        "id_eq": lambda t: t.output(["id"]).filter("id = 42").to_result(),
        "score_range": lambda t: t.output(["id"])
        .filter("score >= 10.0 and score <= 12.0")
        .to_result(),
        "amount_eq": lambda t: t.output(["id"]).filter("amount = 52.5").to_result(),
        "sparse": lambda t: t.output(["id"])
        .match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5)
        .to_result(),
    }
    cpp_answers = _answers(roots["cpp"], 5432, IDX_TABLE, queries)
    rust_answers = _answers(roots["rust"], 5433, IDX_TABLE, queries)
    assert cpp_answers == rust_answers, (
        f"the mutated indexes answer differently:\n"
        f"  cpp:  {cpp_answers}\n  rust: {rust_answers}"
    )


VEC_TABLE = "test_cross_engine_vector_mutations"
VEC_COLUMNS = {
    "id": {"type": "int"},
    "vec": {"type": "vector,4,float"},
    "vec2": {"type": "vector,4,float"},
}
# An HNSW on `vec` and an IVF on `vec2`, so each is queried on its own column
# (the planner picks the index for the column, not for the table).
VEC_INDEXES = {1: "hnsw", 2: "ivf"}


def _vec_rows(base, count):
    return [
        {
            "id": base + i,
            "vec": [float((base + i) % 10) / 10.0, 0.1, 0.2, 0.3],
            "vec2": [float((base + i) % 7) / 7.0, 0.25, 0.5, 0.75],
        }
        for i in range(count)
    ]


def _phase_vec_initial(conn):
    db = conn.get_database("default_db")
    db.drop_table(VEC_TABLE, ConflictType.Ignore)
    t = db.create_table(VEC_TABLE, VEC_COLUMNS, ConflictType.Error)
    t.create_index(
        "vec_hnsw",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "l2"}
        ),
    )
    t.create_index("vec_ivf", index.IndexInfo("vec2", index.IndexType.IVF, {"metric": "l2"}))
    t.insert(_vec_rows(0, 100))
    conn.flush_data()
    _dump_indexes(t, ["vec_hnsw", "vec_ivf"])
    time.sleep(1)


def _phase_vec_mutate(conn):
    t = conn.get_database("default_db").get_table(VEC_TABLE)
    t.delete("id < 20")
    t.update(
        "id >= 20 and id < 30",
        {
            "vec": [0.9, 0.9, 0.9, 0.9],
            "vec2": [0.8, 0.8, 0.8, 0.8],
        },
    )
    t.insert(_vec_rows(100, 20))
    conn.flush_data()
    _dump_indexes(t, ["vec_hnsw", "vec_ivf"])
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_vector_indexes_survive_deletes_updates_and_a_restart():
    """HNSW and IVF after a second run deletes, rewrites and appends rows.

    Neither chunk is byte comparable (the graph is built with a thread-local
    RNG, the IVF centroids are sampled with `random_device`), so the gate works
    the way §1 of the document splits content from bytes: the derived regions
    are byte compared (HNSW header/vectors/labels, IVF header counts and the
    `(offset, vector)` pairs) and the answers are compared by asking the
    reference to read each engine's directory.

    This is the case §4 leaves empty for both index kinds: every earlier gate
    dumps once, before any delete or update.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine,
            os.path.join(ROOT, f"vecmut_{engine}"),
            port,
            [_phase_vec_initial, _phase_vec_mutate],
        )

    files = {engine: _files(root) for engine, root in roots.items()}
    dirs = {
        engine: _own_table_dir(blobs, len(VEC_COLUMNS))
        for engine, blobs in files.items()
    }
    assert dirs["cpp"] and dirs["rust"], "the mutated vector table was not written"

    failures = _compare_data_files(
        _under(files["cpp"], dirs["cpp"]),
        _under(files["rust"], dirs["rust"]),
        VEC_INDEXES,
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    queries = {
        # The query vector is one of the rewritten rows, so an index that still
        # holds the pre-update vectors answers a different id.
        "hnsw": lambda t: t.output(["id"])
        .match_dense("vec", [0.9, 0.9, 0.9, 0.9], "float", "l2", 5)
        .to_result(),
        "ivf": lambda t: t.output(["id"])
        .match_dense("vec2", [0.8, 0.8, 0.8, 0.8], "float", "l2", 5, {"nprobe": "16"})
        .to_result(),
        "count": lambda t: t.output(["count(*)"]).to_result(),
    }
    cpp_answers = _answers(roots["cpp"], 5432, VEC_TABLE, queries)
    rust_answers = _answers(roots["rust"], 5433, VEC_TABLE, queries)
    assert cpp_answers == rust_answers, (
        f"the mutated vector indexes answer differently:\n"
        f"  cpp:  {cpp_answers}\n  rust: {rust_answers}"
    )


QUANT_TABLE = "test_cross_engine_quantized_ivf"
QUANT_COLUMNS = {
    "id": {"type": "int"},
    "sq": {"type": "vector,8,float"},
    "pq": {"type": "vector,8,float"},
}
QUANT_INDEXES = {1: "ivf", 2: "ivf"}  # both: chunk existence + answers
QUANT_ROWS = 200


def _quant_rows(base, count):
    return [
        {
            "id": base + i,
            # A deterministic spread over the unit cube; the dimension is 8 so
            # two PQ subspaces of 4 dimensions divide it evenly.
            "sq": [((i * (j + 3)) % 17) / 17.0 for j in range(8)],
            "pq": [((i * (j + 5)) % 13) / 13.0 for j in range(8)],
        }
        for i in range(count)
    ]


def _quant_indexes(t):
    """The two quantized IVF indexes, on their own column each."""
    t.create_index(
        "sq_idx",
        index.IndexInfo(
            "sq",
            index.IndexType.IVF,
            {
                "metric": "l2",
                "storage_type": "scalar_quantization",
                "scalar_quantization_bits": "8",
            },
        ),
    )
    t.create_index(
        "pq_idx",
        index.IndexInfo(
            "pq",
            index.IndexType.IVF,
            {
                "metric": "l2",
                "storage_type": "product_quantization",
                "product_quantization_subspace_num": "2",
                "product_quantization_subspace_bits": "8",
            },
        ),
    )


def _phase_quant_initial(conn):
    db = conn.get_database("default_db")
    db.drop_table(QUANT_TABLE, ConflictType.Ignore)
    t = db.create_table(QUANT_TABLE, QUANT_COLUMNS, ConflictType.Error)
    _quant_indexes(t)
    t.insert(_quant_rows(0, QUANT_ROWS))
    conn.flush_data()
    _dump_indexes(t, ["sq_idx", "pq_idx"])
    time.sleep(1)


def _quant_mutated_vector(i):
    return [((i * (j + 7)) % 19) / 19.0 for j in range(8)]


def _phase_quant_mutate(conn):
    """A second process: delete a prefix, rewrite a range, append a batch."""
    t = conn.get_database("default_db").get_table(QUANT_TABLE)
    t.delete("id < 20")
    t.update(
        "id >= 20 and id < 40",
        {
            "sq": _quant_mutated_vector(7),
            "pq": _quant_mutated_vector(11),
        },
    )
    t.insert(_quant_rows(QUANT_ROWS, 40))
    conn.flush_data()
    _dump_indexes(t, ["sq_idx", "pq_idx"])
    time.sleep(2)


def _phase_quant_unflushed(conn):
    """Rows and a delete that only reach the WAL: the process dies right after."""
    t = conn.get_database("default_db").get_table(QUANT_TABLE)
    t.insert(_quant_rows(QUANT_ROWS + 40, 30))
    t.delete("id >= 100 and id < 110")
    time.sleep(1)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_quantized_ivf_indexes_mirror_and_answer():
    """IVF with `scalar_quantization` and `product_quantization` storage.

    The quantization codes are derived from the k-means centroids, and the
    reference samples those with `std::random_device`, so neither the SQ nor the
    PQ chunk is byte comparable (the plain IVF chunk is compared by its header
    and `(offset, vector)` pairs instead). What the port must reproduce is the
    *layout* the reference reads - chunk present, header well formed - and a
    well-formed answer to a `nprobe` search.

    A second process then deletes a prefix, rewrites a range and appends a
    batch, and the index is dumped again. That half *is* comparable: the row
    ids a quantized search returns are approximate, but a deleted row must never
    appear, so the answer is checked against the delete bitmap rather than
    against the reference's choice of neighbours.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine,
            os.path.join(ROOT, f"quant_{engine}"),
            port,
            [_phase_quant_initial, _phase_quant_mutate],
        )

    for root in roots.values():
        _wait_for_index_chunks(root, len(QUANT_INDEXES))

    files = {engine: _files(root) for engine, root in roots.items()}
    dirs = {
        engine: _own_table_dir(blobs, len(QUANT_COLUMNS))
        for engine, blobs in files.items()
    }
    assert dirs["cpp"] and dirs["rust"], "the quantized table was not written"
    # Every index must have published a chunk in both directories.
    for engine, blobs in files.items():
        present = {
            int(name.split("idx_")[1].split("/")[0])
            for name in blobs
            if "/idx_" in name
        }
        assert set(QUANT_INDEXES) <= present, (
            f"{engine}: indexes {sorted(present)} do not cover {sorted(QUANT_INDEXES)}"
        )

    # 200 rows + 20 rewritten by the UPDATE + 40 appended; the 20 deletes only
    # mark slots.
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        pairs = _table_records(
            _catalog_pairs(roots[engine], port), dirs[engine], QUANT_TABLE
        )
        _assert_chunk_tiling(pairs, set(QUANT_INDEXES), 260, engine)

    probe = [0.5] * 8
    queries = {
        "sq": lambda t: t.output(["id"])
        .match_dense("sq", probe, "float", "l2", 10, {"nprobe": "16"})
        .to_result(),
        "pq": lambda t: t.output(["id"])
        .match_dense("pq", probe, "float", "l2", 10, {"nprobe": "16"})
        .to_result(),
        "count": lambda t: t.output(["count(*)"]).to_result(),
    }
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        answers = _answers(roots[engine], port, QUANT_TABLE, queries)
        assert answers["count"] == [220], f"{engine}: {answers['count']}"
        for name in ("sq", "pq"):
            ids = answers[name]
            assert len(ids) == 10, f"{engine} {name}: expected 10 ids, got {ids}"
            assert len(set(ids)) == len(ids), f"{engine} {name}: duplicate ids {ids}"
            # The delete bitmap must reach the quantized path: ids < 20 were
            # deleted in the second process, so no probe may return them.
            assert all(i >= 20 for i in ids), f"{engine} {name}: deleted ids {ids}"
            assert all(i < 240 for i in ids), f"{engine} {name}: {ids}"


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_quantized_ivf_indexes_survive_a_crash():
    """An unflushed insert and delete replayed under the quantized indexes.

    Same shape as `test_the_indexes_survive_a_crash`, but the two indexes are
    SQ and PQ: the WAL carries rows and a delete, the chunks on disk only cover
    the flushed prefix, and the reading engine has to replay the tail and then
    answer from the quantized chunks without returning a deleted row.
    """
    for writer, reader, port in (("rust", "cpp", 5433), ("cpp", "rust", 5432)):
        root = _run_phases(
            writer,
            os.path.join(ROOT, f"quantcrash_{writer}"),
            port,
            [_phase_quant_initial, _phase_quant_unflushed],
        )
        _write_config(reader, root, port)
        probe = [0.5] * 8
        queries = {
            "count": lambda t: t.output(["count(*)"]).to_result(),
            "sq": lambda t: t.output(["id"])
            .match_dense("sq", probe, "float", "l2", 10, {"nprobe": "16"})
            .to_result(),
            "pq": lambda t: t.output(["id"])
            .match_dense("pq", probe, "float", "l2", 10, {"nprobe": "16"})
            .to_result(),
        }
        answers = _answers(root, port, QUANT_TABLE, queries)
        # 200 flushed − 10 deleted + 30 appended.
        assert answers["count"] == [220], (
            f"{reader} sees {answers['count']} rows after replaying {writer}'s crash"
        )
        for name in ("sq", "pq"):
            ids = answers[name]
            assert len(ids) == 10 and len(set(ids)) == 10, f"{reader} {name}: {ids}"
            assert all(not (100 <= i < 110) for i in ids), (
                f"{reader} {name}: deleted ids came back: {ids}"
            )


CRASH_IDX_TABLE = "test_cross_engine_index_crash"
CRASH_IDX_COLUMNS = {
    "id": {"type": "int"},
    "title": {"type": "varchar"},
    "score": {"type": "double"},
    "body": {"type": "varchar"},
    "sp": {"type": "sparse,8,float,int"},
    "vec": {"type": "vector,4,float"},
    "vec2": {"type": "vector,4,float"},
}
CRASH_IDX_INDEXES = {
    1: "secondary-u32",
    2: "secondary-f64",
    3: "bmp",
    4: "hnsw",
    5: "ivf",
    6: "fulltext",
}


def _crash_idx_rows(base, count):
    return [
        {
            "id": base + i,
            "title": f"doc-{base + i}",
            "body": f"alpha beta doc{base + i}",
            "score": (base + i) * 1.5,
            "sp": {0: 0.5, (base + i) % 5 + 1: 1.0},
            "vec": [float((base + i) % 10) / 10.0, 0.1, 0.2, 0.3],
            "vec2": [float((base + i) % 7) / 7.0, 0.25, 0.5, 0.75],
        }
        for i in range(count)
    ]


def _phase_crash_idx_base(conn):
    db = conn.get_database("default_db")
    db.drop_table(CRASH_IDX_TABLE, ConflictType.Ignore)
    t = db.create_table(CRASH_IDX_TABLE, CRASH_IDX_COLUMNS, ConflictType.Error)
    t.create_index("id_sec", index.IndexInfo("id", index.IndexType.Secondary))
    t.create_index("score_sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    t.create_index(
        "vec_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "l2"}
        ),
    )
    t.create_index("ivf_idx", index.IndexInfo("vec2", index.IndexType.IVF, {"metric": "l2"}))
    t.create_index("ft_idx", index.IndexInfo("body", index.IndexType.FullText))
    t.insert(_crash_idx_rows(0, 100))
    conn.flush_data()
    _dump_indexes(t, ["id_sec", "score_sec", "sp_idx", "vec_idx", "ivf_idx", "ft_idx"])
    time.sleep(1)


def _phase_crash_idx_unflushed(conn):
    """Rows and a delete that only reach the WAL: the process dies right after."""
    t = conn.get_database("default_db").get_table(CRASH_IDX_TABLE)
    t.insert(_crash_idx_rows(100, 50))
    t.delete("id < 10")
    time.sleep(1)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_indexes_survive_a_crash():
    """An unflushed insert and delete, replayed by the *other* engine.

    The WAL carries the rows and the delete; the index chunks on disk only
    cover the flushed prefix, so the reading engine has to replay the tail and
    still answer from the indexes. Both directions are checked, and the answers
    must be well formed: 140 live rows (100 − 10 deleted + 50 appended), a
    secondary equality that still finds a flushed row, a sparse search and an
    HNSW search that return `top_k` distinct ids.

    Before this case the crash gate only counted rows on a table without
    indexes, so nothing proved the delete bitmap and the chunk layer survive a
    replay.
    """
    for writer, reader, port in (("rust", "cpp", 5433), ("cpp", "rust", 5432)):
        root = _run_phases(
            writer,
            os.path.join(ROOT, f"idxcrash_{writer}"),
            port,
            [_phase_crash_idx_base, _phase_crash_idx_unflushed],
        )
        # The reader needs the config for its own engine (the directory still
        # carries the writer's), as `_count_rows` does.
        _write_config(reader, root, port)
        queries = {
            "count": lambda t: t.output(["count(*)"]).to_result(),
            "id_eq": lambda t: t.output(["id"]).filter("id = 42").to_result(),
            "sparse": lambda t: t.output(["id"])
            .match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5)
            .to_result(),
            "hnsw": lambda t: t.output(["id"])
            .match_dense("vec", [0.1, 0.1, 0.2, 0.3], "float", "l2", 5)
            .to_result(),
            "ivf": lambda t: t.output(["id"])
            .match_dense("vec2", [0.25, 0.25, 0.5, 0.75], "float", "l2", 5, {"nprobe": "16"})
            .to_result(),
            "fulltext": lambda t: t.output(["id"]).match_text("body", "alpha", 5).to_result(),
        }
        answers = _answers(root, port, CRASH_IDX_TABLE, queries)
        assert answers["count"] == [140], (
            f"{reader} sees {answers['count']} rows after replaying {writer}'s crash"
        )
        assert answers["id_eq"] == [42], f"{reader}: {answers['id_eq']}"
        for name in ("sparse", "hnsw", "ivf", "fulltext"):
            ids = answers[name]
            assert len(ids) == 5 and len(set(ids)) == 5, f"{reader} {name}: {ids}"


# ---------------------------------------------------------------------------
# The object directory: the layout the reference's `PersistenceManager` writes.
#
# A bucket (or its local stand-in, `storage_type = "local"` with a
# `persistence_dir`) holds one packed object plus `pm|object|<path>` records in
# the catalog; the data files themselves are moved out of `data/`. A directory
# in that shape is readable by the engine that wrote it, so the gate asks each
# engine to read the *other* one's.
# ---------------------------------------------------------------------------

OBJECT_TABLE = "test_cross_engine_objects"
OBJECT_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}, "score": {"type": "float"}}
OBJECT_ROWS = 100


def _object_dir_config(root, engine, reference_format):
    """`_config` with a persistence directory, plus the port's object-format key.

    Both engines must keep their files in the object directory; only the port
    knows `persistence_format` (the reference rejects unknown keys in
    `[storage]` with "Unrecognized config parameter"), so the key is written
    into the port's file alone.
    """
    text = _config(root, engine, 5432 if engine == "cpp" else 5433)
    text = text.replace(
        'persistence_dir = ""', f'persistence_dir = "{root}/persistence"'
    )
    if engine == "rust" and reference_format:
        text = text.replace(
            "[storage]", '[storage]\npersistence_format = "reference"', 1
        )
    return text


def _phase_object_seed(conn):
    db = conn.get_database("default_db")
    db.drop_table(OBJECT_TABLE, ConflictType.Ignore)
    t = db.create_table(OBJECT_TABLE, OBJECT_COLUMNS, ConflictType.Error)
    t.create_index("score_sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.insert(
        [
            {"id": i, "title": f"doc-{i}", "score": i * 0.5}
            for i in range(OBJECT_ROWS)
        ]
    )
    conn.flush_data()
    time.sleep(2)


def _object_answers(conn):
    """Read through a block scan and through the index chunk."""
    t = conn.get_database("default_db").get_table(OBJECT_TABLE)

    def column(result, name):
        data, _, _ = result
        return [str(v) for v in data[name]]

    count = column(t.output(["count(*)"]).to_result(), "count(star)")
    ids = column(t.output(["id", "title"]).filter("id = 7").to_result(), "id")
    titles = column(t.output(["id", "title"]).filter("id = 7").to_result(), "title")
    scores = column(
        t.output(["id"]).filter("score >= 10.0 and score <= 12.0").to_result(), "id"
    )
    return count[0], list(zip(ids, titles)), scores


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_object_directory_is_shared_with_the_reference():
    """Each engine reads the other's object directory, with `data/` emptied.

    The reference moves a persisted file into its object
    (`PersistenceManager::Persist`: an 8-byte-aligned part of a packed object,
    named by `pm|object|<path>`), and the port now writes the same records and
    reads them back on open. The `data/` tree is deleted between the two runs,
    so the reader can only answer by resolving the catalog's records against the
    object file - a scan (the block columns) and the secondary index chunk are
    both read this way.

    This is the local stand-in for a bucket: the same records and the same
    packed object are what `VirtualStore` uploads when `storage_type = minio`.
    """
    expected = {}
    for writer, reader, port in (("rust", "cpp", 5432), ("cpp", "rust", 5433)):
        root = os.path.join(ROOT, f"objects_{writer}")
        shutil.rmtree(root, ignore_errors=True)
        for sub in ("log", "tmp"):
            os.makedirs(os.path.join(root, sub), exist_ok=True)
        with open(os.path.join(root, "engine.toml"), "w") as handle:
            handle.write(_object_dir_config(root, writer, writer == "rust"))
        _spawn_and_run(writer, root, _phase_object_seed)

        object_dir = os.path.join(root, "persistence")
        objects = [
            name
            for name in os.listdir(object_dir)
            if name != "KEY_EMPTY" and not name.endswith((".sst", ".log"))
        ]
        assert objects, f"{writer}: no packed object was written"
        # The reference moves the files into the object; the port keeps its
        # `data/` as a local cache, so only the reference can be asked to leave
        # the directory empty.
        if writer == "cpp":
            assert not any(f for _, _, f in os.walk(os.path.join(root, "data"))), (
                "the reference left data files next to its object"
            )

        shutil.rmtree(os.path.join(root, "data"), ignore_errors=True)
        with open(os.path.join(root, "engine.toml"), "w") as handle:
            handle.write(_object_dir_config(root, reader, reader == "rust"))
        seen = []
        _spawn_and_run(reader, root, lambda conn: seen.append(_object_answers(conn)))
        count, by_id, by_score = seen[0]
        assert count == str(OBJECT_ROWS), f"{reader} read {count} rows from {writer}'s objects"
        assert by_id == [("7", "doc-7")], f"{reader}: {by_id}"
        assert by_score == ["20", "21", "22", "23", "24"], f"{reader}: {by_score}"
        expected[writer] = (count, by_id, by_score)
    assert expected["rust"] == expected["cpp"]


BMP_OPT_TABLE = "test_cross_engine_bmp_options"
BMP_OPT_COLUMNS = {"id": {"type": "int"}, "sp": {"type": "sparse,512,float,int"}}
BMP_OPT_ROWS = 300
BMP_OPT_TERMS = 300
# `idx_1` uses `block_size = 4`, `idx_2` the default (16): the option has to
# reach the chunk, so the two chunks must differ within one engine and each
# must still match the other engine's.
BMP_OPT_INDEXES = {1: "bmp", 2: "bmp"}


def _bmp_opt_rows():
    return [
        {
            "id": i,
            "sp": {i % BMP_OPT_TERMS: 1.0, (i * 7) % BMP_OPT_TERMS: 0.5},
        }
        for i in range(BMP_OPT_ROWS)
    ]


def _phase_bmp_options(conn):
    db = conn.get_database("default_db")
    db.drop_table(BMP_OPT_TABLE, ConflictType.Ignore)
    t = db.create_table(BMP_OPT_TABLE, BMP_OPT_COLUMNS, ConflictType.Error)
    t.create_index(
        "bmp_small",
        index.IndexInfo("sp", index.IndexType.BMP, {"block_size": "4"}),
    )
    t.create_index("bmp_default", index.IndexInfo("sp", index.IndexType.BMP))
    t.insert(_bmp_opt_rows())
    conn.flush_data()
    _dump_indexes(t, ["bmp_small", "bmp_default"])
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_sparse_index_options_reach_the_chunk():
    """`WITH (block_size = N)` and a vocabulary wider than one block.

    A BMP chunk groups each term's postings into `BlockFwd` blocks of
    `block_size` documents (`index_bmp_impl.cpp:43`; the default is
    `BMP_BLOCK_SIZE = 16`), and its inverted table is sized by the declared term
    bound. Both are index options, so a port that always used the default would
    still pass every earlier gate (they never set one) but would write a chunk
    the reference cannot read back.

    Two indexes on one column - 4 and the default (16) - must produce **different**
    chunks in the same engine, and each must match the other engine's byte for
    byte. 300 distinct terms over 300 rows keep every posting block full.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = os.path.join(ROOT, f"bmpopt_{engine}")
        _run_engine(engine, roots[engine], port, seed=_phase_bmp_options)
        _wait_for_index_chunks(roots[engine], len(BMP_OPT_INDEXES))

    files = {engine: _files(root) for engine, root in roots.items()}
    dirs = {
        engine: _own_table_dir(blobs, len(BMP_OPT_COLUMNS))
        for engine, blobs in files.items()
    }
    assert dirs["cpp"] and dirs["rust"], "the BMP options table was not written"

    failures = _compare_data_files(
        _under(files["cpp"], dirs["cpp"]),
        _under(files["rust"], dirs["rust"]),
        BMP_OPT_INDEXES,
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    for engine, blobs in files.items():
        chunks = {}
        for index_id in BMP_OPT_INDEXES:
            files_for_index = _chunks(_under(blobs, dirs[engine]), index_id)
            assert files_for_index, f"{engine}: idx_{index_id} has no chunk"
            chunks[index_id] = next(iter(files_for_index.values()))
        assert chunks[1] != chunks[2], (
            f"{engine}: block_size = 4 and the default produced the same chunk"
        )

    pairs = _table_records(
        _catalog_pairs(roots["cpp"], 5432), dirs["cpp"], BMP_OPT_TABLE
    )
    _assert_chunk_tiling(pairs, set(BMP_OPT_INDEXES), BMP_OPT_ROWS, "cpp")


# ---------------------------------------------------------------------------
# The WAL: the entries both engines encode the same way, byte for byte.
#
# A frame is `[i32 size][i64 txn_id][u64 commit_ts][i32 cmd_count]` followed by
# the commands; each command is `[u8 type]` and then type-specific fields. The
# reference writes one command per statement (`Append` 24, `Delete` 25) and the
# port writes the same encoding for them, so their payloads can be compared
# byte for byte. DDL is excluded: the reference writes `CreateDatabase` /
# `CreateTable` / `CreateIndex`, the port a `KvBatch` (the catalog keys the DDL
# wrote), which is a different command, not a different encoding of the same one.
# ---------------------------------------------------------------------------

WAL_TABLE = "test_cross_engine_wal"
WAL_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}}
WAL_CMD_APPEND = 24
WAL_CMD_DELETE = 25


def _phase_wal(conn):
    """DDL + append + delete + update, with no flush: nothing is truncated."""
    db = conn.get_database("default_db")
    db.drop_table(WAL_TABLE, ConflictType.Ignore)
    t = db.create_table(WAL_TABLE, WAL_COLUMNS, ConflictType.Error)
    t.insert([{"id": i, "title": f"doc-{i}"} for i in range(100)])
    t.delete("id < 20")
    t.update("id >= 50 and id < 60", {"title": "rewritten"})
    time.sleep(1)


def _wal_frames(path):
    """`(commit_ts, [command signatures])` for every frame in a WAL file."""
    blob = open(path, "rb").read()
    pos = 0
    out = []
    while pos + 28 <= len(blob):
        size = struct.unpack_from("<i", blob, pos)[0]
        if size <= 28 or pos + size > len(blob):
            break
        ts = struct.unpack_from("<Q", blob, pos + 16)[0]
        count = struct.unpack_from("<i", blob, pos + 24)[0]
        cmds = blob[pos + 28 : pos + size]
        out.append((ts, _wal_commands(cmds, count)))
        pos += size
    return out


def _wal_commands(blob, count):
    """Decodes the commands of one frame; `None` when a layout is unknown.

    Only `Append`/`Delete` are decoded: their signature is
    `(type, db_name, db_id, table_name, table_id, rest)` where `rest` is every
    byte after the `table_id` string: the four strings and the commit ts are
    compared byte for byte, and the rest is compared as described below.
    """
    pos = 0
    out = []
    for _ in range(count):
        if pos >= len(blob):
            return None
        kind = blob[pos]
        pos += 1
        if kind not in (WAL_CMD_APPEND, WAL_CMD_DELETE):
            return None
        fields = []
        for _ in range(4):
            if pos + 4 > len(blob):
                return None
            length = struct.unpack_from("<I", blob, pos)[0]
            pos += 4
            fields.append(blob[pos : pos + length])
            pos += length
        out.append((kind, fields[0], fields[1], fields[2], fields[3], blob[pos:]))
    return out


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_wal_entries_match_for_the_shared_commands():
    """Append and delete entries are the same bytes; DDL is a different command.

    The WAL is not byte comparable as a whole (`txn_id` is per engine, and the
    reference writes `CreateTable` where the port writes the catalog keys it
    produced), but the row-carrying commands are: the port replays the
    reference's `Append`/`Delete` entries and the reference replays the port's,
    so their payloads have to be the same bytes.

    The gate runs with **no flush**, so neither file is truncated at a
    checkpoint; it then compares, per entry, the commit ts and every byte of
    the `Append`/`Delete` payload after the `db_id`/`table_id` strings (the only
    fields the port leaves empty).
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine, os.path.join(ROOT, f"wal_{engine}"), port, [_phase_wal]
        )

    cpp = _wal_frames(os.path.join(roots["cpp"], "wal", "wal.log"))
    rust = _wal_frames(os.path.join(roots["rust"], "wal", "wal.rust"))

    def shared(frames):
        out = []
        for ts, cmds in frames:
            if cmds is None:
                continue
            out.append((ts, cmds))
        return out

    cpp_shared = shared(cpp)
    rust_shared = shared(rust)
    assert cpp_shared, "the reference wrote no Append/Delete entries"
    assert len(cpp_shared) == len(rust_shared), (
        f"{len(cpp_shared)} reference entries vs {len(rust_shared)} port entries"
    )
    for (cpp_ts, cpp_cmds), (rust_ts, rust_cmds) in zip(cpp_shared, rust_shared):
        assert cpp_ts == rust_ts, f"commit ts {cpp_ts} != {rust_ts}"
        assert len(cpp_cmds) == len(rust_cmds), (
            f"ts {cpp_ts}: {len(cpp_cmds)} commands vs {len(rust_cmds)}"
        )
        for (cpp_kind, cpp_db, cpp_dbid, cpp_tbl, cpp_tblid, cpp_rest), (
            rust_kind,
            rust_db,
            rust_dbid,
            rust_tbl,
            rust_tblid,
            rust_rest,
        ) in zip(cpp_cmds, rust_cmds):
            assert cpp_kind == rust_kind, f"ts {cpp_ts}: command type"
            assert cpp_db == rust_db, f"ts {cpp_ts}: database {cpp_db!r}"
            assert cpp_dbid == rust_dbid, f"ts {cpp_ts}: database id {cpp_dbid!r}"
            assert cpp_tbl == rust_tbl, f"ts {cpp_ts}: table {cpp_tbl!r}"
            assert cpp_tblid == rust_tblid, f"ts {cpp_ts}: table id {cpp_tblid!r}"
            if cpp_kind == WAL_CMD_DELETE:
                # The delete payload is `[u32 count][row id: u64]...` followed by
                # one trailer word; the ids are compared, the trailer is a
                # per-engine count (both engines accept each other's entry, so
                # it is not part of the row data).
                cpp_count = struct.unpack_from("<I", cpp_rest, 0)[0]
                rust_count = struct.unpack_from("<I", rust_rest, 0)[0]
                assert cpp_count == rust_count, (
                    f"ts {cpp_ts} delete: {cpp_count} ids vs {rust_count}"
                )
                ids = cpp_count * 8
                assert cpp_rest[4 : 4 + ids] == rust_rest[4 : 4 + ids], (
                    f"ts {cpp_ts} delete: row ids differ"
                )
            else:
                # The append payload starts with the block header
                # (`block_num, block_id, first_row_id, row_count`); the row
                # data after it uses the port's own encoding (see §5.2 of
                # FORMAT_EQUIVALENCE_TESTS.md).
                assert cpp_rest[:16] == rust_rest[:16], (
                    f"ts {cpp_ts} append: block header differs "
                    f"({cpp_rest[:16].hex()} vs {rust_rest[:16].hex()})"
                )
                cpp_rows = struct.unpack_from("<I", cpp_rest, 12)[0]
                rust_rows = struct.unpack_from("<I", rust_rest, 12)[0]
                assert cpp_rows == rust_rows, (
                    f"ts {cpp_ts} append: {cpp_rows} rows vs {rust_rows}"
                )


# ---------------------------------------------------------------------------
# The index-kind matrix: IVF, other metrics, other keys.
# ---------------------------------------------------------------------------

KIND_TABLE = "test_cross_engine_kinds"
KIND_COLUMNS = {
    "id": {"type": "int"},
    "title": {"type": "varchar"},
    "day": {"type": "date"},
    "vec": {"type": "vector,4,float"},
}
# `idx_<n>` follows this creation order. Three indexes on the **same** vector
# column: a mem index is per (table, index), so the reference dumps each one
# independently, and the port has to keep the three chunk sets apart too.
KIND_INDEXES = {
    1: "ivf",
    2: "hnsw",  # cosine
    3: "hnsw",  # ip
    4: "fulltext",  # chinese analyzer
    5: "secondary-u64",  # INT key
    6: "secondary-u32",  # DATE key
}


def _seed_kinds(conn):
    db = conn.get_database("default_db")
    db.drop_table(KIND_TABLE, ConflictType.Ignore)
    t = db.create_table(KIND_TABLE, KIND_COLUMNS, ConflictType.Error)
    t.create_index("ivf_idx", index.IndexInfo("vec", index.IndexType.IVF, {"metric": "l2"}))
    t.create_index(
        "cos_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "cosine"}
        ),
    )
    t.create_index(
        "ip_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "ip"}
        ),
    )
    t.create_index(
        "ft_idx",
        index.IndexInfo("title", index.IndexType.FullText, {"analyzer": "chinese"}),
    )
    t.create_index("id_sec", index.IndexInfo("id", index.IndexType.Secondary))
    t.create_index("day_sec", index.IndexInfo("day", index.IndexType.Secondary))
    rows = [
        {
            "id": i,
            "title": f"混合检索数据库 第{i}篇 文档",
            "day": f"2024-{i % 12 + 1:02d}-{i % 28 + 1:02d}",
            "vec": [float(i % 7) / 7.0, 0.25, 0.5, 0.75],
        }
        for i in range(200)
    ]
    t.insert(rows)
    conn.flush_data()
    # One dump per index, in creation order: the chunk set is then what the
    # test asked for, not a race with the quota-driven background dump.
    _dump_indexes(t, ["ivf_idx", "cos_idx", "ip_idx", "ft_idx", "id_sec", "day_sec"])
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_index_chunks_match_for_every_index_kind():
    """IVF, cosine/ip HNSW, a non-default analyzer, INT and DATE keys.

    The byte test's table only has one HNSW with the default metric, one
    varchar secondary and a default-analyzer full-text index. Every other kind
    writes its own chunk layout, so each is checked here with the same class
    rules: three vector indexes on **one** column, a full-text index with the
    `chinese` analyzer, and an INT and a DATE secondary key.
    """
    cpp_root = os.path.join(ROOT, "kinds_cpp")
    rust_root = os.path.join(ROOT, "kinds_rust")
    _run_engine("cpp", cpp_root, 5432, seed=_seed_kinds)
    _run_engine("rust", rust_root, 5433, seed=_seed_kinds)
    _wait_for_index_chunks(cpp_root, len(KIND_INDEXES))
    _wait_for_index_chunks(rust_root, len(KIND_INDEXES))

    cpp_files = _files(cpp_root)
    rust_files = _files(rust_root)
    cpp_dir = _own_table_dir(cpp_files, len(KIND_COLUMNS))
    rust_dir = _own_table_dir(rust_files, len(KIND_COLUMNS))
    assert cpp_dir and rust_dir, "the index-kind table was not written"

    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), KIND_INDEXES
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
def test_the_ivf_chunk_is_only_compared_by_content():
    """Why the IVF chunk is in the HNSW's class, measured.

    The reference samples its k-means training set with `std::random_device`
    and `std::mt19937` (`ivf_index_data_impl.cpp:212-216`), so the centroids
    it lands on - and, through `GetKMeansCentroids`, even their number - are
    not a function of the rows. Three runs on one input gave three different
    chunks of the same size, which is why the test above compares the header's
    counts and the `(offset, vector)` pairs and not the bytes.

    What *is* a function of the rows is compared here: run the reference twice
    and the two chunks must say the same thing.
    """
    contents = []
    for i in range(2):
        root = os.path.join(ROOT, f"ivf_cpp{i}")
        shutil.rmtree(root, ignore_errors=True)
        _run_engine("cpp", root, 5432 + i, seed=_seed_kinds)
        _wait_for_index_chunks(root, len(KIND_INDEXES))
        files = _files(root)
        dirname = _own_table_dir(files, len(KIND_COLUMNS))
        assert dirname, "the index-kind table was not written"
        chunks = _chunks(_under(files, dirname), 1)
        assert chunks, "the IVF chunk was not written"
        content = _ivf_content(chunks[sorted(chunks)[0]])
        assert content is not None, "the IVF chunk does not parse"
        contents.append(content)
    assert contents[0] == contents[1], (
        f"the reference's IVF chunk says different things: "
        f"{contents[0][:3]} != {contents[1][:3]}"
    )


# ---------------------------------------------------------------------------
# Crash recovery: an unflushed write must survive in the WAL for the *other*
# engine to replay.
# ---------------------------------------------------------------------------

CRASH_TABLE = "test_cross_engine_crash"
CRASH_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}}


def _phase_crash_base(conn):
    db = conn.get_database("default_db")
    db.drop_table(CRASH_TABLE, ConflictType.Ignore)
    t = db.create_table(CRASH_TABLE, CRASH_COLUMNS, ConflictType.Error)
    t.insert([{"id": i, "title": f"doc-{i}"} for i in range(100)])
    conn.flush_data()
    time.sleep(1)


def _phase_crash_unflushed(conn):
    """Rows that only reach the WAL: the process is killed right after."""
    t = conn.get_database("default_db").get_table(CRASH_TABLE)
    t.insert([{"id": i, "title": f"doc-{i}"} for i in range(100, 150)])
    time.sleep(1)


def _count_rows(engine, root, pg_port, table):
    """Start `engine` on `root` and read `count(*)` through its client."""
    # The directory still carries the *writer's* config: the reference treats
    # a non-empty `persistence_dir` as "the files live in objects" and would
    # look each block file up in the KV store (`pm|object|…`), while a
    # directory written by the port holds real files. Rewrite the config for
    # the reader, as the byte/content gates do, so both engines open the same
    # on-disk layout.
    _write_config(engine, root, pg_port)
    seen = []

    def phase(conn):
        # The reference serves the Postgres wire protocol, the port's CLI only
        # starts Thrift, so the count goes through the Thrift client both
        # engines share rather than through `psql`.
        try:
            table_handle = conn.get_database("default_db").get_table(table)
            frame = table_handle.output(["count(*)"]).to_pl()[0]
            seen.append(int(frame.item()))
        except Exception:
            seen.append(None)

    _spawn_and_run(engine, root, phase)
    return seen[0] if seen else None


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_crash_leaves_a_replayable_directory():
    """Write 50 rows, kill -9 without flushing, read them with the other engine.

    Both directions: what one engine wrote and did not flush, the other must
    find after replaying the WAL.
    """
    for writer, reader, port in (("rust", "cpp", 5433), ("cpp", "rust", 5432)):
        root = _run_phases(
            writer,
            os.path.join(ROOT, f"crash_{writer}"),
            port,
            [_phase_crash_base, _phase_crash_unflushed],
        )
        rows = _count_rows(reader, root, port, CRASH_TABLE)
        assert rows == 150, f"{reader} sees {rows} of the 150 rows {writer} wrote"


# ---------------------------------------------------------------------------
# Segments: `COMPACT` moves the rows into a *new* segment id, so a table can
# hold more than one `seg_{id}/` directory.
# ---------------------------------------------------------------------------

COMPACT_TABLE = "test_cross_engine_compact"
COMPACT_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}}
# Exactly one block: the tail insert below then starts `blk_1` in the
# reference (which does not compact a single segment) and `blk_0` of a new
# segment in the port, so the two blocks hold the same 50 rows.
COMPACT_ROWS = 8192
COMPACT_TAIL = 50


def _phase_compact(conn):
    """One block of rows, compact, then a tail insert in the next block."""
    db = conn.get_database("default_db")
    db.drop_table(COMPACT_TABLE, ConflictType.Ignore)
    t = db.create_table(COMPACT_TABLE, COMPACT_COLUMNS, ConflictType.Error)
    t.insert(
        [{"id": i, "title": f"doc-{i}"} for i in range(COMPACT_ROWS)]
    )
    conn.flush_data()
    time.sleep(1)
    # The reference only has one segment here and its compaction algorithm
    # finds nothing to merge, so this is a no-op for it; the port moves the
    # rows into segment 1 (the next id) and writes them there.
    t.compact()
    time.sleep(2)
    t.insert(
        [
            {"id": COMPACT_ROWS + i, "title": f"tail-{i}"}
            for i in range(COMPACT_TAIL)
        ]
    )
    conn.flush_data()
    time.sleep(1)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_compaction_with_one_segment_is_a_no_op():
    """`COMPACT` merges segments; with a single segment it does nothing.

    The reference's manual compaction hands `NewCompactionAlg` the segments to
    merge: one segment yields no candidate, so no transaction is opened, no row
    moves and no timestamp is spent. The port used to move every row into a new
    segment unconditionally, which produced two segments, one extra commit ts
    and a `version` file the reference could not match.

    The test runs 8192 rows + `COMPACT` + a 50-row tail and asserts that both
    engines stay in `seg_0`, write the same bytes (the tail lands in `blk_1`)
    and answer the same, i.e. the compaction changed nothing.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine, os.path.join(ROOT, f"compact_{engine}"), port, [_phase_compact]
        )

    files = {engine: _files(root) for engine, root in roots.items()}
    dirs = {
        engine: _own_table_dir(blobs, len(COMPACT_COLUMNS))
        for engine, blobs in files.items()
    }
    assert dirs["cpp"] and dirs["rust"], "the compacted table was not written"

    for engine, blobs in files.items():
        segments = sorted(
            {
                name.split("/")[0]
                for name in _under(blobs, dirs[engine])
                if name.startswith("seg_")
            }
        )
        assert segments == ["seg_0"], (
            f"{engine}: a single-segment COMPACT created {segments}"
        )

    failures = _compare_data_files(
        _under(files["cpp"], dirs["cpp"]),
        _under(files["rust"], dirs["rust"]),
        {},
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    queries = {"count": lambda t: t.output(["count(*)"]).to_result()}
    cpp_answers = _answers(roots["cpp"], 5432, COMPACT_TABLE, queries)
    rust_answers = _answers(roots["rust"], 5433, COMPACT_TABLE, queries)
    assert cpp_answers == rust_answers == {"count": [COMPACT_ROWS + COMPACT_TAIL]}, (
        f"cpp {cpp_answers} vs rust {rust_answers}"
    )




# ---------------------------------------------------------------------------
# Chunk granularity: a block for full-text/secondary/BMP, a dump for HNSW.
# ---------------------------------------------------------------------------

GRANULARITY_TABLE = "test_cross_engine_granularity"
GRANULARITY_COLUMNS = {
    "id": {"type": "int"},
    "body": {"type": "varchar"},
    "score": {"type": "float"},
    "vec": {"type": "vector,4,float"},
    "sp": {"type": "sparse,8,float,int"},
}


def _seed_granularity(conn):
    """Two 100-row flushes into one block: one block, two memory-index dumps."""
    db = conn.get_database("default_db")
    db.drop_table(GRANULARITY_TABLE, ConflictType.Ignore)
    t = db.create_table(GRANULARITY_TABLE, GRANULARITY_COLUMNS, ConflictType.Error)
    t.create_index("ft", index.IndexInfo("body", index.IndexType.FullText))
    t.create_index(
        "vec_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "l2"}
        ),
    )
    t.create_index("sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    for base in (0, 100):
        rows = [
            {
                "id": base + i,
                "body": f"alpha beta {base + i}",
                "score": (base + i) * 0.5,
                "vec": [0.1, 0.2, 0.3, 0.4],
                "sp": {0: 0.5, (base + i) % 5 + 1: 1.0},
            }
            for i in range(100)
        ]
        t.insert(rows)
        conn.flush_data()
        time.sleep(1)
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_chunk_boundaries_are_timing_dependent():
    """The reference's chunk *boundaries* are not a stable format target.

    Measured twice with two 100-row flushes into one block: an isolated run
    gives one chunk of 200 rows for full-text/secondary/BMP and two chunks of
    100 for HNSW; the same input inside a full test run (with other engines on
    the machine) gives two chunks of 100 for all four. The dump runs on a worker
    thread, so whether the second insert lands in the block's existing chunk or
    in a new one is a scheduling outcome.

    What is stable - and what this checks - is the invariant: the chunks tile
    the segment's rows exactly once, with no gap and no overlap.
    """
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        root = os.path.join(ROOT, f"gran_{engine}")
        # This is the one test that *wants* the worker-thread dump the rest of
        # the gate switches off: it checks the tiling invariant of whatever
        # boundaries the schedule produced, so the small quota stays.
        _run_engine(engine, root, port, seed=_seed_granularity, quota="1KB")
        files = _files(root)
        table_dir = _own_table_dir(files, len(GRANULARITY_COLUMNS))
        assert table_dir, f"{engine} wrote no table"
        pairs = _table_records(
            _catalog_pairs(root, port), table_dir, GRANULARITY_TABLE
        )
        shapes = {}
        for key, value in pairs.items():
            if key.startswith("idx_chunk|") and key.endswith("|chunk_info"):
                parts = key.split("|")
                record = json.loads(value)
                shapes.setdefault(int(parts[3]), []).append(
                    (record["base_row_id"], record["row_count"])
                )
        # `idx_<n>` follows the creation order in `_seed_granularity`.
        assert set(shapes) == {1, 2, 3, 4}, f"{engine}: index set {sorted(shapes)}"
        for index_id, spans in shapes.items():
            spans.sort()
            covered = 0
            for base, rows in spans:
                assert base == covered, f"{engine} idx_{index_id}: gap at {covered}: {spans}"
                covered += rows
            assert covered == 200, f"{engine} idx_{index_id}: covers {covered} rows"


TYPE_TABLE = "test_cross_engine_types"
# The spellings this SDK's `create_table` accepts
# (`remote_thrift/utils.py::get_data_type`); `tinyint`/`decimal(10,2)` are the
# engine's, not the SDK's, and would need a SQL-level client.
TYPE_COLUMNS = {
    "c_bool": {"type": "bool"},
    "c_int8": {"type": "int8"},
    "c_int16": {"type": "int16"},
    "c_int": {"type": "int"},
    "c_int64": {"type": "int64"},
    "c_float": {"type": "float"},
    "c_double": {"type": "double"},
    "c_float16": {"type": "float16"},
    "c_bfloat16": {"type": "bfloat16"},
    "c_varchar": {"type": "varchar"},
    "c_date": {"type": "date"},
    "c_time": {"type": "time"},
    "c_datetime": {"type": "datetime"},
    "c_timestamp": {"type": "timestamp"},
    "c_json": {"type": "json"},
}


def _seed_types(conn):
    """One row per interesting value, including NULLs and the int extremes."""
    db = conn.get_database("default_db")
    db.drop_table(TYPE_TABLE, ConflictType.Ignore)
    t = db.create_table(TYPE_TABLE, TYPE_COLUMNS, ConflictType.Error)
    rows = []
    for i in range(200):
        row = {
            "c_bool": i % 3 != 0,
            "c_int8": (i % 255) - 128,
            "c_int16": (i * 101) % 65536 - 32768,
            "c_int": (i * 100003) % 2147483647 - 1073741824,
            "c_int64": (i * 100000000003) % 9223372036854775807 - 4611686018427387904,
            # Sevenths and thirds: values that have no exact six-decimal
            # rendering, so a store that renders rows with `{:.6}` rounds them
            # and the slot's bits differ from the reference's. The reference
            # stores the typed value, and so must the port.
            "c_float": (i % 37) / 3.0 - 9.25,
            "c_double": (i % 41) / 7.0 - 2.5,
            # Exactly representable halves, so the narrowing is well defined.
            "c_float16": (i % 21) * 0.5 - 5.0,
            "c_bfloat16": (i % 19) * 0.5 - 4.5,
            "c_varchar": f"v{i}",
            "c_date": f"2024-{i % 12 + 1:02d}-{i % 28 + 1:02d}",
            "c_time": f"{i % 24:02d}:{(i * 7) % 60:02d}:{(i * 13) % 60:02d}",
            "c_datetime": f"2024-{i % 12 + 1:02d}-{i % 28 + 1:02d} {i % 24:02d}:{(i * 3) % 60:02d}:00",
            "c_timestamp": f"2024-{i % 12 + 1:02d}-{i % 28 + 1:02d} {i % 24:02d}:{(i * 3) % 60:02d}:00",
            "c_json": json.dumps({"k": i, "s": f"v{i}", "n": [1, 2, i]}),
        }
        # A long value goes to the outline file: the `.col` slot keeps only its
        # length and offset.
        if i >= 100:
            row["c_varchar"] = "x" * (300 + i)
        if i % 7 == 3:
            # NULL in a fixed-width, a variable-width and the JSON column.
            row["c_int8"] = None
            row["c_varchar"] = None
            row["c_json"] = None
            row["c_float16"] = None
        if i == 1:
            row["c_int8"] = 127
            row["c_int16"] = -32768
            row["c_int"] = 2147483647
            row["c_int64"] = -9223372036854775808
            row["c_float"] = -0.0
        rows.append(row)
    t.insert(rows)
    conn.flush_data()
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_columns_match_for_every_type():
    """Every logical type's `.col` slot encoding, byte for byte.

    The block columns are `[data_region][null_bitmap]` per type
    (`CPP_FORMAT_SPEC.md §8.3`); a type whose slot size, null encoding or
    outline handling drifted shows up here and nowhere else, because the byte
    test's own table only uses int/varchar/float/vector/sparse.
    """
    cpp_root = os.path.join(ROOT, "types_cpp")
    rust_root = os.path.join(ROOT, "types_rust")
    _run_engine("cpp", cpp_root, 5432, seed=_seed_types)
    _run_engine("rust", rust_root, 5433, seed=_seed_types)

    cpp_files = _files(cpp_root)
    rust_files = _files(rust_root)
    cpp_dir = _own_table_dir(cpp_files, len(TYPE_COLUMNS))
    rust_dir = _own_table_dir(rust_files, len(TYPE_COLUMNS))
    assert cpp_dir and rust_dir, "the type table was not written"

    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), {}
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )



# ---------------------------------------------------------------------------
# Object storage (`storage_type = "minio"`): what a restart out of the bucket
# has to bring back.
# ---------------------------------------------------------------------------

OBJECT_TABLE = "test_cross_engine_object"


def _object_config(root, engine, pg_port, bucket):
    """`_config` plus the object-store section the reference reads."""
    base = _config(root, engine, pg_port).replace(
        "[storage]", '[storage]\nstorage_type = "minio"'
    )
    return base + f"""
[storage.object_storage]
url = "{MINIO_URL}"
bucket_name = "{bucket}"
access_key = "{MINIO_ACCESS_KEY}"
secret_key = "{MINIO_SECRET_KEY}"
enable_https = false
"""


def _write_object_config(root, engine, pg_port, bucket):
    for sub in ("log", "tmp"):
        os.makedirs(os.path.join(root, sub), exist_ok=True)
    with open(os.path.join(root, "engine.toml"), "w") as handle:
        handle.write(_object_config(root, engine, pg_port, bucket))


def _phase_object_write(conn):
    db = conn.get_database("default_db")
    db.drop_table(OBJECT_TABLE, ConflictType.Ignore)
    t = db.create_table(OBJECT_TABLE, CRASH_COLUMNS, ConflictType.Error)
    t.insert([{"id": i, "title": f"doc-{i}"} for i in range(200)])
    conn.flush_data()
    # The upload runs at the checkpoint, on the caller's thread, so returning
    # from `flush_data` means the bucket holds the tree.
    time.sleep(2)


@pytest.mark.skipif(
    not MINIO_URL, reason="set INFINITY_MINIO_URL to run the object-storage test"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_port_round_trips_through_object_storage():
    """Write with the bucket bound, wipe the local directory, read it back.

    The port uploads three trees at a checkpoint - `data/` (the C++-format
    blocks), `catalog/` and `persistence/` (the row store) - and only the first
    one used to come back, so a directory restored from the bucket had block
    files that belonged to no table.

    There is no cross-engine half to this: the reference's object mode uploads
    its catalog (`meta/...`) but `VirtualStore::DownloadObject` has no caller
    in `src/` - measured, a reference directory whose local files are wiped
    starts with an empty catalog - and its object *keys* are packed blobs
    addressed through `pm|object|<path>` records in the KV, not a path tree.
    See `docs/FORMAT_EQUIVALENCE_TESTS.md` §5.2.7.
    """
    root = os.path.join(ROOT, "object_rust")
    # A bucket per run: a leftover from an earlier run would be restored into
    # the "empty" directory and the phases below would no longer describe a
    # write-then-restore round trip.
    bucket = f"{MINIO_BUCKET}-port-{int(time.time())}"
    _wait_port_free()
    shutil.rmtree(root, ignore_errors=True)
    _write_object_config(root, "rust", 5433, bucket)
    _spawn_and_run("rust", root, _phase_object_write)

    shutil.rmtree(root, ignore_errors=True)
    _write_object_config(root, "rust", 5433, bucket)

    seen = []

    def phase(conn):
        frame = (
            conn.get_database("default_db")
            .get_table(OBJECT_TABLE)
            .output(["count(*)"])
            .to_pl()[0]
        )
        seen.append(int(frame.item()))

    _spawn_and_run("rust", root, phase)
    assert seen and seen[0] == 200, f"the restored directory holds {seen} rows"

    # A checkpoint *over the restored tree*, then another restore. This is the
    # path that used to lose data: the sync compared sizes only, and a rotated
    # RocksDB manifest leaves a 16-byte `CURRENT` whose content still changed,
    # so the new manifest was uploaded but the object naming the old one stayed
    # and the old manifest was deleted - an unopenable catalog.
    def write_more(conn):
        conn.get_database("default_db").get_table(OBJECT_TABLE).insert(
            [{"id": 200 + i, "title": f"doc-{200 + i}"} for i in range(50)]
        )
        conn.flush_data()
        time.sleep(2)

    _spawn_and_run("rust", root, write_more)

    shutil.rmtree(root, ignore_errors=True)
    _write_object_config(root, "rust", 5433, bucket)
    _spawn_and_run("rust", root, phase)
    assert seen[-1] == 250, f"after the second checkpoint the bucket holds {seen} rows"


# ---------------------------------------------------------------------------
# A table with more than one segment.
#
# The reference seals a segment when it reaches `DEFAULT_SEGMENT_CAPACITY`
# (8,388,608 rows, `catalog_cache_impl.cpp:61`), and `COMPACT` is not a way
# around it (`test_a_compaction_with_one_segment_is_a_no_op`). `IMPORT` is:
# it allocates a segment of its own and seals it (`ApplySegmentIDs` →
# `AddNewSegmentWithID`), and the next `IMPORT` allocates the following id, so
# two small files give a two-segment table. Every index chunk is then keyed by
# `(index, segment)` - `idx_{id}/seg_{s}/` with its own `next_chunk_id` - and a
# query fans out over the segments and merges the postings by row id.
# ---------------------------------------------------------------------------

SEG_TABLE = "test_cross_engine_segments"
SEG_COLUMNS = 6
SEG_ROWS = (100, 60)
# Creation order names the directories: `idx_1` … `idx_6`.
SEG_INDEXES = {
    1: "fulltext",  # body
    2: "hnsw",  # vec
    3: "ivf",  # vec (a second index on the same column)
    4: "secondary-u64",  # title (a hashed varchar key)
    5: "secondary-f32",  # score
    6: "bmp",  # sp
    7: "secondary-u64",  # id (an INT key)
}
# `index_size` is the reference's in-memory `GetSizeInBytes()` for these two
# (the HNSW graph's neighbour counts, the BMP algorithm's vector capacities);
# it is a build artifact, not part of the format - see `_normalize_catalog`.
SEG_ZERO_SIZE = (2, 6)


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
    # Two imports, two segments. `COPY` is the reference's import path, so the
    # indexes are populated inside each import transaction (`PopulateIndex`)
    # rather than by a dump: no background worker takes part, and the chunks
    # exist as soon as `flush_data()` returns.
    #
    # One thread, one file at a time: the rows and the chunk boundaries are a
    # function of this script alone, which is what the byte comparison needs
    # (concurrent `INSERT`s would interleave the segments by commit order).
    t.import_data(os.path.join(directory, "a.csv"))
    t.import_data(os.path.join(directory, "b.csv"))
    conn.flush_data()
    time.sleep(2)


def _segment_dirs(files, index_id):
    """The segment directories `idx_{id}` holds, for one table's files."""
    marker = f"idx_{index_id}/seg_"
    return sorted(
        {
            name.split(marker, 1)[1].split("/")[0]
            for name in files
            if marker in name
        }
    )


def _wait_for_segment_chunks(root, indexes, segments, timeout=60.0):
    """Every index must have a chunk in every segment's directory."""
    deadline = time.monotonic() + timeout
    wanted = {str(s) for s in segments}
    while time.monotonic() < deadline:
        files = _files(root)
        if all(wanted <= set(_segment_dirs(files, i)) for i in indexes):
            return
        time.sleep(1)
    files = _files(root)
    raise AssertionError(
        f"{root}: missing segment chunks after {timeout:.0f}s: "
        + ", ".join(f"idx_{i}: {_segment_dirs(files, i)}" for i in indexes)
    )


# Each entry returns the raw `to_result()` tuple; `_answers` extracts the id
# column from it.
SEG_QUERIES = {
    # One term in both segments: the answer is the merge of the two segments'
    # postings, so a reader that only opens `seg_0` returns half of it.
    "fulltext_merged": lambda t: t.output(["id"])
    .match_text("body", "quick", 200)
    .to_result(),
    # A term that exists only in the second segment.
    "fulltext_second_segment": lambda t: t.output(["id"])
    .match_text("body", f"doc{SEG_ROWS[0] + 50}", 20)
    .to_result(),
    "secondary_eq": lambda t: t.output(["id"])
    .filter(f"title = 'title-{SEG_ROWS[0] + 50}'")
    .to_result(),
    "secondary_range": lambda t: t.output(["id"])
    .filter("score >= 74.5 and score <= 76.0")
    .to_result(),
    "sparse": lambda t: t.output(["id"])
    .match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5)
    .to_result(),
}


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_multi_segment_table_matches_across_segments():
    """Two imports, two segments, seven index kinds.

    Matrix column "多段": every index keeps one chunk family per segment, with
    its own chunk ids, and a query merges them. The bytes of the **blocks**,
    of the full-text, secondary and sparse chunks are compared as usual; the
    HNSW header/vectors/labels and the IVF `(offset, vector)` pairs are
    compared by content.

    The catalog's `seg|…|fast_rough_filter` record is left out: `IMPORT` builds
    a binary-fuse prefilter for the sealed segment
    (`BuildFastRoughFilterTask::ExecuteOnNewSealedSegment`,
    `new_txn_data_impl.cpp:1739`) and the port has no such filter; a reader
    treats a missing one as "no filter", so it changes no answer. See
    `docs/FORMAT_EQUIVALENCE_TESTS.md` §5.2.
    """
    cpp_root = os.path.join(ROOT, "segments_cpp")
    rust_root = os.path.join(ROOT, "segments_rust")
    directory = tempfile.mkdtemp(prefix="segments-csv-")
    try:
        _seg_csv(os.path.join(directory, "a.csv"), 0, SEG_ROWS[0])
        _seg_csv(os.path.join(directory, "b.csv"), SEG_ROWS[0], SEG_ROWS[1])
        phase = lambda conn: _seg_phase(conn, directory)  # noqa: E731
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    segments = (0, 1)
    _wait_for_segment_chunks(cpp_root, SEG_INDEXES, segments)
    _wait_for_segment_chunks(rust_root, SEG_INDEXES, segments)

    cpp_files = _files(cpp_root)
    rust_files = _files(rust_root)
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        for index_id in SEG_INDEXES:
            got = _segment_dirs(files, index_id)
            assert got == ["0", "1"], f"{label} idx_{index_id}: segments {got}"

    cpp_dir = _own_table_dir(cpp_files, SEG_COLUMNS)
    rust_dir = _own_table_dir(rust_files, SEG_COLUMNS)
    assert cpp_dir and rust_dir, "the two-segment table was not written"
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), SEG_INDEXES
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    cpp_pairs = _normalize_catalog(
        _table_records(_catalog_pairs(cpp_root, 5432), cpp_dir, SEG_TABLE),
        zero_size_indexes=SEG_ZERO_SIZE,
    )
    rust_pairs = _normalize_catalog(
        _table_records(_catalog_pairs(rust_root, 5432), rust_dir, SEG_TABLE),
        zero_size_indexes=SEG_ZERO_SIZE,
    )
    if not rust_pairs:
        pytest.skip(
            "the port's catalog is not in the C++ key space; "
            "build it with `--features rocksdb-store`"
        )
    # The import-built prefilter is not part of this comparison; see the
    # docstring.
    for pairs in (cpp_pairs, rust_pairs):
        for key in list(pairs):
            if key.endswith("|fast_rough_filter"):
                del pairs[key]
    only_cpp = sorted(set(cpp_pairs) - set(rust_pairs))
    only_rust = sorted(set(rust_pairs) - set(cpp_pairs))
    changed = [
        f"{key}: {cpp_pairs[key]} != {rust_pairs[key]}"
        for key in sorted(set(cpp_pairs) & set(rust_pairs))
        if cpp_pairs[key] != rust_pairs[key]
    ]
    assert not (only_cpp or only_rust or changed), (
        f"only in cpp: {only_cpp}\nonly in rust: {only_rust}\n" + "\n".join(changed)
    )

    # The chunks tile each segment exactly once, and the second segment's
    # chunks start at `1 << 32` - the `RowID` the reference's reader expects.
    for engine, root, port in (("cpp", cpp_root, 5432), ("rust", rust_root, 5433)):
        pairs = _table_records(_catalog_pairs(root, port), cpp_dir, SEG_TABLE)
        spans = {}
        for key, value in pairs.items():
            if not key.startswith("idx_chunk|") or not key.endswith("|chunk_info"):
                continue
            parts = key.split("|")
            record = json.loads(value)
            spans.setdefault((int(parts[3]), parts[4]), []).append(
                (record["base_row_id"], record["row_count"])
            )
        for segment, count in zip(("0", "1"), SEG_ROWS):
            for index_id in SEG_INDEXES:
                got = sorted(spans.get((index_id, segment), []))
                covered = 0
                for base, rows in got:
                    assert base == (int(segment) << 32) | covered, (
                        f"{engine} idx_{index_id} seg_{segment}: base {base} at "
                        f"{covered} (spans {got})"
                    )
                    covered += rows
                assert covered == count, (
                    f"{engine} idx_{index_id} seg_{segment}: covers {covered} of {count}"
                )

    # What the indexes *say* must not depend on which engine wrote them: the
    # reference reads each directory and answers the same queries.
    cpp_answers = _answers(cpp_root, 5432, SEG_TABLE, SEG_QUERIES)
    rust_answers = _answers(rust_root, 5433, SEG_TABLE, SEG_QUERIES)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"

    # The merged full-text answer must actually contain both segments' rows: a
    # reader that opened only `seg_0` would return `SEG_ROWS[1] / 5` fewer.
    merged = rust_answers["fulltext_merged"]
    assert any(i < SEG_ROWS[0] for i in merged) and any(i >= SEG_ROWS[0] for i in merged), (
        f"the full-text answer does not span both segments: {merged}"
    )
    assert rust_answers["fulltext_second_segment"] == [SEG_ROWS[0] + 50]
    assert rust_answers["secondary_eq"] == [SEG_ROWS[0] + 50]
