//! PoC E (RUST_REWRITE_PLAN.md 附录 B / §6 Gate A): can a Rust toolchain ingest
//! the existing `test/sql/**/*.slt` acceptance assets?
//!
//! This is a self-contained `.slt` record parser/validator (no external crate,
//! no network). It walks every `.slt` file, parses each record, and reports:
//!   * file count
//!   * `statement ok` / `statement error` / `query` counts
//!   * query type-letter and modifier distribution
//!   * how many query records carry an expected `----` block
//!   * any structurally malformed records
//!
//! It proves the assets are machine-consumable from Rust; actually executing the
//! SQL requires the (not-yet-built) Rust engine, which is out of PoC scope.
//!
//! Run:  cargo run -p slt-runner -- <repo-root-or-test/sql>

use std::collections::BTreeMap;
use std::fs;
use std::path::{Path, PathBuf};

#[derive(Default)]
struct Stats {
    files: usize,
    statement_ok: usize,
    statement_error: usize,
    query: usize,
    query_with_expected: usize,
    other_directives: usize,
    malformed: usize,
    type_letters: BTreeMap<String, usize>,
    modifiers: BTreeMap<String, usize>,
}

fn walk(dir: &Path, out: &mut Vec<PathBuf>) {
    let mut entries: Vec<_> = fs::read_dir(dir).unwrap().filter_map(|e| e.ok()).collect();
    entries.sort_by_key(|e| e.path());
    for e in entries {
        let p = e.path();
        if p.is_dir() {
            walk(&p, out);
        } else if p.extension().and_then(|s| s.to_str()) == Some("slt") {
            out.push(p);
        }
    }
}

fn parse_file(path: &Path, st: &mut Stats) {
    st.files += 1;
    let text = fs::read_to_string(path).unwrap_or_default();
    // records are separated by one or more blank lines
    let mut record: Vec<&str> = vec![];
    let mut records: Vec<Vec<&str>> = vec![];
    for line in text.lines() {
        if line.trim().is_empty() {
            if !record.is_empty() {
                records.push(std::mem::take(&mut record));
            }
        } else {
            record.push(line);
        }
    }
    if !record.is_empty() {
        records.push(record);
    }

    for rec in records {
        if rec.is_empty() {
            continue;
        }
        let head = rec[0].trim();
        if head.starts_with('#') {
            continue; // comment-only
        }
        let mut expected = false;
        for l in &rec {
            if l.trim() == "----" {
                expected = true;
                break;
            }
        }
        if let Some(rest) = head.strip_prefix("statement ") {
            let kind = rest.trim().split_whitespace().next().unwrap_or("");
            match kind {
                "ok" => st.statement_ok += 1,
                "error" => st.statement_error += 1,
                _ => st.other_directives += 1,
            }
        } else if let Some(rest) = head.strip_prefix("query ") {
            st.query += 1;
            if expected {
                st.query_with_expected += 1;
            }
            let mut it = rest.split_whitespace();
            if let Some(letters) = it.next() {
                *st.type_letters.entry(letters.to_string()).or_default() += 1;
            }
            for m in it {
                *st.modifiers.entry(m.to_string()).or_default() += 1;
            }
        } else if head.starts_with("hash-threshold")
            || head.starts_with("halt")
            || head.starts_with("skipif")
            || head.starts_with("onlyif")
            || head.starts_with("include")
            || head.starts_with("loop")
            || head.starts_with("foreach")
        {
            st.other_directives += 1;
        } else {
            st.malformed += 1;
            eprintln!("  [malformed] {}: unknown directive {:?}", path.display(), head);
        }
    }
}

fn main() {
    let arg = std::env::args().nth(1);
    let root = arg.map(PathBuf::from).unwrap_or_else(|| PathBuf::from("test/sql"));
    let root = if root.join("test/sql").is_dir() {
        root.join("test/sql")
    } else {
        root
    };

    let mut files = vec![];
    walk(&root, &mut files);
    let mut st = Stats::default();
    for f in &files {
        parse_file(f, &mut st);
    }

    println!("== PoC E: .slt asset ingestion ({}) ==", root.display());
    println!("  files              : {}", st.files);
    println!("  statement ok       : {}", st.statement_ok);
    println!("  statement error    : {}", st.statement_error);
    println!("  query              : {} (with `----`: {})", st.query, st.query_with_expected);
    println!("  other directives   : {}", st.other_directives);
    println!("  malformed records  : {}", st.malformed);
    let mut tl: Vec<_> = st.type_letters.iter().collect();
    tl.sort_by(|a, b| b.1.cmp(a.1));
    let top: Vec<String> = tl.iter().take(8).map(|(k, v)| format!("{k}={v}")).collect();
    println!("  query type letters : {}", top.join(", "));
    let mut md: Vec<_> = st.modifiers.iter().collect();
    md.sort_by(|a, b| b.1.cmp(a.1));
    let mtop: Vec<String> = md.iter().take(6).map(|(k, v)| format!("{k}={v}")).collect();
    println!("  query modifiers    : {}", if mtop.is_empty() { "(none)".into() } else { mtop.join(", ") });

    let ok = st.files > 0 && st.malformed == 0;
    println!("== PoC E result: {} ==", if ok { "PASS (assets consumable)" } else { "FAIL" });
    std::process::exit(if ok { 0 } else { 1 });
}
