//! Ported from `src/unit_test/storage/invertedindex/posting_writer_ut.cpp`.
//!
//! The reference writes a term's posting list with `PostingWriter::Dump`,
//! reads it back with `PostingWriter::Load` and drives a `PostingIterator` over
//! one or two segments. The port uses the product segment writer/reader for the
//! dump/load pair and the `PostingWriter`/`PostingIterator` types for the
//! in-memory halves.

use std::collections::BTreeMap;

use infinity_storage::fulltext_segment::{
    write_segment_index, PostingIterator, PostingWriter, SegmentIndexReader, INVALID_ROW_ID,
};
use infinity_storage::inverted_format::TermMetaFlags;
use tempfile::tempdir;

/// `PostingWriterTest::test1`.
#[test]
fn test1() {
    let expected = [1u64, 3, 5, 7, 9];
    let dir = tempdir().unwrap();
    let dir_str = dir.path().to_str().unwrap();
    let docs: Vec<(u64, u32)> = expected.iter().map(|doc| (*doc, 10)).collect();
    let postings = BTreeMap::from([(
        "term".to_string(),
        expected.iter().map(|doc| (*doc, 3)).collect::<Vec<_>>(),
    )]);
    let flags = TermMetaFlags::default();
    write_segment_index(dir_str, "posting_writer", 0, &docs, &postings, flags).unwrap();

    let reader = SegmentIndexReader::open(dir_str, "posting_writer", 0, flags).unwrap();
    let mut posting = PostingWriter::with_postings(reader.postings("term").unwrap());
    assert_eq!(posting.doc_freq(), 5);
    assert_eq!(posting.total_tf(), 15);

    // `PostingWriter::Load` then append the tenth document.
    for _ in 0..3 {
        posting.add_position();
    }
    posting.end_document(10);

    let mut iter = PostingIterator::new(vec![posting.postings().to_vec()]);
    let mut doc_id = INVALID_ROW_ID;
    for expected_doc in expected {
        doc_id = iter.seek_doc(expected_doc);
        assert_eq!(doc_id, expected_doc);
        assert_eq!(iter.current_tf(), 3);
    }
    if doc_id != INVALID_ROW_ID {
        doc_id = iter.seek_doc(doc_id + 1);
        assert_eq!(doc_id, 10);
    }
}

/// `PostingWriterTest::tf_after_skip_to_across_segments`.
#[test]
fn tf_after_skip_to_across_segments() {
    let flags = TermMetaFlags::default();
    let dir = tempdir().unwrap();
    let dir_str = dir.path().to_str().unwrap();
    let base = 1u64 << 32;
    let mut segments = Vec::new();
    for segment in 0..2u64 {
        // `RowID(segment, 0)` is the segment's base; documents are 1..=300.
        let first_row = if segment == 0 { 1 } else { base + 1 };
        let docs: Vec<(u64, u32)> = (0..300).map(|offset| (first_row + offset, 10)).collect();
        let postings = BTreeMap::from([(
            "term".to_string(),
            docs.iter().map(|(row, _)| (*row, 1)).collect::<Vec<_>>(),
        )]);
        let base_name = format!("segment_{segment}");
        write_segment_index(dir_str, &base_name, first_row - 1, &docs, &postings, flags).unwrap();
        let reader = SegmentIndexReader::open(dir_str, &base_name, first_row - 1, flags).unwrap();
        segments.push(reader.postings("term").unwrap());
    }

    let mut iter = PostingIterator::new(segments);
    assert_eq!(iter.seek_doc(0), 1);
    assert_eq!(iter.current_tf(), 1);
    // Crossing into the next segment repositions without decoding a tf.
    assert!(iter.skip_to(base + 10));
    assert_eq!(iter.current_tf(), 0);
    assert_eq!(iter.seek_doc(base + 10), base + 10);
    assert_eq!(iter.current_tf(), 1);
}
