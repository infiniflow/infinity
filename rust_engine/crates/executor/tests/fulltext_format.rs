//! The invariant of the full-text chunk writer that a byte comparison once
//! surfaced.
//!
//! The byte comparison against the reference (the `alpha` doc region of an
//! 8192-row chunk) was a **porting-time acceptance**: it needs files the
//! reference wrote, which are deliberately not kept in this tree, and it is not
//! something to re-run forever (`docs/GAPS.md` §2.2). What stays here is the
//! property that needs no reference and that a future refactor could break.

use std::collections::BTreeMap;

use executor::blocks::encode_fulltext_chunk;

/// A term's postings: the document ordinal and the term's position in it.
type Postings = BTreeMap<String, Vec<(u32, u32)>>;

/// One document may repeat a term many times. `GeneratePosting`
/// (`column_inverter_impl.cpp:253-257`) adds a position per occurrence but
/// ends the document once, so `df` counts documents while `total_tf` counts
/// occurrences. Ending the document per position instead repeated the doc id
/// in the doc list, which the skip-list writer asserts on once a term has more
/// than `MAX_DOC_PER_RECORD` occurrences (a 500-row block panicked).
#[test]
fn repeated_term_counts_one_document_and_every_occurrence() {
    const DOCS: u32 = 300;
    const HIT: u32 = 150;
    const REPEATS: u32 = 200;
    let mut postings: Postings = BTreeMap::new();
    let mut lengths = vec![1u32; DOCS as usize];
    lengths[HIT as usize] = REPEATS;
    postings.insert(
        "dup".to_string(),
        (0..REPEATS).map(|pos| (HIT, pos)).collect(),
    );
    let (dic, _pos, _len) = encode_fulltext_chunk(&postings, &lengths);
    let flags = storage::invertedindex::format::TermMetaFlags {
        has_term_frequency: true,
        has_term_payload: true,
        has_doc_payload: false,
        has_block_max: false,
    };
    let (meta, _used) = storage::invertedindex::format::decode_term_meta(&dic, flags)
        .expect("the first record is the only term's meta");
    assert_eq!(meta.doc_freq, 1, "one document contains the term");
    assert_eq!(meta.total_tf, REPEATS, "every occurrence counts once");
}
