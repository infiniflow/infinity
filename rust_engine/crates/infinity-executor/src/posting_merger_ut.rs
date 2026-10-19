//! Ported from `src/unit_test/storage/invertedindex/posting_merger_ut.cpp`.
//!
//! Two memory indexers (one document each) are dumped, their `a` postings are
//! decoded and their positions/tf are checked, then a `PostingMerger` combines
//! them. The on-disk format stores frequencies but no positions, so the
//! position assertion reads the memory index that produced the segment (the
//! same data the reference decodes back from its position list).

use crate::MemoryIndexer;
use infinity_storage::fulltext_segment::{PostingMerger, SegmentIndexReader};
use infinity_storage::inverted_format::TermMetaFlags;
use tempfile::tempdir;

/// `PostingMergerTest::Basic`.
#[test]
fn basic() {
    let dir = tempdir().unwrap();
    let dir_str = dir.path().to_str().unwrap();
    let paragraphs = ["A", "A A"];
    let expected_positions: [&[u32]; 2] = [&[0], &[0, 1]];
    let expected_tf = [1u32, 2];
    let flags = TermMetaFlags::default();

    let mut segment_postings: Vec<Vec<(u64, u32)>> = Vec::new();
    for (index, paragraph) in paragraphs.iter().enumerate() {
        let mut indexer = MemoryIndexer::new();
        assert!(indexer.insert(index as u64, paragraph, &None, usize::MAX));
        assert!(indexer.dump(dir_str, &format!("chunk{}", index + 1)));

        // The segment's only term is `a`; its positions are the reference's
        // decoded deltas.
        let hits = &indexer.postings["a"];
        assert_eq!(hits.len(), 1);
        assert_eq!(hits[0].0, index as u64);
        assert_eq!(hits[0].1, expected_tf[index]);
        assert_eq!(hits[0].2, expected_positions[index]);

        let reader =
            SegmentIndexReader::open(dir_str, &format!("chunk{}", index + 1), index as u64, flags)
                .unwrap();
        assert_eq!(reader.terms().unwrap(), vec!["a".to_string()]);
        let postings = reader.postings("a").unwrap();
        assert_eq!(postings, vec![(index as u64, expected_tf[index])]);
        segment_postings.push(postings);
    }

    let mut merger = PostingMerger::new();
    let lists: Vec<&[(u64, u32)]> = segment_postings.iter().map(Vec::as_slice).collect();
    merger.merge(&lists);
    assert_eq!(merger.doc_freq(), 2);
    assert_eq!(merger.total_tf(), 3);
    assert_eq!(merger.postings(), &[(0, 1), (1, 2)]);
}
