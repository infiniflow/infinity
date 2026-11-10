//! The port's index merge, checked against the port's own writer.
//!
//! Byte parity with the reference was a **porting-time acceptance**, not a
//! long-running test: the golden `.dic`/`.pos`/`.len` files came from the
//! reference engine while the port was written and are deliberately not in this
//! tree - no binaries, no generator (`docs/GAPS.md` §2.2 records what the
//! comparison covered). Keeping them would have meant either committing opaque
//! blobs that nobody can regenerate, or a test that silently skips.
//!
//! What stays is the property that is checkable without the reference: a merge of
//! two chunks must equal one write of the same rows. That pins the merge path
//! (doc-id rebase, position replay, re-blocking) against the writer, so a
//! refactor of either side still fails loudly.
//!
//! The corpus is the same seven documents the one-time comparison used:
//!
//! ```text
//! chunk1 (rows 0..3)            chunk2 (rows 4..6)
//!  0 "alpha beta"                4 "alpha gamma gamma"
//!  1 "alpha"                     5 "beta"
//!  2 "beta gamma"                6 "alpha beta gamma"
//!  3 "alpha alpha beta"
//! ```

use std::collections::BTreeMap;

use storage::invertedindex::disk_segment_reader::{write_segment_index, PostingEntry};
use storage::invertedindex::format::OPTION_FLAG_ALL;
use storage::invertedindex::index_merger::{ColumnIndexMerger, ColumnSource};

const SUFFIXES: [&str; 3] = [".dic", ".pos", ".len"];

/// The corpus as the writer wants it: the documents and one posting list per
/// term.
///
/// Positions are `word_offset`, the analyzer's token ordinal
/// (`ColumnInverter::AddTerm`/`GeneratePosting`,
/// `column_inverter_impl.cpp:112-128,253`), which is why `"alpha alpha beta"`
/// holds `alpha` at 0 and 1 and `beta` at 2.
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

/// A merge of two chunks must equal one write of the same rows.
#[test]
fn a_merge_of_two_chunks_equals_a_single_write() {
    let dir = tempfile::tempdir().unwrap();
    let d = dir.path().to_str().unwrap();
    let (docs, postings) = reference_rows();
    let split = 4usize;
    let right_base = docs[split].0;
    let left_docs = docs[..split].to_vec();
    let right_docs = docs[split..].to_vec();
    let left_postings: BTreeMap<String, Vec<PostingEntry>> = postings
        .iter()
        .map(|(term, entries)| {
            let list = entries
                .iter()
                .filter(|(row, _, _)| *row < right_base)
                .cloned()
                .collect();
            (term.clone(), list)
        })
        .collect();
    let right_postings: BTreeMap<String, Vec<PostingEntry>> = postings
        .iter()
        .map(|(term, entries)| {
            let list = entries
                .iter()
                .filter(|(row, _, _)| *row >= right_base)
                .cloned()
                .collect();
            (term.clone(), list)
        })
        .collect();
    write_segment_index(d, "left", 0, &left_docs, &left_postings, OPTION_FLAG_ALL).unwrap();
    write_segment_index(
        d,
        "right",
        right_base,
        &right_docs,
        &right_postings,
        OPTION_FLAG_ALL,
    )
    .unwrap();
    let sources = [
        ColumnSource {
            base_name: "left".to_string(),
            base_row_id: 0,
        },
        ColumnSource {
            base_name: "right".to_string(),
            base_row_id: right_base,
        },
    ];
    ColumnIndexMerger::new(d, OPTION_FLAG_ALL)
        .merge(&sources, "merged")
        .unwrap();
    write_segment_index(d, "single", 0, &docs, &postings, OPTION_FLAG_ALL).unwrap();

    for suffix in SUFFIXES {
        let merged = std::fs::read(dir.path().join(format!("merged{suffix}"))).unwrap();
        let single = std::fs::read(dir.path().join(format!("single{suffix}"))).unwrap();
        assert_eq!(
            merged, single,
            "merged{suffix} differs from writing the same rows in one segment"
        );
    }
}
