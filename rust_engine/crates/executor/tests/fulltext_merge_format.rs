//! The port's index merge against the reference's own bytes.
//!
//! The fixtures are produced by the reference itself
//! (`src/unit_test/storage/invertedindex/fulltext_merge_golden_ut.cpp`):
//! `MemoryIndexer` dumps two chunks through the production dump path and
//! `ColumnIndexMerger` merges them through the production merge path
//! (`NewTxn::OptimizeFtIndex`). The corpus is seven documents over three terms:
//!
//! ```text
//! chunk1 (rows 0..3)            chunk2 (rows 4..6)
//!  0 "alpha beta"                4 "alpha gamma gamma"
//!  1 "alpha"                     5 "beta"
//!  2 "beta gamma"                6 "alpha beta gamma"
//!  3 "alpha alpha beta"
//! ```
//!
//! Three independent claims are checked, which together say that a merge in
//! this port is byte-for-byte the reference's:
//!
//! 1. reading the reference's two chunks and merging them here reproduces
//!    `merged.{dic,pos,len}` exactly — the merge itself, including the doc-id
//!    rebase, the position replay and the re-blocking;
//! 2. writing the same logical rows as one segment reproduces the same three
//!    files — the writer, including the position encoding and the skip lists;
//! 3. the reference's merged chunk decodes here to the expected postings — the
//!    reader, so a passing 1 and 2 cannot be two matching mistakes.
//!
//! Claim 2 is what makes claim 1 meaningful: it pins the bytes to the *content*
//! (frequencies, positions, column lengths), not merely to the reference's
//! merge code path.

use std::collections::BTreeMap;
use std::path::{Path, PathBuf};

use storage::invertedindex::disk_segment_reader::{
    write_segment_index, PostingEntry, SegmentIndexReader,
};
use storage::invertedindex::format::OPTION_FLAG_ALL;
use storage::invertedindex::index_merger::{ColumnIndexMerger, ColumnSource};

const SUFFIXES: [&str; 3] = [".dic", ".pos", ".len"];

fn fixture(name: &str) -> PathBuf {
    Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("tests/fixtures/ft_merge")
        .join(name)
}

fn copy_fixture(dir: &Path, base_name: &str) {
    for suffix in SUFFIXES {
        let from = fixture(&format!("{base_name}{suffix}"));
        let to = dir.join(format!("{base_name}{suffix}"));
        std::fs::copy(&from, &to)
            .unwrap_or_else(|e| panic!("copy {} to {}: {e}", from.display(), to.display()));
    }
}

fn assert_matches_reference(dir: &Path, got_base: &str, want_base: &str) {
    for suffix in SUFFIXES {
        let want = std::fs::read(fixture(&format!("{want_base}{suffix}"))).unwrap();
        let got = std::fs::read(dir.join(format!("{got_base}{suffix}"))).unwrap();
        assert_eq!(
            got,
            want,
            "{got_base}{suffix} differs from the reference's {want_base}{suffix} ({} vs {} bytes)",
            got.len(),
            want.len()
        );
    }
}

/// The reference's chunks, in the layout `ColumnIndexMerger` wants: base 0 with
/// four documents, base 4 with three.
fn reference_sources() -> [ColumnSource; 2] {
    [
        ColumnSource {
            base_name: "chunk1".to_string(),
            base_row_id: 0,
        },
        ColumnSource {
            base_name: "chunk2".to_string(),
            base_row_id: 4,
        },
    ]
}

/// The same rows as one segment. Positions are `word_offset`, the analyzer's
/// token ordinal (`ColumnInverter::AddTerm`/`GeneratePosting`,
/// `column_inverter_impl.cpp:112-128,253`), which is why `"alpha alpha beta"`
/// holds `alpha` at 0 and 1 and `beta` at 2.
/// The corpus as the writer wants it: the documents and one posting list per
/// term.
type Rows = (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>);

fn reference_rows() -> Rows {
    let docs: Vec<(u64, u32)> = vec![(0, 2), (1, 1), (2, 2), (3, 3), (4, 3), (5, 1), (6, 3)];
    let postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::from([
        (
            "alpha".to_string(),
            vec![
                (0, 1, vec![0]),
                (1, 1, vec![0]),
                (3, 2, vec![0, 1]),
                (4, 1, vec![0]),
                (6, 1, vec![0]),
            ],
        ),
        (
            "beta".to_string(),
            vec![
                (0, 1, vec![1]),
                (2, 1, vec![0]),
                (3, 1, vec![2]),
                (5, 1, vec![0]),
                (6, 1, vec![1]),
            ],
        ),
        (
            "gamma".to_string(),
            vec![(2, 1, vec![1]), (4, 2, vec![1, 2]), (6, 1, vec![2])],
        ),
    ]);
    (docs, postings)
}

/// Claim 1: merge the reference's chunks here, compare with its merge output.
#[test]
fn merging_the_reference_chunks_reproduces_the_reference_merge() {
    let dir = tempfile::tempdir().unwrap();
    copy_fixture(dir.path(), "chunk1");
    copy_fixture(dir.path(), "chunk2");

    ColumnIndexMerger::new(dir.path().to_str().unwrap(), OPTION_FLAG_ALL)
        .merge(&reference_sources(), "merged")
        .unwrap();

    assert_matches_reference(dir.path(), "merged", "merged");
}

/// Claim 2: one write of the same rows is the same three files.
#[test]
fn a_single_write_matches_the_reference_merge() {
    let dir = tempfile::tempdir().unwrap();
    let (docs, postings) = reference_rows();
    write_segment_index(
        dir.path().to_str().unwrap(),
        "merged",
        0,
        &docs,
        &postings,
        OPTION_FLAG_ALL,
    )
    .unwrap();

    assert_matches_reference(dir.path(), "merged", "merged");
}

/// Claim 3: the reference's merged chunk decodes to the expected postings, so
/// claims 1 and 2 are not two matching mistakes.
#[test]
fn the_reference_merge_decodes_to_the_expected_postings() {
    let dir = tempfile::tempdir().unwrap();
    copy_fixture(dir.path(), "merged");

    let reader =
        SegmentIndexReader::open(dir.path().to_str().unwrap(), "merged", 0, OPTION_FLAG_ALL)
            .unwrap();
    assert_eq!(reader.doc_count(), 7);
    assert_eq!(reader.rows(), &[0, 1, 2, 3, 4, 5, 6]);
    for (row, len) in [(0, 2), (1, 1), (2, 2), (3, 3), (4, 3), (5, 1), (6, 3)] {
        assert_eq!(reader.doc_len(row), Some(len), "doc_len of row {row}");
    }

    let (_, postings) = reference_rows();
    for (term, entries) in &postings {
        let want: Vec<(u64, u32)> = entries.iter().map(|(row, tf, _)| (*row, *tf)).collect();
        assert_eq!(reader.postings(term).unwrap(), want, "postings of '{term}'");
        let want_positions: Vec<(u64, Vec<u32>)> = entries
            .iter()
            .map(|(row, _, positions)| (*row, positions.clone()))
            .collect();
        assert_eq!(
            reader.positions(term).unwrap(),
            Some(want_positions),
            "positions of '{term}'"
        );
        let meta = reader
            .lookup(term)
            .unwrap()
            .expect("term is in the dictionary");
        let total_tf: u32 = entries.iter().map(|(_, tf, _)| *tf).sum();
        assert_eq!(meta.doc_freq, entries.len() as u32, "df of '{term}'");
        assert_eq!(meta.total_tf, total_tf, "ttf of '{term}'");
    }
}

/// Claim 1 again, on a pair wider than one 128-document block: the merged list
/// is re-blocked into several blocks, so this is the region where the block
/// boundaries, the skip-list item columns and the block-max values have to agree.
#[test]
fn merging_a_multi_block_pair_reproduces_the_reference_merge() {
    let dir = tempfile::tempdir().unwrap();
    copy_fixture(dir.path(), "big_chunk1");
    copy_fixture(dir.path(), "big_chunk2");

    let sources = [
        ColumnSource {
            base_name: "big_chunk1".to_string(),
            base_row_id: 0,
        },
        ColumnSource {
            base_name: "big_chunk2".to_string(),
            base_row_id: 130,
        },
    ];
    ColumnIndexMerger::new(dir.path().to_str().unwrap(), OPTION_FLAG_ALL)
        .merge(&sources, "big_merged")
        .unwrap();

    assert_matches_reference(dir.path(), "big_merged", "big_merged");

    let reader = SegmentIndexReader::open(
        dir.path().to_str().unwrap(),
        "big_merged",
        0,
        OPTION_FLAG_ALL,
    )
    .unwrap();
    assert_eq!(reader.doc_count(), 300);
    // Three 128-document blocks plus a tail: the cursor walks four of them.
    let mut cursor = reader.term_doc_cursor("alpha").unwrap().unwrap();
    assert_eq!(cursor.df(), 300);
    let mut seen = 0usize;
    while cursor.doc().is_some() {
        assert_eq!(cursor.doc(), Some(seen as u64));
        cursor.skip(1);
        seen += 1;
    }
    assert_eq!(seen, 300);
}

/// Claim 1 again, on chunks whose documents carry a **doc payload** (the
/// `rankfeatures` weight): the merge has to copy it per `(term, document)` while
/// rebasing the doc ids. The fixture is written by the reference's own
/// `PostingWriter`/`TermMetaDumper`/`FstBuilder`
/// (`fulltext_merge_golden_ut.cpp::WritePayloadChunk`).
#[test]
fn merging_chunks_with_document_payloads_reproduces_the_reference_merge() {
    let dir = tempfile::tempdir().unwrap();
    copy_fixture(dir.path(), "payload_chunk1");
    copy_fixture(dir.path(), "payload_chunk2");

    let sources = [
        ColumnSource {
            base_name: "payload_chunk1".to_string(),
            base_row_id: 0,
        },
        ColumnSource {
            base_name: "payload_chunk2".to_string(),
            base_row_id: 2,
        },
    ];
    ColumnIndexMerger::new(dir.path().to_str().unwrap(), OPTION_FLAG_ALL)
        .merge(&sources, "payload_merged")
        .unwrap();

    assert_matches_reference(dir.path(), "payload_merged", "payload_merged");
}

/// A single write of the merged rows (payloads included) is the same three
/// files, which pins the payload column to the content rather than to the merge
/// code path.
#[test]
fn a_single_write_with_payloads_matches_the_reference_merge() {
    use storage::invertedindex::disk_segment_reader::write_segment_index_with_payloads;

    let docs: Vec<(u64, u32)> = vec![(0, 2), (1, 3), (2, 2), (3, 1)];
    let postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::from([
        (
            "a".to_string(),
            vec![
                (0, 1, vec![0]),
                (1, 1, vec![1]),
                (2, 1, vec![0]),
                (3, 1, vec![1]),
            ],
        ),
        ("b".to_string(), vec![(0, 1, vec![1]), (1, 2, vec![0, 2])]),
        ("c".to_string(), vec![(3, 1, vec![0])]),
    ]);
    let payloads: BTreeMap<String, BTreeMap<u64, u16>> = BTreeMap::from([
        (
            "a".to_string(),
            BTreeMap::from([(0, 7), (1, 9), (2, 65535), (3, 2)]),
        ),
        ("b".to_string(), BTreeMap::from([(0, 7), (1, 300)])),
        ("c".to_string(), BTreeMap::from([(3, 1234)])),
    ]);

    let dir = tempfile::tempdir().unwrap();
    write_segment_index_with_payloads(
        dir.path().to_str().unwrap(),
        "payload_merged",
        0,
        &docs,
        &postings,
        &payloads,
        OPTION_FLAG_ALL,
    )
    .unwrap();

    assert_matches_reference(dir.path(), "payload_merged", "payload_merged");
}

/// The reference's **offline** dump — `Insert(offline = true)` spilling sorted
/// runs and `OfflineDump` merging them with `SortMergerTermTuple`
/// (`memory_indexer_impl.cpp:916-935`) — must produce the chunk a single write
/// produces. This is the reference's own merge-before-dump path, so it pins the
/// port's spill + seal output to the same bytes.
#[test]
fn the_reference_offline_dump_is_the_same_chunk() {
    let dir = tempfile::tempdir().unwrap();
    let (docs, postings) = reference_rows();
    write_segment_index(
        dir.path().to_str().unwrap(),
        "offline_merged",
        0,
        &docs,
        &postings,
        OPTION_FLAG_ALL,
    )
    .unwrap();

    assert_matches_reference(dir.path(), "offline_merged", "offline_merged");
    // The reference agrees with itself: its offline dump, its spill round trip
    // and its two-chunk merge are the same bytes.
    for suffix in SUFFIXES {
        let merged = std::fs::read(fixture(&format!("merged{suffix}"))).unwrap();
        for other in ["offline_merged", "spilled_merged"] {
            let bytes = std::fs::read(fixture(&format!("{other}{suffix}"))).unwrap();
            assert_eq!(
                merged, bytes,
                "the reference's {other} and its merge disagree on {suffix}"
            );
        }
    }
}
