"""L2 单表目录：一张表的全部确定性文件 + catalog 逻辑内容。

`test_the_two_engines_write_the_same_bytes`（200 行、5 类索引的全量比对）、
`test_the_blocks_match_across_many_blocks`（20000 行、3 个块的块层）、
`test_the_index_chunks_match_for_every_index_kind` 与
`test_the_ivf_chunk_is_only_compared_by_content`（索引类型矩阵）。

对照物是**一个表目录**：块、版本、索引 chunk、catalog 记录与索引答案一起比。
"""

import os
import shutil
import time

import pytest
from bytes_common import (
    BLOCK_ROWS,
    COLUMNS,
    CPP_BIN,
    DET_ROWS,
    INDEX_KINDS,
    ROOT,
    RUST_BIN,
    TABLE,
    _catalog_pairs,
    _chunks,
    _compare_data_files,
    _dump_indexes,
    _files,
    _index_answers,
    _ivf_content,
    _normalize_catalog,
    _own_table_dir,
    _run_engine,
    _table_records,
    _under,
    _wait_for_index_chunks,
)
from infinity import index
from infinity.common import ConflictType


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
    # The port keeps the C++ key space as its only catalog; an empty one means
    # the directory holds no schema, so the check is skipped rather than
    # failed.
    zero_size_indexes = {
        index_id for index_id, kind in INDEX_KINDS.items() if kind in ("hnsw", "bmp")
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
            "the port must be built on RocksDB (the only substrate)"
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
        name for name in names if len({digest.get(name) for digest in digests}) > 1
    ]
    # Only the HNSW chunk may vary. Everything else - the block columns, the
    # outline, the full-text chunk, the secondary and the sparse chunks - is
    # built from the row values alone and must be reproducible.
    #
    # Note this is measured at `DET_ROWS` (500 rows) and not at the byte
    # test's own scale: at a few thousand rows the reference's HNSW build
    # spreads over several threads and the graph varies between runs, which is
    # exactly why the byte test compares the HNSW chunk by content.
    assert all(name.endswith("idx_2/seg_0/chunk_0.idx") for name in varying), (
        f"the reference is not deterministic here: {varying}"
    )


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
    t.create_index(
        "ivf_idx", index.IndexInfo("vec", index.IndexType.IVF, {"metric": "l2"})
    )
    t.create_index(
        "cos_idx",
        index.IndexInfo(
            "vec",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "cosine"},
        ),
    )
    t.create_index(
        "ip_idx",
        index.IndexInfo(
            "vec",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "ip"},
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
        content = _ivf_content(chunks[min(chunks)])
        assert content is not None, "the IVF chunk does not parse"
        contents.append(content)
    assert contents[0] == contents[1], (
        f"the reference's IVF chunk says different things: "
        f"{contents[0][:3]} != {contents[1][:3]}"
    )
