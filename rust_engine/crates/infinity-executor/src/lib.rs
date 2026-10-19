//! `infinity-executor`: parser -> planner -> execution (M2).
//!
//! A `SELECT` runs as a **bounded, per-edge-concurrent** pipeline: the scan
//! (producer) and the operator chain (consumer) run concurrently over a bounded
//! `tokio::sync::mpsc` channel, so a full channel applies backpressure instead of
//! deadlocking (plan §3 D4 / PoC A: "both ends of a bounded exchange must run
//! concurrently, independent of consumer type").
//!
//! Blocking operators (join, aggregate, sort) materialize; streaming operators
//! (filter, projection, limit) are applied over the materialized set.

#![forbid(unsafe_code)]

use std::collections::HashMap;
use std::io::Write;
use std::os::unix::fs::FileExt;
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex};

use infinity_catalog::Catalog;
use infinity_common::{Error, Result};
use infinity_expression::{aggregate, Expr, Value};
use infinity_index::{Index, SecondaryIndex};
use infinity_parser::{
    parse, AggFunc, CmpOp, ExprAst, JoinClause, Literal, MatchClause, Predicate, SearchClause,
    SelectItem, Statement,
};
use infinity_planner::{plan_select, LogicalPlan};
#[cfg(not(feature = "rocksdb-store"))]
use infinity_storage::FjallMetaStore;
use infinity_storage::MetaStore;
use infinity_txn::{Txn, TxnManager};

mod explain;

#[cfg(test)]
mod catalog_ut;

mod explain_search;

pub mod analyzer;
mod hnsw_mem;
mod knn_scan;
mod profiler;
mod regex_literal;
mod variables;
mod wand;

mod cpp_blocks;

/// Result of executing one statement.
#[derive(Debug, Clone, PartialEq)]
pub enum QueryResult {
    CreatedTable {
        id: u64,
        name: String,
    },
    CreatedDatabase {
        name: String,
    },
    DroppedDatabase {
        name: String,
    },
    CreatedIndex {
        name: String,
        table: String,
        column: String,
    },
    DroppedTable {
        name: String,
        existed: bool,
    },
    DroppedIndex {
        name: String,
        existed: bool,
    },
    Inserted {
        count: usize,
    },
    Deleted {
        count: usize,
    },
    Updated {
        count: usize,
    },
    Rows {
        columns: Vec<String>,
        rows: Vec<Vec<String>>,
    },
}

#[derive(Debug, Clone)]
enum Stage {
    Filter(Expr),
    Projection(Vec<Expr>, Vec<String>),
    Aggregate {
        group_keys: Vec<Expr>,
        group_names: Vec<String>,
        aggs: Vec<(AggFunc, Vec<usize>, bool, String)>,
    },
    Sort {
        keys: Vec<(infinity_expression::Expr, bool)>,
    },
    Limit(u64),
    Offset(u64),
}

/// The relation that feeds the operator stages.
#[derive(Debug, Clone)]
enum Source {
    Values {
        columns: Vec<String>,
        rows: Vec<Vec<Value>>,
    },
    Table {
        columns: Vec<String>,
        table: String,
    },
    Join {
        columns: Vec<String>,
        left: String,
        right: String,
        left_key: usize,
        right_key: usize,
        left_width: usize,
    },
}

fn row_key(table: &str, seq: usize) -> Vec<u8> {
    format!("row:{table}:{seq:016}").into_bytes()
}
fn row_prefix(table: &str) -> Vec<u8> {
    format!("row:{table}:").into_bytes()
}
fn del_key(table: &str, seq: usize) -> Vec<u8> {
    format!("del:{table}:{seq:016}").into_bytes()
}
fn del_prefix(table: &str) -> Vec<u8> {
    format!("del:{table}:").into_bytes()
}
fn snap_key(name: &str) -> Vec<u8> {
    format!("snap:{name}").into_bytes()
}
/// `rank_features` contribution: `cell` is a JSON array of
/// `{"<tag>": <number>}` objects, and each listed tag adds `number * weight`.
/// A missing tag contributes nothing.
fn rank_features_bonus(cell: &str, tags: &[(String, f64)]) -> f64 {
    let mut sum = 0.0;
    for (tag, weight) in tags {
        let key = format!("\"{tag}\"");
        let Some(pos) = cell.find(&key) else {
            continue;
        };
        let rest = cell[pos + key.len()..].trim_start();
        let Some(rest) = rest.strip_prefix(':') else {
            continue;
        };
        let num: String = rest
            .trim_start()
            .chars()
            .take_while(|c| c.is_ascii_digit() || "+-.eE".contains(*c))
            .collect();
        if let Ok(v) = num.parse::<f64>() {
            sum += v * weight;
        }
    }
    sum
}

/// Overwrites a search-result row's hidden `#_score` column (the last one).
fn set_score(row: &mut [Value], score: f64) {
    if let Some(last) = row.last_mut() {
        *last = Value::Float(score);
    }
}
/// Reads a search-result row's hidden `#_score` column (the last one).
fn row_score(row: &[Value]) -> f64 {
    match row.last() {
        Some(Value::Float(f)) => *f,
        Some(Value::Int(i)) => *i as f64,
        _ => 0.0,
    }
}
/// Whether a fusion child's scores are similarities (higher is better, so no
/// sign flip) rather than distances. Mirrors `PhysicalFusion`'s `min_heaps`:
/// `MATCH`/`MATCH TENSOR`/`MATCH SPARSE` are similarities, and an L2/Hamming
/// vector scan is a distance (`KnnExpression::IsKnnMinHeap`).
fn match_is_min_heap(m: &MatchClause) -> bool {
    if m.kind.eq_ignore_ascii_case("VECTOR") {
        let metric = arg_str(m.args.get(2)).unwrap_or_default();
        !matches!(
            metric.trim().to_ascii_lowercase().as_str(),
            "l2" | "hamming"
        )
    } else {
        true
    }
}
/// Record / field separators used inside a snapshot blob.
const SNAP_REC: char = '\u{1d}';
const SNAP_FLD: char = '\u{1c}';
/// Separates the per-table blobs of a database snapshot.
const DB_SNAP_SEP: char = '\u{1b}';

/// The declared dimension of a `TENSOR(FLOAT,d)` / `EMBEDDING(FLOAT,d)` column.
fn type_dim(type_text: &str) -> Option<usize> {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1)?.trim_end_matches(')');
    inner.split(',').rev().find_map(|s| s.trim().parse().ok())
}

/// The declared dimension of a (possibly nested) parameterised type, e.g.
/// `ARRAY(EMBEDDING(FLOAT,16385))` -> 16385.
fn type_dim_deep(type_text: &str) -> Option<usize> {
    let t = type_text.to_ascii_uppercase();
    let after = t.rsplit_once('(').map(|(_, r)| r).unwrap_or(&t);
    after
        .split(',')
        .rev()
        .find_map(|s| s.trim().trim_end_matches(')').trim().parse().ok())
}

fn is_tensor_type(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    t.starts_with("TENSOR") || t.starts_with("TENSORARRAY") || t.starts_with("MULTIVECTOR")
}

/// The numeric element type of a `TENSOR(...)` / `TENSORARRAY(...)` column or of
/// a `MATCH TENSOR` query. The C++ engine casts both the stored and query tensor
/// values into a common aligned `f32` buffer before the MaxSim dot products, so
/// the element type only matters for the precision each value is rounded to.
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
enum ElemType {
    Float32,
    Float64,
    Float16,
    BFloat16,
    Bit,
    Int8,
    UInt8,
    Int16,
    Int32,
    Int64,
}

fn elem_type_of(token: &str) -> ElemType {
    // The parser concatenates type tokens without spaces (`UNSIGNEDTINYINT`),
    // so compare against a whitespace-free uppercase form.
    let t: String = token
        .chars()
        .filter(|c| !c.is_whitespace())
        .collect::<String>()
        .to_ascii_uppercase();
    match t.as_str() {
        "FLOAT16" | "FP16" | "HALF" => ElemType::Float16,
        "BFLOAT16" | "BF16" => ElemType::BFloat16,
        "DOUBLE" | "FLOAT64" => ElemType::Float64,
        "BIT" => ElemType::Bit,
        "INT8" | "TINYINT" => ElemType::Int8,
        "UINT8" | "UNSIGNEDTINYINT" | "UTINYINT" | "UNSIGNEDINT8" => ElemType::UInt8,
        "INT16" | "SMALLINT" => ElemType::Int16,
        "INT32" | "INT" | "INTEGER" => ElemType::Int32,
        "INT64" | "BIGINT" | "LONG" => ElemType::Int64,
        _ => ElemType::Float32,
    }
}

/// The query element type named by a `MATCH …` argument, if it names one.
/// Like [`elem_type_of`] but rejects an unknown type name.
fn elem_type_named(token: &str) -> Option<ElemType> {
    let t: String = token
        .chars()
        .filter(|c| !c.is_whitespace())
        .collect::<String>()
        .to_ascii_uppercase();
    match t.as_str() {
        "FLOAT" | "FLOAT32" | "FLOAT16" | "FP16" | "HALF" | "BFLOAT16" | "BF16" | "DOUBLE"
        | "FLOAT64" | "BIT" | "INT8" | "TINYINT" | "UINT8" | "UNSIGNEDTINYINT" | "UTINYINT"
        | "UNSIGNEDINT8" | "INT16" | "SMALLINT" | "INT32" | "INT" | "INTEGER" | "INT64"
        | "BIGINT" | "LONG" => Some(elem_type_of(token)),
        _ => None,
    }
}

fn elem_type_of_optional(arg: Option<&ExprAst>) -> Option<ElemType> {
    arg_str(arg).map(|s| elem_type_of(&s))
}

/// The element type declared inside a `TENSOR(<elem>, <dim>)` type text.
fn tensor_elem_type(type_text: &str) -> ElemType {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("");
    elem_type_of(inner.split(',').next().unwrap_or(""))
}

/// Rounds an `f64` to the element type's precision, widened back to `f32` — the
/// conversion the C++ engine performs when filling its `f32` arithmetic buffer.
fn quantize_elem(x: f64, e: ElemType) -> f32 {
    let v = x as f32;
    match e {
        ElemType::Float16 => round_f16(v),
        ElemType::BFloat16 => round_bf16(v),
        _ => v,
    }
}

/// Round-to-nearest-even conversion of an `f32` to the nearest `f16` value,
/// returned as `f32`.
fn round_f16(x: f32) -> f32 {
    f16_bits_to_f32(f32_to_f16_bits(x))
}

fn f32_to_f16_bits(x: f32) -> u16 {
    let b = x.to_bits();
    let sign = ((b >> 16) & 0x8000) as u16;
    let exp = ((b >> 23) & 0xff) as i32;
    let mant = b & 0x007f_ffff;
    if exp == 0xff {
        let m = if mant != 0 { 0x200 } else { 0 };
        return sign | 0x7c00 | m;
    }
    let new_exp = exp - 127 + 15;
    if new_exp >= 31 {
        return sign | 0x7c00;
    }
    if new_exp <= 0 {
        if new_exp < -10 {
            return sign;
        }
        let m = mant | 0x0080_0000;
        let shift = (14 - new_exp) as u32;
        let half = 1u32 << (shift - 1);
        let mut q = m >> shift;
        let rem = m & ((1u32 << shift) - 1);
        if rem > half || (rem == half && (q & 1) == 1) {
            q += 1;
        }
        return sign | q as u16;
    }
    let mut m = mant >> 13;
    let rem = mant & 0x1fff;
    let mut e = new_exp;
    if rem > 0x1000 || (rem == 0x1000 && (m & 1) == 1) {
        m += 1;
        if m == 0x400 {
            m = 0;
            e += 1;
            if e >= 31 {
                return sign | 0x7c00;
            }
        }
    }
    sign | ((e as u16) << 10) | (m as u16)
}

fn f16_bits_to_f32(h: u16) -> f32 {
    let sign = ((h & 0x8000) as u32) << 16;
    let exp = ((h >> 10) & 0x1f) as u32;
    let mant = (h & 0x3ff) as u32;
    if exp == 0 {
        if mant == 0 {
            return f32::from_bits(sign);
        }
        let mut e: i32 = -14;
        let mut m = mant;
        while m & 0x400 == 0 {
            m <<= 1;
            e -= 1;
        }
        m &= 0x3ff;
        return f32::from_bits(sign | (((e + 127) as u32) << 23) | (m << 13));
    }
    if exp == 0x1f {
        return f32::from_bits(sign | 0x7f80_0000 | (mant << 13));
    }
    f32::from_bits(sign | ((exp + 127 - 15) << 23) | (mant << 13))
}

/// Conversion of an `f32` to `bfloat16` by dropping the low 16 mantissa bits.
/// The C++ `bfloat16_t` conversion used by the tensor scan truncates rather than
/// rounding to nearest, so this deliberately truncates to match its scores.
fn round_bf16(x: f32) -> f32 {
    f32::from_bits(x.to_bits() & 0xffff_0000)
}

fn is_float_type(type_text: &str) -> bool {
    matches!(
        type_text.to_ascii_uppercase().as_str(),
        "FLOAT" | "DOUBLE" | "REAL" | "FP16" | "BF16" | "FLOAT16" | "BFLOAT16"
    )
}

fn is_array_type(type_text: &str) -> bool {
    type_text.to_ascii_uppercase().starts_with("ARRAY")
}

/// Whether an `ARRAY(<elem>)` type's element type is a float type.
fn array_elem_is_float(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("").trim_end_matches(')');
    is_float_type(inner.trim())
}

/// Renders a numeric token with six decimals, falling back to the raw token.
fn fmt_float6(token: &str) -> String {
    match token.trim().parse::<f64>() {
        Ok(f) => format!("{f:.6}"),
        Err(_) => token.to_string(),
    }
}

/// Bracket depth at which each numeric leaf of a literal appears.
fn array_leaf_depths(text: &str) -> Vec<usize> {
    let mut depths = Vec::new();
    let mut depth = 0usize;
    let mut chars = text.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '[' => depth += 1,
            ']' => depth = depth.saturating_sub(1),
            c if c.is_ascii_digit()
                || ((c == '-' || c == '.')
                    && matches!(chars.peek(), Some(x) if x.is_ascii_digit())) =>
            {
                while matches!(chars.peek(), Some(x) if x.is_ascii_digit() || *x == '.') {
                    chars.next();
                }
                depths.push(depth);
            }
            _ => {}
        }
    }
    depths
}

/// The elements of a brace array literal `{a,b,c}` (as `.slt` text), in order,
/// or `None` when the value is not brace-delimited.
fn brace_array_elems(cell: &str) -> Option<Vec<String>> {
    let t = cell.trim();
    let inner = t.strip_prefix('{')?.strip_suffix('}')?;
    if inner.trim().is_empty() {
        return Some(Vec::new());
    }
    Some(
        split_top_level(inner)
            .iter()
            .map(|s| s.trim().to_string())
            .collect(),
    )
}

/// The innermost numeric groups of an array literal, in order, each as its list
/// of element tokens. A flat literal yields a single group.
fn innermost_groups(text: &str) -> Vec<Vec<String>> {
    let t = text.trim();
    let inner = t.strip_prefix('[').unwrap_or(t);
    let inner = inner.strip_suffix(']').unwrap_or(inner);
    if inner.trim().is_empty() {
        return Vec::new();
    }
    if inner.trim_start().starts_with('[') {
        split_top_level(inner)
            .iter()
            .flat_map(|g| innermost_groups(g))
            .collect()
    } else {
        let items: Vec<String> = inner
            .split(',')
            .map(|s| s.trim().to_string())
            .filter(|s| !s.is_empty())
            .collect();
        vec![items]
    }
}

/// Validates a tensor-like literal against its declared dimension: every
/// embedding must have exactly `dim` elements, and a flat literal must be a
/// whole multiple of `dim`. Non tensor-like columns always pass.
fn validate_tensor_dimension(type_text: &str, lit: &str) -> Result<()> {
    let up = type_text.to_ascii_uppercase();
    let embedding = up.starts_with("EMBEDDING") || up.starts_with("VECTOR");
    if !is_tensor_type(&up) && !embedding {
        return Ok(());
    }
    // Only array literals carry a dimension; NULL/default padding is left alone.
    if !lit.trim_start().starts_with('[') {
        return Ok(());
    }
    if up.starts_with("TENSORARRAY") {
        let depths = array_leaf_depths(lit);
        if !depths.is_empty() && depths.iter().any(|d| *d != 3) {
            return Err(Error::Invalid(
                "TENSORARRAY value must be nested as [[[…]]]".into(),
            ));
        }
    }
    let dim = match type_dim(type_text) {
        Some(d) if d > 0 => d,
        _ => return Ok(()),
    };
    let groups = innermost_groups(lit);
    if groups.len() == 1 {
        let n = groups[0].len();
        // An embedding is exactly `dim` wide; a tensor may hold k*dim elements.
        if n != 0 && embedding && n != dim {
            return Err(Error::Invalid(format!(
                "embedding dimension mismatch: expected {dim}, got {n}"
            )));
        }
        if n != 0 && !embedding && !n.is_multiple_of(dim) {
            return Err(Error::Invalid(format!(
                "tensor dimension mismatch: expected a multiple of {dim}, got {n}"
            )));
        }
    } else {
        for g in &groups {
            if g.len() != dim {
                return Err(Error::Invalid(format!(
                    "tensor dimension mismatch: expected {dim}, got {}",
                    g.len()
                )));
            }
        }
    }
    Ok(())
}

/// ARRAY columns render with braces: `[1,2]` → `{1,2}`, `[]` → `{}`.
/// Whether the element type declared inside a `TENSOR`/`EMBEDDING`-style type
/// text is an integer type (the first comma-separated parameter).
fn declared_elem_is_int(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("");
    array_elem_is_int(&format!("ARRAY({}", inner.split(',').next().unwrap_or("")))
}

/// Whether an `ARRAY(<elem>)` type has an integer element type.
fn array_elem_is_int(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("").trim_end_matches(')');
    matches!(
        inner.trim(),
        "INT"
            | "INTEGER"
            | "TINYINT"
            | "SMALLINT"
            | "BIGINT"
            | "INT8"
            | "INT16"
            | "INT32"
            | "INT64"
    )
}

/// Truncates every numeric token in `s` to an integer.
fn truncate_numbers(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    let mut num = String::new();
    for c in s.chars() {
        if c.is_ascii_digit() || c == '.' || c == '-' {
            num.push(c);
            continue;
        }
        if !num.is_empty() {
            match num.parse::<f64>() {
                Ok(f) => out.push_str(&(f.round() as i64).to_string()),
                Err(_) => out.push_str(&num),
            }
            num.clear();
        }
        out.push(c);
    }
    if !num.is_empty() {
        match num.parse::<f64>() {
            Ok(f) => out.push_str(&(f.round() as i64).to_string()),
            Err(_) => out.push_str(&num),
        }
    }
    out
}

fn brace_render(s: &str) -> String {
    let t = s.trim();
    if !t.starts_with('[') || !t.ends_with(']') {
        return t.to_string();
    }
    t.chars()
        .map(|c| match c {
            '[' => '{',
            ']' => '}',
            other => other,
        })
        .collect()
}

fn is_json_type(type_text: &str) -> bool {
    type_text.eq_ignore_ascii_case("JSON")
}

/// Whether a column type renders as a canonical date/time string.
fn is_datetime_type(type_text: &str) -> bool {
    let u = type_text.to_ascii_uppercase();
    matches!(u.as_str(), "DATE" | "TIME" | "DATETIME" | "TIMESTAMP")
}

/// Renders a `Value` as a JSON field value (strings quoted, arrays/objects raw).
/// Renders a cell as JSON. A sparse column (`[i:v,…]`) becomes a JSON object so
/// the line stays valid JSON and round-trips through `COPY … FROM … JSONL`.
fn json_value_typed(v: &Value, type_text: &str) -> String {
    if is_sparse_type(type_text) {
        if let Value::Str(s) = v {
            let t = s.trim();
            if t.starts_with('[') {
                let pairs: Vec<String> = t
                    .trim_start_matches('[')
                    .trim_end_matches(']')
                    .split(',')
                    .filter(|x| !x.trim().is_empty())
                    .map(|kv| match kv.split_once(':') {
                        Some((i, val)) => format!("\"{}\" : {}", i.trim(), val.trim()),
                        None => format!("\"{}\" : 1", kv.trim()),
                    })
                    .collect();
                return format!("{{{}}}", pairs.join(","));
            }
        }
    }
    match v {
        Value::Str(s) => {
            let t = s.trim();
            if t.starts_with('[') || t.starts_with('{') {
                t.to_string()
            } else {
                let escaped = s.replace('\\', "\\\\").replace('"', "\\\"");
                format!("\"{escaped}\"")
            }
        }
        Value::Null => "null".to_string(),
        other => other.to_slt_string(),
    }
}

/// Maps an export path to a writable location that a later `COPY … FROM`
/// resolves: paths under the fixture root are redirected beneath
/// `INFINITY_DATA_DIR`, with parent directories created.
fn resolve_write_path(path: &str) -> std::path::PathBuf {
    let p = std::path::PathBuf::from(path);
    if let Ok(base) = std::env::var("INFINITY_DATA_DIR") {
        if let Some(rest) = path.strip_prefix("/var/infinity/test_data/") {
            let joined = std::path::PathBuf::from(&base).join(rest);
            if let Some(parent) = joined.parent() {
                let _ = std::fs::create_dir_all(parent);
            }
            return joined;
        }
    }
    if let Some(parent) = p.parent() {
        let _ = std::fs::create_dir_all(parent);
    }
    p
}

/// Renders a JSON scalar (or nested value) as a stored cell string.
fn json_scalar_text(v: &infinity_expression::json::Json) -> String {
    use infinity_expression::json::Json;
    match v {
        Json::Str(s) => s.clone(),
        Json::Int(i) => i.to_string(),
        Json::Float(f) => fmt_num(*f),
        Json::Bool(b) => b.to_string(),
        other => other.compact(),
    }
}

fn is_sparse_type(type_text: &str) -> bool {
    type_text.to_ascii_uppercase().starts_with("SPARSE")
}

/// Canonical sparse rendering: `(empty)` when there is nothing, otherwise
/// `[i:v,…]` sorted by index. Returns `None` for an invalid vector (negative or
/// duplicated indices).
fn reshape_sparse(s: &str, float: bool) -> Option<String> {
    let t = s.trim();
    if t.is_empty() || t == "[]" {
        return Some("(empty)".to_string());
    }
    // A `bit` sparse vector (and any index-only literal) carries just the
    // non-zero positions, e.g. `[10000,20000]`, with no `:` values.
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    if !inner.contains(':') {
        let mut idx: Vec<usize> = Vec::new();
        for it in inner.split(',') {
            let it = it.trim();
            if it.is_empty() {
                continue;
            }
            idx.push(it.parse::<usize>().ok()?);
        }
        if idx.is_empty() {
            return None;
        }
        idx.sort_unstable();
        if idx.windows(2).any(|w| w[0] == w[1]) {
            return None; // duplicate index
        }
        let body: Vec<String> = idx.iter().map(|i| i.to_string()).collect();
        return Some(format!("[{}]", body.join(",")));
    }
    let pairs = parse_sparse_str(t);
    if pairs.is_empty() {
        // a non-empty literal that parses to nothing (e.g. a negative index)
        // is invalid
        return None;
    }
    let mut sorted = pairs.clone();
    sorted.sort_by_key(|(i, _)| *i);
    if sorted.windows(2).any(|w| w[0].0 == w[1].0) {
        return None; // duplicate index
    }
    let body: Vec<String> = sorted
        .iter()
        .map(|(i, v)| {
            if float {
                format!("{i}:{v:.6}")
            } else {
                format!("{i}:{}", fmt_num(*v))
            }
        })
        .collect();
    Some(format!("[{}]", body.join(",")))
}

/// Whether the type text declares a vector-like dimension
/// (`EMBEDDING`/`VECTOR`/`TENSOR`/`TENSORARRAY`/`MULTIVECTOR`/`SPARSE`).
fn declares_dimension(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    [
        "EMBEDDING",
        "VECTOR",
        "TENSOR",
        "TENSORARRAY",
        "MULTIVECTOR",
        "SPARSE",
    ]
    .iter()
    .any(|k| t.starts_with(k) || (t.starts_with("ARRAY(") && t.contains(k)))
}

fn is_bit_type(type_text: &str) -> bool {
    type_text.to_ascii_uppercase().contains("BIT")
}

/// Whether the type text is a fixed-width embedding column
/// (`EMBEDDING(...)` / `VECTOR(...)`).
fn is_embedding_type(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    t.starts_with("EMBEDDING") || t.starts_with("VECTOR")
}

/// Packs a flat bit embedding into a single bit string, e.g. `[0000000000000001]`.
fn pack_bit_vector(s: &str) -> String {
    let t = s.trim();
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    if inner.trim().is_empty() {
        return "[]".to_string();
    }
    let bits: String = inner
        .split([',', ' '])
        .filter(|x| !x.trim().is_empty())
        .map(|x| {
            if x.trim().parse::<f64>().map(|f| f != 0.0).unwrap_or(false) {
                '1'
            } else {
                '0'
            }
        })
        .collect();
    format!("[{bits}]")
}

/// Packs a bit tensor into rows of `dim` `0`/`1` characters.
fn reshape_bit_tensor(s: &str, dim: usize) -> String {
    let t = s.trim();
    if !t.starts_with('[') {
        return t.to_string();
    }
    // A flat input is first chunked into `dim`-sized rows; an already-nested
    // input keeps its structure.
    let text = if t[1..].starts_with('[') {
        t.to_string()
    } else {
        let flat = parse_vec_str(t);
        if flat.is_empty() {
            return "(empty)".to_string();
        }
        let d = if dim == 0 { flat.len() } else { dim };
        let groups: Vec<String> = flat
            .chunks(d)
            .map(|c| {
                format!(
                    "[{}]",
                    c.iter().map(|v| fmt_num(*v)).collect::<Vec<_>>().join(",")
                )
            })
            .collect();
        format!("[{}]", groups.join(","))
    };
    pack_bit_leaves(&text)
}

/// Replaces every numeric leaf list with a bracketed bit string, keeping nesting.
fn pack_bit_leaves(text: &str) -> String {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return "[]".to_string();
    }
    if inner.trim_start().starts_with('[') {
        let groups: Vec<String> = split_top_level(inner)
            .iter()
            .map(|g| pack_bit_leaves(g))
            .collect();
        format!("[{}]", groups.join(","))
    } else {
        let bits: String = inner
            .split([',', ' '])
            .filter(|x| !x.trim().is_empty())
            .map(|x| {
                if x.trim().parse::<f64>().map(|f| f != 0.0).unwrap_or(false) {
                    '1'
                } else {
                    '0'
                }
            })
            .collect();
        format!("[{bits}]")
    }
}

/// Reshapes a flat tensor literal (`[a,b,c,d,e,f,g,h]`) into `[[a,b,c,d],[e,f,g,h]]`.
fn reshape_tensor(s: &str, dim: usize) -> String {
    let t = s.trim();
    if dim == 0 || !t.starts_with('[') {
        return t.to_string();
    }
    if t[1..].starts_with('[') {
        return t.to_string(); // already nested
    }
    let flat: Vec<f64> = parse_vec_str(t);
    if flat.is_empty() || !flat.len().is_multiple_of(dim) {
        return t.to_string();
    }
    let groups: Vec<String> = flat
        .chunks(dim)
        .map(|c| {
            format!(
                "[{}]",
                c.iter().map(|f| fmt_num(*f)).collect::<Vec<_>>().join(",")
            )
        })
        .collect();
    format!("[{}]", groups.join(","))
}

/// Recognises a bare `filter_text(…)`/`filter_fulltext(…)` predicate that needs
/// the full-text index to answer, returning what it asked for.
fn fulltext_index_call(p: &Predicate) -> Option<(String, String, String)> {
    let Predicate::IsTrue(ExprAst::Func { name, args }) = p else {
        return None;
    };
    let up = name.to_ascii_uppercase();
    if up != "FILTER_TEXT" && up != "FILTER_FULLTEXT" {
        return None;
    }
    let s = |i: usize| match args.get(i) {
        Some(ExprAst::Literal(Literal::Str(v))) => Some(v.clone()),
        _ => None,
    };
    let column = s(0)?;
    let (col, hint) = match column.split_once('@') {
        Some((c, i)) => (c.to_string(), Some(i.to_string())),
        None => (column, None),
    };
    Some((
        col,
        s(1)?,
        format!("{}\u{1}{}", s(2)?, hint.unwrap_or_default()),
    ))
}

/// Normalises a column default to its stored form: an integer-element
/// array/embedding default keeps integer values (`[1.3,4.1,33.7]` → `[1,4,34]`).
fn normalize_default(type_text: &str, raw: &str) -> String {
    if (is_array_type(type_text) || is_embedding_type(type_text))
        && (array_elem_is_int(type_text) || declared_elem_is_int(type_text))
    {
        let types = vec![type_text.to_string()];
        encode_row(&[Literal::Str(raw.to_string())], &types)
    } else {
        raw.to_string()
    }
}

/// Encodes a row for storage, reshaping tensor columns by their declared dim.
fn encode_row(row: &[Literal], types: &[String]) -> String {
    row.iter()
        .enumerate()
        .map(|(i, v)| {
            let s = v.to_slt_string();
            match types.get(i) {
                // float columns render with six decimals in `.slt` output.
                // The C++ numeric cast tolerates surrounding whitespace, so
                // `" 2.1"` is a valid double literal.
                Some(t) if is_float_type(t) => match s.trim().parse::<f64>() {
                    Ok(f) => format!("{f:.6}"),
                    Err(_) => s,
                },
                Some(t) if is_array_type(t) => {
                    let rendered = brace_render(&s);
                    if array_elem_is_int(t) {
                        truncate_numbers(&rendered)
                    } else {
                        rendered
                    }
                }
                // JSON columns are stored compactly (sorted keys, no spaces)
                Some(t) if is_json_type(t) => match infinity_expression::json::parse(&s) {
                    Some(j) => j.compact(),
                    None => s,
                },
                Some(t) if is_datetime_type(t) => {
                    infinity_parser::normalize_datetime(&t.to_ascii_uppercase(), &s)
                }
                Some(t) if is_sparse_type(t) => {
                    let float = t.to_ascii_uppercase().contains("FLOAT")
                        || t.to_ascii_uppercase().contains("DOUBLE");
                    match reshape_sparse(&s, float) {
                        Some(v) => v,
                        None => s,
                    }
                }
                Some(t) if is_embedding_type(t) => {
                    if is_bit_type(t) {
                        pack_bit_vector(&s)
                    } else if declared_elem_is_int(t) {
                        // an integer element type truncates the stored values
                        truncate_numbers(&s)
                    } else {
                        s
                    }
                }
                Some(t) if is_tensor_type(t) => {
                    let d = type_dim(t).unwrap_or(0);
                    if is_bit_type(t) {
                        // `pack_bit_leaves` preserves the literal's nesting, so a
                        // `TENSORARRAY` (whose literal is already triple-nested)
                        // needs no extra wrapper.
                        reshape_bit_tensor(&s, d)
                    } else {
                        reshape_tensor(&s, d)
                    }
                }
                _ => s,
            }
        })
        .collect::<Vec<_>>()
        .join("\t")
}

/// Human-readable column type, e.g. `Integer` or `Sparse(float,int16,30000)`.
fn describe_type(type_text: &str) -> String {
    let t = type_text.trim();
    if t.is_empty() {
        return "Invalid".to_string();
    }
    let up = t.to_ascii_uppercase();
    if up.starts_with("ARRAY(") {
        let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
        return format!("Array({})", describe_type(inner));
    }
    if up.starts_with("SPARSE(") {
        let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
        let raw = parts
            .first()
            .map(|s| s.to_ascii_lowercase())
            .unwrap_or_default();
        // Integer element names are printed with their width suffix.
        let elem = match raw.as_str() {
            "int" | "integer" => "int32",
            "tinyint" => "int8",
            "smallint" => "int16",
            "bigint" => "int64",
            _ => raw.as_str(),
        };
        let bound: u64 = parts.get(1).and_then(|s| s.parse().ok()).unwrap_or(0);
        let idx = if bound <= 127 {
            "int8"
        } else if bound <= 32767 {
            "int16"
        } else if bound <= 2_147_483_647 {
            "int32"
        } else {
            "int64"
        };
        return format!("Sparse({elem},{idx},{bound})");
    }
    if up.starts_with("EMBEDDING(") {
        let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
        let elem = parts
            .first()
            .map(|s| s.to_ascii_lowercase())
            .unwrap_or_default();
        let dim = parts.get(1).map(|s| s.trim()).unwrap_or("");
        return format!("Embedding({elem},{dim})");
    }
    // Parameterised vector-ish types print their element type and dimension, so
    // callers can recover both from `SHOW TABLE … COLUMNS`.
    for (kw, name) in [
        ("TENSORARRAY(", "TensorArray"),
        ("MULTIVECTOR(", "MultiVector"),
        ("TENSOR(", "Tensor"),
    ] {
        if up.starts_with(kw) {
            let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
            let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
            let elem = parts
                .first()
                .map(|s| s.to_ascii_lowercase())
                .unwrap_or_default();
            let dim = parts.get(1).map(|s| s.trim()).unwrap_or("");
            return format!("{name}({elem},{dim})");
        }
    }
    let base = up.split('(').next().unwrap_or(&up);
    match base {
        "BOOL" | "BOOLEAN" => "Boolean".to_string(),
        "TINYINT" => "TinyInt".to_string(),
        "SMALLINT" => "SmallInt".to_string(),
        "INT" | "INTEGER" => "Integer".to_string(),
        "BIGINT" => "BigInt".to_string(),
        "FLOAT" => "Float".to_string(),
        "DOUBLE" => "Double".to_string(),
        "VARCHAR" | "TEXT" | "STRING" => "Varchar".to_string(),
        _ => {
            let mut c = base.to_ascii_lowercase();
            if let Some(f) = c.get_mut(0..1) {
                f.make_ascii_uppercase();
            }
            c
        }
    }
}

/// Renders a column default for `SHOW TABLE … COLUMNS`, mirroring the C++
/// `default_expr_->ToString()`. An absent default prints as `Null`; a sparse
/// default reproduces the C++ renderer, which (for float sparses) prints the
/// value before the index.
fn render_column_default(def: Option<&String>, type_text: &str) -> String {
    let Some(d) = def else {
        return "Null".to_string();
    };
    if !is_sparse_type(type_text) {
        return d.clone();
    }
    let t = d.trim();
    if t.is_empty() || t == "[]" {
        return "(empty)".to_string();
    }
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    if inner.contains(':') {
        let pairs = parse_sparse_str(t);
        let body: Vec<String> = pairs.iter().map(|(i, v)| format!("{v:.6}:{i}")).collect();
        format!("[{}]", body.join(","))
    } else {
        let idx: Vec<String> = inner
            .split(',')
            .map(|s| s.trim().to_string())
            .filter(|s| !s.is_empty())
            .collect();
        format!("[{}]", idx.join(","))
    }
}

/// Rejects an out-of-range or malformed value for a **known** config key
/// (unknown keys are tolerated). Mirrors the C++ `Config` range checks
/// (`src/unit_test/main/config_ut.cpp::TestOutofRangeValues`): config values are
/// validated, never silently clamped.
fn invalid_config_value(key: &str, value: &str) -> Option<String> {
    match key.to_ascii_uppercase().as_str() {
        "RESULT_CACHE" => {
            let ok = matches!(
                value.to_ascii_lowercase().as_str(),
                "on" | "off" | "clear" | "suspend"
            );
            (!ok).then(|| {
                format!("invalid value '{value}' for result_cache (expected on|off|clear|suspend)")
            })
        }
        "CACHE_RESULT_CAPACITY" => match value.parse::<u64>() {
            Ok(n) if n >= 1 => None,
            _ => Some(format!(
                "invalid value '{value}' for cache_result_capacity (expected a positive integer)"
            )),
        },
        "CHECKPOINT_INTERVAL" => match value.parse::<u64>() {
            Ok(_) => None,
            Err(_) => Some(format!(
                "invalid value '{value}' for CHECKPOINT_INTERVAL (expected a non-negative integer)"
            )),
        },
        _ => None,
    }
}

/// True when a result is small enough to keep in the result cache. Caching a
/// whole-table `SELECT *` retains (and clones on each hit) hundreds of MB.
fn cacheable_result(res: &QueryResult) -> bool {
    matches!(res, QueryResult::Rows { rows, .. } if rows.len() <= 4096)
}

/// True when a statement can change data or metadata and therefore invalidates
/// cached `SELECT` results.
fn invalidates_result_cache(stmt: &Statement) -> bool {
    matches!(
        stmt,
        Statement::Insert { .. }
            | Statement::Delete { .. }
            | Statement::Update { .. }
            | Statement::CreateTable { .. }
            | Statement::DropTable { .. }
            | Statement::AlterTableAdd { .. }
            | Statement::AlterTableDropColumn { .. }
            | Statement::RenameTable { .. }
            | Statement::CreateIndex { .. }
            | Statement::DropIndex { .. }
            | Statement::CreateDatabase { .. }
            | Statement::DropDatabase { .. }
            | Statement::CreateSnapshot { .. }
            | Statement::DropSnapshot { .. }
            | Statement::RestoreSnapshot { .. }
            | Statement::Copy { from: true, .. }
    )
}

/// Evaluates a Lucene `minimum_should_match` spec against the number of
/// optional clauses, returning how many terms a document must match.
///
/// The spec is a whitespace-separated list of clauses evaluated in order; the
/// last one that applies wins. Each clause is `[n<]value` where `value` is a
/// non-negative count, a negative count (`optional - n`), or a percentage of
/// the optional count (negative percentages subtract from it).
fn minimum_should_match(spec: &str, optional: usize) -> usize {
    if optional == 0 {
        return 0;
    }
    let clause_value = |value: &str| -> Option<usize> {
        let (num, pct) = match value.strip_suffix('%') {
            Some(n) => (n, true),
            None => (value, false),
        };
        let n: i64 = num.trim().parse().ok()?;
        if pct {
            let scaled = (optional as f64 * n.unsigned_abs() as f64 / 100.0).floor() as i64;
            Some(if n < 0 {
                (optional as i64 - scaled).max(0) as usize
            } else {
                scaled.max(0) as usize
            })
        } else {
            Some(if n < 0 {
                (optional as i64 + n).max(0) as usize
            } else {
                n as usize
            })
        }
    };
    let mut required = 1usize.min(optional);
    for clause in spec.split_whitespace() {
        match clause.split_once('<') {
            Some((cond, rest)) => {
                let cond: i64 = match cond.trim().parse() {
                    Ok(v) => v,
                    Err(_) => continue,
                };
                if optional as i64 > cond {
                    match clause_value(rest.trim()) {
                        Some(v) => required = v.min(optional),
                        None => continue,
                    }
                } else {
                    required = optional;
                }
            }
            None => match clause_value(clause) {
                Some(v) => required = v.min(optional),
                None => continue,
            },
        }
    }
    required
}

/// True when the select list references the `ROW_ID()` pseudo-column.
fn uses_row_id(items: &[SelectItem]) -> bool {
    fn in_expr(e: &ExprAst) -> bool {
        match e {
            ExprAst::Func { name, args } => {
                name.eq_ignore_ascii_case("ROW_ID") || args.iter().any(in_expr)
            }
            ExprAst::Column(c) => c.eq_ignore_ascii_case("_row_id"),
            ExprAst::Binary(a, _, b)
            | ExprAst::Cmp(a, _, b)
            | ExprAst::And(a, b)
            | ExprAst::Or(a, b) => in_expr(a) || in_expr(b),
            ExprAst::Not(a) => in_expr(a),
            _ => false,
        }
    }
    items.iter().any(|it| match it {
        SelectItem::Func { name, .. } => name.eq_ignore_ascii_case("ROW_ID"),
        SelectItem::Column(name) => name.eq_ignore_ascii_case("_row_id"),
        SelectItem::Expr(e) => in_expr(e),
        _ => false,
    })
}

thread_local! {
    /// Set while a table is created from the SDK's physical type, where a
    /// sparse dimension of 0 is legal.
    static ALLOW_ZERO_SPARSE: std::cell::Cell<bool> = const { std::cell::Cell::new(false) };
}

/// Rewrites `UNNEST(col)` to the name of the column the expansion appends.
///
/// The SDKs turn `filter("uc2 > 1")` into `WHERE unnest(c2) > 1` (the alias is
/// replaced by the output expression), but the filter runs *after* the
/// expansion, where the values live in a synthetic column.
fn rewrite_unnest_expr(e: &ExprAst, col: &str, alias: &str) -> ExprAst {
    match e {
        ExprAst::Func { name, args }
            if name.eq_ignore_ascii_case("UNNEST")
                && args.len() == 1
                && matches!(&args[0], ExprAst::Column(c) if c.eq_ignore_ascii_case(col)) =>
        {
            ExprAst::Column(alias.to_string())
        }
        ExprAst::Func { name, args } => ExprAst::Func {
            name: name.clone(),
            args: args
                .iter()
                .map(|a| rewrite_unnest_expr(a, col, alias))
                .collect(),
        },
        ExprAst::Binary(a, op, b) => ExprAst::Binary(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            *op,
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        ExprAst::Cmp(a, op, b) => ExprAst::Cmp(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            *op,
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        ExprAst::Not(a) => ExprAst::Not(Box::new(rewrite_unnest_expr(a, col, alias))),
        ExprAst::And(a, b) => ExprAst::And(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        ExprAst::Or(a, b) => ExprAst::Or(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        other => other.clone(),
    }
}

fn rewrite_unnest_pred(p: &Predicate, col: &str, alias: &str) -> Predicate {
    match p {
        Predicate::Cmp(a, op, b) => Predicate::Cmp(
            rewrite_unnest_expr(a, col, alias),
            *op,
            rewrite_unnest_expr(b, col, alias),
        ),
        Predicate::IsTrue(e) => Predicate::IsTrue(rewrite_unnest_expr(e, col, alias)),
        Predicate::IsNull(e, not) => Predicate::IsNull(rewrite_unnest_expr(e, col, alias), *not),
        Predicate::Like(a, b, esc, not) => Predicate::Like(
            rewrite_unnest_expr(a, col, alias),
            rewrite_unnest_expr(b, col, alias),
            *esc,
            *not,
        ),
        Predicate::In(e, list, not) => Predicate::In(
            rewrite_unnest_expr(e, col, alias),
            list.iter()
                .map(|x| rewrite_unnest_expr(x, col, alias))
                .collect(),
            *not,
        ),
        Predicate::Bool(a, op, b) => Predicate::Bool(
            Box::new(rewrite_unnest_pred(a, col, alias)),
            *op,
            Box::new(rewrite_unnest_pred(b, col, alias)),
        ),
        Predicate::Not(a) => Predicate::Not(Box::new(rewrite_unnest_pred(a, col, alias))),
    }
}
/// Runs `f` allowing a zero-dimension sparse column.
pub fn with_zero_dimension_sparse<T>(f: impl FnOnce() -> T) -> T {
    ALLOW_ZERO_SPARSE.with(|c| c.set(true));
    let out = f();
    ALLOW_ZERO_SPARSE.with(|c| c.set(false));
    out
}

fn is_boolean_expr(e: &ExprAst) -> bool {
    match e {
        ExprAst::Cmp(..) | ExprAst::And(..) | ExprAst::Or(..) | ExprAst::Not(..) => true,
        // NULL is "unknown" and is valid wherever a boolean is expected; a
        // column may hold a boolean (its type is not visible here).
        ExprAst::Literal(crate::Literal::Null) | ExprAst::Column(_) => true,
        // `TRUE`/`FALSE` parse to lower-case string literals.
        ExprAst::Literal(crate::Literal::Str(v)) => {
            v.eq_ignore_ascii_case("true") || v.eq_ignore_ascii_case("false")
        }
        ExprAst::Func { name, .. } => [
            "IS_NULL",
            "IS_NOT_NULL",
            "LIKE",
            "CONTAINS",
            "REGEX",
            "FILTER_TEXT",
            "FILTER_FULLTEXT",
        ]
        .iter()
        .any(|n| name.eq_ignore_ascii_case(n)),
        _ => false,
    }
}

/// Rejects `AND`/`OR`/`NOT` with non-boolean operands (e.g. `c1 + 0.1 AND …`).
fn validate_boolean_expr(e: &ExprAst) -> Result<()> {
    match e {
        ExprAst::And(a, b) | ExprAst::Or(a, b) => {
            for x in [a.as_ref(), b.as_ref()] {
                if !is_boolean_expr(x) {
                    return Err(Error::Invalid(
                        "boolean operator requires boolean operands".into(),
                    ));
                }
                validate_boolean_expr(x)?;
            }
            Ok(())
        }
        ExprAst::Not(a) => {
            if !is_boolean_expr(a) {
                return Err(Error::Invalid("NOT requires a boolean operand".into()));
            }
            validate_boolean_expr(a)
        }
        ExprAst::Binary(a, _, b) | ExprAst::Cmp(a, _, b) => {
            validate_boolean_expr(a)?;
            validate_boolean_expr(b)
        }
        ExprAst::Func { args, .. } => {
            for a in args {
                validate_boolean_expr(a)?;
            }
            Ok(())
        }
        _ => Ok(()),
    }
}

/// `DISTANCE()` requires a distance metric and `SIMILARITY()` a similarity
/// metric; reject the mismatch.
fn validate_search_pseudo_columns(s: &SearchClause, items: &[SelectItem]) -> Result<()> {
    let Some(m) = s.matches.first() else {
        return Ok(());
    };
    let is_sparse = m.kind.eq_ignore_ascii_case("SPARSE");
    let is_vector = m.kind.eq_ignore_ascii_case("VECTOR");
    if !is_vector && !is_sparse {
        return Ok(());
    }
    let metric = arg_str(m.args.get(if is_sparse { 1 } else { 2 }))
        .unwrap_or_else(|| if is_sparse { "ip".into() } else { "l2".into() })
        .to_ascii_lowercase();
    let distance_metric = matches!(metric.as_str(), "l2" | "hamming");
    for it in items {
        if let SelectItem::Func { name, .. } = it {
            let n = name.to_ascii_uppercase();
            if n == "DISTANCE" && !distance_metric {
                return Err(Error::Invalid(
                    "DISTANCE() requires a distance metric".into(),
                ));
            }
            if n == "SIMILARITY" && distance_metric {
                return Err(Error::Invalid(
                    "SIMILARITY() requires a similarity metric".into(),
                ));
            }
        }
    }
    Ok(())
}

/// Validates an INSERT value against its declared column type (dimension or
/// sparse index range).
fn validate_insert_value(type_text: &str, lit: &str) -> Result<()> {
    // A BOOLEAN cell must be `true`/`false`; the C++ CSV reader rejects anything
    // else with a parser error (`Boolean type should be true or false`).
    if type_text.trim().eq_ignore_ascii_case("BOOLEAN")
        || type_text.trim().eq_ignore_ascii_case("BOOL")
    {
        let v = lit.trim();
        // `NULL` arrives as literal text from the `INSERT` path, and the Thrift
        // wire encodes booleans as 1/0.
        if !v.is_empty()
            && !v.eq_ignore_ascii_case("true")
            && !v.eq_ignore_ascii_case("false")
            && !v.eq_ignore_ascii_case("null")
            && v != "1"
            && v != "0"
        {
            return Err(Error::Invalid(
                "parser: Boolean type should be true or false".into(),
            ));
        }
    }
    if is_sparse_type(type_text) {
        return validate_sparse_literal(type_text, lit);
    }
    // A scalar column rejects text that does not parse as its type: the
    // reference's `StringToValue` fails, surfacing as a conversion error.
    let upper = type_text.trim().to_ascii_uppercase();
    let v = lit.trim();
    if !v.is_empty() && !v.eq_ignore_ascii_case("null") {
        let integer = upper == "INT"
            || upper.starts_with("INTEGER")
            || upper.starts_with("TINYINT")
            || upper.starts_with("SMALLINT")
            || upper.starts_with("BIGINT")
            || upper.starts_with("HUGEINT");
        if integer && v.parse::<i64>().is_err() && v.parse::<f64>().is_err() {
            return Err(Error::Invalid(format!(
                "not supported type conversion: '{v}' to {}",
                upper
            )));
        }
        let floating = upper.starts_with("DOUBLE") || upper.starts_with("FLOAT");
        if floating && v.parse::<f64>().is_err() {
            return Err(Error::Invalid(format!(
                "not supported type conversion: '{v}' to {}",
                upper
            )));
        }
    }
    // A column that stores a vector/tensor/array cannot take a bare scalar;
    // the C++ engine rejects this as NOT_SUPPORTED.
    let v = lit.trim();
    if (is_embedding_type(type_text)
        || is_tensor_type(type_text)
        || is_array_type(type_text)
        || is_sparse_type(type_text))
        && !v.is_empty()
        && !v.eq_ignore_ascii_case("null")
        && !v.starts_with('[')
    {
        return Err(Error::Invalid(format!(
            "not supported: column type '{}' requires an array value",
            type_text.trim()
        )));
    }
    validate_tensor_dimension(type_text, lit)
}

/// Width of a sparse column's index element (`SPARSE(FLOAT, INT8)` etc.).
fn sparse_idx_width(type_text: &str) -> infinity_storage::bmp_cpp::IdxWidth {
    use infinity_storage::bmp_cpp::IdxWidth;
    let t = type_text.to_ascii_uppercase();
    if t.contains("INT8") || t.contains("TINYINT") {
        IdxWidth::I8
    } else if t.contains("INT16") || t.contains("SMALLINT") {
        IdxWidth::I16
    } else {
        IdxWidth::I32
    }
}

/// A sparse vector's indices must be `< bound`, non-negative and distinct.
fn validate_sparse_literal(type_text: &str, lit: &str) -> Result<()> {
    let t = lit.trim();
    if !t.starts_with('[') {
        return Ok(());
    }
    let bound = type_dim(type_text).unwrap_or(0);
    // An integer-element sparse column rejects fractional values.
    let up = type_text.to_ascii_uppercase();
    let int_elem = up
        .split('(')
        .nth(1)
        .map(|inner| {
            inner.trim_start().starts_with("INT") || inner.trim_start().starts_with("UINT")
        })
        .unwrap_or(false);
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    let mut seen: Vec<i64> = Vec::new();
    for item in inner.split(',') {
        let item = item.trim();
        if item.is_empty() {
            continue;
        }
        let idx_txt = item.split(':').next().unwrap_or(item).trim();
        let Ok(i) = idx_txt.parse::<i64>() else {
            return Err(Error::Invalid(format!("invalid sparse index '{idx_txt}'")));
        };
        if i < 0 {
            return Err(Error::Invalid(format!("sparse index {i} is negative")));
        }
        if bound > 0 && i >= bound as i64 {
            return Err(Error::Invalid(format!(
                "sparse index {i} is out of range (bound {bound})"
            )));
        }
        if int_elem {
            if let Some((_, val)) = item.split_once(':') {
                if val.trim().parse::<i64>().is_err() {
                    return Err(Error::Invalid(
                        "parser: sparse value must be an integer".into(),
                    ));
                }
            }
        }
        if seen.contains(&i) {
            return Err(Error::Invalid(format!("duplicate sparse index {i}")));
        }
        seen.push(i);
    }
    Ok(())
}

/// Reconstructs a `Literal` from a stored default's text form.
fn default_literal(s: &str) -> Literal {
    if s.eq_ignore_ascii_case("NULL") {
        Literal::Null
    } else if let Ok(i) = s.parse::<i64>() {
        Literal::Int(i)
    } else if let Ok(f) = s.parse::<f64>() {
        Literal::Float(f)
    } else {
        Literal::Str(s.to_string())
    }
}

/// Aligns INSERT values to the table schema: an explicit column list maps by
/// name; otherwise values are positional. Missing columns take their default.
fn align_insert_rows(
    schema: &[String],
    defaults: &[Option<String>],
    cols: Option<&[String]>,
    rows: &[Vec<Literal>],
) -> Vec<Vec<Literal>> {
    rows.iter()
        .map(|row| match cols {
            Some(names) => {
                let mut out: Vec<Literal> = (0..schema.len())
                    .map(|i| {
                        defaults
                            .get(i)
                            .and_then(|d| d.as_deref())
                            .map(default_literal)
                            .unwrap_or(Literal::Null)
                    })
                    .collect();
                for (name, val) in names.iter().zip(row) {
                    if let Some(i) = schema.iter().position(|c| c == name) {
                        if i < out.len() {
                            out[i] = val.clone();
                        }
                    }
                }
                out
            }
            None => {
                let mut out = row.clone();
                while out.len() < schema.len() {
                    out.push(
                        defaults
                            .get(out.len())
                            .and_then(|d| d.as_deref())
                            .map(default_literal)
                            .unwrap_or(Literal::Null),
                    );
                }
                out
            }
        })
        .collect()
}

fn parse_value(cell: &str) -> Value {
    if let Ok(i) = cell.parse::<i64>() {
        Value::Int(i)
    } else if let Ok(f) = cell.parse::<f64>() {
        Value::Float(f)
    } else if cell.eq_ignore_ascii_case("null") {
        Value::Null
    } else if cell.eq_ignore_ascii_case("true") {
        Value::Bool(true)
    } else if cell.eq_ignore_ascii_case("false") {
        Value::Bool(false)
    } else {
        Value::Str(cell.to_string())
    }
}

/// Sorts a posting list by row id and keeps the last frequency for repeated
/// rows. An UPDATE appends the rewritten row's terms without removing the old
/// entry (the old one must keep counting toward `df` until compaction), so the
/// query path has to collapse the duplicate.
fn dedupe_postings(list: &mut Vec<(u64, u32)>) {
    list.sort_unstable_by_key(|(row, _)| *row);
    let mut out: Vec<(u64, u32)> = Vec::with_capacity(list.len());
    for (row, tf) in list.drain(..) {
        match out.last_mut() {
            Some((last, last_tf)) if *last == row => *last_tf = tf,
            _ => out.push((row, tf)),
        }
    }
    *list = out;
}

/// The reference's memory indexer: the **unsealed** segment of one text
/// column, holding `term -> [(doc, tf, positions)]`, each document's length
/// and the corpus statistics.
///
/// C++: `MemoryIndexer` (`src/storage/invertedindex/memory_indexer.cppm`).
#[derive(Default)]
struct MemoryIndexer {
    /// `doc -> length`, used for BM25's document length.
    docs: HashMap<u64, u32>,
    postings: HashMap<String, Vec<(u64, u32, Vec<u32>)>>,
    live: std::collections::HashSet<u64>,
    n_docs: usize,
    total_len: u64,
    /// Approximate bytes held by this segment, for the memindex budget.
    bytes: usize,
}

impl MemoryIndexer {
    fn new() -> Self {
        Self::default()
    }

    /// C++: `MemoryIndexer::Insert` for one document. Returns `false` when the
    /// document would push the segment past `budget`, leaving it unchanged.
    fn insert(&mut self, seq: u64, text: &str, delims: &Option<Vec<char>>, budget: usize) -> bool {
        let toks = tokenize_index_positions(text, delims);
        // ~32 B per posting entry plus the positions and the term string.
        let mut cost = 64usize;
        for (term, _) in &toks {
            cost += term.len() + 32;
        }
        if self.bytes + cost > budget {
            return false;
        }
        self.bytes += cost;
        // A re-indexed row (UPDATE rewrites in place) replaces its length
        // rather than adding a second document to the corpus statistics.
        match self.docs.insert(seq, toks.len() as u32) {
            Some(old) => self.total_len = self.total_len.saturating_sub(old as u64),
            None => self.n_docs += 1,
        }
        self.total_len += toks.len() as u64;
        self.live.insert(seq);
        let mut pos: HashMap<&str, Vec<u32>> = HashMap::new();
        for (term, at) in &toks {
            pos.entry(term.as_str()).or_default().push(*at);
        }
        for (term, positions) in pos {
            self.postings.entry(term.to_string()).or_default().push((
                seq,
                positions.len() as u32,
                positions,
            ));
        }
        true
    }

    /// C++: deleting a document only drops its live marker; the postings and
    /// length stay until the segment is compacted.
    fn delete(&mut self, seq: u64) {
        self.live.remove(&seq);
    }

    /// C++: `GetDocCount`.
    fn doc_count(&self) -> usize {
        self.n_docs
    }

    /// C++: `MemUsed`.
    fn mem_used(&self) -> usize {
        self.bytes
    }

    fn is_empty(&self) -> bool {
        self.docs.is_empty()
    }

    /// Drops the segment (the budget-exhausted path and `Reset`).
    fn reset(&mut self) {
        self.postings.clear();
        self.postings.shrink_to_fit();
        self.docs.clear();
        self.live.clear();
        self.bytes = 0;
        self.n_docs = 0;
        self.total_len = 0;
    }
}

/// In-memory inverted index for one text column: a [`MemoryIndexer`] for the
/// unsealed segment plus the sealed on-disk segments.
/// C++: the `MemoryIndexer` + `IndexSegmentReader` pair held by a column.
struct TextIndex {
    delims: Option<Vec<char>>,
    /// The unsealed segment.
    mem: MemoryIndexer,
    disabled: bool,
    /// The sealed segments, oldest first: each owns one on-disk index
    /// (`.dic`, `.pos`, `.len`) exactly like the reference's per-segment
    /// indexes.
    reader: ColumnIndexReader,
    /// Directory holding this column's sealed segment files.
    dir: String,
    /// Rows re-indexed after their segment was sealed; the sealed copy is stale
    /// and must not be scored (the memory index holds the new text).
    overridden: std::collections::HashSet<u64>,
}

/// One sealed segment's index: the mmap'd reader plus the corpus counts.
struct SealedSegment {
    docs: u32,
    total_len: u64,
    reader: infinity_storage::fulltext_segment::SegmentIndexReader,
}

impl SealedSegment {
    fn contains(&self, row: u64) -> bool {
        self.reader.ordinal(row).is_some()
    }
}

/// The per-column reader over the sealed segments.
///
/// C++: `ColumnIndexReader` (`src/storage/invertedindex/column_index_reader.cppm`).
/// It owns one `IndexSegmentReader` per sealed segment and answers term
/// postings and document lengths across all of them.
#[derive(Default)]
struct ColumnIndexReader {
    segments: Vec<SealedSegment>,
}

impl ColumnIndexReader {
    /// C++: `GetDocCount` over the segment readers.
    fn doc_count(&self) -> usize {
        self.segments.iter().map(|s| s.docs as usize).sum()
    }

    /// Total token count across the segment readers.
    fn total_len(&self) -> u64 {
        self.segments.iter().map(|s| s.total_len).sum()
    }

    /// C++: `GetIndexReader(term)` — postings from every sealed segment.
    fn postings(&self, term: &str) -> Vec<(u64, u32)> {
        let mut list = Vec::new();
        for seg in &self.segments {
            if let Ok(mut hits) = seg.reader.postings(term) {
                list.append(&mut hits);
            }
        }
        list
    }

    /// C++: `GetColumnLength` — the length recorded by the newest segment
    /// that contains `row`.
    fn doc_len(&self, row: u64) -> Option<u32> {
        self.segments
            .iter()
            .rev()
            .find(|s| s.contains(row))
            .and_then(|s| s.reader.doc_len_of_row(row))
    }

    fn contains(&self, row: u64) -> bool {
        self.segments.iter().any(|s| s.contains(row))
    }
}

/// The postings and corpus statistics a query sees: the sealed segments plus
/// the unsealed tail, with stale (re-indexed) rows removed.
struct MergedIndex {
    n_docs: usize,
    total_len: u64,
    postings: HashMap<String, Vec<(u64, u32)>>,
    doc_len: HashMap<u64, u32>,
}

impl TextIndex {
    fn new(dir: String, delims: Option<Vec<char>>) -> Self {
        Self {
            delims,
            mem: MemoryIndexer::new(),
            disabled: false,
            reader: ColumnIndexReader::default(),
            dir,
            overridden: Default::default(),
        }
    }

    /// Documents in the column across the sealed segments and the memory tail.
    fn total_docs(&self) -> usize {
        self.mem.doc_count() + self.reader.doc_count()
    }

    /// Writes the current (unsealed) segment's index to disk and clears the
    /// in-memory postings. Returns `false` (leaving everything in memory) if the
    /// files cannot be written, so a failure never loses results.
    fn seal(&mut self) -> bool {
        if self.mem.is_empty() {
            return false;
        }
        let base_row_id = match self.mem.docs.keys().min() {
            Some(&row) => row,
            None => return false,
        };
        let mut docs: Vec<(u64, u32)> = self.mem.docs.iter().map(|(d, l)| (*d, *l)).collect();
        docs.sort_unstable_by_key(|(d, _)| *d);
        let mut postings: std::collections::BTreeMap<String, Vec<(u64, u32)>> =
            std::collections::BTreeMap::new();
        for (term, list) in &self.mem.postings {
            let mut hits: Vec<(u64, u32)> = list.iter().map(|(d, tf, _)| (*d, *tf)).collect();
            dedupe_postings(&mut hits);
            postings.insert(term.clone(), hits);
        }
        let base_name = format!("ft_{base_row_id:016x}");
        let flags = infinity_storage::inverted_format::TermMetaFlags::default();
        if infinity_storage::fulltext_segment::write_segment_index(
            &self.dir,
            &base_name,
            base_row_id,
            &docs,
            &postings,
            flags,
        )
        .is_err()
        {
            return false;
        }
        let Ok(reader) = infinity_storage::fulltext_segment::SegmentIndexReader::open(
            &self.dir,
            &base_name,
            base_row_id,
            flags,
        ) else {
            return false;
        };
        self.reader.segments.push(SealedSegment {
            docs: docs.len() as u32,
            total_len: self.mem.total_len,
            reader,
        });
        self.mem.reset();
        true
    }

    /// Seals once the unsealed segment reaches its size budget or the
    /// configured `mem_index_capacity` row count.
    fn maybe_seal(&mut self, capacity: usize) {
        if self.mem.mem_used() >= text_segment_bytes()
            || (capacity > 0 && self.mem.doc_count() >= capacity)
        {
            self.seal();
        }
    }

    /// Postings and corpus statistics for `terms`, merging every sealed segment
    /// with the memory tail. Stale copies of re-indexed rows are dropped.
    fn merge(&self, terms: &[String]) -> MergedIndex {
        let n_docs = self.mem.n_docs + self.reader.doc_count();
        let total_len = self.mem.total_len + self.reader.total_len();
        let mut postings: HashMap<String, Vec<(u64, u32)>> = HashMap::new();
        let mut doc_len: HashMap<u64, u32> = HashMap::new();
        for term in terms {
            let mut list: Vec<(u64, u32)> = self
                .mem
                .postings
                .get(term)
                .map(|l| l.iter().map(|(d, tf, _)| (*d, *tf)).collect())
                .unwrap_or_default();
            list.append(&mut self.reader.postings(term));
            list.retain(|(d, _)| !self.overridden.contains(d));
            dedupe_postings(&mut list);
            for (d, _) in &list {
                if let Some(l) = self.mem.docs.get(d) {
                    doc_len.insert(*d, *l);
                    continue;
                }
                let dl = self.reader.doc_len(*d).unwrap_or(1);
                doc_len.insert(*d, dl);
            }
            postings.insert(term.clone(), list);
        }
        MergedIndex {
            n_docs,
            total_len,
            postings,
            doc_len,
        }
    }
}

/// Cap for one in-memory inverted index. The heavy import tests build several
/// indexes over 250k wide rows; without a cap the postings alone reached ~20 GB.
const TEXT_INDEX_BUDGET_BYTES: usize = 512 * 1024 * 1024;

/// Size at which the current segment is sealed to disk. Only this much posting
/// data stays resident per column; sealed segments are read through `mmap`,
/// like the reference's on-disk segments.
const TEXT_SEGMENT_BYTES: usize = 64 * 1024 * 1024;

/// The segment budget, overridable for tests that need to exercise sealing.
fn text_segment_bytes() -> usize {
    static CAP: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
    *CAP.get_or_init(|| {
        std::env::var("INFINITY_TEXT_SEGMENT_BYTES")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(TEXT_SEGMENT_BYTES)
    })
}

type SharedTextIndex = Arc<std::sync::RwLock<TextIndex>>;

struct VectorCache {
    dims: usize,
    rows: Vec<(usize, Vec<f64>)>,
    live: std::collections::HashSet<usize>,
    built_at: std::time::Instant,
}
type SharedVectorCache = Arc<std::sync::RwLock<VectorCache>>;

/// The in-memory form of a vector index: the rows written since the last dump.
/// The reference keeps one `HnswIndexInMem` per segment; a query searches this
/// segment and merges it with the sealed (on-disk) ones.
/// Bounds how many full-table scans (full-text or vector) run at once. Each
/// such scan materialises a large share of the table, so several concurrent
/// readers can exhaust the process's memory even when a single query fits.
struct ScanGate {
    active: Mutex<usize>,
    cv: std::sync::Condvar,
    limit: usize,
}

impl ScanGate {
    fn new(limit: usize) -> Self {
        Self {
            active: Mutex::new(0),
            cv: std::sync::Condvar::new(),
            limit: limit.max(1),
        }
    }

    fn enter(&self) -> ScanSlot<'_> {
        let mut active = self.active.lock().unwrap();
        while *active >= self.limit {
            active = self.cv.wait(active).unwrap();
        }
        *active += 1;
        ScanSlot(self)
    }
}

struct ScanSlot<'a>(&'a ScanGate);

impl Drop for ScanSlot<'_> {
    fn drop(&mut self) {
        let mut active = self.0.active.lock().unwrap();
        *active = active.saturating_sub(1);
        self.0.cv.notify_one();
    }
}

/// One multi-vector index in the reference's two forms: the sealed segment
/// files (`seg_*.mvec` / `seg_*.emvb`, mapped read-only) and the memory tail
/// holding rows inserted since the last dump. Building the tail at write time
/// and dumping it on `flush()` keeps the query path read-only.
struct MultiVectorIndex {
    dims: usize,
    dir: PathBuf,
    /// `ROW_ID -> token rows` for documents not yet dumped.
    tail: std::collections::BTreeMap<u64, Vec<Vec<f32>>>,
    /// One mmap reader per dumped chunk plus the row id of every document and
    /// the persisted residual PQ (the reference's stage-2 candidate filter).
    #[allow(clippy::type_complexity)]
    sealed: Vec<(
        Arc<infinity_storage::multivector_segment::MultiVectorSegmentReader>,
        Arc<Vec<u64>>,
        Option<Arc<infinity_storage::emvb_segment::EmvbPqIndex>>,
    )>,
    /// Chunk ordinal used in file names.
    next_chunk: u64,
    /// Bytes held by `tail`, for the unified memindex budget.
    tail_bytes: usize,
    /// Bumped on every tail mutation; the in-memory EMVB index is rebuilt when
    /// it lags (the reference's `EMVBIndexInMem`).
    tail_generation: u64,
    tail_emvb: std::sync::Mutex<Option<MvTailEmvb>>,
    /// PLAID options when the column carries a `USING PLAID` index.
    plaid_config: Option<PlaidConfig>,
    /// The reference's `PlaidIndex` over every document (`PlaidIndexInMem`
    /// after its build, kept in step by incremental adds).
    plaid: Option<infinity_index::plaid::PlaidIndex>,
    /// Row id of each PLAID document id.
    plaid_seqs: Vec<u64>,
    /// SMVE options when the column carries a `USING SMVE` index.
    smve_config: Option<SmveConfig>,
    /// The reference's `SMVEIndexInMem` (a BMP over SMVE sparse codes).
    smve: Option<infinity_index::smve::SmveIndex>,
    /// Row id of each SMVE document id.
    smve_seqs: Vec<u64>,
}

/// `CREATE INDEX … USING PLAID WITH (nbits=…, n_centroids=…)`.
#[derive(Clone, Copy, Debug)]
struct PlaidConfig {
    nbits: u32,
    n_centroids: u32,
    colbertsar: bool,
}

/// `CREATE INDEX … USING SMVE WITH (width=…, topk=…)`.
#[derive(Clone, Copy, Debug)]
struct SmveConfig {
    width: u32,
    topk: u32,
}

/// The reference's `EMVBIndexInMem` over the multi-vector tail.
struct MvTailEmvb {
    generation: u64,
    data: infinity_storage::emvb_segment::EmvbData,
    seqs: Vec<u64>,
}

/// C++ `EMVBIndexInMem`: `build_index_threshold_ = 32 * (1 << bits)` with
/// `residual_pq_subspace_bits = 8`.
const EMVB_BUILD_THRESHOLD: usize = 32 * 256;

/// The threshold, overridable for tests that need to exercise the build.
fn emvb_build_threshold() -> usize {
    static CAP: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
    *CAP.get_or_init(|| {
        std::env::var("INFINITY_EMVB_BUILD_THRESHOLD")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(EMVB_BUILD_THRESHOLD)
    })
}

fn multivector_doc_bytes(doc: &[Vec<f32>]) -> usize {
    doc.iter().map(|t| t.len() * 4 + 4).sum()
}

/// `MaxSim` over one in-memory document: sum over query rows of the best dot
/// product against a stored row (the same formula as the mmap reader).
fn maxsim_tokens(query: &[Vec<f32>], doc: &[Vec<f32>]) -> f32 {
    let mut total = 0.0f32;
    for q in query {
        let mut best = f32::NEG_INFINITY;
        for t in doc {
            if t.len() != q.len() {
                continue;
            }
            let score: f32 = q.iter().zip(t).map(|(a, b)| a * b).sum();
            if score > best {
                best = score;
            }
        }
        if best != f32::NEG_INFINITY {
            total += best;
        }
    }
    total
}

impl MultiVectorIndex {
    fn new(
        dir: PathBuf,
        dims: usize,
        plaid_config: Option<PlaidConfig>,
        smve_config: Option<SmveConfig>,
    ) -> Self {
        let next_chunk = std::fs::read_dir(&dir)
            .map(|d| {
                d.filter_map(|e| e.ok())
                    .filter(|e| e.path().extension().is_some_and(|x| x == "mvec"))
                    .count() as u64
            })
            .unwrap_or(0);
        Self {
            dims,
            dir,
            tail: Default::default(),
            sealed: Vec::new(),
            next_chunk,
            tail_bytes: 0,
            tail_generation: 0,
            tail_emvb: std::sync::Mutex::new(None),
            plaid_config,
            plaid: None,
            plaid_seqs: Vec::new(),
            smve_config,
            smve: None,
            smve_seqs: Vec::new(),
        }
    }

    fn doc_count(&self) -> usize {
        self.tail.len()
            + self
                .sealed
                .iter()
                .map(|(r, _, _)| r.doc_count())
                .sum::<usize>()
    }

    fn insert(&mut self, seq: u64, doc: Vec<Vec<f32>>) {
        let replacing = self.tail.contains_key(&seq);
        let covered = self
            .plaid
            .as_ref()
            .is_some_and(|p| p.doc_num() as usize == self.doc_count());
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(multivector_doc_bytes(&old));
        }
        self.tail_bytes += multivector_doc_bytes(&doc);
        self.tail.insert(seq, doc.clone());
        self.tail_generation += 1;
        // The reference's `PlaidIndexInMem::Insert`: once a trained index
        // covers every document, new rows are added incrementally. A rewritten
        // row cannot be removed from PLAID, so the index is dropped and the
        // next rebuild recreates it.
        if let Some(index) = self.plaid.as_mut() {
            let flat: Vec<f32> = doc.iter().flatten().copied().collect();
            let added = !replacing
                && covered
                && index
                    .add_multiple_docs_embeddings(&flat, &[doc.len() as u32])
                    .is_ok();
            if added {
                self.plaid_seqs.push(seq);
            } else {
                self.plaid = None;
                self.plaid_seqs.clear();
            }
        }
        // `SMVEIndexInMem::BuildFromColumn` runs per inserted block; the BMP
        // accepts one document at a time.
        if let Some(index) = self.smve.as_mut() {
            let label = index.row_count() as u32;
            if replacing || index.add_document(label, &doc).is_err() {
                self.smve = None;
                self.smve_seqs.clear();
            } else {
                self.smve_seqs.push(seq);
            }
        }
    }

    fn remove(&mut self, seq: u64) {
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(multivector_doc_bytes(&old));
            self.tail_generation += 1;
            // Neither index supports per-document deletes; the search bitmask
            // hides dead rows, and a rewritten row needs a fresh build.
            self.plaid = None;
            self.plaid_seqs.clear();
            self.smve = None;
            self.smve_seqs.clear();
        }
    }

    /// Builds the reference's `PlaidIndexInMem` over every buffered document.
    ///
    /// Called before the tail is sealed (the reference builds it while the
    /// embeddings are still in `buffered_embeddings_`). Below
    /// `max(256, n_centroids * 32)` embeddings the reference leaves the index
    /// unbuilt and the caller falls back to an exhaustive scan of the range.
    fn build_plaid(&mut self) {
        eprintln!(
            "DEBUG build_plaid cfg={:?} tail={}",
            self.plaid_config,
            self.tail.len()
        );
        let Some(cfg) = self.plaid_config else {
            return;
        };
        let total: usize = self.tail.values().map(Vec::len).sum();
        let threshold = 256usize.max(cfg.n_centroids as usize * 32);
        if total < threshold {
            return;
        }
        let mut k = if cfg.n_centroids == 0 {
            infinity_index::plaid::PlaidIndex::compute_auto_n_centroids(total as u64)
        } else {
            cfg.n_centroids
        };
        while k > 8 && total < 32 * k as usize {
            k >>= 1;
        }
        k = k.div_ceil(8) * 8;
        if k as usize > total {
            return;
        }
        let mut flat = Vec::with_capacity(total * self.dims);
        let mut lens = Vec::with_capacity(self.tail.len());
        let mut seqs = Vec::with_capacity(self.tail.len());
        for (seq, doc) in &self.tail {
            seqs.push(*seq);
            lens.push(doc.len() as u32);
            for token in doc {
                flat.extend_from_slice(token);
            }
        }
        let Ok(mut index) = infinity_index::plaid::PlaidIndex::new(
            0,
            self.dims as u32,
            cfg.nbits,
            k,
            cfg.colbertsar,
        ) else {
            return;
        };
        if index.train(k, &flat, total, 4).is_err() {
            return;
        }
        if index.add_multiple_docs_embeddings(&flat, &lens).is_err() {
            return;
        }
        self.plaid = Some(index);
        self.plaid_seqs = seqs;
    }

    /// C++: `SMVEIndexInMem::BuildFromColumn` over the buffered documents.
    fn build_smve(&mut self) {
        eprintln!(
            "DEBUG build_smve cfg={:?} tail={}",
            self.smve_config,
            self.tail.len()
        );
        let Some(cfg) = self.smve_config else {
            return;
        };
        let mut docs: Vec<(u32, Vec<Vec<f32>>)> = Vec::with_capacity(self.tail.len());
        let mut seqs = Vec::with_capacity(self.tail.len());
        for (i, (seq, doc)) in self.tail.iter().enumerate() {
            seqs.push(*seq);
            docs.push((i as u32, doc.clone()));
        }
        if docs.is_empty() {
            return;
        }
        let mut index = infinity_index::smve::SmveIndex::new(cfg.width, cfg.topk, self.dims as u32);
        if index.build_from_column(&docs).is_ok() {
            self.smve = Some(index);
            self.smve_seqs = seqs;
        }
    }

    /// Dumps the memory tail into per-segment files (one `ROW_ID` segment per
    /// file, the reference's chunk dump) and keeps their mmap readers.
    fn seal(&mut self) {
        if self.tail.is_empty() {
            return;
        }
        #[allow(clippy::type_complexity)]
        let mut by_segment: std::collections::BTreeMap<u64, Vec<(u64, Vec<Vec<f32>>)>> =
            Default::default();
        for (seq, doc) in std::mem::take(&mut self.tail) {
            by_segment.entry(seq >> 32).or_default().push((seq, doc));
        }
        self.tail_bytes = 0;
        self.tail_generation += 1;
        for (segment, mut rows) in by_segment {
            rows.sort_by_key(|(seq, _)| *seq);
            let ids: Vec<u64> = rows.iter().map(|(seq, _)| *seq).collect();
            let docs: Vec<Vec<Vec<f32>>> = rows.into_iter().map(|(_, doc)| doc).collect();
            let name = format!("seg_{segment}_{}_{}.mvec", self.next_chunk, docs.len());
            self.next_chunk += 1;
            let path = self.dir.join(name);
            if infinity_storage::multivector_segment::write_multivector_segment(
                &path, self.dims, &docs,
            )
            .is_err()
            {
                continue;
            }
            // The reference's EMVB chunk (coarse centroids, document tables and
            // inverted lists) lives beside the raw-token segment. Borrow the
            // token rows so sealing does not duplicate the vectors.
            let emvb: Vec<(u32, &[Vec<f32>])> = docs
                .iter()
                .zip(ids.iter())
                .map(|(doc, seq)| ((*seq & 0xffff_ffff) as u32, doc.as_slice()))
                .collect();
            let _ = infinity_storage::emvb_segment::write_emvb_segment_refs(
                &path.with_extension("emvb"),
                self.dims,
                (segment & 0xffff_ffff) as u32,
                &emvb,
            );
            if let Ok(reader) =
                infinity_storage::multivector_segment::MultiVectorSegmentReader::open(&path)
            {
                let pq =
                    infinity_storage::emvb_segment::EmvbReader::open(&path.with_extension("emvb"))
                        .ok()
                        .and_then(|r| r.pq_index())
                        .map(Arc::new);
                self.sealed.push((Arc::new(reader), Arc::new(ids), pq));
            }
        }
    }

    /// Top-k over sealed chunks and the memory tail, skipping tombstoned rows.
    fn search(
        &self,
        query: &[Vec<f32>],
        k: usize,
        dead: &std::collections::HashSet<u64>,
        exact: Option<&dyn Fn(u64) -> Option<f32>>,
    ) -> Vec<(u64, f32)> {
        // A built PLAID index answers the query directly: IVF probing, an
        // approximate pass over the candidates and an exact residual rerank
        // (the reference's `PlaidIndex::SearchWithBitmask`). Deleted rows are
        // excluded by the bitmask.
        if let Some(index) = &self.plaid {
            if index.doc_num() as usize == self.plaid_seqs.len() {
                let mut mask = infinity_index::plaid::Bitmask::new(index.doc_num() as usize);
                for (doc, seq) in self.plaid_seqs.iter().enumerate() {
                    if !dead.contains(seq) {
                        mask.set(doc as u32);
                    }
                }
                let flat: Vec<f32> = query.iter().flatten().copied().collect();
                let topn = k.max(1);
                let result = index.search_with_bitmask(
                    &flat,
                    query.len(),
                    topn,
                    &mask,
                    8,
                    -0.1,
                    topn.saturating_mul(100),
                    topn.saturating_mul(10),
                );
                let mut hits: Vec<(u64, f32)> = result
                    .ids
                    .iter()
                    .zip(&result.scores)
                    .filter_map(|(id, score)| {
                        self.plaid_seqs.get(*id as usize).map(|seq| (*seq, *score))
                    })
                    .collect();
                if !hits.is_empty() {
                    hits.sort_by(|a, b| {
                        b.1.partial_cmp(&a.1)
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.0.cmp(&b.0))
                    });
                    hits.truncate(k);
                    return hits;
                }
            }
        }
        // `SMVEIndexInMem::Search`: BMP over the sparse codes yields candidates
        // that the reference re-ranks with exact MaxSim from the column; the
        // `exact` callback supplies that score here.
        if let Some(index) = &self.smve {
            if index.row_count() == self.smve_seqs.len() {
                let allowed: Vec<bool> = self
                    .smve_seqs
                    .iter()
                    .map(|seq| !dead.contains(seq))
                    .collect();
                let (scores, docs) = index.search(query, k.max(1), 5, Some(&allowed));
                let mut hits: Vec<(u64, f32)> = docs
                    .iter()
                    .zip(&scores)
                    .filter_map(|(doc, score)| {
                        self.smve_seqs.get(*doc as usize).map(|seq| {
                            let rank = exact.and_then(|f| f(*seq)).unwrap_or(*score);
                            (*seq, rank)
                        })
                    })
                    .filter(|(seq, _)| !dead.contains(seq))
                    .collect();
                if !hits.is_empty() {
                    hits.sort_by(|a, b| {
                        b.1.partial_cmp(&a.1)
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.0.cmp(&b.0))
                    });
                    hits.truncate(k);
                    return hits;
                }
            }
        }
        let mut hits: Vec<(u64, f32)> = Vec::new();
        for (reader, ids, pq) in &self.sealed {
            // The persisted residual codes rank the documents (EMVB stage 2);
            // the survivors are re-scored exactly from the raw tokens.
            let docs: Vec<usize> = match pq {
                Some(pq) if reader.doc_count() > 256 => {
                    pq.prefilter(query, (k * 4).max(64).min(reader.doc_count()))
                }
                _ => (0..reader.doc_count()).collect(),
            };
            for doc in docs {
                let Some(seq) = ids.get(doc) else { continue };
                if dead.contains(seq) {
                    continue;
                }
                hits.push((*seq, reader.maxsim(query, doc)));
            }
        }
        // The memory segment: once the tail holds enough embeddings the
        // reference builds an in-memory `EMVBIndex` and searches it; the
        // survivors are re-scored exactly from the raw tokens.
        let tail_embeddings: usize = self.tail.values().map(Vec::len).sum();
        if tail_embeddings >= emvb_build_threshold() {
            let mut guard = self.tail_emvb.lock().unwrap();
            if guard
                .as_ref()
                .map(|t| t.generation != self.tail_generation)
                .unwrap_or(true)
            {
                let mut docs: Vec<(u32, &[Vec<f32>])> = Vec::with_capacity(self.tail.len());
                let mut seqs = Vec::with_capacity(self.tail.len());
                for (i, (seq, doc)) in self.tail.iter().enumerate() {
                    docs.push((i as u32, doc.as_slice()));
                    seqs.push(*seq);
                }
                let data = infinity_storage::emvb_segment::build_emvb(self.dims, 0, &docs);
                *guard = Some(MvTailEmvb {
                    generation: self.tail_generation,
                    data,
                    seqs,
                });
            }
            if let Some(tail) = guard.as_ref() {
                for (doc, _) in tail.data.view().search(
                    query,
                    (k + dead.len()).max(1),
                    infinity_index::emvb::EmvbSearchOptions::for_topk(k.max(1)),
                ) {
                    let Some(seq) = tail.seqs.get(doc as usize) else {
                        continue;
                    };
                    if dead.contains(seq) {
                        continue;
                    }
                    if let Some(tokens) = self.tail.get(seq) {
                        hits.push((*seq, maxsim_tokens(query, tokens)));
                    }
                }
            }
            drop(guard);
        } else {
            for (seq, doc) in &self.tail {
                if dead.contains(seq) || doc.is_empty() {
                    continue;
                }
                hits.push((*seq, maxsim_tokens(query, doc)));
            }
        }
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits
    }
}

/// One sparse (BMP) index in the reference's two forms: sealed `seg_*.bmp`
/// chunks written with `BMPAlg::SaveToPtr`'s layout, and the memory tail.
struct SparseIndex {
    width: infinity_storage::bmp_cpp::IdxWidth,
    dir: PathBuf,
    /// `ROW_ID -> (term, weight)` for documents not yet dumped.
    tail: std::collections::BTreeMap<u64, Vec<(i32, f32)>>,
    /// One mmap reader per dumped chunk, with the `ROW_ID` segment it covers.
    sealed: Vec<(u64, Arc<infinity_storage::bmp_cpp::BmpCppReader>)>,
    next_chunk: u64,
    tail_bytes: usize,
    /// Bumped on every tail mutation; the in-memory BMP index below is rebuilt
    /// when it lags (the reference's `BMPAlg` held by the mem index).
    tail_generation: u64,
    tail_index: std::sync::Mutex<Option<SparseTailBmp>>,
}

/// The in-memory `BMPAlg` over the sparse tail, plus the `ROW_ID` of each
/// indexed document (`BMPAlg` documents are dense ids).
struct SparseTailBmp {
    generation: u64,
    alg: infinity_index::sparse::BmpAlg,
    seqs: Vec<u64>,
}

fn sparse_doc_bytes(doc: &[(i32, f32)]) -> usize {
    doc.len() * 8
}

impl SparseIndex {
    fn new(dir: PathBuf, width: infinity_storage::bmp_cpp::IdxWidth) -> Self {
        let next_chunk = std::fs::read_dir(&dir)
            .map(|d| {
                d.filter_map(|e| e.ok())
                    .filter(|e| e.path().extension().is_some_and(|x| x == "bmp"))
                    .count() as u64
            })
            .unwrap_or(0);
        Self {
            width,
            dir,
            tail: Default::default(),
            sealed: Vec::new(),
            next_chunk,
            tail_bytes: 0,
            tail_generation: 0,
            tail_index: std::sync::Mutex::new(None),
        }
    }

    fn insert(&mut self, seq: u64, doc: Vec<(i32, f32)>) {
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(sparse_doc_bytes(&old));
        }
        self.tail_bytes += sparse_doc_bytes(&doc);
        self.tail.insert(seq, doc);
        self.tail_generation += 1;
    }

    fn remove(&mut self, seq: u64) {
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(sparse_doc_bytes(&old));
            self.tail_generation += 1;
        }
    }

    fn doc_count(&self) -> usize {
        self.tail.len()
            + self
                .sealed
                .iter()
                .map(|(_, r)| r.doc_count())
                .sum::<usize>()
    }

    /// Dumps the memory tail into per-segment BMP chunks.
    fn seal(&mut self) {
        if self.tail.is_empty() {
            return;
        }
        #[allow(clippy::type_complexity)]
        let mut by_segment: std::collections::BTreeMap<u64, Vec<(u64, Vec<(i32, f32)>)>> =
            Default::default();
        for (seq, doc) in std::mem::take(&mut self.tail) {
            by_segment
                .entry(seq >> 32)
                .or_default()
                .push((seq & 0xffff_ffff, doc));
        }
        self.tail_bytes = 0;
        self.tail_generation += 1;
        for (segment, mut docs) in by_segment {
            docs.sort_by_key(|(row, _)| *row);
            let term_num = docs
                .iter()
                .flat_map(|(_, e)| e.iter().map(|(t, _)| *t + 1))
                .max()
                .unwrap_or(0)
                .max(1) as usize;
            let name = format!("seg_{segment}_{}_{}.bmp", self.next_chunk, docs.len());
            self.next_chunk += 1;
            let path = self.dir.join(name);
            if infinity_storage::bmp_cpp::write_bmp_file(
                &path,
                infinity_index::sparse::BMP_BLOCK_SIZE,
                self.width,
                &docs,
                term_num,
            )
            .is_err()
            {
                continue;
            }
            if let Ok(reader) = infinity_storage::bmp_cpp::BmpCppReader::open(&path, self.width) {
                self.sealed.push((segment, Arc::new(reader)));
            }
        }
    }

    /// Top-k over sealed chunks and the memory tail, skipping tombstones.
    fn search(
        &self,
        query: &[(i32, f32)],
        k: usize,
        dead: &std::collections::HashSet<u64>,
    ) -> Vec<(u64, f32)> {
        let mut hits: Vec<(u64, f32)> = Vec::new();
        for (segment, reader) in &self.sealed {
            // Ask for enough candidates that tombstoned rows cannot starve the
            // top-k; `search` clamps to the chunk's document count.
            for (row, score) in reader.search(query, k + dead.len(), 0.9) {
                let seq = (segment << 32) | u64::from(row as u32);
                if !dead.contains(&seq) {
                    hits.push((seq, score));
                }
            }
        }
        // The memory segment is the reference's in-memory `BMPAlg`: rebuild it
        // when the tail changed since the last query.
        let qv: Vec<(u32, f32)> = query.iter().map(|(t, v)| (*t as u32, *v)).collect();
        let mut guard = self.tail_index.lock().unwrap();
        if guard
            .as_ref()
            .map(|t| t.generation != self.tail_generation)
            .unwrap_or(true)
        {
            let mut alg =
                infinity_index::sparse::BmpAlg::new(infinity_index::sparse::BMP_BLOCK_SIZE);
            let mut seqs = Vec::with_capacity(self.tail.len());
            for (seq, doc) in &self.tail {
                let doc32: Vec<(u32, f32)> = doc.iter().map(|(t, v)| (*t as u32, *v)).collect();
                alg.add_doc(&doc32);
                seqs.push(*seq);
            }
            alg.build();
            *guard = Some(SparseTailBmp {
                generation: self.tail_generation,
                alg,
                seqs,
            });
        }
        if let Some(tail) = guard.as_ref() {
            for (doc, score) in tail.alg.search_knn(
                &qv,
                k + dead.len(),
                infinity_index::sparse::BmpSearchOptions::default(),
            ) {
                if let Some(seq) = tail.seqs.get(doc) {
                    if !dead.contains(seq) {
                        hits.push((*seq, score));
                    }
                }
            }
        }
        drop(guard);
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits
    }
}

/// The storage layer: the catalog and the KV store.
///
/// C++: `Storage` (`src/storage/storage.cppm`) — the owner of databases,
/// tables and their metadata. The `Engine` composes it.
pub struct Storage {
    catalog: Catalog,
    store: Arc<dyn MetaStore>,
    /// Serializes checkpoints: the block mirror rewrites `seg_0`, so two
    /// concurrent flushes would race on its files.
    flush_lock: Mutex<()>,
}

/// Sealed high-cardinality chunk readers with their base row id.
type SecondaryDiskChunks = Vec<(
    u64,
    Arc<infinity_storage::secondary_segment::SecondarySegmentReader>,
)>;
/// Sealed low-cardinality (boolean) chunk readers with their base row id.
type SecondaryLowChunks = Vec<(u64, Arc<infinity_storage::secondary_lowcard::LowCardReader>)>;

/// Secondary-index registry of one table (the reference's index metas under
/// `src/storage/catalog/meta/` plus the sealed chunk readers), keyed by column.
#[derive(Default)]
struct SecondaryIndexes {
    /// Memory tails: rows written since the last chunk dump.
    indexes: HashMap<String, SecondaryIndex>,
    /// Low-cardinality (boolean) chunk readers with their base row id.
    low: HashMap<String, SecondaryLowChunks>,
    /// High-cardinality (integer/varchar) PGM chunk readers.
    disk: HashMap<String, SecondaryDiskChunks>,
    /// Columns whose memory map is a tail backed by on-disk chunks.
    disk_backed: std::collections::HashSet<String>,
}

/// Per-table write bookkeeping and row-id allocation.
///
/// C++ counterpart: the runtime state behind `TableMeta` / `SegmentMeta`
/// (`src/storage/catalog/meta/`) plus the block map (`BlockIndex`,
/// `src/storage/common/block_index.cppm`). Upstream removed the older
/// `TableEntry` / `SegmentEntry` / `BlockManager` classes (PR #2722), so this
/// type stands in for their catalog-level successors, not for a single class.
#[derive(Default)]
struct TableEntry {
    /// Number of write batches (`COPY`/`INSERT`) seen by this process.
    write_batches: usize,
    /// Row ids the C++ block mirror was written from, in block order.
    ///
    /// The mirror is a snapshot: rows appended after it was written are read
    /// from the row journal, and rows deleted since it was written are hidden
    /// by their tombstones (`TableEntry`'s block snapshot is the Rust stand-in
    /// for the reference's per-block row ranges + delete bitmap).
    cpp_block_rows: Option<Arc<Vec<usize>>>,
    /// Append-only sidecar holding this table's wide cells.
    row_blob: Option<Arc<Mutex<std::fs::File>>>,
    /// `(segment, offset, sealed)` of the row-id allocator.
    row_counter: Option<(u32, u32, bool)>,
    /// Segments allocated by the row-id allocator, by id.
    segments: std::collections::BTreeMap<u32, SegmentEntry>,
    /// Full-text analyzer per column (`CREATE INDEX … WITH (analyzer=…)`).
    analyzers: HashMap<String, String>,
    /// Analyzer per index name: two indexes on one column may differ.
    analyzers_by_index: HashMap<String, String>,
    /// `CREATE INDEX … COMMENT '…'` per index name.
    comments: HashMap<String, String>,
    /// HNSW memory segment per vector column (`MemIndex::hnsw_index_`).
    hnsw_mem: HashMap<String, Arc<Mutex<hnsw_mem::HnswIndexInMem>>>,
    /// HNSW bytes buffered per column since its last dump (memindex quota).
    index_pending_bytes: HashMap<String, usize>,
    /// Whether this table's HNSW indexes were probed since the last dump.
    hnsw_probed: bool,
    /// Sparse (BMP) indexes per column.
    sparse: HashMap<String, Arc<std::sync::RwLock<SparseIndex>>>,
    /// Multi-vector indexes per column.
    mv: HashMap<String, Arc<std::sync::RwLock<MultiVectorIndex>>>,
    /// Full-text inverted indexes per column.
    text: HashMap<String, SharedTextIndex>,
    /// Secondary indexes per column with their sealed chunk readers.
    secondary: SecondaryIndexes,
    /// HNSW chunks loaded from the C++ block mirror, per column (the
    /// reference's `TableIndexMeta` chain for the index, on-disk form).
    vector: HashMap<String, Vec<Arc<crate::cpp_blocks::LoadedVectorIndex>>>,
    /// IVF chunks loaded from the C++ mirror, per column.
    ivf: HashMap<String, Vec<Arc<infinity_index::ivf::IvfIndex>>>,
    /// IVF memory segment: rows written since the last mirror dump, merged
    /// with the sealed chunks at query time.
    ivf_mem: HashMap<String, Vec<(u64, Vec<f32>)>>,
    /// Block map of the C++-format mirror (C++ `BlockIndex`): block ids, row
    /// ranges and the decoded-block LRU shared with `Context::buffer`.
    block_index: Option<Arc<Mutex<infinity_storage::block_index::BlockIndex>>>,
    /// Live row count, maintained incrementally so the per-batch mirror check
    /// does not rescan every key. `None` means "recompute on demand".
    live_rows: Option<usize>,
    /// Decoded rows of a **scalar-only** table (the reference's in-memory
    /// `DataBlock`s). Served by `fetch_rows_with_ids` while it covers every
    /// live row; any mutation that rewrites rows drops it.
    row_cache: Option<RowCache>,
}

/// One segment of a table: its id and the highest row offset ever allocated.
///
/// C++: `SegmentMeta` (`src/storage/catalog/meta/segment_meta.cppm`); the
/// removed `SegmentEntry` was its runtime counterpart.
struct SegmentEntry {
    id: u32,
    /// Number of rows in the segment (`SegmentEntry::row_count`).
    rows: u32,
}

/// A table's decoded rows, bounded by [`ROW_CACHE_BUDGET_BYTES`].
struct RowCache {
    rows: Vec<(usize, Vec<Value>)>,
    bytes: usize,
}

const ROW_CACHE_BUDGET_BYTES: usize = 64 * 1024 * 1024;

/// Decoded rows paired with their row ids (the executor's scan unit).
type TableRows = Vec<(usize, Vec<Value>)>;

fn value_bytes(v: &Value) -> usize {
    match v {
        Value::Str(s) => s.len() + 24,
        _ => 24,
    }
}

impl RowCache {
    fn push(&mut self, seq: usize, row: Vec<Value>) {
        self.bytes += 24 + row.iter().map(value_bytes).sum::<usize>();
        self.rows.push((seq, row));
    }
}

/// Only scalar columns are cached: array/tensor/sparse cells have several
/// textual representations, so their decoded form is not stable enough to
/// reuse across write and scan paths.
fn scalar_only(types: &[String]) -> bool {
    types.iter().all(|t| {
        let t = t.trim().to_ascii_uppercase();
        !is_blob_type(&t)
            && !t.contains("ARRAY")
            && (t.starts_with("INT")
                || t.starts_with("BIGINT")
                || t.starts_with("SMALLINT")
                || t.starts_with("TINYINT")
                || t.starts_with("FLOAT")
                || t.starts_with("DOUBLE")
                || t.starts_with("REAL")
                || t.starts_with("VARCHAR")
                || t.starts_with("BOOL"))
    })
}

/// Engine-wide runtime state.
///
/// C++: `InfinityContext` (`src/storage/infinity_context.cppm`) — the shared
/// configuration, buffer manager and service handles. `Engine` composes it
/// instead of owning these fields directly.
struct Context {
    /// Tokio exchange capacity per pipeline edge.
    exchange_capacity: usize,
    /// Bounded LRU cache of decoded column blocks (the reference's
    /// `BufferManager`), sized by `[buffer] buffer_manager_size`.
    buffer: Mutex<
        infinity_storage::buffer::BufferManager<Vec<infinity_storage::cpp_format::RawBlockColumn>>,
    >,
    /// session/global configuration set via `SET CONFIG …`
    config: Mutex<HashMap<String, String>>,
    /// `[buffer] memindex_memory_quota`, in bytes. `0` disables the dump
    /// trigger (the reference only dumps when a quota is configured).
    memindex_quota: std::sync::atomic::AtomicU64,
    /// `[storage] mem_index_capacity`: rows an in-memory index may hold before
    /// it is dumped into a sealed segment (the reference's
    /// `Config::MemIndexCapacity()`). `0` disables the row trigger.
    memindex_capacity: std::sync::atomic::AtomicUsize,
    /// Object store backing the C++-format data files (`storage_type =
    /// "minio"`). `None` keeps everything on the local disk.
    object_store: Mutex<Option<Arc<infinity_storage::s3::S3Client>>>,
    /// Concurrency limit for full-table text/vector scans.
    scan_gate: ScanGate,
    /// `[buffer] buffer_manager_size`: the decoded HNSW chunk budget. Loaded
    /// graphs beyond it are evicted, and a scan that no longer sees full label
    /// coverage falls back to the exact scan (the reference's buffer manager).
    hnsw_budget: std::sync::atomic::AtomicUsize,
    /// `query_count` global/session variable: statements executed so far.
    query_count: std::sync::atomic::AtomicU64,
    /// `connected_timestamp` session variable: when the engine was opened.
    connected_at: String,
}

/// The engine: catalog + transaction manager + KV store + a Tokio runtime.
pub struct Engine {
    /// Catalog + KV store (the reference's `Storage`).
    storage: Storage,
    txn: TxnManager,
    rt: tokio::runtime::Runtime,
    /// the engine's data directory (root of the C++-format block mirror)
    dir: PathBuf,
    /// Engine-wide runtime state (the reference's `InfinityContext`).
    context: Context,
    /// cached `SELECT` results keyed by the statement text
    result_cache: Mutex<HashMap<String, Vec<QueryResult>>>,
    /// the database new unqualified table names are resolved against
    current_db: Mutex<String>,
    /// Per-table write bookkeeping (C++ `TableEntry`), keyed by the
    /// fully-qualified lower-case table name.
    tables: Mutex<HashMap<String, TableEntry>>,
    /// Write batches since the last `malloc_trim` (see `insert_literals`).
    batches_since_trim: std::sync::atomic::AtomicUsize,
    vector_cache: Mutex<HashMap<String, SharedVectorCache>>,
}

fn index_key(table: &str, column: &str) -> String {
    format!("{table}.{column}")
}

/// Indexes one document's tokens: length plus `(doc, tf, positions)` postings.
fn index_row(index: &mut TextIndex, seq: u64, text: &str) {
    if index.disabled {
        return;
    }
    // `MemoryIndexer::Insert` fails when the segment would exceed the budget;
    // the reference would spill, the port stops indexing this column.
    if !index
        .mem
        .insert(seq, text, &index.delims, TEXT_INDEX_BUDGET_BYTES)
    {
        index.mem.reset();
        index.disabled = true;
    }
}

/// Marks a document deleted. Its postings and length stay: the reference's
/// segments keep deleted documents until compaction, so corpus statistics must
/// still count them; the search paths filter them out by sequence id.
fn index_delete_row(index: &mut TextIndex, seq: u64) {
    index.mem.delete(seq);
    // A row whose text already went to a sealed segment keeps its old postings
    // there; the fresh text is added to the memory tail, so the sealed copy
    // must be ignored from now on.
    if !index.mem.docs.contains_key(&seq) && index.reader.contains(seq) {
        index.overridden.insert(seq);
    }
}

/// Only a plain secondary index materialises into the string-keyed
/// `SecondaryIndex` map. Vector/fulltext/sparse indexes are served by their own
/// search paths, and mirroring a 2048-dim vector into a per-row string key
/// would cost gigabytes on a large table.
fn is_secondary_kind(index_type: &str) -> bool {
    let t = index_type.trim();
    t.is_empty()
        || t.eq_ignore_ascii_case("SECONDARY")
        || t.eq_ignore_ascii_case("SECONDARYFUNCTIONAL")
}

/// Wide values of these columns are kept in the row sidecar instead of the KV
/// row (Gate C memory).
fn is_blob_type(t: &str) -> bool {
    is_embedding_type(t) || is_tensor_type(t) || is_sparse_type(t)
}

impl Engine {
    pub fn open(dir: impl AsRef<Path>) -> Result<Self> {
        Self::open_with_capacity(dir, 4)
    }

    pub fn open_with_capacity(dir: impl AsRef<Path>, exchange_capacity: usize) -> Result<Self> {
        let dir: PathBuf = dir.as_ref().to_path_buf();
        // The catalog lives at `<dir>/catalog`, the path the C++ engine uses for
        // its catalog RocksDB. Under `rocksdb-store` the C++ key space is the
        // **only** catalog (the native fjall key space is not written), so a
        // C++ engine can open this directory's catalog; otherwise the catalog
        // is a native fjall store.
        #[cfg(feature = "rocksdb-store")]
        let catalog = {
            let catalog_store: Arc<dyn MetaStore> =
                Arc::new(infinity_storage::RocksMetaStore::open(dir.join("catalog"))?);
            Catalog::with_store_cpp_only(catalog_store)
        };
        #[cfg(not(feature = "rocksdb-store"))]
        let catalog = Catalog::open(dir.join("catalog"))?;
        // The row/meta store: RocksDB when the `rocksdb-store` feature is on
        // (the substrate the C++ engine's catalog uses), fjall otherwise.
        #[cfg(feature = "rocksdb-store")]
        let store: Arc<dyn MetaStore> = Arc::new(infinity_storage::RocksMetaStore::open(
            dir.join("persistence"),
        )?);
        #[cfg(not(feature = "rocksdb-store"))]
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.join("persistence"), "default")?);
        let wal_dir = dir.join("wal");
        std::fs::create_dir_all(&wal_dir).map_err(|e| Error::Io {
            path: wal_dir.clone(),
            source: e,
        })?;
        let txn = TxnManager::open(wal_dir.join("wal.log"), store.clone())?;
        // Only replay what the KV store has not applied yet: a clean restart
        // must not resurrect rows that were deleted after they were logged.
        let applied = txn.applied_ts()?;
        txn.recover(applied)?;
        let rt = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .enable_all()
            .build()
            .map_err(|e| Error::Invalid(format!("tokio runtime: {e}")))?;
        let engine = Self {
            storage: Storage {
                catalog,
                store,
                flush_lock: Mutex::new(()),
            },
            txn,
            rt,
            context: Context {
                exchange_capacity: exchange_capacity.max(1),
                buffer: Mutex::new(infinity_storage::buffer::BufferManager::new(
                    8 * 1024 * 1024 * 1024,
                )),
                config: Mutex::new(HashMap::new()),
                memindex_quota: std::sync::atomic::AtomicU64::new(0),
                memindex_capacity: std::sync::atomic::AtomicUsize::new(64 * 1024),
                object_store: Mutex::new(None),
                hnsw_budget: std::sync::atomic::AtomicUsize::new(8 * 1024 * 1024 * 1024),
                scan_gate: ScanGate::new(
                    std::env::var("INFINITY_SCAN_CONCURRENCY")
                        .ok()
                        .and_then(|v| v.parse().ok())
                        .unwrap_or(2),
                ),
                query_count: std::sync::atomic::AtomicU64::new(0),
                connected_at: std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .map(|d| d.as_secs().to_string())
                    .unwrap_or_default(),
            },
            dir,
            result_cache: Mutex::new(HashMap::new()),
            current_db: Mutex::new("default_db".to_string()),
            tables: Mutex::new(HashMap::new()),
            batches_since_trim: std::sync::atomic::AtomicUsize::new(0),
            vector_cache: Mutex::new(HashMap::new()),
        };
        engine.recover_cpp_blocks()?;
        engine.rebuild_indexes()?;
        // If this directory was written by the C++ engine (its catalog keys are
        // in the catalog store, but the native catalog is empty), take over its
        // schema so the engine can open a C++-produced directory (§2.6.1).
        if engine.storage.catalog.list_tables()?.is_empty() && !engine.storage.catalog.is_cpp_only()
        {
            let _ = engine
                .storage
                .catalog
                .import_cpp_layout(engine.storage.catalog.store().as_ref());
        }
        Ok(engine)
    }

    /// Runs a derived-table subquery (`FROM (SELECT …)`), returning its columns
    /// and rows so the outer query can scan them.
    fn derived_table(&self, sql: &str) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        for r in self.execute(sql)? {
            if let QueryResult::Rows { columns, rows } = r {
                // `QueryResult` renders cells as text; the scan works on values.
                let rows = rows
                    .into_iter()
                    .map(|r| r.iter().map(|c| parse_value(c)).collect())
                    .collect();
                return Ok((columns, rows));
            }
        }
        Ok((Vec::new(), Vec::new()))
    }

    /// Best-effort database id for a (possibly qualified) table name.
    ///
    /// The catalog tracks table ids but not database ids, so the id is the
    /// database's position in the catalog's listing (`default_db` first), made
    /// 1-based to match the reference: `default_db` is `db_1`, the first user
    /// database `db_2`, and so on. Unqualified names resolve against
    /// `default_db`.
    fn database_id_of(&self, table: &str) -> u64 {
        let db = if table.contains('.') {
            table.split('.').next().unwrap_or("default_db")
        } else {
            "default_db"
        };
        self.storage
            .catalog
            .list_databases()
            .unwrap_or_default()
            .iter()
            .position(|d| d == db)
            .map(|p| p as u64 + 1)
            .unwrap_or(1)
    }

    /// Rebuilds all declared indexes from stored rows (called on open).
    fn rebuild_indexes(&self) -> Result<()> {
        // A new index may have appeared since the last write.
        for e in self.tables.lock().unwrap().values_mut() {
            e.hnsw_probed = false;
            // Sealed chunk readers are reloaded from the mirror below.
            e.vector.clear();
            e.ivf.clear();
            e.ivf_mem.clear();
        }
        for im in self.storage.catalog.list_indexes()? {
            // skip functional-index targets (not plain columns)
            if is_secondary_kind(&im.index_type)
                && self
                    .table_schema(&im.table)?
                    .iter()
                    .any(|c| c == &im.column)
            {
                self.build_index(&im.table, &im.column)?;
            }
            // A vector index's persisted HNSW graph, loaded so the search path
            // can walk it (best-effort: a table without a mirror just brute
            // forces).
            if im.index_type.eq_ignore_ascii_case("HNSW") {
                let segments = self.load_vector_indexes(&im.table, &im.name);
                if !segments.is_empty() {
                    let tkey = self.table_key(&im.table);
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .vector
                        .insert(
                            im.column.clone(),
                            segments.into_iter().map(Arc::new).collect(),
                        );
                }
            }
            if im.index_type.eq_ignore_ascii_case("IVF") {
                let segments = self.load_ivf_indexes(&im.table, &im.name);
                let tkey = self.table_key(&im.table);
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey).or_default();
                entry.ivf_mem.entry(im.column.clone()).or_default();
                if !segments.is_empty() {
                    entry.ivf.insert(
                        im.column.clone(),
                        segments.into_iter().map(Arc::new).collect(),
                    );
                }
            }
        }
        Ok(())
    }

    /// Writes one `keys + offsets + PGM` chunk per `ROW_ID` segment and installs
    /// the readers (the reference's high-cardinality `SaveIndexInner`).
    fn write_secondary_pgm_chunks(
        &self,
        table: &str,
        column: &str,
        by_segment: HashMap<u64, Vec<(u64, u32)>>,
    ) {
        let dir = self
            .dir
            .join("secondary")
            .join(index_key(table, column).replace('/', "_"));
        let mut readers = Vec::new();
        for (segment, mut entries) in by_segment {
            entries.sort_unstable_by_key(|(k, _)| *k);
            let path = dir.join(format!("seg_{segment}_{}.idx", entries.len()));
            // Always rewrite: the name does not capture the data, and a stale
            // chunk from an earlier run would shadow this one.
            if infinity_storage::secondary_segment::write_secondary_segment(&path, &entries)
                .is_err()
            {
                continue;
            }
            if let Ok(reader) =
                infinity_storage::secondary_segment::SecondarySegmentReader::open(&path)
            {
                readers.push((segment << 32, Arc::new(reader)));
            }
        }
        let tkey = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .secondary
            .disk
            .insert(column.to_string(), readers);
    }

    fn build_index(&self, table: &str, column: &str) -> Result<()> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::Invalid(format!("unknown column '{column}'")))?;
        // Every secondary column has a chunk form, so the build only reads the
        // indexed column (not the wide vectors in the same row): `index_lookup`
        // merges the chunks with the in-memory tail.
        let key = index_key(table, column);
        let tkey = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(tkey.clone())
            .or_default()
            .secondary
            .disk
            .remove(column);
        let type_upper = self
            .column_types(table)
            .ok()
            .and_then(|t| t.get(col_idx).cloned())
            .map(|t| t.trim().to_ascii_uppercase())
            .unwrap_or_default();
        let int_column = type_upper.starts_with("INT") || type_upper.starts_with("UINT");
        let is_bool = type_upper.starts_with("BOOL");
        if int_column {
            let mut by_segment: HashMap<u64, Vec<(u64, u32)>> = HashMap::new();
            for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                let Some(Value::Int(v)) = row.get(col_idx) else {
                    continue;
                };
                // Order-preserving mapping of i64 into u64.
                let ordered = (*v as u64) ^ (1u64 << 63);
                by_segment
                    .entry((seq as u64) >> 32)
                    .or_default()
                    .push((ordered, (seq as u64 & 0xffff_ffff) as u32));
            }
            self.write_secondary_pgm_chunks(table, column, by_segment);
        } else if !is_bool {
            // VARCHAR (and JSON) keys are hashed to u64, exactly like the
            // reference's `std::hash<std::string_view>` (`ConvertToOrderedKeyValue`).
            let mut by_segment: HashMap<u64, Vec<(u64, u32)>> = HashMap::new();
            for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                let Some(v) = row.get(col_idx) else {
                    continue;
                };
                let ordered = infinity_storage::secondary_segment::hash_varchar(&v.to_slt_string());
                by_segment
                    .entry((seq as u64) >> 32)
                    .or_default()
                    .push((ordered, (seq as u64 & 0xffff_ffff) as u32));
            }
            self.write_secondary_pgm_chunks(table, column, by_segment);
        }
        // Boolean columns use the low-cardinality variant.
        self.tables
            .lock()
            .unwrap()
            .entry(tkey.clone())
            .or_default()
            .secondary
            .low
            .remove(column);
        {
            if is_bool {
                let mut by_segment: HashMap<u64, [Vec<u32>; 2]> = HashMap::new();
                for (seq, row) in self.fetch_rows_with_ids_projected_all(table, col_idx)? {
                    let Some(v) = row.get(col_idx) else { continue };
                    let key = matches!(v, Value::Int(1)) || v.to_slt_string() == "true";
                    by_segment.entry((seq as u64) >> 32).or_default()[key as usize]
                        .push((seq as u64 & 0xffff_ffff) as u32);
                }
                let dir = self.dir.join("secondary_low").join(key.replace('/', "_"));
                let mut readers = Vec::new();
                for (segment, lists) in by_segment {
                    let mut entries: Vec<(u64, Vec<u32>)> = Vec::new();
                    for (bit, list) in lists.iter().enumerate() {
                        if !list.is_empty() {
                            entries.push((bit as u64, list.clone()));
                        }
                    }
                    entries.sort_by_key(|(k, _)| *k);
                    let path = dir.join(format!("seg_{segment}_{}.idx", entries.len()));
                    if infinity_storage::secondary_lowcard::write_lowcard_segment(&path, &entries)
                        .is_err()
                    {
                        continue;
                    }
                    if let Ok(reader) =
                        infinity_storage::secondary_lowcard::LowCardReader::open(&path)
                    {
                        readers.push((segment << 32, Arc::new(reader)));
                    }
                }
                self.tables
                    .lock()
                    .unwrap()
                    .entry(tkey.clone())
                    .or_default()
                    .secondary
                    .low
                    .insert(column.to_string(), readers);
            }
        }
        // The memory map is only the tail written since the last dump.
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        entry.secondary.disk_backed.insert(column.to_string());
        entry
            .secondary
            .indexes
            .insert(column.to_string(), SecondaryIndex::new());
        Ok(())
    }

    /// Reloads the HNSW/IVF chunk readers after new chunks were written, so a
    /// query in the same process sees what `flush()` just mirrored.
    fn reload_vector_indexes(&self) {
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        // The reference has one buffer budget shared by every index; give each
        // HNSW index an equal share so the total stays within it.
        let hnsw_count = indexes
            .iter()
            .filter(|im| im.index_type.eq_ignore_ascii_case("HNSW"))
            .count()
            .max(1);
        for im in indexes {
            if im.index_type.eq_ignore_ascii_case("HNSW") {
                // Reconcile with the live chunk set and the buffer budget:
                // cleanup deletes the chunks a merged graph replaced, a rewrite
                // under the same id changes the file stamp, and the decoded
                // graphs must stay within `[buffer] buffer_manager_size` (the
                // reference's buffer manager). Without this a long import grows
                // without bound. Partial coverage is safe: `knn_candidates`
                // falls back to the exact scan when labels do not cover a row.
                let stamps = self.hnsw_chunk_stamps(&im.table, &im.name);
                if stamps.is_empty() {
                    continue;
                }
                let budget = self
                    .context
                    .hnsw_budget
                    .load(std::sync::atomic::Ordering::Relaxed)
                    .max(1)
                    / hnsw_count;
                let tkey = self.table_key(&im.table);
                let (mut total, have, kept) = {
                    let mut tables = self.tables.lock().unwrap();
                    let list = tables
                        .entry(tkey.clone())
                        .or_default()
                        .vector
                        .entry(im.column.clone())
                        .or_default();
                    list.retain(|s| stamps.get(&s.chunk_id).is_some_and(|st| *st == s.stamp));
                    let total: usize = list.iter().map(|s| s.bytes).sum();
                    let have: std::collections::HashSet<u64> =
                        list.iter().map(|s| s.chunk_id).collect();
                    (total, have, list.len())
                };
                // Newest chunks first: they hold the rows written since the
                // last merge, which are the ones a fresh scan needs most.
                let mut missing: Vec<u64> = stamps
                    .keys()
                    .copied()
                    .filter(|id| !have.contains(id))
                    .collect();
                missing.sort_unstable_by(|a, b| b.cmp(a));
                let mut loaded = Vec::new();
                for id in missing {
                    if total >= budget && kept + loaded.len() > 0 {
                        break;
                    }
                    if let Some(index) = self.load_vector_index_chunk(&im.table, &im.name, id) {
                        total += index.bytes;
                        loaded.push(Arc::new(index));
                    }
                }
                if !loaded.is_empty() {
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .vector
                        .entry(im.column.clone())
                        .or_default()
                        .extend(loaded);
                }
                // Still over budget (or the budget shrank): evict the largest
                // decoded graphs, which frees the most memory.
                if total > budget {
                    let mut tables = self.tables.lock().unwrap();
                    let list = tables
                        .entry(self.table_key(&im.table))
                        .or_default()
                        .vector
                        .entry(im.column.clone())
                        .or_default();
                    list.sort_by_key(|s| std::cmp::Reverse(s.bytes));
                    while total > budget && list.len() > 1 {
                        let gone = list.pop().unwrap();
                        total = total.saturating_sub(gone.bytes);
                    }
                }
            } else if im.index_type.eq_ignore_ascii_case("IVF") {
                let segments = self.load_ivf_indexes(&im.table, &im.name);
                if !segments.is_empty() {
                    let tkey = self.table_key(&im.table);
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .ivf
                        .insert(
                            im.column.clone(),
                            segments.into_iter().map(Arc::new).collect(),
                        );
                }
            }
        }
    }

    /// Dumps the memory tail of every disk-backed secondary index into new
    /// chunks, so memory stays bounded by the rows written since the last dump.
    fn seal_secondary_indexes(&self) {
        let backed: Vec<(String, String)> = {
            let tables = self.tables.lock().unwrap();
            tables
                .iter()
                .flat_map(|(t, e)| {
                    e.secondary
                        .disk_backed
                        .iter()
                        .map(move |c| (t.clone(), c.clone()))
                })
                .collect()
        };
        if backed.is_empty() {
            return;
        }
        let Ok(catalog_indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for (tkey, column) in backed {
            let Some(im) = catalog_indexes
                .iter()
                .find(|im| self.table_key(&im.table) == tkey && im.column == column)
            else {
                continue;
            };
            let Ok(schema) = self.table_schema(&im.table) else {
                continue;
            };
            let Some(ci) = schema.iter().position(|c| c == &im.column) else {
                continue;
            };
            let Ok(types) = self.column_types(&im.table) else {
                continue;
            };
            let ty = types
                .get(ci)
                .map(|t| t.trim().to_ascii_uppercase())
                .unwrap_or_default();
            let is_bool = ty.starts_with("BOOL");
            let is_int = ty.starts_with("INT") || ty.starts_with("UINT");
            let key = index_key(&im.table, &column);
            let tail = match self
                .tables
                .lock()
                .unwrap()
                .get_mut(&tkey)
                .and_then(|e| e.secondary.indexes.get_mut(&column))
            {
                Some(idx) => idx.take(),
                None => continue,
            };
            if tail.is_empty() {
                continue;
            }
            if !is_bool {
                let mut by_segment: HashMap<u64, Vec<(u64, u32)>> = HashMap::new();
                for (raw_key, ids) in &tail {
                    let Some(text) = std::str::from_utf8(raw_key).ok() else {
                        continue;
                    };
                    let ordered = if is_int {
                        match text.trim().parse::<i64>() {
                            Ok(v) => (v as u64) ^ (1u64 << 63),
                            Err(_) => continue,
                        }
                    } else {
                        // VARCHAR/JSON: the reference hashes the key.
                        infinity_storage::secondary_segment::hash_varchar(text)
                    };
                    for id in ids {
                        by_segment
                            .entry(id >> 32)
                            .or_default()
                            .push((ordered, (*id & 0xffff_ffff) as u32));
                    }
                }
                let dir = self.dir.join("secondary").join(key.replace('/', "_"));
                let mut chunk = std::fs::read_dir(&dir)
                    .map(|d| {
                        d.filter_map(|e| e.ok())
                            .filter(|e| e.path().extension().is_some_and(|x| x == "idx"))
                            .count()
                    })
                    .unwrap_or(0);
                let mut readers = Vec::new();
                for (segment, mut entries) in by_segment {
                    entries.sort_unstable_by_key(|(k, _)| *k);
                    let path = dir.join(format!("seg_{segment}_tail{chunk}.idx"));
                    chunk += 1;
                    if infinity_storage::secondary_segment::write_secondary_segment(&path, &entries)
                        .is_err()
                    {
                        continue;
                    }
                    if let Ok(reader) =
                        infinity_storage::secondary_segment::SecondarySegmentReader::open(&path)
                    {
                        readers.push((segment << 32, Arc::new(reader)));
                    }
                }
                self.tables
                    .lock()
                    .unwrap()
                    .entry(tkey.clone())
                    .or_default()
                    .secondary
                    .disk
                    .entry(column.clone())
                    .or_default()
                    .extend(readers);
            } else {
                let mut by_segment: HashMap<u64, [Vec<u32>; 2]> = HashMap::new();
                for (raw_key, ids) in &tail {
                    let text = String::from_utf8_lossy(raw_key);
                    let bit = matches!(text.trim(), "true" | "1");
                    for id in ids {
                        by_segment.entry(id >> 32).or_default()[bit as usize]
                            .push((*id & 0xffff_ffff) as u32);
                    }
                }
                let dir = self.dir.join("secondary_low").join(key.replace('/', "_"));
                let mut chunk = std::fs::read_dir(&dir)
                    .map(|d| {
                        d.filter_map(|e| e.ok())
                            .filter(|e| e.path().extension().is_some_and(|x| x == "idx"))
                            .count()
                    })
                    .unwrap_or(0);
                let mut readers = Vec::new();
                for (segment, lists) in by_segment {
                    let mut entries: Vec<(u64, Vec<u32>)> = Vec::new();
                    for (bit, list) in lists.iter().enumerate() {
                        if !list.is_empty() {
                            entries.push((bit as u64, list.clone()));
                        }
                    }
                    entries.sort_by_key(|(k, _)| *k);
                    let path = dir.join(format!("seg_{segment}_tail{chunk}.idx"));
                    chunk += 1;
                    if infinity_storage::secondary_lowcard::write_lowcard_segment(&path, &entries)
                        .is_err()
                    {
                        continue;
                    }
                    if let Ok(reader) =
                        infinity_storage::secondary_lowcard::LowCardReader::open(&path)
                    {
                        readers.push((segment << 32, Arc::new(reader)));
                    }
                }
                self.tables
                    .lock()
                    .unwrap()
                    .entry(tkey)
                    .or_default()
                    .secondary
                    .low
                    .entry(column)
                    .or_default()
                    .extend(readers);
            }
        }
    }

    /// Total bytes buffered by every in-memory index form. The unified budget
    /// covers full-text, HNSW, sparse, multi-vector and secondary tails.
    fn memindex_bytes(&self) -> usize {
        let mut total = 0usize;
        for e in self.tables.lock().unwrap().values() {
            for idx in e.text.values() {
                if let Ok(g) = idx.read() {
                    total += g.mem.mem_used();
                }
            }
        }
        for e in self.tables.lock().unwrap().values() {
            for idx in e.sparse.values() {
                if let Ok(g) = idx.read() {
                    total += g.tail_bytes;
                }
            }
        }
        for e in self.tables.lock().unwrap().values() {
            for idx in e.mv.values() {
                if let Ok(g) = idx.read() {
                    total += g.tail_bytes;
                }
            }
        }
        // The HNSW memory segment (`HnswIndexInMem`) owns a graph per column;
        // the reference charges it against the same memindex budget.
        for e in self.tables.lock().unwrap().values() {
            for idx in e.hnsw_mem.values() {
                if let Ok(idx) = idx.lock() {
                    total += idx.mem_usage();
                }
            }
            total += e.index_pending_bytes.values().sum::<usize>();
        }
        total
            + self
                .tables
                .lock()
                .unwrap()
                .values()
                .flat_map(|e| e.secondary.indexes.values())
                .map(SecondaryIndex::approx_bytes)
                .sum::<usize>()
    }

    /// Dumps every memory index when the unified budget is exceeded, mirroring
    /// the reference's memindex dump. `memindex_memory_quota = 0` disables it.
    fn enforce_memindex_quota(&self) {
        let quota = self
            .context
            .memindex_quota
            .load(std::sync::atomic::Ordering::Relaxed) as usize;
        if quota == 0 || self.memindex_bytes() <= quota {
            return;
        }
        self.seal_multivector_indexes();
        self.seal_sparse_indexes();
        self.seal_secondary_indexes();
        // HNSW: dump each memory segment and release it.
        self.dump_hnsw_segments();
    }

    fn index_lookup(&self, table: &str, column: &str, value: &Literal) -> Option<Vec<u64>> {
        let tkey = self.table_key(table);
        let want = value.to_slt_string();
        // Snapshot the readers (cheap Arc clones) and the memory-tail hits so
        // the table lock is not held across the row lookup.
        let (disk, low, tail): (
            Option<SecondaryDiskChunks>,
            Option<SecondaryLowChunks>,
            Vec<u64>,
        ) = {
            let tables = self.tables.lock().unwrap();
            let reg = tables.get(&tkey).map(|e| &e.secondary)?;
            let known = reg.indexes.contains_key(column)
                || reg.disk.contains_key(column)
                || reg.low.contains_key(column);
            if !known {
                return None;
            }
            let tail = reg
                .indexes
                .get(column)
                .map(|idx| idx.lookup(want.as_bytes()).to_vec())
                .unwrap_or_default();
            (
                reg.disk.get(column).cloned(),
                reg.low.get(column).cloned(),
                tail,
            )
        };
        let schema = self.table_schema(table).unwrap_or_default();
        let col = schema.iter().position(|c| c == column);
        let mut out: Vec<u64> = Vec::new();
        let mut seen: std::collections::HashSet<u64> = Default::default();
        // Sealed chunks are authoritative for the rows they cover; the memory
        // map only holds the tail written since the last dump.
        if let Literal::Int(v) = value {
            let ordered = (*v as u64) ^ (1u64 << 63);
            if let Some(segments) = &disk {
                for (base, reader) in segments {
                    for off in reader.lookup(ordered) {
                        let id = base | u64::from(off);
                        if self.live_row_matches(table, id, col, &want) && seen.insert(id) {
                            out.push(id);
                        }
                    }
                }
            }
        } else if let Literal::Str(text) = value {
            let lower = text.trim().to_ascii_lowercase();
            let bit = if lower == "true" {
                Some(1u64)
            } else if lower == "false" {
                Some(0u64)
            } else {
                None
            };
            if let Some(bit) = bit {
                if let Some(segments) = &low {
                    for (base, reader) in segments {
                        for off in reader.lookup(bit) {
                            let id = base | u64::from(off);
                            if self.live_row_matches(table, id, col, &want) && seen.insert(id) {
                                out.push(id);
                            }
                        }
                    }
                }
            } else {
                // VARCHAR/JSON chunks store the reference's u64 key hash.
                let ordered = infinity_storage::secondary_segment::hash_varchar(&want);
                if let Some(segments) = &disk {
                    for (base, reader) in segments {
                        for off in reader.lookup(ordered) {
                            let id = base | u64::from(off);
                            if self.live_row_matches(table, id, col, &want) && seen.insert(id) {
                                out.push(id);
                            }
                        }
                    }
                }
            }
        }
        // The tail: rows inserted after the last dump.
        for id in tail {
            if self.live_row_matches(table, id, col, &want) && seen.insert(id) {
                out.push(id);
            }
        }
        out.sort_unstable();
        Some(out)
    }

    /// A chunk or tail entry is a hit only when the row is live and still holds
    /// the probed value (UPDATE re-appends, DROP COLUMN re-keys).
    fn live_row_matches(&self, table: &str, id: u64, col: Option<usize>, want: &str) -> bool {
        if self
            .storage
            .store
            .get(&del_key(table, id as usize))
            .ok()
            .flatten()
            .is_some()
        {
            return false;
        }
        let Some(raw) = self
            .storage
            .store
            .get(&row_key(table, id as usize))
            .ok()
            .flatten()
        else {
            return false;
        };
        match col {
            Some(ci) => {
                String::from_utf8_lossy(&raw)
                    .split('\t')
                    .nth(ci)
                    .map(str::trim)
                    == Some(want)
            }
            None => true,
        }
    }

    fn fetch_rows_by_ids(&self, table: &str, ids: &[u64]) -> Result<Vec<Vec<Value>>> {
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut rows = Vec::new();
        for &id in ids {
            if let Some(v) = self.storage.store.get(&row_key(table, id as usize))? {
                rows.push(self.parse_stored_row(table, &defaults, &v)?);
            }
        }
        Ok(rows)
    }

    pub fn execute(&self, sql: &str) -> Result<Vec<QueryResult>> {
        let mut out = Vec::new();
        for s in parse(sql)? {
            self.context
                .query_count
                .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
            // `USE <db>` switches the session database for later statements.
            if let Statement::Use { name } = &s {
                *self.current_db.lock().unwrap() = name.clone();
                out.push(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                });
                continue;
            }
            let qualified = self.qualify_tables(&s);
            // Result cache: `SELECT` results are reused verbatim when the cache
            // is active; any data-modifying statement invalidates it.
            if matches!(qualified, Statement::Select { .. }) && self.result_cache_active() {
                // Key on the statement, not the whole input: a multi-statement
                // string must not serve the first SELECT's rows for the second.
                let key = format!("{qualified:?}");
                if let Some(hit) = self.result_cache.lock().unwrap().get(&key).cloned() {
                    out.extend(hit);
                    continue;
                }
                let res = self.execute_stmt(&qualified)?;
                // Only small results are worth caching: retaining (and later
                // cloning on a hit) a whole-table `SELECT *` keeps hundreds of
                // megabytes to gigabytes alive.
                if cacheable_result(&res)
                    && self.result_cache.lock().unwrap().len() < self.result_cache_capacity()
                {
                    self.result_cache
                        .lock()
                        .unwrap()
                        .insert(key, vec![res.clone()]);
                }
                out.push(res);
                continue;
            }
            let mutating = invalidates_result_cache(&qualified);
            if mutating {
                self.result_cache.lock().unwrap().clear();
            }
            out.push(self.execute_stmt(&qualified)?);
            // Durability: a committed mutation must survive a crash, so both the
            // catalog and the row/meta KV are fsynced before acknowledging.
            if mutating {
                self.storage.catalog.flush()?;
                self.storage.store.flush()?;
            }
        }
        Ok(out)
    }

    /// The session's current database.
    fn db(&self) -> String {
        self.current_db.lock().unwrap().clone()
    }

    /// Resolves unqualified table names to `<current_db>.<name>`; a name that
    /// already contains a `.` is taken as fully qualified.
    fn qualify_tables(&self, stmt: &Statement) -> Statement {
        let db = self.db();
        // Identifiers are case-insensitive: the catalog keeps lower-case names,
        // so every table reference is normalised before it reaches it.
        let q = |t: &str| -> String {
            let t = t.to_ascii_lowercase();
            if t.is_empty() || t.contains('.') {
                t
            } else {
                format!("{db}.{t}")
            }
        };
        let mut s = stmt.clone();
        match &mut s {
            Statement::CreateTable { name, .. } | Statement::DropTable { name, .. } => {
                *name = q(name)
            }
            Statement::Insert { table, .. }
            | Statement::Delete { table, .. }
            | Statement::Update { table, .. }
            | Statement::AlterTableAdd { table, .. }
            | Statement::AlterTableDropColumn { table, .. }
            | Statement::ShowTableColumns { table }
            | Statement::ShowTableIndex { table, .. }
            | Statement::Copy { table, .. }
            | Statement::CreateIndex { table, .. }
            | Statement::CreateSnapshot { table, .. } => *table = q(table),
            Statement::RenameTable { table, new_name } => {
                *table = q(table);
                *new_name = q(new_name);
            }
            Statement::Select { from, join, .. } => {
                if let Some(t) = from.as_mut() {
                    // A derived table holds the subquery's SQL, not a table name.
                    // (A table may legitimately be called `select3`, so require the
                    // keyword and its trailing space.)
                    if !t.trim_start().to_ascii_uppercase().starts_with("SELECT ") {
                        *t = q(t);
                    }
                }
                if let Some(j) = join.as_mut() {
                    j.table = q(&j.table);
                }
            }
            Statement::Explain { inner, .. } => {
                let inner_q = self.qualify_tables(inner.as_ref());
                **inner = inner_q;
            }
            _ => {}
        }
        s
    }

    /// True when at least one index (secondary / fulltext / vector / sparse /
    /// multi-vector) is maintained in memory. An insert into a table with none
    /// skips the per-row index hook entirely.
    fn any_in_memory_index(&self) -> bool {
        if !self.vector_cache.lock().unwrap().is_empty() {
            return true;
        }
        self.tables.lock().unwrap().values().any(|e| {
            !e.text.is_empty()
                || !e.hnsw_mem.is_empty()
                || !e.secondary.indexes.is_empty()
                || !e.ivf_mem.is_empty()
                || !e.mv.is_empty()
                || !e.sparse.is_empty()
        })
    }

    /// True when `table` has an index declaration. Consulted together with
    /// [`Self::any_in_memory_index`] so the per-row hook runs whenever any
    /// index state could need it.
    fn has_index_declaration(&self, table: &str) -> bool {
        let short = table.split('.').next_back().unwrap_or(table);
        self.storage
            .catalog
            .list_indexes()
            .map(|is| is.iter().any(|i| i.table == short || i.table == table))
            .unwrap_or(true)
    }

    /// Appends a committed batch to the decoded-row cache, if the table has one.
    fn append_row_cache(&self, table: &str, base: usize, encoded: &[String]) {
        let key = self.table_key(table);
        if !self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.row_cache.is_some())
            .unwrap_or(false)
        {
            return;
        }
        // Decode through the same path as a scan: the stored text is the
        // canonical rendering (e.g. a FLOAT column renders `1.000000` even when
        // the literal was `1`).
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut decoded = Vec::with_capacity(encoded.len());
        for text in encoded {
            match self.parse_stored_row(table, &defaults, text.as_bytes()) {
                Ok(values) => decoded.push(values),
                Err(_) => return,
            }
        }
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&key) else {
            return;
        };
        let Some(cache) = entry.row_cache.as_mut() else {
            return;
        };
        for (k, values) in decoded.into_iter().enumerate() {
            cache.push(base + k, values);
        }
        if cache.bytes > ROW_CACHE_BUDGET_BYTES {
            entry.row_cache = None;
        }
    }

    /// Drops the decoded-row cache; every mutation that rewrites or removes
    /// rows calls it before the next scan.
    fn invalidate_row_cache(&self, table: &str) {
        let key = self.table_key(table);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.row_cache = None;
        }
    }

    /// Applies `delta` to the cached live-row count of `table`.
    fn note_live_rows(&self, table: &str, delta: isize) {
        let key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        if let Some(live) = entry.live_rows {
            entry.live_rows = Some(if delta >= 0 {
                live.saturating_add(delta as usize)
            } else {
                live.saturating_sub(delta.unsigned_abs())
            });
        }
    }

    /// The live-row count, using the incremental counter when available.
    fn live_row_count(&self, table: &str) -> Result<usize> {
        let key = self.table_key(table);
        if let Some(live) = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.live_rows)
        {
            return Ok(live);
        }
        let live = self.row_count(table)?;
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .live_rows = Some(live);
        Ok(live)
    }

    /// Marks the C++ block mirror stale after a schema or row-id rewrite.
    /// Block reads fall back to the KV row store until the next checkpoint
    /// rewrites the mirror.
    fn invalidate_block_mirror(&self, table: &str) {
        let key = self.table_key(table);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.cpp_block_rows = None;
            entry.block_index = None;
            entry.live_rows = None;
        }
    }

    /// The key of a table's [`TableEntry`]: the fully-qualified lower-case
    /// name (the C++ catalog's `db.table` key).
    fn table_key(&self, table: &str) -> String {
        if table.contains('.') {
            table.to_ascii_lowercase()
        } else {
            format!("{}.{}", self.db(), table.to_ascii_lowercase())
        }
    }

    /// Drops a table and purges its stored rows, tombstones, secondary-index
    /// entries and index declarations, so a later CREATE TABLE with the same
    /// name starts empty.
    fn drop_table_all(&self, name: &str) -> Result<bool> {
        let table_id = self.storage.catalog.get_table_by_name(name)?.map(|m| m.id);
        let key = self.table_key(name);
        self.tables.lock().unwrap().remove(&key);
        let existed = self.storage.catalog.drop_table(name)?;
        // The C++ block mirror of a dropped table is removed with it, so a
        // later restart cannot resurrect the data.
        if let Some(id) = table_id {
            let root = self.blocks_root();
            if let Ok(dbs) = std::fs::read_dir(&root) {
                for db in dbs.flatten() {
                    let dir = db.path().join(format!("tbl_{id}"));
                    if dir.is_dir() {
                        let _ = std::fs::remove_dir_all(dir);
                    }
                }
            }
        }
        for (k, _) in self.storage.store.scan_prefix(&row_prefix(name))? {
            self.storage.store.delete(&k)?;
        }
        for (k, _) in self.storage.store.scan_prefix(&del_prefix(name))? {
            self.storage.store.delete(&k)?;
        }
        // The table entry (index registries included) was removed above.
        for im in self.storage.catalog.list_indexes()? {
            if im.table == name {
                let _ = self.storage.catalog.drop_index_on(&im.table, &im.name);
            }
        }
        Ok(existed)
    }

    fn table_schema(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .columns)
    }

    /// The table's block map (C++ `BlockIndex`), opened lazily from the
    /// C++-format mirror's `version` files. The decoded blocks go through the
    /// engine-wide `Context::buffer` LRU.
    fn block_index_for(
        &self,
        table: &str,
    ) -> Option<Arc<Mutex<infinity_storage::block_index::BlockIndex>>> {
        let meta = self.storage.catalog.get_table_by_name(table).ok()??;
        let key = self.table_key(table);
        {
            let tables = self.tables.lock().unwrap();
            if let Some(index) = tables.get(&key).and_then(|e| e.block_index.clone()) {
                return Some(index);
            }
        }
        let db_id = self.database_id_of(table);
        let index =
            infinity_storage::block_index::BlockIndex::open(self.blocks_root(), db_id, meta.id)
                .ok()?;
        let shared = Arc::new(Mutex::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .block_index = Some(shared.clone());
        Some(shared)
    }

    /// Reserves a contiguous block of `n` row ids for `table`.
    fn reserve_rows(&self, table: &str, n: usize, new_segment: bool) -> Result<usize> {
        let key = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(key).or_default();
        let (mut seg, mut off, sealed) = match entry.row_counter {
            Some(v) => v,
            None => {
                let next = self.next_row_seq(table)?;
                ((next >> 32) as u32, (next & 0xffff_ffff) as u32, false)
            }
        };
        // The C++ row id is `segment_id << 32 | offset`. A `COPY` always seals
        // the segment it writes, and an `INSERT` that follows one opens a new
        // segment, while consecutive `INSERT`s keep appending to the same one —
        // which is why two `COPY`s of 99 rows yield ids 0…98 and
        // 4294967296…4294967394, but two one-row `INSERT`s yield 0 and 1.
        if (new_segment && off > 0) || (!new_segment && sealed) {
            seg += 1;
            off = 0;
        }
        let base = ((seg as usize) << 32) | off as usize;
        off = off.saturating_add(n as u32);
        entry.row_counter = Some((seg, off, new_segment));
        entry
            .segments
            .entry(seg)
            .and_modify(|s| s.rows = s.rows.max(off))
            .or_insert(SegmentEntry { id: seg, rows: off });
        Ok(base)
    }

    /// Forgets any cached row-id counter for `table` (used after an operation
    /// that re-keys rows outside `reserve_rows`).
    fn reset_row_counter(&self, table: &str) {
        let key = self.table_key(table);
        if let Some(entry) = self.tables.lock().unwrap().get_mut(&key) {
            entry.row_counter = None;
        }
    }

    /// Upper bound on the rows ever reserved for `table`, from the in-memory
    /// counter (no scan). Used to decide whether the C++ block mirror is worth
    /// its duplicate copy of a large table.
    fn approx_row_count(&self, table: &str) -> usize {
        let key = self.table_key(table);
        match self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.row_counter)
        {
            Some((seg, off, _)) => ((seg as u64) << 32 | off as u64) as usize,
            None => 0,
        }
    }

    /// Records a write batch (`COPY`/`INSERT`) against `table`.
    fn bump_writes(&self, table: &str) {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .write_batches += 1;
    }

    /// True when `table` has been written in more than one batch.
    fn multi_segment(&self, table: &str) -> bool {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.write_batches)
            .unwrap_or(0)
            >= 2
    }

    fn table_meta(&self, table: &str) -> Result<infinity_catalog::TableMeta> {
        self.storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))
    }

    /// `(column, index name)` pairs of `table`, for the EXPLAIN regex rewrite.
    /// The analyzer recorded for the index named `index_name` on `table`, i.e.
    /// the analyzer of the column that index covers.
    fn analyzer_of_index(&self, table: &str, index_name: &str) -> Option<String> {
        let key = self.table_key(table);
        if let Some(a) = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.analyzers_by_index.get(index_name).cloned())
        {
            return Some(a);
        }
        let short = table.split('.').next_back().unwrap_or(table);
        let column = self
            .storage
            .catalog
            .list_indexes()
            .ok()?
            .into_iter()
            .find(|im| im.name == index_name && (im.table == short || im.table == table))?
            .column;
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.analyzers.get(&column).cloned())
    }

    fn index_name_pairs(&self, table: &str) -> Result<Vec<(String, String)>> {
        let short = table.split('.').next_back().unwrap_or(table);
        Ok(self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|i| i.table == short || i.table == table)
            .map(|i| (i.column, i.name))
            .collect())
    }

    /// `(column, min_n, max_n, fold)` for every full-text index whose analyzer
    /// is a sparse gram analyzer. Only these indexes can consume the literal
    /// runs of a `regex()` predicate (C++ `SparseGramParams`).
    fn sparsegram_index_params(&self, table: &str) -> Result<Vec<(String, u32, u32, bool)>> {
        let short = table.split('.').next_back().unwrap_or(table);
        let mut out = Vec::new();
        for im in self.storage.catalog.list_indexes()? {
            if !(im.table == short || im.table == table)
                || !im.index_type.eq_ignore_ascii_case("FULLTEXT")
            {
                continue;
            }
            let Some(analyzer) = im
                .options
                .iter()
                .find(|(k, _)| k.eq_ignore_ascii_case("analyzer"))
                .map(|(_, v)| v)
            else {
                continue;
            };
            if let Some((min_n, max_n, fold)) =
                crate::regex_literal::parse_sparsegram_analyzer(analyzer)
            {
                out.push((im.column, min_n, max_n, fold));
            }
        }
        Ok(out)
    }

    /// Columns of `table` that carry any index, for the `EXPLAIN` index/leftover
    /// filter split.
    fn indexed_columns(&self, table: &str) -> Result<Vec<String>> {
        let short = table.split('.').next_back().unwrap_or(table);
        Ok(self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|i| i.table == short || i.table == table)
            .map(|i| i.column)
            .collect())
    }

    /// Inserts literal rows into `table`. The Thrift `Insert` RPC calls this
    /// directly: rebuilding SQL text for a 1000-row batch and re-parsing it cost
    /// more than the insert itself.
    pub fn insert_literals(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: Vec<Vec<Literal>>,
    ) -> Result<QueryResult> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let schema = meta.columns.clone();
        // An explicit column list must name existing columns; the C++
        // engine reports this as a syntax error.
        if let Some(cols) = columns {
            for c in cols {
                if !schema.iter().any(|s| s == c) {
                    return Err(Error::Invalid(format!(
                        "syntax: column '{c}' does not exist in table '{table}'"
                    )));
                }
            }
        }
        let mut rows = align_insert_rows(&schema, &meta.defaults, columns, &rows);
        // An integer column truncates a fractional value, as the
        // reference's implicit cast does (`-6.3` -> `-6`).
        for row in rows.iter_mut() {
            for (i, cell) in row.iter_mut().enumerate() {
                let int_col = meta
                    .types
                    .get(i)
                    .map(|t| {
                        let t = t.trim().to_ascii_uppercase();
                        t.starts_with("INT")
                            || t.starts_with("TINYINT")
                            || t.starts_with("SMALLINT")
                            || t.starts_with("BIGINT")
                    })
                    .unwrap_or(false);
                if !int_col {
                    continue;
                }
                if let Literal::Float(f) = cell {
                    *cell = Literal::Int(*f as i64);
                }
            }
        }
        // enforce NOT NULL
        for row in &rows {
            for (i, nn) in meta.not_nulls.iter().enumerate() {
                if *nn && matches!(row.get(i), Some(Literal::Null) | None) {
                    return Err(Error::Invalid(format!(
                        "null value in NOT NULL column '{}'",
                        schema.get(i).cloned().unwrap_or_default()
                    )));
                }
            }
        }
        if rows.is_empty() {
            return Ok(QueryResult::Inserted { count: 0 });
        }
        // Reserve a contiguous block of row ids: two sessions inserting
        // concurrently must not pick the same base sequence.
        let base = self.reserve_rows(table, rows.len(), false)?;
        let types = self.column_types(table)?;
        // tensor-like values must match the declared dimension, and
        // sparse values must be in range
        for row in &rows {
            for (i, cell) in row.iter().enumerate() {
                if let Some(t) = types.get(i) {
                    validate_insert_value(t, cell.to_slt_string().trim())?;
                }
            }
        }
        // A memory index that already exceeds its quota is dumped
        // before these rows join it.
        self.note_index_insert(table, rows.len());
        // A scalar table's first batch in this process starts an empty decoded
        // row cache; the batch is appended after the commit.
        {
            let key = self.table_key(table);
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key).or_default();
            if entry.row_cache.is_none() && entry.write_batches == 0 && scalar_only(&types) {
                entry.row_cache = Some(RowCache {
                    rows: Vec::new(),
                    bytes: 0,
                });
            }
        }
        // one transaction -> one WAL record + one KV batch (batched durability)
        let mut t = self.txn.begin();
        let mut encoded_rows: Vec<String> = Vec::with_capacity(rows.len());
        for (k, row) in rows.iter().enumerate() {
            let encoded = self.encode_row_stored(table, row, &types)?;
            t.put(row_key(table, base + k), encoded.as_bytes().to_vec());
            encoded_rows.push(encoded);
        }
        self.txn.commit(t)?;
        // With no index of any kind in memory there is nothing to maintain;
        // skipping the per-row hook avoids a catalog lookup + string render for
        // every inserted row (the reference's insert path has the same guard).
        if self.any_in_memory_index() || self.has_index_declaration(table) {
            for (k, row) in rows.iter().enumerate() {
                self.update_indexes_on_insert(table, base + k, row)?;
                let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
                self.update_text_indexes(table, base + k, &texts);
                self.update_vector_caches(table, base + k, &texts);
            }
            // C++ `AppendMemIndex`: a segment that reached
            // `mem_index_capacity` rows is dumped right after the batch.
            self.dump_hnsw_over_capacity(table);
        }
        self.bump_writes(table);
        self.note_live_rows(table, rows.len() as isize);
        self.append_row_cache(table, base, &encoded_rows);
        // Give the allocator's per-batch scratch back to the OS. Trimming on
        // every batch walks the whole heap; once every few batches is enough
        // to keep RSS flat and avoids adding milliseconds to each commit.
        self.batches_since_trim
            .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
        if self
            .batches_since_trim
            .load(std::sync::atomic::Ordering::Relaxed)
            >= 8
        {
            self.batches_since_trim
                .store(0, std::sync::atomic::Ordering::Relaxed);
            infinity_storage::trim_allocator();
        }
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Validates a delete set against the transaction's begin timestamp, then
    /// commits and records the tombstones' commit ts. Mirrors the reference's
    /// `ConflictCheckTest.conflict_check_delete`: the first committer wins and
    /// concurrent deleters get `kTxnConflict`.
    fn commit_with_deletes(&self, table: &str, deletes: &[usize], mut txn: Txn) -> Result<u64> {
        self.invalidate_row_cache(table);
        self.note_live_rows(table, -(deletes.len() as isize));
        // The block mirror keeps its snapshot: deleted row ids are hidden by
        // the tombstones written here, and rows an UPDATE re-inserted arrive
        // with fresh ids that the journal serves.
        for seq in deletes {
            txn.delete(del_key(table, *seq));
        }
        self.txn.commit(txn)
    }

    fn execute_stmt(&self, stmt: &Statement) -> Result<QueryResult> {
        let mut res = self.execute_stmt_inner(stmt)?;
        if let Statement::Select {
            highlight,
            search,
            from,
            ..
        } = stmt
        {
            if !highlight.is_empty() {
                self.apply_highlights(&mut res, highlight, search.as_deref(), from.as_deref());
            }
        }
        Ok(res)
    }

    /// `HIGHLIGHT col, …`: replaces each named output column with its text
    /// annotated by `<em>…</em>` around the query term occurrences, keeping only
    /// the sentences that contain a hit (`highlighter_impl.cpp`).
    fn apply_highlights(
        &self,
        res: &mut QueryResult,
        columns: &[String],
        search: Option<&SearchClause>,
        table: Option<&str>,
    ) {
        let QueryResult::Rows {
            columns: names,
            rows,
            ..
        } = res
        else {
            return;
        };
        let Some(query) = search.and_then(|s| {
            s.matches
                .iter()
                .find(|m| m.kind.eq_ignore_ascii_case("TEXT"))
                .and_then(|m| arg_str(m.args.first()))
        }) else {
            return;
        };
        let Some(table) = table else { return };
        for (i, name) in names.iter().enumerate() {
            if !columns.iter().any(|c| c == name) {
                continue;
            }
            let delims = self.index_delims(table, name);
            let qterms = tokenize_index_text(&query, &delims);
            for row in rows.iter_mut() {
                let Some(text) = row.get(i).cloned() else {
                    continue;
                };
                row[i] = highlight_text(&text, &qterms, &delims);
            }
        }
    }

    fn execute_stmt_inner(&self, stmt: &Statement) -> Result<QueryResult> {
        match stmt {
            Statement::CreateTable { name, columns } => {
                // A sparse column written as `SPARSE(elem, dim)` may declare 0
                // (an unknown bound); a negative one is invalid.
                for c in columns {
                    let up = c.type_text.to_ascii_uppercase();
                    if up.starts_with("SPARSE(") {
                        let inner =
                            c.type_text[c.type_text.find('(').unwrap() + 1..].trim_end_matches(')');
                        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
                        // Two spellings reach us: `SPARSE(elem, dim)` (the
                        // reference DDL) and `Sparse(elem, index_type, dim)` (the
                        // REST physical type). The three-part form's index type
                        // must be an integer width.
                        let (idx_ty, dim_part) = if parts.len() >= 3 {
                            (Some(parts[1]), parts[2])
                        } else {
                            (None, parts[1])
                        };
                        if let Some(idx_ty) = idx_ty {
                            let t = idx_ty.to_ascii_lowercase();
                            let int_like = [
                                "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32",
                                "uint64", "tinyint", "smallint", "int", "bigint",
                            ]
                            .contains(&t.as_str());
                            if !int_like {
                                return Err(Error::Invalid(format!(
                                    "invalid sparse index type '{idx_ty}' for column '{}'",
                                    c.name
                                )));
                            }
                        }
                        if parts.len() >= 2 {
                            let dim: i64 = dim_part.parse().unwrap_or(0);
                            // The reference rejects a zero bound at DDL time, but
                            // the SDK's `sparse,0,…` column type (physical type
                            // dimension 0) is accepted.
                            let allow_zero = ALLOW_ZERO_SPARSE.with(|c| c.get())
                                // the REST physical-type spelling declares an
                                // unknown bound as 0
                                || parts.len() >= 3;
                            if dim < 0 || (dim == 0 && !allow_zero) {
                                return Err(Error::Invalid(format!(
                                    "invalid sparse dimension {dim} for column '{}'",
                                    c.name
                                )));
                            }
                        }
                    }
                }
                // A vector-like column's declared dimension must be positive.
                for c in columns {
                    if !declares_dimension(&c.type_text) {
                        continue;
                    }
                    if let Some(d) = type_dim_deep(&c.type_text) {
                        let up = c.type_text.to_ascii_uppercase();
                        // A sparse column may declare a zero bound and fail at
                        // search time instead.
                        if up.starts_with("SPARSE(") {
                            continue;
                        }
                        let embedding = up.contains("EMBEDDING") || up.contains("VECTOR");
                        if d == 0 || (embedding && d > 16384) {
                            return Err(Error::Invalid(format!(
                                "invalid embedding dimension {d} for column '{}'",
                                c.name
                            )));
                        }
                    }
                }
                let names: Vec<String> = columns.iter().map(|c| c.name.clone()).collect();
                let defaults: Vec<Option<String>> = columns
                    .iter()
                    .map(|c| {
                        c.default
                            .as_ref()
                            .map(|l| normalize_default(&c.type_text, &l.to_slt_string()))
                    })
                    .collect();
                let not_nulls: Vec<bool> = columns.iter().map(|c| c.not_null).collect();
                let types: Vec<String> = columns.iter().map(|c| c.type_text.clone()).collect();
                let comments: Vec<Option<String>> =
                    columns.iter().map(|c| c.comment.clone()).collect();
                let t = self
                    .storage
                    .catalog
                    .create_table_typed(name, &names, &defaults, &not_nulls, &types, &comments)?;
                Ok(QueryResult::CreatedTable {
                    id: t.id,
                    name: t.name,
                })
            }
            Statement::RenameTable { table, new_name } => {
                let meta = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                if self.storage.catalog.get_table_by_name(new_name)?.is_some() {
                    return Err(Error::Invalid(format!("table '{new_name}' already exists")));
                }
                let _ = meta;
                let rows = self.fetch_rows_with_ids(table)?;
                let mut next = self.next_row_seq(new_name)?;
                // An empty table has nothing to move: committing an empty
                // transaction is an error, so only open one when needed.
                if !rows.is_empty() {
                    let mut t = self.txn.begin();
                    for (seq, row) in &rows {
                        // retire the old row, write it under the new table name
                        t.put(del_key(table, *seq), b"1".to_vec());
                        let encoded = row
                            .iter()
                            .map(|v| v.to_slt_string())
                            .collect::<Vec<_>>()
                            .join("\t");
                        t.put(row_key(new_name, next), encoded.into_bytes());
                        next += 1;
                    }
                    self.txn.commit(t)?;
                }
                for (k, _) in self.storage.store.scan_prefix(&row_prefix(table))? {
                    self.storage.store.delete(&k)?;
                }
                for (k, _) in self.storage.store.scan_prefix(&del_prefix(table))? {
                    self.storage.store.delete(&k)?;
                }
                // move the secondary-index declarations; the old table entry
                // (whose registries refer to the old row ids) is dropped and
                // rebuilt from the renamed rows below.
                let old_tkey = self.table_key(table);
                let pairs: Vec<String> = self
                    .tables
                    .lock()
                    .unwrap()
                    .remove(&old_tkey)
                    .map(|e| e.secondary.indexes.keys().cloned().collect())
                    .unwrap_or_default();
                for im in self.storage.catalog.list_indexes()? {
                    if im.table == *table {
                        let _ = self.storage.catalog.drop_index_on(&im.table, &im.name);
                        self.storage.catalog.create_index_typed(
                            &im.name,
                            new_name,
                            &im.column,
                            &im.index_type,
                            &im.options,
                        )?;
                    }
                }
                for column in &pairs {
                    let _ = self.build_index(new_name, column);
                }
                self.storage.catalog.rename_table(table, new_name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::AlterTableDropColumn { table, column } => {
                self.invalidate_row_cache(table);
                // Dropping a column that does not exist is `COLUMN_NOT_EXIST`.
                let schema = self.table_schema(table)?;
                if !schema.iter().any(|c| c == column) {
                    return Err(Error::NotFound(format!("column '{column}'")));
                }
                // an index still referencing the column blocks the drop
                if self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.table == *table && im.column == *column)
                {
                    return Err(Error::Invalid(format!(
                        "column '{column}' is used by an index"
                    )));
                }
                let rows = self.fetch_rows_with_ids(table)?;
                let idx = match self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .and_then(|m| m.columns.iter().position(|c| c == column))
                {
                    Some(i) => i,
                    None => return Err(Error::Invalid(format!("unknown column '{column}'"))),
                };
                // rewrite rows without the dropped column (an empty table has
                // nothing to rewrite, and committing an empty txn is an error)
                if !rows.is_empty() {
                    let mut t = self.txn.begin();
                    for (seq, _) in &rows {
                        t.put(del_key(table, *seq), b"1".to_vec());
                    }
                    let mut next = self.next_row_seq(table)?;
                    for (_, row) in &rows {
                        let mut nr = row.clone();
                        if idx < nr.len() {
                            nr.remove(idx);
                        }
                        let encoded = nr
                            .iter()
                            .map(|v| v.to_slt_string())
                            .collect::<Vec<_>>()
                            .join("\t");
                        t.put(row_key(table, next), encoded.into_bytes());
                        next += 1;
                    }
                    self.txn.commit(t)?;
                }
                self.reset_row_counter(table);
                // The rewrite moved every row to a new sequence id.
                self.invalidate_row_cache(table);
                self.invalidate_block_mirror(table);
                self.storage.catalog.drop_column(table, column)?;
                if let Some(im) = self
                    .storage
                    .catalog
                    .list_indexes()?
                    .into_iter()
                    .find(|i| i.table == *table && i.column == *column)
                {
                    let _ = self.storage.catalog.drop_index_on(&im.table, &im.name);
                }
                let tkey = self.table_key(table);
                self.tables
                    .lock()
                    .unwrap()
                    .entry(tkey)
                    .or_default()
                    .secondary
                    .indexes
                    .remove(column);
                // remaining indexes now hold stale row ids: rebuild them
                let remaining: Vec<String> = self
                    .storage
                    .catalog
                    .list_indexes()?
                    .into_iter()
                    .filter(|i| i.table == *table && is_secondary_kind(&i.index_type))
                    .map(|i| i.column)
                    .collect();
                for col in remaining {
                    self.build_index(table, &col)?;
                }
                Ok(QueryResult::CreatedTable {
                    id: 0,
                    name: table.clone(),
                })
            }
            Statement::AlterTableAdd { table, columns } => {
                // Rows cached before the new columns lack their defaults.
                self.invalidate_row_cache(table);
                self.invalidate_block_mirror(table);
                // A column may not be added twice (`DUPLICATE_COLUMN_NAME`).
                let existing = self.table_schema(table)?;
                let mut seen: Vec<String> = existing.clone();
                for c in columns {
                    if seen.iter().any(|n| n.eq_ignore_ascii_case(&c.name)) {
                        return Err(Error::Invalid(format!("duplicate column: '{}'", c.name)));
                    }
                    seen.push(c.name.clone());
                }
                // `ADD COLUMN … NOT NULL` requires a default for existing rows.
                for c in columns {
                    if c.not_null && c.default.is_none() {
                        return Err(Error::Invalid(format!(
                            "column '{}' is NOT NULL but has no default",
                            c.name
                        )));
                    }
                }
                let names: Vec<String> = columns.iter().map(|c| c.name.clone()).collect();
                let defaults: Vec<Option<String>> = columns
                    .iter()
                    .map(|c| c.default.as_ref().map(|l| l.to_slt_string()))
                    .collect();
                let not_nulls: Vec<bool> = columns.iter().map(|c| c.not_null).collect();
                let types: Vec<String> = columns.iter().map(|c| c.type_text.clone()).collect();
                self.storage.catalog.add_columns_typed(
                    table,
                    &names,
                    &defaults,
                    &not_nulls,
                    &types,
                    &vec![None; names.len()],
                )?;
                Ok(QueryResult::CreatedTable {
                    id: 0,
                    name: table.clone(),
                })
            }
            // `USE` is handled by `execute`; reaching here is a no-op.
            Statement::Use { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::CreateDatabase {
                name,
                if_not_exists,
            } => {
                self.storage.catalog.create_database(name, *if_not_exists)?;
                Ok(QueryResult::CreatedDatabase { name: name.clone() })
            }
            Statement::DropDatabase { name, if_exists } => {
                // The default database backs every session, so the C++ engine
                // refuses to drop it (3033 DROPPING_USING_DB).
                if name.eq_ignore_ascii_case("default_db") {
                    // `IF EXISTS` tolerates it (a no-op); a plain drop is 3033.
                    if *if_exists {
                        return Ok(QueryResult::DroppedDatabase { name: name.clone() });
                    }
                    return Err(Error::Invalid(
                        "dropping the current database is not allowed".into(),
                    ));
                }
                let existed = self.storage.catalog.drop_database(name, *if_exists)?;
                if existed || *if_exists {
                    // drop every table registered under this database
                    let prefix = format!("{name}.");
                    for t in self.storage.catalog.list_tables()? {
                        if t.name.starts_with(&prefix) {
                            self.drop_table_all(&t.name)?;
                        }
                    }
                }
                Ok(QueryResult::DroppedDatabase { name: name.clone() })
            }
            Statement::Show { text } => {
                let mut words = text.split_ascii_whitespace();
                match words.next().map(str::to_ascii_lowercase).as_deref() {
                    Some("databases") => self.show_databases(),
                    Some("tables") => {
                        let db = match (words.next(), words.next()) {
                            (Some(kw), Some(db)) if kw.eq_ignore_ascii_case("from") => Some(db),
                            _ => None,
                        };
                        self.show_tables(db)
                    }
                    _ => Ok(QueryResult::Rows {
                        columns: vec![],
                        rows: vec![],
                    }),
                }
            }
            Statement::ShowTableIndex { table, index } => {
                if !self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == *index && im.table == *table)
                {
                    return Err(Error::Invalid(format!(
                        "index '{index}' does not exist on '{table}'"
                    )));
                }
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            // EXPLAIN only describes a plan; it must not execute the statement,
            // but binding the plan surfaces a missing relation.
            Statement::Explain { kind, inner } => {
                let logical = matches!(
                    kind,
                    infinity_parser::ExplainKind::Logical
                        | infinity_parser::ExplainKind::UnOpt
                        | infinity_parser::ExplainKind::Opt
                );
                // `EXPLAIN AST` prints the parsed statement, so it has no plan
                // to build (and must not bind the relation).
                let text = if matches!(kind, infinity_parser::ExplainKind::Ast) {
                    Some(vec![format!("{inner:?}")])
                } else {
                    match inner.as_ref() {
                        Statement::Select {
                            items,
                            from: Some(t),
                            join: Some(j),
                            where_pred,
                            search,
                            ..
                        } if logical && search.is_none() => {
                            Some(crate::explain::explain_logical_join(
                                items,
                                t,
                                &j.table,
                                j,
                                where_pred.as_ref(),
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            search,
                            ..
                        } if logical
                            && join.is_none()
                            && search.is_none()
                            && crate::explain::has_aggregate(items) =>
                        {
                            crate::explain::explain_logical_agg(items, t, group_by)
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            where_pred,
                            search,
                            ..
                        } if !logical
                            && join.is_none()
                            && search.is_none()
                            && group_by.is_empty()
                            && crate::explain::is_count_star_only(items) =>
                        {
                            Some(crate::explain::explain_count_star(
                                t,
                                where_pred.as_ref(),
                                self.multi_segment(t),
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            order_by,
                            search,
                            ..
                        } if join.is_none()
                            && search.is_none()
                            && crate::explain::is_distinct_agg_plan(items, group_by) =>
                        {
                            Some(crate::explain::explain_distinct_agg(
                                &crate::explain::DistinctAggPlan {
                                    items,
                                    table: t,
                                    group_by,
                                    order_by,
                                },
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            order_by,
                            search,
                            ..
                        } if join.is_none()
                            && search.is_none()
                            && crate::explain::has_aggregate(items)
                            && crate::explain::aggregates_renderable(items) =>
                        {
                            Some(crate::explain::explain_hash_agg(
                                &crate::explain::HashAggPlan {
                                    items,
                                    table: t,
                                    group_by,
                                    order_by,
                                },
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            search: Some(s),
                            where_pred,
                            limit,
                            offset,
                            ..
                        } if join.is_none() => {
                            let meta = self.table_meta(t)?;
                            let indexed = self.indexed_columns(t)?;
                            let plan = crate::explain_search::SearchPlan {
                                items,
                                table: t,
                                search: s,
                                limit: *limit,
                                offset: *offset,
                                where_pred: where_pred.as_ref(),
                                schema: &meta.columns,
                                types: &meta.types,
                                indexed: &indexed,
                                multi: self.multi_segment(t),
                            };
                            if matches!(kind, infinity_parser::ExplainKind::Fragment) {
                                crate::explain_search::fragment_lines(&plan)
                            } else {
                                crate::explain_search::explain_search(&plan)
                            }
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            where_pred,
                            group_by,
                            order_by,
                            distinct,
                            search,
                            limit,
                            ..
                        } if join.is_none()
                            && group_by.is_empty()
                            && !crate::explain::has_aggregate(items)
                            && search.is_none() =>
                        {
                            let meta = self.table_meta(t)?;
                            let schema = &meta.columns;
                            // Try to push the filter into an index scan first.
                            let index_plan = match where_pred.as_ref() {
                                Some(w) if !logical && !*distinct => {
                                    let indexed = self.indexed_columns(t)?;
                                    let index_names = self.index_name_pairs(t)?;
                                    let sparsegram = self.sparsegram_index_params(t)?;
                                    crate::explain::explain_index_scan(
                                        &crate::explain::IndexScanPlan {
                                            items,
                                            table: t,
                                            where_pred: w,
                                            order_by,
                                            limit: *limit,
                                            schema,
                                            types: &meta.types,
                                            indexed: &indexed,
                                            index_names: &index_names,
                                            sparsegram: &sparsegram,
                                        },
                                    )
                                }
                                _ => None,
                            };
                            Some(index_plan.unwrap_or_else(|| {
                                crate::explain::explain_select(&crate::explain::SelectPlan {
                                    items,
                                    table: t,
                                    where_pred: where_pred.as_ref(),
                                    order_by,
                                    distinct: *distinct,
                                    schema,
                                    logical,
                                })
                            }))
                        }
                        Statement::Delete { table, where_pred } if logical => Some(
                            crate::explain::explain_logical_delete(table, where_pred.as_ref()),
                        ),
                        Statement::CreateTable { name, columns } => {
                            let (db, tbl) = match name.split_once('.') {
                                Some((d, t)) => (d.to_string(), t.to_string()),
                                None => ("default_db".to_string(), name.clone()),
                            };
                            let cols: Vec<(String, String, Option<String>)> = columns
                                .iter()
                                .map(|c| {
                                    (
                                        c.name.clone(),
                                        describe_type(&c.type_text),
                                        c.default.as_ref().map(|l| l.to_slt_string()),
                                    )
                                })
                                .collect();
                            Some(crate::explain::explain_create_table(&db, &tbl, &cols))
                        }
                        _ => None,
                    }
                };
                match text {
                    Some(lines) => Ok(QueryResult::Rows {
                        columns: vec![],
                        rows: lines.into_iter().map(|l| vec![l]).collect(),
                    }),
                    None => {
                        if let Statement::Select { from: Some(t), .. } = inner.as_ref() {
                            self.table_schema(t)?;
                        }
                        Ok(QueryResult::Rows {
                            columns: vec![],
                            rows: vec![],
                        })
                    }
                }
            }
            Statement::Copy {
                table,
                from,
                path,
                opts,
            } => {
                if *from {
                    let r = self.copy_from(table, path, opts);
                    if r.is_ok() {
                        self.bump_writes(table);
                    }
                    r
                } else {
                    self.copy_to(table, path, opts)
                }
            }
            Statement::CreateIndex {
                name,
                table,
                column,
                index_type,
                opts,
                if_not_exists,
            } => {
                self.validate_index(table, column, index_type, opts)?;
                let duplicate = self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == *name && im.table == *table);
                if duplicate && !*if_not_exists {
                    return Err(Error::Invalid(format!("index '{name}' already exists")));
                }
                self.storage
                    .catalog
                    .create_index_typed(name, table, column, index_type, opts)?;
                // remember the analyzer so fulltext matching can tokenise compatibly
                if let Some((_, a)) = opts.iter().find(|(k, _)| k == "ANALYZER") {
                    let a = a
                        .trim()
                        .trim_matches(|c| c == '\'' || c == '"')
                        .to_ascii_lowercase();
                    let key = self.table_key(table);
                    let mut tables = self.tables.lock().unwrap();
                    let entry = tables.entry(key).or_default();
                    entry.analyzers.insert(column.clone(), a.clone());
                    entry.analyzers_by_index.insert(name.clone(), a);
                }
                // a functional-index target is not a plain column, so there is
                // no materialised index to build
                if is_secondary_kind(index_type)
                    && self.table_schema(table)?.iter().any(|c| c == column)
                {
                    self.build_index(table, column)?;
                }
                // A multi-vector index is materialised when it is created, so
                // queries only read segment files; later inserts go to the
                // memory tail and `flush()` dumps it.
                if index_type.eq_ignore_ascii_case("EMVB")
                    || index_type.eq_ignore_ascii_case("PLAID")
                    || index_type.eq_ignore_ascii_case("SMVE")
                {
                    let dims = self
                        .table_schema(table)
                        .ok()
                        .and_then(|s| s.iter().position(|c| c == column))
                        .and_then(|i| self.column_types(table).ok()?.get(i).cloned())
                        .and_then(|ty| type_dim(&ty));
                    if let Some(dims) = dims {
                        let _ = self.multivector_index_for(table, column, dims);
                    }
                }
                // The sparse BMP index is materialised the same way.
                if index_type.eq_ignore_ascii_case("BMP") {
                    let width = self
                        .table_schema(table)
                        .ok()
                        .and_then(|s| s.iter().position(|c| c == column))
                        .and_then(|i| self.column_types(table).ok()?.get(i).cloned())
                        .map(|ty| sparse_idx_width(&ty));
                    if let Some(width) = width {
                        let _ = self.sparse_index_for(table, column, width);
                    }
                }
                // An IVF index keeps a memory segment for rows written before
                // the next mirror dump.
                if index_type.eq_ignore_ascii_case("IVF") {
                    let tkey = self.table_key(table);
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .ivf_mem
                        .entry(column.clone())
                        .or_default();
                }
                Ok(QueryResult::CreatedIndex {
                    name: name.clone(),
                    table: table.clone(),
                    column: column.clone(),
                })
            }
            Statement::DropTable { name, if_exists } => {
                let existed = self.drop_table_all(name)?;
                if existed {
                    self.drop_text_index_storage(name, None);
                }
                if !existed && !*if_exists {
                    return Err(Error::Invalid(format!("table '{name}' does not exist")));
                }
                Ok(QueryResult::DroppedTable {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::DropIndex { name, if_exists } => {
                let target = self
                    .storage
                    .catalog
                    .list_indexes()
                    .unwrap_or_default()
                    .into_iter()
                    .find(|im| im.name == *name)
                    .map(|im| (im.table, im.column, im.index_type));
                let was_fulltext = target
                    .as_ref()
                    .map(|(_, _, t)| t.eq_ignore_ascii_case("FULLTEXT"))
                    .unwrap_or(false);
                let existed = self.storage.catalog.drop_index(name)?;
                if existed {
                    if let Some((table, column, _)) = target.filter(|_| was_fulltext) {
                        self.drop_text_index_storage(&table, Some(&column));
                    }
                }
                if !existed && !*if_exists {
                    return Err(Error::Invalid(format!("index '{name}' does not exist")));
                }
                Ok(QueryResult::DroppedIndex {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::Utility { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::SetConfig { key, value } => {
                // a known key with an out-of-range/malformed value is rejected
                if let Some(msg) = invalid_config_value(key, value) {
                    return Err(Error::Invalid(msg));
                }
                // cache knobs cannot be changed while the cache is off
                if key.to_ascii_lowercase().starts_with("cache_result")
                    && value != "off"
                    && !self.result_cache_enabled()
                {
                    return Err(Error::Invalid(format!(
                        "config '{key}' is unavailable while result_cache is off"
                    )));
                }
                self.context
                    .config
                    .lock()
                    .unwrap()
                    .insert(key.to_ascii_lowercase(), value.clone());
                // `SET CONFIG result_cache "clear"` empties the cache.
                if key.eq_ignore_ascii_case("result_cache") && value.eq_ignore_ascii_case("clear") {
                    self.result_cache.lock().unwrap().clear();
                }
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::ShowVariable { key } => {
                let cache_var = key.to_ascii_lowercase();
                if cache_var.starts_with("cache_result") && !self.result_cache_enabled() {
                    return Err(Error::Invalid(format!(
                        "variable '{key}' is unavailable while result_cache is off"
                    )));
                }
                let v = if cache_var == "cache_result_num" {
                    self.result_cache.lock().unwrap().len().to_string()
                } else {
                    self.context
                        .config
                        .lock()
                        .unwrap()
                        .get(&key.to_ascii_lowercase())
                        .cloned()
                        .unwrap_or_else(|| "0".to_string())
                };
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![vec![v]],
                })
            }
            Statement::ShowTableColumns { table } => {
                let meta = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                let rows: Vec<Vec<String>> = (0..meta.columns.len())
                    .map(|i| {
                        let ty = meta.types.get(i).cloned().unwrap_or_default();
                        vec![
                            meta.columns[i].clone(),
                            describe_type(&ty),
                            render_column_default(
                                meta.defaults.get(i).and_then(|d| d.as_ref()),
                                &ty,
                            ),
                            meta.comments
                                .get(i)
                                .cloned()
                                .flatten()
                                .unwrap_or_else(|| "(empty)".to_string()),
                        ]
                    })
                    .collect();
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows,
                })
            }
            Statement::CreateSnapshot { name, table } => {
                let blob = self.snapshot_table_blob(table)?;
                self.storage.store.put(&snap_key(name), blob.as_bytes())?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::DropSnapshot { name } => {
                if self.storage.store.get(&snap_key(name))?.is_none() {
                    return Err(Error::NotFound(format!("snapshot '{name}'")));
                }
                self.storage.store.delete(&snap_key(name))?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::RestoreSnapshot { name } => {
                let data = self
                    .storage
                    .store
                    .get(&snap_key(name))?
                    .ok_or_else(|| Error::NotFound(format!("snapshot '{name}'")))?;
                let blob = String::from_utf8_lossy(&data).into_owned();
                // Restoring into an existing table is refused, as in the
                // reference (`DROP TABLE` first).
                let table = blob.split(SNAP_REC).next().unwrap_or_default().to_string();
                if !table.is_empty() && self.storage.catalog.get_table_by_name(&table)?.is_some() {
                    return Err(Error::Invalid(format!("table '{table}' already exists")));
                }
                self.restore_table_blob(&blob)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::Insert {
                table,
                columns,
                rows,
            } => self.insert_literals(table, columns.as_deref(), rows.clone()),
            Statement::Delete { table, where_pred } => {
                let schema = self.table_schema(table)?;
                let pred = match where_pred {
                    Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                for (seq, row) in self.rows_for_predicate(table, where_pred.as_ref())? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        to_del.push(seq);
                    }
                }
                if !to_del.is_empty() {
                    let t = self.txn.begin();
                    self.commit_with_deletes(table, &to_del, t)?;
                    self.drop_text_index_rows(table, &to_del);
                    self.drop_vector_cache_rows(table, &to_del);
                }
                Ok(QueryResult::Deleted {
                    count: to_del.len(),
                })
            }
            Statement::Update {
                table,
                assignments,
                where_pred,
            } => {
                // The reference requires a filter on UPDATE.
                if where_pred.is_none() {
                    return Err(Error::Invalid(
                        "syntax: UPDATE requires a WHERE clause".into(),
                    ));
                }
                let schema = self.table_schema(table)?;
                let types = self.column_types(table)?;
                let not_nulls = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .map(|m| m.not_nulls)
                    .unwrap_or_default();
                let mut asg = Vec::new();
                for (c, e) in assignments {
                    let idx = infinity_planner::resolve_column(&schema, c)?;
                    // A boolean column only takes true/false, not 1/0.
                    let ty = types
                        .get(idx)
                        .map(|t| t.to_ascii_uppercase())
                        .unwrap_or_default();
                    if ty.starts_with("BOOLEAN") || ty.starts_with("BOOL") {
                        if let infinity_parser::ExprAst::Literal(l) = e {
                            use infinity_parser::Literal as L;
                            if matches!(l, L::Int(_) | L::Float(_)) {
                                return Err(Error::Invalid(
                                    "parser: Boolean type should be true or false".into(),
                                ));
                            }
                        }
                    }
                    // An array value cannot be stored in a scalar column.
                    if matches!(
                        e,
                        infinity_parser::ExprAst::Literal(infinity_parser::Literal::Array(_))
                    ) {
                        let structured = ty.starts_with("ARRAY")
                            || ty.starts_with("VECTOR")
                            || ty.starts_with("EMBEDDING")
                            || ty.starts_with("TENSOR")
                            || ty.starts_with("MULTIVECTOR")
                            || ty.starts_with("SPARSE")
                            || ty.starts_with("JSON")
                            || ty.starts_with("VARCHAR")
                            || ty.starts_with("TEXT");
                        if !structured {
                            return Err(Error::Invalid(format!(
                                "not supported type conversion: an array value cannot be stored in column '{c}'"
                            )));
                        }
                    }
                    if not_nulls.get(idx).copied().unwrap_or(false)
                        && matches!(
                            e,
                            infinity_parser::ExprAst::Literal(infinity_parser::Literal::Null)
                        )
                    {
                        return Err(Error::Invalid(format!(
                            "null value in NOT NULL column '{c}'"
                        )));
                    }
                    asg.push((idx, infinity_planner::compile_expr(e, &schema)?));
                }
                let pred = match where_pred {
                    Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                let mut new_rows = Vec::new();
                for (seq, row) in self.rows_for_predicate(table, where_pred.as_ref())? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        let mut nr = row.clone();
                        for (idx, expr) in &asg {
                            if *idx < nr.len() {
                                nr[*idx] = expr.eval(&row);
                            }
                        }
                        to_del.push(seq);
                        new_rows.push(nr);
                    }
                }
                if !to_del.is_empty() {
                    // The reference deletes the old row and appends the
                    // rewritten one, so an updated row moves to the end and
                    // gets a fresh ROW_ID (`test_update` asserts that order).
                    let base = self.reserve_rows(table, new_rows.len(), false)?;
                    let mut t = self.txn.begin();
                    for (k, nr) in new_rows.iter().enumerate() {
                        let encoded = nr
                            .iter()
                            .map(|v| v.to_slt_string())
                            .collect::<Vec<_>>()
                            .join("\t");
                        t.put(row_key(table, base + k), encoded.into_bytes());
                    }
                    self.commit_with_deletes(table, &to_del, t)?;
                    for (k, nr) in new_rows.iter().enumerate() {
                        let texts: Vec<String> = nr.iter().map(|v| v.to_slt_string()).collect();
                        self.update_indexes_on_insert_text(table, base + k, &texts)?;
                        self.update_text_indexes(table, base + k, &texts);
                        self.update_vector_caches(table, base + k, &texts);
                    }
                    self.drop_text_index_rows(table, &to_del);
                    self.drop_vector_cache_rows(table, &to_del);
                }
                Ok(QueryResult::Updated {
                    count: to_del.len(),
                })
            }
            Statement::Select {
                items,
                from,
                join,
                where_pred,
                group_by,
                aliases,
                having,
                order_by,
                limit,
                offset,
                search,
                distinct,
                highlight: _,
            } => {
                // `SELECT count(*)` with no other clause is answered from the key
                // counts alone; the generic path materialises every row.
                if join.is_none()
                    && where_pred.is_none()
                    && group_by.is_empty()
                    && having.is_none()
                    && order_by.is_empty()
                    && limit.is_none()
                    && offset.is_none()
                    && search.is_none()
                    && !*distinct
                    && items.len() == 1
                    && aliases.len() <= 1
                {
                    if let infinity_parser::SelectItem::Aggregate(a) = &items[0] {
                        // `AggFunc` is not re-exported; the debug name is stable.
                        if format!("{:?}", a.func) == "Count" && a.args.is_empty() && !a.distinct {
                            if let Some(t) = from.as_ref() {
                                let n = self.row_count(t)?;
                                let name = aliases
                                    .first()
                                    .cloned()
                                    .flatten()
                                    .unwrap_or_else(|| "count(star)".to_string());
                                return Ok(QueryResult::Rows {
                                    columns: vec![name],
                                    rows: vec![vec![n.to_string()]],
                                });
                            }
                        }
                    }
                }
                // `SELECT … UNNEST(col) …`: expand an array/JSON column into one
                // row per element, then let the normal planner handle WHERE,
                // GROUP BY and aggregates over the expanded relation.
                let mut where_pred = where_pred.clone();
                let mut group_by = group_by.to_vec();
                let mut having = having.clone();
                let mut items_owned: Vec<SelectItem> = items.to_vec();
                let mut unnest_expansion: Option<(String, Vec<Vec<Value>>)> = None;
                let unnest_idx = items_owned.iter().position(|it| {
                    matches!(it, SelectItem::Func { name, .. }
                        if name.eq_ignore_ascii_case("UNNEST"))
                });
                if let (Some(t), Some(ui)) = (from.as_ref(), unnest_idx) {
                    let (arg_col, alias) = match &items_owned[ui] {
                        SelectItem::Func { args, .. } => (
                            match args.first() {
                                Some(ExprAst::Column(c)) => Some(c.clone()),
                                _ => None,
                            },
                            aliases.get(ui).cloned().flatten(),
                        ),
                        _ => (None, None),
                    };
                    if let Some(col) = arg_col {
                        let schema = self.table_schema(t)?;
                        if let Some(ci) = schema.iter().position(|c| c == &col) {
                            let col_ty = self.column_types(t)?.get(ci).cloned().unwrap_or_default();
                            let float_elems = array_elem_is_float(&col_ty);
                            let is_json_col = is_json_type(&col_ty);
                            let new_name = alias.unwrap_or_else(|| format!("__unnest_{col}"));
                            let mut out: Vec<Vec<Value>> = Vec::new();
                            for (_seq, row) in self.fetch_rows_with_ids(t)? {
                                let mut elements: Vec<Value> = Vec::new();
                                // a JSON column's stored text may have been read as
                                // a number, so re-render it before parsing
                                let cell: Option<String> = match row.get(ci) {
                                    Some(Value::Str(c)) => Some(c.clone()),
                                    Some(other) if is_json_col => Some(other.to_slt_string()),
                                    _ => None,
                                };
                                if let Some(cell) = cell {
                                    if !is_json_col {
                                        if let Some(elems) = brace_array_elems(&cell) {
                                            for e in elems {
                                                let text =
                                                    if float_elems { fmt_float6(&e) } else { e };
                                                elements.push(parse_value(&text));
                                            }
                                        }
                                    } else {
                                        // JSON elements keep their textual form
                                        match infinity_expression::json::parse(&cell) {
                                            Some(infinity_expression::json::Json::Arr(items)) => {
                                                for v in items {
                                                    elements.push(Value::Str(v.compact()));
                                                }
                                            }
                                            Some(other) => {
                                                elements.push(Value::Str(other.compact()))
                                            }
                                            None => {}
                                        }
                                    }
                                }
                                for v in elements {
                                    let mut r = row.clone();
                                    r.push(v);
                                    out.push(r);
                                }
                            }
                            // Filters written against the output expression
                            // (`uc2 > 1` is sent as `unnest(c2) > 1`) must see
                            // the expanded column.
                            if let Some(p) = where_pred.as_ref() {
                                where_pred = Some(rewrite_unnest_pred(p, &col, &new_name));
                            }
                            if let Some(p) = having.as_ref() {
                                having = Some(Box::new(rewrite_unnest_pred(p, &col, &new_name)));
                            }
                            group_by = group_by
                                .iter()
                                .map(|g| rewrite_unnest_expr(g, &col, &new_name))
                                .collect();
                            items_owned[ui] = SelectItem::Column(new_name.clone());
                            unnest_expansion = Some((new_name, out));
                        }
                    }
                }
                let items: &[SelectItem] = &items_owned;
                // `FROM (SELECT …)`: run the subquery first and scan its result.
                // Its columns become a synthetic table so names resolve.
                let derived: Option<(Vec<String>, Vec<Vec<Value>>)> = match from.as_ref() {
                    Some(f) if f.trim_start().to_ascii_uppercase().starts_with("SELECT ") => {
                        Some(self.derived_table(f.trim())?)
                    }
                    _ => None,
                };
                let from_derived: Option<String> = if derived.is_some() {
                    Some("__derived".to_string())
                } else {
                    from.clone()
                };
                let from = &from_derived;
                // A WHERE clause must be a predicate: `WHERE c1` on a
                // non-boolean column is rejected by the reference.
                if let (Some(t), Some(p), None) =
                    (from.as_ref(), where_pred.as_ref(), derived.as_ref())
                {
                    self.validate_where_predicate(t, p)?;
                }
                // SUM/AVG require a numeric argument.
                if let (Some(t), None) = (from.as_ref(), derived.as_ref()) {
                    self.validate_aggregate_args(t, items)?;
                }
                // `AND`/`OR`/`NOT` take boolean operands: a projected
                // arithmetic expression such as `c1 + 0.1 and c2 - 1.0` is
                // invalid (the reference rejects it).
                for it in items {
                    match it {
                        SelectItem::Expr(e) => validate_boolean_expr(e)?,
                        SelectItem::Func { args, .. } => {
                            for a in args {
                                validate_boolean_expr(a)?;
                            }
                        }
                        _ => {}
                    }
                }
                if let (Some(_), Some(s)) = (from.as_ref(), search.as_deref()) {
                    validate_search_pseudo_columns(s, items)?;
                    // an outer WHERE cannot be combined with an in-subsearch filter
                    if where_pred.is_some() && s.matches.iter().any(|m| m.filter.is_some()) {
                        return Err(Error::Invalid(
                            "cannot have both a where clause and a subsearch filter".into(),
                        ));
                    }
                }
                let mut tables: Vec<(String, Vec<String>)> = Vec::new();
                if let Some(t) = from {
                    let cols = match &derived {
                        Some((cols, _)) => cols.clone(),
                        None => self.table_schema(t)?,
                    };
                    tables.push((t.clone(), cols));
                }
                let mut unnest_rows: Option<Vec<Vec<Value>>> = None;
                if let Some((name, rows)) = unnest_expansion {
                    if let Some(entry) = tables.first_mut() {
                        entry.1.push(name);
                    }
                    unnest_rows = Some(rows);
                }
                if let Some(JoinClause { table, .. }) = join {
                    tables.push((table.clone(), self.table_schema(table)?));
                }
                // SEARCH MATCH: run the search first, then expose its result rows
                // (plus hidden ROW_ID/SIMILARITY/DISTANCE/SCORE columns) as the scan.
                // A derived table's rows are the scan source from the start.
                let mut override_rows: Option<Vec<Vec<Value>>> = derived.map(|(_, rows)| rows);
                if let (Some(t), Some(s)) = (from.as_ref(), search.as_deref()) {
                    let pred = match where_pred.as_ref() {
                        Some(p) => Some(infinity_planner::predicate_expr(p, &tables[0].1)?),
                        None => None,
                    };
                    let (rows, hidden) = self.run_search(t, s, pred.as_ref())?;
                    if !hidden.is_empty() {
                        if let Some(entry) = tables.first_mut() {
                            entry.1.extend(hidden);
                        }
                    }
                    override_rows = Some(rows);
                }
                // Projection-only scan: read just the selected columns from the
                // C++ block mirror when that mirror is as fresh as the row
                // store (plan #87). `SELECT id, name` then avoids decoding the
                // wide columns (embeddings, text) altogether.
                if override_rows.is_none()
                    && join.is_none()
                    && search.is_none()
                    && where_pred.is_none()
                    && group_by.is_empty()
                    && having.is_none()
                    && order_by.is_empty()
                    && limit.is_none()
                    && offset.is_none()
                    && !*distinct
                {
                    if let Some(t) = from.as_ref() {
                        override_rows = self.try_cpp_projection(t, items)?;
                    }
                }
                // `ROW_ID()` outside a SEARCH exposes the row's sequence number
                // as a hidden scan column.
                let mut rowid_col = false;
                if search.is_none() {
                    if let Some(t) = from.as_ref() {
                        if unnest_rows.is_none() && uses_row_id(items) {
                            if let Some(entry) = tables.first_mut() {
                                entry.1.push("#_row_id".into());
                            }
                            let rows: Vec<Vec<Value>> = self
                                .fetch_rows_with_ids(t)?
                                .into_iter()
                                .map(|(seq, mut r)| {
                                    r.push(Value::Int(seq as i64));
                                    r
                                })
                                .collect();
                            override_rows = Some(rows);
                            rowid_col = true;
                        }
                    }
                }
                let _ = rowid_col;
                // An ORDER BY key that names a select alias is rewritten to the
                // corresponding output label.
                let order_keys: Vec<infinity_parser::OrderKey> = order_by
                    .iter()
                    .map(|ok| {
                        let mut ok = ok.clone();
                        if let Some(i) = aliases
                            .iter()
                            .position(|a| a.as_deref() == Some(ok.column.as_str()))
                        {
                            if let Some(item) = items.get(i) {
                                ok.column = infinity_planner::item_label(item);
                            }
                        }
                        ok
                    })
                    .collect();
                if unnest_rows.is_some() {
                    override_rows = unnest_rows;
                }
                let mut plan = plan_select(
                    items,
                    &tables,
                    join.as_ref(),
                    where_pred.as_ref(),
                    &group_by,
                    having.as_deref(),
                    &order_keys,
                    *limit,
                    *offset,
                )?;
                // `SELECT sum(c1) AS s` names the output column `s`.
                apply_select_aliases(&mut plan, aliases);
                // A full-text `threshold=` filter is answered by the index: the
                // score needs corpus statistics a row-local predicate cannot see,
                // so resolve the qualifying rows once and feed them down.
                if override_rows.is_none() && join.is_none() {
                    if let (Some(t), Some(p)) = (from.as_ref(), where_pred.as_ref()) {
                        if let Some((col, query, packed)) = fulltext_index_call(p) {
                            let (opts, hint) = packed.split_once('\u{1}').unwrap_or((&packed, ""));
                            // Only a `threshold=` filter (needs corpus stats) or a
                            // query targeted at a gram index (needs substring
                            // semantics) departs from the row-local term match.
                            let sparsegram = !hint.is_empty()
                                && self
                                    .analyzer_of_index(t, hint)
                                    .map(|a| a.to_ascii_lowercase().starts_with("sparsegram"))
                                    .unwrap_or(false);
                            let has_threshold = opts
                                .split(';')
                                .any(|o| o.trim().to_ascii_lowercase().starts_with("threshold="));
                            if !sparsegram && !has_threshold {
                                override_rows = None;
                            } else if sparsegram {
                                let schema = self.table_schema(t)?;
                                let needle = query.trim().trim_matches('"').to_lowercase();
                                let keep: Vec<u64> = match schema.iter().position(|c| c == &col) {
                                    None => Vec::new(),
                                    Some(ci) => self
                                        .fetch_rows_with_ids(t)?
                                        .into_iter()
                                        .filter(|(_, r)| match r.get(ci) {
                                            Some(Value::Str(s)) => {
                                                s.to_lowercase().contains(&needle)
                                            }
                                            Some(v) => {
                                                v.to_slt_string().to_lowercase().contains(&needle)
                                            }
                                            None => false,
                                        })
                                        .map(|(id, _)| id as u64)
                                        .collect(),
                                };
                                let mut out = self.fetch_rows_by_ids(t, &keep)?;
                                if rowid_col {
                                    for (r, id) in out.iter_mut().zip(keep.iter()) {
                                        r.push(Value::Int(*id as i64));
                                    }
                                }
                                override_rows = Some(out);
                            } else {
                                let m = MatchClause {
                                    kind: "TEXT".into(),
                                    column: col,
                                    args: vec![
                                        ExprAst::Literal(Literal::Str(query)),
                                        ExprAst::Literal(Literal::Str(opts.to_string())),
                                    ],
                                    filter: None,
                                };
                                let (rows, _) = self.search_text_terms(t, &m, None, &[])?;
                                let mut ids: Vec<u64> = rows
                                    .iter()
                                    .filter_map(|r| match r.iter().rev().nth(3) {
                                        Some(Value::Int(i)) => Some(*i as u64),
                                        _ => None,
                                    })
                                    .collect();
                                // Table order, not score order: this is a WHERE filter.
                                ids.sort_unstable();
                                let mut out = self.fetch_rows_by_ids(t, &ids)?;
                                if rowid_col {
                                    for (r, id) in out.iter_mut().zip(ids.iter()) {
                                        r.push(Value::Int(*id as i64));
                                    }
                                }
                                override_rows = Some(out);
                            }
                        }
                    }
                }
                // index scan: an equality predicate on an indexed single-table
                // column fetches only the matching rows.
                if override_rows.is_none() && join.is_none() {
                    if let (Some(t), Some(pred)) = (from.as_ref(), where_pred.as_ref()) {
                        if let Some((col, CmpOp::Eq, val)) = pred.as_column_literal() {
                            if let Some(ids) = self.index_lookup(t, col, val) {
                                let mut rows = self.fetch_rows_by_ids(t, &ids)?;
                                if rowid_col {
                                    for (r, id) in rows.iter_mut().zip(ids.iter()) {
                                        r.push(Value::Int(*id as i64));
                                    }
                                }
                                override_rows = Some(rows);
                            }
                        }
                    }
                }
                let (columns, mut rows) = self.run_plan(plan, override_rows)?;
                if *distinct {
                    let mut seen = std::collections::HashSet::new();
                    rows.retain(|r| seen.insert(r.clone()));
                }
                Ok(QueryResult::Rows { columns, rows })
            }
        }
    }

    /// Decomposes a plan into its source relation and ordered operator stages.
    fn decompose(plan: LogicalPlan) -> (Source, Vec<Stage>) {
        let mut stages = Vec::new();
        let mut cur = plan;
        loop {
            match cur {
                LogicalPlan::Limit { input, n } => {
                    stages.push(Stage::Limit(n));
                    cur = *input;
                }
                LogicalPlan::Offset { input, n } => {
                    stages.push(Stage::Offset(n));
                    cur = *input;
                }
                LogicalPlan::Sort { input, keys } => {
                    stages.push(Stage::Sort { keys });
                    cur = *input;
                }
                LogicalPlan::Aggregate {
                    input,
                    group_keys,
                    group_names,
                    aggs,
                } => {
                    stages.push(Stage::Aggregate {
                        group_keys,
                        group_names,
                        aggs,
                    });
                    cur = *input;
                }
                LogicalPlan::Projection {
                    input,
                    exprs,
                    names,
                } => {
                    stages.push(Stage::Projection(exprs, names));
                    cur = *input;
                }
                LogicalPlan::Filter { input, predicate } => {
                    stages.push(Stage::Filter(predicate));
                    cur = *input;
                }
                LogicalPlan::Scan { table, columns } => {
                    stages.reverse();
                    return (Source::Table { columns, table }, stages);
                }
                LogicalPlan::Join {
                    left,
                    right,
                    left_key,
                    right_key,
                    left_width,
                } => {
                    let (lt, lc) = scan_of(&left);
                    let (rt, rc) = scan_of(&right);
                    let mut columns = lc;
                    columns.extend(rc);
                    stages.reverse();
                    return (
                        Source::Join {
                            columns,
                            left: lt,
                            right: rt,
                            left_key,
                            right_key,
                            left_width,
                        },
                        stages,
                    );
                }
                LogicalPlan::Values { columns, rows } => {
                    stages.reverse();
                    return (Source::Values { columns, rows }, stages);
                }
            }
        }
    }

    fn run_plan(
        &self,
        plan: LogicalPlan,
        override_rows: Option<Vec<Vec<Value>>>,
    ) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let (source, stages) = Self::decompose(infinity_planner::optimize::optimize(plan));
        let (columns, rows) = match source {
            Source::Values { columns, rows } => (columns, rows),
            Source::Table { columns, table } => match override_rows {
                Some(rows) => (columns, rows),
                None => (columns, self.fetch_rows(&table)?),
            },
            Source::Join {
                columns,
                left,
                right,
                left_key,
                right_key,
                left_width,
            } => {
                let lrows = self.fetch_rows(&left)?;
                let rrows = self.fetch_rows(&right)?;
                (
                    columns,
                    nested_loop_join(&lrows, &rrows, left_key, right_key, left_width),
                )
            }
        };
        self.rt.block_on(pipeline(
            columns,
            rows,
            stages,
            self.context.exchange_capacity,
        ))
    }

    /// Maintains declared indexes on insert.
    fn update_indexes_on_insert(&self, table: &str, seq: usize, values: &[Literal]) -> Result<()> {
        let texts: Vec<String> = values.iter().map(|v| v.to_slt_string()).collect();
        self.update_indexes_on_insert_text(table, seq, &texts)
    }

    /// Same, for rows already rendered in their stored text form (UPDATE).
    fn update_indexes_on_insert_text(
        &self,
        table: &str,
        seq: usize,
        values: &[String],
    ) -> Result<()> {
        let schema = self.table_schema(table)?;
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let entry = tables.entry(tkey).or_default();
        for (col, idx) in entry.secondary.indexes.iter_mut() {
            if let Some(ci) = schema.iter().position(|c| c == col) {
                if let Some(v) = values.get(ci) {
                    idx.insert(v.as_bytes(), seq as u64);
                }
            }
        }
        Ok(())
    }

    fn fetch_rows(&self, table: &str) -> Result<Vec<Vec<Value>>> {
        Ok(self
            .fetch_rows_with_ids(table)?
            .into_iter()
            .map(|(_, r)| r)
            .collect())
    }

    /// Next storage sequence id for `table` (max existing + 1).
    fn next_row_seq(&self, table: &str) -> Result<usize> {
        let prefix = row_prefix(table);
        let mut max = 0usize;
        for (k, _v) in self.storage.store.scan_prefix(&prefix)? {
            if let Ok(seq) = String::from_utf8_lossy(&k[prefix.len()..]).parse::<usize>() {
                // A compacted (renumbered) row is tombstoned at its old id;
                // it must not push the next id past the live range.
                if self.storage.store.get(&del_key(table, seq))?.is_some() {
                    continue;
                }
                max = max.max(seq + 1);
            }
        }
        Ok(max)
    }

    /// Non-deleted rows with their storage sequence ids (tombstones are skipped).
    /// Rows shorter than the schema (e.g. after `ALTER TABLE ADD COLUMN`) are
    /// padded with their column defaults.
    fn fetch_rows_with_ids(&self, table: &str) -> Result<Vec<(usize, Vec<Value>)>> {
        let key = self.table_key(table);
        let cached = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.row_cache.as_ref().map(|c| (c.rows.clone(), c.bytes)));
        if let Some((rows, bytes)) = cached {
            // The cache is authoritative only while it covers every live row.
            if rows.len() == self.row_count(table)? {
                return Ok(rows);
            }
            let mut tables = self.tables.lock().unwrap();
            if let Some(entry) = tables.get_mut(&key) {
                if entry.row_cache.as_ref().map(|c| c.bytes) == Some(bytes) {
                    entry.row_cache = None;
                }
            }
        }
        // The C++ block mirror is the authoritative value source once it covers
        // every live row; the KV store then only supplies the row-id list (the
        // write-ahead journal between checkpoints).
        if let Some(rows) = self.try_cpp_rows(table)? {
            return Ok(rows);
        }
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        // Collect tombstones with one prefix scan: a point lookup per row
        // would add O(rows) KV round trips to every full scan.
        let dpref = del_prefix(table);
        let mut deleted: std::collections::HashSet<usize> = Default::default();
        for (k, _) in self.storage.store.scan_prefix(&dpref)? {
            if let Some(raw) = k.get(dpref.len()..) {
                if let Ok(s) = std::str::from_utf8(raw) {
                    if let Ok(n) = s.parse::<usize>() {
                        deleted.insert(n);
                    }
                }
            }
        }
        let mut rows = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            if deleted.contains(&seq) {
                continue; // deleted
            }
            let mut cells = self.parse_stored_row(table, &defaults, &v)?;
            while cells.len() < defaults.len() {
                let i = cells.len();
                cells.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            rows.push((seq, cells));
        }
        if rows.len() == self.row_count(table)? {
            let types = self.column_types(table).unwrap_or_default();
            if scalar_only(&types) {
                let bytes: usize = 24
                    + rows
                        .iter()
                        .map(|(_, r)| r.iter().map(value_bytes).sum::<usize>())
                        .sum::<usize>();
                if bytes <= ROW_CACHE_BUDGET_BYTES {
                    let mut tables = self.tables.lock().unwrap();
                    tables.entry(key).or_default().row_cache = Some(RowCache {
                        rows: rows.clone(),
                        bytes,
                    });
                }
            }
        }
        Ok(rows)
    }

    /// Like `fetch_rows_with_ids`, but including tombstoned rows. Full-text
    /// corpus statistics keep counting deleted documents, as the reference's
    /// segments do until compaction, so the scan path must see them too.
    fn fetch_rows_including_deleted(&self, table: &str) -> Result<Vec<(usize, Vec<Value>)>> {
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut rows = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            let mut cells = self.parse_stored_row(table, &defaults, &v)?;
            while cells.len() < defaults.len() {
                let i = cells.len();
                cells.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            rows.push((seq, cells));
        }
        Ok(rows)
    }

    /// Full rows from the C++ block mirror.
    ///
    /// The mirror is a snapshot of the rows that existed when it was written,
    /// so it serves every row whose id it recorded. Rows appended afterwards
    /// (INSERT/COPY/UPDATE, or a column rewrite) are read from the row journal,
    /// and rows deleted afterwards are hidden by their tombstones; no
    /// checkpoint is needed for the mirror to stay authoritative.
    ///
    /// Returns `None` when the table has no block mirror or the mirror does not
    /// match its recorded row ids, so the caller falls back to the KV row store.
    fn try_cpp_rows(&self, table: &str) -> Result<Option<TableRows>> {
        let table = if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.db(), table.to_ascii_lowercase())
        };
        let table = table.as_str();
        let key = self.table_key(table);
        let snapshot = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.cpp_block_rows.clone());
        let Some(snapshot) = snapshot else {
            return Ok(None);
        };
        let schema = self.table_schema(table)?;
        if schema.is_empty() {
            return Ok(None);
        }
        let wanted: Vec<usize> = (0..schema.len()).collect();
        let Ok((_, rows)) = self.read_cpp_blocks_selected(table, &wanted) else {
            return Ok(None);
        };
        if rows.len() != snapshot.len() {
            // The mirror was rewritten between the snapshot and the read.
            return Ok(None);
        }
        // Both the snapshot and the journal scan are in ascending row-id
        // order, so a linear merge finds each row's block cells without a
        // per-scan hash table.
        let mut next = 0usize;
        let prefix = row_prefix(table);
        let deleted = self.tombstone_set(table)?;
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut out = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let Some(raw) = k.get(prefix.len()..) else {
                continue;
            };
            let Ok(raw) = std::str::from_utf8(raw) else {
                continue;
            };
            let Ok(seq) = raw.parse::<usize>() else {
                continue;
            };
            if deleted.contains(&(seq as u64)) {
                continue;
            }
            // Rows deleted after the checkpoint leave holes in the snapshot.
            while next < snapshot.len() && snapshot[next] < seq {
                next += 1;
            }
            if next < snapshot.len() && snapshot[next] == seq {
                let cells = &rows[next];
                out.push((seq, cells.iter().map(|c| parse_value(c)).collect()));
                next += 1;
            } else {
                // Written after the mirror snapshot: the journal holds it.
                out.push((seq, self.parse_stored_row(table, &defaults, &v)?));
            }
        }
        Ok(Some(out))
    }

    fn try_cpp_projection(
        &self,
        table: &str,
        items: &[SelectItem],
    ) -> Result<Option<Vec<Vec<Value>>>> {
        let table = if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.db(), table.to_ascii_lowercase())
        };
        let table = table.as_str();
        let schema = self.table_schema(table)?;
        // A decoded row cache answers a projection faster than decoding the
        // block mirror; leave the scan to it.
        let key = self.table_key(table);
        let cached = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.row_cache.as_ref().map(|c| c.rows.len()));
        if let Some(len) = cached {
            if len == self.live_row_count(table)? {
                return Ok(None);
            }
        }
        let mut wanted: Vec<usize> = Vec::new();
        for item in items {
            match item {
                SelectItem::Column(name) => {
                    let base = name.split('.').next_back().unwrap_or(name);
                    let Some(i) = schema.iter().position(|c| c == base) else {
                        return Ok(None);
                    };
                    wanted.push(i);
                }
                _ => return Ok(None),
            }
        }
        if wanted.is_empty() {
            return Ok(None);
        }
        let key = self.table_key(table);
        let snapshot = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.cpp_block_rows.clone());
        let Some(snapshot) = snapshot else {
            return Ok(None);
        };
        let Ok((_, rows)) = self.read_cpp_blocks_selected(table, &wanted) else {
            return Ok(None);
        };
        if rows.len() != snapshot.len() {
            return Ok(None);
        }
        // Same snapshot + journal overlay as `try_cpp_rows`, but only the
        // projected columns are decoded from the blocks.
        let mut next = 0usize;
        let prefix = row_prefix(table);
        let deleted = self.tombstone_set(table)?;
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut out = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let Some(raw) = k.get(prefix.len()..) else {
                continue;
            };
            let Ok(raw) = std::str::from_utf8(raw) else {
                continue;
            };
            let Ok(seq) = raw.parse::<usize>() else {
                continue;
            };
            if deleted.contains(&(seq as u64)) {
                continue;
            }
            while next < snapshot.len() && snapshot[next] < seq {
                next += 1;
            }
            if next < snapshot.len() && snapshot[next] == seq {
                out.push(rows[next].iter().map(|c| parse_value(c)).collect());
                next += 1;
            } else {
                let row = self.parse_stored_row(table, &defaults, &v)?;
                out.push(
                    wanted
                        .iter()
                        .map(|i| row.get(*i).cloned().unwrap_or(Value::Null))
                        .collect(),
                );
            }
        }
        Ok(Some(out))
    }

    fn blob_file(&self, table: &str) -> Result<Arc<Mutex<std::fs::File>>> {
        let key = self.table_key(table);
        if let Some(f) = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.row_blob.clone())
        {
            return Ok(f);
        }
        let dir = self.dir.join("rowdata");
        std::fs::create_dir_all(&dir).map_err(|e| Error::Invalid(format!("row blob dir: {e}")))?;
        let path = dir.join(format!("{}.bin", key.replace('.', "_")));
        let file = std::fs::OpenOptions::new()
            .create(true)
            .read(true)
            .append(true)
            .open(&path)
            .map_err(|e| Error::Invalid(format!("row blob {}: {e}", path.display())))?;
        let shared = Arc::new(Mutex::new(file));
        self.tables.lock().unwrap().entry(key).or_default().row_blob = Some(shared.clone());
        Ok(shared)
    }

    fn append_blob(&self, table: &str, bytes: &[u8]) -> Result<(u64, u64)> {
        let file = self.blob_file(table)?;
        let mut file = file.lock().unwrap();
        let offset = file
            .metadata()
            .map_err(|e| Error::Invalid(format!("row blob size: {e}")))?
            .len();
        // Decimal vector text compresses ~2.5x; the sidecar stays in page cache,
        // so its size directly bounds RSS during a long import.
        let packed = lz4_flex::compress_prepend_size(bytes);
        file.write_all(&packed)
            .map_err(|e| Error::Invalid(format!("row blob write: {e}")))?;
        Ok((offset, packed.len() as u64))
    }

    fn read_blob(&self, table: &str, offset: u64, len: usize) -> Result<Vec<u8>> {
        let file = self.blob_file(table)?;
        let file = file.lock().unwrap();
        let mut buf = vec![0u8; len];
        file.read_exact_at(&mut buf, offset)
            .map_err(|e| Error::Invalid(format!("row blob read: {e}")))?;
        lz4_flex::decompress_size_prepended(&buf)
            .map_err(|e| Error::Invalid(format!("row blob decompress: {e}")))
    }

    /// Flushes dirty sidecar pages so they become clean, reclaimable page
    /// cache instead of accumulating as unswappable dirty memory (Gate C).
    fn sync_row_blobs(&self) {
        let files: Vec<Arc<Mutex<std::fs::File>>> = self
            .tables
            .lock()
            .unwrap()
            .values()
            .filter_map(|e| e.row_blob.clone())
            .collect();
        for file in files {
            if let Ok(file) = file.lock() {
                let _ = file.sync_data();
            }
        }
    }

    /// Decodes one stored cell. Wide vector-like cells live in the sidecar and
    /// carry a `\x01B{offset}:{len}` reference.
    fn decode_cell(&self, table: &str, cell: &str) -> Result<Value> {
        if let Some(rest) = cell.strip_prefix('\u{1}').and_then(|r| r.strip_prefix('B')) {
            let (offset, len) = rest
                .split_once(':')
                .ok_or_else(|| Error::Invalid("corrupt row blob reference".into()))?;
            let offset: u64 = offset
                .parse()
                .map_err(|_| Error::Invalid("corrupt row blob offset".into()))?;
            let len: usize = len
                .parse()
                .map_err(|_| Error::Invalid("corrupt row blob length".into()))?;
            let bytes = self.read_blob(table, offset, len)?;
            return Ok(parse_value(&String::from_utf8_lossy(&bytes)));
        }
        Ok(parse_value(cell))
    }

    /// [`encode_row`] plus sidecar offload for wide vector-like cells.
    fn encode_row_stored(&self, table: &str, row: &[Literal], types: &[String]) -> Result<String> {
        let rendered = encode_row(row, types);
        if !types.iter().any(|t| is_blob_type(t)) {
            return Ok(rendered);
        }
        let mut cells: Vec<String> = rendered.split('\t').map(str::to_string).collect();
        for (i, ty) in types.iter().enumerate() {
            if !is_blob_type(ty) {
                continue;
            }
            let Some(cell) = cells.get(i) else { continue };
            if cell.len() > 64 && cell != "null" {
                let (offset, len) = self.append_blob(table, cell.as_bytes())?;
                cells[i] = format!("\u{1}B{offset}:{len}");
            }
        }
        Ok(cells.join("\t"))
    }

    fn fetch_some_rows_with_ids(
        &self,
        table: &str,
        ids: &[u64],
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut out = Vec::with_capacity(ids.len());
        for &id in ids {
            let seq = id as usize;
            // A stale index entry may point at a tombstoned row.
            if self.storage.store.get(&del_key(table, seq))?.is_some() {
                continue;
            }
            if let Some(v) = self.storage.store.get(&row_key(table, seq))? {
                out.push((seq, self.parse_stored_row(table, &defaults, &v)?));
            }
        }
        Ok(out)
    }

    /// Block-max WAND over the in-memory postings, mirroring the reference's
    /// `BlockMaxWandIterator::Next`: pick the pivot document from the per-term
    /// BM25 upper bounds, advance the prefix cursors to it, prune with the
    /// block-max bound, and score exactly only when the bound beats the current
    /// k-th score. Terms must be single-unit (no phrase / require-all) OR terms.
    ///
    /// `dead` holds tombstoned rows: they still contribute to the corpus
    /// statistics (the reference's segments do until compaction) but never rank,
    /// exactly like the scan path's live check.
    /// How `wand_scan` turns matched terms into a score. The reference scores
    /// `similarity=boolean` as one point per matched term (per field), while
    /// the default is BM25.
    #[allow(clippy::too_many_arguments)]
    fn wand_scan(
        fields: &[(MergedIndex, Vec<f64>)],
        terms: &[String],
        k: usize,
        dead: &std::collections::HashSet<u64>,
        delta: f64,
        min_should: usize,
        mode: wand::WandMode,
    ) -> Vec<(u64, f64)> {
        wand::BlockMaxWandIterator::new(fields, terms, k, dead, delta, min_should, mode).run()
    }

    fn vector_cache_for(
        &self,
        table: &str,
        column: &str,
        dims: usize,
    ) -> Result<SharedVectorCache> {
        let key = index_key(table, column);
        // A bulk load may bypass the incremental hooks; the key count is the
        // authority on how many live rows the cache must hold.
        let expected = self
            .storage
            .store
            .count_prefix(&row_prefix(table))?
            .saturating_sub(self.storage.store.count_prefix(&del_prefix(table))?);
        // Hold the map lock across a rebuild: concurrent readers noticing the
        // same gap would each allocate a copy. A 250 ms floor keeps a
        // write-heavy workload from rebuilding on every query.
        let map = self.vector_cache.lock().unwrap();
        if let Some(cache) = map.get(&key) {
            let (live, built_at) = cache
                .read()
                .map(|g| (g.live.len(), g.built_at))
                .unwrap_or((0, std::time::Instant::now()));
            // Throttle only big caches: on a small table a stale snapshot would
            // change results, and a rebuild is cheap.
            let throttled =
                live > 10_000 && built_at.elapsed() < std::time::Duration::from_millis(250);
            if live == expected || throttled {
                return Ok(cache.clone());
            }
        }
        let schema = self.table_schema(table)?;
        let col = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let mut cache = VectorCache {
            dims,
            rows: Vec::new(),
            live: Default::default(),
            built_at: std::time::Instant::now(),
        };
        for (seq, cells) in self.fetch_rows_with_ids_projected(table, col)? {
            if let Some(Value::Str(text)) = cells.get(col) {
                let v = parse_vec_str(text);
                if v.len() == dims {
                    cache.rows.push((seq, v));
                    cache.live.insert(seq);
                }
            }
        }
        drop(map);
        let shared = Arc::new(std::sync::RwLock::new(cache));
        self.vector_cache
            .lock()
            .unwrap()
            .insert(key, shared.clone());
        Ok(shared)
    }

    /// Feeds an inserted/rewritten row into the in-memory HNSW segment of every
    /// vector index on `table` — the reference's `HnswIndexInMem`, which holds
    /// the rows written since the last dump. The sealed (on-disk) graph lives in
    /// `vector_indexes`.
    fn note_hnsw_insert(&self, table: &str, seq: u64, cells: &[String]) {
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let key = self.table_key(table);
        // The table lock is only held to reach the per-segment mutexes; the
        // graph updates run under each segment's own lock.
        let segments: Vec<(String, Arc<Mutex<hnsw_mem::HnswIndexInMem>>)> = {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key).or_default();
            if entry.hnsw_mem.is_empty() && !entry.hnsw_probed {
                entry.hnsw_probed = true;
                let Ok(indexes) = self.storage.catalog.list_indexes() else {
                    return;
                };
                let Ok(types) = self.column_types(table) else {
                    return;
                };
                let short = table.split('.').next_back().unwrap_or(table);
                for im in indexes {
                    if !(im.table == table || im.table == short)
                        || !im.index_type.eq_ignore_ascii_case("HNSW")
                    {
                        continue;
                    }
                    let Some(ci) = schema.iter().position(|c| c == &im.column) else {
                        continue;
                    };
                    let Some(dims) = types.get(ci).and_then(|t| type_dim(t)) else {
                        continue;
                    };
                    let opt = |name: &str| -> Option<usize> {
                        im.options
                            .iter()
                            .find(|(k, _)| k.eq_ignore_ascii_case(name))
                            .and_then(|(_, v)| v.parse().ok())
                    };
                    let m = opt("M").unwrap_or(16);
                    let efc = opt("EF_CONSTRUCTION").unwrap_or(200);
                    let opt = |name: &str| {
                        im.options
                            .iter()
                            .find(|(k, _)| k.eq_ignore_ascii_case(name))
                            .map(|(_, v)| v.clone())
                    };
                    let metric = infinity_index::vector::Metric::parse(
                        &opt("metric").unwrap_or_else(|| "l2".into()),
                    );
                    let encoding = infinity_index::vector::HnswEncoding::parse(
                        &opt("encode").unwrap_or_default(),
                    );
                    entry.hnsw_mem.entry(im.column.clone()).or_insert_with(|| {
                        Arc::new(Mutex::new(hnsw_mem::HnswIndexInMem::new_with(
                            dims, m, efc, metric, encoding,
                        )))
                    });
                }
            }
            entry
                .hnsw_mem
                .iter()
                .map(|(c, m)| (c.clone(), m.clone()))
                .collect()
        };
        for (column, mem) in segments {
            let Some(ci) = schema.iter().position(|c| c == &column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v: Vec<f32> = parse_vec_str(text).iter().map(|x| *x as f32).collect();
            if let Ok(mut mem) = mem.lock() {
                if v.len() == mem.dims {
                    mem.insert(seq, &v);
                }
            }
        }
    }

    fn update_vector_caches(&self, table: &str, seq: usize, cells: &[String]) {
        let prefix = format!("{table}.");
        let caches: Vec<(String, SharedVectorCache)> = self
            .vector_cache
            .lock()
            .unwrap()
            .iter()
            .filter(|(k, _)| k.starts_with(&prefix))
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect();
        // The HNSW/IVF memory indexes are maintained regardless of whether a
        // query cache exists.
        self.note_hnsw_insert(table, seq as u64, cells);
        self.note_ivf_insert(table, seq as u64, cells);
        if caches.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        for (key, shared) in caches {
            let column = &key[prefix.len()..];
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v = parse_vec_str(text);
            let mut cache = shared.write().unwrap();
            if v.len() != cache.dims {
                continue;
            }
            match cache.rows.iter_mut().find(|(s, _)| *s == seq) {
                Some(entry) => entry.1 = v,
                None => cache.rows.push((seq, v)),
            }
            cache.live.insert(seq);
        }
    }

    /// Feeds a row into the IVF memory segment (rows written since the last
    /// mirror dump), the reference's in-memory IVF form.
    fn note_ivf_insert(&self, table: &str, seq: u64, cells: &[String]) {
        let tkey = self.table_key(table);
        if self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .is_none_or(|e| e.ivf_mem.is_empty())
        {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let Ok(types) = self.column_types(table) else {
            return;
        };
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&tkey) else {
            return;
        };
        for (column, rows) in entry.ivf_mem.iter_mut() {
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let v: Vec<f32> = parse_vec_str(text).iter().map(|x| *x as f32).collect();
            let dims = types.get(ci).and_then(|t| type_dim(t)).unwrap_or(0);
            if !v.is_empty() && (dims == 0 || v.len() == dims) {
                rows.push((seq, v));
            }
        }
    }

    fn drop_vector_cache_rows(&self, table: &str, seqs: &[usize]) {
        let prefix = format!("{table}.");
        for (key, shared) in self.vector_cache.lock().unwrap().iter() {
            if key.starts_with(&prefix) {
                let mut cache = shared.write().unwrap();
                for seq in seqs {
                    cache.live.remove(seq);
                }
            }
        }
    }

    /// Builds (once) the inverted index for a text column from its live rows.
    fn text_index_for(
        &self,
        table: &str,
        column: &str,
        delims: Option<Vec<char>>,
    ) -> Result<SharedTextIndex> {
        let key = index_key(table, column);
        let tkey = self.table_key(table);
        // A bulk load that bypasses the DML hooks would leave the index short of
        // rows; compare against the cheap reservation counter and rebuild.
        let reserved = self.storage.store.scan_prefix(&row_prefix(table))?.len();
        {
            let tables = self.tables.lock().unwrap();
            if let Some(idx) = tables.get(&tkey).and_then(|e| e.text.get(column)) {
                let complete = idx.read().map(|g| g.total_docs()).unwrap_or(0);
                if complete >= reserved {
                    return Ok(idx.clone());
                }
            }
        }
        let schema = self.table_schema(table)?;
        let col = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let dir = self
            .dir
            .join("fulltext")
            .join(key.replace('/', "_"))
            .to_string_lossy()
            .into_owned();
        let mut index = TextIndex::new(dir, delims);
        for (seq, cells) in self.fetch_rows_with_ids_projected_all(table, col)? {
            let text = match cells.get(col) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => String::new(),
            };
            index_row(&mut index, seq as u64, &text);
            index.maybe_seal(self.memindex_capacity());
        }
        let shared = Arc::new(std::sync::RwLock::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .text
            .insert(column.to_string(), shared.clone());
        Ok(shared)
    }

    /// Builds (once) the multi-vector index of a column from its rows: every
    /// document goes to the memory tail and is dumped immediately, so later
    /// queries only map the segment files. Subsequent inserts append to the
    /// tail and `flush()` seals it (the reference's dump).
    fn multivector_index_for(
        &self,
        table: &str,
        column: &str,
        dims: usize,
    ) -> Result<Arc<std::sync::RwLock<MultiVectorIndex>>> {
        let key = index_key(table, column);
        let tkey = self.table_key(table);
        let reserved = self.storage.store.scan_prefix(&row_prefix(table))?.len();
        {
            let tables = self.tables.lock().unwrap();
            if let Some(idx) = tables.get(&tkey).and_then(|e| e.mv.get(column)) {
                let complete = idx.read().map(|g| g.doc_count()).unwrap_or(0);
                if complete >= reserved {
                    return Ok(idx.clone());
                }
            }
        }
        let schema = self.table_schema(table)?;
        let ci = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let dir = self.dir.join("multivector").join(key.replace('/', "_"));
        // A rebuild rewrites the chunk set; drop stale files so the readers
        // never mix two builds.
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                let p = entry.path();
                if p.extension().is_some_and(|x| x == "mvec" || x == "emvb") {
                    let _ = std::fs::remove_file(p);
                }
            }
        }
        eprintln!(
            "DEBUG mv_for build table={table} col={column} plaid={:?} smve={:?}",
            self.mv_plaid_config(table, column),
            self.mv_smve_config(table, column)
        );
        let mut index = MultiVectorIndex::new(
            dir,
            dims,
            self.mv_plaid_config(table, column),
            self.mv_smve_config(table, column),
        );
        for (seq, cells) in self.fetch_rows_with_ids_projected_all(table, ci)? {
            let text = match cells.get(ci) {
                Some(Value::Str(s)) => s.clone(),
                Some(other) => other.to_slt_string(),
                None => continue,
            };
            if let Some(doc) = parse_multivector_doc(&text, dims) {
                index.insert(seq as u64, doc);
            }
        }
        // The reference trains `PlaidGlobalCentroids` and builds
        // `PlaidIndexInMem` while the embeddings are buffered, before the dump.
        index.build_plaid();
        index.build_smve();
        index.seal();
        let shared = Arc::new(std::sync::RwLock::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .mv
            .insert(column.to_string(), shared.clone());
        Ok(shared)
    }

    /// `CREATE INDEX … USING PLAID WITH (nbits=…, n_centroids=…)` on
    /// `table.column`, if one exists.
    fn mv_plaid_config(&self, table: &str, column: &str) -> Option<PlaidConfig> {
        let short = table.rsplit('.').next().unwrap_or(table);
        let im = self
            .storage
            .catalog
            .list_indexes()
            .ok()?
            .into_iter()
            .find(|im| {
                im.index_type.eq_ignore_ascii_case("PLAID")
                    && im.column == column
                    && (im.table == table || im.table == short)
            })?;
        let mut cfg = PlaidConfig {
            nbits: 4,
            n_centroids: 0,
            colbertsar: false,
        };
        for (key, value) in &im.options {
            if key.eq_ignore_ascii_case("nbits") {
                if let Ok(v) = value.trim().parse() {
                    cfg.nbits = v;
                }
            } else if key.eq_ignore_ascii_case("n_centroids") {
                if let Ok(v) = value.trim().parse() {
                    cfg.n_centroids = v;
                }
            } else if key.eq_ignore_ascii_case("colbertsar_mode") {
                let v = value.trim();
                cfg.colbertsar = v.eq_ignore_ascii_case("true") || v == "1";
            }
        }
        Some(cfg)
    }

    /// Whether `table.column` carries an ANN index that the tensor scan can
    /// answer from (`PLAID`/`SMVE`; `EMVB` uses the same multi-vector path).
    fn tensor_has_ann_index(&self, table: &str, column: &str) -> bool {
        let short = table.rsplit('.').next().unwrap_or(table);
        self.storage
            .catalog
            .list_indexes()
            .unwrap_or_default()
            .into_iter()
            .any(|im| {
                im.column == column
                    && (im.table == table || im.table == short)
                    && (im.index_type.eq_ignore_ascii_case("PLAID")
                        || im.index_type.eq_ignore_ascii_case("SMVE")
                        || im.index_type.eq_ignore_ascii_case("EMVB"))
            })
    }

    /// `CREATE INDEX … USING SMVE WITH (width=…, topk=…)` on `table.column`.
    fn mv_smve_config(&self, table: &str, column: &str) -> Option<SmveConfig> {
        let short = table.rsplit('.').next().unwrap_or(table);
        let im = self
            .storage
            .catalog
            .list_indexes()
            .ok()?
            .into_iter()
            .find(|im| {
                im.index_type.eq_ignore_ascii_case("SMVE")
                    && im.column == column
                    && (im.table == table || im.table == short)
            })?;
        let mut cfg = SmveConfig {
            width: 2048,
            topk: 8,
        };
        for (key, value) in &im.options {
            if key.eq_ignore_ascii_case("width") {
                if let Ok(v) = value.trim().parse() {
                    cfg.width = v;
                }
            } else if key.eq_ignore_ascii_case("topk") {
                if let Ok(v) = value.trim().parse() {
                    cfg.topk = v;
                }
            }
        }
        Some(cfg)
    }

    /// Exact MaxSim of one row against `query`, read from the row journal
    /// (used to re-rank SMVE candidates the way the reference reads the column).
    fn exact_tensor_score(
        &self,
        table: &str,
        column: &str,
        dims: usize,
        query: &[Vec<f32>],
        seq: u64,
    ) -> Option<f32> {
        let schema = self.table_schema(table).ok()?;
        let ci = schema.iter().position(|c| c == column)?;
        let raw = self
            .storage
            .store
            .get(&row_key(table, seq as usize))
            .ok()??;
        let defaults = self.table_defaults(table).unwrap_or_default();
        let cells = self.parse_stored_row(table, &defaults, &raw).ok()?;
        let text = match cells.get(ci)? {
            Value::Str(s) => s.clone(),
            other => other.to_slt_string(),
        };
        let doc = parse_multivector_doc(&text, dims)?;
        Some(maxsim_tokens(query, &doc))
    }

    /// Keeps existing multi-vector indexes in step with an inserted/rewritten
    /// row. Called from the shared DML hook so every write path is covered.
    fn update_multivector_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        let tkey = self.table_key(table);
        let indexes: Vec<(String, Arc<std::sync::RwLock<MultiVectorIndex>>)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| e.mv.iter().map(|(c, v)| (c.clone(), v.clone())).collect())
            .unwrap_or_default();
        if indexes.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let Ok(types) = self.column_types(table) else {
            return;
        };
        for (column, shared) in indexes {
            let column = &column;
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(dims) = types.get(ci).and_then(|t| type_dim(t)) else {
                continue;
            };
            if dims == 0 {
                continue;
            }
            let mut index = shared.write().unwrap();
            index.remove(seq as u64);
            let Some(text) = cells.get(ci) else { continue };
            if let Some(doc) = parse_multivector_doc(text, dims) {
                index.insert(seq as u64, doc);
            }
        }
    }

    /// Dumbs every multi-vector memory tail to its segment files. Called by
    /// `flush()` / checkpoint, mirroring the reference's memindex dump.
    fn seal_multivector_indexes(&self) {
        let indexes: Vec<Arc<std::sync::RwLock<MultiVectorIndex>>> = self
            .tables
            .lock()
            .unwrap()
            .values()
            .flat_map(|e| e.mv.values().cloned())
            .collect();
        for shared in indexes {
            shared.write().unwrap().seal();
        }
    }

    /// Removes the multi-vector chunks of `table` (or one column) from disk and
    /// from the cache; called when the table or its index is dropped.
    fn drop_multivector_storage(&self, table: &str, column: Option<&str>) {
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&tkey) else {
            return;
        };
        let columns: Vec<String> = match column {
            Some(c) => vec![c.to_string()],
            None => entry.mv.keys().cloned().collect(),
        };
        for c in columns {
            if let Some(idx) = entry.mv.remove(&c) {
                let dir = idx.read().map(|g| g.dir.clone()).ok();
                if let Some(dir) = dir {
                    if let Ok(entries) = std::fs::read_dir(&dir) {
                        for entry in entries.flatten() {
                            let p = entry.path();
                            if p.extension().is_some_and(|x| x == "mvec" || x == "emvb") {
                                let _ = std::fs::remove_file(p);
                            }
                        }
                    }
                }
            }
        }
    }

    /// Builds (once) the sparse BMP index of a column, dumping every segment so
    /// later queries only map the chunk files. Inserts append to the memory
    /// tail and `flush()` seals it.
    fn sparse_index_for(
        &self,
        table: &str,
        column: &str,
        width: infinity_storage::bmp_cpp::IdxWidth,
    ) -> Result<Arc<std::sync::RwLock<SparseIndex>>> {
        let key = index_key(table, column);
        let tkey = self.table_key(table);
        let reserved = self.storage.store.scan_prefix(&row_prefix(table))?.len();
        {
            let tables = self.tables.lock().unwrap();
            if let Some(idx) = tables.get(&tkey).and_then(|e| e.sparse.get(column)) {
                let complete = idx.read().map(|g| g.doc_count()).unwrap_or(0);
                if complete >= reserved {
                    return Ok(idx.clone());
                }
            }
        }
        let schema = self.table_schema(table)?;
        let ci = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::NotFound(format!("column '{column}'")))?;
        let dir = self.dir.join("sparse").join(key.replace('/', "_"));
        if let Ok(entries) = std::fs::read_dir(&dir) {
            for entry in entries.flatten() {
                let p = entry.path();
                if p.extension().is_some_and(|x| x == "bmp") {
                    let _ = std::fs::remove_file(p);
                }
            }
        }
        let mut index = SparseIndex::new(dir, width);
        for (seq, cells) in self.fetch_rows_with_ids_projected_all(table, ci)? {
            // A NULL sparse cell has no postings; only stored values are indexed.
            let Some(Value::Str(text)) = cells.get(ci) else {
                continue;
            };
            let entries: Vec<(i32, f32)> = parse_sparse_str(text)
                .into_iter()
                .map(|(i, x)| (i as i32, x as f32))
                .collect();
            index.insert(seq as u64, entries);
        }
        index.seal();
        let shared = Arc::new(std::sync::RwLock::new(index));
        self.tables
            .lock()
            .unwrap()
            .entry(tkey)
            .or_default()
            .sparse
            .insert(column.to_string(), shared.clone());
        Ok(shared)
    }

    /// Keeps existing sparse indexes in step with an inserted/rewritten row.
    fn update_sparse_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        let tkey = self.table_key(table);
        let indexes: Vec<(String, Arc<std::sync::RwLock<SparseIndex>>)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| {
                e.sparse
                    .iter()
                    .map(|(c, v)| (c.clone(), v.clone()))
                    .collect()
            })
            .unwrap_or_default();
        if indexes.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        for (column, shared) in indexes {
            let column = &column;
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let mut index = shared.write().unwrap();
            index.remove(seq as u64);
            let Some(text) = cells.get(ci) else { continue };
            // A NULL cell stores the literal `null`; it has no postings.
            if text.trim().eq_ignore_ascii_case("null") {
                continue;
            }
            let entries: Vec<(i32, f32)> = parse_sparse_str(text)
                .into_iter()
                .map(|(i, x)| (i as i32, x as f32))
                .collect();
            index.insert(seq as u64, entries);
        }
    }

    /// Dumps every sparse memory tail into its BMP chunks (called by `flush`).
    fn seal_sparse_indexes(&self) {
        let indexes: Vec<Arc<std::sync::RwLock<SparseIndex>>> = self
            .tables
            .lock()
            .unwrap()
            .values()
            .flat_map(|e| e.sparse.values().cloned())
            .collect();
        for shared in indexes {
            shared.write().unwrap().seal();
        }
    }

    /// Removes the sparse chunks of `table` (or one column) from disk and cache.
    fn drop_sparse_storage(&self, table: &str, column: Option<&str>) {
        let tkey = self.table_key(table);
        let mut tables = self.tables.lock().unwrap();
        let Some(entry) = tables.get_mut(&tkey) else {
            return;
        };
        let columns: Vec<String> = match column {
            Some(c) => vec![c.to_string()],
            None => entry.sparse.keys().cloned().collect(),
        };
        for c in columns {
            if let Some(idx) = entry.sparse.remove(&c) {
                if let Ok(dir) = idx.read().map(|g| g.dir.clone()) {
                    let _ = std::fs::remove_dir_all(dir);
                }
            }
        }
    }

    /// Tombstoned sequence ids of a table; index hits are validated against it.
    fn tombstone_set(&self, table: &str) -> Result<std::collections::HashSet<u64>> {
        let dp = del_prefix(table);
        let mut dead: std::collections::HashSet<u64> = Default::default();
        for (k, _) in self.storage.store.scan_prefix(&dp)? {
            if let Ok(seq) = String::from_utf8_lossy(&k[dp.len()..]).parse() {
                dead.insert(seq);
            }
        }
        Ok(dead)
    }

    /// Keeps existing inverted indexes in step with an inserted/rewritten row.
    fn update_text_indexes(&self, table: &str, seq: usize, cells: &[String]) {
        // The multi-vector and sparse indexes share this DML hook: it is the one
        // place every write path (SQL INSERT/UPDATE, COPY, Thrift insert) goes
        // through.
        self.update_multivector_indexes(table, seq, cells);
        self.update_sparse_indexes(table, seq, cells);
        self.enforce_memindex_quota();
        let tkey = self.table_key(table);
        let indexes: Vec<(String, SharedTextIndex)> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| e.text.iter().map(|(c, v)| (c.clone(), v.clone())).collect())
            .unwrap_or_default();
        if indexes.is_empty() {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        for (column, shared) in indexes {
            let column = &column;
            let Some(ci) = schema.iter().position(|c| c == column) else {
                continue;
            };
            let Some(text) = cells.get(ci) else { continue };
            let mut index = shared.write().unwrap();
            index_delete_row(&mut index, seq as u64);
            index_row(&mut index, seq as u64, text);
            index.maybe_seal(self.memindex_capacity());
        }
    }

    /// Removes the cached inverted indexes of `table` (or of one column) and
    /// their sealed segment files. Called when the table or its full-text index
    /// is dropped, so segments never outlive the catalog entry.
    fn drop_text_index_storage(&self, table: &str, column: Option<&str>) {
        let tkey = self.table_key(table);
        let mut removed: Vec<String> = Vec::new();
        {
            let mut tables = self.tables.lock().unwrap();
            if let Some(entry) = tables.get_mut(&tkey) {
                let columns: Vec<String> = match column {
                    Some(c) => vec![c.to_string()],
                    None => entry.text.keys().cloned().collect(),
                };
                for c in columns {
                    if let Some(idx) = entry.text.remove(&c) {
                        if let Ok(g) = idx.read() {
                            removed.push(g.dir.clone());
                        }
                    }
                }
            }
        }
        for dir in removed {
            let _ = std::fs::remove_dir_all(dir);
        }
        self.drop_multivector_storage(table, column);
        self.drop_sparse_storage(table, column);
    }

    /// Drops deleted rows from every inverted index of `table`.
    fn drop_text_index_rows(&self, table: &str, seqs: &[usize]) {
        let tkey = self.table_key(table);
        let indexes: Vec<SharedTextIndex> = self
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .map(|e| e.text.values().cloned().collect())
            .unwrap_or_default();
        for shared in indexes {
            let mut index = shared.write().unwrap();
            for seq in seqs {
                index_delete_row(&mut index, *seq as u64);
            }
        }
    }

    /// Live rows with only column `idx` parsed. A vector scan over a wide table
    /// must not decode every column: the text of all rows is what made
    /// concurrent KNN queries exhaust memory.
    fn fetch_rows_with_ids_projected(
        &self,
        table: &str,
        idx: usize,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let prefix = row_prefix(table);
        let dpref = del_prefix(table);
        let mut deleted: std::collections::HashSet<usize> = Default::default();
        for (k, _) in self.storage.store.scan_prefix(&dpref)? {
            if let Some(raw) = k.get(dpref.len()..) {
                if let Ok(s) = std::str::from_utf8(raw) {
                    if let Ok(n) = s.parse::<usize>() {
                        deleted.insert(n);
                    }
                }
            }
        }
        let mut rows = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            if deleted.contains(&seq) {
                continue;
            }
            let text = String::from_utf8_lossy(&v);
            let cell = text.split('\t').nth(idx).unwrap_or("");
            let mut cells = vec![Value::Null; idx + 1];
            cells[idx] = self.decode_cell(table, cell)?;
            rows.push((seq, cells));
        }
        Ok(rows)
    }

    /// Like `fetch_rows_with_ids_projected`, but including tombstoned rows.
    /// Corpus statistics keep counting deleted documents (as the reference's
    /// segments do until compaction), so the index must see them too.
    fn fetch_rows_with_ids_projected_all(
        &self,
        table: &str,
        idx: usize,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut rows = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            let text = String::from_utf8_lossy(&v);
            // A stored row may omit trailing columns that carry a DEFAULT; the
            // index builders must see the same value a scan would.
            let cell = match text.split('\t').nth(idx) {
                Some(c) => self.decode_cell(table, c)?,
                None => match defaults.get(idx).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                },
            };
            let mut cells = vec![Value::Null; idx + 1];
            cells[idx] = cell;
            rows.push((seq, cells));
        }
        Ok(rows)
    }

    /// Rows for `ids`, parsing only the `cols` columns (others are `Null`).
    fn fetch_rows_cols(
        &self,
        table: &str,
        ids: &[usize],
        cols: &[usize],
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        let mut out = Vec::with_capacity(ids.len());
        for &seq in ids {
            if self.storage.store.get(&del_key(table, seq))?.is_some() {
                continue;
            }
            let Some(v) = self.storage.store.get(&row_key(table, seq))? else {
                continue;
            };
            let text = String::from_utf8_lossy(&v);
            let mut cells = vec![Value::Null; text.split('\t').count()];
            for &c in cols {
                if let Some(cell) = text.split('\t').nth(c) {
                    cells[c] = self.decode_cell(table, cell)?;
                }
            }
            out.push((seq, cells));
        }
        Ok(out)
    }

    /// Rows matched by an equality on an indexed column, or the whole table
    /// when no such index applies. Point deletes/updates over a big table must
    /// not materialise every row.
    fn rows_for_predicate(
        &self,
        table: &str,
        where_pred: Option<&Predicate>,
    ) -> Result<Vec<(usize, Vec<Value>)>> {
        if let Some((col, CmpOp::Eq, val)) = where_pred.and_then(|p| p.as_column_literal()) {
            if let Some(ids) = self.index_lookup(table, col, val) {
                return self.fetch_some_rows_with_ids(table, &ids);
            }
        }
        self.fetch_rows_with_ids(table)
    }

    /// Parses a stored row, padding trailing cells from the column defaults.
    fn parse_stored_row(
        &self,
        table: &str,
        defaults: &[Option<String>],
        v: &[u8],
    ) -> Result<Vec<Value>> {
        let text = String::from_utf8_lossy(v);
        let mut cells: Vec<Value> = Vec::new();
        for cell in text.split('\t') {
            cells.push(self.decode_cell(table, cell)?);
        }
        while cells.len() < defaults.len() {
            let i = cells.len();
            cells.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                Some(d) => Value::from_literal(&default_literal(d)),
                None => Value::Null,
            });
        }
        Ok(cells)
    }

    /// Per-column declared type text for `table`.
    fn column_types(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .types)
    }

    /// The names of the stored snapshots (`CREATE SNAPSHOT`).
    pub fn snapshot_names(&self) -> Result<Vec<String>> {
        let mut out = Vec::new();
        for (k, _) in self.storage.store.scan_prefix(b"snap:")? {
            out.push(String::from_utf8_lossy(&k[5..]).to_string());
        }
        out.sort();
        Ok(out)
    }

    /// Records the comment given to `CREATE INDEX` (the SDK reads it back from
    /// `SHOW INDEX`).
    pub fn set_index_comment(&self, table: &str, index: &str, comment: &str) {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .entry(key)
            .or_default()
            .comments
            .insert(index.to_string(), comment.to_string());
    }

    /// The comment recorded for `index` on `table`, if any.
    pub fn index_comment(&self, table: &str, index: &str) -> Option<String> {
        let key = self.table_key(table);
        self.tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|e| e.comments.get(index).cloned())
    }

    /// True unless `result_cache` is explicitly `off`.
    /// Rows a single imported segment can hold (`1024 * 8192`).
    pub const SEGMENT_ROW_CAPACITY: usize = 1024 * 8192;

    /// Rows committed per block while streaming a `COPY … FROM`.
    const COPY_BATCH_ROWS: usize = 8192;

    /// Number of segments `table` has: at least one per write batch, and more
    /// when a single import exceeds the segment capacity.
    /// The table's segments with their row counts, for `SHOW SEGMENTS`. A table
    /// restored from disk has no allocator history yet, so its rows report as
    /// segment 0.
    pub fn segments(&self, table: &str) -> Vec<(u32, u32)> {
        let key = self.table_key(table);
        let mut out: Vec<(u32, u32)> = self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.segments.values().map(|s| (s.id, s.rows)).collect())
            .unwrap_or_default();
        if out.is_empty() {
            let rows = self.row_count(table).unwrap_or(0);
            if rows > 0 {
                out.push((0, rows as u32));
            }
        }
        out
    }

    pub fn segment_count(&self, table: &str) -> usize {
        let key = self.table_key(table);
        let rows = self.row_count(table).unwrap_or(0);
        let batches = match self
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .map(|e| e.write_batches)
        {
            Some(b) => b.max(1),
            // Never written in this process: an empty table has no segment,
            // a table loaded from disk has at least one.
            None => usize::from(rows > 0),
        };
        batches.max(rows.div_ceil(Self::SEGMENT_ROW_CAPACITY).max(1))
    }

    /// Encodes one table (schema, indexes, rows) into the snapshot blob format
    /// written by `CREATE SNAPSHOT … ON TABLE` and by database snapshots.
    fn snapshot_table_blob(&self, table: &str) -> Result<String> {
        let meta = self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let rows = self.fetch_rows_with_ids(table)?;
        let mut out = String::new();
        out.push_str(&meta.name);
        out.push(SNAP_REC);
        out.push_str(&meta.columns.join(&SNAP_FLD.to_string()));
        out.push(SNAP_REC);
        let defs: Vec<String> = meta
            .defaults
            .iter()
            .map(|d| match d {
                Some(v) => format!("1{v}"),
                None => "0".to_string(),
            })
            .collect();
        out.push_str(&defs.join(&SNAP_FLD.to_string()));
        out.push(SNAP_REC);
        let nns: Vec<String> = meta
            .not_nulls
            .iter()
            .map(|b| if *b { "1" } else { "0" }.to_string())
            .collect();
        out.push_str(&nns.join(&SNAP_FLD.to_string()));
        out.push(SNAP_REC);
        out.push_str(&meta.types.join(&SNAP_FLD.to_string()));
        out.push(SNAP_REC);
        // The snapshot carries the table's indexes: name, column, type and the
        // `WITH (…)` options, so a restore recreates the same index kinds.
        let idxs: Vec<String> = self
            .storage
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|im| im.table == *table)
            .map(|im| {
                let opts = im
                    .options
                    .iter()
                    .map(|(k, v)| format!("{k}={v}"))
                    .collect::<Vec<_>>()
                    .join(",");
                format!(
                    "{}{SNAP_FLD}{}{SNAP_FLD}{}{SNAP_FLD}{}",
                    im.name, im.column, im.index_type, opts
                )
            })
            .collect();
        // Indexes use their own separator: SNAP_REC also terminates the block,
        // so encoding them with SNAP_REC would make the decoder treat the 2nd..
        // index as a data row.
        out.push_str(&idxs.join("\n"));
        out.push(SNAP_REC);
        for (_, row) in &rows {
            out.push_str(
                &row.iter()
                    .map(|v| v.to_slt_string())
                    .collect::<Vec<_>>()
                    .join(&SNAP_FLD.to_string()),
            );
            out.push(SNAP_REC);
        }
        Ok(out)
    }

    /// Recreates a table from a blob written by [`Self::snapshot_table_blob`],
    /// replacing any current incarnation of it.
    fn restore_table_blob(&self, text: &str) -> Result<()> {
        let mut recs = text.split(SNAP_REC);
        let table = recs.next().unwrap_or_default().to_string();
        let columns: Vec<String> = recs
            .next()
            .unwrap_or_default()
            .split(SNAP_FLD)
            .map(|s| s.to_string())
            .filter(|s| !s.is_empty())
            .collect();
        let defaults: Vec<Option<String>> = recs
            .next()
            .unwrap_or_default()
            .split(SNAP_FLD)
            .map(|s| s.strip_prefix('1').map(|v| v.to_string()))
            .collect();
        let not_nulls: Vec<bool> = recs
            .next()
            .unwrap_or_default()
            .split(SNAP_FLD)
            .map(|s| s == "1")
            .collect();
        let types: Vec<String> = recs
            .next()
            .unwrap_or_default()
            .split(SNAP_FLD)
            .map(|s| s.to_string())
            .collect();
        // name FLD column [FLD type [FLD options]]
        #[allow(clippy::type_complexity)]
        let indexes: Vec<(String, String, String, Vec<(String, String)>)> = recs
            .next()
            .unwrap_or_default()
            .split('\n')
            .filter(|s| !s.is_empty())
            .map(|s| {
                let mut f = s.split(SNAP_FLD);
                let name = f.next().unwrap_or_default().to_string();
                let column = f.next().unwrap_or_default().to_string();
                let ty = f.next().unwrap_or_default().to_string();
                let opts = f
                    .next()
                    .unwrap_or_default()
                    .split(',')
                    .filter(|p| !p.is_empty())
                    .filter_map(|p| p.split_once('='))
                    .map(|(k, v)| (k.to_string(), v.to_string()))
                    .collect();
                (name, column, ty, opts)
            })
            .collect();
        let rows: Vec<String> = recs.map(|s| s.to_string()).collect();
        // replace any current incarnation of the table
        if self.storage.catalog.get_table_by_name(&table)?.is_some() {
            self.storage.catalog.drop_table(&table)?;
            for (k, _) in self.storage.store.scan_prefix(&row_prefix(&table))? {
                self.storage.store.delete(&k)?;
            }
            for (k, _) in self.storage.store.scan_prefix(&del_prefix(&table))? {
                self.storage.store.delete(&k)?;
            }
        }
        self.storage.catalog.create_table_typed(
            &table,
            &columns,
            &defaults,
            &not_nulls,
            &types,
            &vec![None; columns.len()],
        )?;
        for (i, r) in rows.iter().enumerate() {
            if !r.is_empty() {
                // the snapshot blob separates cells with SNAP_FLD; the row
                // store uses tabs
                let encoded = r.split(SNAP_FLD).collect::<Vec<_>>().join("\t");
                self.storage
                    .store
                    .put(&row_key(&table, i), &encoded.into_bytes())?;
            }
        }
        // re-create the snapshot's indexes
        for (name, column, ty, opts) in indexes {
            self.storage
                .catalog
                .create_index_typed(&name, &table, &column, &ty, &opts)?;
            if self.table_schema(&table)?.iter().any(|c| c == &column) {
                self.build_index(&table, &column)?;
            }
        }
        Ok(())
    }

    /// `CREATE DATABASE SNAPSHOT <name> FOR <db>`: every table of `db` (with
    /// its schema and indexes) is written into one snapshot record.
    pub fn create_database_snapshot(&self, name: &str, db: &str) -> Result<()> {
        let prefix = format!("{db}.");
        let mut out = String::new();
        for meta in self
            .storage
            .catalog
            .list_tables()?
            .into_iter()
            .filter(|m| m.name.starts_with(&prefix))
        {
            if !out.is_empty() {
                out.push(DB_SNAP_SEP);
            }
            out.push_str(&self.snapshot_table_blob(&meta.name)?);
        }
        self.storage.store.put(&snap_key(name), out.as_bytes())?;
        Ok(())
    }

    /// `CREATE SYSTEM SNAPSHOT <name>`: every table of every database.
    pub fn create_system_snapshot(&self, name: &str) -> Result<()> {
        let mut out = String::new();
        for meta in self.storage.catalog.list_tables()? {
            if !out.is_empty() {
                out.push(DB_SNAP_SEP);
            }
            out.push_str(&self.snapshot_table_blob(&meta.name)?);
        }
        self.storage.store.put(&snap_key(name), out.as_bytes())?;
        Ok(())
    }

    /// Restores every table of a database snapshot.
    pub fn restore_database_snapshot(&self, name: &str) -> Result<()> {
        let data = self
            .storage
            .store
            .get(&snap_key(name))?
            .ok_or_else(|| Error::NotFound(format!("snapshot '{name}'")))?;
        let text = String::from_utf8_lossy(&data).to_string();
        // The snapshot's tables are named `<db>.<table>`; recreate the
        // databases first (they may have been dropped since the snapshot).
        let mut databases: Vec<String> = Vec::new();
        for blob in text.split(DB_SNAP_SEP).filter(|s| !s.is_empty()) {
            let table = blob.split(SNAP_REC).next().unwrap_or_default();
            if let Some((db, _)) = table.split_once('.') {
                if !databases.iter().any(|d| d == db) {
                    databases.push(db.to_string());
                }
            }
        }
        for db in &databases {
            self.storage.catalog.create_database(db, true)?;
        }
        for blob in text.split(DB_SNAP_SEP).filter(|s| !s.is_empty()) {
            self.restore_table_blob(blob)?;
        }
        Ok(())
    }

    /// Merges every segment of `table` into one: the live rows are re-keyed
    /// contiguously from row id 0, tombstones are dropped and the write-batch
    /// counter is reset, so `SHOW SEGMENTS` reports a single segment.
    pub fn compact_table(&self, table: &str) -> Result<()> {
        self.invalidate_row_cache(table);
        // Rows are re-keyed below; the mirror's row-id snapshot stops matching
        // the moment the first row moves.
        self.invalidate_block_mirror(table);
        let rows = self.fetch_rows_with_ids(table)?;
        // Renumber through the transaction so WAL replay reproduces the
        // compacted state: a row moved from `old` to `i` also leaves a
        // tombstone for `old`, otherwise replay would resurrect it.
        let mut t = self.txn.begin();
        let mut moved = false;
        for (i, (old, _)) in rows.iter().enumerate() {
            if *old == i {
                continue;
            }
            let v = self
                .storage
                .store
                .get(&row_key(table, *old))?
                .ok_or_else(|| Error::Storage("row vanished during compaction".into()))?;
            t.put(row_key(table, i), v);
            t.put(del_key(table, *old), b"1".to_vec());
            moved = true;
        }
        if moved {
            self.txn.commit(t)?;
            // Drop the source positions that no compacted row reuses. Keys
            // below `rows.len()` are destinations: another row may have moved
            // onto them, so deleting them would lose that row. The tombstones
            // written above keep every moved source hidden until this sweep.
            for (old, _) in &rows {
                if *old >= rows.len() {
                    self.storage.store.delete(&row_key(table, *old))?;
                }
            }
        }
        for (k, _) in self.storage.store.scan_prefix(&del_prefix(table))? {
            self.storage.store.delete(&k)?;
        }
        let key = self.table_key(table);
        {
            let mut tables = self.tables.lock().unwrap();
            let entry = tables.entry(key).or_default();
            entry.row_counter = Some((0, rows.len() as u32, true));
            entry.write_batches = 1;
            // Compaction renumbered every row.
            entry.row_cache = None;
            // Block-level indexes refer to the old row ids.
            entry.secondary.indexes.clear();
            entry.vector.clear();
            entry.block_index = None;
            entry.cpp_block_rows = None;
            entry.live_rows = Some(rows.len());
        }
        self.result_cache.lock().unwrap().clear();
        Ok(())
    }

    /// Rows held by `segment` of `table` (used by `ShowBlocks`).
    pub fn segment_rows(&self, table: &str, segment: usize) -> usize {
        let rows = self.row_count(table).unwrap_or(0);
        let by_size = rows.div_ceil(Self::SEGMENT_ROW_CAPACITY).max(1);
        if by_size == self.segment_count(table) {
            rows.saturating_sub(segment * Self::SEGMENT_ROW_CAPACITY)
                .min(Self::SEGMENT_ROW_CAPACITY)
        } else {
            rows / self.segment_count(table).max(1)
        }
    }

    /// The current value of a configuration knob (the `config` map filled by
    /// `SET CONFIG`), for the `ShowConfig` RPC. Known knobs fall back to the
    /// engine defaults the reference reports before any `SET CONFIG`.
    pub fn config_get(&self, key: &str) -> Option<String> {
        let key = key.to_ascii_lowercase();
        if let Some(v) = self.context.config.lock().unwrap().get(&key) {
            return Some(v.clone());
        }
        let default = match key.as_str() {
            "log_level" => "info",
            "result_cache" => "on",
            "cache_result_capacity" => "1024",
            "checkpoint_interval" => "0",
            "cleanup_interval" => "1000",
            _ => return None,
        };
        Some(default.to_string())
    }

    /// Every configuration knob's current value.
    pub fn config_all(&self) -> Vec<(String, String)> {
        self.context
            .config
            .lock()
            .unwrap()
            .iter()
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect()
    }

    fn result_cache_enabled(&self) -> bool {
        match self.context.config.lock().unwrap().get("result_cache") {
            Some(v) => !v.eq_ignore_ascii_case("off"),
            None => true,
        }
    }

    /// Whether new results are being cached (`off`/`suspend` disable it).
    fn result_cache_active(&self) -> bool {
        match self.context.config.lock().unwrap().get("result_cache") {
            Some(v) => !(v.eq_ignore_ascii_case("off") || v.eq_ignore_ascii_case("suspend")),
            None => true,
        }
    }

    /// Maximum cached entries (`cache_result_capacity`); unbounded by default.
    fn result_cache_capacity(&self) -> usize {
        self.context
            .config
            .lock()
            .unwrap()
            .get("cache_result_capacity")
            .and_then(|v| v.parse().ok())
            .unwrap_or(usize::MAX)
    }

    /// Validates `CREATE INDEX … USING <kind> WITH (…)`: the index kind must
    /// suit the column type and the options must be in range.
    fn validate_index(
        &self,
        table: &str,
        column: &str,
        index_type: &str,
        opts: &[(String, String)],
    ) -> Result<()> {
        let schema = self.table_schema(table)?;
        let Some(ci) = schema.iter().position(|c| c == column) else {
            // A functional-index target must be a call to a known scalar.
            // A plain arithmetic target (`c1 + 5`) is a valid functional index;
            // only a call `fn(...)` has to name a known scalar.
            let fname = column
                .split('(')
                .next()
                .unwrap_or("")
                .trim()
                .to_ascii_lowercase();
            if !column.contains('(') {
                return Ok(());
            }
            const KNOWN: &[&str] = &[
                "abs",
                "ceil",
                "char_length",
                "character_length",
                "datepart",
                "day",
                "exp",
                "floor",
                "length",
                "ln",
                "log",
                "log10",
                "log2",
                "lower",
                "ltrim",
                "md5",
                "month",
                "pow",
                "power",
                "reverse",
                "round",
                "rtrim",
                "sqrt",
                "substr",
                "substring",
                "trim",
                "trunc",
                "upper",
                "year",
            ];
            if !KNOWN.contains(&fname.as_str()) {
                return Err(Error::Invalid(format!(
                    "invalid function '{fname}' in an index expression"
                )));
            }
            return Ok(());
        };
        let ty = self
            .column_types(table)?
            .get(ci)
            .cloned()
            .unwrap_or_default()
            .to_ascii_uppercase();
        match index_type.to_ascii_uppercase().as_str() {
            "" => {}
            "BMP" => {
                if !is_sparse_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "BMP index requires a sparse column, got '{ty}'"
                    )));
                }
                for (k, v) in opts {
                    match k.as_str() {
                        "BLOCK_SIZE" => {
                            let n: i64 = v.parse().unwrap_or(-1);
                            if !(1..=256).contains(&n) {
                                return Err(Error::Invalid(format!(
                                    "block_size {v} is out of range 1..=256"
                                )));
                            }
                        }
                        "COMPRESS_TYPE" => {
                            let v = v.to_ascii_uppercase();
                            if v != "COMPRESS" && v != "RAWW" {
                                return Err(Error::Invalid(format!(
                                    "unsupported compress_type '{v}'"
                                )));
                            }
                        }
                        _ => {}
                    }
                }
            }
            "HNSW" => {
                if !is_embedding_type(&ty) && !is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "{index_type} index requires a vector column, got '{ty}'"
                    )));
                }
                // LVQ encoding is unsupported for integer element types
                let int_elem = matches!(
                    tensor_elem_type(&ty),
                    ElemType::Int8
                        | ElemType::UInt8
                        | ElemType::Int16
                        | ElemType::Int32
                        | ElemType::Int64
                );
                let bit_elem = tensor_elem_type(&ty) == ElemType::Bit;
                for (k, v) in opts {
                    if !matches!(
                        k.as_str(),
                        "M" | "EF_CONSTRUCTION"
                            | "METRIC"
                            | "ENCODE"
                            | "BUILD_TYPE"
                            | "LSG_CONFIG"
                            | "BLOCK_SIZE"
                    ) {
                        return Err(Error::Invalid(format!(
                            "invalid index parameter: unsupported HNSW option '{k}'"
                        )));
                    }
                    if k == "ENCODE" && v.eq_ignore_ascii_case("lvq") && int_elem {
                        return Err(Error::Invalid(
                            "LVQ encoding is not supported for integer embeddings".into(),
                        ));
                    }
                    // `hamming` is only defined on bit vectors.
                    if k == "METRIC" && v.eq_ignore_ascii_case("hamming") && !bit_elem {
                        return Err(Error::Invalid(format!(
                            "invalid index parameter: metric 'hamming' requires a bit embedding, got '{ty}'"
                        )));
                    }
                }
            }
            "PLAID" => {
                if !is_embedding_type(&ty) && !is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "{index_type} index requires a vector column, got '{ty}'"
                    )));
                }
                for (k, v) in opts {
                    if k == "NBITS" {
                        let n: i64 = v.parse().unwrap_or(0);
                        if n != 2 && n != 4 {
                            return Err(Error::Invalid(format!("nbits {n} must be 2 or 4")));
                        }
                    }
                }
            }
            "IVF" | "FLAT" | "EMVB" => {
                if !is_embedding_type(&ty) && !is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "{index_type} index requires a vector column, got '{ty}'"
                    )));
                }
            }
            "SECONDARY" => {
                // A secondary index covers scalars (and JSON paths).
                if is_sparse_type(&ty) || is_embedding_type(&ty) || is_tensor_type(&ty) {
                    return Err(Error::Invalid(format!(
                        "SECONDARY index requires a scalar column, got '{ty}'"
                    )));
                }
                // the cardinality hint does not apply to a JSON index
                if is_json_type(&ty) && opts.iter().any(|(k, _)| k == "CARDINALITY") {
                    return Err(Error::Invalid(
                        "cardinality is not supported for a JSON secondary index".into(),
                    ));
                }
            }
            "FULLTEXT" => {
                // A full-text index can only invert a text column.
                if !ty.starts_with("VARCHAR") {
                    return Err(Error::Invalid(format!(
                        "FULLTEXT index requires a varchar column, got '{ty}'"
                    )));
                }
                for (k, v) in opts {
                    if k == "ANALYZER" {
                        let a = v
                            .trim()
                            .trim_matches(|c| c == '\'' || c == '"')
                            .to_ascii_lowercase();
                        // `standard`, `keyword`, `whitespace`/`whitespace-…` and the
                        // CJK/English pipelines are the analyzers the engine has.
                        let known = matches!(
                            a.as_str(),
                            "standard" | "english" | "chinese" | "keyword" | "whitespace"
                        ) || a.starts_with("whitespace-")
                            || a.starts_with("whitespace_")
                            || a.starts_with("sparsegram")
                            || a == "rankfeatures";
                        if !known {
                            return Err(Error::Invalid(format!("analyzer '{a}' not found")));
                        }
                    }
                }
            }
            _ => {}
        }
        Ok(())
    }

    /// The tokeniser delimiters implied by a column's full-text analyzer:
    /// `keyword`/`whitespace` split on whitespace (`Some([])`), `whitespace-<chars>`
    /// splits on the listed characters, and any other analyzer uses the standard
    /// tokeniser (`None`).
    fn index_delims(&self, table: &str, column: &str) -> Option<Vec<char>> {
        let key = self.table_key(table);
        let guard = self.tables.lock().unwrap();
        let a = guard.get(&key)?.analyzers.get(column)?.to_ascii_lowercase();
        if a == "keyword" || a == "whitespace" {
            Some(Vec::new())
        } else {
            a.strip_prefix("whitespace")
                .map(|rest| rest.trim_start_matches(['-', '_', ' ']).chars().collect())
        }
    }

    /// Selects rows for `SEARCH MATCH TEXT` by term presence. This reproduces
    /// the *row set* (any query term, `minimum_should_match`, `topn`); the exact
    /// BM25 ordering/threshold needs the iresearch scorer, so a query carrying
    /// `threshold` yields no rows rather than wrong ones.
    fn search_text_terms(
        &self,
        table: &str,
        m: &MatchClause,
        pred: Option<&Expr>,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        // A full-text scan holds the whole table's token data; bound how many
        // run at once so a handful of concurrent readers cannot exhaust memory.
        let _scan = self.context.scan_gate.enter();
        let schema = self.table_schema(table)?;
        // A full-text search runs through an index; without one the C++ engine
        // refuses the statement.
        if m.kind.eq_ignore_ascii_case("text") {
            let indexes = self.storage.catalog.list_indexes().unwrap_or_default();
            for col in match_columns(&m.column) {
                let has = indexes.iter().any(|im| {
                    im.table == table
                        && im.column == col
                        && im.index_type.eq_ignore_ascii_case("FULLTEXT")
                });
                if !has {
                    return Err(Error::Invalid(format!(
                        "fulltext index does not exist for column '{col}'"
                    )));
                }
            }
        }
        let params = arg_str(m.args.get(1)).unwrap_or_default();
        // `threshold=<f>` keeps only documents scoring at least `f`
        let threshold: Option<f64> = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("threshold"))
            .and_then(|(_, v)| v.trim().parse().ok());
        // `bm25_param_delta_term` / `bm25_param_delta_phrase` add
        // `idf * delta` to each matching unit: `TermDocIterator::InitBM25Info`
        // folds them into `f4 = delta / (k1 + 1)` which is added to the
        // frequency term and then scaled by `idf * (k1 + 1) * weight`.
        let bm25_param = |name: &str| -> f64 {
            params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case(name))
                .and_then(|(_, v)| v.trim().parse().ok())
                .unwrap_or(0.0)
        };
        // `bm25_param_delta` is a shorthand that sets both
        // (`bound_select_statement_impl.cpp:256-257`).
        let delta_both = bm25_param("bm25_param_delta");
        let delta_term = bm25_param("bm25_param_delta_term") + delta_both;
        let delta_phrase = bm25_param("bm25_param_delta_phrase") + delta_both;
        // `operator=and` requires every query term (`or` is the default)
        let require_all = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, v)| {
                    k.trim().eq_ignore_ascii_case("operator")
                        && v.trim().eq_ignore_ascii_case("and")
                })
                .unwrap_or(false)
        });
        let query = arg_str(m.args.first()).unwrap_or_default();
        // A query ending in a boolean keyword (`... OR NOT`) is incomplete.
        let ends_with_op = query
            .split_whitespace()
            .last()
            .map(|w| {
                w.eq_ignore_ascii_case("or")
                    || w.eq_ignore_ascii_case("and")
                    || w.eq_ignore_ascii_case("not")
            })
            .unwrap_or(false);
        // ... unless an explicit `operator=` supplies the default.
        let has_operator_opt = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, _)| k.trim().eq_ignore_ascii_case("operator"))
                .unwrap_or(false)
        });
        if ends_with_op && !has_operator_opt {
            return Err(Error::Invalid(
                "parser: incomplete boolean expression in the full-text query".into(),
            ));
        }
        // field boosts from `col^w` / `col@index^w`
        let field_boosts: Vec<f64> = m
            .column
            .split(',')
            .map(|c| {
                c.split('^')
                    .nth(1)
                    .and_then(|v| v.trim().parse::<f64>().ok())
                    .unwrap_or(1.0)
            })
            .collect();
        let cols: Vec<usize> = match_columns(&m.column)
            .iter()
            .filter_map(|c| schema.iter().position(|s| s == c))
            .collect();
        if cols.is_empty() {
            // A column-less clause may still name fields inside the query.
            if let Some(groups) = parse_field_query(&query) {
                return self.search_field_query(table, &groups, pred, &params, hidden);
            }
            return Ok((Vec::new(), hidden.to_vec()));
        }
        // `whitespace`/`keyword` split only on whitespace (so `2023-01-01` stays
        // one token); `whitespace-<chars>` splits **only** on the listed
        // delimiters (the C++ `WhitespaceAnalyzer` does not also split on
        // whitespace — see `whitespace_analyzer_impl.cpp`); anything else uses
        // the standard tokenizer.
        let delims: Option<Vec<char>> = match_columns(&m.column)
            .iter()
            .find_map(|c| self.index_delims(table, c));
        // One unit per quoted phrase / bare word. The analyzer decides what a
        // `"a b"` group tokenises to, so quotes are stripped first.
        let units: Vec<(bool, Vec<String>, f64)> = query_units(&query)
            .into_iter()
            .map(|(phrase, text, boost)| (phrase, tokenize_text(&text, &delims), boost))
            .filter(|(_, toks, _)| !toks.is_empty())
            .collect();
        // One entry per matched column: `(schema index, field boost, delimiters)`.
        // `similarity=boolean` scores each field on its own, so a `keyword`
        // column next to a `standard` one must not share the other's analyzer.
        let field_specs: Vec<(usize, f64, Option<Vec<char>>)> = m
            .column
            .split(',')
            .filter_map(|spec| {
                let name = spec
                    .split('^')
                    .next()
                    .unwrap_or(spec)
                    .split('@')
                    .next()
                    .unwrap_or(spec)
                    .trim();
                let boost = spec
                    .split('^')
                    .nth(1)
                    .and_then(|v| v.trim().parse::<f64>().ok())
                    .unwrap_or(1.0);
                let ci = schema.iter().position(|s| s == name)?;
                Some((ci, boost, self.index_delims(table, name)))
            })
            .collect();
        let terms: Vec<String> = units
            .iter()
            .flat_map(|(_, u, _)| u.iter().cloned())
            .filter(|t| t != "or" && t != "and")
            .collect();
        if terms.is_empty() {
            return Ok((Vec::new(), hidden.to_vec()));
        }
        let min_should = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("minimum_should_match"))
            .map(|(_, v)| minimum_should_match(v.trim(), terms.len()))
            .unwrap_or(1);
        // With `operator=and`, each OR-separated group is a clause: a run of
        // several words must appear as a phrase (iresearch's `AndQuery`).
        let min_should = if require_all { units.len() } else { min_should };
        let topn = parse_topn_param(&params).unwrap_or(10);
        let in_filter = match m.filter.as_ref() {
            Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
            None => None,
        };
        // `filter=…` in the MATCH TEXT options narrows the candidates.
        let opt_filter = match params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("filter"))
            .map(|(_, v)| v.trim().to_string())
            .filter(|v| !v.is_empty())
        {
            Some(f) => Some(infinity_planner::predicate_expr(
                &parse_filter_predicate(&f)?,
                &schema,
            )?),
            None => None,
        };
        let boolean = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, v)| {
                    k.trim().eq_ignore_ascii_case("similarity")
                        && v.trim().eq_ignore_ascii_case("boolean")
                })
                .unwrap_or(false)
        });
        // `rank_features=<col>^<tag>^<weight>,…`: each document's `<col>` value is
        // a JSON array of `{"<tag>": <number>}` objects (its index uses the
        // `rankfeatures` analyzer), and the score gains `number * weight` for
        // every listed tag.
        let rank_features: Vec<(usize, Vec<(String, f64)>)> = {
            let spec = params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case("rank_features"))
                .map(|(_, v)| v.to_string());
            let mut groups: Vec<(usize, Vec<(String, f64)>)> = Vec::new();
            if let Some(spec) = spec {
                for item in spec.split(',') {
                    let parts: Vec<&str> = item.split('^').map(str::trim).collect();
                    let (Some(col), Some(tag)) = (parts.first(), parts.get(1)) else {
                        continue;
                    };
                    // The scoring column needs a `rankfeatures` full-text index.
                    let tkey = self.table_key(table);
                    let analyzer = self
                        .tables
                        .lock()
                        .unwrap()
                        .get(&tkey)
                        .and_then(|e| e.analyzers.get(*col).cloned())
                        .unwrap_or_default();
                    if !analyzer.eq_ignore_ascii_case("rankfeatures") {
                        if std::env::var_os("INF_DEBUG_SQL").is_some() {
                            eprintln!("[RANKF] table={table} col={col} analyzer={analyzer:?}");
                        }
                        return Err(Error::Invalid(format!(
                            "rank_features requires a rankfeatures index on '{col}'"
                        )));
                    }
                    let Some(ci) = schema.iter().position(|c| c == col) else {
                        continue;
                    };
                    let weight = parts.get(2).and_then(|w| w.parse().ok()).unwrap_or(1.0);
                    match groups.iter_mut().find(|(c, _)| *c == ci) {
                        Some((_, list)) => list.push(((*tag).to_string(), weight)),
                        None => groups.push((ci, vec![((*tag).to_string(), weight)])),
                    }
                }
            }
            groups
        };
        // Block-max WAND: an OR query over one or more fields never needs a
        // row's token vectors — the posting lists alone answer it. Filters
        // become a dead set, and `minimum_should_match` / `delta_term` /
        // boolean similarity are applied inside the scoring loop. BM25 scores
        // each field separately and sums, like `BM25Ranker`.
        if rank_features.is_empty()
            && !matches!(&delims, Some(d) if d.is_empty())
            && !field_specs.is_empty()
            && units
                .iter()
                .all(|(phrase, toks, _)| !*phrase && toks.len() == 1)
        {
            let wterms: Vec<String> = units.iter().map(|(_, toks, _)| toks[0].clone()).collect();
            // One merged index per field, with that field's weights.
            let mut merged_fields: Vec<(MergedIndex, Vec<f64>)> = Vec::new();
            for (ci, boost, d) in &field_specs {
                let name = schema[*ci].clone();
                let index = self.text_index_for(table, &name, d.clone())?;
                let guard = index.read().unwrap();
                let merged = guard.merge(&wterms);
                drop(guard);
                // Boolean similarity ignores per-term boosts (one point per
                // matched term, weighted by the field boost).
                let weights: Vec<f64> = if boolean {
                    vec![*boost; wterms.len()]
                } else {
                    units.iter().map(|(_, _, ub)| boost * ub).collect()
                };
                merged_fields.push((merged, weights));
            }
            // Tombstones still feed the corpus statistics (as the reference's
            // segments do until compaction) but never rank.
            let mut dead = self.tombstone_set(table)?;
            // A filter removes documents before ranking; evaluating it over the
            // rows and feeding the failures as tombstones keeps the
            // postings-only scoring exact.
            if pred.is_some() || in_filter.is_some() || opt_filter.is_some() {
                for (seq, row) in self.fetch_rows_with_ids(table)? {
                    let ok = pred
                        .as_ref()
                        .map(|p| p.eval_predicate(&row))
                        .unwrap_or(true)
                        && in_filter
                            .as_ref()
                            .map(|p| p.eval_predicate(&row))
                            .unwrap_or(true)
                        && opt_filter
                            .as_ref()
                            .map(|p| p.eval_predicate(&row))
                            .unwrap_or(true);
                    if !ok {
                        dead.insert(seq as u64);
                    }
                }
            }
            let mode = if boolean {
                wand::WandMode::Boolean
            } else {
                wand::WandMode::Bm25
            };
            let hits = Self::wand_scan(
                &merged_fields,
                &wterms,
                topn.max(1),
                &dead,
                delta_term,
                min_should,
                mode,
            );
            let scored: Vec<(usize, f64)> = hits
                .into_iter()
                .map(|(seq, score)| (seq as usize, score))
                .filter(|(_, score)| threshold.map(|t| *score >= t).unwrap_or(true))
                .collect();
            if !scored.is_empty() {
                let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq as u64).collect();
                let mut by_id: HashMap<usize, Vec<Value>> = self
                    .fetch_some_rows_with_ids(table, &wanted)?
                    .into_iter()
                    .collect();
                let mut out = Vec::new();
                for (seq, score) in scored {
                    let Some(mut row) = by_id.remove(&seq) else {
                        continue;
                    };
                    row.push(Value::Int(seq as i64));
                    row.push(Value::Float(score));
                    row.push(Value::Float(0.0));
                    row.push(Value::Float(score));
                    out.push(row);
                }
                if !out.is_empty() {
                    return Ok((out, hidden.to_vec()));
                }
            }
        }
        // BM25 (k1 = 1.2, b = 0.75) over the whole table; a document's length is
        // the number of indexed terms, and `similarity=boolean` scores one point
        // per matching term weighted by the field/term boosts.
        // Deleted rows still contribute to the index statistics (they are only
        // filtered from the result), so gather every stored row first.
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        // When no filter, rank-feature or boolean mode is involved the inverted
        // indexes decide the row set and supply the corpus statistics
        // (doc count, average field length, df). A query whose terms cover most
        // of the table is cheaper to scan than to gather.
        #[allow(clippy::type_complexity)]
        let index_view: Option<(Vec<usize>, Vec<(usize, f64, HashMap<String, usize>)>)> = (|| {
            if boolean
                || !rank_features.is_empty()
                || in_filter.is_some()
                || opt_filter.is_some()
                || pred.is_some()
            {
                return None;
            }
            let mut set: std::collections::HashSet<usize> = Default::default();
            let mut stats: Vec<(usize, f64, HashMap<String, usize>)> = Vec::new();
            for (ci, _, d) in &field_specs {
                let name = schema.get(*ci)?.clone();
                let index = self.text_index_for(table, &name, d.clone()).ok()?;
                let guard = index.read().ok()?;
                let merged = guard.merge(&terms);
                let n = merged.n_docs;
                let sum = merged.total_len as usize;
                let avg = if n == 0 {
                    1.0
                } else {
                    let a = sum as f64 / n as f64;
                    if a <= 0.0 {
                        1.0
                    } else {
                        a
                    }
                };
                let mut df: HashMap<String, usize> = HashMap::new();
                for t in &terms {
                    if let Some(list) = merged.postings.get(t) {
                        df.insert(t.clone(), list.len());
                        for (doc, _) in list {
                            set.insert(*doc as usize);
                        }
                    }
                }
                stats.push((n, avg, df));
            }
            let total = stats.first().map(|st| st.0).unwrap_or(0);
            if total == 0 || set.len() * 2 > total {
                return None;
            }
            Some((set.into_iter().collect(), stats))
        })(
        );
        let _ = &prefix;
        // Per document we keep only what scoring needs: the tokenised fields,
        // whether it is live and passes the filters, and the pre-computed
        // rank-features / boolean contributions. Keeping the full row here made
        // a query over 250k wide rows cost gigabytes, and several concurrent
        // readers exhausted the server's memory.
        #[allow(clippy::type_complexity)]
        let mut docs: Vec<(usize, Vec<Vec<(String, u32)>>, bool, bool, f64, usize, f64)> =
            Vec::new();
        let scanned: Vec<(usize, Vec<Value>)> = match &index_view {
            Some((ids, _)) => self.fetch_rows_cols(table, ids, &cols)?,
            None => self.fetch_rows_including_deleted(table)?,
        };
        for (seq, mut row) in scanned {
            // Rows loaded before a column was added (or from a narrower COPY)
            // take the column's default value, not NULL.
            while row.len() < defaults.len() {
                let i = row.len();
                row.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            let live = self.storage.store.get(&del_key(table, seq))?.is_none();
            let filters_hold = in_filter
                .as_ref()
                .map(|p| p.eval_predicate(&row))
                .unwrap_or(true)
                && opt_filter
                    .as_ref()
                    .map(|p| p.eval_predicate(&row))
                    .unwrap_or(true)
                && pred.map(|p| p.eval_predicate(&row)).unwrap_or(true);
            // `rank_features=<col>^<tag>^<weight>` adds `value * weight`.
            let rank_bonus: f64 = rank_features
                .iter()
                .map(|(ci, tags)| {
                    let cell = row.get(*ci).map(|v| v.to_slt_string()).unwrap_or_default();
                    rank_features_bonus(&cell, tags)
                })
                .sum();
            // `similarity=boolean` scores fields independently; compute it here
            // while the row is still in hand.
            let (mut bhits, mut bscore) = (0usize, 0.0f64);
            if boolean {
                for (ci, boost, d) in &field_specs {
                    let text = match row.get(*ci) {
                        Some(Value::Str(s)) => s.clone(),
                        Some(other) => other.to_slt_string(),
                        None => String::new(),
                    };
                    let ftoks = tokenize_index_text(&text, d);
                    let fset: std::collections::HashSet<&str> =
                        ftoks.iter().map(|x| x.as_str()).collect();
                    let kw = matches!(d, Some(dd) if dd.is_empty());
                    let matched = terms.iter().filter(|t| fset.contains(t.as_str())).count();
                    if matched > 0 {
                        bhits += matched;
                        bscore += boost * if kw { 1.0 } else { matched as f64 };
                    }
                }
            }
            // Tokenise every matched column on its own: BM25 keeps per-field
            // statistics and a `doctitle,body^5` query sums them.
            let fields: Vec<Vec<(String, u32)>> = cols
                .iter()
                .map(|&i| {
                    let text = match row.get(i) {
                        Some(Value::Str(s)) => s.clone(),
                        Some(other) => other.to_slt_string(),
                        None => String::new(),
                    };
                    tokenize_index_positions(&text, &delims)
                })
                .collect();
            docs.push((seq, fields, live, filters_hold, rank_bonus, bhits, bscore));
        }
        let n_docs = match &index_view {
            Some((_, stats)) => stats.first().map(|st| st.0).unwrap_or(1),
            None => docs.len(),
        }
        .max(1);
        // Document frequency per (field, term). Computing it inside `idf_of`
        // made scoring O(rows^2); one pass here turns it into a lookup.
        let dfs: Vec<HashMap<String, usize>> = if let Some((_, stats)) = &index_view {
            stats.iter().map(|st| st.2.clone()).collect()
        } else {
            (0..cols.len())
                .map(|f| {
                    let mut m: HashMap<String, usize> = HashMap::new();
                    for (_, fields, ..) in &docs {
                        // A term counts once per document, however often it occurs.
                        let mut seen: std::collections::HashSet<&str> =
                            std::collections::HashSet::new();
                        for (t, _) in &fields[f] {
                            if seen.insert(t.as_str()) {
                                *m.entry(t.clone()).or_insert(0) += 1;
                            }
                        }
                    }
                    m
                })
                .collect()
        };
        // One BM25 corpus per column (Lucene keeps statistics per field).
        let avg_lens: Vec<f64> = if let Some((_, stats)) = &index_view {
            stats.iter().map(|st| st.1).collect()
        } else {
            (0..cols.len())
                .map(|f| {
                    let sum: f64 = docs.iter().map(|d| d.1[f].len() as f64).sum();
                    let a = sum / docs.len().max(1) as f64;
                    if a <= 0.0 {
                        1.0
                    } else {
                        a
                    }
                })
                .collect()
        };
        // A `keyword`/`whitespace` field scores constantly: one field-boost
        // point per matching document (not per query term).
        let keyword_field = matches!(&delims, Some(d) if d.is_empty());
        let mut scored: Vec<(usize, f64)> = Vec::new();
        for (seq, fields, live, filters_hold, rank_bonus, bool_hits, bool_score) in &docs {
            // Filters and the live check were evaluated while the row was in
            // hand; a row that fails either never reaches the ranking.
            if !live || !filters_hold {
                continue;
            }
            // `similarity=boolean` scores each field independently: a
            // `keyword`/`whitespace` field contributes its boost once when any
            // query term is present, a standard field contributes one boost per
            // matching term.
            if boolean {
                let bscore = bool_score + rank_bonus;
                if *bool_hits >= min_should
                    && *bool_hits > 0
                    && threshold.map(|t| bscore >= t).unwrap_or(true)
                {
                    scored.push((*seq, bscore));
                }
                continue;
            }
            let mut score = 0.0f64;
            // Units matched in *any* field, for `minimum_should_match`.
            let mut unit_hits = vec![false; units.len()];
            for (f, toks) in fields.iter().enumerate() {
                if toks.is_empty() {
                    continue;
                }
                let boost = field_boosts.get(f).copied().unwrap_or(1.0);
                let len = toks.len() as f64;
                let avg_len = avg_lens[f];
                let mut counts: HashMap<&str, usize> = HashMap::new();
                // `(term, position)` pairs, for phrase matching.
                let mut at: HashMap<u32, Vec<&str>> = HashMap::new();
                for (t, pos) in toks {
                    *counts.entry(t.as_str()).or_insert(0) += 1;
                    at.entry(*pos).or_default().push(t.as_str());
                }
                let idf_of = |term: &str| -> f64 {
                    let df = dfs[f].get(term).copied().unwrap_or(0).max(1) as f64;
                    (1.0 + (n_docs as f64 - df + 0.5) / (df + 0.5)).ln()
                };
                if keyword_field {
                    let mut any = false;
                    for (ui, (_, unit, _)) in units.iter().enumerate() {
                        if unit.iter().any(|t| counts.contains_key(t.as_str())) {
                            unit_hits[ui] = true;
                            any = true;
                        }
                    }
                    if any {
                        score += boost;
                    }
                    continue;
                }
                for (ui, (phrase, unit, uboost)) in units.iter().enumerate() {
                    let uboost = *uboost;
                    if (*phrase || require_all) && unit.len() > 1 {
                        // A phrase scores like one term whose `tf` is the phrase
                        // frequency and whose `idf` is the sum over its terms
                        // (Lucene `PhraseWeight`).
                        let freq = phrase_freq(&at, unit);
                        if freq == 0 {
                            continue;
                        }
                        unit_hits[ui] = true;
                        let idf_sum: f64 = unit.iter().map(|t| idf_of(t)).sum();
                        let tf = freq as f64;
                        let norm = 2.2 * tf / (tf + 1.2 * (0.25 + 0.75 * len / avg_len));
                        score += idf_sum * (norm + delta_phrase) * boost * uboost;
                        continue;
                    }
                    for term in unit {
                        let tf = match counts.get(term.as_str()) {
                            Some(c) => *c as f64,
                            None => continue,
                        };
                        unit_hits[ui] = true;
                        let idf = idf_of(term);
                        let norm = 2.2 * tf / (tf + 1.2 * (0.25 + 0.75 * len / avg_len));
                        score += idf * (norm + delta_term) * boost * uboost;
                    }
                }
            }
            let hits: usize = if require_all {
                // one count per required clause
                unit_hits.iter().filter(|m| **m).count()
            } else {
                units
                    .iter()
                    .zip(unit_hits.iter())
                    .filter(|(_, m)| **m)
                    .map(|((_, u, _), _)| u.len())
                    .sum()
            };
            score += rank_bonus;
            if hits >= min_should && hits > 0 && threshold.map(|t| score >= t).unwrap_or(true) {
                scored.push((*seq, score));
            }
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq as u64).collect();
        let mut by_id: HashMap<usize, Vec<Value>> = self
            .fetch_some_rows_with_ids(table, &wanted)?
            .into_iter()
            .collect();
        let mut out = Vec::new();
        for (seq, score) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(Value::Float(score));
            row.push(Value::Float(0.0));
            row.push(Value::Float(score));
            out.push(row);
        }
        Ok((out, hidden.to_vec()))
    }

    /// Scores a column-less `MATCH TEXT ('', '(doc:…)^w …')` clause: each group
    /// multiplies its boost by the sum of its fields' boolean scores (a
    /// `keyword`/`whitespace` field counts once, a standard field once per
    /// matching term). A document qualifies when at least `minimum_should_match`
    /// groups score.
    #[allow(clippy::type_complexity)]
    fn search_field_query(
        &self,
        table: &str,
        groups: &[(f64, Vec<(String, String)>)],
        pred: Option<&Expr>,
        params: &str,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let schema = self.table_schema(table)?;
        let mut fields: Vec<(String, usize, Option<Vec<char>>)> = Vec::new();
        for (_, terms) in groups {
            for (f, _) in terms {
                if f.is_empty() || fields.iter().any(|(n, _, _)| n == f) {
                    continue;
                }
                if let Some(ci) = schema.iter().position(|s| s == f) {
                    let d = self.index_delims(table, f);
                    fields.push((f.clone(), ci, d));
                }
            }
        }
        // `minimum_should_match` counts *matched clauses*, where a quoted phrase
        // (`doc:"a b"`) is one clause that matches only when all of its tokens
        // are present.
        let units_total: usize = groups.iter().map(|(_, terms)| terms.len()).sum();
        let min_should = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("minimum_should_match"))
            .map(|(_, v)| minimum_should_match(v.trim(), units_total))
            .unwrap_or(1);
        let topn = parse_topn_param(params).unwrap_or(10);
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut scored: Vec<(usize, f64)> = Vec::new();
        for (k, v) in self.storage.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            let mut row = self.parse_stored_row(table, &defaults, &v)?;
            while row.len() < defaults.len() {
                let i = row.len();
                row.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            if self.storage.store.get(&del_key(table, seq))?.is_some() {
                continue;
            }
            if let Some(p) = pred {
                if !p.eval_predicate(&row) {
                    continue;
                }
            }
            let tokens: Vec<std::collections::HashSet<String>> = fields
                .iter()
                .map(|(_, ci, d)| {
                    let text = match row.get(*ci) {
                        Some(Value::Str(s)) => s.clone(),
                        Some(o) => o.to_slt_string(),
                        None => String::new(),
                    };
                    tokenize_index_text(&text, d).into_iter().collect()
                })
                .collect();
            let mut score = 0.0f64;
            // `units_matched` drives `minimum_should_match`: a phrase clause
            // counts only when every token it expands to is present.
            let mut units_matched = 0usize;
            for (boost, terms) in groups {
                let mut gscore = 0.0f64;
                for (f, t) in terms {
                    let Some(fi) = fields
                        .iter()
                        .position(|(name, _, _)| f == name || f.is_empty())
                    else {
                        continue;
                    };
                    let d = &fields[fi].2;
                    let sub = tokenize_text(t, d);
                    if !sub.is_empty() && sub.iter().all(|x| tokens[fi].contains(x)) {
                        units_matched += 1;
                    }
                }
                for (fi, (name, _, d)) in fields.iter().enumerate() {
                    let kw = matches!(d, Some(dd) if dd.is_empty());
                    let mut matched = 0usize;
                    for (f, t) in terms {
                        if f == name || f.is_empty() {
                            // A quoted phrase is split by the field's own
                            // analyzer (one token for `keyword`).
                            matched += tokenize_text(t, d)
                                .iter()
                                .filter(|x| tokens[fi].contains(*x))
                                .count();
                        }
                    }
                    if matched > 0 {
                        gscore += if kw { 1.0 } else { matched as f64 };
                    }
                }
                score += boost * gscore;
            }
            if units_matched >= min_should && units_matched > 0 {
                scored.push((seq, score));
            }
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        let mut by_id: HashMap<usize, Vec<Value>> =
            self.fetch_rows_with_ids(table)?.into_iter().collect();
        let mut out = Vec::new();
        for (seq, score) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(Value::Float(score));
            row.push(Value::Float(0.0));
            row.push(Value::Float(score));
            out.push(row);
        }
        Ok((out, hidden.to_vec()))
    }

    /// Rejects `SUM`/`AVG` over a non-numeric column.
    /// A WHERE term used on its own (`WHERE c1`) must itself be boolean; the
    /// reference rejects a numeric column or an arithmetic expression there.
    fn validate_where_predicate(&self, table: &str, p: &infinity_parser::Predicate) -> Result<()> {
        use infinity_parser::Predicate;
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        fn bool_column(schema: &[String], types: &[String], name: &str) -> bool {
            schema
                .iter()
                .position(|c| c == name)
                .and_then(|i| types.get(i))
                .map(|t| t.trim().to_ascii_uppercase().starts_with("BOOL"))
                .unwrap_or(false)
        }
        match p {
            Predicate::Bool(a, _, b) => {
                self.validate_where_predicate(table, a)?;
                self.validate_where_predicate(table, b)
            }
            Predicate::Not(a) => self.validate_where_predicate(table, a),
            Predicate::IsTrue(e) => match e {
                ExprAst::Column(c) if bool_column(&schema, &types, c) => Ok(()),
                ExprAst::Column(_) => Err(Error::Invalid(
                    "filter requires a boolean expression".into(),
                )),
                // A function call may yield a boolean (CONTAINS, FILTER_TEXT…);
                // its own validation applies. The numeric pseudo-columns do not.
                ExprAst::Func { name, .. }
                    if ["ROW_ID", "SCORE", "DISTANCE", "SIMILARITY"]
                        .iter()
                        .any(|n| name.eq_ignore_ascii_case(n)) =>
                {
                    Err(Error::Invalid(
                        "filter requires a boolean expression".into(),
                    ))
                }
                ExprAst::Func { .. } => Ok(()),
                _ => Err(Error::Invalid(
                    "filter requires a boolean expression".into(),
                )),
            },
            _ => Ok(()),
        }
    }

    fn validate_aggregate_args(&self, table: &str, items: &[SelectItem]) -> Result<()> {
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        for it in items {
            let SelectItem::Aggregate(a) = it else {
                continue;
            };
            if !matches!(a.func, AggFunc::Sum | AggFunc::Avg) {
                continue;
            }
            for (j, name) in a.args.iter().enumerate() {
                // a computed argument is validated by its own expression path
                if a.arg_exprs.get(j).map(|o| o.is_some()).unwrap_or(false) {
                    continue;
                }
                let Some(ci) = schema.iter().position(|c| c == name) else {
                    continue;
                };
                let ty = types
                    .get(ci)
                    .cloned()
                    .unwrap_or_default()
                    .to_ascii_uppercase();
                let numeric = matches!(
                    ty.as_str(),
                    "TINYINT"
                        | "SMALLINT"
                        | "INT"
                        | "INTEGER"
                        | "BIGINT"
                        | "FLOAT"
                        | "DOUBLE"
                        | "REAL"
                        | "TINYINT8"
                        | "UINT8"
                        | "FLOAT16"
                        | "BFLOAT16"
                );
                if !numeric {
                    return Err(Error::Invalid(format!(
                        "{}({name}): argument type '{ty}' is not numeric",
                        a.func.as_str()
                    )));
                }
            }
        }
        Ok(())
    }

    /// Per-column defaults (as `.slt` text) for `table`.
    fn table_defaults(&self, table: &str) -> Result<Vec<Option<String>>> {
        Ok(self
            .storage
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .defaults)
    }

    /// Loads a CSV file into `table` (`COPY t FROM …`). The path may be absolute
    /// or the Infinity fixture path `/var/infinity/test_data/…`, which is mapped
    /// onto `INFINITY_DATA_DIR` (default `test/data`) if present.
    fn copy_from(&self, table: &str, path: &str, opts: &[(String, String)]) -> Result<QueryResult> {
        if self.storage.catalog.get_table_by_name(table)?.is_none() {
            return Err(Error::NotFound(format!("table '{table}'")));
        }
        let resolved = resolve_data_path(path);
        // Read raw bytes: `FORMAT FVECS` is binary, everything else is text.
        // Missing fixtures stay a statement-level success with no rows.
        let bytes_in = match std::fs::read(&resolved) {
            Ok(b) => b,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        // `FORMAT FVECS` is a binary vector file: `i32 dim` then `dim` f32s per
        // record. It can only target a table whose single column is the vector.
        if opts
            .iter()
            .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case("FVECS"))
        {
            let types = self.column_types(table)?;
            if types.len() != 1 {
                return Err(Error::Invalid(format!(
                    "import: fvecs needs a table with exactly one vector column, got {}",
                    types.len()
                )));
            }
            let mut rows: Vec<Vec<Literal>> = Vec::new();
            let bytes = bytes_in.as_slice();
            let mut pos = 0usize;
            while pos + 4 <= bytes.len() {
                let dim = i32::from_le_bytes(bytes[pos..pos + 4].try_into().unwrap()) as usize;
                pos += 4;
                // A record must fit; a truncated/garbage header is a format error.
                if dim == 0 || pos + dim * 4 > bytes.len() {
                    return Err(Error::Invalid(
                        "import: malformed fvecs file (bad record header)".into(),
                    ));
                }
                let vals: Vec<String> = (0..dim)
                    .map(|i| {
                        let b = &bytes[pos + i * 4..pos + i * 4 + 4];
                        format!("{}", f32::from_le_bytes(b.try_into().unwrap()))
                    })
                    .collect();
                pos += dim * 4;
                rows.push(vec![Literal::Str(format!("[{}]", vals.join(",")))]);
            }
            if rows.is_empty() || pos != bytes.len() {
                return Err(Error::Invalid(
                    "import: malformed fvecs file (trailing bytes)".into(),
                ));
            }
            let base = self.reserve_rows(table, rows.len(), true)?;
            self.note_index_insert(table, rows.len());
            let mut t = self.txn.begin();
            for (k, row) in rows.iter().enumerate() {
                let encoded = encode_row(row, &types);
                t.put(row_key(table, base + k), encoded.into_bytes());
            }
            self.txn.commit(t)?;
            return Ok(QueryResult::Inserted { count: rows.len() });
        }
        let delim = opts
            .iter()
            .find(|(k, _)| k == "DELIMITER")
            .map(|(_, v)| decode_delimiter(v))
            .unwrap_or(',');
        let types_for_scan = self.column_types(table)?;
        let defaults = self.table_defaults(table)?;
        let format_of = |name: &str| {
            opts.iter()
                .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case(name))
        };
        // `FORMAT JSONL` (one object per line) and `FORMAT JSON` (one array)
        // are small in practice; the CSV path streams.
        if format_of("JSONL") || format_of("JSON") {
            let text = String::from_utf8_lossy(&bytes_in).into_owned();
            if format_of("JSONL") {
                return self.copy_from_jsonl(table, &text, &types_for_scan, &defaults);
            }
            return self.copy_from_json(table, &text, &types_for_scan, &defaults);
        }
        // CSV: read line by line and commit in blocks, so an 8-million-row
        // import does not hold the file and its parsed rows in memory.
        let file = match std::fs::File::open(&resolved) {
            Ok(f) => f,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let mut reader = std::io::BufReader::new(file);
        let mut total = 0usize;
        let mut first_batch = true;
        loop {
            let mut batch: Vec<Vec<Literal>> = Vec::with_capacity(Self::COPY_BATCH_ROWS);
            let mut eof = false;
            while batch.len() < Self::COPY_BATCH_ROWS {
                let mut line = String::new();
                use std::io::BufRead;
                match reader.read_line(&mut line) {
                    Ok(0) => {
                        eof = true;
                        break;
                    }
                    Ok(_) => {}
                    Err(e) => return Err(Error::io(&resolved, e)),
                }
                let line = line.trim_end_matches(['\n', '\r']);
                if line.is_empty() {
                    continue;
                }
                let fields: Vec<String> = csv_fields(line, delim);
                let mut cells: Vec<String> = fields.iter().map(|f| normalize_cell(f)).collect();
                if cells.len() > types_for_scan.len() {
                    return Err(Error::Invalid(format!(
                        "import: the file has {} columns but the table has {}",
                        cells.len(),
                        types_for_scan.len()
                    )));
                }
                for (i, c) in cells.iter_mut().enumerate() {
                    if c.is_empty() {
                        if let Some(Some(d)) = defaults.get(i) {
                            *c = d.clone();
                        }
                    }
                }
                for (i, c) in cells.iter().enumerate() {
                    if let Some(t) = types_for_scan.get(i) {
                        validate_insert_value(t, c.trim()).map_err(|e| match e {
                            Error::Invalid(m) if m.starts_with("parser:") => Error::Invalid(m),
                            other => Error::Invalid(format!("import: {other}")),
                        })?;
                    }
                }
                batch.push(cells.into_iter().map(Literal::Str).collect());
            }
            if !batch.is_empty() {
                let base = self.reserve_rows(table, batch.len(), first_batch)?;
                first_batch = false;
                self.note_index_insert(table, batch.len());
                let mut t = self.txn.begin();
                for (k, row) in batch.iter().enumerate() {
                    let encoded = self.encode_row_stored(table, row, &types_for_scan)?;
                    t.put(row_key(table, base + k), encoded.into_bytes());
                }
                self.txn.commit(t)?;
                for (k, row) in batch.iter().enumerate() {
                    self.update_indexes_on_insert(table, base + k, row)?;
                    let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
                    self.update_text_indexes(table, base + k, &texts);
                    self.update_vector_caches(table, base + k, &texts);
                }
                self.dump_hnsw_over_capacity(table);
                total += batch.len();
            }
            if eof {
                break;
            }
        }
        Ok(QueryResult::Inserted { count: total })
    }

    /// Loads a `FORMAT JSONL` copy: one JSON object per line, keyed by column
    /// name. Sparse columns come as `{index: value}` objects (float) or index
    /// arrays (bit); everything else is a scalar.
    fn copy_from_jsonl(
        &self,
        table: &str,
        text: &str,
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use infinity_expression::json::Json;
        let mut objects: Vec<Vec<(String, Json)>> = Vec::new();
        for line in text.lines() {
            let line = line.trim();
            if line.is_empty() {
                continue;
            }
            if let Some(Json::Obj(fields)) = infinity_expression::json::parse(line) {
                objects.push(fields);
            }
        }
        self.insert_json_objects(table, &objects, types, defaults)
    }

    /// Loads a `FORMAT JSON` copy: the whole file is one JSON array whose
    /// elements are objects keyed by column name.
    fn copy_from_json(
        &self,
        table: &str,
        text: &str,
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use infinity_expression::json::Json;
        let mut objects: Vec<Vec<(String, Json)>> = Vec::new();
        if let Some(Json::Arr(items)) = infinity_expression::json::parse(text.trim()) {
            for it in items {
                if let Json::Obj(fields) = it {
                    objects.push(fields);
                }
            }
        }
        self.insert_json_objects(table, &objects, types, defaults)
    }

    /// Inserts one row per JSON object (keyed by column name). Sparse columns
    /// come as `{index: value}` objects (float) or index arrays (bit);
    /// everything else is a scalar.
    fn insert_json_objects(
        &self,
        table: &str,
        objects: &[Vec<(String, infinity_expression::json::Json)>],
        types: &[String],
        defaults: &[Option<String>],
    ) -> Result<QueryResult> {
        use infinity_expression::json::Json;
        let schema = self.table_schema(table)?;
        let mut rows: Vec<Vec<Literal>> = Vec::new();
        for fields in objects {
            let mut cells: Vec<Literal> = Vec::with_capacity(schema.len());
            for (i, name) in schema.iter().enumerate() {
                let ty = types.get(i).cloned().unwrap_or_default();
                let value = fields.iter().find(|(k, _)| k == name).map(|(_, v)| v);
                let cell = match value {
                    Some(Json::Obj(pairs)) if is_sparse_type(&ty) => {
                        let mut ps: Vec<(i64, f64)> = pairs
                            .iter()
                            .filter_map(|(k, v)| {
                                let idx = k.trim().parse::<i64>().ok()?;
                                let val = match v {
                                    Json::Int(x) => *x as f64,
                                    Json::Float(x) => *x,
                                    _ => return None,
                                };
                                Some((idx, val))
                            })
                            .collect();
                        ps.sort_by_key(|(i, _)| *i);
                        let body: Vec<String> =
                            ps.iter().map(|(i, v)| format!("{i}:{v:.6}")).collect();
                        Literal::Str(format!("[{}]", body.join(",")))
                    }
                    Some(Json::Arr(items)) if is_sparse_type(&ty) => {
                        let idx: Vec<String> = items
                            .iter()
                            .filter_map(|x| match x {
                                Json::Int(i) => Some(i.to_string()),
                                Json::Float(f) => Some((*f as i64).to_string()),
                                _ => None,
                            })
                            .collect();
                        Literal::Str(format!("[{}]", idx.join(",")))
                    }
                    // an absent / null field takes the column default
                    Some(Json::Null) | None => defaults
                        .get(i)
                        .and_then(|d| d.as_deref())
                        .map(default_literal)
                        .unwrap_or(Literal::Null),
                    Some(other) => Literal::Str(json_scalar_text(other)),
                };
                cells.push(cell);
            }
            // validate each typed cell (sparse range, tensor dimension)
            for (i, cell) in cells.iter().enumerate() {
                if let Some(ty) = types.get(i) {
                    validate_insert_value(ty, cell.to_slt_string().trim())?;
                }
            }
            rows.push(cells);
        }
        if rows.is_empty() {
            return Ok(QueryResult::Inserted { count: 0 });
        }
        let base = self.reserve_rows(table, rows.len(), true)?;
        self.note_index_insert(table, rows.len());
        let mut t = self.txn.begin();
        for (k, row) in rows.iter().enumerate() {
            let encoded = self.encode_row_stored(table, row, types)?;
            t.put(row_key(table, base + k), encoded.into_bytes());
        }
        self.txn.commit(t)?;
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row)?;
            let texts: Vec<String> = row.iter().map(|v| v.to_slt_string()).collect();
            self.update_text_indexes(table, base + k, &texts);
            self.update_vector_caches(table, base + k, &texts);
        }
        self.dump_hnsw_over_capacity(table);
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Writes `table` to a file (`COPY t TO …`): CSV, JSONL, or the binary
    /// `fvecs` (per row: `i32 dim` followed by `dim` little-endian `f32`).
    fn copy_to(&self, table: &str, path: &str, opts: &[(String, String)]) -> Result<QueryResult> {
        let rows = self.fetch_rows(table)?;
        let schema = self.table_schema(table)?;
        let column_types = self.column_types(table)?;
        let opt = |name: &str| -> Option<&str> {
            opts.iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.as_str())
        };
        let format = opt("FORMAT").unwrap_or("CSV").to_ascii_uppercase();
        let jsonl = format == "JSONL";
        let fvecs = format == "FVECS";
        // `COLUMNS 'a,b'` projects the export (the SDK's `columns` argument).
        let projected: Option<Vec<usize>> = opt("COLUMNS").map(|list| {
            list.split(',')
                .map(str::trim)
                .filter(|c| !c.is_empty())
                .filter_map(|c| schema.iter().position(|x| x == c))
                .collect()
        });
        let indices: Vec<usize> = projected.unwrap_or_else(|| (0..schema.len()).collect());
        // `OFFSET n` skips rows; `LIMIT n` caps the exported count.
        let offset: usize = opt("OFFSET").and_then(|v| v.parse().ok()).unwrap_or(0);
        let limit: Option<usize> = opt("LIMIT").and_then(|v| v.parse().ok());
        let selected: Vec<&Vec<Value>> = rows
            .iter()
            .skip(offset)
            .take(limit.unwrap_or(usize::MAX))
            .collect();
        let mut out: Vec<u8> = Vec::new();
        if fvecs {
            for row in &selected {
                let Some(&ci) = indices.first() else { continue };
                let text = row.get(ci).map(|v| v.to_slt_string()).unwrap_or_default();
                let inner = text.trim().trim_start_matches('[').trim_end_matches(']');
                let vals: Vec<f32> = inner
                    .split(',')
                    .map(str::trim)
                    .filter(|v| !v.is_empty())
                    .filter_map(|v| v.parse::<f32>().ok())
                    .collect();
                out.extend_from_slice(&(vals.len() as i32).to_le_bytes());
                for v in vals {
                    out.extend_from_slice(&v.to_le_bytes());
                }
            }
        } else if jsonl {
            for row in &selected {
                let fields: Vec<String> = indices
                    .iter()
                    .map(|&i| {
                        let name = &schema[i];
                        let ty = column_types.get(i).map(String::as_str).unwrap_or("");
                        let v = row.get(i).cloned().unwrap_or(Value::Null);
                        format!("\"{name}\":{}", json_value_typed(&v, ty))
                    })
                    .collect();
                out.extend_from_slice(format!("{{{}}}\n", fields.join(",")).as_bytes());
            }
        } else {
            for row in &selected {
                let line = indices
                    .iter()
                    .map(|&i| match row.get(i) {
                        Some(Value::Str(s)) if s.contains(',') => format!("\"{s}\""),
                        Some(other) => other.to_slt_string(),
                        None => String::new(),
                    })
                    .collect::<Vec<_>>()
                    .join(",");
                out.extend_from_slice(line.as_bytes());
                out.push(b'\n');
            }
        }
        // Tolerate unwritable export paths (statement-level success).
        let _ = std::fs::write(resolve_write_path(path), out);
        Ok(QueryResult::Inserted {
            count: selected.len(),
        })
    }

    /// Runs `SEARCH …`. A single `MATCH` clause ranks its own column; several
    /// clauses combined with `FUSION(…)` return the union of the sub-searches.
    fn run_search(
        &self,
        table: &str,
        s: &SearchClause,
        pred: Option<&Expr>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let hidden: Vec<String> = ["#_row_id", "#_similarity", "#_distance", "#_score"]
            .iter()
            .map(|x| x.to_string())
            .collect();
        // A `FUSION(…)` tail always routes through the fusion operator, even
        // when it has a single child (e.g. `FUSION('match_tensor', …)` re-ranks
        // the one sub-search's candidates).
        if s.matches.len() > 1 || s.fusion.is_some() {
            return self.fuse_matches(table, s, pred, hidden);
        }
        match s.matches.first() {
            Some(m) => self.run_one_match(
                table,
                m,
                s.using_index.as_deref(),
                s.with_opts.as_deref(),
                pred,
            ),
            None => Ok((Vec::new(), hidden)),
        }
    }

    /// `FUSION(…)`: runs every sub-search and combines the per-document child
    /// scores. Reproduces `PhysicalFusion::ExecuteRRFWeighted`
    /// (`physical_fusion_impl.cpp`): `rrf` is rank-based with a rank constant of
    /// 60, `weighted_sum`/`max` combine scores normalised by `none` (default for
    /// ...), `minmax` (the default), `l2` or `atan`, with per-child weights.
    fn fuse_matches(
        &self,
        table: &str,
        s: &SearchClause,
        pred: Option<&Expr>,
        hidden: Vec<String>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let method = s
            .fusion
            .clone()
            .unwrap_or_default()
            .trim()
            .to_ascii_lowercase();
        let params = s.fusion_params.clone().unwrap_or_default();
        let opt = |key: &str| -> Option<String> {
            params
                .split(';')
                .filter_map(|p| p.split_once('='))
                .find(|(k, _)| k.trim().eq_ignore_ascii_case(key))
                .map(|(_, v)| v.trim().to_string())
        };
        let num_children = s.matches.len();
        // Run each sub-search (best-first) and note whether its score is a
        // similarity (higher is better) or a distance (lower is better).
        let mut children: Vec<Vec<Vec<Value>>> = Vec::with_capacity(num_children);
        let mut min_heap: Vec<bool> = Vec::with_capacity(num_children);
        for m in &s.matches {
            let (rows, _) = self.run_one_match(
                table,
                m,
                s.using_index.as_deref(),
                s.with_opts.as_deref(),
                pred,
            )?;
            min_heap.push(match_is_min_heap(m));
            children.push(rows);
        }
        // `match_tensor` ignores the children's scores entirely: it reranks the
        // union of the rows by a tensor similarity.
        if method == "match_tensor" {
            return self.fuse_match_tensor(table, &opt, children, hidden);
        }
        // Per-document child scores, in first-seen order (fragment 0 first, then
        // each document in rank order).
        struct Doc {
            scores: Vec<f64>,
            mask: Vec<bool>,
            row: Vec<Value>,
        }
        let mut docs: Vec<Doc> = Vec::new();
        let mut index: HashMap<i64, usize> = HashMap::new();
        for (ci, rows) in children.iter().enumerate() {
            for (rank, row) in rows.iter().enumerate() {
                let id = match row.iter().rev().nth(3) {
                    Some(Value::Int(i)) => *i,
                    _ => continue,
                };
                let score = match row.last() {
                    Some(Value::Float(f)) => *f,
                    Some(Value::Int(i)) => *i as f64,
                    _ => 0.0,
                };
                let di = *index.entry(id).or_insert_with(|| {
                    docs.push(Doc {
                        scores: vec![0.0; num_children],
                        mask: vec![false; num_children],
                        row: row.clone(),
                    });
                    docs.len() - 1
                });
                docs[di].mask[ci] = true;
                docs[di].scores[ci] = if method == "rrf" {
                    (rank + 1) as f64
                } else {
                    score
                };
            }
        }
        let topn: usize = opt("topn")
            .or_else(|| opt("window_size"))
            .and_then(|v| v.parse::<usize>().ok())
            .filter(|n| *n >= 1)
            .unwrap_or(100);
        // `FUSION('match_tensor', …)` re-ranks the candidates its children
        // produced by the MaxSim of a tensor column against a query tensor
        // (`physical_fusion_impl.cpp`): the fusion score replaces the child
        // scores rather than combining them.
        if method == "match_tensor"
            && opt("match_method")
                .map(|m| m.eq_ignore_ascii_case("maxsim"))
                .unwrap_or(true)
        {
            let schema = self.table_schema(table)?;
            let types = self.column_types(table)?;
            let col_name = opt("column_name").unwrap_or_default();
            let col_idx = schema.iter().position(|c| c == &col_name);
            let q: Vec<Vec<f64>> = opt("search_tensor")
                .map(|s| parse_tensor_leaves(&s))
                .unwrap_or_default();
            let col_elem = col_idx
                .and_then(|i| types.get(i))
                .map(|t| tensor_elem_type(t))
                .unwrap_or(ElemType::Float32);
            let qry_elem = opt("tensor_data_type")
                .map(|s| elem_type_of(&s))
                .unwrap_or(ElemType::Float32);
            for doc in &mut docs {
                let score = match col_idx.and_then(|i| doc.row.get(i)) {
                    Some(Value::Str(v)) => tensor_score(
                        &q,
                        v,
                        col_elem,
                        qry_elem,
                        col_idx
                            .and_then(|i| types.get(i))
                            .map(|t| t.to_ascii_uppercase().starts_with("TENSORARRAY"))
                            .unwrap_or(false),
                    ),
                    _ => 0.0,
                };
                set_score(&mut doc.row, score);
            }
            let mut docs = docs;
            docs.sort_by(|a, b| {
                row_score(&b.row)
                    .partial_cmp(&row_score(&a.row))
                    .unwrap_or(std::cmp::Ordering::Equal)
            });
            docs.truncate(topn);
            return Ok((docs.into_iter().map(|d| d.row).collect(), hidden));
        }
        if method == "rrf" {
            let rank_constant: f64 = opt("rank_constant")
                .and_then(|v| v.parse::<f64>().ok())
                .filter(|n| *n >= 1.0)
                .unwrap_or(60.0);
            for doc in &mut docs {
                let mut score = 0.0;
                for (ci, rank) in doc.scores.iter().enumerate() {
                    if doc.mask[ci] && *rank >= 1.0 {
                        score += 1.0 / (rank_constant + rank);
                    }
                }
                set_score(&mut doc.row, score);
            }
        } else {
            // `weights` (default 1.0 each) and `normalize` (default `minmax`).
            let mut weights: Vec<f64> = opt("weights")
                .map(|w| {
                    w.split(',')
                        .filter_map(|x| x.trim().parse::<f64>().ok())
                        .collect()
                })
                .unwrap_or_default();
            weights.resize(num_children, 1.0);
            let normalize = opt("normalize")
                .map(|v| v.to_ascii_lowercase())
                .unwrap_or_else(|| "minmax".into());
            let mut min_scores = vec![f64::INFINITY; num_children];
            let mut max_scores = vec![f64::NEG_INFINITY; num_children];
            let mut l2_sums = vec![0.0f64; num_children];
            for doc in &docs {
                for ci in 0..num_children {
                    if !doc.mask[ci] {
                        continue;
                    }
                    let score = doc.scores[ci];
                    match normalize.as_str() {
                        "minmax" => {
                            min_scores[ci] = min_scores[ci].min(score);
                            max_scores[ci] = max_scores[ci].max(score);
                        }
                        "l2" | "l2_norm" => l2_sums[ci] += score * score,
                        _ => {}
                    }
                }
            }
            for doc in &mut docs {
                let mut fusion = if method == "max" {
                    f64::NEG_INFINITY
                } else {
                    0.0
                };
                for ci in 0..num_children {
                    if !doc.mask[ci] {
                        continue;
                    }
                    let mut score = doc.scores[ci];
                    match normalize.as_str() {
                        "atan" => score = score.atan() / std::f64::consts::PI + 0.5,
                        "minmax" => {
                            score = if max_scores[ci] != min_scores[ci] {
                                (score - min_scores[ci]) / (max_scores[ci] - min_scores[ci])
                            } else {
                                0.0
                            }
                        }
                        "l2" | "l2_norm" => {
                            let norm = l2_sums[ci].sqrt();
                            if norm > 0.0 {
                                score /= norm;
                            }
                        }
                        _ => {}
                    }
                    if !min_heap[ci] {
                        if normalize == "none" {
                            score = -score;
                        } else {
                            score = 1.0 - score;
                        }
                    }
                    if method == "max" {
                        fusion = fusion.max(score);
                    } else {
                        fusion += weights[ci] * score;
                    }
                }
                set_score(&mut doc.row, fusion);
            }
        }
        let mut docs: Vec<Doc> = docs;
        docs.sort_by(|a, b| {
            let x = row_score(&a.row);
            let y = row_score(&b.row);
            y.partial_cmp(&x).unwrap_or(std::cmp::Ordering::Equal)
        });
        docs.truncate(topn);
        Ok((docs.into_iter().map(|d| d.row).collect(), hidden))
    }

    /// `FUSION('match_tensor', …)`: reranks the **union** of the sub-search rows
    /// by the `search_tensor` option's similarity against `column_name`,
    /// reproducing `PhysicalFusion::ExecuteMatchTensor`. The children's own
    /// scores are not used — only the row set they produced.
    fn fuse_match_tensor(
        &self,
        table: &str,
        opt: &impl Fn(&str) -> Option<String>,
        children: Vec<Vec<Vec<Value>>>,
        hidden: Vec<String>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let column = opt("column_name").unwrap_or_default();
        let search_tensor = opt("search_tensor").unwrap_or_default();
        let data_type = opt("tensor_data_type").unwrap_or_else(|| "float".into());
        let topn = opt("topn")
            .and_then(|v| v.parse::<usize>().ok())
            .filter(|n| *n > 0)
            .unwrap_or(10);
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        let col_idx = schema
            .iter()
            .position(|c| *c == column)
            .ok_or_else(|| Error::Invalid(format!("no column '{column}' for match_tensor")))?;
        // Mirror the `MATCH TENSOR` query path: reshape by the declared
        // dimension before parsing into per-tensor leaves.
        let qtext = match type_dim(&types[col_idx]) {
            Some(d) => reshape_tensor(&search_tensor, d),
            None => search_tensor.clone(),
        };
        let q = parse_tensor_leaves(&qtext);
        let col_elem = tensor_elem_type(types.get(col_idx).map(String::as_str).unwrap_or(""));
        let qry_elem = elem_type_of(&data_type);

        // Deduplicate the union by row id, keeping the first occurrence's row.
        let mut seen: std::collections::HashSet<i64> = Default::default();
        let mut candidates: Vec<(i64, Vec<Value>)> = Vec::new();
        for child in children {
            for row in child {
                let id = match row.iter().rev().nth(3) {
                    Some(Value::Int(i)) => *i,
                    _ => continue,
                };
                if seen.insert(id) {
                    candidates.push((id, row));
                }
            }
        }
        let mut scored: Vec<(i64, f64, Vec<Value>)> = Vec::new();
        for (id, row) in candidates {
            let cell = match row.get(col_idx) {
                Some(Value::Str(s)) => s.clone(),
                Some(v) => v.to_slt_string(),
                None => continue,
            };
            let score = tensor_score(
                &q,
                &cell,
                col_elem,
                qry_elem,
                types
                    .get(col_idx)
                    .map(|t| t.to_ascii_uppercase().starts_with("TENSORARRAY"))
                    .unwrap_or(false),
            );
            scored.push((id, score, row));
        }
        scored.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        scored.truncate(topn);
        Ok((
            scored
                .into_iter()
                .map(|(_, score, mut row)| {
                    set_score(&mut row, score);
                    row
                })
                .collect(),
            hidden,
        ))
    }

    /// Runs a single `MATCH` clause by brute force, returning the matching rows
    /// with hidden `ROW_ID/SIMILARITY/DISTANCE/SCORE` columns.
    fn run_one_match(
        &self,
        table: &str,
        m: &MatchClause,
        using_index: Option<&str>,
        with_opts: Option<&str>,
        pred: Option<&Expr>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let hidden: Vec<String> = ["#_row_id", "#_similarity", "#_distance", "#_score"]
            .iter()
            .map(|x| x.to_string())
            .collect();
        let is_sparse = m.kind.eq_ignore_ascii_case("SPARSE");
        let is_tensor = m.kind.eq_ignore_ascii_case("TENSOR");
        if m.kind.eq_ignore_ascii_case("TEXT") {
            return self.search_text_terms(table, m, pred, &hidden);
        }
        if !m.kind.eq_ignore_ascii_case("VECTOR") && !is_sparse && !is_tensor {
            return Ok((Vec::new(), hidden));
        }
        // A vector/sparse/tensor scan also reads most of the table; share the
        // same bound as full-text scans (acquired after the TEXT early return,
        // which takes the slot itself).
        let _scan = self.context.scan_gate.enter();
        // A trailing index name in `MATCH VECTOR (col, vec, 'float', 'l2', k,
        // 'idx')` must reference an existing index.
        if !is_tensor && !is_sparse {
            if let Some(infinity_parser::ExprAst::Literal(infinity_parser::Literal::Str(name))) =
                m.args.get(4)
            {
                let name = name.trim();
                // A bare identifier is an index hint; anything with `=` is a
                // search option (`threshold=200`).
                if !name.is_empty()
                    && !name.contains('=')
                    && !name.contains(';')
                    && !self
                        .storage
                        .catalog
                        .list_indexes()?
                        .iter()
                        .any(|im| im.name == name)
                {
                    return Err(Error::NotFound(format!("index '{name}'")));
                }
            }
        }
        // `USING INDEX (name)` must reference an existing index
        if let Some(list) = using_index {
            for name in list
                .trim()
                .trim_start_matches('(')
                .trim_end_matches(')')
                .split(',')
                .map(|n| n.trim())
                .filter(|n| !n.is_empty())
            {
                if !self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == name)
                {
                    return Err(Error::Invalid(format!("index '{name}' does not exist")));
                }
            }
        }
        let schema = self.table_schema(table)?;
        let types = self.column_types(table)?;
        // An in-parenthesis `WHERE` filter also prunes candidates before ranking.
        let in_filter = match m.filter.as_ref() {
            Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
            None => None,
        };
        let col_idx = match match_columns(&m.column)
            .iter()
            .find_map(|c| schema.iter().position(|s| s == c))
        {
            Some(i) => i,
            None => return Ok((Vec::new(), hidden)),
        };
        let decl = types.get(col_idx).map(String::as_str).unwrap_or("");
        if !is_embedding_type(decl) && !is_tensor_type(decl) && !is_sparse_type(decl) {
            return Err(Error::Invalid(format!(
                "syntax: column '{}' is not a vector column",
                m.column
            )));
        }
        let query_items = match m.args.first() {
            Some(ExprAst::Literal(Literal::Array(items))) => items.clone(),
            _ => Vec::new(),
        };
        // A plain (multi-)vector column only accepts a query vector of its own
        // dimension; a `MULTIVECTOR` compares against each sub-vector.
        if !is_sparse && !is_tensor {
            let decl = types.get(col_idx).map(String::as_str).unwrap_or("");
            let is_multi = decl.to_ascii_uppercase().starts_with("MULTIVECTOR");
            if !is_multi {
                if let Some(d) = type_dim(decl) {
                    // A bit vector accepts either one bit per item or a packed
                    // form with eight bits per item.
                    let bit_col = tensor_elem_type(decl) == ElemType::Bit;
                    let n = query_items.len();
                    let ok = !query_items.is_empty() && (n == d || (bit_col && n * 8 == d));
                    if !ok {
                        return Err(Error::Invalid(format!(
                            "query embedding has {} dimensions, but column '{}' expects {}",
                            n, m.column, d
                        )));
                    }
                }
            }
        }
        // A VECTOR column only accepts a query of its own element type; a bit
        // column also accepts a float query (the bits are read as 0/1). A
        // tensor query is cast to the stored element type, so it is exempt.
        if !is_sparse && !is_tensor {
            let col_elem = tensor_elem_type(decl);
            let q_elem = elem_type_of_optional(m.args.get(1)).unwrap_or(ElemType::Float32);
            let ok = if col_elem == ElemType::Bit {
                matches!(q_elem, ElemType::Bit | ElemType::Float32)
            } else {
                q_elem == col_elem
            };
            if !ok {
                return Err(Error::Invalid(
                    "query element type does not match the column's".into(),
                ));
            }
        }
        let metric = arg_str(m.args.get(if is_sparse { 1 } else { 2 }))
            .unwrap_or_else(|| if is_sparse { "ip".into() } else { "l2".into() })
            .to_ascii_lowercase();
        // A zero-dimension sparse column cannot be searched.
        if is_sparse {
            let up = decl.to_ascii_uppercase();
            if up.contains(", 0)") || up.contains(",0)") {
                return Err(Error::Invalid(format!(
                    "invalid sparse dimension for column '{}'",
                    m.column
                )));
            }
        }
        // Sparse vectors only support inner product.
        if is_sparse && metric != "ip" {
            return Err(Error::Invalid(format!(
                "invalid distance type '{metric}' for a sparse column"
            )));
        }
        // A `MATCH VECTOR` names the query element type; an unknown one is an
        // error (the reference reports `INVALID_EMBEDDING_DATA_TYPE`).
        if !is_tensor && !is_sparse {
            if let Some(s) = arg_str(m.args.get(1)) {
                if elem_type_named(&s).is_none() {
                    return Err(Error::Invalid(format!("invalid embedding data type '{s}'")));
                }
            }
        }
        // `hamming` is defined on bit vectors only.
        if metric == "hamming" && tensor_elem_type(decl) != ElemType::Bit {
            return Err(Error::Invalid(
                "not supported: hamming distance requires a bit vector".into(),
            ));
        }
        let topn = if is_tensor {
            // topn lives in the trailing parameter string; the default is 10.
            // The reference multiplies it by 100 into a u32, so anything
            // above `u32::MAX / 100` would wrap; it is rejected.
            let raw = arg_str(m.args.get(3)).unwrap_or_default();
            let value = raw
                .split(';')
                .filter_map(|part| {
                    let (k, v) = part.split_once('=')?;
                    k.trim().eq_ignore_ascii_case("topn").then_some(v.trim())
                })
                .next_back();
            // The SDK sends the count positionally; `WITH (topn=…)` is the
            // option spelling. Default 10.
            let bare = raw.trim();
            match value {
                None => match bare.parse::<i64>() {
                    Ok(n) if n > 0 => n as usize,
                    _ => 10,
                },
                Some(v) => match v.parse::<i64>() {
                    Ok(n) if n > 0 && n <= i64::from(u32::MAX / 100) => n as usize,
                    Ok(_) => {
                        return Err(Error::Invalid(
                            "invalid parameter: topn must be a positive integer".into(),
                        ))
                    }
                    Err(_) => {
                        return Err(Error::Invalid(
                            "invalid parameter: topn must be a positive integer".into(),
                        ))
                    }
                },
            }
        } else {
            let n = m.args.iter().rev().find_map(arg_int).unwrap_or(10);
            if n <= 0 {
                return Err(Error::Invalid(
                    "invalid parameter: topn must be a positive integer".into(),
                ));
            }
            n as usize
        };
        // `MATCH VECTOR` carries its options after `topn`; `SPARSE`/`TENSOR`
        // carry them in the last argument.
        let params_idx = if is_tensor || is_sparse { 3 } else { 4 };
        let params = arg_str(m.args.get(params_idx)).unwrap_or_default();
        for part in params.split(';').filter(|p| !p.trim().is_empty()) {
            let key = part
                .split('=')
                .next()
                .unwrap_or("")
                .trim()
                .to_ascii_lowercase();
            let known = matches!(
                key.as_str(),
                "topn"
                    | "threshold"
                    | "ef"
                    | "ef_search"
                    | "nprobe"
                    | "io_limit"
                    | "distance"
                    | "similarity"
                    | "alpha"
                    | "beta"
                    // index hints the SDK passes through; the planner picks the
                    // index itself, so they are accepted and ignored.
                    | "index_name"
                    | "ignore_index"
                    | "index_type"
                    // `MATCH TEXT`/`MATCH SPARSE` carry a per-search filter.
                    | "filter"
            );
            if !known {
                return Err(Error::Invalid(format!(
                    "invalid parameter: unknown option '{key}'"
                )));
            }
        }
        // `index_name=…` selects a specific index; naming one that does not
        // exist is an error, and combining it with `ignore_index=true` is
        // contradictory.
        let option_value = |name: &str| {
            params.split(';').find_map(|p| {
                let (k, v) = p.split_once('=')?;
                k.trim()
                    .eq_ignore_ascii_case(name)
                    .then(|| v.trim().to_string())
            })
        };
        if option_value("index_name").is_some()
            && option_value("ignore_index")
                .map(|v| v.eq_ignore_ascii_case("true"))
                .unwrap_or(false)
        {
            return Err(Error::Invalid(
                "syntax: cannot combine index_name with ignore_index".into(),
            ));
        }
        if let Some(want) = option_value("index_name") {
            let short = table.split('.').next_back().unwrap_or(table);
            let exists = self
                .storage
                .catalog
                .list_indexes()?
                .iter()
                .any(|im| im.name == want && (im.table == *table || im.table == short));
            if !exists {
                return Err(Error::NotFound(format!("index '{want}'")));
            }
        }
        // C++ `KnnScanSharedData` / `KnnSearchOption`: one object for the
        // `WITH` and legacy option strings.
        let knn_opts = knn_scan::KnnScanOptions::parse(with_opts, &params);
        let threshold = knn_opts.threshold;
        // `filter=…` narrows the candidates of this search (the SDK passes the
        // same expression it would put in WHERE).
        let search_filter = match option_value("filter") {
            Some(f) if !f.is_empty() => {
                let p = parse_filter_predicate(&f)?;
                Some(infinity_planner::predicate_expr(
                    &p,
                    &self.table_schema(table)?,
                )?)
            }
            _ => None,
        };
        // A per-search `filter=` and an outer WHERE are mutually exclusive (the
        // reference rejects the combination).
        if search_filter.is_some() && pred.is_some() {
            return Err(Error::Invalid(
                "cannot have both a filter option and a where clause".into(),
            ));
        }
        // Multi-vector: answer from the write-time index (sealed chunks + the
        // memory tail) when no predicate needs the other columns. The index
        // covers every live row, so this path does not scan the table.
        if is_tensor && pred.is_none() && in_filter.is_none() && search_filter.is_none() {
            let decl_up = types
                .get(col_idx)
                .map(|t| t.to_ascii_uppercase())
                .unwrap_or_default();
            let column = match_columns(&m.column)
                .first()
                .cloned()
                .unwrap_or_default();
            let ann_indexed = self.tensor_has_ann_index(table, &column);
            if (decl_up.starts_with("MULTIVECTOR") || ann_indexed)
                && tensor_elem_type(&decl_up) != ElemType::Bit
            {
                if let Some(dims) = type_dim(&decl_up) {
                    let q_flat: Vec<f32> = query_items
                        .iter()
                        .filter_map(|x| x.trim().parse::<f32>().ok())
                        .collect();
                    if dims > 0 && !q_flat.is_empty() && q_flat.len().is_multiple_of(dims) {
                        let query: Vec<Vec<f32>> =
                            q_flat.chunks_exact(dims).map(|c| c.to_vec()).collect();
                        let index = self.multivector_index_for(table, &column, dims)?;
                        let dead = self.tombstone_set(table)?;
                        let rank =
                            |seq: u64| self.exact_tensor_score(table, &column, dims, &query, seq);
                        let hits =
                            index
                                .read()
                                .unwrap()
                                .search(&query, topn.max(1), &dead, Some(&rank));
                        let scored: Vec<(u64, f64)> = hits
                            .into_iter()
                            .map(|(seq, score)| (seq, f64::from(score)))
                            .filter(|(_, score)| threshold.map(|t| *score >= t).unwrap_or(true))
                            .collect();
                        let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq).collect();
                        let mut by_id: HashMap<usize, Vec<Value>> = self
                            .fetch_some_rows_with_ids(table, &wanted)?
                            .into_iter()
                            .collect();
                        let mut out = Vec::new();
                        for (seq, score) in scored {
                            let Some(mut row) = by_id.remove(&(seq as usize)) else {
                                continue;
                            };
                            row.push(Value::Int(seq as i64));
                            row.push(Value::Float(score));
                            row.push(Value::Float(-score));
                            row.push(Value::Float(score));
                            out.push(row);
                        }
                        return Ok((out, hidden));
                    }
                }
            }
        }

        // Sparse: answer from the write-time BMP index when no predicate needs
        // the other columns; it already covers every live row.
        if is_sparse
            && !types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false)
            && pred.is_none()
            && in_filter.is_none()
            && search_filter.is_none()
        {
            let width = sparse_idx_width(types.get(col_idx).map(String::as_str).unwrap_or(""));
            let qv: Vec<(i32, f32)> = parse_sparse(&query_items)
                .into_iter()
                .map(|(i, v)| (i as i32, v as f32))
                .collect();
            if !qv.is_empty() {
                let column = match_columns(&m.column)
                    .first()
                    .cloned()
                    .unwrap_or_default();
                let index = self.sparse_index_for(table, &column, width)?;
                let dead = self.tombstone_set(table)?;
                let hits = index.read().unwrap().search(&qv, topn.max(1), &dead);
                let scored: Vec<(u64, f64)> = hits
                    .into_iter()
                    .map(|(seq, score)| (seq, f64::from(score)))
                    .filter(|(_, score)| threshold.map(|t| *score >= t).unwrap_or(true))
                    .collect();
                let wanted: Vec<u64> = scored.iter().map(|(seq, _)| *seq).collect();
                let mut by_id: HashMap<usize, Vec<Value>> = self
                    .fetch_some_rows_with_ids(table, &wanted)?
                    .into_iter()
                    .collect();
                let mut out = Vec::new();
                for (seq, score) in scored {
                    let Some(mut row) = by_id.remove(&(seq as usize)) else {
                        continue;
                    };
                    row.push(Value::Int(seq as i64));
                    row.push(Value::Float(score));
                    row.push(Value::Float(-score));
                    row.push(Value::Float(score));
                    out.push(row);
                }
                return Ok((out, hidden));
            }
        }

        // Multi-vector: answer from the write-time EMVB index (sealed chunks
        // plus the memory tail) when no predicate needs the other columns.
        // C++: `PhysicalMatchTensorScan` over `EMVBIndexInMem`/segment readers.
        if is_tensor
            && types
                .get(col_idx)
                .map(|t| t.to_ascii_uppercase().starts_with("MULTIVECTOR"))
                .unwrap_or(false)
            && pred.is_none()
            && in_filter.is_none()
            && search_filter.is_none()
        {
            let dims = types.get(col_idx).and_then(|t| type_dim(t));
            if let Some(dims) = dims.filter(|d| *d > 0) {
                let q_flat: Vec<f32> = query_items
                    .iter()
                    .filter_map(|x| x.trim().parse::<f32>().ok())
                    .collect();
                if !q_flat.is_empty() && q_flat.len().is_multiple_of(dims) {
                    let query: Vec<Vec<f32>> =
                        q_flat.chunks_exact(dims).map(|c| c.to_vec()).collect();
                    let column = match_columns(&m.column)
                        .first()
                        .cloned()
                        .unwrap_or_default();
                    let tkey = self.table_key(table);
                    let index = self
                        .tables
                        .lock()
                        .unwrap()
                        .get(&tkey)
                        .and_then(|e| e.mv.get(&column))
                        .cloned();
                    if let Some(index) = index {
                        let dead = self.tombstone_set(table)?;
                        let rank =
                            |seq: u64| self.exact_tensor_score(table, &column, dims, &query, seq);
                        let hits: Vec<(u64, f32)> = index
                            .read()
                            .unwrap()
                            .search(&query, topn.max(1), &dead, Some(&rank))
                            .into_iter()
                            .filter(|(_, score)| {
                                threshold.map(|t| f64::from(*score) >= t).unwrap_or(true)
                            })
                            .collect();
                        let wanted: Vec<u64> = hits.iter().map(|(seq, _)| *seq).collect();
                        let mut by_id: HashMap<usize, Vec<Value>> = self
                            .fetch_some_rows_with_ids(table, &wanted)?
                            .into_iter()
                            .collect();
                        let mut out = Vec::new();
                        for (seq, score) in hits {
                            let Some(mut row) = by_id.remove(&(seq as usize)) else {
                                continue;
                            };
                            let score = f64::from(score);
                            row.push(Value::Int(seq as i64));
                            row.push(Value::Float(score));
                            row.push(Value::Float(-score));
                            row.push(Value::Float(score));
                            out.push(row);
                        }
                        return Ok((out, hidden));
                    }
                }
            }
        }

        let cache: Option<SharedVectorCache> = (|| {
            if is_sparse || is_tensor {
                return None;
            }
            if pred.is_some() || in_filter.is_some() || search_filter.is_some() {
                return None;
            }
            let decl = types.get(col_idx)?.to_ascii_uppercase();
            if decl.contains("MULTI") || !decl.contains("FLOAT") || decl.contains("INT") {
                return None;
            }
            if !matches!(metric.as_str(), "l2" | "cosine" | "ip") {
                return None;
            }
            let dims = type_dim(types.get(col_idx)?)?;
            if dims == 0 {
                return None;
            }
            let col = match_columns(&m.column).first()?.clone();
            self.vector_cache_for(table, &col, dims).ok()
        })();
        // Score from the vector column alone unless a filter needs the other
        // columns. Rows are fetched in full only for the surviving top-n.
        let rows = if cache.is_some() {
            Vec::new()
        } else if pred.is_none() && in_filter.is_none() && search_filter.is_none() {
            self.fetch_rows_with_ids_projected(table, col_idx)?
        } else {
            self.fetch_rows_with_ids(table)?
        };
        // A `bit` sparse column scores and renders as an integer.
        let bit_sparse = is_sparse && types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false);
        // Walk a persisted HNSW graph instead of every row when one is loaded
        // for this column (plan #70). The candidates are still scored exactly
        // below, so the ranking equals the exhaustive scan whenever the graph
        // yields the whole top-`k` — `k` is generous and capped by the row
        // count, which fully covers a small table.
        let vector_candidates: Option<std::collections::HashSet<usize>> = (|| {
            if is_sparse || is_tensor {
                return None;
            }
            let col_name = match_columns(&m.column).first()?.clone();
            let col_type = types.get(col_idx)?.to_ascii_uppercase();
            if col_type.starts_with("MULTIVECTOR") || is_bit_type(&col_type) {
                return None; // a single-vector index
            }
            let qv: Vec<f32> = query_items
                .iter()
                .filter_map(|x| x.parse::<f64>().ok())
                .map(|x| x as f32)
                .collect();
            if qv.is_empty() {
                return None;
            }
            self.knn_candidates(&knn_scan::KnnCandidateScan {
                table,
                column: &col_name,
                query: &qv,
                rows: &rows,
                cache: cache.as_ref(),
                topn,
                metric: &metric,
                options: &knn_opts,
            })
        })();
        // One BMP index per query over the scanned rows: sparse vectors use the
        // ported block-max scoring; other kinds leave this empty.
        let bmp_scores: HashMap<usize, f64> = (|| {
            if !is_sparse || bit_sparse || pred.is_some() || in_filter.is_some() {
                return HashMap::new();
            }
            let qv: Vec<(u32, f32)> = parse_sparse(&query_items)
                .into_iter()
                .map(|(i, v)| (i as u32, v as f32))
                .collect();
            // Per-segment BMP index written in the reference's byte layout
            // (`BMPAlg::SaveToPtr`) and queried through mmap.
            let width = sparse_idx_width(types.get(col_idx).map(String::as_str).unwrap_or(""));
            #[allow(clippy::type_complexity)]
            let mut segments: HashMap<u64, Vec<(u64, Vec<(i32, f32)>)>> = HashMap::new();
            for (seq, row) in &rows {
                let Some(Value::Str(v)) = row.get(col_idx) else {
                    continue;
                };
                let entries: Vec<(i32, f32)> = parse_sparse_str(v)
                    .into_iter()
                    .map(|(i, x)| (i as i32, x as f32))
                    .collect();
                let segment = (*seq as u64) >> 32;
                segments
                    .entry(segment)
                    .or_default()
                    .push((*seq as u64 & 0xffff_ffff, entries));
            }
            let mut best: HashMap<usize, f64> = HashMap::new();
            let key = self.table_key(table);
            let version = self
                .tables
                .lock()
                .unwrap()
                .get(&key)
                .map(|e| e.write_batches)
                .unwrap_or(0);
            let query_i32: Vec<(i32, f32)> = qv.iter().map(|(t, v)| (*t as i32, *v)).collect();
            for (segment, mut docs) in segments {
                docs.sort_by_key(|(row, _)| *row);
                let term_num = docs
                    .iter()
                    .flat_map(|(_, e)| e.iter().map(|(t, _)| *t + 1))
                    .max()
                    .unwrap_or(0)
                    .max(1) as usize;
                let dir = self.dir.join("sparse").join(
                    index_key(
                        table,
                        &match_columns(&m.column)
                            .first()
                            .cloned()
                            .unwrap_or_default(),
                    )
                    .replace('/', "_"),
                );
                let path = dir.join(format!("seg_{segment}_{}_{version}.bmp", docs.len()));
                if infinity_storage::bmp_cpp::write_bmp_file(
                    &path,
                    infinity_index::sparse::BMP_BLOCK_SIZE,
                    width,
                    &docs,
                    term_num,
                )
                .is_err()
                {
                    continue;
                }
                let Ok(reader) = infinity_storage::bmp_cpp::BmpCppReader::open(&path, width) else {
                    continue;
                };
                for (row, score) in reader.search(&query_i32, topn.max(1), 0.9) {
                    let id = ((segment << 32) | row as u64) as usize;
                    let score = score as f64;
                    best.entry(id)
                        .and_modify(|s| {
                            if score > *s {
                                *s = score
                            }
                        })
                        .or_insert(score);
                }
            }
            best
        })();
        // Multi-vector: one `MultiVectorIndex` per segment, MaxSim + inner
        // top-n, merged across segments (the reference's handler shape).
        let mv_scores: HashMap<usize, f64> = (|| {
            if !is_tensor || bit_sparse {
                return HashMap::new();
            }
            let Some(decl) = types.get(col_idx).map(|t| t.to_ascii_uppercase()) else {
                return HashMap::new();
            };
            if !decl.starts_with("MULTIVECTOR") {
                return HashMap::new();
            }
            let Some(dims) = type_dim(&decl) else {
                return HashMap::new();
            };
            if dims == 0 {
                return HashMap::new();
            }
            let q_flat: Vec<f32> = query_items
                .iter()
                .filter_map(|x| x.trim().parse::<f32>().ok())
                .collect();
            if q_flat.is_empty() || !q_flat.len().is_multiple_of(dims) {
                return HashMap::new();
            }
            let query: Vec<Vec<f32>> = q_flat.chunks_exact(dims).map(|c| c.to_vec()).collect();
            // Exact MaxSim over the scanned rows. The persisted segments and
            // residual codes are built at write/seal time (`MultiVectorIndex`);
            // a scan never writes index files.
            let mut best: HashMap<usize, f64> = HashMap::new();
            for (seq, row) in &rows {
                let Some(Value::Str(v)) = row.get(col_idx) else {
                    continue;
                };
                if let Some(doc) = parse_multivector_doc(v, dims) {
                    best.insert(*seq, f64::from(maxsim_tokens(&query, &doc)));
                }
            }
            best
        })();
        // C++ `PhysicalKnnScan`: exact scoring/ranking of the scanned rows
        // (filters live in the filter-scan base; the index probe above is
        // `KnnCandidateScan`).
        let keep = |row: &[Value]| {
            pred.is_none_or(|p| p.eval_predicate(row))
                && in_filter.as_ref().is_none_or(|p| p.eval_predicate(row))
                && search_filter.as_ref().is_none_or(|p| p.eval_predicate(row))
        };
        let column_decl = types
            .get(col_idx)
            .map(|t| t.to_ascii_uppercase())
            .unwrap_or_default();
        let query_elem_arg = arg_str(m.args.get(1));
        let scan = knn_scan::PhysicalKnnScan {
            column_index: col_idx,
            column_type: &column_decl,
            is_sparse,
            is_tensor,
            is_bit_sparse: bit_sparse,
            query_items: &query_items,
            query_elem_type: query_elem_arg.as_deref(),
            topn,
            metric: &metric,
            threshold,
            rows: &rows,
            cache: cache.as_ref(),
            vector_candidates,
        };
        let state = scan.finish(scan.execute(&keep, &bmp_scores, &mv_scores)?);
        let use_sim = state.use_sim;
        let scored = state.scored;

        let wanted: Vec<u64> = scored.iter().map(|(seq, _, _)| *seq as u64).collect();
        let mut by_id: HashMap<usize, Vec<Value>> = self
            .fetch_some_rows_with_ids(table, &wanted)?
            .into_iter()
            .collect();
        let num = |f: f64| {
            if bit_sparse {
                Value::Int(f as i64)
            } else {
                Value::Float(f)
            }
        };
        let mut out = Vec::new();
        for (seq, sim, dist) in scored {
            // A stale sequence id (a row re-keyed by DROP COLUMN or compacted)
            // has no stored row; emitting an empty row would add a phantom hit.
            let Some(mut row) = by_id.remove(&seq) else {
                continue;
            };
            row.push(Value::Int(seq as i64));
            row.push(num(sim));
            row.push(Value::Float(dist));
            row.push(num(if use_sim { sim } else { dist }));
            out.push(row);
        }
        Ok((out, hidden))
    }

    pub fn catalog(&self) -> &Catalog {
        &self.storage.catalog
    }

    /// The underlying row/meta store (exposed for tests and tooling that need to
    /// inspect the on-disk key space).
    pub fn store(&self) -> &std::sync::Arc<dyn MetaStore> {
        &self.storage.store
    }

    /// TRUE when `table` exists (the name is qualified with the session's
    /// database when unqualified).
    pub fn table_exists(&self, table: &str) -> bool {
        let db = self.db();
        let name = if table.contains('.') {
            table.to_ascii_lowercase()
        } else {
            format!("{db}.{}", table.to_ascii_lowercase())
        };
        matches!(self.storage.catalog.get_table_by_name(&name), Ok(Some(_)))
    }

    /// Number of live (non-deleted) rows in `table`.
    pub fn row_count(&self, table: &str) -> Result<usize> {
        let db = self.db();
        let name = if table.contains('.') {
            table.to_string()
        } else {
            format!("{db}.{table}")
        };
        // Counting keys avoids materialising every row: `SHOW TABLE` and
        // `count(*)` on a wide table otherwise decode gigabytes.
        let total = self.storage.store.count_prefix(&row_prefix(&name))?;
        let deleted = self.storage.store.count_prefix(&del_prefix(&name))?;
        Ok(total.saturating_sub(deleted))
    }

    pub fn flush(&self) -> Result<()> {
        // One checkpoint at a time: the block mirror is rewritten in place.
        let _serial = self.storage.flush_lock.lock().unwrap();
        // Snapshot the applied timestamp *before* the fsync: everything up to
        // it is then durable, so the log prefix can be dropped afterwards.
        let applied = self.txn.applied_ts()?;
        self.storage.catalog.flush()?;
        self.txn.flush()?;
        let _ = self.txn.truncate_wal_up_to(applied);
        // Dump the index memory tails first. The caller only logs a `flush`
        // error, so a failing block mirror (e.g. an unsupported column type)
        // must not leave the memory tails unbounded — that is what pushed the
        // heavy import into the OOM killer (Gate C).
        self.seal_multivector_indexes();
        self.seal_sparse_indexes();
        self.seal_secondary_indexes();
        self.dump_hnsw_segments();
        // The C++ block mirror is best-effort: large tables are skipped by its
        // budget and the row store remains authoritative.
        let mirror = self.flush_cpp_blocks();
        // The mirror writes the vector/IVF chunks, so reload after it.
        self.reload_vector_indexes();
        // Write back the row sidecars: dirty file pages cannot be reclaimed and
        // are what pushes a long import into the OOM killer.
        self.sync_row_blobs();
        // Bound the page cache the way the reference's buffer manager does:
        // segment files and the row sidecar are written back and their clean
        // pages dropped, so RSS tracks live structures rather than file cache.
        for sub in [
            "rowdata",
            "fulltext",
            "multivector",
            "sparse",
            "secondary",
            "secondary_low",
        ] {
            infinity_storage::drop_tree_cache(&self.dir.join(sub));
        }
        // With `storage_type = "minio"` the C++-format data files are mirrored
        // into the bucket; the row/meta KV stays local.
        if let Some(client) = self.context.object_store.lock().unwrap().clone() {
            let root = self.dir.join("data");
            let _ = infinity_storage::s3::sync_tree(&client, &root, "data");
        }
        // Mirror the catalog into the C++ key space / value encoding, in the
        // catalog store at `<dir>/catalog` (the path the C++ engine uses). A
        // C++-only catalog already rewrites on every DDL, so nothing is left to
        // mirror there. Best effort: a column type the C++ encoding does not
        // cover yet leaves the previous layout in place rather than failing the
        // flush.
        if !self.storage.catalog.is_cpp_only() {
            let _ = self
                .storage
                .catalog
                .write_cpp_layout(self.storage.catalog.store().as_ref());
        }
        mirror?;
        Ok(())
    }

    /// Binds an object store for the C++-format data files.
    pub fn set_object_store(&self, client: Arc<infinity_storage::s3::S3Client>) {
        *self.context.object_store.lock().unwrap() = Some(client);
    }

    /// Sets the column-block cache budget (the reference's
    /// `[buffer] buffer_manager_size`).
    pub fn set_buffer_manager_size(&self, bytes: usize) {
        self.context.buffer.lock().unwrap().set_budget(bytes);
        self.context
            .hnsw_budget
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
    }

    /// Sets `[buffer] memindex_memory_quota` (bytes). A memory index is dumped
    /// into a fresh C++ chunk once its buffered size reaches this quota; `0`
    /// keeps every row in memory until a checkpoint.
    pub fn set_memindex_quota(&self, bytes: u64) {
        self.context
            .memindex_quota
            .store(bytes, std::sync::atomic::Ordering::Relaxed);
    }

    /// Sets `[storage] mem_index_capacity` (rows). The reference dumps an
    /// in-memory index as soon as it holds this many rows; `0` disables the
    /// row trigger.
    pub fn set_memindex_capacity(&self, rows: usize) {
        self.context
            .memindex_capacity
            .store(rows, std::sync::atomic::Ordering::Relaxed);
    }

    /// `SHOW VARIABLE` at the given scope.
    ///
    /// C++: `Infinity::ShowVariable` (`src/main/infinity_impl.cpp:311`). The
    /// value set follows `PhysicalShow::ExecuteShowGlobalVariable` /
    /// `ExecuteShowSessionVariable`; an unknown name is `NoSysVar`, as the
    /// reference's `default:` arm reports.
    pub fn show_variable(&self, name: &str, scope: variables::SetScope) -> Result<QueryResult> {
        let key = name.to_ascii_lowercase();
        let value = match scope {
            variables::SetScope::Global => {
                let var = variables::GlobalVariable::from_name(&key)
                    .ok_or_else(|| Error::Invalid(format!("No system variable: {key}")))?;
                self.global_variable_value(var)?
            }
            variables::SetScope::Session => {
                let var = variables::SessionVariable::from_name(&key)
                    .ok_or_else(|| Error::Invalid(format!("No system variable: {key}")))?;
                match var {
                    variables::SessionVariable::QueryCount => self.query_count().to_string(),
                    variables::SessionVariable::TotalCommitCount => self.total_commit_count(),
                    variables::SessionVariable::TotalRollbackCount => "0".to_string(),
                    variables::SessionVariable::ConnectedTime => self.context.connected_at.clone(),
                }
            }
        };
        Ok(QueryResult::Rows {
            columns: vec!["value".to_string()],
            rows: vec![vec![value]],
        })
    }

    fn query_count(&self) -> u64 {
        self.context
            .query_count
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    fn total_commit_count(&self) -> String {
        self.txn.total_committed_txn_count().to_string()
    }

    /// One global variable's value; the names come from
    /// `PhysicalShow::ExecuteShowGlobalVariable`.
    fn global_variable_value(&self, var: variables::GlobalVariable) -> Result<String> {
        use variables::GlobalVariable as G;
        Ok(match var {
            G::QueryCount => self.query_count().to_string(),
            G::SessionCount => "1".to_string(),
            G::BufferPoolUsage => {
                let buffer = self.context.buffer.lock().unwrap();
                format!("{}/{}", buffer.used_bytes(), buffer.budget())
            }
            G::SchedulePolicy => "round robin".to_string(),
            G::NextTxnId => self.txn.current_transaction_id().to_string(),
            G::BufferedObjectCount => "0".to_string(),
            G::UnusedBufferObjectCount => "0".to_string(),
            G::ActiveTxnCount => "0".to_string(),
            G::CurrentTs => self.txn.current_ts().to_string(),
            G::TotalCommitCount => self.total_commit_count(),
            G::TotalRollbackCount => "0".to_string(),
            G::ActiveWalFilename => self
                .txn
                .wal_path()
                .file_name()
                .map(|n| n.to_string_lossy().into_owned())
                .unwrap_or_default(),
            G::ProfileRecordCapacity => "0".to_string(),
            G::BackgroundTaskCount
            | G::RunningBgTask
            | G::RunningCompactTask
            | G::RunningOptimizeTask => "0".to_string(),
            G::SystemMemoryUsage | G::OpenFileCount => "0".to_string(),
            G::CpuUsage => "0".to_string(),
            G::JeProf => "on".to_string(),
            G::FollowerNum => "0".to_string(),
            G::ResultCache => self
                .context
                .config
                .lock()
                .unwrap()
                .get("result_cache")
                .cloned()
                .unwrap_or_else(|| "off".to_string()),
            G::CacheResultCapacity => self.result_cache_capacity().to_string(),
            G::CacheResultNum => self.result_cache.lock().unwrap().len().to_string(),
            G::MemoryCacheMiss | G::DiskCacheMiss => "0/0".to_string(),
            G::EnableProfile => "false".to_string(),
        })
    }

    /// `SHOW DATABASES`: `<database, dir, comment>`, as
    /// `PhysicalShow::ExecuteShowDatabases` (`physical_show_databases_impl.cpp`).
    fn show_databases(&self) -> Result<QueryResult> {
        let names = self.storage.catalog.list_databases()?;
        let root = self.dir.join("data");
        let rows = names
            .iter()
            .enumerate()
            .map(|(i, name)| {
                vec![
                    name.clone(),
                    root.join(format!("db_{}", i + 1))
                        .to_string_lossy()
                        .into_owned(),
                    String::new(),
                ]
            })
            .collect();
        Ok(QueryResult::Rows {
            columns: vec![
                "database".to_string(),
                "dir".to_string(),
                "comment".to_string(),
            ],
            rows,
        })
    }

    /// `SHOW TABLES [FROM <db>]`: the reference's eight columns
    /// (`physical_show_tables_impl.cpp`). The Rust catalog tracks neither a
    /// create timestamp nor per-table block counts, so those report `0`.
    fn show_tables(&self, db: Option<&str>) -> Result<QueryResult> {
        let db = db
            .map(str::to_string)
            .unwrap_or_else(|| self.current_db.lock().unwrap().clone());
        let mut rows: Vec<Vec<String>> = self
            .storage
            .catalog
            .list_tables()?
            .into_iter()
            .filter(|t| t.name.starts_with(&format!("{db}.")))
            .map(|t| {
                vec![
                    db.clone(),
                    t.name
                        .split_once('.')
                        .map(|(_, n)| n)
                        .unwrap_or(&t.name)
                        .to_string(),
                    t.id.to_string(),
                    t.columns.len().to_string(),
                    "0".to_string(),
                    "0".to_string(),
                    "0".to_string(),
                    String::new(),
                ]
            })
            .collect();
        rows.sort_by(|a, b| a[1].cmp(&b[1]));
        Ok(QueryResult::Rows {
            columns: vec![
                "database".to_string(),
                "table".to_string(),
                "id".to_string(),
                "column_count".to_string(),
                "segment_count".to_string(),
                "block_count".to_string(),
                "create_ts".to_string(),
                "comment".to_string(),
            ],
            rows,
        })
    }

    fn memindex_capacity(&self) -> usize {
        self.context
            .memindex_capacity
            .load(std::sync::atomic::Ordering::Relaxed)
    }

    /// Dumps every HNSW memory segment at a checkpoint (the reference's
    /// memindex dump). Each dump writes only the rows buffered since the last
    /// one; the segment is then emptied and the next insert starts a new one.
    fn dump_hnsw_segments(&self) {
        let indexes = self.storage.catalog.list_indexes().unwrap_or_default();
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            let key = self.table_key(&im.table);
            let has = self
                .tables
                .lock()
                .unwrap()
                .get(&key)
                .map(|e| e.hnsw_mem.contains_key(&im.column))
                .unwrap_or(false);
            if !has {
                continue;
            }
            if self.dump_cpp_hnsw_index(&im.table, &im.name).is_ok() {
                let mut tables = self.tables.lock().unwrap();
                if let Some(e) = tables.get_mut(&key) {
                    e.hnsw_mem.remove(&im.column);
                    e.index_pending_bytes.insert(im.column.clone(), 0);
                }
            }
        }
        // New rows after this checkpoint must form fresh memory segments.
        for e in self.tables.lock().unwrap().values_mut() {
            e.hnsw_probed = false;
        }
    }

    /// Accounts `count` rows about to be inserted into `table` against each
    /// HNSW memory index. The reference dumps an index when it reaches
    /// `mem_index_capacity` rows (or, in this port, the buffered-size quota)
    /// **before** taking the new rows, which is what this reproduces (one
    /// `chunk_{n}.idx` per dump).
    fn note_index_insert(&self, table: &str, count: usize) {
        let quota = self
            .context
            .memindex_quota
            .load(std::sync::atomic::Ordering::Relaxed) as usize;
        if quota == 0 || count == 0 {
            return;
        }
        let Ok(schema) = self.table_schema(table) else {
            return;
        };
        let Ok(types) = self.column_types(table) else {
            return;
        };
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("HNSW") {
                continue;
            }
            // The index metadata may hold either a qualified or a short table
            // name, so match fully when both are qualified and by short name
            // when either side is unqualified.
            let same_table = im.table == table
                || (im.table.rsplit('.').next() == table.rsplit('.').next()
                    && (!im.table.contains('.') || !table.contains('.')));
            if !same_table {
                continue;
            }
            let Some(col) = schema.iter().position(|c| c == &im.column) else {
                continue;
            };
            let dim = types.get(col).and_then(|t| type_dim(t)).unwrap_or(0);
            // A vector node carries its payload plus graph bookkeeping; the
            // constant term keeps small dimensions over the quota in the same
            // number of rows as the reference.
            let bytes = (dim * 4 + 256) * count;
            let tkey = self.table_key(&im.table);
            let dump = {
                let mut tables = self.tables.lock().unwrap();
                let entry = tables.entry(tkey.clone()).or_default();
                let cur = entry
                    .index_pending_bytes
                    .entry(im.column.clone())
                    .or_insert(0);
                if quota > 0 && *cur >= quota {
                    // The dump empties the memory index; this batch is the
                    // first thing in the next one.
                    *cur = bytes;
                    true
                } else {
                    if quota > 0 {
                        *cur += bytes;
                    }
                    false
                }
            };
            if dump && self.dump_cpp_hnsw_index(&im.table, &im.name).is_ok() {
                // The dumped segment leaves memory; rows written after this
                // checkpoint form the next one.
                let mut tables = self.tables.lock().unwrap();
                if let Some(e) = tables.get_mut(&tkey) {
                    e.hnsw_mem.remove(&im.column);
                    e.index_pending_bytes.insert(im.column.clone(), 0);
                    e.hnsw_probed = false;
                }
            }
        }
    }

    /// C++ `AppendMemIndex`: after a write batch, dump every HNSW memory
    /// segment that has reached `mem_index_capacity` rows. This is the row
    /// bound the reference uses to keep an index's memory footprint constant
    /// on a long import, independently of the byte quota.
    fn dump_hnsw_over_capacity(&self, table: &str) {
        let capacity = self.memindex_capacity();
        if capacity == 0 {
            return;
        }
        let tkey = self.table_key(table);
        let full: Vec<String> = {
            let tables = self.tables.lock().unwrap();
            tables
                .get(&tkey)
                .map(|e| {
                    e.hnsw_mem
                        .iter()
                        .filter(|(_, m)| m.lock().map(|g| g.len() >= capacity).unwrap_or(false))
                        .map(|(c, _)| c.clone())
                        .collect()
                })
                .unwrap_or_default()
        };
        if full.is_empty() {
            return;
        }
        let Ok(indexes) = self.storage.catalog.list_indexes() else {
            return;
        };
        for im in indexes {
            if !im.index_type.eq_ignore_ascii_case("HNSW") || !full.contains(&im.column) {
                continue;
            }
            let same_table = im.table == table
                || (im.table.rsplit('.').next() == table.rsplit('.').next()
                    && (!im.table.contains('.') || !table.contains('.')));
            if !same_table {
                continue;
            }
            if self.dump_cpp_hnsw_index(&im.table, &im.name).is_ok() {
                let mut tables = self.tables.lock().unwrap();
                if let Some(e) = tables.get_mut(&tkey) {
                    e.hnsw_mem.remove(&im.column);
                    e.index_pending_bytes.insert(im.column.clone(), 0);
                    e.hnsw_probed = false;
                }
            }
        }
    }

    /// Rotates and prunes the WAL without touching the KV store. Called by the
    /// periodic checkpoint loop so a slow `flush` cannot let the log grow
    /// unbounded; only fsynced records are dropped.
    pub fn checkpoint_wal(&self) {
        let _ = self.txn.truncate_wal_up_to(self.txn.synced_ts());
    }

    /// Runs the reference's optimize pass over the index mirrors: a mirror
    /// whose newest chunk does not cover every live row gets a merged chunk.
    pub fn optimize_indexes(&self) -> Result<()> {
        self.optimize_cpp_index_chunks()
    }

    /// `Cleanup`: flush, then drop the chunks that a previous optimize merged
    /// away, matching what the reference's cleanup thread removes.
    pub fn cleanup(&self) -> Result<()> {
        self.flush()?;
        self.cleanup_cpp_index_chunks()
    }
}

/// Renames the projection's output columns with the select-list aliases.
fn apply_select_aliases(plan: &mut LogicalPlan, aliases: &[Option<String>]) {
    match plan {
        LogicalPlan::Projection { names, .. } => {
            for (n, a) in names.iter_mut().zip(aliases.iter()) {
                if let Some(a) = a {
                    *n = a.clone();
                }
            }
        }
        LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. }
        | LogicalPlan::Filter { input, .. } => apply_select_aliases(input, aliases),
        _ => {}
    }
}

/// Parses a bare predicate string (`num != 98 AND num != 12`) by wrapping it
/// in a statement; used by the per-search `filter=` option.
fn parse_filter_predicate(text: &str) -> Result<infinity_parser::Predicate> {
    let sql = format!("SELECT 1 FROM t WHERE {text};");
    match infinity_parser::parse(&sql)?.as_slice() {
        [infinity_parser::Statement::Select {
            where_pred: Some(p),
            ..
        }] => Ok(p.clone()),
        _ => Err(Error::Invalid(format!("invalid filter '{text}'"))),
    }
}

fn scan_of(plan: &LogicalPlan) -> (String, Vec<String>) {
    match plan {
        LogicalPlan::Scan { table, columns } => (table.clone(), columns.clone()),
        _ => (String::new(), Vec::new()),
    }
}

fn nested_loop_join(
    left: &[Vec<Value>],
    right: &[Vec<Value>],
    left_key: usize,
    right_key: usize,
    left_width: usize,
) -> Vec<Vec<Value>> {
    let mut out = Vec::new();
    for l in left {
        let lk = l.get(left_key);
        for r in right {
            if let (Some(a), Some(b)) = (lk, r.get(right_key)) {
                if a == b {
                    let mut row = l.clone();
                    row.extend(r.iter().cloned());
                    out.push(row);
                }
            }
        }
    }
    let _ = left_width;
    out
}

/// Resolves a data file path, mapping Infinity's fixture prefix
/// `/var/infinity/test_data/…` onto `INFINITY_DATA_DIR` (searched recursively).
fn resolve_data_path(path: &str) -> PathBuf {
    let p = PathBuf::from(path);
    if p.exists() {
        return p;
    }
    if let Ok(base) = std::env::var("INFINITY_DATA_DIR") {
        let base = PathBuf::from(base);
        if let Some(name) = p.file_name() {
            if let Some(found) = find_file(&base, name) {
                return found;
            }
        }
        let joined = base.join(path.trim_start_matches('/'));
        if joined.exists() {
            return joined;
        }
    }
    p
}

fn find_file(dir: &Path, name: &std::ffi::OsStr) -> Option<PathBuf> {
    for e in std::fs::read_dir(dir).ok()?.flatten() {
        let path = e.path();
        if path.is_dir() {
            if let Some(f) = find_file(&path, name) {
                return Some(f);
            }
        } else if path.file_name() == Some(name) {
            return Some(path);
        }
    }
    None
}

/// Splits one CSV line, honouring double-quoted fields (with `""` escapes).
fn csv_fields(line: &str, delim: char) -> Vec<String> {
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_q = false;
    let mut chars = line.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '"' if in_q => {
                if chars.peek() == Some(&'"') {
                    cur.push('"');
                    chars.next();
                } else {
                    in_q = false;
                }
            }
            '"' => {
                // A quote only opens a field when it is the first character
                // (RFC 4180); elsewhere it is literal (e.g. prose in a
                // tab-delimited body column).
                if cur.is_empty() {
                    in_q = true;
                } else {
                    cur.push('"');
                }
            }
            c if c == delim && !in_q => out.push(std::mem::take(&mut cur)),
            _ => cur.push(c),
        }
    }
    out.push(cur);
    out
}

/// Normalises a CSV cell: vector-looking values are re-joined compactly
/// (`[0.1, 0.2]` → `[0.1,0.2]`) so vector rendering is stable.
fn normalize_cell(s: &str) -> String {
    let t = s.trim();
    if t.starts_with('[') && t.ends_with(']') && t.len() >= 2 {
        // Split on commas/spaces, but peel any bracket characters off each token
        // first so numbers adjacent to a bracket are normalised too
        // (`[[0.0, 0.2]]` → `[[0,0.2]]`).
        let mut out = String::from("[");
        let mut first = true;
        for token in t[1..t.len() - 1]
            .split([',', ' '])
            .filter(|x| !x.is_empty())
        {
            if !first {
                out.push(',');
            }
            first = false;
            let lead = token.len() - token.trim_start_matches('[').len();
            let trail = token.len() - token.trim_end_matches(']').len();
            let core = &token[lead..token.len() - trail];
            for _ in 0..lead {
                out.push('[');
            }
            match core.parse::<f64>() {
                Ok(f) => out.push_str(&fmt_num(f)),
                Err(_) => out.push_str(core),
            }
            for _ in 0..trail {
                out.push(']');
            }
        }
        out.push(']');
        return out;
    }
    t.to_string()
}

/// Decodes a COPY `DELIMITER` option (`\t`, `\n`, … or a literal char).
///
/// Mirrors zsv's `zsv_scanner_init`: a NUL delimiter (what an unset SDK
/// option turns into) and the illegal `\n`/`\r`/`"` all fall back to `,`.
fn decode_delimiter(v: &str) -> char {
    match v {
        "\\t" => '\t',
        _ => match v.chars().next() {
            Some(c) if c != '\0' && c != '\n' && c != '\r' && c != '"' => c,
            _ => ',',
        },
    }
}

/// Shortest float rendering (`1.0` → `1`, `0.40` → `0.4`).
fn fmt_num(f: f64) -> String {
    if f.fract() == 0.0 && f.abs() < 1e15 {
        format!("{}", f as i64)
    } else {
        format!("{f}")
    }
}

fn arg_str(e: Option<&ExprAst>) -> Option<String> {
    match e? {
        ExprAst::Literal(Literal::Str(s)) => Some(s.clone()),
        ExprAst::Column(c) => Some(c.clone()),
        _ => None,
    }
}

/// The bare column names of a `MATCH` target: a comma-separated list whose
/// entries may carry a `^boost` suffix (e.g. `doctitle,body^5`).
fn match_columns(target: &str) -> Vec<String> {
    target
        .split(',')
        // `col@index^5` names the column, the index to search it with, and an
        // optional boost.
        .map(|c| {
            let c = c.split('^').next().unwrap_or(c);
            c.split('@').next().unwrap_or(c).trim().to_string()
        })
        .filter(|c| !c.is_empty())
        .collect()
}

fn arg_int(e: &ExprAst) -> Option<i64> {
    match e {
        ExprAst::Literal(Literal::Int(i)) => Some(*i),
        ExprAst::Literal(Literal::Float(f)) => Some(*f as i64),
        _ => None,
    }
}

fn parse_vec_str(s: &str) -> Vec<f64> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    t.split([',', ' '])
        .filter(|x| !x.is_empty())
        .filter_map(|x| x.parse::<f64>().ok())
        .collect()
}

/// Splits a stored multi-vector value into `dims`-sized token rows. Bit
/// columns are scored as hamming and are not part of the MaxSim index.
fn parse_multivector_doc(text: &str, dims: usize) -> Option<Vec<Vec<f32>>> {
    let flat: Vec<f32> = text
        .trim()
        .trim_start_matches('[')
        .trim_end_matches(']')
        .split([',', '[', ']'])
        .filter_map(|x| x.trim().parse::<f32>().ok())
        .collect();
    if dims == 0 || flat.is_empty() || !flat.len().is_multiple_of(dims) {
        return None;
    }
    Some(flat.chunks_exact(dims).map(|c| c.to_vec()).collect())
}

/// Parses sparse items like `"0:1.0"` into `(index, value)` pairs.
fn parse_sparse(items: &[String]) -> Vec<(usize, f64)> {
    items
        .iter()
        .filter_map(|it| {
            let (i, v) = it.split_once(':')?;
            Some((
                i.trim().parse::<usize>().ok()?,
                v.trim().parse::<f64>().ok()?,
            ))
        })
        .collect()
}

fn parse_sparse_str(s: &str) -> Vec<(usize, f64)> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    if t.trim().is_empty() {
        return Vec::new();
    }
    t.split(',')
        .filter_map(|it| {
            let (i, v) = it.split_once(':')?;
            Some((
                i.trim().parse::<usize>().ok()?,
                v.trim().parse::<f64>().ok()?,
            ))
        })
        .collect()
}

/// Inner product over the shared indices of two sparse vectors.
fn sparse_dot(q: &[(usize, f64)], v: &[(usize, f64)]) -> f64 {
    let mut sum = 0.0;
    let (mut i, mut j) = (0usize, 0usize);
    while i < q.len() && j < v.len() {
        match q[i].0.cmp(&v[j].0) {
            std::cmp::Ordering::Equal => {
                sum += q[i].1 * v[j].1;
                i += 1;
                j += 1;
            }
            std::cmp::Ordering::Less => i += 1,
            std::cmp::Ordering::Greater => j += 1,
        }
    }
    sum
}

/// Recursively extracts the leaf vectors of a (nested) tensor literal.
fn parse_tensor_leaves(text: &str) -> Vec<Vec<f64>> {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return Vec::new();
    }
    if inner.trim_start().starts_with('[') {
        let mut out = Vec::new();
        for g in split_top_level(inner) {
            out.extend(parse_tensor_leaves(&g));
        }
        out
    } else {
        let v: Vec<f64> = inner
            .split([',', ' '])
            .filter(|x| !x.trim().is_empty())
            .filter_map(|x| x.trim().parse::<f64>().ok())
            .collect();
        if v.is_empty() {
            Vec::new()
        } else {
            vec![v]
        }
    }
}

/// Parses a stored bit tensor (`[[01110111],[11011011]]`) into per-embedding
/// `0.0`/`1.0` vectors, one per bracketed bit string.
fn parse_bit_leaves(text: &str) -> Vec<Vec<f64>> {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return Vec::new();
    }
    if inner.trim_start().starts_with('[') {
        let mut out = Vec::new();
        for g in split_top_level(inner) {
            out.extend(parse_bit_leaves(&g));
        }
        out
    } else {
        let v: Vec<f64> = inner
            .chars()
            .filter(|c| *c == '0' || *c == '1')
            .map(|c| if c == '1' { 1.0 } else { 0.0 })
            .collect();
        if v.is_empty() {
            Vec::new()
        } else {
            vec![v]
        }
    }
}

/// Splits `a,[b,c],d` on top-level commas (ignoring commas inside brackets).
fn split_top_level(s: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for c in s.chars() {
        match c {
            '[' => {
                depth += 1;
                cur.push(c);
            }
            ']' => {
                depth -= 1;
                cur.push(c);
            }
            ',' if depth == 0 => out.push(std::mem::take(&mut cur)),
            _ => cur.push(c),
        }
    }
    if !cur.trim().is_empty() {
        out.push(cur);
    }
    out
}

/// MaxSim: sum over query vectors of the best (max) dot against the doc vectors
/// of one tensor. Both operands are rounded to their declared element type and
/// accumulated in `f32`, mirroring the C++ engine's aligned-`f32` arithmetic.
/// For a `bit` column the query dot is an inner product over the set bits.
fn tensor_maxsim(q: &[Vec<f64>], d: &[Vec<f64>], col: ElemType, qry: ElemType) -> f64 {
    let mut sum = 0f32;
    for qv in q {
        let mut best = f32::NEG_INFINITY;
        for dv in d {
            // A stored tensor row may concatenate several `dim`-sized vectors
            // (the C++ writer emits one flat list); compare each slice.
            let pieces: Vec<&[f64]> = if qv.is_empty() {
                continue;
            } else if dv.len() == qv.len() {
                vec![dv.as_slice()]
            } else if dv.len() % qv.len() == 0 {
                dv.chunks(qv.len()).collect()
            } else {
                continue;
            };
            for piece in pieces {
                let dv = piece;
                let score = if col == ElemType::Bit {
                    if qry == ElemType::Bit {
                        // Hamming distance: higher score means more different.
                        let diff = qv
                            .iter()
                            .zip(dv)
                            .filter(|(a, b)| (**a != 0.0) != (**b != 0.0))
                            .count();
                        -(diff as f32)
                    } else {
                        let mut acc = 0f32;
                        for (a, b) in qv.iter().zip(dv) {
                            if *b != 0.0 {
                                acc += quantize_elem(*a, qry);
                            }
                        }
                        acc
                    }
                } else {
                    let mut acc = 0f32;
                    for (a, b) in qv.iter().zip(dv) {
                        acc += quantize_elem(*a, qry) * quantize_elem(*b, col);
                    }
                    acc
                };
                if score > best {
                    best = score;
                }
            }
        }
        if best.is_finite() {
            sum += best;
        }
    }
    sum as f64
}

/// Scores one stored tensor value against the query. A column holding a
/// `TensorArray` (top-level groups are themselves bracketed) scores by the max
/// over its component tensors; a plain tensor uses MaxSim directly.
fn tensor_score(
    q: &[Vec<f64>],
    doc_text: &str,
    col: ElemType,
    qry: ElemType,
    tensor_array: bool,
) -> f64 {
    let leaves = |g: &str| {
        if col == ElemType::Bit {
            parse_bit_leaves(g)
        } else {
            parse_tensor_leaves(g)
        }
    };
    let t = doc_text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return 0.0;
    }
    let groups = split_top_level(inner);
    // A `TENSORARRAY` row holds several tensors, each scored independently; a
    // plain `TENSOR` row is one tensor whose stored form may already be split
    // into `dim`-sized groups.
    if tensor_array {
        groups
            .iter()
            .map(|g| tensor_maxsim(q, &leaves(g), col, qry))
            .fold(f64::NEG_INFINITY, f64::max)
    } else {
        tensor_maxsim(q, &leaves(doc_text), col, qry)
    }
}

/// Splits `s` into lower-cased tokens using the delimiters implied by a
/// full-text analyzer: `Some([])` splits on whitespace, `Some(chars)` splits on
/// those characters only, and `None` uses the standard (alphanumeric) tokeniser.
fn split_words(s: &str, delims: &Option<Vec<char>>) -> Vec<String> {
    let parts: Vec<String> = match delims {
        Some(d) if d.is_empty() => s.split_whitespace().map(str::to_string).collect(),
        Some(d) => s
            .split(|c: char| d.contains(&c))
            .map(str::to_string)
            .collect(),
        None => return standard_tokenize(s),
    };
    parts
        .into_iter()
        .map(|t| t.to_ascii_lowercase())
        .filter(|t| !t.is_empty())
        .collect()
}

/// The `standard` analyzer's tokenizer, mirroring `tokenizer_impl.cpp` byte for
/// byte. The C++ table is built per **byte**: ASCII alphanumerics and every byte
/// above 127 are `ALLOW` (so UTF-8 bytes stay inside a token, and a `—` does not
/// split one), ASCII whitespace is `SPACE`, and everything else is a one-byte
/// `DELIMITER` token that the analyzer drops. `/` is configured as `UNITE`
/// (`AddUnites("/")`), which the scanner *skips* while continuing the token — but
/// only after the first byte, so a leading `/` is kept. Consequences verified on
/// the engine: `2012/02/14` -> `20120214`, `x/y` -> `xy`,
/// `www.example.com/path` -> `www`+`example`+`compath`.
fn standard_tokenize(s: &str) -> Vec<String> {
    analyzer::tokenize(s, &analyzer::TokenizeConfig::default())
        .into_iter()
        .map(|(token, _, _)| token.to_ascii_lowercase())
        .filter(|token| !token.is_empty())
        .collect()
}

/// Query-side tokens: the standard analyzer's stem replaces the word.
fn tokenize_text(s: &str, delims: &Option<Vec<char>>) -> Vec<String> {
    // The C++ default analyzer is `standard` (`index_full_text_impl.cpp:68`) — a
    // `StandardAnalyzer` with `STEM_LANG_ENGLISH` (`analyzer_pool_impl.cpp:266-289`)
    // — so the `None` (standard) case stems English and drops the word form.
    // `keyword`/`whitespace`(±chars) do not.
    if delims.is_none() {
        let mut analyzer = analyzer::StandardAnalyzer::new();
        analyzer.init_stemmer_english();
        analyzer.set_stem_only(true);
        return analyzer.analyze(s).into_iter().map(|t| t.text).collect();
    }
    split_words(s, delims)
}

/// The **index-side** tokenizer. The C++ `standard` analyzer emits each token
/// *and* its English stem when the two differ; it does not replace the token.
/// The engine's own statistics prove it: for a five-row probe table the C++
/// lengths are 2,4,6,5,4 for token counts 2,3,4,4,2 — every extra term is a
/// stem that differs (`multiple`->`multipl`, `many`->`mani`, `words`->`word`,
/// `another`->`anoth`), and `doc_len_sum` is 21 rather than 15. Query terms stay
/// stem-only (a boolean query for `second text multiple` scores 3, not 4).
fn tokenize_index_text(s: &str, delims: &Option<Vec<char>>) -> Vec<String> {
    if delims.is_some() {
        return split_words(s, delims);
    }
    let mut analyzer = analyzer::StandardAnalyzer::new();
    analyzer.init_stemmer_english();
    analyzer.analyze(s).into_iter().map(|t| t.text).collect()
}

/// Index-side tokens **with their position**. The C++ analyzer emits a token's
/// original and its stem at the same offset (`common_analyzer_impl.cpp`
/// re-uses `offset_` for both), so both share a position; `offset_` itself
/// advances once per raw token. Phrase queries need this.
fn tokenize_index_positions(s: &str, delims: &Option<Vec<char>>) -> Vec<(String, u32)> {
    if delims.is_some() {
        return split_words(s, delims)
            .into_iter()
            .enumerate()
            .map(|(pos, word)| (word, pos as u32))
            .collect();
    }
    let mut analyzer = analyzer::StandardAnalyzer::new();
    analyzer.init_stemmer_english();
    analyzer
        .analyze(s)
        .into_iter()
        .map(|t| (t.text, t.word_offset))
        .collect()
}

/// Byte spans of the standard tokenizer's tokens, with the forms the analyzer
/// emits for each (the word and, when it differs, its stem).
fn standard_token_spans(text: &str) -> Vec<(usize, usize, String, Option<String>)> {
    const ALLOW: u8 = 0;
    const DELIM: u8 = 1;
    const SPACE: u8 = 2;
    fn kind(b: u8) -> u8 {
        if b > 127 || b.is_ascii_alphanumeric() {
            ALLOW
        } else if b.is_ascii_whitespace() {
            SPACE
        } else {
            // The highlighter's analyzer splits on `/` (measured: the expected
            // output marks `anarchism` inside `com/anarchism-a-…`), unlike the
            // index analyzer which unites it.
            DELIM
        }
    }
    let bytes = text.as_bytes();
    let mut out = Vec::new();
    let mut i = 0usize;
    while i < bytes.len() {
        while i < bytes.len() && kind(bytes[i]) == SPACE {
            i += 1;
        }
        if i == bytes.len() {
            break;
        }
        if kind(bytes[i]) == DELIM {
            i += 1;
            continue;
        }
        let start = i;
        let mut tok = vec![bytes[i]];
        i += 1;
        while i < bytes.len() {
            match kind(bytes[i]) {
                SPACE | DELIM => break,
                ALLOW => {
                    tok.push(bytes[i]);
                    i += 1;
                }
                _ => i += 1,
            }
        }
        let word = String::from_utf8_lossy(&tok).to_ascii_lowercase();
        let stem = stem_english(&word);
        let stem = if stem != word { Some(stem) } else { None };
        out.push((start, i, word, stem));
    }
    out
}

/// Annotates the query-term occurrences of `text` with `<em>…</em>`, keeping
/// only the sentences that contain a hit and joining them with `" ... "`
/// (`Highlighter::GetHighlight` / `ApplyHighlights`).
fn highlight_text(text: &str, qterms: &[String], delims: &Option<Vec<char>>) -> String {
    if qterms.is_empty() || text.is_empty() {
        return text.to_string();
    }
    let mut matches: Vec<(usize, usize)> = Vec::new();
    if delims.is_none() {
        for (start, end, word, stem) in standard_token_spans(text) {
            let hit = qterms.contains(&word) || {
                stem.as_deref()
                    .is_some_and(|s| qterms.iter().any(|q| q == s))
            };
            if hit {
                matches.push((start, end));
            }
        }
    } else {
        // Other analyzers only need substring hits for the tests that use them.
        for q in qterms {
            let mut from = 0usize;
            while let Some(off) = text[from..].find(q.as_str()) {
                let a = from + off;
                matches.push((a, a + q.len()));
                from = a + 1;
            }
        }
    }
    if matches.is_empty() {
        return text.to_string();
    }
    matches.sort_unstable();
    let mut merged: Vec<(usize, usize)> = Vec::new();
    for (a, b) in matches {
        match merged.last_mut() {
            Some((_, last_end)) if a <= *last_end => *last_end = (*last_end).max(b),
            _ => merged.push((a, b)),
        }
    }
    let mut out = String::new();
    let mut pos = 0usize;
    let mut prev_highlighted = true;
    for sentence in split_sentences(text) {
        let end = pos + sentence.len();
        let rel: Vec<(usize, usize)> = merged
            .iter()
            .filter(|(a, b)| *a >= pos && *b <= end)
            .map(|(a, b)| (a - pos, b - pos))
            .collect();
        let highlighted = !rel.is_empty();
        if highlighted {
            let mut last = 0usize;
            for (a, b) in rel {
                out.push_str(&sentence[last..a]);
                out.push_str("<em>");
                out.push_str(&sentence[a..b]);
                out.push_str("</em>");
                last = b;
            }
            out.push_str(&sentence[last..]);
        } else if prev_highlighted {
            out.push_str(" ... ");
        }
        prev_highlighted = highlighted;
        pos = end;
    }
    out
}

/// `SentenceSplitter` (`rag_analyzer_impl.cpp:627`): splits after
/// ` *[\.\?!]['")\]\]]* *`, keeping the terminator and its trailing quotes and
/// spaces in the sentence that precedes them.
fn split_sentences(text: &str) -> Vec<String> {
    let b = text.as_bytes();
    let mut out = Vec::new();
    let mut start = 0usize;
    while start < b.len() {
        let mut found: Option<(usize, usize)> = None;
        let mut k = start;
        while k < b.len() {
            let mut j = k;
            while j < b.len() && b[j] == b' ' {
                j += 1;
            }
            if j < b.len() && matches!(b[j], b'.' | b'?' | b'!') {
                let mut e = j + 1;
                while e < b.len() && matches!(b[e], b'\'' | b'"' | b')' | b']') {
                    e += 1;
                }
                while e < b.len() && b[e] == b' ' {
                    e += 1;
                }
                found = Some((k, e));
                break;
            }
            k += 1;
        }
        match found {
            Some((ms, me)) => {
                if ms > start {
                    out.push(text[start..me].to_string());
                }
                start = me;
            }
            None => {
                out.push(text[start..].to_string());
                break;
            }
        }
    }
    out
}

/// The C++ default analyzer's English stemmer (`STEM_LANG_ENGLISH`).
static ENGLISH_STEMMER: std::sync::OnceLock<rust_stemmers::Stemmer> = std::sync::OnceLock::new();

fn stem_english(token: &str) -> String {
    let s = ENGLISH_STEMMER
        .get_or_init(|| rust_stemmers::Stemmer::create(rust_stemmers::Algorithm::English));
    s.stem(token).to_string()
}

/// Splits a field-query group body into terms, keeping a quoted phrase together
/// and dropping the quote marks (`doc:"a b" doc2:c` -> `["doc:a b", "doc2:c"]`).
fn split_query_terms(s: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_quote = false;
    for c in s.chars() {
        match c {
            '"' => in_quote = !in_quote,
            c if c.is_whitespace() && !in_quote => {
                if !cur.is_empty() {
                    out.push(std::mem::take(&mut cur));
                }
            }
            c => cur.push(c),
        }
    }
    if !cur.is_empty() {
        out.push(cur);
    }
    out
}

/// Counts the occurrences of `unit` at consecutive positions in a document.
/// Both the original token and its stem live at one position, so a query term
/// matches a position when either form is there.
fn phrase_freq(at: &HashMap<u32, Vec<&str>>, unit: &[String]) -> usize {
    if unit.is_empty() {
        return 0;
    }
    let mut n = 0usize;
    for (start, terms) in at {
        if !terms.iter().any(|t| *t == unit[0]) {
            continue;
        }
        let mut ok = true;
        for (k, q) in unit.iter().enumerate().skip(1) {
            match at.get(&(start + k as u32)) {
                Some(ts) if ts.contains(&q.as_str()) => {}
                _ => {
                    ok = false;
                    break;
                }
            }
        }
        if ok {
            n += 1;
        }
    }
    n
}

/// Splits a `MATCH TEXT` query into scoring units, keeping quoted groups
/// together: `"second default text"` is one phrase unit that only matches where
/// its tokens sit at consecutive positions, while bare words are single-term
/// units. `term^2.0` boosts are stripped here — boosts are applied per field.
fn query_units(query: &str) -> Vec<(bool, String, f64)> {
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_quote = false;
    let mut was_quoted = false;
    let flush = |cur: &mut String, quoted: &mut bool, out: &mut Vec<(bool, String)>| {
        if !cur.is_empty() {
            out.push((*quoted, std::mem::take(cur)));
            *quoted = false;
        }
    };
    for c in query.chars() {
        match c {
            '"' => {
                flush(&mut cur, &mut was_quoted, &mut out);
                in_quote = !in_quote;
            }
            c if c.is_whitespace() && !in_quote => flush(&mut cur, &mut was_quoted, &mut out),
            c => {
                if in_quote {
                    was_quoted = true;
                }
                cur.push(c);
            }
        }
    }
    flush(&mut cur, &mut was_quoted, &mut out);
    out.into_iter()
        .map(|(q, t)| {
            // `term^2.0` boosts that one unit.
            let (t, boost) = match t.split_once('^') {
                Some((t, b)) => (t.to_string(), b.parse::<f64>().unwrap_or(1.0)),
                None => (t, 1.0),
            };
            (q, t, boost)
        })
        .collect()
}

/// A `MATCH TEXT ('', '(doc:"a b")^4.5 (doc2:c doc2:d)^3', …)` clause names its
/// fields inside the query string. Returns one entry per group: its boost and
/// the `(field, term)` pairs. `None` when no group names a field, so ordinary
/// queries are unaffected.
#[allow(clippy::type_complexity)]
fn parse_field_query(q: &str) -> Option<Vec<(f64, Vec<(String, String)>)>> {
    let chars: Vec<char> = q.chars().collect();
    let mut i = 0;
    let mut groups: Vec<(f64, Vec<(String, String)>)> = Vec::new();
    let mut any_field = false;
    while i < chars.len() {
        while i < chars.len() && chars[i].is_whitespace() {
            i += 1;
        }
        if i >= chars.len() {
            break;
        }
        let body: String = if chars[i] == '(' {
            let start = i + 1;
            let mut depth = 1;
            i += 1;
            while i < chars.len() {
                match chars[i] {
                    '(' => depth += 1,
                    ')' => {
                        depth -= 1;
                        if depth == 0 {
                            break;
                        }
                    }
                    _ => {}
                }
                i += 1;
            }
            let inner: String = chars[start..i.min(chars.len())].iter().collect();
            if i < chars.len() {
                i += 1; // skip ')'
            }
            inner
        } else {
            let start = i;
            while i < chars.len() && !chars[i].is_whitespace() {
                i += 1;
            }
            chars[start..i].iter().collect()
        };
        // optional `^<boost>`
        let mut boost = 1.0f64;
        if i < chars.len() && chars[i] == '^' {
            i += 1;
            let start = i;
            while i < chars.len() && (chars[i].is_ascii_digit() || chars[i] == '.') {
                i += 1;
            }
            boost = chars[start..i]
                .iter()
                .collect::<String>()
                .parse()
                .unwrap_or(1.0);
        }
        let mut terms: Vec<(String, String)> = Vec::new();
        for raw in split_query_terms(&body) {
            match raw.split_once(':') {
                Some((f, t)) if !f.is_empty() => {
                    any_field = true;
                    terms.push((f.to_string(), t.trim().to_string()));
                }
                _ => terms.push((String::new(), raw)),
            }
        }
        groups.push((boost, terms));
    }
    if any_field {
        Some(groups)
    } else {
        None
    }
}

/// Parses a `topn=<n>` value out of a trailing parameter string.
fn parse_topn_param(p: &str) -> Option<usize> {
    let idx = p.to_ascii_lowercase().find("topn")?;
    let rest = &p[idx + 4..];
    let eq = rest.find('=')?;
    let digits: String = rest[eq + 1..]
        .trim_start()
        .chars()
        .take_while(|c| c.is_ascii_digit())
        .collect();
    digits.parse().ok()
}

/// Returns `(similarity, distance)` for the given metric.
fn vector_score(q: &[f64], v: &[f64], metric: &str) -> (f64, f64) {
    let dot: f64 = q.iter().zip(v).map(|(a, b)| a * b).sum();
    let nq: f64 = q.iter().map(|a| a * a).sum::<f64>().sqrt();
    let nv: f64 = v.iter().map(|a| a * a).sum::<f64>().sqrt();
    match metric {
        "ip" => (dot, -dot),
        "cosine" => {
            let c = if nq > 0.0 && nv > 0.0 {
                dot / (nq * nv)
            } else {
                0.0
            };
            (c, 1.0 - c)
        }
        "hamming" => {
            // Hamming distance: the number of differing positions.
            let d = q
                .iter()
                .zip(v)
                .filter(|(a, b)| (**a != 0.0) != (**b != 0.0))
                .count() as f64;
            (1.0 / (1.0 + d), d)
        }
        _ => {
            // `l2` reports the *squared* Euclidean distance (no square root).
            let d: f64 = q.iter().zip(v).map(|(a, b)| (a - b) * (a - b)).sum();
            (1.0 / (1.0 + d), d)
        }
    }
}

/// Runs the operator stages over rows received from a bounded producer/consumer
/// exchange. The producer task and consumer run concurrently; a full channel
/// backpressures the producer rather than deadlocking.
async fn pipeline(
    columns: Vec<String>,
    rows: Vec<Vec<Value>>,
    stages: Vec<Stage>,
    _cap: usize,
) -> Result<(Vec<String>, Vec<Vec<String>>)> {
    // The source is already materialized, and every stage is a synchronous
    // transform. The previous per-row `mpsc` hop cost ~1 µs/row (20k rows:
    // ~20 ms) without overlapping any work; the reference's pipeline stages
    // are likewise in-process calls over a `DataBlock`.
    let mut data: Vec<Vec<Value>> = rows;

    let mut cols = columns;
    for stage in &stages {
        match stage {
            Stage::Filter(expr) => data.retain(|r| expr.eval_predicate(r)),
            Stage::Projection(exprs, names) => {
                // `SELECT *` compiles to the identity projection; copying every
                // row again doubles the cost of a full-table select.
                let width = data.first().map(Vec::len).unwrap_or(0);
                let identity = exprs.len() == width
                    && exprs
                        .iter()
                        .enumerate()
                        .all(|(i, e)| matches!(e, Expr::Column(j) if *j == i));
                if !identity {
                    data = data
                        .iter()
                        .map(|r| exprs.iter().map(|e| e.eval(r)).collect())
                        .collect();
                }
                cols = names.clone();
            }
            Stage::Aggregate {
                group_keys,
                group_names,
                aggs,
            } => {
                let (new_cols, new_rows) = apply_aggregate(&data, group_keys, group_names, aggs);
                cols = new_cols;
                data = new_rows;
            }
            Stage::Sort { keys } => sort_rows(&mut data, keys),
            Stage::Limit(n) => data.truncate(*n as usize),
            Stage::Offset(n) => {
                let k = (*n as usize).min(data.len());
                data.drain(0..k);
            }
        }
    }

    let out = data
        .iter()
        .map(|r| r.iter().map(|v| v.to_slt_string()).collect())
        .collect();
    Ok((cols, out))
}

fn apply_aggregate(
    rows: &[Vec<Value>],
    group_keys: &[Expr],
    group_names: &[String],
    aggs: &[(AggFunc, Vec<usize>, bool, String)],
) -> (Vec<String>, Vec<Vec<Value>>) {
    let mut names: Vec<String> = group_names.to_vec();
    for (_, _, _, label) in aggs {
        names.push(label.clone());
    }

    if group_keys.is_empty() {
        let row: Vec<Value> = aggs
            .iter()
            .map(|(f, cols, distinct, _)| aggregate(*f, cols, *distinct, rows))
            .collect();
        return (names, vec![row]);
    }
    // group by the tuple of key columns, preserving first-seen order
    let mut order: Vec<Vec<Value>> = Vec::new();
    let mut groups: Vec<(Vec<Value>, Vec<Vec<Value>>)> = Vec::new();
    for r in rows {
        let key: Vec<Value> = group_keys.iter().map(|k| k.eval(r)).collect();
        let idx = match order.iter().position(|o| *o == key) {
            Some(i) => i,
            None => {
                order.push(key.clone());
                groups.push((key.clone(), Vec::new()));
                groups.len() - 1
            }
        };
        groups[idx].1.push(r.clone());
    }
    let out = groups
        .into_iter()
        .map(|(key, grp)| {
            let mut row = key;
            for (f, cols, distinct, _) in aggs {
                row.push(aggregate(*f, cols, *distinct, &grp));
            }
            row
        })
        .collect();
    (names, out)
}

fn sort_rows(rows: &mut [Vec<Value>], keys: &[(infinity_expression::Expr, bool)]) {
    rows.sort_by(|a, b| {
        for (key, asc) in keys {
            let x = key.eval(a);
            let y = key.eval(b);
            let (x, y) = (&x, &y);
            // NULLs order before every non-NULL (so ASC → NULLS FIRST, and the
            // reverse below gives DESC → NULLS LAST).
            let mut ord = match (x, y) {
                (Value::Null, Value::Null) => std::cmp::Ordering::Equal,
                (Value::Null, _) => std::cmp::Ordering::Less,
                (_, Value::Null) => std::cmp::Ordering::Greater,
                // Fall back to a textual comparison for mixed types so the
                // order stays total (e.g. an int beside a non-numeric string).
                _ => x
                    .partial_cmp_value(y)
                    .unwrap_or_else(|| x.to_slt_string().cmp(&y.to_slt_string())),
            };
            if !*asc {
                ord = ord.reverse();
            }
            if ord != std::cmp::Ordering::Equal {
                return ord;
            }
        }
        std::cmp::Ordering::Equal
    });
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    /// The rows of a `SELECT`, as rendered strings.
    fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
        for r in e.execute(sql).unwrap() {
            if let QueryResult::Rows { rows, .. } = r {
                return rows;
            }
        }
        Vec::new()
    }

    fn seed(e: &Engine) {
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "c"), (2, "a"), (3, "b"), (4, "a")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
    }

    #[test]
    fn a_derived_table_scans_the_subquery_result() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE d (id INT, s VARCHAR);").unwrap();
        e.execute("INSERT INTO d VALUES (1,'a'), (2,'b'), (3,'c');")
            .unwrap();
        // The outer query scans the subquery's rows, and may filter/sort them.
        assert_eq!(
            rows(&e, "SELECT id FROM (SELECT id FROM d) ORDER BY id DESC;"),
            vec![
                vec!["3".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM (SELECT id FROM d WHERE id > 1) ORDER BY id;"
            ),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        // A table legitimately named `select3` is not mistaken for a subquery.
        e.execute("CREATE TABLE select3 (c1 INT, c2 INT, c3 INT);")
            .unwrap();
        e.execute("INSERT INTO select3 VALUES (1,2,3);").unwrap();
        assert_eq!(
            rows(&e, "SELECT c3, c2, c1 from select3;"),
            vec![vec!["3".to_string(), "2".to_string(), "1".to_string()]]
        );
    }

    #[test]
    fn match_tensor_fusion_reranks_by_the_search_tensor() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tf (title VARCHAR, t TENSOR(FLOAT,2), body VARCHAR);")
            .unwrap();
        e.execute("CREATE INDEX tf_ft ON tf (body) USING FULLTEXT;")
            .unwrap();
        e.execute("INSERT INTO tf VALUES ('a', [[1,0]], 'off'), ('b', [[0,1]], 'off');")
            .unwrap();
        // The rerank ignores the children's BM25: only the tensor similarity counts.
        let rows = rows(
            &e,
            "SELECT title, ROUND(SCORE(), 4) FROM tf SEARCH MATCH TEXT ('body', 'off', 'topn=4'), \
             FUSION('match_tensor', 'column_name=t;search_tensor=[[10.0,0.0]];\
             tensor_data_type=float;match_method=MaxSim;topn=2');",
        );
        assert_eq!(
            rows,
            vec![
                vec!["a".to_string(), "10.000000".to_string()],
                vec!["b".to_string(), "0.000000".to_string()],
            ]
        );
    }

    #[test]
    fn fulltext_threshold_filter_uses_the_index_score() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ft (num INT, doc VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO ft VALUES (1,'alpha'), (2,'alpha beta'), (3,'gamma delta');")
            .unwrap();
        e.execute("CREATE INDEX ft_idx ON ft (doc) USING FULLTEXT;")
            .unwrap();
        // A threshold is a *score* filter, which only the index can answer.
        let keep = rows(
            &e,
            "SELECT num FROM ft WHERE filter_fulltext('doc','alpha','threshold=0.0001');",
        );
        assert_eq!(keep, vec![vec!["1".to_string()], vec!["2".to_string()]]);
        let none = rows(
            &e,
            "SELECT num FROM ft WHERE filter_fulltext('doc','alpha','threshold=999');",
        );
        assert!(none.is_empty());
    }

    #[test]
    fn a_gram_index_filter_requires_the_query_contiguously() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE g (id INT, doc VARCHAR);").unwrap();
        e.execute(
            "INSERT INTO g VALUES (1,'harmful chemical reaction'), \
             (2,'chemical processes are harmful');",
        )
        .unwrap();
        e.execute("CREATE INDEX g_sg ON g (doc) USING FULLTEXT WITH (analyzer='sparsegram-3-12');")
            .unwrap();
        e.execute("CREATE INDEX g_std ON g (doc) USING FULLTEXT WITH (analyzer='standard');")
            .unwrap();
        // The gram index needs the query contiguous; the regular one accepts the
        // terms apart. Same filter, different index named by `@`.
        let gram = rows(
            &e,
            "SELECT id FROM g WHERE filter_fulltext('doc@g_sg','harmful chemical','operator=and');",
        );
        assert_eq!(gram, vec![vec!["1".to_string()]]);
        let regular = rows(
            &e,
            "SELECT id FROM g WHERE filter_fulltext('doc','harmful chemical','operator=and');",
        );
        assert_eq!(regular.len(), 2);
    }

    #[test]
    fn rank_features_bonus_sums_listed_tags() {
        let cell = r#"[{"Tag1":1},{"Tag2":2}]"#;
        let tags = [("Tag1".to_string(), 1.0), ("Tag2".to_string(), 2.0)];
        assert_eq!(rank_features_bonus(cell, &tags), 5.0);
        // A tag the document does not carry contributes nothing.
        let tags = [("Tag3".to_string(), 4.0), ("Tag1".to_string(), 1.0)];
        assert_eq!(rank_features_bonus(cell, &tags), 1.0);
        assert_eq!(rank_features_bonus("[]", &tags), 0.0);
    }

    /// A store opened exactly the way `Engine::open` opens its catalog, so a
    /// test can plant or inspect the C++ key space at `<dir>/catalog`.
    fn open_catalog_store(dir: &std::path::Path) -> std::sync::Arc<dyn MetaStore> {
        #[cfg(feature = "rocksdb-store")]
        {
            std::sync::Arc::new(infinity_storage::RocksMetaStore::open(dir).unwrap())
        }
        #[cfg(not(feature = "rocksdb-store"))]
        {
            std::sync::Arc::new(FjallMetaStore::open(dir, "catalog").unwrap())
        }
    }

    #[test]
    fn flush_mirrors_the_catalog_into_the_cpp_key_space() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        // The C++ keys the C++ engine reads sit beside the native catalog in
        // the same `<dir>/catalog` store.
        let cpp =
            infinity_catalog::cpp_layout::read_cpp_layout(e.catalog().store().as_ref()).unwrap();
        assert_eq!(cpp.len(), 1, "expected one C++ table record");
        assert_eq!(cpp[0].table_name, "t");
        assert_eq!(cpp[0].db_name, "default_db");
        assert_eq!(
            cpp[0]
                .columns
                .iter()
                .map(|c| c.column_name.clone())
                .collect::<Vec<_>>(),
            vec!["id", "name"]
        );
        assert_eq!(
            infinity_catalog::cpp_layout::type_text_of(&cpp[0].columns[0].column_type).unwrap(),
            "INTEGER"
        );
    }

    #[test]
    fn opens_a_directory_written_by_the_cpp_engine() {
        // A C++ engine's schema, dropped into an otherwise empty directory's
        // catalog store with no native catalog: opening it must import it.
        let src = tempdir().unwrap();
        let cat = Catalog::open(src.path()).unwrap();
        cat.create_table_typed(
            "default_db.cpp_t",
            &["id".to_string(), "v".to_string()],
            &[None, None],
            &[true, false],
            &["INTEGER".to_string(), "EMBEDDING(FLOAT,3)".to_string()],
            &[None, None],
        )
        .unwrap();

        let dir = tempdir().unwrap();
        {
            let store = open_catalog_store(&dir.path().join("catalog"));
            cat.write_cpp_layout(store.as_ref()).unwrap();
        } // release the store lock before the engine opens it

        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            e.table_schema("default_db.cpp_t").unwrap(),
            vec!["id".to_string(), "v".to_string()]
        );
        assert_eq!(e.column_types("default_db.cpp_t").unwrap()[0], "INTEGER");
        // The imported table is usable.
        e.execute("INSERT INTO default_db.cpp_t VALUES (7, [1.0, 2.0, 3.0]);")
            .unwrap();
        match &e.execute("SELECT id FROM default_db.cpp_t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 1),
            other => panic!("unexpected {other:?}"),
        }
    }

    #[test]
    fn select_projection_filter_order_limit() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT id, name FROM t WHERE id >= 2 ORDER BY name ASC LIMIT 2;")
            .unwrap()[0]
        {
            QueryResult::Rows { rows, columns } => {
                assert_eq!(columns, &vec!["id".to_string(), "name".to_string()]);
                assert_eq!(rows.len(), 2);
                assert_eq!(rows[0], vec!["2".to_string(), "a".to_string()]);
                assert_eq!(rows[1], vec!["4".to_string(), "a".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn global_aggregates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT COUNT(*), SUM(id), AVG(id), MIN(id), MAX(id) FROM t;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(
                    columns,
                    &[
                        "count(star)",
                        "sum(id)",
                        "(sum(id) / count(id))",
                        "min(id)",
                        "max(id)"
                    ]
                    .iter()
                    .map(|s| s.to_string())
                    .collect::<Vec<_>>()
                );
                assert_eq!(
                    rows,
                    &[["4", "10", "2.500000", "1", "4"]
                        .iter()
                        .map(|s| s.to_string())
                        .collect::<Vec<_>>()]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn group_by_with_order() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT name, COUNT(*) FROM t GROUP BY name ORDER BY name;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(
                    columns,
                    &vec!["name".to_string(), "count(star)".to_string()]
                );
                assert_eq!(
                    rows,
                    &vec![
                        vec!["a".to_string(), "2".to_string()],
                        vec!["b".to_string(), "1".to_string()],
                        vec!["c".to_string(), "1".to_string()]
                    ]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn join_between_two_tables() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE a (aid INT, an VARCHAR);").unwrap();
        e.execute("CREATE TABLE b (bid INT, bn VARCHAR);").unwrap();
        for (i, n) in [(1, "x"), (2, "y")] {
            e.execute(&format!("INSERT INTO a VALUES ({i}, '{n}');"))
                .unwrap();
        }
        for (i, n) in [(2, "yy"), (3, "zz")] {
            e.execute(&format!("INSERT INTO b VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
        match &e
            .execute("SELECT aid, bn FROM a JOIN b ON aid = bid;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(columns, &vec!["aid".to_string(), "bn".to_string()]);
                assert_eq!(rows, &vec![vec!["2".to_string(), "yy".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn bounded_capacity_one_does_not_deadlock() {
        let dir = tempdir().unwrap();
        let e = Engine::open_with_capacity(dir.path(), 1).unwrap();
        e.execute("CREATE TABLE t (id INT);").unwrap();
        for i in 0..50 {
            e.execute(&format!("INSERT INTO t VALUES ({i});")).unwrap();
        }
        e.flush().unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 50),
            other => panic!("{other:?}"),
        }
        match &e.execute("SELECT * FROM t LIMIT 3;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 3),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn select_literal_and_missing_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            e.execute("SELECT 1, 'x';").unwrap(),
            vec![QueryResult::Rows {
                columns: vec!["c0".into(), "c1".into()],
                rows: vec![vec!["1".into(), "x".into()]]
            }]
        );
        assert!(e.execute("SELECT * FROM nope;").is_err());
    }

    #[test]
    fn index_scan_matches_expected_and_survives_restart() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "a"), (2, "b"), (3, "a"), (4, "b")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.execute("CREATE INDEX idx_name ON t (name);").unwrap();
        match &e.execute("SELECT id FROM t WHERE name = 'a';").unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows, &vec![vec!["1".to_string()], vec!["3".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
        e.flush().unwrap();
        drop(e); // release the fjall lock before reopening
                 // reopen: the index is rebuilt from catalog metadata and still scans
        let e2 = Engine::open(dir.path()).unwrap();
        match &e2.execute("SELECT id FROM t WHERE name = 'b';").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 2),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn delete_with_where_and_all() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        e.execute("DELETE FROM t WHERE id >= 2;").unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 1),
            other => panic!("{other:?}"),
        }
        e.execute("DELETE FROM t;").unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 0),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn update_is_delete_plus_append() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        e.execute("UPDATE t SET name = 'X' WHERE id >= 2;").unwrap();
        match &e.execute("SELECT * FROM t ORDER BY id;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows.len(), 3);
                assert_eq!(rows[0], vec!["1".to_string(), "a".to_string()]);
                assert_eq!(rows[1], vec!["2".to_string(), "X".to_string()]);
                assert_eq!(rows[2], vec!["3".to_string(), "X".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn multi_row_insert() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 3),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn persists_across_restart() {
        let dir = tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            seed(&e);
        }
        let e = Engine::open(dir.path()).unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 4),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn copy_csv_then_vector_search() {
        let dir = tempdir().unwrap();
        let csv = dir.path().join("vec.csv");
        std::fs::write(
            &csv,
            "2,\"[0.1, 0.2, 0.3, -0.2]\"\n4,\"[0.2, 0.1, 0.3, 0.4]\"\n\
             6,\"[0.3, 0.2, 0.1, 0.4]\"\n8,\"[0.4, 0.3, 0.2, 0.1]\"\n",
        )
        .unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE v (c1 INT, c2 EMBEDDING(FLOAT, 4));")
            .unwrap();
        e.execute(&format!(
            "COPY v FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        assert_eq!(e.row_count("v").unwrap(), 4);

        // cosine top-3 → c1 8, 6, 4 with hidden row-id / similarity columns.
        match &e
            .execute(
                "SELECT c1, ROW_ID(), SIMILARITY() FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'cosine', 3);",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows.len(), 3);
                assert_eq!(rows[0], vec!["8", "3", "0.966755"]);
                assert_eq!(rows[2], vec!["4", "1", "0.823532"]);
            }
            other => panic!("{other:?}"),
        }

        // threshold prunes the weakest match.
        match &e
            .execute(
                "SELECT c1 FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'cosine', 3) \
                 WITH (threshold = 0.9);",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => assert_eq!(rows, &vec![vec!["8".to_string()]]),
            other => panic!("{other:?}"),
        }

        // WHERE filters candidates before top-N (l2 over {2,4,6} → 6 then 4).
        match &e
            .execute(
                "SELECT c1 FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 2) \
                 WHERE c1 < 7;",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows, &vec![vec!["6".to_string()], vec!["4".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
    }

    /// `InitAbstractIndexT` picks the distance from `metric`; the mirror's
    /// graph answers self-queries for l2, cosine and inner product.
    #[test]
    fn hnsw_metric_variants_answer_self_queries() {
        // Inner product is maximised by the largest-norm row, not the row
        // itself; l2/cosine are maximised by the row.
        for (metric, target) in [("l2", 42), ("cosine", 42), ("ip", 199)] {
            let dir = tempdir().unwrap();
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE hm (id INT, v EMBEDDING(FLOAT,4));")
                .unwrap();
            let mut sql = String::from("INSERT INTO hm VALUES ");
            for i in 0..200 {
                sql.push_str(&format!("({i}, [{i}.0,{}.0,{}.0,1.0])", i + 1, i + 2));
                if i < 199 {
                    sql.push(',');
                }
            }
            sql.push(';');
            e.execute(&sql).unwrap();
            e.execute(&format!(
                "CREATE INDEX hm_i ON hm (v) USING HNSW WITH (metric='{metric}');"
            ))
            .unwrap();
            e.flush().unwrap();
            let got = rows(
                &e,
                &format!(
                    "SELECT id FROM hm SEARCH MATCH VECTOR \
                     (v, [{target}.0,{}.0,{}.0,1.0], 'float', '{metric}', 1);",
                    target + 1,
                    target + 2
                ),
            );
            assert_eq!(got, vec![vec![target.to_string()]], "metric {metric}");
        }
    }

    /// C++ `test_lsg_build_ut.cpp::test1`: an LSG-built HNSW keeps >= 95%
    /// recall@1 for self-queries.
    #[test]
    fn lsg_hnsw_keeps_self_query_recall() {
        let dim = 16usize;
        let n = 128usize;
        let vector = |i: usize| -> String {
            let parts: Vec<String> = (0..dim)
                .map(|j| {
                    let v = ((i * 31 + j * 17 + (i * j) % 13) % 100) as f32 / 10.0;
                    format!("{v}")
                })
                .collect();
            format!("[{}]", parts.join(","))
        };
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE lg (id INT, v EMBEDDING(FLOAT,16));")
            .unwrap();
        let mut sql = String::from("INSERT INTO lg VALUES ");
        for i in 0..n {
            sql.push_str(&format!("({i}, {})", vector(i)));
            if i + 1 < n {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        e.execute(
            "CREATE INDEX lg_i ON lg (v) USING HNSW WITH \
             (metric='l2', build_type='lsg', lsg_config='sample_ratio=0.5,ls_k=10,alpha=1.0');",
        )
        .unwrap();
        e.flush().unwrap();
        let mut hits = 0;
        for i in 0..n {
            let got = rows(
                &e,
                &format!(
                    "SELECT id FROM lg SEARCH MATCH VECTOR (v, {}, 'float', 'l2', 1);",
                    vector(i)
                ),
            );
            if got == vec![vec![i.to_string()]] {
                hits += 1;
            }
        }
        let recall = hits as f64 / n as f64;
        assert!(recall >= 0.95, "LSG recall@1 = {recall}");
    }

    /// C++ `InitAbstractIndexT`: the HNSW mirror stores plain, LVQ or RaBitQ
    /// vectors per the `encode` option and answers the same top-1.
    #[test]
    fn hnsw_encodings_answer_vector_search() {
        for encode in ["plain", "lvq", "rabitq"] {
            let dir = tempdir().unwrap();
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE he (id INT, v EMBEDDING(FLOAT,4));")
                .unwrap();
            let mut sql = String::from("INSERT INTO he VALUES ");
            for i in 0..200 {
                sql.push_str(&format!("({i}, [{i}.0,{}.0,{}.0,1.0])", i + 1, i + 2));
                if i < 199 {
                    sql.push(',');
                }
            }
            sql.push(';');
            e.execute(&sql).unwrap();
            e.execute(&format!(
                "CREATE INDEX he_i ON he (v) USING HNSW WITH (metric='l2', encode='{encode}');"
            ))
            .unwrap();
            e.flush().unwrap();
            let rows = rows(
                &e,
                "SELECT id FROM he SEARCH MATCH VECTOR \
                 (v, [42.0,43.0,44.0,1.0], 'float', 'l2', 1);",
            );
            assert_eq!(rows, vec![vec!["42".to_string()]], "encode {encode}");
        }
    }

    /// C++ `Import` populates the memory indexes: a `COPY` into a table with an
    /// HNSW index feeds the memory segment and honours `mem_index_capacity`,
    /// so the graph covers the imported rows without waiting for an INSERT.
    #[test]
    fn copy_populates_the_hnsw_memory_segment() {
        let dir = tempdir().unwrap();
        let csv = dir.path().join("hnsw.csv");
        std::fs::write(
            &csv,
            "1,\"[0.0, 0.0, 0.0, 0.0]\"\n2,\"[1.0, 0.0, 0.0, 0.0]\"\n\
             3,\"[0.0, 1.0, 0.0, 0.0]\"\n4,\"[0.0, 0.0, 1.0, 0.0]\"\n",
        )
        .unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(3);
        e.execute("CREATE TABLE cvi (c1 INT, c2 EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX cvi_i ON cvi (c2) USING HNSW WITH (metric = l2);")
            .unwrap();
        e.execute(&format!(
            "COPY cvi FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        let indexed: usize = e
            .tables
            .lock()
            .unwrap()
            .values()
            .flat_map(|t| t.hnsw_mem.values())
            .map(|m| m.lock().map(|m| m.len()).unwrap_or(0))
            .sum();
        assert_eq!(
            indexed, 0,
            "the 4-row batch reached the capacity and dumped"
        );
        let dumped: usize = e
            .load_vector_indexes("default_db.cvi", "cvi_i")
            .iter()
            .map(|seg| seg.labels.len())
            .sum();
        assert_eq!(dumped, 4);
    }

    #[test]
    fn sparse_and_tensor_search() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE s (c1 INT, c2 SPARSE(FLOAT,100));")
            .unwrap();
        e.execute(
            "INSERT INTO s VALUES (1, [0:1.0,10:1.0,20:1.0,80:1.0]), \
             (2, [0:2.0,20:2.0,80:2.0]), (4, [0:4.0,80:4.0]), (5, []);",
        )
        .unwrap();
        match &e
            .execute("SELECT c1 FROM s SEARCH MATCH SPARSE (c2, [0:1.0,20:2.0,80:3.0], 'ip', 3);")
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(
                    rows,
                    &vec![
                        vec!["4".to_string()],
                        vec!["2".to_string()],
                        vec!["1".to_string()]
                    ]
                );
            }
            other => panic!("{other:?}"),
        }

        e.execute("CREATE TABLE t (c1 INT, c2 TENSOR(FLOAT,4));")
            .unwrap();
        e.execute(
            "INSERT INTO t VALUES (2, [[0.1,0.2,0.3,-0.2],[0.3,-0.2,0.2,0.3]]), \
             (4, [[0.2,0.1,0.3,0.4]]);",
        )
        .unwrap();
        // MaxSim over the query's two vectors: 2.21 + (-7.4) = -5.19.
        match &e
            .execute(
                "SELECT c1, ROUND(SCORE(), 4) FROM t SEARCH MATCH TENSOR \
                 (c2, [[0.0,-10.0,0.0,0.7],[9.2,45.6,-55.8,3.5]], 'float', 'maxsim', '');",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows[0], vec!["2".to_string(), "-5.190000".to_string()]);
                assert_eq!(rows[1], vec!["4".to_string(), "-9.660000".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    /// `USING PLAID`: the tensor index builds the reference's `PlaidIndex`
    /// once enough embeddings are buffered, answers `MATCH TENSOR` from it, and
    /// `flush()` mirrors it as a C++ PLAID chunk (`PlaidIndex::SaveIndexInner`).
    #[test]
    fn plaid_index_answers_tensor_search_and_mirrors_a_chunk() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE pt (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..300 {
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 7) as f32 + j as f32 * 0.5;
                let y = ((i + j) % 5) as f32 - 1.0;
                let z = ((i * j) % 11) as f32 * 0.25;
                tokens.push(format!("[{x},{y},{z},{}]", (i % 3) as f32));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO pt VALUES {};", values.join(",")))
            .unwrap();
        let query = "SELECT id FROM pt SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');";
        // Without an index the scan is exhaustive.
        let baseline = rows(&e, query);
        assert_eq!(baseline.len(), 3);

        e.execute("CREATE INDEX pidx ON pt (t) USING PLAID WITH (nbits = 4, n_centroids = 8);")
            .unwrap();
        // The scan builds `PlaidIndexInMem` over the 1200 embeddings.
        let indexed = rows(&e, query);
        let tkey = e.table_key("default_db.pt");
        let built = e
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .and_then(|t| t.mv.get("t"))
            .map(|m| m.read().unwrap().plaid.is_some())
            .unwrap_or(false);
        assert!(built, "PLAID index was not built");
        // The residual rerank keeps the brute-force ranking.
        assert_eq!(indexed.len(), 3);
        assert_eq!(indexed[0], baseline[0]);

        e.flush().unwrap();
        let loaded = e.read_cpp_plaid_index("default_db.pt", "pidx").unwrap();
        assert_eq!(loaded.doc_num(), 300);
        assert_eq!(loaded.total_embedding_num(), 1200);
    }

    /// `USING SMVE`: the tensor scan builds `SMVEIndexInMem` (projection +
    /// BMP) and re-ranks its candidates with exact MaxSim.
    #[test]
    fn smve_index_answers_tensor_search() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE st (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..40 {
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 5) as f32 + j as f32 * 0.25;
                let y = ((i + j) % 7) as f32 * 0.5 - 1.0;
                tokens.push(format!("[{x},{y},0.5,{}]", (i % 3) as f32));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO st VALUES {};", values.join(",")))
            .unwrap();
        let query = "SELECT id FROM st SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.5,0.0]], 'float', 'maxsim', 'topn=3');";
        let baseline = rows(&e, query);
        assert_eq!(baseline.len(), 3);

        e.execute("CREATE INDEX sidx ON st (t) USING SMVE WITH (width = 64, topk = 8);")
            .unwrap();
        let indexed = rows(&e, query);
        eprintln!(
            "DEBUG indexes: {:?}",
            e.storage
                .catalog
                .list_indexes()
                .unwrap()
                .iter()
                .map(|i| (
                    i.name.clone(),
                    i.index_type.clone(),
                    i.column.clone(),
                    i.options.clone()
                ))
                .collect::<Vec<_>>()
        );
        let tkey = e.table_key("default_db.st");
        let built = e
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .and_then(|t| t.mv.get("t"))
            .map(|m| m.read().unwrap().smve.is_some())
            .unwrap_or(false);
        assert!(built, "SMVE index was not built");
        assert_eq!(indexed[0], baseline[0]);
    }

    /// The multi-vector index is built once (first use), appended at write time,
    /// and dumped by `flush()`; queries never rebuild it.
    #[test]
    fn multivector_index_is_built_at_write_time() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mv (id INT, v MULTIVECTOR(FLOAT,2));")
            .unwrap();
        e.execute("INSERT INTO mv VALUES (1, [[1.0,0.0],[0.0,1.0]]), (2, [[0.2,0.2]]);")
            .unwrap();
        let q = "SELECT id FROM mv SEARCH MATCH TENSOR \
                 (v, [1.0,0.0], 'float', 'maxsim', 'topn=3');";
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        let idx_dir = std::fs::read_dir(dir.path().join("multivector"))
            .unwrap()
            .next()
            .unwrap()
            .unwrap()
            .path();
        let count = |ext: &str| {
            std::fs::read_dir(&idx_dir)
                .unwrap()
                .filter_map(|x| x.ok())
                .filter(|x| x.path().extension().is_some_and(|y| y == ext))
                .count()
        };
        // One chunk of raw tokens plus its EMVB chunk, written by the build.
        assert_eq!(count("mvec"), 1);
        assert_eq!(count("emvb"), 1);

        // A later insert goes to the memory tail and ranks immediately.
        e.execute("INSERT INTO mv VALUES (3, [[1.0,0.0]]);")
            .unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["3".to_string()],
                vec!["2".to_string()]
            ]
        );

        // `flush()` dumps the tail as a second chunk; the query still agrees.
        e.flush().unwrap();
        assert_eq!(count("mvec"), 2);
        assert_eq!(count("emvb"), 2);
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["3".to_string()],
                vec!["2".to_string()]
            ]
        );

        // A tombstoned document is filtered out of the index hits.
        e.execute("DELETE FROM mv WHERE id = 3;").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    /// The sparse BMP index is built once, appended at write time, and dumped
    /// by `flush()`; queries read the chunk files instead of rebuilding.
    #[test]
    fn sparse_index_is_built_at_write_time() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sp (id INT, v SPARSE(FLOAT,100));")
            .unwrap();
        e.execute("INSERT INTO sp VALUES (1, [0:1.0,10:1.0]), (2, [0:2.0,20:2.0]);")
            .unwrap();
        let q = "SELECT id FROM sp SEARCH MATCH SPARSE \
                 (v, [0:1.0,20:2.0], 'ip', 3);";
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["1".to_string()]]
        );
        let idx_dir = std::fs::read_dir(dir.path().join("sparse"))
            .unwrap()
            .next()
            .unwrap()
            .unwrap()
            .path();
        let count = |ext: &str| {
            std::fs::read_dir(&idx_dir)
                .unwrap()
                .filter_map(|x| x.ok())
                .filter(|x| x.path().extension().is_some_and(|y| y == ext))
                .count()
        };
        assert_eq!(count("bmp"), 1);

        // The write-time tail is visible before the dump.
        e.execute("INSERT INTO sp VALUES (3, [0:4.0,20:4.0]);")
            .unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["3".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );

        // `flush()` seals the tail as a second chunk and the ranking holds.
        e.flush().unwrap();
        assert_eq!(count("bmp"), 2);
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["3".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );

        // Tombstoned rows never appear.
        e.execute("DELETE FROM sp WHERE id = 3;").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["1".to_string()]]
        );
    }

    /// A secondary index keeps only post-dump rows in memory; `flush()` seals
    /// the tail into a chunk and lookups merge chunks with the tail.
    #[test]
    fn secondary_index_seals_its_memory_tail() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sec (id INT, v INT);").unwrap();
        e.execute("INSERT INTO sec VALUES (1, 10), (2, 20);")
            .unwrap();
        e.execute("CREATE INDEX idx_v ON sec (v);").unwrap();
        let q = "SELECT id FROM sec WHERE v = 20;";
        assert_eq!(rows(&e, q), vec![vec!["2".to_string()]]);

        // New rows land in the memory tail and are visible immediately.
        e.execute("INSERT INTO sec VALUES (3, 20);").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert!(idx.key_count() > 0);
        drop(tables);

        // The dump drains the tail; the answer comes from the chunk + new tail.
        e.flush().unwrap();
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert_eq!(idx.key_count(), 0);
        drop(tables);
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT id FROM sec WHERE v = 10;"),
            vec![vec!["1".to_string()]]
        );

        // Tombstoned rows are filtered from both forms.
        e.execute("DELETE FROM sec WHERE id = 2;").unwrap();
        assert_eq!(rows(&e, q), vec![vec!["3".to_string()]]);
    }

    /// Boolean columns use the low-cardinality (roaring) chunks with the same
    /// tail/dump lifecycle.
    #[test]
    fn low_card_secondary_index_seals_its_memory_tail() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE b (id INT, flag BOOL);").unwrap();
        e.execute("INSERT INTO b VALUES (1, true), (2, false);")
            .unwrap();
        e.execute("CREATE INDEX idx_flag ON b (flag);").unwrap();
        let q = "SELECT id FROM b WHERE flag = true;";
        assert_eq!(rows(&e, q), vec![vec!["1".to_string()]]);
        e.execute("INSERT INTO b VALUES (3, true);").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        e.flush().unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        e.execute("DELETE FROM b WHERE id = 1;").unwrap();
        assert_eq!(rows(&e, q), vec![vec!["3".to_string()]]);
    }

    /// One quota governs every index form: when it is exceeded the tails are
    /// dumped and released, and the results stay correct.
    #[test]
    fn memindex_quota_seals_every_tail() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // Any positive budget is exceeded by the first row's tails.
        e.set_memindex_quota(1);
        e.execute("CREATE TABLE q (id INT, m MULTIVECTOR(FLOAT,2), s SPARSE(FLOAT,10), c INT);")
            .unwrap();
        e.execute("CREATE INDEX qi ON q (c);").unwrap();
        e.execute("CREATE INDEX qm ON q (m) USING EMVB;").unwrap();
        e.execute("CREATE INDEX qs ON q (s) USING BMP;").unwrap();
        e.execute(
            "INSERT INTO q VALUES (1, [[1.0,0.0]], [0:1.0, 1:2.0], 7), \
             (2, [[0.0,1.0]], [1:3.0], 7);",
        )
        .unwrap();

        let mv = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.mv.values().next().cloned())
            .unwrap();
        let mv_tail = mv.read().unwrap().tail_bytes;
        assert_eq!(mv_tail, 0);
        let sp = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.sparse.values().next().cloned())
            .unwrap();
        let sp_tail = sp.read().unwrap().tail_bytes;
        assert_eq!(sp_tail, 0);
        let tables = e.tables.lock().unwrap();
        let sec = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert_eq!(sec.key_count(), 0);
        drop(tables);

        // The sealed forms answer every path.
        assert_eq!(
            rows(&e, "SELECT id FROM q WHERE c = 7;"),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM q SEARCH MATCH SPARSE (s, [1:1.0], 'ip', 2);"
            ),
            vec![vec!["2".to_string()], vec!["1".to_string()]]
        );
        assert!(!rows(
            &e,
            "SELECT id FROM q SEARCH MATCH TENSOR (m, [1.0,0.0], 'float', 'maxsim', 'topn=2');"
        )
        .is_empty());
    }

    /// Under `rocksdb-store` the catalog is the C++ key space only: no native
    /// `tbl:`/`db:`/`idx:` record may be written (plan #69).
    #[cfg(feature = "rocksdb-store")]
    #[test]
    fn cpp_only_catalog_writes_no_native_keys() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (a INT, b VARCHAR);").unwrap();
        e.execute("CREATE INDEX t_a ON t (a);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'x');").unwrap();
        let store = e.catalog().store();
        for prefix in ["tbl:", "tblname:", "db:", "idx:"] {
            assert!(
                store.scan_prefix(prefix.as_bytes()).unwrap().is_empty(),
                "native key prefix {prefix} survived"
            );
        }
        assert!(!store.scan_prefix(b"catalog|tbl|").unwrap().is_empty());
        assert!(!store.scan_prefix(b"catalog|idx|").unwrap().is_empty());
        // The same directory reopens with the schema restored.
        e.flush().unwrap();
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(rows(&e, "SELECT a FROM t;"), vec![vec!["1".to_string()]]);
    }

    /// WAND now covers filters, `minimum_should_match`, `delta_term` and
    /// `similarity=boolean`; each must agree with the reference semantics.
    #[test]
    fn wand_covers_boolean_filters_min_should_and_delta() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ftw (id INT, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO ftw VALUES (1,'alpha beta'),(2,'alpha gamma'),\
             (3,'beta gamma'),(4,'alpha beta gamma');",
        )
        .unwrap();
        e.execute("CREATE INDEX ftw_i ON ftw(body) USING FULLTEXT;")
            .unwrap();

        // Boolean similarity: one point per matched term.
        assert_eq!(
            rows(
                &e,
                "SELECT id, ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
                 ('body','alpha beta','topn=10;similarity=boolean');"
            ),
            vec![
                vec!["1".to_string(), "2.000000".to_string()],
                vec!["4".to_string(), "2.000000".to_string()],
                vec!["2".to_string(), "1.000000".to_string()],
                vec!["3".to_string(), "1.000000".to_string()],
            ]
        );

        // `minimum_should_match=2` keeps only documents with both terms.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM ftw SEARCH MATCH TEXT \
                 ('body','alpha beta','topn=10;minimum_should_match=2');"
            ),
            vec![vec!["1".to_string()], vec!["4".to_string()]]
        );

        // A filter is applied before ranking; the surviving scores match the
        // unfiltered query.
        let all = rows(
            &e,
            "SELECT id, ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
             ('body','alpha beta','topn=10');",
        );
        let filtered = rows(
            &e,
            "SELECT id, ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
             ('body','alpha beta','topn=10;filter=id <= 2');",
        );
        assert_eq!(filtered.len(), 2);
        for row in &filtered {
            assert!(all.contains(row), "{row:?} not in {all:?}");
        }

        // `delta_term` adds a constant to the tf norm:
        // df=3, N=4 → idf=ln(1+1.5/3.5), len=2, avg=2.25,
        // norm=2.2/(1+1.2*(0.25+0.75*2/2.25))=1.047619…
        let idf = (1.0f64 + 1.5 / 3.5).ln();
        let norm = 2.2 / (1.0 + 1.2 * (0.25 + 0.75 * 2.0 / 2.25));
        let expected = 2.0 * idf * (norm + 1.0);
        let got = rows(
            &e,
            "SELECT ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
             ('body','alpha beta','topn=1;bm25_param_delta_term=1');",
        );
        assert_eq!(got.len(), 1);
        let value: f64 = got[0][0].parse().unwrap();
        assert!((value - expected).abs() < 1e-4, "{value} vs {expected}");
    }

    /// IVF chunks written by the mirror are loaded and probed for candidates.
    #[test]
    fn ivf_index_serves_vector_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE iv (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute(
            "INSERT INTO iv VALUES (1,[0.0,0.0]),(2,[1.0,0.0]),\
             (3,[0.0,1.0]),(4,[2.0,2.0]);",
        )
        .unwrap();
        e.execute("CREATE INDEX ivx ON iv (v) USING IVF WITH (metric = l2);")
            .unwrap();
        // Rows written before the dump live in the IVF memory segment and are
        // merged with the sealed chunks.
        e.execute("INSERT INTO iv VALUES (5,[0.9,0.1]);").unwrap();
        let tables = e.tables.lock().unwrap();
        let entry = tables.get("default_db.iv").unwrap();
        assert!(!entry.ivf_mem.get("v").unwrap().is_empty());
        drop(tables);
        let q = "SELECT id FROM iv SEARCH MATCH VECTOR \
                 (v, [0.9,0.1], 'float', 'l2', 2) WITH (nprobe = 16);";
        assert_eq!(
            rows(&e, q),
            vec![vec!["5".to_string()], vec!["2".to_string()]]
        );
        e.flush().unwrap();
        let tables = e.tables.lock().unwrap();
        let entry = tables.get("default_db.iv").unwrap();
        assert!(entry.ivf_mem.get("v").unwrap().is_empty());
        assert!(entry.ivf.values().any(|segments| !segments.is_empty()));
        drop(tables);
        assert_eq!(
            rows(&e, q),
            vec![vec!["5".to_string()], vec!["2".to_string()]]
        );
    }

    /// A sealed multi-vector chunk ranks with its persisted residual codes
    /// (EMVB stage 2) before the exact MaxSim re-score.
    #[test]
    fn multivector_prefilter_uses_the_persisted_residual_codes() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mvp (id INT, v MULTIVECTOR(FLOAT,2));")
            .unwrap();
        e.execute("CREATE INDEX mvp_i ON mvp (v) USING EMVB;")
            .unwrap();
        let mut sql = String::from("INSERT INTO mvp VALUES ");
        for i in 0..300 {
            sql.push_str(&format!("({i}, [[{i}.0, 0.0]])"));
            if i < 299 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        e.flush().unwrap();
        let mv = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.mv.values().next().cloned())
            .unwrap();
        let has_codes = {
            let idx = mv.read().unwrap();
            idx.sealed
                .first()
                .map(|(_, _, pq)| pq.is_some())
                .unwrap_or(false)
        };
        assert!(has_codes, "the sealed chunk carries residual codes");
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM mvp SEARCH MATCH TENSOR \
                 (v, [299.0, 0.0], 'float', 'maxsim', 'topn=1');"
            ),
            vec![vec!["299".to_string()]]
        );
    }

    /// Multi-field WAND: each field keeps its own BM25 statistics and the
    /// scores sum, matching the reference's per-field `BM25Ranker`.
    #[test]
    fn wand_scores_multiple_fields_separately() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ftm (id INT, title VARCHAR, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO ftm VALUES (1,'alpha','beta'),(2,'alpha alpha',''),\
             (3,'beta','alpha');",
        )
        .unwrap();
        e.execute("CREATE INDEX ftm_t ON ftm(title) USING FULLTEXT;")
            .unwrap();
        e.execute("CREATE INDEX ftm_b ON ftm(body) USING FULLTEXT;")
            .unwrap();
        let got = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM ftm SEARCH MATCH TEXT \
             ('title^2,body','alpha beta','topn=10');",
        );
        let expected = [(3, 2.9990f64), (1, 1.8614), (2, 1.1332)];
        assert_eq!(got.len(), expected.len(), "{got:?}");
        for (row, (id, score)) in got.iter().zip(expected) {
            assert_eq!(row[0], id.to_string());
            let value: f64 = row[1].parse().unwrap();
            assert!((value - score).abs() < 1e-3, "{row:?} vs {score}");
        }
        // Boolean similarity sums the per-field points.
        assert_eq!(
            rows(
                &e,
                "SELECT id, ROUND(SCORE(),4) FROM ftm SEARCH MATCH TEXT \
                 ('title^2,body','alpha beta','topn=10;similarity=boolean');"
            ),
            vec![
                vec!["1".to_string(), "3.000000".to_string()],
                vec!["3".to_string(), "3.000000".to_string()],
                vec!["2".to_string(), "2.000000".to_string()],
            ]
        );
    }

    /// `SELECT id, name` reads only those column files from the C++ block
    /// mirror, and only while the mirror matches the row store.
    #[test]
    fn projection_reads_columns_from_the_block_mirror() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE p (id INT, name VARCHAR, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute(
            "INSERT INTO p VALUES (1,'a',[0.0,0.0]),(2,'b',[1.0,0.0]),\
             (3,'c',[0.0,1.0]);",
        )
        .unwrap();
        e.flush().unwrap();
        assert_eq!(e.cpp_block_row_count("p").unwrap(), 3);
        let items = [
            SelectItem::Column("id".to_string()),
            SelectItem::Column("name".to_string()),
        ];
        let projected = e.try_cpp_projection("p", &items).unwrap();
        assert!(projected.is_some());
        assert_eq!(
            rows(&e, "SELECT id, name FROM p;"),
            vec![
                vec!["1".to_string(), "a".to_string()],
                vec!["2".to_string(), "b".to_string()],
                vec!["3".to_string(), "c".to_string()],
            ]
        );
        // A row written after the flush is not in the block snapshot: the
        // projection overlays it from the row journal, so the new row is
        // visible without waiting for the next checkpoint.
        e.execute("INSERT INTO p VALUES (4,'d',[1.0,1.0]);")
            .unwrap();
        let projected = e.try_cpp_projection("p", &items).unwrap().unwrap();
        assert_eq!(projected.len(), 4);
        assert_eq!(projected[3][0], Value::Int(4));
        assert_eq!(projected[3][1], Value::Str("d".into()));
        assert_eq!(rows(&e, "SELECT id, name FROM p;").len(), 4);
    }

    /// Wide vector-like cells are kept in an append-only sidecar; the KV row
    /// holds only a short reference, and reads (including after restart)
    /// reconstruct the original value.
    #[test]
    fn wide_vectors_live_in_the_row_sidecar() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sb (id INT, v EMBEDDING(FLOAT,64), s SPARSE(FLOAT,64));")
            .unwrap();
        let vector = (0..64)
            .map(|i| format!("{i}.0"))
            .collect::<Vec<_>>()
            .join(",");
        let sparse = (0..20)
            .map(|i| format!("{i}:1.0"))
            .collect::<Vec<_>>()
            .join(",");
        e.execute(&format!(
            "INSERT INTO sb VALUES (1, [{vector}], [{sparse}]);"
        ))
        .unwrap();
        let raw = e
            .store()
            .get(&row_key("default_db.sb", 0))
            .unwrap()
            .unwrap();
        assert!(raw.len() < 200, "row is {} bytes", raw.len());
        assert!(
            raw.contains(&1u8),
            "row carries a blob reference: {:?}",
            String::from_utf8_lossy(&raw)
        );
        let blob = dir.path().join("rowdata").join("default_db_sb.bin");
        assert!(std::fs::metadata(&blob).unwrap().len() > 0);

        let vector = rows(&e, "SELECT v FROM sb;");
        assert_eq!(vector.len(), 1);
        assert!(vector[0][0].starts_with("[0"), "{:?}", vector[0]);
        assert!(vector[0][0].contains("63"));

        e.flush().unwrap();
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        let vector = rows(&e, "SELECT v FROM sb;");
        assert!(vector[0][0].starts_with("[0"), "{:?}", vector[0]);
        assert_eq!(rows(&e, "SELECT id FROM sb;"), vec![vec!["1".to_string()]]);
    }

    /// VARCHAR secondary indexes seal hashed keys into PGM chunks and keep only
    /// the tail in memory, like the reference's `ConvertToOrderedKeyValue`.
    #[test]
    fn string_secondary_index_seals_hashed_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ss (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO ss VALUES (1,'a'),(2,'b');").unwrap();
        e.execute("CREATE INDEX ss_n ON ss (name);").unwrap();
        let q = "SELECT id FROM ss WHERE name = 'b';";
        assert_eq!(rows(&e, q), vec![vec!["2".to_string()]]);
        e.execute("INSERT INTO ss VALUES (3,'b');").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert!(idx.key_count() > 0);
        drop(tables);
        e.flush().unwrap();
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert_eq!(idx.key_count(), 0);
        drop(tables);
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        e.execute("DELETE FROM ss WHERE id = 2;").unwrap();
        assert_eq!(rows(&e, q), vec![vec!["3".to_string()]]);
    }

    /// Port of the reference's `ConflictCheckTest.conflict_check_delete`: three
    /// transactions delete the same row, the first commit wins and the other
    /// two get `kTxnConflict`; overlapping sets behave the same way.
    #[test]
    fn delete_conflicts_follow_the_reference() {
        fn stale_delete(e: &Engine, table: &str, seq: usize) -> Txn {
            let mut txn = e.txn.begin();
            txn.delete(del_key(table, seq));
            txn
        }
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE dc (v INT);").unwrap();
        e.execute("INSERT INTO dc VALUES (0),(1),(2),(3),(4),(5),(6);")
            .unwrap();
        let table = "default_db.dc";
        let seqs: Vec<usize> = e
            .fetch_rows_with_ids(table)
            .unwrap()
            .iter()
            .map(|(seq, _)| *seq)
            .collect();

        let t1 = stale_delete(&e, table, seqs[0]);
        let t2 = stale_delete(&e, table, seqs[0]);
        let t3 = stale_delete(&e, table, seqs[0]);
        e.commit_with_deletes(table, &[seqs[0]], t1).unwrap();
        assert!(matches!(
            e.commit_with_deletes(table, &[seqs[0]], t2),
            Err(Error::TxnConflict(_))
        ));
        assert!(matches!(
            e.commit_with_deletes(table, &[seqs[0]], t3),
            Err(Error::TxnConflict(_))
        ));
        assert_eq!(e.row_count("dc").unwrap(), 6);

        // A transaction that starts after the winning commit is not stale.
        let t4 = stale_delete(&e, table, seqs[1]);
        e.commit_with_deletes(table, &[seqs[1]], t4).unwrap();
        assert_eq!(e.row_count("dc").unwrap(), 5);

        // Overlapping delete sets: the later committer conflicts on the shared
        // row even though it has its own disjoint write.
        let mut a = stale_delete(&e, table, seqs[2]);
        a.delete(del_key(table, seqs[3]));
        let b = stale_delete(&e, table, seqs[2]);
        e.commit_with_deletes(table, &[seqs[2], seqs[3]], a)
            .unwrap();
        assert!(matches!(
            e.commit_with_deletes(table, &[seqs[2]], b),
            Err(Error::TxnConflict(_))
        ));
        assert_eq!(e.row_count("dc").unwrap(), 3);
    }

    #[test]
    fn defaults_and_alter_add_column() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (a INT, b INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 2);").unwrap();
        e.execute("ALTER TABLE t ADD COLUMN (c VARCHAR DEFAULT 'default');")
            .unwrap();
        e.execute("ALTER TABLE t ADD COLUMN (d INT);").unwrap();
        // duplicate column must be rejected
        assert!(e.execute("ALTER TABLE t ADD COLUMN (b INT);").is_err());
        // explicit column list leaves c/d at their defaults
        e.execute("INSERT INTO t(a, b) VALUES (2, 2);").unwrap();
        e.execute("INSERT INTO t VALUES (3, 2, 'hello', 4);")
            .unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, columns } => {
                assert_eq!(columns.len(), 4);
                assert_eq!(rows[0], vec!["1", "2", "default", "null"]);
                assert_eq!(rows[1], vec!["2", "2", "default", "null"]);
                assert_eq!(rows[2], vec!["3", "2", "hello", "4"]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn f16_round_trip_is_nearest_even() {
        // Exact halves round to even; ordinary values round to the nearest f16.
        assert_eq!(round_f16(1.0), 1.0);
        assert_eq!(round_f16(0.5), 0.5);
        // 0.1 is not representable in f16; the nearest value is 0.0999755859375.
        assert!((round_f16(0.1) - 0.099_975_586).abs() < 1e-9);
        // 65504 is the largest finite f16; beyond it rounds to infinity.
        assert_eq!(round_f16(65504.0), 65504.0);
        assert!(round_f16(1.0e30).is_infinite());
    }

    #[test]
    fn bf16_truncates_low_bits() {
        // bfloat16 keeps the top 16 bits of an f32 by truncation.
        assert_eq!(round_bf16(1.0).to_bits(), 0x3f80_0000);
        assert_eq!(round_bf16(0.1).to_bits(), 0.1f32.to_bits() & 0xffff_0000);
        // The truncated value is slightly below the rounded-to-nearest value.
        assert!(round_bf16(0.1) < 0.1f32);
    }

    #[test]
    fn tensor_maxsim_quantizes_by_element_type() {
        // Two query vectors against two doc vectors; MaxSim sums the per-query
        // row maxima. A float16 doc rounds each stored value before the dot.
        let q = vec![vec![0.0, -10.0, 0.0, 0.7], vec![9.2, 45.6, -55.8, 3.5]];
        let d = vec![vec![0.1, 0.2, 0.3, -0.2], vec![0.3, -0.2, 0.2, 0.3]];
        let f32_score = tensor_maxsim(&q, &d, ElemType::Float32, ElemType::Float32);
        assert!((f32_score - (-5.19)).abs() < 1e-6, "got {f32_score}");
        let f16_score = tensor_maxsim(&q, &d, ElemType::Float16, ElemType::Float32);
        assert!((f16_score - (-5.1955)).abs() < 1e-3, "got {f16_score}");
    }

    #[test]
    fn bit_tensor_scores_over_set_bits() {
        // `bit` columns contribute the query component where the doc bit is set.
        let q = vec![vec![0.0, -10.0, 0.0, 0.7]];
        let d = vec![vec![1.0, 0.0, 0.0, 0.0]];
        let s = tensor_maxsim(&q, &d, ElemType::Bit, ElemType::Float32);
        assert!((s - 0.0).abs() < 1e-9, "got {s}");
        let d2 = vec![vec![0.0, 1.0, 0.0, 0.0]];
        let s2 = tensor_maxsim(&q, &d2, ElemType::Bit, ElemType::Float32);
        assert!((s2 - (-10.0)).abs() < 1e-9, "got {s2}");
    }

    #[test]
    fn l2_distance_is_squared() {
        // `l2` reports the squared Euclidean distance (integer and float alike).
        let (_, d) = vector_score(&[0.0, 0.0, 0.0], &[2.0, 3.0, 4.0], "l2");
        assert!((d - 29.0).abs() < 1e-9, "got {d}");
        // ip is a plain dot product.
        let (s, _) = vector_score(&[3.0, 2.0, 1.0], &[10.0, 11.0, 12.0], "ip");
        assert!((s - 64.0).abs() < 1e-9, "got {s}");
        // hamming counts differing positions.
        let (_, h) = vector_score(&[1.0, 0.0, 1.0], &[1.0, 1.0, 1.0], "hamming");
        assert!((h - 1.0).abs() < 1e-9, "got {h}");
    }

    #[test]
    fn element_type_detection_is_space_insensitive() {
        assert_eq!(elem_type_of("UNSIGNED TINYINT"), ElemType::UInt8);
        assert_eq!(elem_type_of("UNSIGNEDTINYINT"), ElemType::UInt8);
        assert_eq!(
            tensor_elem_type("EMBEDDING(UNSIGNEDTINYINT,3)"),
            ElemType::UInt8
        );
        assert_eq!(tensor_elem_type("TENSOR(FLOAT16, 4)"), ElemType::Float16);
    }

    #[test]
    fn databases_are_namespaced() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE DATABASE db1;").unwrap();
        // a duplicate without IF NOT EXISTS is an error
        assert!(e.execute("CREATE DATABASE db1;").is_err());
        e.execute("CREATE DATABASE IF NOT EXISTS db1;").unwrap();
        e.execute("USE db1;").unwrap();
        e.execute("CREATE TABLE t (a INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1);").unwrap();
        e.execute("USE default_db;").unwrap();
        e.execute("CREATE TABLE t (a INT);").unwrap();
        e.execute("INSERT INTO t VALUES (2);").unwrap();
        let rows = |sql: &str| match &e.execute(sql).unwrap()[0] {
            QueryResult::Rows { rows, .. } => rows.clone(),
            other => panic!("{other:?}"),
        };
        assert_eq!(rows("SELECT * FROM t;"), vec![vec!["2".to_string()]]);
        assert_eq!(rows("SELECT * FROM db1.t;"), vec![vec!["1".to_string()]]);
        // dropping the database removes its tables
        e.execute("DROP DATABASE db1;").unwrap();
        assert!(e.execute("SELECT * FROM db1.t;").is_err());
        assert_eq!(rows("SELECT * FROM t;"), vec![vec!["2".to_string()]]);
    }

    #[test]
    fn tensor_dimension_validation() {
        let t = "TENSOR(FLOAT,16)";
        // a flat literal must be a whole multiple of the dimension
        assert!(validate_tensor_dimension(t, "[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]").is_ok());
        assert!(validate_tensor_dimension(t, "[1,2,3]").is_err());
        // nested literals must have every embedding of exactly `dim`
        assert!(validate_tensor_dimension(
            t,
            "[[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],[2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]]"
        )
        .is_ok());
        // a TENSORARRAY must be triple-nested and every leaf the declared size
        let ta = "TENSORARRAY(BIT,8)";
        assert!(
            validate_tensor_dimension(ta, "[[[0,1,2,3,0,5,6,7],[8,9,0,11,12,0,14,15]]]").is_ok()
        );
        assert!(
            validate_tensor_dimension(ta, "[[[1,2,3,4,5,6,7,8],[9,10,11,12,13,14,15]]]").is_err()
        );
        // scalars / NULL padding are not dimension-checked
        assert!(validate_tensor_dimension(t, "null").is_ok());
        assert!(validate_tensor_dimension("VARCHAR", "'no tensor'").is_ok());
    }

    #[test]
    fn describe_type_names() {
        assert_eq!(describe_type("INT"), "Integer");
        assert_eq!(describe_type("ARRAY(VARCHAR)"), "Array(Varchar)");
        assert_eq!(
            describe_type("EMBEDDING(FLOAT,1024)"),
            "Embedding(float,1024)"
        );
        assert_eq!(
            describe_type("ARRAY(EMBEDDING(FLOAT,1024))"),
            "Array(Embedding(float,1024))"
        );
        assert_eq!(
            describe_type("SPARSE(INT,30000)"),
            "Sparse(int32,int16,30000)"
        );
        assert_eq!(
            describe_type("SPARSE(FLOAT,10000000000)"),
            "Sparse(float,int64,10000000000)"
        );
    }

    #[test]
    fn column_default_rendering() {
        assert_eq!(render_column_default(None, "INT"), "Null");
        assert_eq!(render_column_default(Some(&"0".to_string()), "INT"), "0");
        // an empty sparse default prints as `(empty)`
        assert_eq!(
            render_column_default(Some(&"[]".to_string()), "SPARSE(FLOAT,30000)"),
            "(empty)"
        );
        // the C++ renderer prints the value before the index for float sparses
        assert_eq!(
            render_column_default(Some(&"[0:0.0]".to_string()), "SPARSE(FLOAT,30000)"),
            "[0.000000:0]"
        );
        assert_eq!(
            render_column_default(Some(&"[0]".to_string()), "SPARSE(BIT,30000)"),
            "[0]"
        );
    }

    /// Invariant G5 (full-text): the index analyzer emits the word **and** its
    /// stem, while query analysis emits the stem only. Measured against the C++
    /// engine (doc_len 2,4,6,5,4 and doc_len_sum 21 on the five-row probe).
    #[test]
    fn analyzer_emits_word_and_stem_only_on_the_index_side() {
        let none: Option<Vec<char>> = None;
        assert_eq!(
            tokenize_index_text("second text multiple", &none),
            vec!["second", "text", "multiple", "multipl"]
        );
        assert_eq!(
            tokenize_text("second text multiple", &none),
            vec!["second", "text", "multipl"]
        );
    }

    /// Invariant G5: the standard tokenizer is the C++ byte table — `/` unites
    /// (`2012/02/14` -> `20120214`), while `_`, `.` and `,` split.
    #[test]
    fn standard_tokenizer_matches_the_cpp_byte_table() {
        assert_eq!(standard_tokenize("2012/02/14"), vec!["20120214"]);
        assert_eq!(standard_tokenize("x/y"), vec!["xy"]);
        assert_eq!(
            standard_tokenize("www.example.com/path"),
            vec!["www", "example", "compath"]
        );
        assert_eq!(standard_tokenize("a_b"), vec!["a", "b"]);
        assert_eq!(standard_tokenize("1,234"), vec!["1", "234"]);
        assert_eq!(standard_tokenize("a@b#c$d"), vec!["a", "b", "c", "d"]);
    }

    /// Invariant G5: a highlighted cell keeps only the sentences with a hit and
    /// joins them with `" ... "`.
    #[test]
    fn highlight_keeps_hit_sentences_and_marks_terms() {
        let none: Option<Vec<char>> = None;
        let q = tokenize_index_text("anarchism", &none);
        let text = "Anarchism is a philosophy. Cats sleep. Anarchism spread.";
        assert_eq!(
            highlight_text(text, &q, &none),
            "<em>Anarchism</em> is a philosophy.  ... <em>Anarchism</em> spread."
        );
    }

    /// A column declared `DEFAULT ''` keeps the empty string, not NULL.
    #[test]
    fn empty_string_default_survives_a_catalog_round_trip() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (num INTEGER, c VARCHAR DEFAULT '')")
            .unwrap();
        e.execute("INSERT INTO t (num) VALUES (1)").unwrap();
        let rows = rows(&e, "SELECT * FROM t");
        assert_eq!(rows, vec![vec!["1".to_string(), String::new()]]);
    }

    #[test]
    fn memory_indexer_records_tf_positions_and_length() {
        // C++ `column_inverter_ut.cpp`: tf, positions and the document length
        // come out of one insertion.
        let delims = Some(vec![' ']);
        let mut mem = MemoryIndexer::new();
        assert!(mem.insert(10, "alpha beta alpha", &delims, usize::MAX));
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.docs[&10], 3);
        let alpha = &mem.postings["alpha"];
        assert_eq!(alpha.len(), 1);
        assert_eq!(alpha[0].0, 10);
        assert_eq!(alpha[0].1, 2);
        assert_eq!(alpha[0].2, vec![0, 2]);

        // Re-indexing a row replaces its length, not the document count.
        assert!(mem.insert(10, "gamma", &delims, usize::MAX));
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.docs[&10], 1);
        assert_eq!(mem.total_len, 1);

        // A full segment refuses the next document without changing itself.
        let before = mem.mem_used();
        assert!(!mem.insert(11, "delta", &delims, before));
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.mem_used(), before);
    }

    /// C++: `function/scalar/{add,div,equals}_functions_ut.cpp`: checked
    /// arithmetic (overflow -> NULL), division by zero -> NULL, and equality.
    #[test]
    fn arithmetic_and_equality_follow_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ar (a TINYINT, b TINYINT);")
            .unwrap();
        e.execute("INSERT INTO ar VALUES (50, 50), (100, 100), (-128, -1), (5, 2), (0, 0);")
            .unwrap();

        // `+` on TINYINT (values within range; the i64-wide evaluator does not
        // model per-width checked overflow — recorded in CLASS_MAP).
        assert_eq!(
            rows(&e, "SELECT a + b FROM ar WHERE a = 50;"),
            vec![vec!["100".to_string()]]
        );
        // `=` compares values.
        assert_eq!(
            rows(&e, "SELECT a = b FROM ar WHERE a = 50;"),
            vec![vec!["true".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT a = b FROM ar WHERE a = 5;"),
            vec![vec!["false".to_string()]]
        );
        // The comparison family.
        for (sql, want) in [
            ("SELECT a > b FROM ar WHERE a = 5;", "true"),
            ("SELECT a >= b FROM ar WHERE a = 5;", "true"),
            ("SELECT a < b FROM ar WHERE a = 5;", "false"),
            ("SELECT a <= b FROM ar WHERE a = 5;", "false"),
            ("SELECT a != b FROM ar WHERE a = 5;", "true"),
            ("SELECT a != b FROM ar WHERE a = 50;", "false"),
            ("SELECT a >= b FROM ar WHERE a = 50;", "true"),
            ("SELECT a <= b FROM ar WHERE a = 50;", "true"),
        ] {
            assert_eq!(rows(&e, sql), vec![vec![want.to_string()]], "{sql}");
        }
        // `/` always returns DOUBLE; 0/0 is NULL.
        assert_eq!(
            rows(&e, "SELECT a / b FROM ar WHERE a = 5;"),
            vec![vec!["2.500000".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM ar WHERE a = 0 AND a / b IS NULL;"),
            vec![vec!["1".to_string()]]
        );
    }

    /// C++: `function/scalar/abs_functions_ut.cpp::AbsFunctionsTest/abs_func`:
    /// `ABS` over TinyInt/SmallInt/Integer/BigInt/Float/Double, with NULL
    /// propagation.
    #[test]
    fn abs_function_covers_every_numeric_type() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute(
            "CREATE TABLE av (a TINYINT, b SMALLINT, c INTEGER, d BIGINT, f FLOAT, g DOUBLE);",
        )
        .unwrap();
        e.execute("INSERT INTO av VALUES (-5, -300, -70000, -5000000000, -1.5, -2.5);")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT abs(a), abs(b), abs(c), abs(d), abs(f), abs(g) FROM av;"
            ),
            vec![vec![
                "5".to_string(),
                "300".to_string(),
                "70000".to_string(),
                "5000000000".to_string(),
                "1.500000".to_string(),
                "2.500000".to_string(),
            ]]
        );
        // A NULL input yields a NULL output for every width.
        e.execute("INSERT INTO av VALUES (NULL, NULL, NULL, NULL, NULL, NULL);")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT count(*) FROM av WHERE abs(a) IS NULL AND abs(b) IS NULL AND abs(c) IS NULL AND abs(d) IS NULL AND abs(f) IS NULL AND abs(g) IS NULL;"
            ),
            vec![vec!["1".to_string()]]
        );
    }

    /// C++: `MemoryIndexerTest.Chunk` (`memory_indexer_ut.cpp:191`): the
    /// five FST paragraphs produce the reference postings.
    #[test]
    fn memory_indexer_builds_the_reference_postings() {
        let paragraphs = [
            "A finite-state transducer (FST) is a finite-state machine with two memory tapes, following the terminology for Turing machines: an input tape and an output tape. This contrasts with an ordinary finite-state automaton, which has a single tape. An FST is a type of finite-state automaton (FSA) that maps between two sets of symbols.[1] An FST is more general than an FSA. An FSA defines a formal language by defining a set of accepted strings, while an FST defines a relation between sets of strings.",
            "An FST will read a set of strings on the input tape and generates a set of relations on the output tape. An FST can be thought of as a translator or relater between strings in a set.",
            "In morphological parsing, an example would be inputting a string of letters into the FST, the FST would then output a string of morphemes.",
            "An automaton can be said to recognize a string if we view the content of its tape as input. In other words, the automaton computes a function that maps strings into the set {0,1}. Alternatively, we can say that an automaton generates strings, which means viewing its tape as an output tape. On this view, the automaton generates a formal language, which is a set of strings. The two views of automata are equivalent: the function that the automaton computes is precisely the indicator function of the set of strings it generates. The class of languages generated by finite automata is known as the class of regular languages.",
            "The two tapes of a transducer are typically viewed as an input tape and an output tape. On this view, a transducer is said to transduce (i.e., translate) the contents of its input tape to its output tape, by accepting a string on its input tape and generating another string on its output tape. It may do so nondeterministically and it may produce more than one output for each input string. A transducer may also produce no output for a given input string, in which case it is said to reject the input. In general, a transducer computes a relation between two formal languages.",
        ];
        let mut mem = MemoryIndexer::new();
        for (i, paragraph) in paragraphs.iter().enumerate() {
            assert!(mem.insert(i as u64, paragraph, &None, usize::MAX));
        }
        let expected: [(&str, &[(u64, u32)]); 3] = [
            ("fst", &[(0, 4), (1, 2), (2, 2)]),
            ("automaton", &[(0, 2), (3, 5)]),
            ("transducer", &[(0, 1), (4, 4)]),
        ];
        for (term, want) in expected {
            let got: Vec<(u64, u32)> = mem.postings[term]
                .iter()
                .map(|(doc, tf, _)| (*doc, *tf))
                .collect();
            assert_eq!(got, want, "term {term}");
        }
    }

    #[test]
    fn memory_indexer_delete_keeps_the_postings() {
        let delims = Some(vec![' ']);
        let mut mem = MemoryIndexer::new();
        assert!(mem.insert(7, "alpha", &delims, usize::MAX));
        mem.delete(7);
        assert!(!mem.live.contains(&7));
        // The reference keeps deleted documents until compaction.
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.postings["alpha"].len(), 1);
    }

    /// A checkpoint dumps only the memory segment; new rows form the next
    /// segment and every row is indexed exactly once (Gate C: rebuilding the
    /// whole graph per checkpoint caused the OOM).
    #[test]
    fn hnsw_checkpoint_dumps_only_the_memory_segment() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE hv (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX hv_i ON hv (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        let mut sql = String::from("INSERT INTO hv VALUES ");
        for i in 0..10 {
            sql.push_str(&format!("({i}, [{i}.0, 0.0, 0.0, 0.0])"));
            if i < 9 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        let indexed = |e: &Engine| {
            e.tables
                .lock()
                .unwrap()
                .values()
                .flat_map(|t| t.hnsw_mem.values())
                .map(|m| m.lock().map(|m| m.len()).unwrap_or(0))
                .sum::<usize>()
        };
        assert_eq!(indexed(&e), 10);

        e.flush().unwrap();
        assert!(
            e.tables.lock().unwrap().values().all(|t| t
                .hnsw_mem
                .values()
                .all(|m| m.lock().map(|m| m.is_empty()).unwrap_or(true))),
            "the checkpoint empties the HNSW memory segment"
        );

        e.execute("INSERT INTO hv VALUES (10, [10.0,0.0,0.0,0.0]), (11, [11.0,0.0,0.0,0.0]);")
            .unwrap();
        assert_eq!(indexed(&e), 2);

        e.flush().unwrap();
        let indexed: usize = e
            .load_vector_indexes("default_db.hv", "hv_i")
            .iter()
            .map(|seg| seg.labels.len())
            .sum();
        assert_eq!(indexed, 12, "every row is in exactly one dumped chunk");
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM hv SEARCH MATCH VECTOR (v, [11.0,0.0,0.0,0.0], 'float', 'l2', 1);"
            ),
            vec![vec!["11".to_string()]]
        );
    }

    /// C++ `AppendMemIndex`: after a write batch, an HNSW memory segment that
    /// reached `mem_index_capacity` rows is dumped without waiting for a
    /// checkpoint (the row bound that keeps a long import's memory flat).
    #[test]
    fn mem_index_capacity_dumps_the_hnsw_segment_after_the_batch() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(4);
        e.execute("CREATE TABLE hc (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX hc_i ON hc (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        let mut sql = String::from("INSERT INTO hc VALUES ");
        for i in 0..10 {
            sql.push_str(&format!("({i}, [{i}.0, 0.0, 0.0, 0.0])"));
            if i < 9 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        let indexed = |e: &Engine| {
            e.tables
                .lock()
                .unwrap()
                .values()
                .flat_map(|t| t.hnsw_mem.values())
                .map(|m| m.lock().map(|m| m.len()).unwrap_or(0))
                .sum::<usize>()
        };
        assert_eq!(indexed(&e), 0, "the batch reached the capacity and dumped");

        e.execute("INSERT INTO hc VALUES (10, [10.0,0.0,0.0,0.0]);")
            .unwrap();
        assert_eq!(indexed(&e), 1, "below the capacity the segment stays");
        let dumped: usize = e
            .load_vector_indexes("default_db.hc", "hc_i")
            .iter()
            .map(|seg| seg.labels.len())
            .sum();
        assert_eq!(dumped, 10);
    }

    /// C++ optimize/cleanup: incremental chunks are all needed until a merged
    /// chunk covers every live row; cleanup must not delete them early.
    #[test]
    fn cleanup_keeps_incremental_hnsw_chunks_until_a_merge_covers_the_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(2);
        e.execute("CREATE TABLE hk (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute("CREATE INDEX hk_i ON hk (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        e.execute("INSERT INTO hk VALUES (0,[0.0,0.0]),(1,[1.0,0.0]);")
            .unwrap();
        e.execute("INSERT INTO hk VALUES (2,[2.0,0.0]),(3,[3.0,0.0]);")
            .unwrap();
        let chunks = |e: &Engine| e.load_vector_indexes("default_db.hk", "hk_i");
        assert_eq!(chunks(&e).len(), 2, "one chunk per memory-segment dump");

        e.cleanup().unwrap();
        assert_eq!(chunks(&e).len(), 2, "no chunk covers the whole table yet");

        e.optimize_indexes().unwrap();
        e.cleanup().unwrap();
        let merged = chunks(&e);
        assert_eq!(merged.len(), 1, "the merged chunk supersedes the others");
        assert_eq!(merged[0].labels.len(), 4);
    }

    /// Scalar tables keep a decoded row cache (the reference's in-memory
    /// `DataBlock`s); array/vector tables do not, because their decoded form is
    /// not stable across the write and scan paths.
    /// Once the mirror covers every live row, full scans read the values from
    /// the column blocks: the row KV only supplies the row-id list. Poisoning
    /// the KV values proves the read path no longer depends on them.
    #[test]
    fn full_rows_come_from_the_block_mirror_when_current() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bm (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO bm VALUES (1,'a'),(2,'b');").unwrap();
        e.flush().unwrap();
        // The scalar row cache would answer first; drop it so the test proves
        // the block path.
        e.invalidate_row_cache("default_db.bm");
        for seq in 0..2 {
            e.store()
                .put(&row_key("default_db.bm", seq), b"999\t[9.0,9.0]")
                .unwrap();
        }
        let rows = e.fetch_rows("default_db.bm").unwrap();
        assert_eq!(rows.len(), 2);
        assert_eq!(rows[0][0], Value::Int(1));
        assert_eq!(rows[1][0], Value::Int(2));
    }

    #[test]
    fn block_mirror_covers_rows_after_deletes_and_updates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bmu (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO bmu VALUES (1,'a'),(2,'b'),(3,'c');")
            .unwrap();
        e.flush().unwrap();
        e.invalidate_row_cache("default_db.bmu");
        // Poison the journal: the mirror must still supply every row it
        // snapshotted, and only the rows written after the checkpoint may come
        // from the journal.
        for seq in 0..3 {
            e.store()
                .put(&row_key("default_db.bmu", seq), b"999\tzzz")
                .unwrap();
        }
        e.execute("DELETE FROM bmu WHERE id = 2;").unwrap();
        e.execute("UPDATE bmu SET name = 'u' WHERE id = 3;")
            .unwrap();
        let rows = e.try_cpp_rows("default_db.bmu").unwrap().unwrap();
        let ids: Vec<&Value> = rows.iter().map(|(_, r)| &r[0]).collect();
        let names: Vec<&Value> = rows.iter().map(|(_, r)| &r[1]).collect();
        assert_eq!(ids, vec![&Value::Int(1), &Value::Int(3)]);
        assert_eq!(
            names,
            vec![&Value::Str("a".into()), &Value::Str("u".into())]
        );
        assert_eq!(
            rows.iter().map(|(seq, _)| *seq).collect::<Vec<_>>(),
            vec![0, 3]
        );
    }

    #[test]
    fn scalar_tables_use_the_row_cache() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rc (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO rc VALUES (1,'a'),(2,'b');").unwrap();
        assert_eq!(rows(&e, "SELECT * FROM rc;").len(), 2);
        let key = e.table_key("rc");
        assert!(e
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|t| t.row_cache.as_ref())
            .is_some());
        assert_eq!(
            rows(&e, "SELECT id FROM rc WHERE id = 2;"),
            vec![vec!["2".to_string()]]
        );

        e.execute("CREATE TABLE av (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute("INSERT INTO av VALUES (1,[0.0,0.0]);").unwrap();
        assert_eq!(rows(&e, "SELECT * FROM av;").len(), 1);
        let key = e.table_key("av");
        assert!(e
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|t| t.row_cache.as_ref())
            .is_none());
    }

    /// Row-id allocation creates segments; `SHOW SEGMENTS` reports their ids
    /// and row counts.
    #[test]
    fn segments_track_row_id_allocations() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sg (id INT);").unwrap();
        e.reserve_rows("sg", 2, true).unwrap();
        e.reserve_rows("sg", 3, true).unwrap();
        assert_eq!(e.segments("sg"), vec![(0, 2), (1, 3)]);
        // An INSERT after a sealed COPY opens a segment; the next one appends.
        e.reserve_rows("sg", 4, false).unwrap();
        e.reserve_rows("sg", 4, false).unwrap();
        assert_eq!(e.segments("sg"), vec![(0, 2), (1, 3), (2, 8)]);
    }

    /// Writers and readers share the table/index state; this exercises the
    /// table-lock boundaries introduced for `TableEntry` (a deadlock hangs the
    /// test binary, which the harness timeout catches).
    #[test]
    fn concurrent_inserts_and_vector_searches_do_not_deadlock() {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE cc (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX cc_i ON cc (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        let mut handles = Vec::new();
        for t in 0..4 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for b in 0..25 {
                    e.execute(&format!(
                        "INSERT INTO cc VALUES ({}, [1.0,0.0,0.0,0.0]), ({}, [0.0,1.0,0.0,0.0]);",
                        t * 1000 + b * 2,
                        t * 1000 + b * 2 + 1
                    ))
                    .unwrap();
                    if b % 5 == 0 {
                        e.flush().unwrap();
                    }
                }
            }));
        }
        for _ in 0..2 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for _ in 0..40 {
                    let _ = e.execute(
                        "SELECT id FROM cc SEARCH MATCH VECTOR (v, [1.0,0.0,0.0,0.0], 'float', 'l2', 3);",
                    );
                    let _ = e.execute("SELECT count(*) FROM cc;");
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        assert_eq!(e.row_count("default_db.cc").unwrap(), 200);
    }

    /// `src/unit_test/main/infinity_ut.cpp::InfinityTest/test1`: the catalog
    /// lifecycle through the SQL path (`ListDatabases` / `CreateDatabase` /
    /// `DropDatabase` / `ShowTables` / insert + select / `Query`).
    #[test]
    fn catalog_lifecycle_matches_the_reference_test() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();

        // `ListDatabases` starts with `default_db` only.
        assert_eq!(
            rows(&e, "SHOW DATABASES;"),
            vec![vec![
                "default_db".to_string(),
                dir.path()
                    .join("data")
                    .join("db_1")
                    .to_string_lossy()
                    .into_owned(),
                String::new()
            ]]
        );

        e.execute("CREATE DATABASE db1;").unwrap();
        let dbs = rows(&e, "SHOW DATABASES;");
        // The reference lists `db1` first (hash order); the Rust catalog is
        // sorted, so compare the set.
        let mut names: Vec<&str> = dbs.iter().map(|r| r[0].as_str()).collect();
        names.sort_unstable();
        assert_eq!(names, vec!["db1", "default_db"]);
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "db1"));
        assert!(!e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "db2"));
        // Dropping a missing database with the default (`kError`) conflict type
        // fails, while `default_db` stays.
        assert!(e.catalog().drop_database("db2", false).is_err());
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));
        e.catalog().drop_database("db1", false).unwrap();
        assert_eq!(rows(&e, "SHOW DATABASES;").len(), 1);
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));

        // Create/Show/Get/Drop a table in `default_db`.
        e.execute("CREATE TABLE default_db.table1 (col1 BOOLEAN, col2 BIGINT);")
            .unwrap();
        let tables = rows(&e, "SHOW TABLES FROM default_db;");
        assert_eq!(tables.len(), 1);
        assert_eq!(tables[0][0], "default_db");
        assert_eq!(tables[0][1], "table1");
        assert_eq!(tables[0][3], "2");
        assert!(e
            .catalog()
            .get_table_by_name("default_db.table1")
            .unwrap()
            .is_some());
        e.execute("DROP TABLE default_db.table1;").unwrap();
        assert!(rows(&e, "SHOW TABLES FROM default_db;").is_empty());
        assert!(e
            .catalog()
            .get_table_by_name("default_db.table1")
            .unwrap()
            .is_none());

        // Insert and select through two columns of different widths.
        e.execute("CREATE TABLE default_db.table1 (col1 BIGINT, col2 SMALLINT);")
            .unwrap();
        e.execute("INSERT INTO default_db.table1 (col1, col2) VALUES (11, 22);")
            .unwrap();
        assert_eq!(
            rows(&e, "SELECT col1, col2 FROM default_db.table1;"),
            vec![vec!["11".to_string(), "22".to_string()]]
        );
        e.execute("DROP TABLE default_db.table1;").unwrap();

        // `Query("create database db1;")` + `Query("show databases;")`.
        e.execute("CREATE DATABASE db1;").unwrap();
        let mut names: Vec<String> = rows(&e, "SHOW DATABASES;")
            .into_iter()
            .map(|r| r[0].clone())
            .collect();
        names.sort();
        assert_eq!(names, vec!["db1", "default_db"]);
        e.execute("DROP DATABASE db1;").unwrap();
        assert_eq!(rows(&e, "SHOW DATABASES;").len(), 1);
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));
    }

    /// `src/unit_test/main/infinity_ut.cpp::InfinityTest/test2`: every listed
    /// global/session variable resolves, and `error` is rejected at both scopes
    /// (`Status::NoSysVar`).
    #[test]
    fn show_variable_covers_the_reference_names() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        for name in [
            "total_commit_count",
            "active_txn_count",
            "total_rollback_count",
            "buffer_object_count",
            "unused_buffer_object",
            "next_transaction_id",
            "schedule_policy",
            "active_wal_filename",
            "current_timestamp",
            "session_count",
            "query_count",
            "buffer_usage",
            "profile_record_capacity",
            "profile",
        ] {
            let r = e.show_variable(name, variables::SetScope::Global).unwrap();
            let QueryResult::Rows { columns, rows } = r else {
                panic!("{name} must return a row")
            };
            assert_eq!(columns, vec!["value".to_string()]);
            assert_eq!(rows.len(), 1);
        }
        for name in [
            "connected_timestamp",
            "total_rollback_count",
            "total_commit_count",
            "query_count",
        ] {
            e.show_variable(name, variables::SetScope::Session).unwrap();
        }
        assert!(e
            .show_variable("error", variables::SetScope::Global)
            .is_err());
        assert!(e
            .show_variable("error", variables::SetScope::Session)
            .is_err());
    }

    /// `src/unit_test/main/table_ut.cpp::InfinityTableTest/test1`: a table with
    /// an `EMBEDDING(FLOAT, 128)` and a `BIGINT` column explains in every plan
    /// mode without executing the inner statement.
    #[test]
    fn explain_covers_every_plan_kind() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE DATABASE db1;").unwrap();
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "db1"));
        e.execute("CREATE TABLE db1.knn_table (col1 EMBEDDING(FLOAT, 128), col2 BIGINT);")
            .unwrap();
        // The reference asks for AST, unoptimized/optimized logical, physical,
        // fragment and pipeline plans; each must succeed.
        for sql in [
            "EXPLAIN AST SELECT col2 FROM db1.knn_table;",
            "EXPLAIN LOGICAL SELECT col2 FROM db1.knn_table;",
            "EXPLAIN OPTIMIZED SELECT col2 FROM db1.knn_table;",
            "EXPLAIN PHYSICAL SELECT col2 FROM db1.knn_table;",
            "EXPLAIN FRAGMENT SELECT col2 FROM db1.knn_table;",
            "EXPLAIN PIPELINE SELECT col2 FROM db1.knn_table;",
        ] {
            let mut res = e.execute(sql).unwrap_or_else(|err| panic!("{sql}: {err}"));
            assert_eq!(res.len(), 1, "{sql}");
            assert!(matches!(res.remove(0), QueryResult::Rows { .. }), "{sql}");
        }
    }
}
