//! C++ `FastRoughFilter` (`src/storage/fast_rough_filter/`): the per-segment and
//! per-block prefilter the reference builds when a segment is sealed
//! (`BuildFastRoughFilterTask::ExecuteOnNewSealedSegment`, called from
//! `NewTxn::Import` / `NewTxn::Compact`).
//!
//! The record has two halves:
//!
//! ```text
//! [u32 total][u64 build_time]
//!   [u32 probabilistic_total][u32 column_count]{ per column: u8 exists, u8 built }
//!   [u32 minmax_total][u32 column_count]{ per column: u8 variant_id, min, max }
//! ```
//!
//! The probabilistic (binary-fuse) half is only built for a column whose
//! `ColumnDef::build_bloom_filter_` was set by DDL (`CREATE TABLE … (c INT
//! /* bloom_filter */)`, `logical_planner_impl.cpp:716`); every other column
//! serializes as `1, 0` — the filter object exists, nothing was built into it.
//! The min/max half is built for every column whose type supports it.
//!
//! `build_time` is the segment's **begin** ts, i.e. the commit ts of the
//! transaction that created it minus one (`segment_meta->begin_ts()`).

use infinity_expression::Value;

/// `InnerMinMaxDataFilterVariant`: the index of each alternative in
/// `std::variant<monostate, TinyIntT, SmallIntT, IntegerT, BigIntT, HugeIntT,
/// DateT, TimeT, DateTimeT, TimestampT, VarcharT, FloatT, DoubleT>`
/// (`filter_value_type_classification.cppm:72`).
mod variant {
    pub const MONOSTATE: u8 = 0;
    pub const TINY_INT: u8 = 1;
    pub const SMALL_INT: u8 = 2;
    pub const INTEGER: u8 = 3;
    pub const BIG_INT: u8 = 4;
    pub const HUGE_INT: u8 = 5;
    pub const DATE: u8 = 6;
    pub const TIME: u8 = 7;
    pub const DATE_TIME: u8 = 8;
    pub const TIMESTAMP: u8 = 9;
    pub const VARCHAR: u8 = 10;
    pub const FLOAT: u8 = 11;
    pub const DOUBLE: u8 = 12;
}

/// `InnerMinMaxDataFilterVarcharType::INNER_STORED_LENGTH`.
const VARCHAR_STORED: usize = 16;

/// The variant id a column's declared type maps to, or `None` when the type
/// contributes no min/max filter (bool and decimal only get a bloom filter;
/// vector/sparse/tensor get neither).
fn variant_of(type_text: &str) -> Option<u8> {
    let t = type_text.trim().to_ascii_uppercase();
    let head = t.split(['(', ',']).next().unwrap_or("").trim();
    Some(match head {
        "TINYINT" => variant::TINY_INT,
        "SMALLINT" => variant::SMALL_INT,
        "INTEGER" | "INT" => variant::INTEGER,
        "BIGINT" => variant::BIG_INT,
        "HUGEINT" => variant::HUGE_INT,
        "DATE" => variant::DATE,
        "TIME" => variant::TIME,
        "DATETIME" => variant::DATE_TIME,
        "TIMESTAMP" => variant::TIMESTAMP,
        "VARCHAR" | "JSON" => variant::VARCHAR,
        "FLOAT" | "FLOAT32" => variant::FLOAT,
        "DOUBLE" | "FLOAT64" => variant::DOUBLE,
        _ => return None,
    })
}

/// One column's `InnerMinMaxDataFilterVarcharType`: 16 stored bytes plus the
/// length they were truncated to.
#[derive(Clone, Copy)]
struct Text {
    bytes: [u8; VARCHAR_STORED],
    len: u8,
}

impl Text {
    /// `InnerMinMaxDataFilterVarcharType::lowest()` — an empty string over a
    /// zeroed buffer.
    fn lowest() -> Self {
        Text {
            bytes: [0; VARCHAR_STORED],
            len: 0,
        }
    }

    /// `InnerMinMaxDataFilterVarcharType::max()` — a full-length string of
    /// `char` 127 over a 0x7f-filled buffer.
    fn highest() -> Self {
        Text {
            bytes: [0x7f; VARCHAR_STORED],
            len: VARCHAR_STORED as u8,
        }
    }

    /// `SetToTruncate`: copy at most 16 bytes, keep the rest of the buffer.
    fn truncate(mut self, input: &[u8]) -> Self {
        let len = input.len().min(VARCHAR_STORED);
        self.bytes[..len].copy_from_slice(&input[..len]);
        self.len = len as u8;
        self
    }

    fn view(&self) -> &[u8] {
        &self.bytes[..usize::from(self.len)]
    }
}

/// A column's min and max in the prefilter's representation.
#[derive(Clone, Copy)]
enum Bound {
    /// TinyInt … BigInt.
    Integral(i64),
    /// Float / Double, widened for comparison.
    Real(f64),
    /// Date / Time: the ordered day or second count.
    Day(i32),
    /// DateTime / Timestamp: `(date, time)`.
    Stamp(i32, i32),
    Text(Text),
}

fn bound_of(variant: u8, value: &Value) -> Option<Bound> {
    match variant {
        variant::TINY_INT
        | variant::SMALL_INT
        | variant::INTEGER
        | variant::BIG_INT
        | variant::HUGE_INT => Some(Bound::Integral(match value {
            Value::Int(i) => *i,
            Value::Str(s) => s.trim().parse::<i64>().ok()?,
            _ => return None,
        })),
        variant::FLOAT | variant::DOUBLE => Some(Bound::Real(match value {
            Value::Float(f) => *f,
            Value::Int(i) => *i as f64,
            Value::Str(s) => s.trim().parse::<f64>().ok()?,
            _ => return None,
        })),
        variant::DATE | variant::TIME => {
            let text = match value {
                Value::Str(s) => s.clone(),
                other => other.to_slt_string(),
            };
            // `DateT` / `TimeT` hold the ordered day / second count; the
            // storage layer already renders the text form, so the count comes
            // from the same conversion the block writer uses.
            Some(Bound::Day(ordinal_of(variant, &text)?))
        }
        variant::DATE_TIME | variant::TIMESTAMP => {
            let text = match value {
                Value::Str(s) => s.clone(),
                other => other.to_slt_string(),
            };
            Some(Bound::Stamp(
                ordinal_of(variant::DATE, &text)?,
                ordinal_of(variant::TIME, &text)?,
            ))
        }
        variant::VARCHAR => {
            let text = match value {
                Value::Str(s) => s.as_bytes().to_vec(),
                other => other.to_slt_string().into_bytes(),
            };
            Some(Bound::Text(Text::lowest().truncate(&text)))
        }
        _ => None,
    }
}

/// The raw bytes of a string value, for the comparisons `SetToTruncate`'s
/// callers make: they compare the **full** input string against the stored
/// (truncated) view.
fn text_bytes(value: &Value) -> Vec<u8> {
    match value {
        Value::Str(s) => s.as_bytes().to_vec(),
        other => other.to_slt_string().into_bytes(),
    }
}

/// The ordered integer behind a `DATE`/`TIME` column's text.
fn ordinal_of(variant: u8, text: &str) -> Option<i32> {
    let text = text.trim();
    if variant == variant::TIME {
        let (h, m, s) = split_time(text)?;
        return Some(h as i32 * 3600 + m as i32 * 60 + s as i32);
    }
    let date = text.split([' ', 'T']).next().unwrap_or(text);
    let mut parts = date.split('-');
    let y: i32 = parts.next()?.parse().ok()?;
    let m: i32 = parts.next()?.parse().ok()?;
    let d: i32 = parts.next()?.parse().ok()?;
    Some(days_from_civil(y, m, d))
}

/// `days_from_civil` (Howard Hinnant): days since 1970-01-01.
fn days_from_civil(y: i32, m: i32, d: i32) -> i32 {
    let y = if m <= 2 { y - 1 } else { y };
    let era = if y >= 0 { y } else { y - 399 } / 400;
    let yoe = y - era * 400;
    let mp = (m + 9) % 12;
    let doy = (153 * mp + 2) / 5 + d - 1;
    let doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    era * 146097 + doe - 719468
}

fn split_time(text: &str) -> Option<(u32, u32, u32)> {
    let time = text.split([' ', 'T']).nth(1).unwrap_or(text);
    let mut parts = time.split(':');
    Some((
        parts.next()?.parse().ok()?,
        parts.next()?.parse().ok()?,
        parts
            .next()
            .unwrap_or("0")
            .split('.')
            .next()?
            .parse()
            .ok()?,
    ))
}

/// `<` for two bounds of one variant (`InnerMinMaxDataFilterT`'s comparison;
/// text compares the truncated views, as `GetStringView()` does).
fn less(variant: u8, a: &Bound, b: &Bound) -> bool {
    match (variant, a, b) {
        (_, Bound::Integral(x), Bound::Integral(y)) => x < y,
        (_, Bound::Real(x), Bound::Real(y)) => x < y,
        (_, Bound::Day(x), Bound::Day(y)) => x < y,
        (_, Bound::Stamp(x0, x1), Bound::Stamp(y0, y1)) => (x0, x1) < (y0, y1),
        (_, Bound::Text(x), Bound::Text(y)) => x.view() < y.view(),
        _ => false,
    }
}

fn encode(variant: u8, bound: &Bound, out: &mut Vec<u8>) {
    match bound {
        Bound::Integral(v) => match variant {
            variant::TINY_INT => out.push(*v as u8),
            variant::SMALL_INT => out.extend_from_slice(&(*v as i16).to_le_bytes()),
            variant::INTEGER => out.extend_from_slice(&(*v as i32).to_le_bytes()),
            variant::BIG_INT => out.extend_from_slice(&v.to_le_bytes()),
            // `HugeIntT`: (upper, lower), each an i64.
            variant::HUGE_INT => {
                out.extend_from_slice(&0i64.to_le_bytes());
                out.extend_from_slice(&v.to_le_bytes());
            }
            _ => {}
        },
        Bound::Real(v) => {
            if variant == variant::FLOAT {
                out.extend_from_slice(&(*v as f32).to_le_bytes());
            } else {
                out.extend_from_slice(&v.to_le_bytes());
            }
        }
        Bound::Day(v) => out.extend_from_slice(&v.to_le_bytes()),
        Bound::Stamp(d, t) => {
            out.extend_from_slice(&d.to_le_bytes());
            out.extend_from_slice(&t.to_le_bytes());
        }
        Bound::Text(text) => {
            out.extend_from_slice(&text.bytes);
            out.push(text.len);
        }
    }
}

/// The "no values" sentinels: `numeric_limits<InnerValueType>::max()` for the
/// min and `lowest()` for the max, exactly as the reference starts them.
fn initial(variant: u8) -> (Bound, Bound) {
    match variant {
        variant::TINY_INT => (
            Bound::Integral(i8::MAX as i64),
            Bound::Integral(i8::MIN as i64),
        ),
        variant::SMALL_INT => (
            Bound::Integral(i16::MAX as i64),
            Bound::Integral(i16::MIN as i64),
        ),
        variant::INTEGER => (
            Bound::Integral(i32::MAX as i64),
            Bound::Integral(i32::MIN as i64),
        ),
        variant::BIG_INT | variant::HUGE_INT => {
            (Bound::Integral(i64::MAX), Bound::Integral(i64::MIN))
        }
        variant::DATE | variant::TIME => (Bound::Day(i32::MAX), Bound::Day(i32::MIN)),
        variant::DATE_TIME | variant::TIMESTAMP => (
            Bound::Stamp(i32::MAX, i32::MAX),
            Bound::Stamp(i32::MIN, i32::MIN),
        ),
        variant::FLOAT => (Bound::Real(f32::MAX as f64), Bound::Real(f32::MIN as f64)),
        variant::DOUBLE => (Bound::Real(f64::MAX), Bound::Real(f64::MIN)),
        _ => (Bound::Text(Text::highest()), Bound::Text(Text::lowest())),
    }
}

/// The prefilter of one segment or block: per column, its variant and the
/// running min/max.
///
/// Built incrementally so a segment's filter does not hold its rows: the block
/// loop adds each block and merges it into the segment's.
pub struct Filters {
    columns: Vec<Option<(u8, Bound, Bound)>>,
    types: usize,
}

impl Filters {
    pub fn new(types: &[String]) -> Self {
        Filters {
            columns: types
                .iter()
                .map(|t| {
                    variant_of(t).map(|v| {
                        let (min, max) = initial(v);
                        (v, min, max)
                    })
                })
                .collect(),
            types: types.len(),
        }
    }

    pub fn add_row(&mut self, cells: &[Value]) {
        for (column, slot) in self.columns.iter_mut().enumerate() {
            let Some((variant, min, max)) = slot else {
                continue;
            };
            let Some(value) = cells.get(column) else {
                continue;
            };
            if matches!(value, Value::Null) {
                continue;
            }
            if *variant == variant::VARCHAR {
                // `UpdateMin(InnerMinMaxDataFilterVarcharType&, const std::string&)`:
                // the comparison uses the whole string, the store truncates the
                // struct that is already there - so its unused bytes keep the
                // sentinel filler (0x7f for the min, 0x00 for the max).
                let text = text_bytes(value);
                if let Bound::Text(current) = *min {
                    if text.as_slice() < current.view() {
                        *min = Bound::Text(current.truncate(&text));
                    }
                }
                if let Bound::Text(current) = *max {
                    if text.as_slice() > current.view() {
                        *max = Bound::Text(current.truncate(&text));
                    }
                }
                continue;
            }
            let Some(bound) = bound_of(*variant, value) else {
                continue;
            };
            if less(*variant, &bound, min) {
                *min = bound;
            }
            if less(*variant, max, &bound) {
                *max = bound;
            }
        }
    }

    /// Folds another filter's min/max into this one (`UpdateMin`/`UpdateMax`
    /// over the block values, as the reference merges block min/max into the
    /// segment's).
    pub fn merge(&mut self, other: &Filters) {
        for (column, slot) in self.columns.iter_mut().enumerate() {
            let Some((variant, min, max)) = slot else {
                continue;
            };
            let Some((_, other_min, other_max)) =
                other.columns.get(column).and_then(|c| c.as_ref())
            else {
                continue;
            };
            if less(*variant, other_min, min) {
                *min = *other_min;
            }
            if less(*variant, max, other_max) {
                *max = *other_max;
            }
        }
    }

    /// The record `seg|…|fast_rough_filter` / `blk|…|fast_rough_filter` holds.
    ///
    /// `build_time` is the segment's begin ts (the creating transaction's
    /// commit ts minus one).
    pub fn encode(&self, build_time: u64) -> Vec<u8> {
        let mut minmax: Vec<u8> = Vec::new();
        for slot in &self.columns {
            match slot {
                None => minmax.push(variant::MONOSTATE),
                Some((variant, min, max)) => {
                    minmax.push(*variant);
                    encode(*variant, min, &mut minmax);
                    encode(*variant, max, &mut minmax);
                }
            }
        }
        let column_count = self.types as u32;
        // Every column gets a filter object (the vector is sized to the column
        // count); `built = 0` because only a column declared with the
        // bloom-filter option is ever built, and no gate's schema uses it.
        let probabilistic_size = 8 + 2 * column_count as usize;
        let minmax_size = 8 + minmax.len();
        let total = 12 + probabilistic_size + minmax_size;
        let mut out = Vec::with_capacity(total);
        out.extend_from_slice(&(total as u32).to_le_bytes());
        out.extend_from_slice(&build_time.to_le_bytes());
        out.extend_from_slice(&(probabilistic_size as u32).to_le_bytes());
        out.extend_from_slice(&column_count.to_le_bytes());
        for _ in 0..column_count {
            out.push(1); // the filter object exists
            out.push(0); // nothing was built into it
        }
        out.extend_from_slice(&(minmax_size as u32).to_le_bytes());
        out.extend_from_slice(&column_count.to_le_bytes());
        out.extend_from_slice(&minmax);
        out
    }
}

/// [`Filters`] over one set of rows (the tests' entry point).
#[cfg(test)]
pub fn build(types: &[String], rows: &[(usize, Vec<Value>)], build_time: u64) -> Vec<u8> {
    let mut filters = Filters::new(types);
    for (_, cells) in rows {
        filters.add_row(cells);
    }
    filters.encode(build_time)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn schema() -> Vec<String> {
        vec![
            "INTEGER".to_string(),
            "VARCHAR".to_string(),
            "VARCHAR".to_string(),
            "EMBEDDING(FLOAT,4)".to_string(),
            "SPARSE(FLOAT,INT)".to_string(),
            "FLOAT".to_string(),
        ]
    }

    fn rows(count: i64) -> Vec<(usize, Vec<Value>)> {
        let words = ["the", "quick", "brown", "fox", "jumps"];
        (0..count)
            .map(|i| {
                let k = i as usize;
                let body = format!(
                    "{} doc{}",
                    words[k % 5..]
                        .iter()
                        .chain(words[..k % 5].iter())
                        .cloned()
                        .collect::<Vec<_>>()
                        .join(" "),
                    i
                );
                (
                    i as usize,
                    vec![
                        Value::Int(i),
                        Value::Str(format!("title-{i}")),
                        Value::Str(body),
                        Value::Str("[0.0,0.1,0.2,0.3]".to_string()),
                        Value::Str("[0:0.5,1:1.0]".to_string()),
                        Value::Float(i as f64 * 0.5),
                    ],
                )
            })
            .collect()
    }

    /// The reference's record for segment 0 of the six-column, 100-row import
    /// (captured with a catalog dump: `seg|1|0|0|fast_rough_filter`, 130 bytes,
    /// build time 21 = the import's commit ts 22 minus one).
    const REFERENCE_SEGMENT_ZERO: &str = "820000001500000000000000140000000600000001000100010001000100010062000000060000000300000000630000000a7469746c652d307f7f7f7f7f7f7f7f7f077469746c652d39390000000000000000080a62726f776e20666f78206a756d7073201074686520717569636b2062726f776e201000000b0000000000004642";

    #[test]
    fn the_record_matches_the_reference_bytes() {
        let record = build(&schema(), &rows(100), 21);
        let expected: Vec<u8> = (0..REFERENCE_SEGMENT_ZERO.len() / 2)
            .map(|i| u8::from_str_radix(&REFERENCE_SEGMENT_ZERO[i * 2..i * 2 + 2], 16).unwrap())
            .collect();
        assert_eq!(record, expected);
    }

    /// The block's record is the same shape over its own rows; a table without
    /// any min-max-capable column only carries the header and the empty halves.
    #[test]
    fn a_vector_only_table_has_no_minmax() {
        let types = vec!["EMBEDDING(FLOAT,4)".to_string()];
        let record = build(&types, &[(0, vec![Value::Str("[0.0]".into())])], 7);
        // total = 12 + (8 + 2) + (8 + 1)
        assert_eq!(u32::from_le_bytes(record[0..4].try_into().unwrap()), 31);
        assert_eq!(
            record[12..].to_vec(),
            vec![10, 0, 0, 0, 1, 0, 0, 0, 1, 0, 9, 0, 0, 0, 1, 0, 0, 0, 0]
        );
        assert_eq!(u64::from_le_bytes(record[4..12].try_into().unwrap()), 7);
    }
}
