"""The two engines must write the same bytes for the same data.

`test_cross_engine_compat.py` proves the two engines can *read* each other's
directories. This test proves the stronger claim: given the same input, they
*write* the same files. It is the format-equivalence check the behavioural
test cannot make — a chunk that both engines happen to interpret correctly is
still a format difference if their bytes differ.

The comparison is per file class, because the files are not all equally
determined:

* **Deterministic** — the block columns (`.col`), the variable-length
  outline (`col_*_out`) and the full-text chunk (`.dic`/`.pos`/`.len`). Both
  engines derive these from the row values alone, so they must be **byte for
  byte identical**. This is the strong signal.
* **Deterministic modulo a timestamp** — the block `version` file: the
  reference records the commit ts of the append that filled the block, the
  port records its own mirror ts. The layout must match; the ts must not be
  compared.
* **Not byte comparable** — the HNSW chunk, the secondary-index chunk and the
  sparse (BMP) chunk. These are *built*, not laid out: the HNSW graph depends
  on the order the vectors were inserted and on the level a node was assigned,
  the secondary index carries a learned PGM model, and the sparse index orders
  its terms by score. Two runs of the *same* engine on the same data already
  differ here, so a byte comparison would be meaningless. What must match is
  the *content*: the same vectors, the same labels, the same postings.
* **Not comparable at all** — the RocksDB `catalog/` directory and the `wal/`
  directory. Both carry session ids, timestamps and LSNs; the reference is
  not even byte-reproducible against *itself* here.

The test therefore asserts:
1. every deterministic file is byte identical;
2. the `version` file is identical once its ts is normalized;
3. the HNSW chunk's header, vector region and labels are identical (only the
   graph may differ);
4. the catalog's *logical* content (the key/value pairs the reference sees
   through `SHOW CATALOG 'all'`) is identical, modulo the ts the reference
   stamps on the records it wrote itself.
"""

import hashlib
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
ROWS = 200

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


def _seed(conn):
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
    for i in range(ROWS):
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
    t.insert(rows)
    conn.flush_data()
    time.sleep(2)


def _run_engine(engine, root, pg_port):
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
        _seed(conn)
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
    index's parameters alone, so it must match. The **enterpoint** is the node
    the graph hangs from - a property of the build, not of the format, so it
    belongs with the graph. The vector region and the labels are derived from
    the input and must match.
    """
    _m, _ef, cur = struct.unpack_from("<3Q", chunk, 0)
    # The store meta is 8 bytes here (the dim), then the graph parameters.
    meta_len = 8
    mmax0, mmax = struct.unpack_from("<2Q", chunk, 24 + meta_len)
    _max_layer, _enterpoint = struct.unpack_from("<2i", chunk, 40 + meta_len)
    header_end = 44 + meta_len  # up to and including `max_layer`
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


def _normalize_catalog(pairs):
    """The catalog's records, with the ts taken out of the keys and values."""
    return {
        _normalize_catalog_key(key): _normalize_catalog_value(value)
        for key, value in pairs.items()
    }


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

    # The two engines must write the same *set* of files: the same block
    # columns, the same outline, the same index chunks.
    assert set(cpp_files) == set(rust_files), (
        f"only in cpp: {sorted(set(cpp_files) - set(rust_files))}, "
        f"only in rust: {sorted(set(rust_files) - set(cpp_files))}"
    )

    # The deterministic files: the block columns and the outline. These are
    # laid out from the row values alone, so they must match byte for byte.
    for name in sorted(cpp_files):
        if name.endswith("/version"):
            continue
        if "/idx_" in name:
            continue  # the index chunks are compared below
        assert cpp_files[name] == rust_files[name], (
            f"{name} differs: "
            f"cpp {hashlib.md5(cpp_files[name]).hexdigest()} "
            f"({len(cpp_files[name])} bytes), "
            f"rust {hashlib.md5(rust_files[name]).hexdigest()} "
            f"({len(rust_files[name])} bytes)"
        )

    # The `version` file: same layout, different commit ts.
    version = "db_1/tbl_0/seg_0/blk_0/version"
    assert _normalize_version(cpp_files[version]) == _normalize_version(
        rust_files[version]
    ), "the version file's layout differs"

    # The full-text chunk: the reference's own encoders, byte for byte.
    for suffix in (".dic", ".pos", ".len"):
        name = f"db_1/tbl_0/idx_1/seg_0/ft_0000000000000000{suffix}"
        assert cpp_files[name] == rust_files[name], f"{name} differs"

    # The HNSW chunk: the header, the vectors and the labels are derived from
    # the input; the graph is built and may differ.
    hnsw = "db_1/tbl_0/idx_2/seg_0/chunk_0.idx"
    cpp_hnsw = _hnsw_regions(cpp_files[hnsw])
    rust_hnsw = _hnsw_regions(rust_files[hnsw])
    assert cpp_hnsw["header"] == rust_hnsw["header"], "the HNSW header differs"
    assert cpp_hnsw["vectors"] == rust_hnsw["vectors"], "the HNSW vectors differ"
    assert cpp_hnsw["labels"] == rust_hnsw["labels"], "the HNSW labels differ"

    # The catalog: the same logical records. The reference stamps the commit
    # ts of the transaction that created a record into its key and, for some
    # records, into its value; the port writes its own mirror ts. The ts is
    # the only part that may differ, so it is taken out before comparing -
    # the key's *structure* and the value's *content* are the format.
    #
    # The port writes the C++ key space only in its `rocksdb-store` build;
    # without it the reference opens an empty catalog and there is nothing to
    # compare, so the check is skipped rather than failed.
    cpp_catalog = _normalize_catalog(_catalog_pairs(cpp_root, cpp_pg))
    rust_catalog = _normalize_catalog(_catalog_pairs(rust_root, rust_pg))
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
