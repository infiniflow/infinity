//! Full-text scoring invariants (plan §6 Gate G / INVARIANT_MATRIX G5).
//!
//! The BM25 scorer is an approximation of the C++/iresearch one, so these tests
//! assert *monotonic invariants* rather than exact scores:
//!   * more occurrences of the query term (higher tf) score no lower;
//!   * a document with the same tf but more tokens (longer) scores no higher.

use infinity_executor::{Engine, QueryResult};

/// Runs a text search and returns `(id, score)` pairs keyed by the first column.
fn scores(engine: &Engine, query: &str) -> Vec<(i64, f64)> {
    let sql = format!(
        "SELECT id, ROUND(SCORE(), 6) FROM t SEARCH MATCH TEXT ('doc', '{query}', 'topn=10');"
    );
    let rs = engine.execute(&sql).expect("select");
    let rows = match &rs[0] {
        QueryResult::Rows { rows, .. } => rows.clone(),
        other => panic!("expected rows, got {other:?}"),
    };
    rows.iter()
        .map(|r| {
            let id = r[0].parse::<i64>().expect("id");
            let score = r[1].parse::<f64>().expect("score");
            (id, score)
        })
        .collect()
}

fn score_of(pairs: &[(i64, f64)], id: i64) -> f64 {
    pairs
        .iter()
        .find(|(i, _)| *i == id)
        .map(|(_, s)| *s)
        .unwrap_or_else(|| panic!("id {id} not found in {pairs:?}"))
}

#[test]
fn higher_term_frequency_scores_no_lower() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO t VALUES (1, 'apple apple'), (3, 'apple banana cherry date');")
        .unwrap();
    let s = scores(&e, "apple");
    // doc 1 has tf=2, doc 3 has tf=1 (and is longer): doc 1 must not score lower
    assert!(
        score_of(&s, 1) >= score_of(&s, 3),
        "tf=2 should not score below tf=1: {s:?}"
    );
}

#[test]
fn shorter_document_scores_no_lower_at_equal_tf() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .unwrap();
    // same tf (1) for the query term, different document lengths
    e.execute(
        "INSERT INTO t VALUES (2, 'apple banana'), (3, 'apple banana cherry date fig grape');",
    )
    .unwrap();
    let s = scores(&e, "apple");
    assert!(
        score_of(&s, 2) >= score_of(&s, 3),
        "shorter document should not score below a longer one at equal tf: {s:?}"
    );
}

#[test]
fn non_matching_document_is_absent() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.execute("CREATE TABLE t (id INTEGER, doc VARCHAR);")
        .unwrap();
    e.execute("INSERT INTO t VALUES (1, 'apple'), (2, 'pear');")
        .unwrap();
    let s = scores(&e, "apple");
    assert_eq!(s.iter().map(|(i, _)| *i).collect::<Vec<_>>(), vec![1]);
}
