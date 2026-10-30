//! A typed, column-major result set.
//!
//! The reference's DataBlock holds one ColumnVector per column: a contiguous,
//! typed buffer plus a null bitmap. The thrift writer memcpy's it into the
//! ColumnField chunk it sends, so a scan never builds a per-row object.
//!
//! This is the same shape for the plain-projection fast path: the block reader
//! fills these buffers column by column, and the protocol layer packs them
//! without touching a Value per cell. Everything else (operators, filters,
//! expressions) still runs on Value rows, which is why this exists only as an
//! *output* form: Batch::rows materialises rows for the callers that need them.

use infinity_expression::Value;

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
    Text(Vec<String>),
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

    /// Appends a value, keeping the column's type. A value of another kind is
    /// rendered to text, which is what the wire would carry for it anyway.
    pub fn push_value(&mut self, value: &Value) {
        match (self, value) {
            (ColumnData::Int(v), Value::Int(i)) => v.push(*i),
            (ColumnData::Int(v), Value::Float(f)) => v.push(*f as i64),
            (ColumnData::Int(v), Value::Bool(b)) => v.push(*b as i64),
            // NULL keeps the slot; the caller records it in the column's
            // validity bitmap.
            (ColumnData::Int(v), _) => v.push(0),
            (ColumnData::Float(v), Value::Float(f)) => v.push(*f),
            (ColumnData::Float(v), Value::Int(i)) => v.push(*i as f64),
            (ColumnData::Float(v), _) => v.push(0.0),
            (ColumnData::Bool(v), Value::Bool(b)) => v.push(*b),
            (ColumnData::Bool(v), _) => v.push(false),
            (ColumnData::Text(v), Value::Str(s)) => v.push(s.clone()),
            (ColumnData::Text(v), other) => v.push(other.to_slt_string()),
        }
    }

    /// Appends a text cell, moving the string when the column is text (a text
    /// column is the common case, and a clone per cell is what a varchar
    /// projection cannot afford).
    pub fn push_text(&mut self, s: String) {
        match self {
            ColumnData::Text(v) => v.push(s),
            other => other.push_value(&Value::Str(s)),
        }
    }

    pub fn value_at(&self, i: usize) -> Value {
        match self {
            ColumnData::Int(v) => v.get(i).map(|i| Value::Int(*i)).unwrap_or(Value::Null),
            ColumnData::Float(v) => v.get(i).map(|f| Value::Float(*f)).unwrap_or(Value::Null),
            ColumnData::Bool(v) => v.get(i).map(|b| Value::Bool(*b)).unwrap_or(Value::Null),
            ColumnData::Text(v) => v
                .get(i)
                .map(|s| Value::Str(s.clone()))
                .unwrap_or(Value::Null),
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