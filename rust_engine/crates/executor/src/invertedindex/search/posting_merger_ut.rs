//! Ported from `src/unit_test/storage/invertedindex/posting_merger_ut.cpp`.
//!
//! Two memory indexers (one document each) are dumped, each chunk's posting is
//! decoded through `SegmentTermPosting`/`PostingDecoder`, then a
//! `PostingMerger` combines them the way the reference does: doc ids rebased by
//! `base_row_id - merge_base_rowid`, positions replayed through a fresh
//! `PostingWriter`, and the frequencies recounted from those positions.

use crate::{MemoryIndexer, TextTokenizer};
use storage::invertedindex::disk_segment_reader::SegmentIndexReader;
use storage::invertedindex::format::OPTION_FLAG_ALL;
use storage::invertedindex::index_merger::{
    ColumnIndexMerger, ColumnSource, MergingPosting, PostingMerger, SegmentTermPosting,
};
use tempfile::tempdir;

/// `PostingMergerTest::Basic`.
#[test]
fn basic() {
    let dir = tempdir().unwrap();
    let dir_str = dir.path().to_str().unwrap();
    let paragraphs = ["A", "A A"];
    let expected_positions: [&[u32]; 2] = [&[0], &[0, 1]];
    let expected_tf = [1u32, 2];

    for (index, paragraph) in paragraphs.iter().enumerate() {
        let mut indexer = MemoryIndexer::new();
        assert!(indexer.insert(
            index as u64,
            paragraph,
            &TextTokenizer::default(),
            usize::MAX
        ));
        assert!(indexer.dump(dir_str, &format!("chunk{}", index + 1)));
    }

    // `ColumnIndexIterator::Next` + `PostingDecoder::DecodePosList`: each
    // chunk's only term is `a`, at the offsets the encoder wrote.
    let mut sources = Vec::new();
    for index in 0..paragraphs.len() {
        let mut posting = SegmentTermPosting::open(
            dir_str,
            &format!("chunk{}", index + 1),
            index as u64,
            OPTION_FLAG_ALL,
        )
        .unwrap();
        assert!(posting.has_next());
        assert_eq!(posting.term(), Some("a"));

        let mut decoder = posting.decoder().unwrap();
        let block = decoder.decode_doc_list().unwrap().expect("one document");
        assert_eq!(block.tfs, vec![expected_tf[index]]);
        let positions = decoder.decode_doc_positions(expected_tf[index]).unwrap();
        assert_eq!(positions, expected_positions[index]);
        assert!(decoder.decode_doc_list().unwrap().is_none());

        sources.push((posting, index as u64));
    }

    // The merged column-length array of the two chunks: `[1, 2]`.
    let column_lengths = [1u32, 2];
    let mut merger = PostingMerger::new(OPTION_FLAG_ALL, &column_lengths);
    let mut postings: Vec<MergingPosting> = sources
        .iter()
        .map(|(posting, base_row_id)| MergingPosting {
            base_row_id: *base_row_id,
            decoder: posting.decoder().unwrap(),
        })
        .collect();
    let merge_base_rowid = 0;
    merger.merge(&mut postings, merge_base_rowid).unwrap();

    assert_eq!(merger.df(), 2);
    assert_eq!(merger.total_tf(), 3);

    // `Dump` fills the offsets of one shared `.pos` image.
    let mut image = Vec::new();
    let meta = merger.dump(&mut image);
    assert_eq!(meta.doc_freq, 2);
    assert_eq!(meta.total_tf, 3);
    assert_eq!(meta.doc_start, 0);
    assert!(meta.pos_start > meta.doc_start);
    assert_eq!(meta.pos_end as usize, image.len());
}

/// The whole path end to end: `ColumnIndexMerger` writes one segment the
/// product reader then serves, with the merged row range and positions.
#[test]
fn column_merger_writes_one_readable_segment() {
    let dir = tempdir().unwrap();
    let dir_str = dir.path().to_str().unwrap();
    for (index, paragraph) in ["A", "A A"].iter().enumerate() {
        let mut indexer = MemoryIndexer::new();
        assert!(indexer.insert(
            index as u64,
            paragraph,
            &TextTokenizer::default(),
            usize::MAX
        ));
        assert!(indexer.dump(dir_str, &format!("chunk{}", index + 1)));
    }

    let sources = [
        ColumnSource {
            base_name: "chunk1".to_string(),
            base_row_id: 0,
        },
        ColumnSource {
            base_name: "chunk2".to_string(),
            base_row_id: 1,
        },
    ];
    ColumnIndexMerger::new(dir_str, OPTION_FLAG_ALL)
        .merge(&sources, "merged_index")
        .unwrap();

    let reader = SegmentIndexReader::open(dir_str, "merged_index", 0, OPTION_FLAG_ALL).unwrap();
    assert_eq!(reader.rows(), &[0, 1]);
    assert_eq!(reader.doc_len(0), Some(1));
    assert_eq!(reader.doc_len(1), Some(2));
    let meta = reader.lookup("a").unwrap().unwrap();
    assert_eq!(meta.doc_freq, 2);
    assert_eq!(meta.total_tf, 3);
    assert_eq!(reader.postings("a").unwrap(), vec![(0, 1), (1, 2)]);
    assert_eq!(
        reader.positions("a").unwrap(),
        Some(vec![(0, vec![0]), (1, vec![0, 1])])
    );
}
