// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Ported from `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp`.
//!
//! The reference builds a `RAGAnalyzer` with positions enabled and coarse
//! tokenization, then calls `Highlighter::GetHighlight(query, raw_text, …)`.
//! The C++ tests only print the result; the port asserts the same observable
//! behaviour they imply (the hit sentences survive, the misses are dropped,
//! and the matched spans are wrapped in `<em>`).

use crate::highlight_with_analyzer;
use analyzer::{Analyzer, AnalyzerPool};

/// `HighlighterTest::SetUp`: `RAGAnalyzer` with `SetEnablePosition(true)` and
/// `SetFineGrained(false)`.
fn analyzer() -> Analyzer {
    let mut analyzer = AnalyzerPool::instance()
        .get_analyzer("rag")
        .expect("RAGAnalyzer");
    let Analyzer::Rag(rag) = &mut analyzer else {
        panic!("rag did not return a RAGAnalyzer");
    };
    rag.set_enable_position(true);
    rag.set_fine_grained(false);
    analyzer
}

/// `HighlighterTest::test1`.
#[test]
fn test1() {
    let raw_text = "\nonce upon a time there lives an dog.this is a sentence,That is another.你好 世界 dog 你好dog\n\n真的好么？\n    ";
    let query = "dog 你好";
    let output = highlight_with_analyzer(query, raw_text, &analyzer());
    println!("{output}");

    assert!(output.contains("<em>dog</em>"), "{output}");
    assert!(output.contains("<em>你好</em>"), "{output}");
    // The sentence without a hit is not part of the highlight.
    assert!(!output.contains("another"), "{output}");
}

/// `HighlighterTest::test2`.
#[test]
fn test2() {
    let raw_text = "The runner is running and jumped over jumps happily\n    ";
    let query = "run jump happy";
    let output = highlight_with_analyzer(query, raw_text, &analyzer());
    println!("{output}");

    assert!(output.contains("<em>running</em>"), "{output}");
    assert!(output.contains("<em>jumped</em>"), "{output}");
    assert!(output.contains("<em>jumps</em>"), "{output}");
}

/// `HighlighterTest::test3`.
#[test]
fn test3() {
    let raw_text = "{{Redirect|Anarchist|the fictional character|Anarchist (comics)}} {{Redirect|Anarchists}} {{Anarchism sidebar}} {{Libertarianism sidebar}}  '''Anarchism''' is generally defined as the [[political philosophy]] which holds the [[state (polity)|state]] to be undesirable, unnecessary, and harmful,<ref name=\"definition\"> {{Cite journal|last=Malatesta|first=Errico|title=Towards Anarchism|journal=MAN!|publisher=International Group of San Francisco|location=Los Angeles|oclc=3930443|url=http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm|authorlink=Errico Malatesta}} {{Cite journal|url=http://www.theglobeandmail.com/servlet/story/RTGAM.20070514.wxlanarchist14/BNStory/lifeWork/home/ |title=Working for The Man |journal=[[The Globe and Mail]] |accessdate=2008-04-14 |last=Agrell |first=Siri |date=2007-05-14}} {{cite web|url=http://www.britannica.com/eb/article-9117285|title=Anarchism|year=2006|work=Encyclopædia Britannica|publisher=Encyclopædia Britannica Premium Service|accessdate=2006-08-29| archiveurl=";
    let query = "harmful chemical anarchism Premium";
    let output = highlight_with_analyzer(query, raw_text, &analyzer());
    println!("{output}");

    assert!(output.contains("<em>Anarchism</em>"), "{output}");
    assert!(output.contains("<em>harmful</em>"), "{output}");
    assert!(output.contains("<em>Premium</em>"), "{output}");
    // `chemical` only appears in the query, not in the document.
    assert!(!output.contains("<em>chemical</em>"), "{output}");
}

/// `HighlighterTest::test4`.
#[test]
fn test4() {
    let raw_text = "南京的南京长江大桥是中国的一条重要的交通. 南京长江大桥很壮观";
    let query = "南京长江大桥";
    let output = highlight_with_analyzer(query, raw_text, &analyzer());
    println!("{output}");

    assert_eq!(
        output.matches("<em>南京长江大桥</em>").count(),
        2,
        "{output}"
    );
    assert!(
        output.starts_with("南京的<em>南京长江大桥</em>"),
        "{output}"
    );
}

/// A document stored decomposed (NFD) is highlighted over its own bytes when
/// the query term is NFC, which is what the analyzer emits for any query.
#[test]
fn nfd_document_highlights_nfc_query() {
    let raw_text = "tie\u{302}\u{301}ng vie\u{323}\u{302}t";
    let output = highlight_with_analyzer("ti\u{1ebf}ng", raw_text, &analyzer());
    assert_eq!(output, "<em>tie\u{302}\u{301}ng</em> vie\u{323}\u{302}t");
}

/// An NFD query goes through the analyzer first, so it reaches the highlighter
/// as NFC and still finds the NFD document.
#[test]
fn nfd_document_highlights_nfd_query() {
    let raw_text = "tie\u{302}\u{301}ng vie\u{323}\u{302}t";
    let output = highlight_with_analyzer("vie\u{323}\u{302}t", raw_text, &analyzer());
    assert_eq!(output, "tie\u{302}\u{301}ng <em>vie\u{323}\u{302}t</em>");
}

/// A document that is already NFC highlights exactly as before.
#[test]
fn nfc_document_highlight_unchanged() {
    let raw_text = "ti\u{1ebf}ng vi\u{1ec7}t";
    let output = highlight_with_analyzer("ti\u{1ebf}ng", raw_text, &analyzer());
    assert_eq!(output, "<em>ti\u{1ebf}ng</em> vi\u{1ec7}t");
}
