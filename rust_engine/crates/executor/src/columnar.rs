//! A typed, column-major result set.
//!
//! The reference's DataBlock holds one ColumnVector per column: a contiguous,
//! typed buffer plus a null bitmap. The thrift writer memcpy's it into the
//! ColumnField chunk it sends, so a scan never builds a per-row object.
//!
//! This is the same shape for the plain-projection fast path: the block reader
//! fills these buffers column by column, and the protocol layer packs them
//! without touching a Value per cell. Rows are materialised only where a caller
//! still needs them (an operator, a test, the CLI).

use expression::Value;

/// A text column: every cell concatenated, with the end offset of each.
///
/// A `Vec<String>` would allocate once per cell; the wire wants a length prefix
/// and the bytes, so one buffer plus offsets is both smaller and what the
/// reference's varchar column already is.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct TextColumn {
    data: String,
    ends: Vec<u32>,
}

impl TextColumn {
    pub fn push(&mut self, cell: &str) {
        self.data.push_str(cell);
        self.ends.push(self.data.len() as u32);
    }

    pub fn push_owned(&mut self, cell: String) {
        self.data.push_str(&cell);
        self.ends.push(self.data.len() as u32);
    }

    pub fn len(&self) -> usize {
        self.ends.len()
    }

    pub fn is_empty(&self) -> bool {
        self.ends.is_empty()
    }

    pub fn get(&self, i: usize) -> &str {
        let end = match self.ends.get(i) {
            Some(end) => *end as usize,
            None => return "",
        };
        let start = if i == 0 { 0 } else { self.ends[i - 1] as usize };
        self.data.get(start..end).unwrap_or("")
    }

    pub fn bytes(&self) -> &str {
        &self.data
    }
}

/// One column of a Batch.
#[derive(Debug, Clone, PartialEq)]
pub enum ColumnData {
    /// TINYINT .. BIGINT: the width is recovered by the packer from the
    /// column's declared type.
    Int(Vec<i64>),
    /// FLOAT, DOUBLE, FLOAT16, BFLOAT16.
    Float(Vec<f64>),
    Bool(Vec<bool>),
    /// Everything whose cell is text on the wire: varchar, json, date, time,
    /// datetime.
    Text(TextColumn),
}

impl ColumnData {
    pub fn len(&self) -> usize {
        match self {
            ColumnData::Int(v) => v.len(),
            ColumnData::Float(v) => v.len(),
            ColumnData::Bool(v) => v.len(),
            ColumnData::Text(v) => v.len(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    /// Appends a value, keeping the column's type. NULL keeps the slot; the
    /// caller records it in the column's validity bitmap.
    pub fn push_value(&mut self, value: &Value) {
        match (self, value) {
            (ColumnData::Int(v), Value::Int(i)) => v.push(*i),
            (ColumnData::Int(v), Value::Float(f)) => v.push(*f as i64),
            (ColumnData::Int(v), Value::Bool(b)) => v.push(*b as i64),
            (ColumnData::Int(v), _) => v.push(0),
            (ColumnData::Float(v), Value::Float(f)) => v.push(*f),
            (ColumnData::Float(v), Value::Int(i)) => v.push(*i as f64),
            (ColumnData::Float(v), _) => v.push(0.0),
            (ColumnData::Bool(v), Value::Bool(b)) => v.push(*b),
            (ColumnData::Bool(v), _) => v.push(false),
            (ColumnData::Text(v), Value::Str(s)) => v.push(s),
            (ColumnData::Text(v), other) => {
                let text = other.to_slt_string();
                v.push(&text);
            }
        }
    }

    /// Appends a text cell, moving the string when the column is text.
    pub fn push_text(&mut self, s: String) {
        match self {
            ColumnData::Text(v) => v.push_owned(s),
            other => other.push_value(&Value::Str(s)),
        }
    }

    pub fn value_at(&self, i: usize) -> Value {
        match self {
            ColumnData::Int(v) => v.get(i).map(|i| Value::Int(*i)).unwrap_or(Value::Null),
            ColumnData::Float(v) => v.get(i).map(|f| Value::Float(*f)).unwrap_or(Value::Null),
            ColumnData::Bool(v) => v.get(i).map(|b| Value::Bool(*b)).unwrap_or(Value::Null),
            ColumnData::Text(v) => match v.ends.get(i) {
                Some(_) => Value::Str(v.get(i).to_string()),
                None => Value::Null,
            },
        }
    }

    /// The rows whose value equals `value` (a NULL never matches).
    pub fn equality_mask(&self, valid: &[bool], value: &Value) -> Vec<bool> {
        let eq = |v: &Value| v.partial_cmp_value(value) == Some(std::cmp::Ordering::Equal);
        match self {
            ColumnData::Int(v) => v
                .iter()
                .enumerate()
                .map(|(i, x)| valid.get(i).copied().unwrap_or(false) && eq(&Value::Int(*x)))
                .collect(),
            ColumnData::Float(v) => v
                .iter()
                .enumerate()
                .map(|(i, x)| valid.get(i).copied().unwrap_or(false) && eq(&Value::Float(*x)))
                .collect(),
            ColumnData::Bool(v) => v
                .iter()
                .enumerate()
                .map(|(i, x)| valid.get(i).copied().unwrap_or(false) && eq(&Value::Bool(*x)))
                .collect(),
            ColumnData::Text(v) => (0..v.len())
                .map(|i| {
                    valid.get(i).copied().unwrap_or(false) && eq(&Value::Str(v.get(i).to_string()))
                })
                .collect(),
        }
    }

    /// Keeps the rows in `keep` (a mask over the current rows), compacting the
    /// buffer.
    pub fn retain(&mut self, keep: &[bool]) {
        match self {
            ColumnData::Int(v) => {
                let mut at = 0usize;
                v.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
            }
            ColumnData::Float(v) => {
                let mut at = 0usize;
                v.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
            }
            ColumnData::Bool(v) => {
                let mut at = 0usize;
                v.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
            }
            ColumnData::Text(v) => {
                let mut data = String::with_capacity(v.data.len());
                let mut ends = Vec::with_capacity(v.ends.len());
                for (i, _) in v.ends.iter().enumerate() {
                    if keep.get(i).copied().unwrap_or(false) {
                        data.push_str(v.get(i));
                        ends.push(data.len() as u32);
                    }
                }
                v.data = data;
                v.ends = ends;
            }
        }
    }
}

/// A column-major result set: the columns, their values and, per cell, whether
/// it is present (false is SQL NULL).
#[derive(Debug, Clone, PartialEq)]
pub struct Batch {
    pub columns: Vec<String>,
    pub data: Vec<ColumnData>,
    pub valid: Vec<Vec<bool>>,
    pub row_count: usize,
}

impl Batch {
    /// The rows, for the callers that work on Value rows.
    pub fn rows(&self) -> Vec<Vec<Value>> {
        (0..self.row_count)
            .map(|r| {
                self.data
                    .iter()
                    .enumerate()
                    .map(|(c, column)| {
                        if self.valid.get(c).and_then(|v| v.get(r)).copied() == Some(false) {
                            Value::Null
                        } else {
                            column.value_at(r)
                        }
                    })
                    .collect()
            })
            .collect()
    }
}
