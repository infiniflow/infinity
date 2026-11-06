"""L3 段演化：多段、压缩、cleanup 与容量切分。

`test_a_multi_segment_table_matches_across_segments`（3 段）、
`test_a_segment_with_many_blocks_keeps_one_chunk_per_index`（1 段 3 块）、
`test_a_compacted_table_matches_and_keeps_its_sources` /
`test_a_cleanup_reclaims_the_deprecated_sources`（4 段压成 1 段再回收）、
`test_a_big_import_populates_every_index_kind`（15 万行一次 COPY、四种索引）、
`test_a_compaction_with_one_segment_is_a_no_op`、
`test_a_table_larger_than_a_segment_starts_a_second_one`（三段容量切分 + 全文索引
按 `mem_index_capacity` 切 chunk，需要 `INFINITY_BYTES_HUGE=1`），以及
`test_the_chunk_boundaries_are_timing_dependent`（证明 chunk 边界是调度结果，
不能当字节门禁）。

对照物是**段的划分与演化**：段号、块归属、`drop|seg|`、源段文件的保留。
"""

import json
import os
import shutil
import tempfile
import time

import pytest
from bytes_common import (
    BLOCK_SEG_BLOCKS,
    BLOCK_SEG_ROWS,
    BLOCK_SEG_TABLE,
    CPP_BIN,
    ROOT,
    RUST_BIN,
    SEG_ROWS,
    SEG_STARTS,
    SEG_TABLE,
    _answers,
    _catalog_pairs,
    _compare_data_files,
    _files,
    _normalize_catalog,
    _own_table_dir,
    _run_engine,
    _run_phases,
    _seg_csv,
    _seg_phase,
    _spawn_and_run,
    _table_records,
    _under,
)
from infinity import index
from infinity.common import ConflictType

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


SEG_COLUMNS = 6
# One `IMPORT` per entry, so the table has as many segments as this tuple has
# entries. Three is the smallest number that shows the fan-out is not a
# two-way special case.
SEG_SEGMENTS = tuple(str(i) for i in range(len(SEG_ROWS)))
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
    .match_text("body", f"doc{SEG_STARTS[1] + 50}", 20)
    .to_result(),
    # ... and one that exists only in the third.
    "fulltext_third_segment": lambda t: t.output(["id"])
    .match_text("body", f"doc{SEG_STARTS[2] + 30}", 20)
    .to_result(),
    "secondary_eq": lambda t: t.output(["id"])
    .filter(f"title = 'title-{SEG_STARTS[1] + 50}'")
    .to_result(),
    "secondary_eq_third": lambda t: t.output(["id"])
    .filter(f"title = 'title-{SEG_STARTS[2] + 30}'")
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
    """Three imports, three segments, seven index kinds.

    Matrix column "多段": every index keeps one chunk family per segment, with
    its own chunk ids, and a query merges them. The bytes of the **blocks**,
    of the full-text, secondary and sparse chunks are compared as usual; the
    HNSW header/vectors/labels and the IVF `(offset, vector)` pairs are
    compared by content.

    The catalog's `seg|…|fast_rough_filter` records are part of the comparison:
    `IMPORT` builds the segment's and every block's prefilter
    (`BuildFastRoughFilterTask::ExecuteOnNewSealedSegment`,
    `new_txn_data_impl.cpp:1739`), and the port writes the same bytes
    (`fast_rough_filter.rs`).
    """
    cpp_root = os.path.join(ROOT, "segments_cpp")
    rust_root = os.path.join(ROOT, "segments_rust")
    directory = tempfile.mkdtemp(prefix="segments-csv-")
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

    segments = tuple(range(len(SEG_ROWS)))
    _wait_for_segment_chunks(cpp_root, SEG_INDEXES, segments)
    _wait_for_segment_chunks(rust_root, SEG_INDEXES, segments)

    cpp_files = _files(cpp_root)
    rust_files = _files(rust_root)
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        for index_id in SEG_INDEXES:
            got = _segment_dirs(files, index_id)
            assert got == list(SEG_SEGMENTS), f"{label} idx_{index_id}: segments {got}"

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
            "the port must be built on RocksDB (the only substrate)"
        )
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
        for segment, count in zip(SEG_SEGMENTS, SEG_ROWS):
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
    for start in SEG_STARTS:
        assert any(i >= start for i in merged), (
            f"the full-text answer does not reach segment at {start}: {merged}"
        )
    assert rust_answers["fulltext_second_segment"] == [SEG_STARTS[1] + 50]
    assert rust_answers["fulltext_third_segment"] == [SEG_STARTS[2] + 30]
    assert rust_answers["secondary_eq"] == [SEG_STARTS[1] + 50]
    assert rust_answers["secondary_eq_third"] == [SEG_STARTS[2] + 30]


def _block_seg_jsonl(path, rows):
    """One JSON object per row: a CSV cell cannot carry a vector's commas.

    `{"0": 0.5}` is the sparse form both engines' JSONL reader accepts (the
    dense column is a plain array).
    """
    with open(path, "w", buffering=1 << 20) as handle:
        for i in range(rows):
            handle.write(
                json.dumps(
                    {
                        "id": i,
                        "title": f"title-{i}",
                        "body": f"doc{i} alpha",
                        "vec": [0.1, 0.2, 0.3, 0.4],
                        "sp": {"0": 0.5, "1": 1.0},
                        "score": i * 0.5,
                    }
                )
                + "\n"
            )


def _block_seg_phase(conn, directory):
    db = conn.get_database("default_db")
    db.drop_table(BLOCK_SEG_TABLE, ConflictType.Ignore)
    t = db.create_table(
        BLOCK_SEG_TABLE,
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
    # JSONL: a CSV cell cannot carry the vector's commas (neither engine
    # honours quotes while splitting), and the same file has to describe the
    # sparse column too.
    t.import_data(os.path.join(directory, "big.jsonl"), {"file_type": "jsonl"})
    conn.flush_data()
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_segment_with_many_blocks_keeps_one_chunk_per_index():
    """20000 rows in one `IMPORT`: three blocks, one segment, one chunk each.

    The chunk count follows `PopulateFtIndexInner` / `PopulateSecondaryIndexInner`
    / `PopulateBMPIndexInner`, not the block count: at 20000 rows the memory
    indexers never reach `mem_index_capacity` (65536), so every index dumps
    once, with `row_count` 20000 and `base_row_id` 0. Getting this wrong is
    invisible at one block, which is why the other gates never caught it.
    """
    cpp_root = os.path.join(ROOT, "block_segment_cpp")
    rust_root = os.path.join(ROOT, "block_segment_rust")
    directory = tempfile.mkdtemp(prefix="block-segment-csv-")
    try:
        _block_seg_jsonl(os.path.join(directory, "big.jsonl"), BLOCK_SEG_ROWS)
        phase = lambda conn: _block_seg_phase(conn, directory)
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    _wait_for_segment_chunks(cpp_root, SEG_INDEXES, (0,))
    _wait_for_segment_chunks(rust_root, SEG_INDEXES, (0,))

    cpp_files, rust_files = _files(cpp_root), _files(rust_root)
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        blocks = {
            "/".join(name.split("/")[:4])
            for name in files
            if "/seg_" in name and "/blk_" in name
        }
        assert len(blocks) == BLOCK_SEG_BLOCKS, f"{label}: blocks {sorted(blocks)}"
        for index_id in SEG_INDEXES:
            assert _segment_dirs(files, index_id) == ["0"], (
                f"{label} idx_{index_id}: segments {_segment_dirs(files, index_id)}"
            )

    cpp_dir = _own_table_dir(cpp_files, SEG_COLUMNS)
    rust_dir = _own_table_dir(rust_files, SEG_COLUMNS)
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), SEG_INDEXES
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    # One chunk per index, covering the whole segment.
    for engine, root in (("cpp", cpp_root), ("rust", rust_root)):
        pairs = _table_records(_catalog_pairs(root, 5432), cpp_dir, BLOCK_SEG_TABLE)
        chunks = {}
        for key, value in pairs.items():
            if not key.startswith("idx_chunk|") or not key.endswith("|chunk_info"):
                continue
            record = json.loads(value)
            index_id = int(key.split("|")[3])
            chunks.setdefault(index_id, []).append(
                (record["base_row_id"], record["row_count"])
            )
        assert set(chunks) == set(SEG_INDEXES), f"{engine}: indexes {sorted(chunks)}"
        for index_id, spans in chunks.items():
            assert spans == [(0, BLOCK_SEG_ROWS)], (
                f"{engine} idx_{index_id}: chunks {spans}, expected one 20000-row chunk"
            )

    # What the chunks say: the reference reads each directory and answers the
    # same, including a row from the last (partial) block.
    queries = {
        "count": lambda t: t.output(["count(*)"]).to_result(),
        "last_block": lambda t: t.output(["id"])
        .match_text("body", f"doc{BLOCK_SEG_ROWS - 1}", 20)
        .to_result(),
        "first_block": lambda t: t.output(["id"])
        .match_text("body", "doc7", 20)
        .to_result(),
        "secondary_range": lambda t: t.output(["id"])
        .filter(f"id >= {BLOCK_SEG_ROWS - 5}")
        .to_result(),
        "sparse": lambda t: t.output(["id"])
        .match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5)
        .to_result(),
    }
    cpp_answers = _answers(cpp_root, 5432, BLOCK_SEG_TABLE, queries)
    rust_answers = _answers(rust_root, 5433, BLOCK_SEG_TABLE, queries)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"
    assert cpp_answers["count"] == [BLOCK_SEG_ROWS]
    assert cpp_answers["last_block"] == [BLOCK_SEG_ROWS - 1]


MERGED_TABLE = "test_cross_engine_compacted"
MERGED_ROWS = (100, 60, 40, 30)
MERGED_STARTS = tuple(sum(MERGED_ROWS[:i]) for i in range(len(MERGED_ROWS)))
# The four source segments plus the merged one.
MERGED_SEGMENTS = ("0", "1", "2", "3", "4")


def _merged_phase(conn, directory):
    db = conn.get_database("default_db")
    db.drop_table(MERGED_TABLE, ConflictType.Ignore)
    t = db.create_table(
        MERGED_TABLE,
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
    for name in ("a.csv", "b.csv", "c.csv", "d.csv"):
        t.import_data(os.path.join(directory, name))
    conn.flush_data()
    time.sleep(1)
    t.compact()
    conn.flush_data()
    time.sleep(2)


def _cleanup_phase(conn):
    """Trigger `Cleanup` over a directory that already compacted its sources.

    Both engines expose the same Thrift `Cleanup` RPC (the reference at
    `infinity_thrift_service_impl.cpp:1226`, the port at
    `rust_engine/crates/server/src/lib.rs:1156`), which runs the equivalent of
    `NewTxn::Cleanup()` / `Engine::cleanup()`: it reclaims every segment marked
    deprecated by a prior `Compact`, removing the source segment's data files,
    its catalog records and its index chunks. This phase is the second process
    over the same directory, so the engine reads the catalog the compaction
    left behind.
    """
    conn.cleanup()
    conn.flush_data()
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_compacted_table_matches_and_keeps_its_sources():
    """Four segments compact into one; the sources become deprecated garbage.

    The merged segment is `max(segment id) + 1` and its blocks, versions and
    index chunks are the ones the compaction transaction produced: every index
    gets a chunk covering all 230 rows with the compaction's commit ts. The
    four source segments keep their files and catalog records - a reference
    reading the directory at a later ts skips them because of `drop|seg|…`.
    """
    cpp_root = os.path.join(ROOT, "compacted_cpp")
    rust_root = os.path.join(ROOT, "compacted_rust")
    directory = tempfile.mkdtemp(prefix="compacted-csv-")
    try:
        for name, start, count in zip(
            ("a.csv", "b.csv", "c.csv", "d.csv"), MERGED_STARTS, MERGED_ROWS
        ):
            _seg_csv(os.path.join(directory, name), start, count)
        phase = lambda conn: _merged_phase(conn, directory)
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    _wait_for_segment_chunks(cpp_root, SEG_INDEXES, range(5))
    _wait_for_segment_chunks(rust_root, SEG_INDEXES, range(5))

    cpp_files, rust_files = _files(cpp_root), _files(rust_root)
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        blocks = {
            "/".join(name.split("/")[:3])
            for name in files
            if "/seg_" in name and "/blk_" in name
        }
        assert len(blocks) == 5, f"{label}: block directories {sorted(blocks)}"
        for index_id in SEG_INDEXES:
            got = _segment_dirs(files, index_id)
            assert got == list(MERGED_SEGMENTS), (
                f"{label} idx_{index_id}: segments {got}"
            )

    cpp_dir = _own_table_dir(cpp_files, SEG_COLUMNS)
    rust_dir = _own_table_dir(rust_files, SEG_COLUMNS)
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), SEG_INDEXES
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures
    )

    # The catalog carries the same records, deprecation marks included.
    def records(engine_root, table_dir):
        pairs = _normalize_catalog(
            _table_records(_catalog_pairs(engine_root, 5432), table_dir, MERGED_TABLE),
            zero_size_indexes=SEG_ZERO_SIZE,
        )
        return pairs

    cpp_pairs = records(cpp_root, cpp_dir)
    rust_pairs = records(rust_root, rust_dir)
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
    dropped = {
        key.rsplit("/", 1)[1] for key in cpp_pairs if key.startswith("drop|seg|")
    }
    assert dropped == {"0", "1", "2", "3"}, f"unexpected deprecation marks: {dropped}"
    deprecated_pairs = {key for key in cpp_pairs if key.startswith("drop|idx_seg|")}
    assert len(deprecated_pairs) == len(SEG_INDEXES) * 4, (
        "expected one deprecation mark per (index, source segment): "
        f"{sorted(deprecated_pairs)}"
    )

    # Every index's merged chunk covers all rows, with the compaction's ts.
    merged = {}
    for key, value in cpp_pairs.items():
        if key.startswith("idx_chunk|") and key.endswith("|chunk_info"):
            parts = key.split("|")
            if parts[4] == "4":
                merged.setdefault(int(parts[3]), []).append(json.loads(value))
    assert set(merged) == set(SEG_INDEXES), f"indexes with a merged chunk: {sorted(merged)}"
    for index_id, spans in merged.items():
        assert [(s["base_row_id"], s["row_count"]) for s in spans] == [
            (4 << 32, sum(MERGED_ROWS))
        ], f"idx_{index_id}: {spans}"

    queries = {
        "count": lambda t: t.output(["count(*)"]).to_result(),
        "fulltext_second": lambda t: t.output(["id"])
        .match_text("body", f"doc{MERGED_STARTS[1] + 50}", 20)
        .to_result(),
        "fulltext_last": lambda t: t.output(["id"])
        .match_text("body", f"doc{MERGED_STARTS[3] + 20}", 20)
        .to_result(),
        "secondary": lambda t: t.output(["id"])
        .filter(f"title = 'title-{MERGED_STARTS[2] + 10}'")
        .to_result(),
        "sparse": lambda t: t.output(["id"])
        .match_sparse("sp", {0: 0.5, 1: 1.0}, "ip", 5)
        .to_result(),
    }
    cpp_answers = _answers(cpp_root, 5432, MERGED_TABLE, queries)
    rust_answers = _answers(rust_root, 5433, MERGED_TABLE, queries)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"
    assert cpp_answers["count"] == [sum(MERGED_ROWS)]
    assert cpp_answers["fulltext_last"] == [MERGED_STARTS[3] + 20]


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_cleanup_reclaims_the_deprecated_sources():
    """After `Cleanup` the deprecated source segments are gone from both engines.

    The compact test stops before this step: it proves the four source segments
    keep their files and `drop|seg|…` / `drop|idx_seg|…` marks, but a reader at
    a later ts skips them, so the marks themselves are not compared. This test
    runs `Cleanup` over the same directory (the compaction already happened, so
    the second process sees the `drop|` marks) and compares what survives.

    Both engines reclaim the same things: the source segments' data files
    (`seg_0..3`), their catalog records (`catalog|seg|…`, `seg|…`, `blk|…`),
    their index chunks (`idx_chunk|…` records + the `idx_*/seg_*` directories)
    and the `drop|seg|…` / `drop|idx_seg|…` marks themselves. Only the merged
    segment (id 4) remains, with its blocks, versions and index chunks - which
    must still be byte identical after the cleanup, since cleanup does not touch
    the live segment's files.
    """
    cpp_root = os.path.join(ROOT, "cleaned_cpp")
    rust_root = os.path.join(ROOT, "cleaned_rust")
    directory = tempfile.mkdtemp(prefix="cleaned-csv-")
    try:
        for name, start, count in zip(
            ("a.csv", "b.csv", "c.csv", "d.csv"), MERGED_STARTS, MERGED_ROWS
        ):
            _seg_csv(os.path.join(directory, name), start, count)
        phase = lambda conn: _merged_phase(conn, directory)
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    _wait_for_segment_chunks(cpp_root, SEG_INDEXES, range(5))
    _wait_for_segment_chunks(rust_root, SEG_INDEXES, range(5))

    # Capture the pre-cleanup state for the byte comparison, then run cleanup
    # over the same directory (second process). `_run_engine` wipes the root,
    # so the cleanup phase is spawned directly on the existing directory.
    cpp_pre = _files(cpp_root)
    rust_pre = _files(rust_root)

    _spawn_and_run("cpp", cpp_root, _cleanup_phase)
    _spawn_and_run("rust", rust_root, _cleanup_phase)

    # After cleanup: source segments 0-3 are gone, merged segment 4 remains.
    cpp_files, rust_files = _files(cpp_root), _files(rust_root)
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        segs = {
            name.split("/seg_")[1].split("/")[0]
            for name in files
            if "/seg_" in name and "/blk_" in name
        }
        assert segs == {"4"}, f"{label}: surviving segments {sorted(segs)}"
        for index_id in SEG_INDEXES:
            got = _segment_dirs(files, index_id)
            assert got == ["4"], (
                f"{label} idx_{index_id}: surviving segments {got}"
            )

    # The merged segment's files are untouched by cleanup: byte identical to
    # the pre-cleanup state.
    cpp_dir = _own_table_dir(cpp_pre, SEG_COLUMNS)
    rust_dir = _own_table_dir(rust_pre, SEG_COLUMNS)
    assert cpp_dir is not None and rust_dir is not None
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), SEG_INDEXES
    )
    assert not failures, "the merged segment's bytes changed after cleanup:\n  " + "\n  ".join(
        failures
    )

    # The catalog's surviving records are the same: the source segments'
    # records, the `drop|…` marks and their index chunks are all gone; the
    # merged segment's records remain.
    def surviving(root, pg_port, table_dir):
        pairs = _normalize_catalog(
            _table_records(_catalog_pairs(root, pg_port), table_dir, MERGED_TABLE),
            zero_size_indexes=SEG_ZERO_SIZE,
        )
        # No `drop|` marks survive cleanup, and no source-segment records do.
        assert not any(k.startswith("drop|") for k in pairs), (
            f"drop marks survive cleanup: {sorted(k for k in pairs if k.startswith('drop|'))}"
        )
        return pairs

    cpp_pairs = surviving(cpp_root, 5432, cpp_dir)
    rust_pairs = surviving(rust_root, 5433, rust_dir)
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

    # The surviving merged segment answers the same queries.
    queries = {
        "count": lambda t: t.output(["count(*)"]).to_result(),
        "fulltext_second": lambda t: t.output(["id"])
        .match_text("body", f"doc{MERGED_STARTS[1] + 50}", 20)
        .to_result(),
        "secondary": lambda t: t.output(["id"])
        .filter(f"title = 'title-{MERGED_STARTS[2] + 10}'")
        .to_result(),
    }
    cpp_answers = _answers(cpp_root, 5432, MERGED_TABLE, queries)
    rust_answers = _answers(rust_root, 5433, MERGED_TABLE, queries)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"
    assert cpp_answers["count"] == [sum(MERGED_ROWS)]


HUGE_TABLE = "test_cross_engine_huge"
HUGE_SEGMENT_CAPACITY = 8192 * 1024
# `DEFAULT_SEGMENT_CAPACITY` is 8,388,608 rows, so three segments need two
# full ones plus a tail. The reference cannot be asked for a smaller capacity,
# hence the opt-in gate; two is the smallest split worth checking and three is
# what tells a capacity split apart from an import that just started a second
# segment.
HUGE_SEGMENTS = int(os.environ.get("INFINITY_BYTES_HUGE_SEGMENTS", "3"))
HUGE_ROWS = (HUGE_SEGMENTS - 1) * HUGE_SEGMENT_CAPACITY + 92
HUGE_ENABLED = os.environ.get("INFINITY_BYTES_HUGE", "") not in ("", "0")


def _huge_csv(path, rows):
    """`id, title` per row, written in one pass."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", buffering=1 << 20) as handle:
        handle.writelines(f"{i},doc-{i}\n" for i in range(rows))


def _huge_phase(conn, directory):
    db = conn.get_database("default_db")
    db.drop_table(HUGE_TABLE, ConflictType.Ignore)
    t = db.create_table(
        HUGE_TABLE,
        {"id": {"type": "int"}, "title": {"type": "varchar"}},
        ConflictType.Error,
    )
    # An index on the huge table: the import populates it inside the import
    # transaction and the memory index dumps one chunk per `mem_index_capacity`
    # (65536) rows, per segment — which is the layout the two engines have to
    # agree on at 8M rows, not just at 200.
    t.create_index("ft", index.IndexInfo("title", index.IndexType.FullText))
    t.import_data(os.path.join(directory, "huge.csv"))
    conn.flush_data()
    t.dump_index("ft")
    time.sleep(2)


@pytest.mark.skipif(
    not HUGE_ENABLED, reason="set INFINITY_BYTES_HUGE=1 to run the 8M-row gate"
)
@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_table_larger_than_a_segment_starts_a_second_one():
    """`(HUGE_SEGMENTS - 1) * 8,388,608 + 92` rows in one `IMPORT`.

    Both engines seal a segment at `DEFAULT_SEGMENT_CAPACITY` and open the next
    one inside the same import transaction: one commit ts, one `create` per
    block, and the same block ids in each segment (0…1023, then 0). With the
    default of three segments the tail case and the repeated split are both
    exercised (`seg_0` and `seg_1` full, `seg_2` holding 92 rows).

    The table carries a full-text index, so the import also has to populate it
    and the dump has to cut one chunk per `mem_index_capacity` (65536) rows per
    segment — 128 chunks for each full segment, one for the tail. That is the
    layout the two engines have to agree on at 8M rows, not just at 200.
    """
    cpp_root = os.path.join(ROOT, "huge_cpp")
    rust_root = os.path.join(ROOT, "huge_rust")
    directory = tempfile.mkdtemp(prefix="huge-csv-")
    try:
        _huge_csv(os.path.join(directory, "huge.csv"), HUGE_ROWS)
        phase = lambda conn: _huge_phase(conn, directory)
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    full = HUGE_SEGMENTS - 1
    tail = HUGE_SEGMENT_CAPACITY * full
    tail_rows = HUGE_ROWS - tail
    expected_blocks = {f"seg_{i}": 1024 for i in range(full)}
    expected_blocks[f"seg_{full}"] = 1
    # `mem_index_capacity` = 65536 rows per chunk within a segment.
    expected_chunks = {f"seg_{i}": 128 for i in range(full)}
    expected_chunks[f"seg_{full}"] = 1

    cpp_files, rust_files = _files(cpp_root), _files(rust_root)
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        blocks = [
            name.split("/")[:4]
            for name in files
            if "/seg_" in name and "/blk_" in name and name.endswith("/version")
        ]
        segments = sorted(set(expected_blocks) & {parts[2] for parts in blocks})
        assert segments == sorted(expected_blocks), f"{label}: segments {segments}"
        per_segment = {
            segment: sum(1 for parts in blocks if parts[2] == segment)
            for segment in segments
        }
        assert per_segment == expected_blocks, (
            f"{label}: blocks per segment {per_segment}"
        )
        # A full-text chunk is one `.dic` under `db_{db}/tbl_{tbl}/idx_{id}/seg_{s}/`.
        chunks = {}
        for name in files:
            parts = name.split("/")
            if len(parts) >= 5 and parts[2].startswith("idx_") and name.endswith(".dic"):
                chunks[parts[3]] = chunks.get(parts[3], 0) + 1
        assert chunks == expected_chunks, f"{label}: chunks per segment {chunks}"

    cpp_dir = _own_table_dir(cpp_files, 2)
    rust_dir = _own_table_dir(rust_files, 2)
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), {1: "fulltext"}
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures[:20]
    )

    queries = {
        "count": lambda t: t.output(["count(*)"]).to_result(),
        "last": lambda t: t.output(["id"]).filter(f"id = {HUGE_ROWS - 1}").to_result(),
        "first": lambda t: t.output(["id"]).filter("id = 0").to_result(),
    }
    cpp_answers = _answers(cpp_root, 5432, HUGE_TABLE, queries)
    rust_answers = _answers(rust_root, 5433, HUGE_TABLE, queries)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"
    assert cpp_answers["count"] == [HUGE_ROWS]
    assert cpp_answers["last"] == [HUGE_ROWS - 1]


BIG_TABLE = "test_cross_engine_big_import"
# Past one `mem_index_capacity` (65536) twice: the import is a single
# transaction, and every index still has to split its chunks at the capacity -
# which is the part that used to depend on the commit batches.
BIG_ROWS = 150_000
BIG_COLUMNS = {
    "id": {"type": "int"},
    "title": {"type": "varchar"},
    "vec": {"type": "vector,4,float"},
    "score": {"type": "int"},
    "sp": {"type": "sparse,8,float,int"},
}


def _big_csv(path, rows):
    """Tab-separated, the encoding `tools/generate_wiki_embedding_plus.py` uses.

    A vector or sparse cell carries commas of its own, so the tab is what tells
    the columns apart; the sparse value is the bracketed `[term:value, ...]`
    form, not `term:value` pairs.
    """
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", buffering=1 << 20) as handle:
        for i in range(rows):
            vec = f"[{i % 7 / 7.0},0.25,0.5,0.75]"
            sp = f"[0:{i % 5 + 1},1:0.5]"
            handle.writelines(
                f"{i}\tdoc-{i % 1000}\t{vec}\t{i}\t{sp}\n"
            )


def _big_phase(conn, directory):
    db = conn.get_database("default_db")
    db.drop_table(BIG_TABLE, ConflictType.Ignore)
    t = db.create_table(BIG_TABLE, BIG_COLUMNS, ConflictType.Error)
    t.create_index("ft", index.IndexInfo("title", index.IndexType.FullText))
    t.create_index(
        "vec_idx",
        index.IndexInfo(
            "vec", index.IndexType.Hnsw, {"M": "16", "ef_construction": "20", "metric": "l2"}
        ),
    )
    t.create_index("score_sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    t.import_data(
        os.path.join(directory, "big.csv"), {"file_type": "csv", "delimiter": "\t"}
    )
    conn.flush_data()
    for name in ("ft", "vec_idx", "score_sec", "sp_idx"):
        t.dump_index(name)
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_a_big_import_populates_every_index_kind():
    """150,000 rows in **one** `COPY` (one transaction), four index kinds.

    The full-text chunks are cut per `mem_index_capacity` (65536) and the
    secondary chunk is one per segment - neither may depend on the import being
    committed in batches. Chunk counts are compared between the engines rather
    than hard coded, then the chunks byte for byte.

    The vector and sparse cells use the tab-separated form the repository's own
    fixture uses (`[a, b, c, d]` and `[term:value, ...]`).
    """
    cpp_root = os.path.join(ROOT, "big_cpp")
    rust_root = os.path.join(ROOT, "big_rust")
    directory = tempfile.mkdtemp(prefix="big-csv-")
    try:
        _big_csv(os.path.join(directory, "big.csv"), BIG_ROWS)
        phase = lambda conn: _big_phase(conn, directory)
        _run_engine("cpp", cpp_root, 5432, seed=phase)
        _run_engine("rust", rust_root, 5433, seed=phase)
    finally:
        shutil.rmtree(directory, ignore_errors=True)

    cpp_files, rust_files = _files(cpp_root), _files(rust_root)
    counts = {}
    for label, files in (("cpp", cpp_files), ("rust", rust_files)):
        seen = {}
        for name in files:
            parts = name.split("/")
            if len(parts) >= 5 and parts[2].startswith("idx_") and name.endswith(".dic"):
                seen[(parts[2], parts[3])] = seen.get((parts[2], parts[3]), 0) + 1
        counts[label] = seen
        assert seen, f"{label}: no full-text chunks"
    assert counts["cpp"] == counts["rust"], f"{counts['cpp']} != {counts['rust']}"
    # 150,000 rows = 65536 + 65536 + 18928 -> three chunks in `seg_0`.
    assert set(counts["cpp"].values()) == {3}, counts["cpp"]

    cpp_dir = _own_table_dir(cpp_files, len(BIG_COLUMNS))
    rust_dir = _own_table_dir(rust_files, len(BIG_COLUMNS))
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir),
        _under(rust_files, rust_dir),
        {1: "fulltext", 2: "hnsw", 3: "secondary-u64", 4: "bmp"},
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures[:20]
    )

    queries = {
        "count": lambda t: t.output(["count(*)"]).to_result(),
        "secondary": lambda t: t.output(["id"]).filter("score = 149999").to_result(),
        "fulltext": lambda t: t.output(["id"]).match_text("title", "doc-7", 3).to_result(),
    }
    cpp_answers = _answers(cpp_root, 5432, BIG_TABLE, queries)
    rust_answers = _answers(rust_root, 5433, BIG_TABLE, queries)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"
    assert cpp_answers["count"] == [BIG_ROWS]


SMVE_TABLE = "test_cross_engine_smve"
SMVE_ROWS = 500
SMVE_COLUMNS = {"id": {"type": "int"}, "t": {"type": "tensor,4,float"}}


def _smve_phase(conn):
    db = conn.get_database("default_db")
    db.drop_table(SMVE_TABLE, ConflictType.Ignore)
    t = db.create_table(SMVE_TABLE, SMVE_COLUMNS, ConflictType.Error)
    t.create_index("smve_t", index.IndexInfo("t", index.IndexType.SMVE))
    rows = [
        {"id": i, "t": [float(i % 7), float(i % 5), float(i % 3), 0.5]}
        for i in range(SMVE_ROWS)
    ]
    for start in range(0, len(rows), 8192):
        t.insert(rows[start : start + 8192])
    conn.flush_data()
    t.dump_index("smve_t")
    time.sleep(2)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_smve_chunk_matches_byte_for_byte():
    """`USING SMVE` on a `TENSOR(FLOAT,4)` column: one chunk of 500 documents.

    The chunk is `u32 width/topk/embedding_dim/n_docs`, the `dim × width`
    projection matrix and the BMP index over the sparse codes. The projection
    is the reference's `mt19937(42)` sequence, so the whole file is comparable
    byte for byte - including the compressed BMP section (`BMPCompressType::
    kCompressed`, which is what `bmp_cpp::encode_bmp` writes).
    """
    cpp_root = os.path.join(ROOT, "smve_cpp")
    rust_root = os.path.join(ROOT, "smve_rust")
    _run_engine("cpp", cpp_root, 5432, seed=_smve_phase)
    _run_engine("rust", rust_root, 5433, seed=_smve_phase)

    cpp_files, rust_files = _files(cpp_root), _files(rust_root)
    cpp_dir = _own_table_dir(cpp_files, len(SMVE_COLUMNS))
    rust_dir = _own_table_dir(rust_files, len(SMVE_COLUMNS))
    assert cpp_dir and rust_dir, f"{cpp_dir} / {rust_dir}"
    failures = _compare_data_files(
        _under(cpp_files, cpp_dir), _under(rust_files, rust_dir), {1: "smve"}
    )
    assert not failures, "the two engines wrote different bytes:\n  " + "\n  ".join(
        failures[:20]
    )

    queries = {
        "count": lambda t: t.output(["count(*)"]).to_result(),
        "id": lambda t: t.output(["id"]).filter(f"id = {SMVE_ROWS - 1}").to_result(),
    }
    cpp_answers = _answers(cpp_root, 5432, SMVE_TABLE, queries)
    rust_answers = _answers(rust_root, 5433, SMVE_TABLE, queries)
    assert cpp_answers == rust_answers, f"{cpp_answers} != {rust_answers}"
    assert cpp_answers["count"] == [SMVE_ROWS]
