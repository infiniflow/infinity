"""L5 变更与恢复：DELETE / UPDATE / 重启 / 崩溃之后。

`test_the_bytes_after_deletes_updates_and_a_restart`（块与 `version` 的删除槽）、
`test_the_secondary_and_sparse_indexes_survive_deletes_updates_and_a_restart`、
`test_the_vector_indexes_survive_deletes_updates_and_a_restart`、
`test_the_quantized_ivf_indexes_mirror_and_answer`、
`test_the_sparse_search_options_change_the_answer_the_same_way`、
`test_a_crash_leaves_a_replayable_directory`（未 flush 的写在 WAL 里，另一个引擎
replay）、`test_the_indexes_survive_a_crash`、
`test_the_quantized_ivf_indexes_survive_a_crash`。

对照物是**一轮写入历史的结果**：同样的语句序列之后，两个目录必须仍然同形，
且索引答案相同。
"""

import json
import os
import time

import pytest
from bytes_common import (
    CPP_BIN,
    CRASH_COLUMNS,
    ROOT,
    RUST_BIN,
    _answers,
    _assert_chunk_tiling,
    _catalog_pairs,
    _compare_data_files,
    _dump_indexes,
    _files,
    _normalize_catalog,
    _own_table_dir,
    _run_phases,
    _spawn_and_run,
    _table_records,
    _under,
    _wait_for_index_chunks,
    _write_config,
)
from infinity import index
from infinity.common import ConflictType

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


SPARSE_OPT_TABLE = "test_cross_engine_sparse_options"
SPARSE_OPT_COLUMNS = {
    "id": {"type": "int"},
    "sp": {"type": "sparse,8,float,int"},
}
# `alpha`/`beta`/`tail`/`threshold` are the names the reference's binder admits
# (`expression_binder_impl.cpp:1140`); each variant is `(name, opt_params, topn)`.
SPARSE_OPT_QUERIES = [
    ("default", None, 5),
    ("beta_one_term", {"beta": "0.34"}, 5),
    ("alpha_half", {"alpha": "0.5"}, 5),
    ("tail_noop", {"tail": "F"}, 5),
    ("threshold", {"threshold": "0.9"}, 5),
    ("all_set", {"alpha": "0.7", "beta": "0.67", "threshold": "0.5"}, 3),
]


def _phase_sparse_options(conn, root):
    """Insert a three-term sparse column, dump the BMP chunk, query variants."""
    db = conn.get_database("default_db")
    db.drop_table(SPARSE_OPT_TABLE, ConflictType.Ignore)
    t = db.create_table(SPARSE_OPT_TABLE, SPARSE_OPT_COLUMNS, ConflictType.Error)
    t.create_index("sp_idx", index.IndexInfo("sp", index.IndexType.BMP))
    rows = []
    for i in range(40):
        # `beta` keeps the two strongest query terms (0.9 and 0.5) and drops
        # term 2 (0.1). Every even row gets a big term-2 bonus that dominates
        # the 0.001 steps of term 1, so dropping it reorders the answer.
        doc = {0: 1.0, 1: i * 0.001}
        if i % 2 == 0:
            doc[2] = 1.0
        rows.append({"id": i, "sp": doc})
    t.insert(rows)
    conn.flush_data()
    _dump_indexes(t, ["sp_idx"])
    time.sleep(1)
    answers = {}
    for name, opts, topn in SPARSE_OPT_QUERIES:
        data, _types, _extra = t.output(["id"]).match_sparse(
            "sp", {0: 0.9, 1: 0.5, 2: 0.1}, "ip", topn, opts
        ).to_result()
        # A variant that filters everything out answers no columns at all.
        answers[name] = (
            {name: [str(v) for v in values] for name, values in data.items()}
            if data
            else {}
        )
    # `use_tail` is not one of the binder's names: both engines reject it.
    try:
        t.output(["id"]).match_sparse(
            "sp", {0: 0.9, 1: 0.5, 2: 0.1}, "ip", 5, {"use_tail": "F"}
        ).to_result()
        answers["use_tail"] = "accepted"
    # NB: the exception's code and text are the contract here.
    except Exception as error:
        # The reference appends its source location (`…@src/…:1143`).
        answers["use_tail"] = [
            getattr(error, "error_code", None),
            str(error).split("@", 1)[0],
        ]
    with open(os.path.join(root, "sparse_options.json"), "w") as handle:
        json.dump(answers, handle)


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_sparse_search_options_change_the_answer_the_same_way():
    """`alpha` / `beta` / `tail` / `threshold` on a dumped BMP chunk.

    The options reach `BMPAlgBase::SearchKnn` in the reference: `beta` keeps
    `ceil(nnz * beta)` of the strongest query terms, `alpha` scales the
    early-stop bound, and `threshold` filters the results. The port used to pass
    only `alpha` to its on-disk reader, so a `beta` query answered differently;
    now both engines run the same variants over the same chunk and must return
    the same ids. `tail` is one of the binder's names but never reaches
    `BmpSearchOptions` in either engine, and `use_tail` is rejected by name.
    """
    roots = {}
    for engine, port in (("cpp", 5432), ("rust", 5433)):
        root = os.path.join(ROOT, f"sparseopt_{engine}")
        roots[engine] = _run_phases(
            engine,
            root,
            port,
            [lambda conn, r=root: _phase_sparse_options(conn, r)],
        )

    def load(root):
        with open(os.path.join(root, "sparse_options.json")) as handle:
            return json.load(handle)

    cpp = load(roots["cpp"])
    rust = load(roots["rust"])
    assert cpp, "the reference answered nothing"
    for name, _, _ in SPARSE_OPT_QUERIES:
        assert cpp[name] == rust[name], (
            f"`{name}` answered differently:\n  cpp:  {cpp[name]}\n  rust: {rust[name]}"
        )
    assert isinstance(cpp["use_tail"], list), (
        f"the reference accepted `use_tail`: {cpp['use_tail']}"
    )
    assert isinstance(rust["use_tail"], list), (
        f"the port accepted `use_tail`: {rust['use_tail']}"
    )
    assert cpp["use_tail"][0] == rust["use_tail"][0] == 3013, (
        f"cpp: {cpp['use_tail']}, rust: {rust['use_tail']}"
    )
    for side in (cpp["use_tail"], rust["use_tail"]):
        assert "Unsupported optional parameter: use_tail" in side[1], side
    # The variants must not all collapse to the default, otherwise the gate
    # would pass on a reader that ignores the options.
    assert cpp["default"] != cpp["beta_one_term"], (
        "beta did not change the reference's answer; pick a query that shows it"
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
        }
        queries["pq"] = lambda t: t.output(["id"]).match_dense(
            "pq", probe, "float", "l2", 10, {"nprobe": "16"}
        ).to_result()
        answers = _answers(root, port, QUANT_TABLE, queries)
        # 200 flushed − 10 deleted + 30 appended.
        assert answers["count"] == [220], (
            f"{reader} sees {answers['count']} rows after replaying {writer}'s crash"
        )
        for name in [n for n in ("sq", "pq") if n in answers]:
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


CRASH_TABLE = "test_cross_engine_crash"
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
