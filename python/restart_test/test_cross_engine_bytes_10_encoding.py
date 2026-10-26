"""L1 编码原语：一个文件、一条记录的字节。

`test_the_columns_match_for_every_type`（每种类型一列的表，块里 `.col` 与 outline）、
`test_the_sparse_index_options_reach_the_chunk`（BMP 的 `block_size` 进了 chunk 字节）、
`test_the_fast_rough_filters_match_byte_for_byte`（`seg|…` / `blk|…|fast_rough_filter`
的 130 字节记录）。

对照物只有一个文件或一条记录，失败时不必读段/表布局：问题一定在编码本身。
"""

import json
import os
import shutil
import subprocess
import tempfile
import time

import pytest
from bytes_common import (
    CPP_BIN,
    ROOT,
    RUST_BIN,
    RUST_CLI,
    SEG_ROWS,
    SEG_STARTS,
    _assert_chunk_tiling,
    _catalog_pairs,
    _chunks,
    _compare_data_files,
    _dump_indexes,
    _files,
    _own_table_dir,
    _run_engine,
    _seg_csv,
    _seg_phase,
    _table_records,
    _under,
    _wait_for_index_chunks,
)
from infinity import index
from infinity.common import ConflictType

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



def _raw_catalog_hex(root):
    """Every catalog value as `key -> hex`, read straight out of the store.

    `SHOW CATALOG 'all'` returns a binary value through the Postgres text
    protocol, which turns the `fast_rough_filter` records into an empty string
    - that is why the gate could only assert that the record *exists*. The Rust
    CLI's `dump-catalog` opens the same RocksDB catalog and prints the bytes as
    hex, so the gate can compare them byte for byte.
    """
    out = subprocess.run(
        [RUST_CLI, "dump-catalog", root],
        capture_output=True,
        text=True,
    )
    assert out.returncode == 0, (
        f"dump-catalog failed on {root}: rc={out.returncode} {out.stderr.strip()}"
    )
    pairs = {}
    for line in out.stdout.splitlines():
        if "\t" not in line:
            continue
        key, hexvalue = line.split("\t", 1)
        pairs[key] = hexvalue
    return pairs


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_CLI, os.X_OK), reason=f"{RUST_CLI} is not executable"
)
def test_the_fast_rough_filters_match_byte_for_byte():
    """The `fast_rough_filter` records are the same bytes, not just present.

    `BuildFastRoughFilterTask::ExecuteOnNewSealedSegment`
    (`new_txn_data_impl.cpp:1739`) writes one record per sealed segment
    (`seg|…|fast_rough_filter`) and one per block (`blk|…|fast_rough_filter`).
    They are binary, so `show catalog 'all'` renders them as an empty string and
    the gate used to stop at "the key exists". This reads the bytes out of the
    catalog store instead and compares them: three `IMPORT`s seal three
    segments, so both levels are covered.
    """
    cpp_root = os.path.join(ROOT, "frf_cpp")
    rust_root = os.path.join(ROOT, "frf_rust")
    directory = tempfile.mkdtemp(prefix="frf-csv-")
    try:
        for name, start, count in zip(
            ("a.csv", "b.csv", "c.csv"), SEG_STARTS, SEG_ROWS
        ):
            _seg_csv(os.path.join(directory, name), start, count)
        phase = lambda conn: _seg_phase(conn, directory)
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    cpp_hex = _raw_catalog_hex(cpp_root)
    rust_hex = _raw_catalog_hex(rust_root)

    def filters(pairs):
        return {k: v for k, v in pairs.items() if "fast_rough_filter" in k}

    cpp_filters = filters(cpp_hex)
    rust_filters = filters(rust_hex)
    assert cpp_filters, "the reference wrote no fast_rough_filter record"
    assert rust_filters, "the port wrote no fast_rough_filter record"

    # Both levels are present: one per sealed segment and one per block.
    for label, pairs in (("cpp", cpp_filters), ("rust", rust_filters)):
        assert any(k.startswith("seg|") for k in pairs), (
            f"{label}: no segment-level fast_rough_filter {sorted(pairs)}"
        )
        assert any(k.startswith("blk|") for k in pairs), (
            f"{label}: no block-level fast_rough_filter {sorted(pairs)}"
        )
        # A binary value: not an empty string once hex-encoded.
        assert all(len(v) > 0 and len(v) % 2 == 0 for v in pairs.values()), (
            f"{label}: a fast_rough_filter record is empty or malformed"
        )

    only_cpp = sorted(set(cpp_filters) - set(rust_filters))
    only_rust = sorted(set(rust_filters) - set(cpp_filters))
    changed = [
        f"{key}:\n    cpp  {cpp_filters[key]}\n    rust {rust_filters[key]}"
        for key in sorted(set(cpp_filters) & set(rust_filters))
        if cpp_filters[key] != rust_filters[key]
    ]
    assert not (only_cpp or only_rust or changed), (
        f"only in cpp: {only_cpp}\nonly in rust: {only_rust}\n" + "\n".join(changed)
    )


def _phase_catalog_keys(conn):
    """The smallest workload that touches every catalog record family."""
    db = conn.get_database("default_db")
    db.drop_table("kb", ConflictType.Ignore)
    t = db.create_table(
        "kb",
        {"id": {"type": "int"}, "v": {"type": "vector,4,float"}},
        ConflictType.Error,
    )
    t.insert(
        [
            {"id": 1, "v": [0.1, 0.2, 0.3, 0.4]},
            {"id": 2, "v": [0.5, 0.6, 0.7, 0.8]},
            {"id": 3, "v": [0.9, 1.0, 1.1, 1.2]},
        ]
    )
    t.delete("id = 2")


@pytest.mark.xfail(
    strict=True,
    reason=(
        "the port's catalog still holds four keys the reference does not "
        "(`__counter:table`, `db:<name>`, `tbl:<id>`, `tblname:<db>.<tbl>`) and "
        "a separate row store under persistence/; see PORTING.md §3.1"
    ),
)
@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_catalog_key_set_and_values_match_exactly():
    """The catalog holds **exactly** the reference's keys, with equal values.

    Not "the records we know about match" - the whole key space. A key the port
    writes and the reference does not (its own bookkeeping) is as much a
    divergence as a value that differs, because a directory must be readable by
    either engine and hold nothing the reference would not have written.
    """
    cpp_root = os.path.join(ROOT, "catalog_keys_cpp")
    rust_root = os.path.join(ROOT, "catalog_keys_rust")
    _run_engine("cpp", cpp_root, 5432, seed=_phase_catalog_keys)
    _run_engine("rust", rust_root, 5433, seed=_phase_catalog_keys)

    cpp = _raw_catalog_hex(cpp_root)
    rust = _raw_catalog_hex(rust_root)

    only_cpp = sorted(set(cpp) - set(rust))
    only_rust = sorted(set(rust) - set(cpp))
    assert not only_rust, f"keys the port writes and the reference does not: {only_rust}"
    assert not only_cpp, f"keys the reference writes and the port does not: {only_cpp}"

    differing = sorted(k for k in cpp if cpp[k] != rust[k])
    assert not differing, "same key, different value: " + ", ".join(
        f"{k} ({cpp[k]} vs {rust[k]})" for k in differing[:5]
    )
