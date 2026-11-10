//! Port of `posting_merger_ut.cpp` plus the layout checks the reference's
//! `ColumnIndexMerger` guarantees.

use std::collections::BTreeMap;

use super::*;
use crate::invertedindex::disk_segment_reader::{
    write_segment_index, PostingEntry, SegmentIndexReader,
};
use crate::invertedindex::format::OPTION_FLAG_ALL;

/// The reference's test flag: `optionflag_t flag_{OPTION_FLAG_ALL}`
/// (`posting_merger_ut.cpp:49`).
fn all_flags() -> TermMetaFlags {
    OPTION_FLAG_ALL
}

fn segment<S: AsRef<str>>(
    dir: &str,
    base_name: &str,
    base_row_id: u64,
    docs: &[(u64, u32)],
    postings: &[(S, Vec<PostingEntry>)],
    flags: TermMetaFlags,
) {
    let map: BTreeMap<String, Vec<PostingEntry>> = postings
        .iter()
        .map(|(term, list)| (term.as_ref().to_string(), list.clone()))
        .collect();
    write_segment_index(dir, base_name, base_row_id, docs, &map, flags).unwrap();
}

fn source(base_name: &str, base_row_id: u64) -> ColumnSource {
    ColumnSource {
        base_name: base_name.to_string(),
        base_row_id,
    }
}

/// `PostingMergerTest::Basic` (`posting_merger_ut.cpp:79-215`): two chunks,
/// `"A"` at row 0 and `"A A"` at row 1, merged into `merged_index`.
///
/// The expectations are the reference's: `GetDF() == 2`, `GetTotalTF() == 3`,
/// positions `{0}` and `{0, 1}`, and the merged column lengths `[1, 2]`.
#[test]
fn merges_two_single_document_chunks_like_the_reference() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    segment(
        d,
        "chunk1",
        0,
        &[(0, 1)],
        &[("a", vec![(0, 1, vec![0])])],
        flags,
    );
    segment(
        d,
        "chunk2",
        1,
        &[(1, 2)],
        &[("a", vec![(1, 2, vec![0, 1])])],
        flags,
    );

    let merger = ColumnIndexMerger::new(d, flags);
    merger
        .merge(&[source("chunk1", 0), source("chunk2", 1)], "merged_index")
        .unwrap();

    let reader = SegmentIndexReader::open(d, "merged_index", 0, flags).unwrap();
    assert_eq!(reader.rows(), &[0, 1]);
    assert_eq!(reader.doc_len(0), Some(1));
    assert_eq!(reader.doc_len(1), Some(2));

    let meta = reader.lookup("a").unwrap().unwrap();
    assert_eq!(meta.doc_freq, 2, "one document per chunk");
    assert_eq!(meta.total_tf, 3, "every occurrence counts once");
    assert_eq!(reader.postings("a").unwrap(), vec![(0, 1), (1, 2)]);
    assert_eq!(
        reader.positions("a").unwrap(),
        Some(vec![(0, vec![0]), (1, vec![0, 1])])
    );
}

/// A merge must produce the bytes a single write of the same rows would:
/// rebasing by `base_row_id - merge_base_rowid` is the identity when the
/// merged base is the first chunk's base, so the two paths coincide.
#[test]
fn merge_matches_a_single_write() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    let (docs, postings) = corpus(300);
    let split = 137;

    let whole: BTreeMap<String, Vec<PostingEntry>> = postings.iter().cloned().collect();
    write_segment_index(d, "whole", 0, &docs, &whole, flags).unwrap();

    let head = &docs[..split];
    let tail = &docs[split..];
    let head_postings = slice_postings(&postings, 0, split);
    let tail_postings = slice_postings(&postings, split, docs.len());
    segment(d, "chunk1", 0, head, &head_postings, flags);
    segment(d, "chunk2", split as u64, tail, &tail_postings, flags);

    ColumnIndexMerger::new(d, flags)
        .merge(
            &[source("chunk1", 0), source("chunk2", split as u64)],
            "merged",
        )
        .unwrap();

    for suffix in [
        FULLTEXT_DICT_SUFFIX,
        FULLTEXT_POSTING_SUFFIX,
        FULLTEXT_LENGTH_SUFFIX,
    ] {
        let whole_bytes = std::fs::read(fulltext_chunk_file(d, "whole", suffix)).unwrap();
        let merged_bytes = std::fs::read(fulltext_chunk_file(d, "merged", suffix)).unwrap();
        assert_eq!(whole_bytes, merged_bytes, "{suffix} differs");
    }
}

/// The merged segment's base is the **smallest** source base, so doc ids are
/// rebased by `base_row_id - merge_base_rowid` and stay `u32`
/// (`posting_merger_impl.cpp:178-192`).
#[test]
fn rebases_doc_ids_by_the_smallest_base_row_id() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    let base = 1u64 << 32;
    segment(
        d,
        "later",
        base + 10,
        &[(base + 10, 1), (base + 11, 1)],
        &[("t", vec![(base + 11, 1, vec![3])])],
        flags,
    );
    segment(
        d,
        "first",
        base,
        &[(base, 1)],
        &[("t", vec![(base, 1, vec![7])])],
        flags,
    );

    ColumnIndexMerger::new(d, flags)
        .merge(
            &[source("later", base + 10), source("first", base)],
            "merged",
        )
        .unwrap();

    let reader = SegmentIndexReader::open(d, "merged", base, flags).unwrap();
    assert_eq!(reader.row_range(), Some((base, base + 11)));
    assert_eq!(reader.doc_count(), 12, "the shorter sources pad with zeros");
    // Row `base + 11` is the merged doc 11, i.e. 10 (the second source's base
    // offset) + 1 (the doc id inside it).
    assert_eq!(
        reader.postings("t").unwrap(),
        vec![(base, 1), (base + 11, 1)]
    );
    assert_eq!(
        reader.positions("t").unwrap(),
        Some(vec![(base, vec![7]), (base + 11, vec![3])])
    );
}

/// A merged list longer than one mini-block is re-blocked from scratch: the
/// 128-document boundaries of the merged list are not the sources'.
#[test]
fn merges_across_document_blocks() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    let docs: Vec<(u64, u32)> = (0..400).map(|row| (row, 1)).collect();
    let list: Vec<PostingEntry> = (0..400).map(|row| (row, 1, vec![0])).collect();
    let head: Vec<PostingEntry> = list[..200].to_vec();
    let tail: Vec<PostingEntry> = list[200..].to_vec();
    segment(d, "chunk1", 0, &docs[..200], &[("t", head)], flags);
    segment(d, "chunk2", 200, &docs[200..], &[("t", tail)], flags);

    ColumnIndexMerger::new(d, flags)
        .merge(&[source("chunk1", 0), source("chunk2", 200)], "merged")
        .unwrap();

    let reader = SegmentIndexReader::open(d, "merged", 0, flags).unwrap();
    let expected: Vec<(u64, u32)> = (0..400).map(|row| (row, 1)).collect();
    assert_eq!(reader.postings("t").unwrap(), expected);
    // Four 128-document blocks (400 docs), decoded by the lazy cursor.
    let mut cursor = reader.term_doc_cursor("t").unwrap().unwrap();
    assert_eq!(cursor.df(), 400);
    let mut seen = 0usize;
    while cursor.doc().is_some() {
        assert_eq!(cursor.doc(), Some(seen as u64));
        assert_eq!(cursor.tf(), 1);
        cursor.skip(1);
        seen += 1;
    }
    assert_eq!(seen, 400, "every merged document is reachable");
}

/// `SegmentTermPostingQueue` groups the equal terms of every source, ordered by
/// `(term, base_row_id)` (`segment_term_posting.cppm:23-31`).
#[test]
fn queue_groups_equal_terms_in_base_order() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    segment(
        d,
        "chunk1",
        0,
        &[(0, 1)],
        &[("a", vec![(0, 1, vec![0])]), ("c", vec![(0, 1, vec![1])])],
        flags,
    );
    segment(
        d,
        "chunk2",
        1,
        &[(1, 1)],
        &[("b", vec![(1, 1, vec![0])]), ("c", vec![(1, 1, vec![1])])],
        flags,
    );

    let mut queue = SegmentTermPostingQueue::new(
        d,
        &[("chunk1".to_string(), 0), ("chunk2".to_string(), 1)],
        flags,
    )
    .unwrap();
    let mut groups = Vec::new();
    while let Some((term, group)) = queue.next_group().unwrap() {
        groups.push((
            term,
            group
                .iter()
                .map(|posting| posting.base_row_id)
                .collect::<Vec<_>>(),
        ));
    }
    assert_eq!(
        groups,
        vec![
            ("a".to_string(), vec![0]),
            ("b".to_string(), vec![1]),
            ("c".to_string(), vec![0, 1]),
        ]
    );
    assert!(queue.is_empty());
}

/// A source without any term contributes nothing, like the reference's
/// constructor, which drops it (`segment_term_posting_impl.cpp:31-37`).
#[test]
fn source_without_terms_is_skipped() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    segment::<&str>(d, "empty", 0, &[(0, 1)], &[], flags);
    segment(
        d,
        "chunk2",
        1,
        &[(1, 1)],
        &[("t", vec![(1, 1, vec![0])])],
        flags,
    );

    ColumnIndexMerger::new(d, flags)
        .merge(&[source("empty", 0), source("chunk2", 1)], "merged")
        .unwrap();

    let reader = SegmentIndexReader::open(d, "merged", 0, flags).unwrap();
    assert_eq!(reader.term_entries().unwrap().len(), 1);
    assert_eq!(reader.postings("t").unwrap(), vec![(1, 1)]);
}

/// Without a position list the frequencies cannot be recounted, so they come
/// from the source's doc list (`SetCurrentTF`,
/// `posting_merger_impl.cpp:60-62`).
#[test]
fn frequencies_survive_a_merge_without_positions() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = all_flags();
    segment(
        d,
        "chunk1",
        0,
        &[(0, 3)],
        &[("t", vec![(0, 3, Vec::new())])],
        flags,
    );
    segment(
        d,
        "chunk2",
        1,
        &[(1, 5)],
        &[("t", vec![(1, 2, Vec::new())])],
        flags,
    );

    ColumnIndexMerger::new(d, flags)
        .merge(&[source("chunk1", 0), source("chunk2", 1)], "merged")
        .unwrap();

    let reader = SegmentIndexReader::open(d, "merged", 0, flags).unwrap();
    assert_eq!(reader.postings("t").unwrap(), vec![(0, 3), (1, 2)]);
    let meta = reader.lookup("t").unwrap().unwrap();
    assert_eq!(meta.doc_freq, 2);
    assert_eq!(meta.total_tf, 5);
    assert_eq!(reader.positions("t").unwrap(), None);
}

/// The generated corpus: the documents and one posting list per term.
type Corpus = (Vec<(u64, u32)>, Vec<(String, Vec<PostingEntry>)>);

/// A position list without a frequency column has no per-document boundary
/// (`DocMerger::MergePosition` has no `HasTfList()` stop then), so such an index
/// is refused rather than silently collapsed to one occurrence per document.
#[test]
fn a_position_list_without_frequencies_is_refused() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let flags = TermMetaFlags {
        has_term_frequency: false,
        has_term_payload: false,
        has_doc_payload: false,
        has_block_max: false,
    };
    assert!(
        flags.has_position_list(),
        "this store always sets of_position_list"
    );
    let result = ColumnIndexMerger::new(d, flags).merge(&[source("chunk1", 0)], "merged");
    assert!(
        matches!(result, Err(FormatError::Invalid(_))),
        "expected a refused merge, got {result:?}"
    );
}

fn corpus(n: usize) -> Corpus {
    let mut docs = Vec::with_capacity(n);
    let mut alpha: Vec<PostingEntry> = Vec::new();
    let mut beta: Vec<PostingEntry> = Vec::new();
    let mut gamma: Vec<PostingEntry> = Vec::new();
    for row in 0..n as u64 {
        let mut positions = Vec::new();
        positions.push(0);
        if row % 3 == 0 {
            positions.push(1);
        }
        if row % 5 == 0 {
            positions.push(2);
        }
        let len = positions.len() as u32;
        docs.push((row, len));
        alpha.push((row, len, positions.clone()));
        if row % 3 == 0 {
            beta.push((row, 1, vec![1]));
        }
        if row % 5 == 0 {
            gamma.push((row, 1, vec![2]));
        }
    }
    (
        docs,
        vec![
            ("alpha".to_string(), alpha),
            ("beta".to_string(), beta),
            ("gamma".to_string(), gamma),
        ],
    )
}

/// The postings of `postings` restricted to the documents `docs[start..end]`.
fn slice_postings(
    postings: &[(String, Vec<PostingEntry>)],
    start: usize,
    end: usize,
) -> Vec<(String, Vec<PostingEntry>)> {
    postings
        .iter()
        .map(|(term, list)| {
            let sliced: Vec<PostingEntry> = list
                .iter()
                .filter(|(row, _, _)| {
                    let row = *row as usize;
                    row >= start && row < end
                })
                .cloned()
                .collect();
            (term.clone(), sliced)
        })
        .filter(|(_, list)| !list.is_empty())
        .collect()
}
