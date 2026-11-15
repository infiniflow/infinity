//! Large-corpus format check against the reference's own bytes.
//!
//! The generator mirrors
//! `src/unit_test/storage/invertedindex/fulltext_big_golden_ut.cpp` term for
//! term, so both sides build the same corpus:
//!
//! ```text
//! tokens  = 5 + doc % 40, or 2000 when doc % 1000 == 999
//! word id = (doc * 7 + j * 13 + (doc * j) % 97) % 4096
//! word    = 'w' + three base-26 letters ('waaa' .. 'wdpp')
//! ```
//!
//! 4096 pure-lowercase-ASCII terms and tens of thousands of documents give
//! every posting list hundreds of documents (many 128-document blocks and long
//! skip lists), a real FST dictionary, and 2000-occurrence documents that force
//! the wider numeric encodings — the shapes a handful of rows never reach.
//!
//! The test is `#[ignore]`d because it needs the reference's output:
//!
//! ```text
//! ./build/src/test_main --gtest_filter='*FulltextBigGolden*'
//! cargo test -p executor --lib fulltext_big -- --ignored --nocapture
//! ```
//!
//! It then (1) writes the same chunks with `write_segment_index`, (2) merges
//! them with `ColumnIndexMerger`, (3) compares all nine files with the
//! reference's byte for byte, and (4) decodes the reference's merged chunk here
//! and checks its postings against the generator, so a matching pair of
//! mistakes cannot pass.

use std::collections::BTreeMap;
use std::path::Path;

use storage::invertedindex::disk_segment_reader::{
    write_segment_index, PostingEntry, SegmentIndexReader,
};
use storage::invertedindex::format::OPTION_FLAG_ALL;
use storage::invertedindex::index_merger::{ColumnIndexMerger, ColumnSource};

use crate::engine::text_index::TextTokenizer;

const WORD_COUNT: u32 = 4096;

fn env_u32(name: &str, fallback: u32) -> u32 {
    std::env::var(name)
        .ok()
        .and_then(|value| value.parse::<u32>().ok())
        .filter(|value| *value > 0)
        .unwrap_or(fallback)
}

fn word_name(id: u32) -> String {
    let mut word = String::from("w");
    word.push(char::from(b'a' + ((id / 676) % 26) as u8));
    word.push(char::from(b'a' + ((id / 26) % 26) as u8));
    word.push(char::from(b'a' + (id % 26) as u8));
    word
}

/// The reference's generator (`BigParagraphs`).
fn big_paragraphs(first_doc: u32, doc_count: u32) -> Vec<String> {
    let mut paragraphs = Vec::with_capacity(doc_count as usize);
    for offset in 0..doc_count {
        let doc = first_doc + offset;
        let mut tokens = 5 + doc % 40;
        if doc % 1000 == 999 {
            tokens = 2000;
        }
        let mut text = String::with_capacity(tokens as usize * 5);
        for j in 0..tokens {
            let word_id =
                ((u64::from(doc) * 7 + u64::from(j) * 13 + (u64::from(doc) * u64::from(j)) % 97)
                    % u64::from(WORD_COUNT)) as u32;
            if j != 0 {
                text.push(' ');
            }
            text.push_str(&word_name(word_id));
        }
        paragraphs.push(text);
    }
    paragraphs
}

type Rows = (Vec<(u64, u32)>, BTreeMap<String, Vec<PostingEntry>>);

/// The postings the analyzer produces for `paragraphs`, in the writer's shape.
fn rows_from_paragraphs(
    tokenizer: &TextTokenizer,
    paragraphs: &[String],
    base_row_id: u64,
) -> Rows {
    let mut docs = Vec::with_capacity(paragraphs.len());
    let mut postings: BTreeMap<String, Vec<PostingEntry>> = BTreeMap::new();
    for (index, text) in paragraphs.iter().enumerate() {
        let row = base_row_id + index as u64;
        let mut per_term: BTreeMap<String, Vec<u32>> = BTreeMap::new();
        let mut length = 0u32;
        let streamed = tokenizer.for_each_term(text, &mut |term| {
            length += 1;
            per_term
                .entry(term.text.to_string())
                .or_default()
                .push(term.word_offset);
        });
        assert!(streamed, "the default tokenizer streams its terms");
        docs.push((row, length));
        for (term, positions) in per_term {
            postings
                .entry(term)
                .or_default()
                .push((row, positions.len() as u32, positions));
        }
    }
    (docs, postings)
}

fn golden_dir() -> String {
    std::env::var("FT_BIG_OUT").unwrap_or_else(|_| "/tmp/rust_ft_big".to_string())
}

#[test]
#[ignore = "needs the reference's bytes: ./build/src/test_main --gtest_filter='*FulltextBigGolden*'"]
fn a_large_corpus_is_byte_identical_to_the_reference() {
    let doc_count = env_u32("FT_BIG_DOCS", 20_000);
    let chunk_docs = env_u32("FT_BIG_CHUNK_DOCS", 10000);
    assert_eq!(
        doc_count % chunk_docs,
        0,
        "the corpus has to split into equal chunks, like the generator's"
    );
    let chunk_count = doc_count / chunk_docs;

    let out_dir = golden_dir();
    let cpp_dir = Path::new(&out_dir).join("cpp");
    assert!(
        cpp_dir.join("big_merged.pos").exists(),
        "run the reference first: ./build/src/test_main --gtest_filter='*FulltextBigGolden*'"
    );

    // The write side: the same chunks, the same row ranges.
    let work = tempfile::tempdir().unwrap();
    let tokenizer = TextTokenizer::default();
    let mut sources = Vec::with_capacity(chunk_count as usize);
    for chunk in 0..chunk_count {
        let first_doc = chunk * chunk_docs;
        let base_name = format!("big_chunk{chunk}");
        let paragraphs = big_paragraphs(first_doc, chunk_docs);
        let (docs, postings) = rows_from_paragraphs(&tokenizer, &paragraphs, u64::from(first_doc));
        write_segment_index(
            work.path().to_str().unwrap(),
            &base_name,
            u64::from(first_doc),
            &docs,
            &postings,
            OPTION_FLAG_ALL,
        )
        .unwrap();
        sources.push(ColumnSource {
            base_name,
            base_row_id: u64::from(first_doc),
        });
    }

    // The merge side.
    ColumnIndexMerger::new(work.path().to_str().unwrap(), OPTION_FLAG_ALL)
        .merge(&sources, "big_merged")
        .unwrap();

    // Byte-for-byte, every file the reference wrote.
    let mut bases: Vec<String> = (0..chunk_count)
        .map(|chunk| format!("big_chunk{chunk}"))
        .collect();
    bases.push("big_merged".to_string());
    for base in &bases {
        for suffix in [".dic", ".pos", ".len"] {
            let want = std::fs::read(cpp_dir.join(format!("{base}{suffix}"))).unwrap();
            let got = std::fs::read(work.path().join(format!("{base}{suffix}"))).unwrap();
            assert_eq!(
                got.len(),
                want.len(),
                "{base}{suffix}: {} bytes here, {} in the reference",
                got.len(),
                want.len()
            );
            assert_eq!(got, want, "{base}{suffix} differs from the reference");
        }
    }

    // Decode the reference's merged chunk here: the postings have to be the
    // generator's, so a matching pair of mistakes cannot pass.
    let reader =
        SegmentIndexReader::open(cpp_dir.to_str().unwrap(), "big_merged", 0, OPTION_FLAG_ALL)
            .unwrap();
    assert_eq!(reader.doc_count(), doc_count as usize);

    let mut expected_docs = Vec::with_capacity(doc_count as usize);
    let mut expected_postings: BTreeMap<String, Vec<(u64, u32)>> = BTreeMap::new();
    let mut expected_positions: BTreeMap<String, usize> = BTreeMap::new();
    for chunk in 0..chunk_count {
        let first_doc = chunk * chunk_docs;
        let paragraphs = big_paragraphs(first_doc, chunk_docs);
        let (docs, postings) = rows_from_paragraphs(&tokenizer, &paragraphs, u64::from(first_doc));
        expected_docs.extend(docs);
        for (term, entries) in postings {
            let list = expected_postings.entry(term.clone()).or_default();
            let total = expected_positions.entry(term).or_insert(0);
            for (row, tf, positions) in entries {
                list.push((row, tf));
                *total += positions.len();
            }
        }
    }
    for (row, len) in &expected_docs {
        assert_eq!(
            reader.doc_len(*row as u32),
            Some(*len),
            "doc_len of row {row}"
        );
    }
    // Spot-check the dense end of the dictionary (every term is heavy at this
    // size) without walking 4096 lists: the first, the last and the middle.
    let mut terms: Vec<&String> = expected_postings.keys().collect();
    terms.sort();
    for index in [0usize, terms.len() / 2, terms.len() - 1] {
        let term = terms[index];
        assert_eq!(
            reader.postings(term).unwrap(),
            expected_postings[term],
            "postings of '{term}'"
        );
        let meta = reader
            .lookup(term)
            .unwrap()
            .expect("term is in the dictionary");
        assert_eq!(
            meta.doc_freq as usize,
            expected_postings[term].len(),
            "df of '{term}'"
        );
        assert_eq!(
            meta.total_tf as usize, expected_positions[term],
            "ttf of '{term}'"
        );
    }
}
