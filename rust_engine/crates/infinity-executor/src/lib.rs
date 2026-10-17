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
use infinity_txn::TxnManager;
use tokio::sync::mpsc;

mod explain;

mod explain_search;

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

/// The engine: catalog + transaction manager + KV store + a Tokio runtime.
pub struct Engine {
    catalog: Catalog,
    txn: TxnManager,
    store: Arc<dyn MetaStore>,
    rt: tokio::runtime::Runtime,
    exchange_capacity: usize,
    /// the engine's data directory (root of the C++-format block mirror)
    dir: PathBuf,
    /// secondary indexes keyed by `"<table>.<column>"`
    indexes: Mutex<HashMap<String, SecondaryIndex>>,
    /// Vector indexes loaded from the C++ block mirror, keyed by
    /// `"<table>.<column>"` (plan #70: the persisted HNSW graph, on disk).
    vector_indexes: Mutex<HashMap<String, Arc<crate::cpp_blocks::LoadedVectorIndex>>>,
    /// session/global configuration set via `SET CONFIG …`
    config: Mutex<HashMap<String, String>>,
    /// fulltext analyzer per `"<table>.<column>"`, from `CREATE INDEX … WITH (analyzer=…)`
    index_analyzers: Mutex<HashMap<String, String>>,
    /// `CREATE INDEX … COMMENT '…'`, keyed by `"<table>#<index>"`.
    index_comments: Mutex<HashMap<String, String>>,
    /// cached `SELECT` results keyed by the statement text
    result_cache: Mutex<HashMap<String, Vec<QueryResult>>>,
    /// the database new unqualified table names are resolved against
    current_db: Mutex<String>,
    /// number of write batches (`COPY`/`INSERT`) per table; a table with more
    /// than one batch is planned as several segments (a `MERGE` operator).
    write_batches: Mutex<HashMap<String, usize>>,
    /// Next free row id per table; reserved in blocks so concurrent inserts do
    /// not collide on the same base sequence.
    row_counters: Mutex<HashMap<String, (u32, u32, bool)>>,
}

fn index_key(table: &str, column: &str) -> String {
    format!("{table}.{column}")
}

impl Engine {
    pub fn open(dir: impl AsRef<Path>) -> Result<Self> {
        Self::open_with_capacity(dir, 4)
    }

    pub fn open_with_capacity(dir: impl AsRef<Path>, exchange_capacity: usize) -> Result<Self> {
        let dir: PathBuf = dir.as_ref().to_path_buf();
        // The catalog lives at `<dir>/catalog`, the path the C++ engine uses for
        // its catalog RocksDB. Under `rocksdb-store` the native catalog shares
        // that one RocksDB with the C++ key space (see `flush`), so a C++ engine
        // can open this directory's catalog; otherwise it is a fjall store.
        #[cfg(feature = "rocksdb-store")]
        let catalog = {
            let catalog_store: Arc<dyn MetaStore> =
                Arc::new(infinity_storage::RocksMetaStore::open(dir.join("catalog"))?);
            Catalog::with_store(catalog_store)
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
            catalog,
            txn,
            store,
            rt,
            exchange_capacity: exchange_capacity.max(1),
            dir,
            indexes: Mutex::new(HashMap::new()),
            vector_indexes: Mutex::new(HashMap::new()),
            config: Mutex::new(HashMap::new()),
            index_analyzers: Mutex::new(HashMap::new()),
            index_comments: Mutex::new(HashMap::new()),
            result_cache: Mutex::new(HashMap::new()),
            current_db: Mutex::new("default_db".to_string()),
            write_batches: Mutex::new(HashMap::new()),
            row_counters: Mutex::new(HashMap::new()),
        };
        engine.recover_cpp_blocks()?;
        engine.rebuild_indexes()?;
        // If this directory was written by the C++ engine (its catalog keys are
        // in the catalog store, but the native catalog is empty), take over its
        // schema so the engine can open a C++-produced directory (§2.6.1).
        if engine.catalog.list_tables()?.is_empty() {
            let _ = engine
                .catalog
                .import_cpp_layout(engine.catalog.store().as_ref());
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
    /// database's position in the sorted name list — deterministic, and equal
    /// to C++'s own counter for databases created in name order. Unqualified
    /// names resolve against `default_db`.
    fn database_id_of(&self, table: &str) -> u64 {
        let db = if table.contains('.') {
            table.split('.').next().unwrap_or("default_db")
        } else {
            "default_db"
        };
        self.catalog
            .list_databases()
            .unwrap_or_default()
            .iter()
            .position(|d| d == db)
            .unwrap_or(0) as u64
    }

    /// Rebuilds all declared indexes from stored rows (called on open).
    fn rebuild_indexes(&self) -> Result<()> {
        for im in self.catalog.list_indexes()? {
            // skip functional-index targets (not plain columns)
            if self
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
                if let Ok(loaded) = self.load_vector_index(&im.table, &im.name) {
                    self.vector_indexes
                        .lock()
                        .unwrap()
                        .insert(index_key(&im.table, &im.column), Arc::new(loaded));
                }
            }
        }
        Ok(())
    }

    fn build_index(&self, table: &str, column: &str) -> Result<()> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::Invalid(format!("unknown column '{column}'")))?;
        let mut idx = SecondaryIndex::new();
        // key by the *storage* sequence id (not the positional index: a rewrite
        // such as DROP COLUMN re-appends rows under new seqs)
        for (seq, row) in self.fetch_rows_with_ids(table)? {
            if let Some(v) = row.get(col_idx) {
                idx.insert(v.to_slt_string().as_bytes(), seq as u64);
            }
        }
        self.indexes
            .lock()
            .unwrap()
            .insert(index_key(table, column), idx);
        Ok(())
    }

    fn index_lookup(&self, table: &str, column: &str, value: &Literal) -> Option<Vec<u64>> {
        let idxs = self.indexes.lock().unwrap();
        let idx = idxs.get(&index_key(table, column))?;
        Some(idx.lookup(value.to_slt_string().as_bytes()).to_vec())
    }

    fn fetch_rows_by_ids(&self, table: &str, ids: &[u64]) -> Result<Vec<Vec<Value>>> {
        let mut rows = Vec::new();
        for &id in ids {
            if let Some(v) = self.store.get(&row_key(table, id as usize))? {
                rows.push(
                    String::from_utf8_lossy(&v)
                        .split('\t')
                        .map(parse_value)
                        .collect(),
                );
            }
        }
        Ok(rows)
    }

    pub fn execute(&self, sql: &str) -> Result<Vec<QueryResult>> {
        let mut out = Vec::new();
        for s in parse(sql)? {
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
                if self.result_cache.lock().unwrap().len() < self.result_cache_capacity() {
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
                self.catalog.flush()?;
                self.store.flush()?;
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

    /// Drops a table and purges its stored rows, tombstones, secondary-index
    /// entries and index declarations, so a later CREATE TABLE with the same
    /// name starts empty.
    fn drop_table_all(&self, name: &str) -> Result<bool> {
        let table_id = self.catalog.get_table_by_name(name)?.map(|m| m.id);
        self.row_counters
            .lock()
            .unwrap()
            .remove(name.split('.').next_back().unwrap_or(name));
        let existed = self.catalog.drop_table(name)?;
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
        for (k, _) in self.store.scan_prefix(&row_prefix(name))? {
            self.store.delete(&k)?;
        }
        for (k, _) in self.store.scan_prefix(&del_prefix(name))? {
            self.store.delete(&k)?;
        }
        let key_prefix = format!("{name}.");
        self.indexes
            .lock()
            .unwrap()
            .retain(|k, _| !k.starts_with(&key_prefix));
        for im in self.catalog.list_indexes()? {
            if im.table == name {
                let _ = self.catalog.drop_index(&im.name);
            }
        }
        Ok(existed)
    }

    fn table_schema(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .columns)
    }

    /// Reserves a contiguous block of `n` row ids for `table`.
    fn reserve_rows(&self, table: &str, n: usize, new_segment: bool) -> Result<usize> {
        let mut counters = self.row_counters.lock().unwrap();
        let key = table.split('.').next_back().unwrap_or(table).to_string();
        let (mut seg, mut off, sealed) = match counters.get(&key) {
            Some(v) => *v,
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
        counters.insert(key, (seg, off, new_segment));
        Ok(base)
    }

    /// Forgets any cached row-id counter for `table` (used after an operation
    /// that re-keys rows outside `reserve_rows`).
    fn reset_row_counter(&self, table: &str) {
        self.row_counters
            .lock()
            .unwrap()
            .remove(table.split('.').next_back().unwrap_or(table));
    }

    /// Records a write batch (`COPY`/`INSERT`) against `table`.
    fn bump_writes(&self, table: &str) {
        let key = table
            .split('.')
            .next_back()
            .unwrap_or(table)
            .to_ascii_lowercase();
        *self.write_batches.lock().unwrap().entry(key).or_insert(0) += 1;
    }

    /// True when `table` has been written in more than one batch.
    fn multi_segment(&self, table: &str) -> bool {
        let key = table
            .split('.')
            .next_back()
            .unwrap_or(table)
            .to_ascii_lowercase();
        self.write_batches
            .lock()
            .unwrap()
            .get(&key)
            .copied()
            .unwrap_or(0)
            >= 2
    }

    fn table_meta(&self, table: &str) -> Result<infinity_catalog::TableMeta> {
        self.catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))
    }

    /// `(column, index name)` pairs of `table`, for the EXPLAIN regex rewrite.
    /// The analyzer recorded for the index named `index_name` on `table`, i.e.
    /// the analyzer of the column that index covers.
    fn analyzer_of_index(&self, table: &str, index_name: &str) -> Option<String> {
        let short = table.split('.').next_back().unwrap_or(table);
        if let Some(a) = self
            .index_analyzers
            .lock()
            .unwrap()
            .get(&format!("{table}#{index_name}"))
        {
            return Some(a.clone());
        }
        let column = self
            .catalog
            .list_indexes()
            .ok()?
            .into_iter()
            .find(|im| im.name == index_name && (im.table == short || im.table == table))?
            .column;
        self.index_analyzers
            .lock()
            .unwrap()
            .get(&index_key(table, &column))
            .cloned()
    }

    fn index_name_pairs(&self, table: &str) -> Result<Vec<(String, String)>> {
        let short = table.split('.').next_back().unwrap_or(table);
        Ok(self
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|i| i.table == short || i.table == table)
            .map(|i| (i.column, i.name))
            .collect())
    }

    /// Columns of `table` that carry any index, for the `EXPLAIN` index/leftover
    /// filter split.
    fn indexed_columns(&self, table: &str) -> Result<Vec<String>> {
        let short = table.split('.').next_back().unwrap_or(table);
        Ok(self
            .catalog
            .list_indexes()?
            .into_iter()
            .filter(|i| i.table == short || i.table == table)
            .map(|i| i.column)
            .collect())
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
                        if parts.len() == 2 {
                            let dim: i64 = parts[1].parse().unwrap_or(0);
                            // The reference rejects a zero bound at DDL time, but
                            // the SDK's `sparse,0,…` column type (physical type
                            // dimension 0) is accepted.
                            let allow_zero = ALLOW_ZERO_SPARSE.with(|c| c.get());
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
                    .catalog
                    .create_table_typed(name, &names, &defaults, &not_nulls, &types, &comments)?;
                Ok(QueryResult::CreatedTable {
                    id: t.id,
                    name: t.name,
                })
            }
            Statement::RenameTable { table, new_name } => {
                let meta = self
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                if self.catalog.get_table_by_name(new_name)?.is_some() {
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
                for (k, _) in self.store.scan_prefix(&row_prefix(table))? {
                    self.store.delete(&k)?;
                }
                for (k, _) in self.store.scan_prefix(&del_prefix(table))? {
                    self.store.delete(&k)?;
                }
                // move the secondary-index entries and declarations
                let old_prefix = format!("{table}.");
                let pairs: Vec<(String, String)> = self
                    .indexes
                    .lock()
                    .unwrap()
                    .keys()
                    .filter(|k| k.starts_with(&old_prefix))
                    .map(|k| (k.clone(), k[old_prefix.len()..].to_string()))
                    .collect();
                let mut guard = self.indexes.lock().unwrap();
                for (old_key, column) in pairs {
                    if let Some(v) = guard.remove(&old_key) {
                        guard.insert(index_key(new_name, &column), v);
                    }
                }
                drop(guard);
                for im in self.catalog.list_indexes()? {
                    if im.table == *table {
                        let _ = self.catalog.drop_index(&im.name);
                        self.catalog.create_index_typed(
                            &im.name,
                            new_name,
                            &im.column,
                            &im.index_type,
                            &im.options,
                        )?;
                    }
                }
                self.catalog.rename_table(table, new_name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::AlterTableDropColumn { table, column } => {
                // Dropping a column that does not exist is `COLUMN_NOT_EXIST`.
                let schema = self.table_schema(table)?;
                if !schema.iter().any(|c| c == column) {
                    return Err(Error::NotFound(format!("column '{column}'")));
                }
                // an index still referencing the column blocks the drop
                if self
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
                self.catalog.drop_column(table, column)?;
                if let Some(im) = self
                    .catalog
                    .list_indexes()?
                    .into_iter()
                    .find(|i| i.table == *table && i.column == *column)
                {
                    let _ = self.catalog.drop_index(&im.name);
                }
                self.indexes
                    .lock()
                    .unwrap()
                    .remove(&index_key(table, column));
                // remaining indexes now hold stale row ids: rebuild them
                let remaining: Vec<String> = self
                    .catalog
                    .list_indexes()?
                    .into_iter()
                    .filter(|i| i.table == *table)
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
                self.catalog.add_columns_typed(
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
                self.catalog.create_database(name, *if_not_exists)?;
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
                let existed = self.catalog.drop_database(name, *if_exists)?;
                if existed || *if_exists {
                    // drop every table registered under this database
                    let prefix = format!("{name}.");
                    for t in self.catalog.list_tables()? {
                        if t.name.starts_with(&prefix) {
                            self.drop_table_all(&t.name)?;
                        }
                    }
                }
                Ok(QueryResult::DroppedDatabase { name: name.clone() })
            }
            Statement::Show { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::ShowTableIndex { table, index } => {
                if !self
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
                let logical = matches!(kind, infinity_parser::ExplainKind::Logical);
                let text = match inner.as_ref() {
                    Statement::Select {
                        items,
                        from: Some(t),
                        join: Some(j),
                        where_pred,
                        search,
                        ..
                    } if logical && search.is_none() => Some(crate::explain::explain_logical_join(
                        items,
                        t,
                        &j.table,
                        j,
                        where_pred.as_ref(),
                    )),
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
                                crate::explain::explain_index_scan(&crate::explain::IndexScanPlan {
                                    items,
                                    table: t,
                                    where_pred: w,
                                    order_by,
                                    limit: *limit,
                                    schema,
                                    types: &meta.types,
                                    indexed: &indexed,
                                    index_names: &index_names,
                                })
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
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == *name && im.table == *table);
                if duplicate && !*if_not_exists {
                    return Err(Error::Invalid(format!("index '{name}' already exists")));
                }
                self.catalog
                    .create_index_typed(name, table, column, index_type, opts)?;
                // remember the analyzer so fulltext matching can tokenise compatibly
                if let Some((_, a)) = opts.iter().find(|(k, _)| k == "ANALYZER") {
                    let a = a
                        .trim()
                        .trim_matches(|c| c == '\'' || c == '"')
                        .to_ascii_lowercase();
                    let mut analyzers = self.index_analyzers.lock().unwrap();
                    // By column, for the match path; and by index name, because two
                    // indexes on one column can have different analyzers.
                    analyzers.insert(index_key(table, column), a.clone());
                    analyzers.insert(format!("{table}#{name}"), a);
                }
                // a functional-index target is not a plain column, so there is
                // no materialised index to build
                if self.table_schema(table)?.iter().any(|c| c == column) {
                    self.build_index(table, column)?;
                }
                Ok(QueryResult::CreatedIndex {
                    name: name.clone(),
                    table: table.clone(),
                    column: column.clone(),
                })
            }
            Statement::DropTable { name, if_exists } => {
                let existed = self.drop_table_all(name)?;
                if !existed && !*if_exists {
                    return Err(Error::Invalid(format!("table '{name}' does not exist")));
                }
                Ok(QueryResult::DroppedTable {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::DropIndex { name, if_exists } => {
                let existed = self.catalog.drop_index(name)?;
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
                self.config
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
                    self.config
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
                self.store.put(&snap_key(name), blob.as_bytes())?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::DropSnapshot { name } => {
                if self.store.get(&snap_key(name))?.is_none() {
                    return Err(Error::NotFound(format!("snapshot '{name}'")));
                }
                self.store.delete(&snap_key(name))?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::RestoreSnapshot { name } => {
                let data = self
                    .store
                    .get(&snap_key(name))?
                    .ok_or_else(|| Error::NotFound(format!("snapshot '{name}'")))?;
                let blob = String::from_utf8_lossy(&data).into_owned();
                // Restoring into an existing table is refused, as in the
                // reference (`DROP TABLE` first).
                let table = blob.split(SNAP_REC).next().unwrap_or_default().to_string();
                if !table.is_empty() && self.catalog.get_table_by_name(&table)?.is_some() {
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
            } => {
                let meta = self
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                let schema = meta.columns.clone();
                // An explicit column list must name existing columns; the C++
                // engine reports this as a syntax error.
                if let Some(cols) = columns.as_deref() {
                    for c in cols {
                        if !schema.iter().any(|s| s == c) {
                            return Err(Error::Invalid(format!(
                                "syntax: column '{c}' does not exist in table '{table}'"
                            )));
                        }
                    }
                }
                let mut rows = align_insert_rows(&schema, &meta.defaults, columns.as_deref(), rows);
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
                // one transaction -> one WAL record + one KV batch (batched durability)
                let mut t = self.txn.begin();
                for (k, row) in rows.iter().enumerate() {
                    let encoded = encode_row(row, &types);
                    t.put(row_key(table, base + k), encoded.into_bytes());
                }
                self.txn.commit(t)?;
                for (k, row) in rows.iter().enumerate() {
                    self.update_indexes_on_insert(table, base + k, row)?;
                }
                self.bump_writes(table);
                Ok(QueryResult::Inserted { count: rows.len() })
            }
            Statement::Delete { table, where_pred } => {
                let schema = self.table_schema(table)?;
                let pred = match where_pred {
                    Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                for (seq, row) in self.fetch_rows_with_ids(table)? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        to_del.push(seq);
                    }
                }
                if !to_del.is_empty() {
                    let mut t = self.txn.begin();
                    for seq in &to_del {
                        t.put(del_key(table, *seq), b"1".to_vec());
                    }
                    self.txn.commit(t)?;
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
                for (seq, row) in self.fetch_rows_with_ids(table)? {
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
                    // update = delete + append within a single transaction
                    let mut next_seq = self.next_row_seq(table)?;
                    let mut t = self.txn.begin();
                    for seq in &to_del {
                        t.put(del_key(table, *seq), b"1".to_vec());
                    }
                    for nr in &new_rows {
                        let encoded = nr
                            .iter()
                            .map(|v| v.to_slt_string())
                            .collect::<Vec<_>>()
                            .join("\t");
                        t.put(row_key(table, next_seq), encoded.into_bytes());
                        next_seq += 1;
                    }
                    self.txn.commit(t)?;
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
        self.rt
            .block_on(pipeline(columns, rows, stages, self.exchange_capacity))
    }

    /// Maintains declared indexes on insert.
    fn update_indexes_on_insert(&self, table: &str, seq: usize, values: &[Literal]) -> Result<()> {
        let schema = self.table_schema(table)?;
        let mut idxs = self.indexes.lock().unwrap();
        for (k, idx) in idxs.iter_mut() {
            if let Some(col) = k.strip_prefix(&format!("{table}.")) {
                if let Some(ci) = schema.iter().position(|c| c == col) {
                    if let Some(v) = values.get(ci) {
                        idx.insert(v.to_slt_string().as_bytes(), seq as u64);
                    }
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
        for (k, _v) in self.store.scan_prefix(&prefix)? {
            if let Ok(seq) = String::from_utf8_lossy(&k[prefix.len()..]).parse::<usize>() {
                // A compacted (renumbered) row is tombstoned at its old id;
                // it must not push the next id past the live range.
                if self.store.get(&del_key(table, seq))?.is_some() {
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
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut rows = Vec::new();
        for (k, v) in self.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            if self.store.get(&del_key(table, seq))?.is_some() {
                continue; // deleted
            }
            let mut cells: Vec<Value> = String::from_utf8_lossy(&v)
                .split('\t')
                .map(parse_value)
                .collect();
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

    /// Per-column declared type text for `table`.
    fn column_types(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .types)
    }

    /// The names of the stored snapshots (`CREATE SNAPSHOT`).
    pub fn snapshot_names(&self) -> Result<Vec<String>> {
        let mut out = Vec::new();
        for (k, _) in self.store.scan_prefix(b"snap:")? {
            out.push(String::from_utf8_lossy(&k[5..]).to_string());
        }
        out.sort();
        Ok(out)
    }

    /// Records the comment given to `CREATE INDEX` (the SDK reads it back from
    /// `SHOW INDEX`).
    pub fn set_index_comment(&self, table: &str, index: &str, comment: &str) {
        self.index_comments
            .lock()
            .unwrap()
            .insert(format!("{table}#{index}"), comment.to_string());
    }

    /// The comment recorded for `index` on `table`, if any.
    pub fn index_comment(&self, table: &str, index: &str) -> Option<String> {
        self.index_comments
            .lock()
            .unwrap()
            .get(&format!("{table}#{index}"))
            .cloned()
    }

    /// True unless `result_cache` is explicitly `off`.
    /// Rows a single imported segment can hold (`1024 * 8192`).
    pub const SEGMENT_ROW_CAPACITY: usize = 1024 * 8192;

    /// Rows committed per block while streaming a `COPY … FROM`.
    const COPY_BATCH_ROWS: usize = 8192;

    /// Number of segments `table` has: at least one per write batch, and more
    /// when a single import exceeds the segment capacity.
    pub fn segment_count(&self, table: &str) -> usize {
        let key = &table
            .split('.')
            .next_back()
            .unwrap_or(table)
            .to_ascii_lowercase();
        let rows = self.row_count(table).unwrap_or(0);
        let batches = match self.write_batches.lock().unwrap().get(key).copied() {
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
        if self.catalog.get_table_by_name(&table)?.is_some() {
            self.catalog.drop_table(&table)?;
            for (k, _) in self.store.scan_prefix(&row_prefix(&table))? {
                self.store.delete(&k)?;
            }
            for (k, _) in self.store.scan_prefix(&del_prefix(&table))? {
                self.store.delete(&k)?;
            }
        }
        self.catalog.create_table_typed(
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
                self.store.put(&row_key(&table, i), &encoded.into_bytes())?;
            }
        }
        // re-create the snapshot's indexes
        for (name, column, ty, opts) in indexes {
            self.catalog
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
        self.store.put(&snap_key(name), out.as_bytes())?;
        Ok(())
    }

    /// `CREATE SYSTEM SNAPSHOT <name>`: every table of every database.
    pub fn create_system_snapshot(&self, name: &str) -> Result<()> {
        let mut out = String::new();
        for meta in self.catalog.list_tables()? {
            if !out.is_empty() {
                out.push(DB_SNAP_SEP);
            }
            out.push_str(&self.snapshot_table_blob(&meta.name)?);
        }
        self.store.put(&snap_key(name), out.as_bytes())?;
        Ok(())
    }

    /// Restores every table of a database snapshot.
    pub fn restore_database_snapshot(&self, name: &str) -> Result<()> {
        let data = self
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
            self.catalog.create_database(db, true)?;
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
                .store
                .get(&row_key(table, *old))?
                .ok_or_else(|| Error::Storage("row vanished during compaction".into()))?;
            t.put(row_key(table, i), v);
            t.put(del_key(table, *old), b"1".to_vec());
            moved = true;
        }
        if moved {
            self.txn.commit(t)?;
            // Drop the old positions once the tombstones are durable; recovery
            // re-creates both, and the tombstone keeps the old row hidden.
            for (i, (old, _)) in rows.iter().enumerate() {
                if *old != i {
                    self.store.delete(&row_key(table, *old))?;
                }
            }
        }
        for (k, _) in self.store.scan_prefix(&del_prefix(table))? {
            self.store.delete(&k)?;
        }
        let key = table
            .split('.')
            .next_back()
            .unwrap_or(table)
            .to_ascii_lowercase();
        self.row_counters
            .lock()
            .unwrap()
            .insert(key.clone(), (0, rows.len() as u32, true));
        self.write_batches.lock().unwrap().insert(key, 1);
        // Block-level indexes and cached results refer to the old row ids.
        let prefix = format!("{table}.");
        self.indexes
            .lock()
            .unwrap()
            .retain(|k, _| !k.starts_with(&prefix));
        self.vector_indexes
            .lock()
            .unwrap()
            .retain(|k, _| !k.starts_with(&prefix));
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
        if let Some(v) = self.config.lock().unwrap().get(&key) {
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
        self.config
            .lock()
            .unwrap()
            .iter()
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect()
    }

    fn result_cache_enabled(&self) -> bool {
        match self.config.lock().unwrap().get("result_cache") {
            Some(v) => !v.eq_ignore_ascii_case("off"),
            None => true,
        }
    }

    /// Whether new results are being cached (`off`/`suspend` disable it).
    fn result_cache_active(&self) -> bool {
        match self.config.lock().unwrap().get("result_cache") {
            Some(v) => !(v.eq_ignore_ascii_case("off") || v.eq_ignore_ascii_case("suspend")),
            None => true,
        }
    }

    /// Maximum cached entries (`cache_result_capacity`); unbounded by default.
    fn result_cache_capacity(&self) -> usize {
        self.config
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
        let a = self
            .index_analyzers
            .lock()
            .unwrap()
            .get(&index_key(table, column))?
            .to_ascii_lowercase();
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
        let schema = self.table_schema(table)?;
        // A full-text search runs through an index; without one the C++ engine
        // refuses the statement.
        if m.kind.eq_ignore_ascii_case("text") {
            let indexes = self.catalog.list_indexes().unwrap_or_default();
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
        // BM25 (k1 = 1.2, b = 0.75) over the whole table; a document's length is
        // the number of indexed terms, and `similarity=boolean` scores one point
        // per matching term weighted by the field/term boosts.
        // Deleted rows still contribute to the index statistics (they are only
        // filtered from the result), so gather every stored row first.
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        // (row id, row, per-column tokens with positions, live)
        #[allow(clippy::type_complexity)]
        let mut docs: Vec<(usize, Vec<Value>, Vec<Vec<(String, u32)>>, bool)> = Vec::new();
        for (k, v) in self.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            let mut row: Vec<Value> = String::from_utf8_lossy(&v)
                .split('\t')
                .map(parse_value)
                .collect();
            // Rows loaded before a column was added (or from a narrower COPY)
            // take the column's default value, not NULL.
            while row.len() < defaults.len() {
                let i = row.len();
                row.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
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
            let live = self.store.get(&del_key(table, seq))?.is_none();
            docs.push((seq, row, fields, live));
        }
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
                    let analyzer = self
                        .index_analyzers
                        .lock()
                        .unwrap()
                        .get(&index_key(table, col))
                        .cloned()
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
        let n_docs = docs.len().max(1);
        // One BM25 corpus per column (Lucene keeps statistics per field).
        let avg_lens: Vec<f64> = (0..cols.len())
            .map(|f| {
                let sum: f64 = docs.iter().map(|d| d.2[f].len() as f64).sum();
                let a = sum / docs.len().max(1) as f64;
                if a <= 0.0 {
                    1.0
                } else {
                    a
                }
            })
            .collect();
        // A `keyword`/`whitespace` field scores constantly: one field-boost
        // point per matching document (not per query term).
        let keyword_field = matches!(&delims, Some(d) if d.is_empty());
        let mut scored: Vec<(usize, f64)> = Vec::new();
        for (seq, row, fields, live) in &docs {
            if !live {
                continue;
            }
            if let Some(p) = &in_filter {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            // The outer WHERE (a fusion child's inherited filter) prunes
            // candidates before the top-N ranking.
            if let Some(p) = pred {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            // `similarity=boolean` scores each field independently: a
            // `keyword`/`whitespace` field contributes its boost once when any
            // query term is present, a standard field contributes one boost per
            // matching term.
            if boolean {
                let mut bscore = 0.0f64;
                let mut bhits = 0usize;
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
                for (ci, tags) in &rank_features {
                    let cell = row.get(*ci).map(|v| v.to_slt_string()).unwrap_or_default();
                    bscore += rank_features_bonus(&cell, tags);
                }
                if bhits >= min_should
                    && bhits > 0
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
                    let df = docs
                        .iter()
                        .filter(|(_, _, fs, _)| fs[f].iter().any(|(x, _)| x == term))
                        .count()
                        .max(1) as f64;
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
            for (ci, tags) in &rank_features {
                let cell = row.get(*ci).map(|v| v.to_slt_string()).unwrap_or_default();
                score += rank_features_bonus(&cell, tags);
            }
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
        for (k, v) in self.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            let mut row: Vec<Value> = String::from_utf8_lossy(&v)
                .split('\t')
                .map(parse_value)
                .collect();
            while row.len() < defaults.len() {
                let i = row.len();
                row.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            if self.store.get(&del_key(table, seq))?.is_some() {
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
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .defaults)
    }

    /// Loads a CSV file into `table` (`COPY t FROM …`). The path may be absolute
    /// or the Infinity fixture path `/var/infinity/test_data/…`, which is mapped
    /// onto `INFINITY_DATA_DIR` (default `test/data`) if present.
    fn copy_from(&self, table: &str, path: &str, opts: &[(String, String)]) -> Result<QueryResult> {
        if self.catalog.get_table_by_name(table)?.is_none() {
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
                let mut t = self.txn.begin();
                for (k, row) in batch.iter().enumerate() {
                    let encoded = encode_row(row, &types_for_scan);
                    t.put(row_key(table, base + k), encoded.into_bytes());
                }
                self.txn.commit(t)?;
                for (k, row) in batch.iter().enumerate() {
                    self.update_indexes_on_insert(table, base + k, row)?;
                }
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
        let mut t = self.txn.begin();
        for (k, row) in rows.iter().enumerate() {
            let encoded = encode_row(row, types);
            t.put(row_key(table, base + k), encoded.into_bytes());
        }
        self.txn.commit(t)?;
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row)?;
        }
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
                    | "io_limit"
                    | "distance"
                    | "similarity"
                    | "alpha"
                    | "beta"
            );
            if !known {
                return Err(Error::Invalid(format!(
                    "invalid parameter: unknown option '{key}'"
                )));
            }
        }
        let threshold = with_opts
            .and_then(parse_threshold)
            .or_else(|| parse_threshold(&params));

        let rows = self.fetch_rows_with_ids(table)?;
        // A `bit` sparse column scores and renders as an integer.
        let bit_sparse = is_sparse && types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false);
        // Walk a persisted HNSW graph instead of every row when one is loaded
        // for this column (plan #70). The candidates are still scored exactly
        // below, so the ranking equals the exhaustive scan whenever the graph
        // yields the whole top-`k` — `k` is generous and capped by the row
        // count, which fully covers a small table.
        let vector_candidates: Option<std::collections::HashSet<usize>> = (|| {
            if is_sparse || is_tensor || metric != "l2" {
                return None;
            }
            let col_name = match_columns(&m.column).first()?.clone();
            let loaded = self
                .vector_indexes
                .lock()
                .unwrap()
                .get(&index_key(table, &col_name))
                .cloned()?;
            let col_type = types.get(col_idx)?.to_ascii_uppercase();
            if col_type.starts_with("MULTIVECTOR") || is_bit_type(&col_type) {
                return None; // the graph is a single-vector L2 index
            }
            let qv: Vec<f32> = query_items
                .iter()
                .filter_map(|x| x.parse::<f64>().ok())
                .map(|x| x as f32)
                .collect();
            if qv.is_empty() || qv.len() != loaded.index.dims() {
                return None;
            }
            let k = (topn * 4).max(64).min(loaded.labels.len());
            Some(
                loaded
                    .index
                    .search(&qv, k)
                    .into_iter()
                    .map(|(node, _)| loaded.labels[node] as usize)
                    .collect(),
            )
        })();
        let mut scored: Vec<(usize, f64, f64)> = Vec::new();
        for (seq, row) in &rows {
            // The WHERE predicate filters candidates *before* top-N ranking.
            if let Some(p) = pred {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            if let Some(p) = &in_filter {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            if let Some(cands) = &vector_candidates {
                if !cands.contains(seq) {
                    continue;
                }
            }
            if let Some(Value::Str(v)) = row.get(col_idx) {
                if is_tensor {
                    let qtext = format!("[{}]", query_items.join(","));
                    let qtext = match type_dim(&types[col_idx]) {
                        Some(d) => reshape_tensor(&qtext, d),
                        None => qtext,
                    };
                    let q = parse_tensor_leaves(&qtext);
                    // The column's declared element type and the query's
                    // declared element type both drive the rounding precision.
                    let col_elem =
                        tensor_elem_type(types.get(col_idx).map(String::as_str).unwrap_or(""));
                    let qry_elem = arg_str(m.args.get(1))
                        .map(|s| elem_type_of(&s))
                        .unwrap_or(ElemType::Float32);
                    let score = tensor_score(
                        &q,
                        v,
                        col_elem,
                        qry_elem,
                        types
                            .get(col_idx)
                            .map(|t| t.to_ascii_uppercase().starts_with("TENSORARRAY"))
                            .unwrap_or(false),
                    );
                    scored.push((*seq, score, -score));
                } else if is_sparse {
                    // A `bit` sparse vector holds bare indices; its inner product
                    // is the number of shared positions.
                    let dot = if bit_sparse {
                        let qidx: Vec<i64> = query_items
                            .iter()
                            .filter_map(|x| x.trim().parse::<i64>().ok())
                            .collect();
                        let ridx: Vec<i64> = v
                            .trim()
                            .trim_start_matches('[')
                            .trim_end_matches(']')
                            .split(',')
                            .filter_map(|x| x.trim().parse::<i64>().ok())
                            .collect();
                        qidx.iter().filter(|i| ridx.contains(i)).count() as f64
                    } else {
                        let (qv, rv) = (parse_sparse(&query_items), parse_sparse_str(v));
                        sparse_dot(&qv, &rv)
                    };
                    scored.push((*seq, dot, -dot));
                } else {
                    // A `bit` embedding column stores a bit string, so both the
                    // query and the stored value are read as 0/1 positions.
                    let bit_col = types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false);
                    let mut qv: Vec<f64> = query_items
                        .iter()
                        .filter_map(|x| x.parse::<f64>().ok())
                        .collect();
                    // The SDK packs a bit query into bytes (LSB first); expand
                    // it to one bit per position unless it is already un-packed.
                    if bit_col {
                        let decl = types.get(col_idx).map(String::as_str).unwrap_or("");
                        let dim = type_dim(decl).unwrap_or(0);
                        let multi = decl.to_ascii_uppercase().starts_with("MULTIVECTOR");
                        if dim > 0 && qv.len() != dim && qv.len() * 8 >= dim {
                            let mut bits = Vec::with_capacity(qv.len() * 8);
                            for b in &qv {
                                let byte = *b as u8;
                                for k in 0..8 {
                                    bits.push(((byte >> k) & 1) as f64);
                                }
                            }
                            // A multivector query packs several `dim`-sized rows.
                            if !multi {
                                bits.truncate(dim);
                            }
                            qv = bits;
                        } else {
                            qv = qv
                                .into_iter()
                                .map(|x| if x != 0.0 { 1.0 } else { 0.0 })
                                .collect();
                        }
                    }
                    if qv.is_empty() {
                        continue;
                    }
                    // a MULTIVECTOR holds several vectors; the row scores by the
                    // best-matching sub-vector
                    let multivector = types
                        .get(col_idx)
                        .map(|t| t.to_ascii_uppercase().starts_with("MULTIVECTOR"))
                        .unwrap_or(false);
                    if multivector {
                        // MAXSIM: split both sides into `dim`-sized rows and sum,
                        // over the query rows, the best similarity to a stored row.
                        let dim = type_dim(types.get(col_idx).map(String::as_str).unwrap_or(""))
                            .unwrap_or(qv.len())
                            .max(1);
                        // A bit multivector stores one bit string per row.
                        let doc_rows = if tensor_elem_type(
                            types.get(col_idx).map(String::as_str).unwrap_or(""),
                        ) == ElemType::Bit
                        {
                            parse_bit_leaves(v)
                        } else {
                            parse_tensor_leaves(v)
                        };
                        if doc_rows.is_empty() || doc_rows.iter().all(|r| r.is_empty()) {
                            continue;
                        }
                        let qrows: Vec<&[f64]> = qv.chunks(dim).collect();
                        let (mut total, mut total_dist) = (0.0f64, 0.0f64);
                        for qr in qrows {
                            let mut best: Option<(f64, f64)> = None;
                            for dr in &doc_rows {
                                if dr.len() != qr.len() {
                                    continue;
                                }
                                let (sim, dist) = vector_score(qr, dr, &metric);
                                if best.map(|(b, _)| sim > b).unwrap_or(true) {
                                    best = Some((sim, dist));
                                }
                            }
                            if let Some((sim, dist)) = best {
                                total += sim;
                                total_dist += dist;
                            }
                        }
                        // MAXSIM distance is the sum of the best per-row
                        // distances; the similarity is its `1/(1+d)` form.
                        scored.push((*seq, total, total_dist));
                        continue;
                    }
                    let rv: Vec<f64> = if bit_col {
                        parse_bit_leaves(v).into_iter().flatten().collect()
                    } else {
                        parse_vec_str(v)
                    };
                    if rv.len() != qv.len() {
                        continue;
                    }
                    let (sim, dist) = vector_score(&qv, &rv, &metric);
                    scored.push((*seq, sim, dist));
                }
            }
        }
        let use_sim = is_sparse || is_tensor || matches!(metric.as_str(), "cosine" | "ip");
        scored.sort_by(|a, b| {
            let (x, y) = if use_sim { (b.1, a.1) } else { (a.2, b.2) };
            x.partial_cmp(&y)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        if let Some(th) = threshold {
            // For a distance metric the threshold is an upper bound; for a
            // similarity metric it is a lower bound.
            let is_dist = !is_sparse && !is_tensor && matches!(metric.as_str(), "l2" | "hamming");
            scored.retain(|(_, sim, dist)| {
                if is_dist {
                    *dist <= th
                } else if use_sim {
                    *sim >= th
                } else {
                    *dist >= th
                }
            });
        }
        scored.truncate(topn);

        let mut by_id: HashMap<usize, Vec<Value>> = rows.into_iter().collect();
        let num = |f: f64| {
            if bit_sparse {
                Value::Int(f as i64)
            } else {
                Value::Float(f)
            }
        };
        let mut out = Vec::new();
        for (seq, sim, dist) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(num(sim));
            row.push(Value::Float(dist));
            row.push(num(if use_sim { sim } else { dist }));
            out.push(row);
        }
        Ok((out, hidden))
    }

    pub fn catalog(&self) -> &Catalog {
        &self.catalog
    }

    /// The underlying row/meta store (exposed for tests and tooling that need to
    /// inspect the on-disk key space).
    pub fn store(&self) -> &std::sync::Arc<dyn MetaStore> {
        &self.store
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
        matches!(self.catalog.get_table_by_name(&name), Ok(Some(_)))
    }

    /// Number of live (non-deleted) rows in `table`.
    pub fn row_count(&self, table: &str) -> Result<usize> {
        let db = self.db();
        let name = if table.contains('.') {
            table.to_string()
        } else {
            format!("{db}.{table}")
        };
        Ok(self.fetch_rows(&name)?.len())
    }

    pub fn flush(&self) -> Result<()> {
        self.catalog.flush()?;
        self.txn.flush()?;
        self.flush_cpp_blocks()?;
        // Mirror the catalog into the C++ key space / value encoding, in the
        // catalog store at `<dir>/catalog` (the path the C++ engine uses), so a
        // C++ engine can read this directory. Best effort: a column type the
        // C++ encoding does not cover yet leaves the previous layout in place
        // rather than failing the flush.
        let _ = self.catalog.write_cpp_layout(self.catalog.store().as_ref());
        Ok(())
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

/// Extracts a `threshold = <n>` value from a `WITH (…)` option string.
fn parse_threshold(opts: &str) -> Option<f64> {
    let idx = opts.to_ascii_lowercase().find("threshold")?;
    let rest = &opts[idx..];
    let eq = rest.find('=')?;
    let tail: String = rest[eq + 1..]
        .trim_start()
        .chars()
        .take_while(|c| c.is_ascii_digit() || *c == '.' || *c == '-' || *c == '+')
        .collect();
    tail.trim().parse().ok()
}

fn parse_vec_str(s: &str) -> Vec<f64> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    t.split([',', ' '])
        .filter(|x| !x.is_empty())
        .filter_map(|x| x.parse::<f64>().ok())
        .collect()
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
    const ALLOW: u8 = 0;
    const DELIM: u8 = 1;
    const SPACE: u8 = 2;
    const UNITE: u8 = 3;
    fn kind(b: u8) -> u8 {
        if b > 127 || b.is_ascii_alphanumeric() {
            ALLOW
        } else if b.is_ascii_whitespace() {
            SPACE
        } else if b == b'/' {
            UNITE
        } else {
            DELIM
        }
    }
    let bytes = s.as_bytes();
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
            // A lone delimiter is its own token, which `DoNext` then skips.
            i += 1;
            continue;
        }
        let mut tok = vec![bytes[i]];
        i += 1;
        while i < bytes.len() {
            match kind(bytes[i]) {
                SPACE | DELIM => break,
                ALLOW => {
                    tok.push(bytes[i]);
                    i += 1;
                }
                _ => i += 1, // UNITE: dropped, token continues
            }
        }
        let text = String::from_utf8_lossy(&tok).to_ascii_lowercase();
        if !text.is_empty() {
            out.push(text);
        }
    }
    out
}

/// Query-side tokens: the standard analyzer's stem replaces the word.
fn tokenize_text(s: &str, delims: &Option<Vec<char>>) -> Vec<String> {
    let words = split_words(s, delims);
    // The C++ default analyzer is `standard` (`index_full_text_impl.cpp:68`) — a
    // `StandardAnalyzer` with `STEM_LANG_ENGLISH` (`analyzer_pool_impl.cpp:266-289`)
    // — so the `None` (standard) case stems English. `keyword`/`whitespace`(±chars)
    // do not.
    if delims.is_none() {
        words.into_iter().map(|t| stem_english(&t)).collect()
    } else {
        words
    }
}

/// The **index-side** tokenizer. The C++ `standard` analyzer emits each token
/// *and* its English stem when the two differ; it does not replace the token.
/// The engine's own statistics prove it: for a five-row probe table the C++
/// lengths are 2,4,6,5,4 for token counts 2,3,4,4,2 — every extra term is a
/// stem that differs (`multiple`->`multipl`, `many`->`mani`, `words`->`word`,
/// `another`->`anoth`), and `doc_len_sum` is 21 rather than 15. Query terms stay
/// stem-only (a boolean query for `second text multiple` scores 3, not 4).
fn tokenize_index_text(s: &str, delims: &Option<Vec<char>>) -> Vec<String> {
    let words = split_words(s, delims);
    if delims.is_some() {
        return words;
    }
    let mut out = Vec::with_capacity(words.len());
    for t in words {
        let stem = stem_english(&t);
        if stem != t {
            out.push(t);
            out.push(stem);
        } else {
            out.push(t);
        }
    }
    out
}

/// Index-side tokens **with their position**. The C++ analyzer emits a token's
/// original and its stem at the same offset (`common_analyzer_impl.cpp`
/// re-uses `offset_` for both), so both share a position; `offset_` itself
/// advances once per raw token. Phrase queries need this.
fn tokenize_index_positions(s: &str, delims: &Option<Vec<char>>) -> Vec<(String, u32)> {
    let words = split_words(s, delims);
    let mut out = Vec::with_capacity(words.len());
    for (pos, word) in words.into_iter().enumerate() {
        if delims.is_some() {
            out.push((word, pos as u32));
            continue;
        }
        let stem = stem_english(&word);
        if stem != word {
            out.push((word, pos as u32));
            out.push((stem, pos as u32));
        } else {
            out.push((word, pos as u32));
        }
    }
    out
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
    cap: usize,
) -> Result<(Vec<String>, Vec<Vec<String>>)> {
    let (tx, mut rx) = mpsc::channel::<Vec<Value>>(cap);
    let producer = tokio::spawn(async move {
        for r in rows {
            if tx.send(r).await.is_err() {
                break;
            }
        }
    });
    let mut data: Vec<Vec<Value>> = Vec::new();
    while let Some(row) = rx.recv().await {
        data.push(row);
    }
    let _ = producer.await;

    let mut cols = columns;
    for stage in &stages {
        match stage {
            Stage::Filter(expr) => data.retain(|r| expr.eval_predicate(r)),
            Stage::Projection(exprs, names) => {
                data = data
                    .iter()
                    .map(|r| exprs.iter().map(|e| e.eval(r)).collect())
                    .collect();
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
}
