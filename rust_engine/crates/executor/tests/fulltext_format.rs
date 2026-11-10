//! The port's full-text chunk must be **byte for byte** the reference's.
//!
//! The fixture is the reference's own doc region - the skip list and the doc
//! list of its first term - taken from a directory the reference wrote
//! (`ft_0000000000000000.pos`, the term `alpha` of a table with 8192 rows of
//! `alpha beta doc{i}`). The port has to produce the same bytes for the same
//! input, because the reference decodes the region with its own
//! `DocListEncoder::Dump` framing (a skip-list length, a doc-list length, the
//! skip list and then the doc list) and its own
//! `CompositeCodec<SIMDBinaryPacking, VariableByte>` field encoding.

use std::collections::BTreeMap;

use executor::blocks::encode_fulltext_chunk;

/// A term's postings: the document ordinal and the term's position in it.
type Postings = BTreeMap<String, Vec<(u32, u32)>>;

/// The reference's input: 8192 rows of `alpha beta doc{i}`.
///
/// The analyzer emits the word and, when it differs, its stem at the same
/// offset, so `alpha` and `beta` occur in every row at offsets 0 and 6, and
/// `doc{i}` occurs once at offset 12.
fn reference_input() -> (Postings, Vec<u32>) {
    let mut postings: Postings = BTreeMap::new();
    let mut lengths = Vec::with_capacity(8192);
    for doc in 0..8192u32 {
        lengths.push(3);
        postings
            .entry("alpha".to_string())
            .or_default()
            .push((doc, 0));
        postings
            .entry("beta".to_string())
            .or_default()
            .push((doc, 6));
        postings
            .entry(format!("doc{doc}"))
            .or_default()
            .push((doc, 12));
    }
    (postings, lengths)
}

/// The doc region of the first term: the skip list and the doc list, without
/// the two lengths that frame them (the reference writes those in the `.pos`
/// file, not in the region).
fn first_term_doc_region(pos: &[u8]) -> Vec<u8> {
    let mut at = 0usize;
    let read_vint = |buf: &[u8], at: &mut usize| {
        let mut value = 0u64;
        let mut shift = 0u32;
        loop {
            let byte = buf[*at];
            *at += 1;
            value |= u64::from(byte & 0x7f) << shift;
            if byte & 0x80 == 0 {
                break;
            }
            shift += 7;
        }
        value
    };
    let skiplist_len = read_vint(pos, &mut at) as usize;
    let doc_list_len = read_vint(pos, &mut at) as usize;
    let region = &pos[at..at + skiplist_len + doc_list_len];
    region.to_vec()
}

#[test]
fn fulltext_doc_region_matches_the_reference_bytes() {
    let fixture = include_bytes!("fixtures/cpp_ft_doc_region.bin");
    let (postings, lengths) = reference_input();
    let (_dic, pos, _len) = encode_fulltext_chunk(&postings, &lengths);
    let got = first_term_doc_region(&pos);
    assert_eq!(
        got.len(),
        fixture.len(),
        "the port's doc region is a different length than the reference's"
    );
    assert!(
        got == fixture.to_vec(),
        "the port's doc region differs from the reference's bytes"
    );
}

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
