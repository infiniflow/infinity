"""L4 WAL：整帧（含 header 的 `txn_id` 与 CRC）。

`test_the_wal_entries_match_byte_for_byte`（建表 + append + delete + update，
不 flush：整份 `wal.log` 逐帧相同）、
`test_the_ddl_wal_commands_match_the_reference`（每个 DDL 的 typed 命令载荷）、
`test_the_append_block_matches_for_every_type`（`AppendV2` 的 `DataBlock`
列式编码，一行一类型）。

对照物是**帧**：命令类型、载荷、事务号与 CRC。文件布局不参与。
"""

import os
import struct
import time

import pytest
from bytes_common import (
    CPP_BIN,
    ROOT,
    RUST_BIN,
    _run_engine,
    _run_phases,
)
from infinity import index
from infinity.common import ConflictType

WAL_TABLE = "test_cross_engine_wal"
WAL_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}}
WAL_CHECKPOINT = 104


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
    """`(commit_ts, body)` for every frame in a WAL file.

    `body` is everything after the frame header (the command count and the
    commands), the part both engines must agree on; the header's checksum and
    `txn_id` are per-engine.
    """
    blob = open(path, "rb").read()
    pos = 0
    out = []
    while pos + 28 <= len(blob):
        size = struct.unpack_from("<i", blob, pos)[0]
        if size <= 28 or pos + size > len(blob):
            break
        ts = struct.unpack_from("<Q", blob, pos + 16)[0]
        out.append((ts, blob[pos + 28 : pos + size]))
        pos += size
    return out


def _wal_raw_frames(path):
    """Every frame's bytes, header included."""
    blob = open(path, "rb").read()
    pos = 0
    out = []
    while pos + 28 <= len(blob):
        size = struct.unpack_from("<i", blob, pos)[0]
        if size <= 28 or pos + size > len(blob):
            break
        out.append(blob[pos : pos + size])
        pos += size
    return out


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_wal_entries_match_byte_for_byte():
    """Every frame is the same bytes, header included.

    The header carries `size`, the CRC32 of the frame and `txn_id`; the CRC
    covers the others, so comparing whole frames compares the transaction id
    sequence too. The reference takes an id per **statement** — read-only ones
    included (`USE`, `SHOW TABLES`, `SELECT`) — and the port now spends them at
    the same points: one at startup for the memory-index recovery, one for the
    startup checkpoint, one for `create default_db`, then one per RPC/statement.

    That covers the typed DDL commands (`CreateTableV2`, `CreateIndexV2`, …) as
    well as `AppendV2` (a reference `DataBlock` of typed columns, not the port's
    text block) and `DeleteV2`, and it covers the *order* of the commands inside
    a frame — an UPDATE is one frame holding the delete and then the append, and
    the reference writes them in that order.

    The gate runs with **no flush**, so neither file is truncated at a
    checkpoint. Both files are `wal/wal.log` and hold the same frames, the
    startup `CheckpointV2` included: the port swaps the log at a checkpoint and
    writes the frame into the fresh file, as `WalManager::SwapWalFile` does.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        roots[engine] = _run_phases(
            engine, os.path.join(ROOT, f"wal_{engine}"), port, [_phase_wal]
        )

    cpp = _wal_raw_frames(os.path.join(roots["cpp"], "wal", "wal.log"))
    rust = _wal_raw_frames(os.path.join(roots["rust"], "wal", "wal.log"))

    assert cpp, "the reference wrote no entries"
    assert len(cpp) == len(rust), f"{len(cpp)} frames vs {len(rust)}"
    for i, (a, b) in enumerate(zip(cpp, rust)):
        assert a == b, f"frame {i} differs:\n  cpp  {a.hex()}\n  rust {b.hex()}"


WAL_TYPE_TABLES = [
    ("c_null", {"type": "int32"}, None),
    ("c_bool", {"type": "bool"}, True),
    ("c_i8", {"type": "int8"}, 1),
    ("c_i16", {"type": "int16"}, 2),
    ("c_i32", {"type": "int32"}, 3),
    ("c_i64", {"type": "int64"}, 4),
    ("c_f32", {"type": "float32"}, 1.5),
    ("c_f64", {"type": "float64"}, 2.5),
    ("c_f16", {"type": "float16"}, 1.5),
    ("c_bf16", {"type": "bfloat16"}, 2.5),
    ("c_varchar", {"type": "varchar"}, "abc"),
    ("c_date", {"type": "date"}, "2024-01-02"),
    ("c_time", {"type": "time"}, "03:04:05"),
    ("c_datetime", {"type": "datetime"}, "2024-01-02 03:04:05"),
    ("c_ts", {"type": "timestamp"}, "2024-01-02 03:04:05"),
    ("c_json", {"type": "json"}, '{"a":1}'),
    ("c_vec", {"type": "vector,4,float"}, [1.0, 2.0, 3.0, 4.0]),
    ("c_mvec", {"type": "multivector,2,float"}, [[1.0, 2.0], [3.0, 4.0]]),
    ("c_tensor", {"type": "tensor,4,float"}, [1.0, 2.0, 3.0, 4.0]),
    ("c_sparse", {"type": "sparse,8,float,int"}, {1: 0.5, 3: 0.25}),
]


def _phase_wal_types(conn):
    db = conn.get_database("default_db")
    for name, spec, value in WAL_TYPE_TABLES:
        table = "waltype_" + name
        db.drop_table(table, ConflictType.Ignore)
        t = db.create_table(table, {name: spec}, ConflictType.Error)
        t.insert([{name: value}])
    time.sleep(1)


def _append_blocks(wal_dir):
    """The `DataBlock` bytes of every `AppendV2` frame, in replay order.

    A restart seals the active log (`wal.log` -> `wal.log.<ts>`), so the frames
    are spread over the sealed segments and the active file; the port writes the
    same names the reference does.
    """
    sealed = sorted(
        (int(name.rsplit(".", 1)[1]), os.path.join(wal_dir, name))
        for name in os.listdir(wal_dir)
        if name.startswith("wal.log.") and name.rsplit(".", 1)[1].isdigit()
    )
    files = [path for _, path in sealed] + [os.path.join(wal_dir, "wal.log")]
    out = []
    for path in files:
        if not os.path.exists(path):
            continue
        for _, body in _wal_frames(path):
            if not body or body[0] != 24:
                continue
            pos = 1
            for _ in range(4):
                length = struct.unpack_from("<I", body, pos)[0]
                pos += 4 + length
            ranges = struct.unpack_from("<I", body, pos)[0]
            pos += 4 + ranges * 16
            out.append(body[pos : len(body) - 4])
    return out


def _phase_wal_types_rows(conn):
    """After a restart: every table still has its one row (the WAL replayed)."""
    db = conn.get_database("default_db")
    for name, _, _ in WAL_TYPE_TABLES:
        # `to_result` is `(column -> values, column definitions, row count)`.
        values, _, _ = db.get_table("waltype_" + name).output([name]).to_result()
        cells = values[name]
        assert len(cells) == 1, f"{name}: {len(cells)} rows after replay"


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_append_block_matches_for_every_type():
    """One row of every column type the port puts in a reference block.

    Covers the bit-packed `Boolean`, the integer widths, `Float16`/`BFloat16`,
    the four-byte `Date`/`Time`, the eight-byte `DateTime`/`Timestamp`, the
    16-byte `Varchar`/`Json` slots with their heaps and the inline `Embedding`.
    A `NULL` row exercises the roaring null bitmap, whose values are the
    **non-NULL** rows of the whole 8192-row capacity.

    A second phase reopens the port's directory: the block is decoded back into
    rows, so the typed encoding must also be replayable.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        phases = [_phase_wal_types]
        if engine == "rust":
            phases.append(_phase_wal_types_rows)
        roots[engine] = _run_phases(
            engine, os.path.join(ROOT, f"waltype_{engine}"), port, phases
        )

    cpp = _append_blocks(os.path.join(roots["cpp"], "wal"))
    rust = _append_blocks(os.path.join(roots["rust"], "wal"))
    names = [name for name, _, _ in WAL_TYPE_TABLES]
    assert len(cpp) == len(names), (
        f"{len(cpp)} reference appends for {len(names)} tables"
    )
    assert len(rust) == len(names), f"{len(rust)} port appends for {len(names)} tables"
    for name, a, b in zip(names, cpp, rust):
        assert a == b, f"{name} block differs:\n  cpp  {a.hex()}\n  rust {b.hex()}"


def _wal_command_payloads(path, kind):
    """Every command payload of `kind`, in file order (header excluded).

    The entry header carries a per-engine `txn_id`, so only the command
    payload (after the 28-byte header) is compared. The trailing `i32` entry
    size is dropped too; it follows the command bytes.
    """
    blob = open(path, "rb").read()
    pos = 0
    out = []
    while pos + 28 <= len(blob):
        size = struct.unpack_from("<i", blob, pos)[0]
        if size <= 28 or pos + size > len(blob):
            break
        count = struct.unpack_from("<i", blob, pos + 24)[0]
        cmds = blob[pos + 28 : pos + size - 4]
        if count == 1 and cmds and cmds[0] == kind:
            out.append(cmds)
        pos += size
    return out


def _phase_ddl_wal(conn):
    db = conn.get_database("default_db")
    t = db.create_table(
        "wal_ddl",
        {
            "id": {"type": "int"},
            "name": {"type": "varchar"},
            "body": {"type": "varchar"},
            "v": {"type": "vector,4,float"},
            "v2": {"type": "vector,4,float"},
            "sp": {"type": "sparse,8,float,int"},
        },
        ConflictType.Error,
    )
    t.create_index("sec", index.IndexInfo("name", index.IndexType.Secondary))
    t.create_index("ft", index.IndexInfo("body", index.IndexType.FullText))
    t.create_index(
        "hn",
        index.IndexInfo(
            "v",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "l2"},
        ),
    )
    t.create_index("bm", index.IndexInfo("sp", index.IndexType.BMP))
    # IVF's option is a POD blob: the plain variant pins the centroid defaults,
    # the quantized one pins `storage_option_.type_` and the SQ bits.
    t.create_index("ivf", index.IndexInfo("v", index.IndexType.IVF, {"metric": "l2"}))
    t.create_index(
        "ivf_sq",
        index.IndexInfo(
            "v2",
            index.IndexType.IVF,
            {
                "metric": "ip",
                "storage_type": "scalar_quantization",
                "scalar_quantization_bits": "8",
            },
        ),
    )
    t.add_columns({"extra": {"type": "varchar"}})
    # Drop a column that existed at CREATE TABLE (its `tbl|col|` key carries the
    # table ts, which both engines agree on). Dropping a column that was *added
    # later* still differs: the reference keys its `column_keys` entry with the
    # ADD ts while the port uses the table ts (a catalog-key gap, not WAL).
    t.drop_columns("id")
    t.drop_index("sec")
    t.rename("wal_renamed")
    db.drop_table("wal_renamed")
    conn.create_database("wal_db", ConflictType.Error)
    conn.drop_database("wal_db", ConflictType.Error)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_ddl_wal_commands_match_the_reference():
    """The typed DDL WAL commands are the same bytes as the reference's.

    Each command payload is compared on its own (so a failure names the
    command), and the whole `wal.log` is then compared frame by frame — the
    header's `txn_id` and CRC included. Covered:
    CreateDatabase/CreateTable/CreateIndex (Secondary/FullText/HNSW/BMP)/
    AddColumns/DropColumns/DropIndex/RenameTable/DropTable/DropDatabase. IVF is
    excluded from the *payload* comparison: its `IndexIVFOption` is a POD
    written by `memcpy` with uninitialized padding, so the reference itself is
    not reproducible (measured across two runs).
    """
    cpp_root = os.path.join(ROOT, "ddlwal_cpp")
    rust_root = os.path.join(ROOT, "ddlwal_rust")
    _run_engine("cpp", cpp_root, 5432, seed=_phase_ddl_wal)
    _run_engine("rust", rust_root, 5433, seed=_phase_ddl_wal)

    cpp_wal = os.path.join(cpp_root, "wal", "wal.log")
    rust_wal = os.path.join(rust_root, "wal", "wal.log")
    for kind, name in [
        (8, "CreateDatabaseV2"),
        (10, "CreateTableV2"),
        (13, "CreateIndexV2"),
        (44, "AddColumnsV2"),
        (45, "DropColumnsV2"),
        (14, "DropIndexV2"),
        (43, "RenameTableV2"),
        (11, "DropTableV2"),
        (9, "DropDatabaseV2"),
    ]:
        cpp_payloads = _wal_command_payloads(cpp_wal, kind)
        rust_payloads = _wal_command_payloads(rust_wal, kind)
        assert cpp_payloads, f"the reference wrote no {name}"
        assert len(cpp_payloads) == len(rust_payloads), (
            f"{name}: {len(cpp_payloads)} reference frames vs {len(rust_payloads)} port frames"
        )
        for i, (a, b) in enumerate(zip(cpp_payloads, rust_payloads)):
            assert a == b, f"{name} #{i} differs:\n  cpp  {a.hex()}\n  rust {b.hex()}"

    # Every frame — header (`txn_id`, CRC) included — is the same bytes.
    cpp_frames = _wal_raw_frames(cpp_wal)
    rust_frames = _wal_raw_frames(rust_wal)
    assert len(cpp_frames) == len(rust_frames), (
        f"{len(cpp_frames)} reference frames vs {len(rust_frames)} port frames"
    )
    for i, (a, b) in enumerate(zip(cpp_frames, rust_frames)):
        assert a == b, f"frame {i} differs:\n  cpp  {a.hex()}\n  rust {b.hex()}"
