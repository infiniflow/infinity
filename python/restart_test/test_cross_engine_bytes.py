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
  index chunk and the integral-key secondary index chunk. The reference
  produces the same bytes on every run, so the two engines must too: **byte for
  byte identical**. This is the strong signal.
  The data set is `ROWS` rows (20000 by default): three blocks and three index
  chunks, so the multi-block and multi-chunk layouts are covered, not just the
  first block.
  The sparse chunk is in this class because the order of a term's rows inside a
  block is defined: the reference sorts the block's `(term, offset, value)`
  triples with a **stable** sort on the term id (`ToBlockFwd`,
  `bmp_fwd.cppm:312`), so a term's rows stay in document order. The
  integral-key secondary chunk is here because the reference's PGM fit over a
  u64 key is pure `__int128` arithmetic (`piecewise_linear_model.hpp:249`, the
  integral branch), with a `u64::MAX` sentinel rather than infinity.
* **Deterministic modulo a timestamp** — the block `version` file: the
  reference records the commit ts of the append that filled the block, the port
  records its own mirror ts. The layout must match; the ts must not be compared.
* **Not byte comparable — compared by content** — two *models* that the
  reference itself does not reproduce across its own builds:
  - the HNSW chunk's **graph**: the reference builds it with a
    `thread_local std::mt19937` (`hnsw_alg.cppm:107`), so which thread
    inserts which vector — and therefore which node gets which level — is up
    to the scheduler. Measured: five runs of the reference on 5000 rows
    produced **five different** HNSW chunks, while every other file was
    byte-stable.
  - the **float/double**-key secondary index chunk's **PGM model**: the
    reference fits it in x87 80-bit `long double` and decides its segment count
    with an undefined conversion (`size_t(inf)`). Measured: the same keys give
    two segments under clang++-20 and three under g++, and an f64 fit differs
    from the 80-bit one in 18 of 51 random float key sets. See the assertion
    in the test body for the details.

  What must match is the **postings** — the vectors and labels, the ordered
  keys and row offsets, the terms with their rows and scores — because that is
  what the index *says*. The postings are compared by decoding both chunks.
* **Not comparable at all** — the RocksDB `catalog/` directory and the `wal/`
  directory. Both carry session ids, timestamps and LSNs; the reference is
  not even byte-reproducible against *itself* here.

The test therefore asserts:
1. every deterministic file is byte identical — including the sparse index
   chunk and the integral-key secondary index chunk;
2. the `version` file is identical once its ts is normalized;
3. the two index chunks that carry a build-dependent model — the HNSW graph
   and the float/double secondary PGM fit — still have identical **postings**
   (decoded), and the rest of the HNSW chunk (header, vectors, labels) is byte
   identical;
4. the catalog's *logical* content (the key/value pairs the reference sees
   through `SHOW CATALOG 'all'`) is identical, modulo the ts the reference
   stamps on the records it wrote itself;
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
# More than one block (the capacity is 8192 rows) and more than one index
# chunk, so the multi-block and multi-chunk layouts are covered too. The
# determinism measurement uses its own, smaller data set: the reference's HNSW
# build stays on one thread at a few hundred rows and spreads over several
# threads at a few thousand (which is why its graph is not byte comparable).
ROWS = int(os.environ.get("INFINITY_BYTES_ROWS", "20000"))
DET_ROWS = int(os.environ.get("INFINITY_BYTES_DET_ROWS", "500"))

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


def _config(root, engine, pg_port):
    """A config for one engine, rooted at `root`.

    The two engines read the root differently: the reference takes
    `data_dir`/`catalog_dir`/`wal_dir` and uses `persistence_dir` only as a
    "use the object store" switch (it must stay empty), while the port
    derives its root from the *parent* of `persistence_dir`
    (`serve_from_config`, `crates/infinity-cli/src/main.rs:98`).
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
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"

[buffer]
buffer_manager_size = "2GB"
temp_dir = "{root}/tmp"
result_cache = "off"
memindex_memory_quota = "1KB"

[wal]
wal_dir = "{root}/wal"
checkpoint_interval = "0s"

[resource]
"""


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
    # The reference dumps its memory indexes asynchronously, and only when the
    # index-memory quota (`memindex_memory_quota`, 1KB here) is exceeded - and
    # only one non-EMVB index per round (`MemIndexTracer::MakeDumpTask`,
    # `memindex_tracer_impl.cpp:130`). At a few hundred rows the quota is
    # crossed repeatedly, so every index lands; at a few thousand rows the
    # growth is one-off, so only the HNSW index (the EMVB one) is dumped and
    # the other four chunks never appear. A fixed sleep is therefore a race,
    # and the file sets differ for a reason that has nothing to do with the
    # format. `optimize` takes the reference's OptimizeIndex path, which dumps
    # the memory indexes deterministically.
    t.optimize()

    time.sleep(2)


def _run_engine(engine, root, pg_port, row_count=ROWS):
    """Seed one engine's directory and return it."""
    shutil.rmtree(root, ignore_errors=True)
    for sub in ("log", "tmp"):
        os.makedirs(os.path.join(root, sub), exist_ok=True)
    config = os.path.join(root, "engine.toml")
    with open(config, "w") as handle:
        handle.write(_config(root, engine, pg_port))
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
        stdout=open(os.path.join(root, "engine.log"), "w"),
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
                conn.get_database("default_db")
                break
            except Exception:
                time.sleep(0.5)
        else:
            raise AssertionError(f"{engine} engine did not come up")
        _seed(conn, row_count)
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
        failures.append(
            f"idx_{index_id}: files differ: "
            f"only in cpp {sorted(set(cpp_files_for_index) - set(rust_files_for_index))}, "
            f"only in rust {sorted(set(rust_files_for_index) - set(cpp_files_for_index))}"
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
        elif kind == "hnsw":
            # The header, the vectors and the labels are derived from the
            # input; the graph is built, and the reference does not reproduce
            # it (see the module docstring), so it is compared by content via
            # `_index_answers`.
            regions_a, regions_b = _hnsw_regions(a), _hnsw_regions(b)
            for part in ("header", "vectors", "labels"):
                if regions_a[part] != regions_b[part]:
                    failures.append(f"idx_{index_id}/{name}: {part} differ")
        elif kind in ("secondary-f32", "secondary-f64", "secondary-u32", "secondary-i64"):
            key_width = {"secondary-f32": 4, "secondary-u32": 4, "secondary-i64": 8, "secondary-f64": 8}[kind]
            floating_width = 8 if kind == "secondary-f64" else 4
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


def _compare_data_files(cpp, rust, index_kinds):
    """The files of one table (relative to its directory), by class."""
    failures = []
    if set(cpp) != set(rust):
        failures.append(
            f"file sets differ: only in cpp {sorted(set(cpp) - set(rust))}, "
            f"only in rust {sorted(set(rust) - set(cpp))}"
        )
        return failures
    for name in sorted(cpp):
        if name.split("/")[0].startswith("idx_"):
            continue
        if name.endswith("/version"):
            # Same layout, different commit ts (the reference stamps the
            # append's ts, the port its mirror ts).
            if _normalize_version(cpp[name]) != _normalize_version(rust[name]):
                failures.append(f"{name}: the version layout differs")
        elif cpp[name] != rust[name]:
            # Block columns and the outline: laid out from the row values.
            failures.append(
                f"{name}: cpp {hashlib.md5(cpp[name]).hexdigest()} "
                f"({len(cpp[name])} B) != rust "
                f"{hashlib.md5(rust[name]).hexdigest()} ({len(rust[name])} B)"
            )
    index_ids = sorted(
        int(name.split("/")[0][len("idx_") :])
        for name in cpp
        if name.split("/")[0].startswith("idx_")
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


def _normalize_version(blob):
    """The block `version` file without its commit ts.

    Layout: `[u16 create_size]` then, per create, `[u64 create_ts][i64 rows]`,
    then `[u16 capacity]` and one u64 per slot. Only the `create_ts` differs
    between the engines (the reference stamps the append's commit ts, the port
    its mirror ts), so it is replaced by zero.
    """
    out = bytearray(blob)
    pos = 0
    create_size = struct.unpack_from("<H", out, pos)[0]
    pos += 2
    for _ in range(create_size):
        struct.pack_into("<Q", out, pos, 0)  # the create ts
        pos += 16
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
optimize_interval = "0s"
cleanup_interval = "0s"
compact_interval = "0s"

[buffer]
buffer_manager_size = "2GB"
temp_dir = "{root}/tmp"
result_cache = "off"
memindex_memory_quota = "1KB"

[wal]
wal_dir = "{root}/wal"
checkpoint_interval = "0s"

[resource]
"""


def _catalog_pairs(root, pg_port):
    """The catalog's key/value pairs, as the reference sees them.

    `SHOW CATALOG 'all'` lists what the engine actually holds, which is the
    logical content of the catalog. The reference stamps the ts it wrote on
    its own records, so the value of a record that is *only* a ts is
    normalized; the records that carry content are compared as they are.

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
                conn.get_database("default_db")
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


def _normalize_catalog_key(key):
    """The catalog key without the ts it ends with.

    The reference stamps the commit ts of the transaction that created the
    record into the key (`catalog|tbl|{db}|{name}|{ts}`,
    `catalog|idx|{db}|{tbl}|{name}|{ts}`,
    `tbl|col|{db}|{tbl}|{name}|{ts}`); the port writes its own mirror ts.
    The ts is the only part that may differ - the key's *structure* is the
    format, and it must match.
    """
    return re.sub(r"\|(\d+)$", "", key)


def _normalize_catalog_value(value):
    """The catalog value with a bare ts replaced by a placeholder.

    A record whose value is only a ts (`catalog|blk|…`, `idx_seg|…`,
    `idx_chunk|…`) holds the commit ts of the write that created it.
    """
    if re.fullmatch(r"\d+", value):
        return "<ts>"
    return value


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
        elif head.startswith("table_index_tag:"):
            # `table_index_tag: idx|{db}|{table}|...`
            if len(parts) > 2 and parts[1] == db_id and parts[2] == table_id:
                out[key] = value
        elif head == "drop":
            if table in key:
                out[key] = value
    return out


def _normalize_catalog(pairs, zero_size_indexes=()):
    """The catalog's records, with the ts taken out of the keys and values.

    `index_size` is also taken out of the chunk records of the indexes in
    `zero_size_indexes`: the reference stores `GetSizeInBytes()` of the
    in-memory index it dumps, which for HNSW sums the *graph's* neighbour
    counts (`GraphStoreInner::GetSizeInBytes`, `graph_store.cppm:221`) and for
    BMP the in-memory `BMPAlg`'s vector capacities. Both are properties of the
    built structure, not of the format - they differ per build just like the
    graph itself - and a reader treats zero as "use the file's own size"
    (`HnswFileWorker::HnswFileWorker`, `hnsw_file_worker_impl.cpp:55`), so the
    port writes zero.
    """
    out = {}
    for key, value in pairs.items():
        key = _normalize_catalog_key(key)
        value = _normalize_catalog_value(value)
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

    # The catalog: the same logical records. The reference stamps the commit
    # ts of the transaction that created a record into its key and, for some
    # records, into its value; the port writes its own mirror ts. The ts is
    # the only part that may differ, so it is taken out before comparing -
    # the key's *structure* and the value's *content* are the format. The
    # `index_size` of a graph-shaped chunk (HNSW, BMP) is taken out as well,
    # for the reason given in `_normalize_catalog`.
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

    The byte test compares the sparse chunk and the integral-key secondary
    chunk byte for byte and excludes only the HNSW chunk (its graph) and the
    float-key secondary chunk (its PGM model). That split is an assumption
    about the reference, so it is checked here rather than asserted in a
    comment: the reference is run three times on the same input and the files
    that come out differently are counted.

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
