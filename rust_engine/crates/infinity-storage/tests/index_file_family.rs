//! Gate G / G2 — the index **file-family contract** (an invariant from the C++
//! engine, `RUST_REWRITE_PLAN.md` 附录 A.3).
//!
//! The C++ engine gives every index kind the same *lifecycle* skeleton
//! (build a chunk -> persist its file family -> publish `ChunkIndexMetaInfo`
//! into the catalog) but a different *file set*: vector indexes write a single
//! `chunk_{n}.idx`, while a full-text chunk writes `{base}.dic` + `.pos` +
//! `.len` (plus a transient `.spill`). This pins those shapes and the layouts
//! the files must carry, so a format change cannot silently break the
//! contract.

use infinity_storage::cpp_format as cf;
use infinity_storage::inverted_format as inv;

#[test]
fn index_paths_use_the_cpp_layout() {
    // `db_{db}/tbl_{tbl}/idx_{index}/seg_{seg}` (decimal ids, e.g.
    // `TableIndexMeta::GetTableIndexDir` = `db_{db}/tbl_{tbl}/idx_{index}`).
    let dir = cf::segment_index_dir(0, 1, 2, 3);
    assert_eq!(dir, "db_0/tbl_1/idx_2/seg_3");
    // The vector family's per-chunk file.
    assert_eq!(cf::chunk_index_file(4), "chunk_4.idx");
    assert_eq!(
        format!("{dir}/{}", cf::chunk_index_file(4)),
        "db_0/tbl_1/idx_2/seg_3/chunk_4.idx"
    );
    // Table data lives under the block dir.
    assert_eq!(cf::block_dir(0, 1, 2, 5), "db_0/tbl_1/seg_2/blk_5");
    assert_eq!(cf::column_file(7), "7.col");
    assert_eq!(cf::column_outline_file(7), "col_7_out");
    assert_eq!(cf::VERSION_FILE, "version");
}

#[test]
fn fulltext_file_family_is_dic_pos_len() {
    // The suffixes the C++ reader opens (`index_defines.cppm:55-58`; the
    // transient `.spill` is removed once the chunk is built).
    assert_eq!(inv::FULLTEXT_DICT_SUFFIX, ".dic");
    assert_eq!(inv::FULLTEXT_POSTING_SUFFIX, ".pos");
    assert_eq!(inv::FULLTEXT_SPILL_SUFFIX, ".spill");
    assert_eq!(inv::FULLTEXT_LENGTH_SUFFIX, ".len");
    assert_eq!(
        inv::FULLTEXT_CHUNK_FILES,
        [".dic", ".pos", ".len"],
        "a built chunk keeps dict/posting/length"
    );
    // `{index_dir}/{base_name}{suffix}` (`disk_segment_reader_impl.cpp:51-57`).
    let dir = cf::segment_index_dir(0, 1, 2, 3);
    assert_eq!(
        inv::fulltext_chunk_file(&dir, "ft_0", inv::FULLTEXT_DICT_SUFFIX),
        "db_0/tbl_1/idx_2/seg_3/ft_0.dic"
    );
}

#[test]
fn vector_chunk_is_one_idx_file_per_chunk() {
    // Unlike full text, the vector family (HNSW and IVF alike) persists one
    // `chunk_{n}.idx` per chunk under the segment-index directory; the kind is
    // told apart by the header inside the file and the index type in the
    // catalog's `IndexBase`. The header/region layouts themselves are covered
    // by `knn_format`'s own tests.
    let dir = cf::segment_index_dir(0, 1, 2, 3);
    let path = format!("{dir}/{}", cf::chunk_index_file(0));
    assert_eq!(path, "db_0/tbl_1/idx_2/seg_3/chunk_0.idx");
    // Successive chunks are siblings, not nested.
    assert_eq!(
        format!("{dir}/{}", cf::chunk_index_file(1)),
        "db_0/tbl_1/idx_2/seg_3/chunk_1.idx"
    );
}

#[test]
fn fulltext_chunk_files_compose_into_a_readable_index() {
    // The two persistent text files, built with the real codecs, form a
    // term -> postings index that reads back (the `.dic` half is exercised in
    // `inverted_format`'s unit tests; this pins the *file family* end to end).
    let mut dict = Vec::new();
    let mut posting = Vec::new();

    let terms = ["alpha", "beta"];
    let docs: [Vec<u32>; 2] = [vec![0, 3, 7], vec![1, 2]];
    let flags = inv::TermMetaFlags {
        has_term_frequency: false,
        has_term_payload: false,
    };
    let mut metas = Vec::new();
    for d in docs.iter() {
        let start = posting.len() as u64;
        let tp = inv::TermPostings {
            doc_skiplist: Vec::new(),
            doc_list: inv::encode_u32_block(d),
            pos_skiplist: Vec::new(),
            pos_list: Vec::new(),
        };
        posting.extend_from_slice(&tp.encode());
        metas.push(inv::TermMeta {
            doc_freq: d.len() as u32,
            total_tf: d.len() as u32,
            payload: 0,
            doc_start: start,
            pos_start: start,
            pos_end: posting.len() as u64,
        });
    }
    let (region, offsets) = inv::encode_term_meta_region_with_offsets(&metas, flags);
    let mut fst = inv::FstBuilder::new();
    for (i, t) in terms.iter().enumerate() {
        fst.insert(t.as_bytes(), offsets[i]);
    }
    dict.extend_from_slice(&inv::encode_dic(&region, &fst.finish()));

    // `.len` is a bare `u32[row_cnt]`.
    let len_file = inv::encode_column_lengths(&[2, 1, 3, 5, 1, 1, 2, 4]);
    assert_eq!(
        inv::decode_column_lengths(&len_file).unwrap(),
        vec![2, 1, 3, 5, 1, 1, 2, 4]
    );

    // Read the family back: term -> offset -> meta -> postings slice.
    let fst_bytes = inv::dic_fst(&dict).unwrap();
    for (i, t) in terms.iter().enumerate() {
        let off = inv::fst_get(fst_bytes, t.as_bytes()).unwrap().unwrap();
        let meta = inv::decode_dic_meta_at(&dict, off, flags).unwrap();
        assert_eq!(meta, metas[i]);
        let slice = &posting[meta.doc_start as usize..meta.pos_end as usize];
        let (tp, _) = inv::TermPostings::decode(slice).unwrap();
        let (got, _) = inv::decode_u32_block(&tp.doc_list, docs[i].len()).unwrap();
        assert_eq!(got, docs[i], "{t}");
    }
}
