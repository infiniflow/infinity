//! PoC H (RUST_REWRITE_PLAN.md 附录 B / §附录 C R11): result determinism rules.
//!
//! The `.slt` acceptance path renders query results through the PostgreSQL wire
//! protocol, which calls `ColumnVector::ToString(row_index)`
//! (`src/network/connection_impl.cpp:409`). For REAL/DOUBLE that is
//! `std::to_string(double)` (`src/storage/column_vector/column_vector_impl.cpp:971-975`),
//! i.e. printf `%f` -> **fixed 6 decimals** (`27.1332` -> `27.133200`), NOT the
//! shortest round-trip used by `Value::ToString()` (`value_impl.cpp:1423-1439`).
//!
//! This programs checks three things:
//!   1. Rust `format!("{:.6}", x)` agrees with C `%f` on a set of edge-case doubles.
//!   2. Every 6-decimal float literal in the real `.slt` corpus round-trips.
//!   3. An EXPLAIN comparator: exact vs structural (ignoring operator ids), so
//!      the rewrite can pin a *tolerance policy* rather than silently relaxing.
//!
//! Run:  cargo run -p determinism -- scan <test/sql>   (default mode)
//!       cargo run -p determinism -- values <file>

use std::fs;
use std::path::{Path, PathBuf};

fn main() {
    let mut args = std::env::args().skip(1);
    match args.next().as_deref() {
        Some("values") => {
            let f = args.next().unwrap_or_else(|| "test_values.txt".into());
            for line in fs::read_to_string(&f).unwrap().lines() {
                if line.trim().is_empty() {
                    continue;
                }
                let v: f64 = line.trim().parse().unwrap();
                println!("{:.6}", v);
            }
        }
        Some("scan") | None => {
            let root = args.next().unwrap_or_else(|| "/home/zhichyu/github.com/infiniflow/infinity/test/sql".into());
            scan_slt(Path::new(&root));
            explain_demo();
        }
        Some(other) => eprintln!("unknown mode {other}"),
    }
}

/// Extract every `\d+\.\d{6}` literal from result blocks and verify it is a
/// fixed point of `format!("{:.6}", parse(x))`.
fn scan_slt(root: &Path) {
    let mut files = vec![];
    walk(root, &mut files);
    let re_like = |tok: &str| {
        let (int, frac) = match tok.split_once('.') {
            Some(v) => v,
            None => return false,
        };
        !int.is_empty() && frac.len() == 6 && frac.chars().all(|c| c.is_ascii_digit()) && int.chars().all(|c| c.is_ascii_digit() || c == '-')
    };

    let mut total = 0usize;
    let mut ok = 0usize;
    let mut bad = vec![];
    for f in &files {
        let text = fs::read_to_string(f).unwrap_or_default();
        for tok in text.split(|c: char| !(c.is_ascii_digit() || c == '.' || c == '-')) {
            if re_like(tok) {
                total += 1;
                let v: f64 = tok.parse().unwrap();
                let s = format!("{:.6}", v);
                if s == tok {
                    ok += 1;
                } else if bad.len() < 20 {
                    bad.push(format!("{}: {tok} -> {s}", f.display()));
                }
            }
        }
    }
    println!("== PoC H.2: .slt 6-decimal literal round-trip ==");
    println!("  files scanned : {}", files.len());
    println!("  literals      : {total}, reproduced exactly: {ok}");
    for b in &bad {
        println!("  MISMATCH {b}");
    }
    println!(
        "  -> {}",
        if ok == total { "PASS (all fixed-points of {:.6})" } else { "ISSUES" }
    );
}

fn walk(dir: &Path, out: &mut Vec<PathBuf>) {
    if let Ok(entries) = fs::read_dir(dir) {
        let mut es: Vec<_> = entries.filter_map(|e| e.ok()).collect();
        es.sort_by_key(|e| e.path());
        for e in es {
            let p = e.path();
            if p.is_dir() {
                walk(&p, out);
            } else if p.extension().and_then(|s| s.to_str()) == Some("slt") {
                out.push(p);
            }
        }
    }
}

/// Normalise an EXPLAIN plan for *structural* comparison: drop operator ids
/// (`NAME (12)`) and internal column refs (`#4`), collapse whitespace.
fn normalize_explain(text: &str) -> String {
    let mut out = String::new();
    let bytes = text.as_bytes();
    let mut i = 0;
    while i < bytes.len() {
        // strip "#<digits>"
        if bytes[i] == b'#' {
            i += 1;
            while i < bytes.len() && bytes[i].is_ascii_digit() {
                i += 1;
            }
            out.push('#');
            continue;
        }
        // strip " (<digits>)" after an operator name
        if bytes[i] == b'(' && i > 0 && bytes[i - 1] == b' ' {
            let mut j = i + 1;
            let mut digits = 0;
            while j < bytes.len() && bytes[j].is_ascii_digit() {
                j += 1;
                digits += 1;
            }
            if digits > 0 && j < bytes.len() && bytes[j] == b')' {
                i = j + 1;
                continue;
            }
        }
        out.push(bytes[i] as char);
        i += 1;
    }
    out.split_whitespace().collect::<Vec<_>>().join(" ")
}

fn explain_demo() {
    // Two EXPLAIN outputs from the same query with different assigned ids.
    let a = " PROJECT (3)\n  - table index: #4\n  - expressions: [i (#0)]\n -> TABLE SCAN (2)\n    - table name: explain1(default_db.explain1)";
    let b = " PROJECT (7)\n  - table index: #9\n  - expressions: [i (#1)]\n -> TABLE SCAN (5)\n    - table name: explain1(default_db.explain1)";
    let exact = a == b;
    let structural = normalize_explain(a) == normalize_explain(b);
    println!("== PoC H.3: EXPLAIN comparator ==");
    println!("  exact match          : {exact}");
    println!("  structural match     : {structural}  (ids stripped)");
    println!(
        "  -> policy: accept EXACT for stable ids; otherwise require STRUCTURAL + explicit allow-list"
    );
}
