//! Ported from
//! `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp`.
//!
//! Each test feeds the same query stream through `SearchDriver::ParseSingle`
//! and `QueryNode::GetOptimizedQueryTree`; the reference only asserts that the
//! stream is consumed (`rc == 0`). A parse failure returns `-1`; an optimization
//! failure is a *recoverable* error and keeps the stream going, exactly like
//! the reference's `catch (const RecoverableException &)`.

use std::collections::HashMap;

use crate::fulltext_query::{OperatorOption, QueryParseError, SearchDriver};

const OPERATORS: [OperatorOption; 2] = [OperatorOption::Or, OperatorOption::And];

/// The reference's `ParseAndOptimizeFromStream`.
fn parse_and_optimize_from_stream(
    driver: &SearchDriver,
    stream: &str,
) -> Result<i32, QueryParseError> {
    for line in stream.lines() {
        let line = line.trim_start();
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let tree = match driver.parse_single(line) {
            Ok(tree) => tree,
            Err(QueryParseError::Analyzer(error)) => {
                return Err(QueryParseError::Analyzer(error));
            }
            Err(_) => return Ok(-1),
        };
        // The optimizer's errors are the reference's recoverable exceptions:
        // the line is reported and the stream continues.
        let _ = tree.optimize();
    }
    Ok(0)
}

#[test]
fn test1() {
    let queries = r#"
#basic_filter with implicit field
dune

#basic_filter with explicit field
name:dune

#basic_filter_boost with implicit field
dune^1.2

#basic_filter_boost with explicit field
name:dune^1.2

#term
NOT name:microsoft^1.2
-name:google^1.2
!name:microsoft^1.2
(name:star OR name:duna)
(dune god)
(dune god)^1.3
!(dune god)^1.3

#clause
foo AND name:duna^1.2
foo +bar
duna^2 AND foo
duna AND foo
(dune god) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
dune god
dune OR god
name:star OR name:duna
_exists_:"author" AND page_count:yyy AND (name:star OR name:duna)
_exists_:"author" AND page_count:zzz^1.3 AND (name:star^0.1 OR name:duna^1.2)^1.2

#test invalid not query
NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2
(NOT name:god^2 OR NOT kddd:ss^4) OR ee:ff^1.2
(NOT name:god^2 OR NOT kddd:ss^4) OR NOT ee:ff^1.2
(NOT name:god^2 AND NOT kddd:ss^4) OR NOT ee:ff^1.2

#test optimization rules
(dune^1.2 AND NOT name:god^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(dune^1.2 AND NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(dune^1.2 AND (NOT name:god^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3
sda:rtw AND ((NOT name:god^2 AND NOT kddd:ss^4) OR NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 OR NOT kddd:ss^4) AND NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 AND NOT kddd:ss^4) AND NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 OR NOT kddd:ss^4) OR NOT ee:ff^1.2)
    "#;

    let driver = SearchDriver::new(HashMap::new(), "body");
    assert_eq!(parse_and_optimize_from_stream(&driver, queries), Ok(0));
}

#[test]
fn operator_option_test_standard() {
    let queries = r#"
#query
dune god
dune OR god
name:star OR name:duna
_exists_:"author" AND page_count:yyy AND (name:star OR name:duna)
_exists_:"author" AND page_count:zzz^1.3 AND (name:star^0.1 OR name:duna^1.2)^1.2

#test invalid not query
NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2
(NOT name:god^2 OR NOT kddd:ss^4) OR ee:ff^1.2
(NOT name:god^2 OR NOT kddd:ss^4) OR NOT ee:ff^1.2
(NOT name:god^2 AND NOT kddd:ss^4) OR NOT ee:ff^1.2

#test optimization rules
(dune^1.2 AND NOT name:god^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(dune^1.2 AND NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(dune^1.2 AND (NOT name:god^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3
sda:rtw AND ((NOT name:god^2 AND NOT kddd:ss^4) OR NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 OR NOT kddd:ss^4) AND NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 AND NOT kddd:ss^4) AND NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 OR NOT kddd:ss^4) OR NOT ee:ff^1.2)
    "#;

    for operator in OPERATORS {
        let driver = SearchDriver::new(HashMap::new(), "body").with_operator(operator);
        assert_eq!(
            parse_and_optimize_from_stream(&driver, queries),
            Ok(0),
            "{operator:?}"
        );
    }
}

#[test]
fn operator_option_test_chinese() {
    let queries = r#"
#basic_filter_boost with explicit field
name:芯片^1.2
name:dune^1.2
num:123.456^7.8

#clause
(邓肯 上帝) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
(邓肯^1.2 AND (NOT name:上帝^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#quote
TO BE OR NOT TO BE
吉祥物“羽宝”头部
nanjing吉祥物"羽宝"头部head "DS-K3AJ303/Dm140"
吉祥物nanjing"DS-K3AJ303/Dm140"头部
graphic cards
    "#;

    let mut field2analyzer = HashMap::new();
    let mut indexes = HashMap::new();
    indexes.insert("ft_index".to_string(), "chinese".to_string());
    field2analyzer.insert("body".to_string(), indexes);

    for operator in OPERATORS {
        let driver = SearchDriver::new(field2analyzer.clone(), "body").with_operator(operator);
        match parse_and_optimize_from_stream(&driver, queries) {
            Ok(rc) => assert_eq!(rc, 0, "{operator:?}"),
            // The reference catches the recoverable exception raised when the
            // analyzer's dictionary files are unavailable in CI.
            Err(QueryParseError::Analyzer(_)) => {}
            Err(error) => panic!("{operator:?}: {error}"),
        }
    }
}
