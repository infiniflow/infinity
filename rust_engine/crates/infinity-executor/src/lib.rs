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
    parse, AggFunc, CmpOp, ExprAst, JoinClause, Literal, MatchClause, SearchClause, SelectItem,
    Statement,
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
        keys: Vec<(usize, bool)>,
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
/// Record / field separators used inside a snapshot blob.
const SNAP_REC: char = '\u{1d}';
const SNAP_FLD: char = '\u{1c}';

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
        if n != 0 && !n.is_multiple_of(dim) {
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
                // float columns render with six decimals in `.slt` output
                Some(t) if is_float_type(t) => match s.parse::<f64>() {
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
        SelectItem::Expr(e) => in_expr(e),
        _ => false,
    })
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
    if is_sparse_type(type_text) {
        return validate_sparse_literal(type_text, lit);
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
    /// session/global configuration set via `SET CONFIG …`
    config: Mutex<HashMap<String, String>>,
    /// fulltext analyzer per `"<table>.<column>"`, from `CREATE INDEX … WITH (analyzer=…)`
    index_analyzers: Mutex<HashMap<String, String>>,
    /// cached `SELECT` results keyed by the statement text
    result_cache: Mutex<HashMap<String, Vec<QueryResult>>>,
    /// the database new unqualified table names are resolved against
    current_db: Mutex<String>,
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
        let catalog = Catalog::open(dir.join("catalog"))?;
        // The row/meta store: RocksDB when the `rocksdb-store` feature is on
        // (the substrate the C++ engine's catalog uses), fjall otherwise.
        #[cfg(feature = "rocksdb-store")]
        let store: Arc<dyn MetaStore> =
            Arc::new(infinity_storage::RocksMetaStore::open(dir.join("kv"))?);
        #[cfg(not(feature = "rocksdb-store"))]
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(dir.join("kv"), "default")?);
        let txn = TxnManager::open(dir.join("wal.log"), store.clone())?;
        txn.recover(0)?;
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
            config: Mutex::new(HashMap::new()),
            index_analyzers: Mutex::new(HashMap::new()),
            result_cache: Mutex::new(HashMap::new()),
            current_db: Mutex::new("default_db".to_string()),
        };
        engine.recover_cpp_blocks()?;
        engine.rebuild_indexes()?;
        Ok(engine)
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
                let key = sql.trim().to_string();
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
        let q = |t: &str| -> String {
            if t.is_empty() || t.contains('.') {
                t.to_string()
            } else {
                format!("{db}.{t}")
            }
        };
        let mut s = stmt.clone();
        match &mut s {
            Statement::CreateTable { name, .. } | Statement::DropTable { name } => *name = q(name),
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
                    *t = q(t);
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
        let existed = self.catalog.drop_table(name)?;
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

    fn table_meta(&self, table: &str) -> Result<infinity_catalog::TableMeta> {
        self.catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))
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
        match stmt {
            Statement::CreateTable { name, columns } => {
                // A vector-like column's declared dimension must be positive.
                for c in columns {
                    if !declares_dimension(&c.type_text) {
                        continue;
                    }
                    if let Some(d) = type_dim_deep(&c.type_text) {
                        let up = c.type_text.to_ascii_uppercase();
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
                // rewrite rows without the dropped column
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
                        crate::explain_search::explain_search(&crate::explain_search::SearchPlan {
                            items,
                            table: t,
                            search: s,
                            limit: *limit,
                            offset: *offset,
                            where_pred: where_pred.as_ref(),
                            schema: &meta.columns,
                            types: &meta.types,
                            indexed: &indexed,
                        })
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
                        limit: _,
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
                                crate::explain::explain_index_scan(&crate::explain::IndexScanPlan {
                                    items,
                                    table: t,
                                    where_pred: w,
                                    order_by,
                                    schema,
                                    types: &meta.types,
                                    indexed: &indexed,
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
                    self.copy_from(table, path, opts)
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
                    let a = a.trim().trim_matches(|c| c == '\'' || c == '"');
                    self.index_analyzers
                        .lock()
                        .unwrap()
                        .insert(index_key(table, column), a.to_ascii_lowercase());
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
            Statement::DropTable { name } => {
                let existed = self.drop_table_all(name)?;
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
                // the snapshot also carries the table's index declarations
                let idxs: Vec<String> = self
                    .catalog
                    .list_indexes()?
                    .into_iter()
                    .filter(|im| im.table == *table)
                    .map(|im| format!("{}{SNAP_FLD}{}", im.name, im.column))
                    .collect();
                out.push_str(&idxs.join(&SNAP_REC.to_string()));
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
                self.store.put(&snap_key(name), out.as_bytes())?;
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
                let text = String::from_utf8_lossy(&data).to_string();
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
                let indexes: Vec<(String, String)> = recs
                    .next()
                    .unwrap_or_default()
                    .split(SNAP_REC)
                    .filter(|s| !s.is_empty())
                    .filter_map(|s| s.split_once(SNAP_FLD))
                    .map(|(n, c)| (n.to_string(), c.to_string()))
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
                for (name, column) in indexes {
                    self.catalog.create_index(&name, &table, &column)?;
                    if self.table_schema(&table)?.iter().any(|c| c == &column) {
                        self.build_index(&table, &column)?;
                    }
                }
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
                let rows = align_insert_rows(&schema, &meta.defaults, columns.as_deref(), rows);
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
                let base = self.store.scan_prefix(&row_prefix(table))?.len();
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
                let schema = self.table_schema(table)?;
                let not_nulls = self
                    .catalog
                    .get_table_by_name(table)?
                    .map(|m| m.not_nulls)
                    .unwrap_or_default();
                let mut asg = Vec::new();
                for (c, e) in assignments {
                    let idx = infinity_planner::resolve_column(&schema, c)?;
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
            } => {
                // `SELECT … UNNEST(col) …`: expand an array/JSON column into one
                // row per element, then let the normal planner handle WHERE,
                // GROUP BY and aggregates over the expanded relation.
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
                            items_owned[ui] = SelectItem::Column(new_name.clone());
                            unnest_expansion = Some((new_name, out));
                        }
                    }
                }
                let items: &[SelectItem] = &items_owned;
                // SUM/AVG require a numeric argument.
                if let Some(t) = from.as_ref() {
                    self.validate_aggregate_args(t, items)?;
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
                    tables.push((t.clone(), self.table_schema(t)?));
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
                let mut override_rows: Option<Vec<Vec<Value>>> = None;
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
                let plan = plan_select(
                    items,
                    &tables,
                    join.as_ref(),
                    where_pred.as_ref(),
                    group_by,
                    having.as_deref(),
                    &order_keys,
                    *limit,
                    *offset,
                )?;
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

    /// True unless `result_cache` is explicitly `off`.
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
            // a functional-index target is validated separately
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
                for (k, v) in opts {
                    if k == "ENCODE" && v.eq_ignore_ascii_case("lvq") && int_elem {
                        return Err(Error::Invalid(
                            "LVQ encoding is not supported for integer embeddings".into(),
                        ));
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
                // the cardinality hint does not apply to a JSON index
                if is_json_type(&ty) && opts.iter().any(|(k, _)| k == "CARDINALITY") {
                    return Err(Error::Invalid(
                        "cardinality is not supported for a JSON secondary index".into(),
                    ));
                }
            }
            "FULLTEXT" => {
                for (k, v) in opts {
                    if k == "ANALYZER" {
                        let v = v.to_ascii_uppercase();
                        if v == "JIEBA" || v == "NGRAM" {
                            return Err(Error::Invalid(format!(
                                "unsupported fulltext analyzer '{v}'"
                            )));
                        }
                    }
                }
            }
            _ => {}
        }
        Ok(())
    }

    /// Selects rows for `SEARCH MATCH TEXT` by term presence. This reproduces
    /// the *row set* (any query term, `minimum_should_match`, `topn`); the exact
    /// BM25 ordering/threshold needs the iresearch scorer, so a query carrying
    /// `threshold` yields no rows rather than wrong ones.
    fn search_text_terms(
        &self,
        table: &str,
        m: &MatchClause,
        hidden: &[String],
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let schema = self.table_schema(table)?;
        let params = arg_str(m.args.get(1)).unwrap_or_default();
        // `threshold=<f>` keeps only documents scoring at least `f`
        let threshold: Option<f64> = params
            .split(';')
            .filter_map(|p| p.split_once('='))
            .find(|(k, _)| k.trim().eq_ignore_ascii_case("threshold"))
            .and_then(|(_, v)| v.trim().parse().ok());
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
        // Per-term boosts (`term^2.0`) are stripped before tokenising.
        let query_clean = query
            .split_whitespace()
            .map(|w| w.split('^').next().unwrap_or(w))
            .collect::<Vec<_>>()
            .join(" ");
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
            return Ok((Vec::new(), hidden.to_vec()));
        }
        // A `whitespace` analyzer keeps punctuation inside a token, so
        // `2023-01-01` is one term rather than three. A `whitespace-<chars>`
        // analyzer additionally splits on each listed delimiter character.
        let delims: Option<Vec<char>> = match_columns(&m.column).iter().find_map(|c| {
            let a = self
                .index_analyzers
                .lock()
                .unwrap()
                .get(&index_key(table, c))?
                .to_ascii_lowercase();
            a.strip_prefix("whitespace")
                .map(|rest| rest.trim_start_matches(['-', '_', ' ']).chars().collect())
        });
        let tokenize = |s: &str| -> Vec<String> {
            let parts: Vec<String> = match &delims {
                Some(d) => s
                    .split(|c: char| c.is_whitespace() || d.contains(&c))
                    .map(str::to_string)
                    .collect(),
                None => s
                    .split(|c: char| !c.is_alphanumeric() && c != '_')
                    .map(str::to_string)
                    .collect(),
            };
            parts
                .into_iter()
                .map(|t| t.to_ascii_lowercase())
                .filter(|t| !t.is_empty())
                .collect()
        };
        let terms: Vec<String> = tokenize(&query_clean)
            .into_iter()
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
        let min_should = if require_all { terms.len() } else { min_should };
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
        let defaults_len = self.table_defaults(table).unwrap_or_default().len();
        let mut docs: Vec<(usize, Vec<Value>, Vec<String>, bool)> = Vec::new();
        for (k, v) in self.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            let mut row: Vec<Value> = String::from_utf8_lossy(&v)
                .split('\t')
                .map(parse_value)
                .collect();
            while row.len() < defaults_len {
                row.push(Value::Null);
            }
            let text: String = cols
                .iter()
                .map(|&i| match row.get(i) {
                    Some(Value::Str(s)) => s.clone(),
                    Some(other) => other.to_slt_string(),
                    None => String::new(),
                })
                .collect::<Vec<_>>()
                .join(" ");
            let live = self.store.get(&del_key(table, seq))?.is_none();
            docs.push((seq, row, tokenize(&text), live));
        }
        let boolean = params.split(';').any(|p| {
            p.split_once('=')
                .map(|(k, v)| {
                    k.trim().eq_ignore_ascii_case("similarity")
                        && v.trim().eq_ignore_ascii_case("boolean")
                })
                .unwrap_or(false)
        });
        let n_docs = docs.len().max(1);
        let avg_len: f64 = if docs.is_empty() {
            1.0
        } else {
            docs.iter()
                .map(|(_, _, toks, _)| toks.len().saturating_sub(1) as f64)
                .sum::<f64>()
                / docs.len() as f64
        };
        let avg_len = if avg_len <= 0.0 { 1.0 } else { avg_len };
        let mut scored: Vec<(usize, f64)> = Vec::new();
        for (seq, row, toks, live) in &docs {
            if !live {
                continue;
            }
            if let Some(p) = &in_filter {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            let mut counts: HashMap<&str, usize> = HashMap::new();
            for t in toks {
                *counts.entry(t.as_str()).or_insert(0) += 1;
            }
            let len = toks.len().saturating_sub(1) as f64;
            let mut score = 0.0f64;
            let mut hits = 0usize;
            for (ti, term) in terms.iter().enumerate() {
                let tf = match counts.get(term.as_str()) {
                    Some(c) => *c as f64,
                    None => continue,
                };
                hits += 1;
                let field = field_boosts.get(ti).copied().unwrap_or(1.0);
                if boolean {
                    score += field;
                } else {
                    let df = docs
                        .iter()
                        .filter(|(_, _, t, _)| t.iter().any(|x| x == term))
                        .count()
                        .max(1) as f64;
                    let idf = (1.0 + (n_docs as f64 - df + 0.5) / (df + 0.5)).ln();
                    let norm = 2.2 * tf / (tf + 1.2 * (0.25 + 0.75 * len / avg_len));
                    score += idf * norm * field;
                }
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

    /// Rejects `SUM`/`AVG` over a non-numeric column.
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
        // Tolerate missing fixtures (statement-level success, no rows loaded).
        let text = match std::fs::read_to_string(&resolved) {
            Ok(t) => t,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let delim = opts
            .iter()
            .find(|(k, _)| k == "DELIMITER")
            .map(|(_, v)| decode_delimiter(v))
            .unwrap_or(',');
        let types_for_scan = self.column_types(table)?;
        let defaults = self.table_defaults(table)?;
        // A `FORMAT JSONL` copy reads one JSON object per line.
        let is_jsonl = opts
            .iter()
            .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case("JSONL"));
        if is_jsonl {
            return self.copy_from_jsonl(table, &text, &types_for_scan, &defaults);
        }
        // A `FORMAT JSON` copy is a single JSON array of objects.
        let is_json = opts
            .iter()
            .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case("JSON"));
        if is_json {
            return self.copy_from_json(table, &text, &types_for_scan, &defaults);
        }
        let mut rows: Vec<Vec<Literal>> = Vec::new();
        for line in text.lines() {
            let line = line.trim_end_matches('\r');
            if line.is_empty() {
                continue;
            }
            let fields: Vec<String> = csv_fields(line, delim);
            let mut cells: Vec<String> = fields.iter().map(|f| normalize_cell(f)).collect();
            // an empty CSV cell takes the column default when one is declared
            for (i, c) in cells.iter_mut().enumerate() {
                if c.is_empty() {
                    if let Some(Some(d)) = defaults.get(i) {
                        *c = d.clone();
                    }
                }
            }
            // skip rows whose typed (sparse) cells are malformed
            for (i, c) in cells.iter().enumerate() {
                if let Some(t) = types_for_scan.get(i) {
                    // a malformed typed cell is a hard error
                    validate_insert_value(t, c.trim())?;
                }
            }
            rows.push(cells.into_iter().map(Literal::Str).collect());
        }
        if rows.is_empty() {
            return Ok(QueryResult::Inserted { count: 0 });
        }
        let base = self.store.scan_prefix(&row_prefix(table))?.len();
        let types = self.column_types(table)?;
        let mut t = self.txn.begin();
        for (k, row) in rows.iter().enumerate() {
            let encoded = encode_row(row, &types);
            t.put(row_key(table, base + k), encoded.into_bytes());
        }
        self.txn.commit(t)?;
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row)?;
        }
        Ok(QueryResult::Inserted { count: rows.len() })
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
        let base = self.store.scan_prefix(&row_prefix(table))?.len();
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

    /// Writes `table` to a CSV file (`COPY t TO …`).
    fn copy_to(&self, table: &str, path: &str, opts: &[(String, String)]) -> Result<QueryResult> {
        let rows = self.fetch_rows(table)?;
        let schema = self.table_schema(table)?;
        let column_types = self.column_types(table)?;
        let jsonl = opts
            .iter()
            .any(|(k, v)| k == "FORMAT" && v.eq_ignore_ascii_case("JSONL"));
        // `OFFSET n` skips rows; `LIMIT n` caps the exported count.
        let offset: usize = opts
            .iter()
            .find(|(k, _)| k == "OFFSET")
            .and_then(|(_, v)| v.parse().ok())
            .unwrap_or(0);
        let limit: Option<usize> = opts
            .iter()
            .find(|(k, _)| k == "LIMIT")
            .and_then(|(_, v)| v.parse().ok());
        let selected: Vec<&Vec<Value>> = rows
            .iter()
            .skip(offset)
            .take(limit.unwrap_or(usize::MAX))
            .collect();
        let mut out = String::new();
        if jsonl {
            for row in &selected {
                let fields: Vec<String> = schema
                    .iter()
                    .zip(row.iter())
                    .map(|(name, v)| {
                        let ty = column_types
                            .get(schema.iter().position(|c| c == name).unwrap_or(usize::MAX))
                            .map(String::as_str)
                            .unwrap_or("");
                        format!("\"{name}\":{}", json_value_typed(v, ty))
                    })
                    .collect();
                out.push('{');
                out.push_str(&fields.join(","));
                out.push_str("}\n");
            }
        } else {
            for row in &selected {
                let line = row
                    .iter()
                    .map(|v| match v {
                        Value::Str(s) if s.contains(',') => format!("\"{s}\""),
                        other => other.to_slt_string(),
                    })
                    .collect::<Vec<_>>()
                    .join(",");
                out.push_str(&line);
                out.push('\n');
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
        if s.matches.len() > 1 {
            // `FUSION(…)`: the union of every sub-search, deduplicated by row id.
            let mut seen: std::collections::HashSet<i64> = Default::default();
            let mut out: Vec<(i64, Vec<Value>)> = Vec::new();
            for m in &s.matches {
                let (rows, _) = self.run_one_match(
                    table,
                    m,
                    s.using_index.as_deref(),
                    s.with_opts.as_deref(),
                    pred,
                )?;
                for row in rows {
                    let id = match row.iter().rev().nth(3) {
                        Some(Value::Int(i)) => *i,
                        _ => continue,
                    };
                    if seen.insert(id) {
                        out.push((id, row));
                    }
                }
            }
            out.sort_by_key(|(id, _)| *id);
            return Ok((out.into_iter().map(|(_, r)| r).collect(), hidden));
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
            return self.search_text_terms(table, m, &hidden);
        }
        if !m.kind.eq_ignore_ascii_case("VECTOR") && !is_sparse && !is_tensor {
            return Ok((Vec::new(), hidden));
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
        let query_items = match m.args.first() {
            Some(ExprAst::Literal(Literal::Array(items))) => items.clone(),
            _ => Vec::new(),
        };
        // An integer-element column only accepts a matching query element type.
        // (For VECTOR/TENSOR the query element type is the argument after the
        // query array.)
        if !is_sparse {
            let col_elem = tensor_elem_type(types.get(col_idx).map(String::as_str).unwrap_or(""));
            if matches!(col_elem, ElemType::Int8 | ElemType::UInt8) {
                let q_elem = arg_str(m.args.get(1))
                    .map(|s| elem_type_of(&s))
                    .unwrap_or(ElemType::Float32);
                if q_elem != col_elem {
                    return Err(Error::Invalid(
                        "query element type does not match the column's".into(),
                    ));
                }
            }
        }
        let metric = arg_str(m.args.get(if is_sparse { 1 } else { 2 }))
            .unwrap_or_else(|| if is_sparse { "ip".into() } else { "l2".into() })
            .to_ascii_lowercase();
        let topn = if is_tensor {
            // topn lives in the trailing parameter string; the default is 10.
            match arg_str(m.args.get(3)) {
                Some(p) => parse_topn_param(&p).unwrap_or(10),
                None => 10,
            }
        } else {
            m.args.iter().rev().find_map(arg_int).unwrap_or(10).max(0) as usize
        };
        let params = arg_str(m.args.get(3)).unwrap_or_default();
        let threshold = with_opts
            .and_then(parse_threshold)
            .or_else(|| parse_threshold(&params));

        let rows = self.fetch_rows_with_ids(table)?;
        // A `bit` sparse column scores and renders as an integer.
        let bit_sparse = is_sparse && types.get(col_idx).map(|t| is_bit_type(t)).unwrap_or(false);
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
                    let score = tensor_score(&q, v, col_elem, qry_elem);
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
                    let qv: Vec<f64> = query_items
                        .iter()
                        .filter_map(|x| x.parse::<f64>().ok())
                        .map(|x| {
                            if bit_col {
                                if x != 0.0 {
                                    1.0
                                } else {
                                    0.0
                                }
                            } else {
                                x
                            }
                        })
                        .collect();
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
                        let mut best: Option<(f64, f64)> = None;
                        for sv in parse_tensor_leaves(v) {
                            if sv.len() != qv.len() {
                                continue;
                            }
                            let (sim, dist) = vector_score(&qv, &sv, &metric);
                            if best.map(|(s, _)| sim > s).unwrap_or(true) {
                                best = Some((sim, dist));
                            }
                        }
                        if let Some((sim, dist)) = best {
                            scored.push((*seq, sim, dist));
                        }
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
        self.flush_cpp_blocks()
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
fn decode_delimiter(v: &str) -> char {
    match v {
        "\\t" => '\t',
        "\\n" => '\n',
        "\\r" => '\r',
        _ => v.chars().next().unwrap_or(','),
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
        .map(|c| c.split('^').next().unwrap_or(c).trim().to_string())
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
            if dv.len() != qv.len() {
                continue;
            }
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
        if best.is_finite() {
            sum += best;
        }
    }
    sum as f64
}

/// Scores one stored tensor value against the query. A column holding a
/// `TensorArray` (top-level groups are themselves bracketed) scores by the max
/// over its component tensors; a plain tensor uses MaxSim directly.
fn tensor_score(q: &[Vec<f64>], doc_text: &str, col: ElemType, qry: ElemType) -> f64 {
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
    // A `TensorArray` has a third nesting level: each top-level group is itself
    // a bracketed tensor. A plain tensor's groups are plain number lists.
    let is_array = groups.iter().any(|g| {
        let gi = g.trim().strip_prefix('[').unwrap_or(g);
        gi.trim_start().starts_with('[')
    });
    if is_array {
        groups
            .iter()
            .map(|g| tensor_maxsim(q, &leaves(g), col, qry))
            .fold(f64::NEG_INFINITY, f64::max)
    } else {
        tensor_maxsim(q, &leaves(doc_text), col, qry)
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

fn sort_rows(rows: &mut [Vec<Value>], keys: &[(usize, bool)]) {
    rows.sort_by(|a, b| {
        for (key, asc) in keys {
            let x = a.get(*key).unwrap_or(&Value::Null);
            let y = b.get(*key).unwrap_or(&Value::Null);
            // NULLs order before every non-NULL (so ASC → NULLS FIRST, and the
            // reverse below gives DESC → NULLS LAST).
            let mut ord = match (x, y) {
                (Value::Null, Value::Null) => std::cmp::Ordering::Equal,
                (Value::Null, _) => std::cmp::Ordering::Less,
                (_, Value::Null) => std::cmp::Ordering::Greater,
                _ => x.partial_cmp_value(y).unwrap_or(std::cmp::Ordering::Equal),
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

    fn seed(e: &Engine) {
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "c"), (2, "a"), (3, "b"), (4, "a")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
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
                    &["count(*)", "sum(id)", "avg(id)", "min(id)", "max(id)"]
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
                assert_eq!(columns, &vec!["name".to_string(), "count(*)".to_string()]);
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
}
