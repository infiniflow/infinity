//! Ported from `src/unit_test/storage/invertedindex/search/query_match_ut.cpp`.
//!
//! The reference builds a table with three varchar columns, a full-text index
//! on `text` and four documents, then queries through `QueryBuilder`. The
//! in-memory column index below is the query view of the reference's
//! `MemoryIndexer` (the fixture inserts a single unsealed segment).

use std::collections::HashMap;

use crate::fulltext_query::{MemoryColumnIndex, MemorySegment, SearchDriver};
use crate::query_builder::{DocIteratorType, EarlyTermAlgo, FullTextQueryContext, QueryBuilder};

const DOCS: &[(&str, &str, &str)] = &[
    (
        "1",
        "Animalia (book)",
        r#"Animalia is an illustrated children's book by anarchism Graeme Base. It was originally published in 1986, followed by a tenth anniversary edition in 1996, and a 25th anniversary edition in 2012. Over three million copies have been sold. A special numbered and signed anniversary edition was also published in 1996, with an embossed gold jacket."#,
    ),
    (
        "2",
        "Academy Award for Best Production Design",
        r#"The Academy Awards are the oldest awards ceremony for achievements in motion pictures. one of The add test Academy Award for Best Production Design recognizes achievement in art direction on a film. The category's original name was Best Art Direction, but was changed to its current name in 2012 for the 85th Academy Awards.  This change resulted from the Art Director's branch of the Academy being renamed the Designer's branch."#,
    ),
    (
        "3",
        "Animation",
        r#"The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots."#,
    ),
    (
        "4",
        "Foobar",
        r#"周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。"#,
    ),
];

/// The table's full-text index: one segment over the `text` column.
fn test_index() -> MemoryColumnIndex {
    let mut index = MemoryColumnIndex::new();
    let mut segment = MemorySegment::new("text");
    for (row, (_, _, text)) in DOCS.iter().enumerate() {
        segment.insert(row as u64, text);
    }
    index.push(segment);
    index
}

/// `QueryBuilder::GetColumn2Analyzer` for an index on `text` using `standard`.
fn driver() -> SearchDriver {
    let mut fields = HashMap::new();
    fields.insert("text".to_string(), HashMap::new());
    // `SearchOptions` supplies no `default_field`; the field list does.
    SearchDriver::new(fields, "")
}

/// `QueryMatchTest::QueryMatch`: parse `match_text` on `text`, build the
/// iterator, drive it to exhaustion and compare the accumulated statistics.
fn query_match(
    index: &MemoryColumnIndex,
    match_text: &str,
    expected_doc_freq: u32,
    expected_freq: f64,
    phrase: bool,
) {
    let tree = driver()
        .parse_single_with_fields("text", match_text)
        .unwrap();
    let builder = QueryBuilder::new(index);
    let mut iterator = builder
        .create_search(FullTextQueryContext::new(tree).with_topn(10))
        .unwrap()
        .expect("query tree produced no iterator");

    if !iterator.next(0) {
        // The reference skips its assertions when the iterator is empty; the
        // table's expected document frequency is zero in that case.
        assert_eq!(
            expected_doc_freq, 0,
            "{match_text}: expected {expected_doc_freq} documents"
        );
        return;
    }
    loop {
        // `QueryMatch` reads Score() for every visited document; the term
        // frequency is accumulated there.
        iterator.score();
        let doc_id = iterator.doc_id();
        if !iterator.next(doc_id + 1) {
            break;
        }
    }

    if phrase {
        assert_eq!(
            iterator.iterator_type(),
            DocIteratorType::PhraseIterator,
            "{match_text}"
        );
        assert_eq!(iterator.doc_freq(), expected_doc_freq, "{match_text}");
        assert!(
            (iterator.phrase_freq() - expected_freq).abs() < 1e-6,
            "{match_text}: phrase freq {} != {expected_freq}",
            iterator.phrase_freq()
        );
    } else {
        assert_eq!(
            iterator.iterator_type(),
            DocIteratorType::TermDocIterator,
            "{match_text}"
        );
        assert_eq!(iterator.doc_freq(), expected_doc_freq, "{match_text}");
        assert_eq!(iterator.term_freq(), expected_freq as u64, "{match_text}");
    }
}

#[test]
fn basic_term() {
    let index = test_index();
    let terms = ["the", "harmful", "chemical", "anarchism"];
    let expected_doc_freq = [2u32, 1, 1, 2];
    let expected_term_freq = [16.0f64, 2.0, 2.0, 2.0];
    for ((term, doc_freq), term_freq) in terms.iter().zip(expected_doc_freq).zip(expected_term_freq)
    {
        query_match(&index, term, doc_freq, term_freq, false);
    }
}

#[test]
fn phrase() {
    let index = test_index();
    let phrases = [
        r#""Animalia is an""#,
        r#""one of""#,
        r#""are book""#,
        r#""add test""#,
        r#""harmful chemical""#,
        r#""chemical harmful"~2"#,
        r#""harmful chemical"~10"#,
        r#""make up team"~2"#,
    ];
    let expected_doc_freq = [1u32, 2, 0, 2, 1, 1, 1, 1];
    let expected_phrase_freq = [
        1.0f64,
        2.0,
        0.0,
        3.0,
        2.0,
        2.0 / 3.0,
        2.0 + 2.0 / 9.0,
        1.0 / 3.0,
    ];
    for ((phrase, doc_freq), phrase_freq) in phrases
        .iter()
        .zip(expected_doc_freq)
        .zip(expected_phrase_freq)
    {
        query_match(&index, phrase, doc_freq, phrase_freq, true);
    }
}

/// `std::mt19937` seeded with 42, as in the reference fixture.
struct Mt19937 {
    state: [u32; 624],
    index: usize,
}

impl Mt19937 {
    fn new(seed: u32) -> Self {
        let mut state = [0u32; 624];
        state[0] = seed;
        for i in 1..624 {
            state[i] = 1_812_433_253u32
                .wrapping_mul(state[i - 1] ^ (state[i - 1] >> 30))
                .wrapping_add(i as u32);
        }
        Self { state, index: 624 }
    }

    fn next(&mut self) -> u32 {
        if self.index >= 624 {
            for i in 0..624 {
                let y = (self.state[i] & 0x8000_0000) | (self.state[(i + 1) % 624] & 0x7fff_ffff);
                let mut next = self.state[(i + 397) % 624] ^ (y >> 1);
                if !y.is_multiple_of(2) {
                    next ^= 0x9908_b0df;
                }
                self.state[i] = next;
            }
            self.index = 0;
        }
        let mut y = self.state[self.index];
        self.index += 1;
        y ^= y >> 11;
        y ^= (y << 7) & 0x9d2c_5680;
        y ^= (y << 15) & 0xefc6_0000;
        y ^= y >> 18;
        y
    }
}

/// `pick`: `fmt::format("term{:03d} ", rng() % (1 + rng() % 300))`.
fn pick(rng: &mut Mt19937) -> String {
    let low = rng.next();
    let bound = 1 + rng.next() % 300;
    format!("term{:03} ", low % bound)
}

fn words(rng: &mut Mt19937, count: u32) -> String {
    (0..count).map(|_| pick(rng)).collect()
}

#[test]
fn bmw_many_terms() {
    // Four insert batches: four segments of 1500 documents, 40 terms each.
    let mut rng = Mt19937::new(42);
    let mut index = MemoryColumnIndex::new();
    for segment_id in 0..4u32 {
        let mut segment = MemorySegment::new("text");
        for i in 0..1500u32 {
            segment.insert(u64::from(segment_id * 1500 + i), &words(&mut rng, 40));
        }
        index.push(segment);
    }
    // 200 query terms drawn after the corpus, as in the reference.
    let query = words(&mut rng, 200);

    let tree = driver().parse_single_with_fields("text", &query).unwrap();
    let builder = QueryBuilder::new(&index);
    let mut iterator = builder
        .create_search(
            FullTextQueryContext::new(tree)
                .with_topn(10)
                .with_early_term(EarlyTermAlgo::Bmw),
        )
        .unwrap()
        .expect("BMW search produced no iterator");
    assert_eq!(iterator.iterator_type(), DocIteratorType::BmwIterator);

    // The reference fills a `FullTextScoreResultHeap(topn)` from the
    // iterator's results and expects it to be full.
    let mut result_size = 0usize;
    let mut cursor = 0u64;
    while iterator.next(cursor) {
        iterator.score();
        result_size += 1;
        cursor = iterator.doc_id() + 1;
    }
    assert_eq!(result_size, 10);
}
