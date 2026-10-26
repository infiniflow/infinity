"""EMVB 索引 chunk：参考的训练是随机的，只能按内容比。

EMVB 的残差 PQ 与 OPQ 用 `std::random_device` 采样（`product_quantizer_impl.cpp`
的 k-means 初始化），同一份输入在参考里两次跑出的 centroids/codes 都不同，所以
chunk 不能逐字节比。这里比的是**输入决定的结构**：头部（dims / subspace 数 /
bits / centroid 数）、文档数、embedding 数与每个文档的长度、段内偏移、文档偏移
——也就是 chunk 里描述"这段索引覆盖了哪些行、每行几个 token"的那部分。

chunk 本身只在内存里超过 `build_index_threshold_`（8192 个 embedding）之后才建，
所以用例写 8300 行 `TENSOR(FLOAT,2)`（每行一个 embedding）。

参考侧另外有两个门槛，写在这里以免下次踩：`USING EMVB` 必须带
`pq_subspace_num` / `pq_subspace_bits`（bits 只接受 8/16），且 `dims` 必须是
`pq_subspace_num` 的整数倍，否则 `GetEMVBOPQ` 直接 `UnrecoverableError` 让进程
abort（`product_quantizer_impl.cpp:447`）；EMVB 只接受 `TENSOR(FLOAT,n)` 列，
`MULTIVECTOR` 列会被 `IndexEMVB::ValidateColumnDataType` 拒绝
（`index_emvb_impl.cpp:46`）。
"""

import os
import struct
import time

import pytest
from bytes_common import (
    CPP_BIN,
    ROOT,
    RUST_BIN,
    _dump_indexes,
    _files,
    _own_table_dir,
    _run_engine,
    _under,
)
from infinity import index
from infinity.common import ConflictType

EMVB_TABLE = "test_cross_engine_emvb"
PLAID_TABLE = "test_cross_engine_plaid"
COLUMNS = {"id": {"type": "int"}, "v": {"type": "tensor,2,float"}}
# `build_index_threshold_` is 8192 embeddings; one token per row, so the tail
# has to cross it.
ROWS = 8300


def _phase_emvb(conn):
    db = conn.get_database("default_db")
    db.drop_table(EMVB_TABLE, ConflictType.Ignore)
    t = db.create_table(EMVB_TABLE, COLUMNS, ConflictType.Error)
    t.create_index(
        "emvb_i",
        index.IndexInfo(
            "v",
            index.IndexType.EMVB,
            {"pq_subspace_num": "2", "pq_subspace_bits": "8"},
        ),
    )
    rows = [{"id": i, "v": [float(i % 7), 1.0]} for i in range(ROWS)]
    for start in range(0, len(rows), 8192):
        t.insert(rows[start : start + 8192])
    conn.flush_data()
    _dump_indexes(t, ["emvb_i"])
    time.sleep(2)


def _emvb_structure(chunk):
    """The part of an EMVB chunk the input decides.

    Layout (`EmvbData::encode`, the reference's `EMVBIndex::SaveIndexInner` +
    `PQ::Save`): seven `u32` header words, the centroids (`n_centroids × dims`
    f32) and their norms, the document counters, then the per-document arrays
    (`doc_lens`, `doc_segment_offsets`, `doc_offsets`), the assignments, the
    residual-PQ codebook/codes and the OPQ rotation.

    The centroids, assignments, codebook, codes and rotation are the random
    k-means result, so they are not compared. What must match is the header and
    the three document arrays - they say which rows the chunk covers and how
    many tokens each row holds, which is exactly what a reader uses to decide
    the chunk is the same segment of the same table.
    """
    magic, version, start_offset, dims, subspaces, bits, n_centroids = struct.unpack_from(
        "<7I", chunk, 0
    )
    # One token per row in this fixture, so the token counts are all 1 and the
    # two offset arrays are `0..ROWS`.
    doc_lens = struct.pack(f"<{ROWS}I", *([1] * ROWS))
    offsets = struct.pack(f"<{ROWS}I", *range(ROWS))
    return {
        "magic": magic,
        "version": version,
        "start_segment_offset": start_offset,
        "dims": dims,
        "subspaces": subspaces,
        "bits": bits,
        "n_centroids": n_centroids,
        "size": len(chunk),
        "has_doc_lens": doc_lens in chunk,
        "has_segment_offsets": offsets in chunk,
    }


def _emvb_chunk(root):
    files = _files(root)
    # `_own_table_dir` keys off the last column's `.col`, so it wants the count.
    table_dir = _own_table_dir(files, len(COLUMNS))
    assert table_dir, "the EMVB table was not written"
    chunks = {
        name: blob
        for name, blob in _under(files, table_dir).items()
        if name.endswith(".idx")
    }
    assert len(chunks) == 1, f"expected one EMVB chunk, got {sorted(chunks)}"
    return next(iter(chunks.values()))


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_emvb_chunk_matches_by_content():
    cpp_root = os.path.join(ROOT, "emvb", "cpp")
    rust_root = os.path.join(ROOT, "emvb", "rust")
    cpp_pg = int(os.environ.get("INFINITY_BYTES_CPP_PG_PORT", "5432"))
    rust_pg = int(os.environ.get("INFINITY_BYTES_RUST_PG_PORT", "5433"))

    _run_engine("cpp", cpp_root, cpp_pg, row_count=ROWS, seed=_phase_emvb)
    _run_engine("rust", rust_root, rust_pg, row_count=ROWS, seed=_phase_emvb)

    cpp = _emvb_structure(_emvb_chunk(cpp_root))
    rust = _emvb_structure(_emvb_chunk(rust_root))

    for field in (
        "magic",
        "version",
        "start_segment_offset",
        "dims",
        "subspaces",
        "bits",
        "n_centroids",
        "size",
        "has_doc_lens",
        "has_segment_offsets",
    ):
        assert cpp[field] == rust[field], f"{field}: {cpp[field]} != {rust[field]}"
    # The fixture's structure really is in the chunk (a reader needs it).
    assert cpp["has_doc_lens"] and cpp["has_segment_offsets"]


PLAID_COLUMNS = {"id": {"type": "int"}, "v": {"type": "tensor,4,float"}}
PLAID_ROWS = 2000


def _phase_plaid(conn):
    db = conn.get_database("default_db")
    db.drop_table(PLAID_TABLE, ConflictType.Ignore)
    t = db.create_table(PLAID_TABLE, PLAID_COLUMNS, ConflictType.Error)
    t.create_index(
        "plaid_i",
        index.IndexInfo(
            "v",
            index.IndexType.PLAID,
            {"nbits": "4", "n_centroids": "16"},
        ),
    )
    rows = [
        {"id": i, "v": [float(i % 5), 1.0, float(i % 3), 0.5]}
        for i in range(PLAID_ROWS)
    ]
    for start in range(0, len(rows), 8192):
        t.insert(rows[start : start + 8192])
    conn.flush_data()
    _dump_indexes(t, ["plaid_i"])
    time.sleep(2)


def _plaid_files(root):
    files = _files(root)
    table_dir = _own_table_dir(files, len(PLAID_COLUMNS))
    assert table_dir, "the PLAID table was not written"
    return _under(files, table_dir)


@pytest.mark.xfail(
    strict=True,
    reason=(
        "the port does not write the reference's plaid_centroids.bin sidecar "
        "next to the chunk; see GAPS.md §4"
    ),
)
@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_plaid_chunk_matches_by_content():
    """PLAID's k-means initialisation is random too, so the chunk is compared
    by what the input decides: the file set, the document count in the chunk
    header and the size of the centroid sidecar (one `dims`-wide vector per
    centroid, so its length is `n_centroids` whatever the values are)."""
    cpp_root = os.path.join(ROOT, "plaid", "cpp")
    rust_root = os.path.join(ROOT, "plaid", "rust")
    cpp_pg = int(os.environ.get("INFINITY_BYTES_CPP_PG_PORT", "5432"))
    rust_pg = int(os.environ.get("INFINITY_BYTES_RUST_PG_PORT", "5433"))

    _run_engine("cpp", cpp_root, cpp_pg, row_count=PLAID_ROWS, seed=_phase_plaid)
    _run_engine("rust", rust_root, rust_pg, row_count=PLAID_ROWS, seed=_phase_plaid)

    cpp = _plaid_files(cpp_root)
    rust = _plaid_files(rust_root)
    # A PLAID index publishes its chunk plus the trained centroids.
    assert set(cpp) == set(rust), f"cpp {sorted(cpp)} != rust {sorted(rust)}"
    assert any(name.endswith(".idx") for name in cpp)
    assert any(name.endswith("centroids.bin") for name in cpp)

    for name in sorted(cpp):
        if name.endswith(".idx"):
            # Bytes 20..24 of the chunk header are the document count.
            a = struct.unpack_from("<I", cpp[name], 20)[0]
            b = struct.unpack_from("<I", rust[name], 20)[0]
            assert a == b == PLAID_ROWS, f"{name}: {a} vs {b} documents"
        elif name.endswith("centroids.bin"):
            assert len(cpp[name]) == len(rust[name]), (
                f"{name}: {len(cpp[name])} vs {len(rust[name])} bytes"
            )
