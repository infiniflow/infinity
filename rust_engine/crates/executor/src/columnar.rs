//! A typed, column-major result block.
//!
//! The reference's `DataBlock` holds one `ColumnVector` per column: a
//! contiguous, typed buffer plus a null bitmap. The thrift writer memcpy's it
//! into the `ColumnField` chunk it sends, so a scan never builds a per-row
//! object.
//!
//! This is that shape. A [`Batch`] is what `QueryResult::Rows` carries and what
//! the protocol layer packs; [`ColumnData`] is one column of it. Rows are
//! materialised only where a caller still needs them (an operator, a test, the
//! CLI, the `.slt` runner, the HTTP and postgres writers).

use expression::Value;

/// A validity bitmap: one bit per row, set means the cell is present.
///
/// One bit per row instead of a `bool` per row keeps a wide result set eight
/// times smaller and lets `retain`/`permute` move whole words.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct Bitmap {
    words: Vec<u64>,
    len: usize,
}

impl Bitmap {
    /// An empty bitmap.
    pub fn new() -> Self {
        Self::default()
    }

    /// `len` bits, all `valid`.
    pub fn all(len: usize, valid: bool) -> Self {
        let mut bitmap = Self::new();
        for _ in 0..len {
            bitmap.push(valid);
        }
        bitmap
    }

    /// Appends one bit.
    pub fn push(&mut self, valid: bool) {
        if self.len % 64 == 0 {
            self.words.push(0);
        }
        if valid {
            self.words[self.len / 64] |= 1 << (self.len % 64);
        }
        self.len += 1;
    }

    /// Overwrites bit `i` (already pushed).
    pub fn set(&mut self, i: usize, valid: bool) {
        if i >= self.len {
            return;
        }
        if valid {
            self.words[i / 64] |= 1 << (i % 64);
        } else {
            self.words[i / 64] &= !(1 << (i % 64));
        }
    }

    /// Whether cell `i` is present. A bitmap that never recorded `i` (a column
    /// before any row was appended) says yes.
    pub fn get(&self, i: usize) -> bool {
        if i >= self.len {
            return true;
        }
        self.words[i / 64] & (1 << (i % 64)) != 0
    }

    pub fn len(&self) -> usize {
        self.len
    }

    pub fn is_empty(&self) -> bool {
        self.len == 0
    }

    /// Whether every recorded bit is set.
    pub fn all_valid(&self) -> bool {
        if self.len == 0 {
            return true;
        }
        let full = self.len / 64;
        if !(0..full).all(|w| self.words[w] == u64::MAX) {
            return false;
        }
        let rest = self.len % 64;
        if rest == 0 {
            return true;
        }
        let mask = (1u64 << rest) - 1;
        self.words[full] & mask == mask
    }

    /// The bits as `bool`s, for the thrift writer's `bitmasks` field.
    pub fn to_vec(&self) -> Vec<bool> {
        (0..self.len).map(|i| self.get(i)).collect()
    }

    /// Keeps the bits whose `keep` entry is true.
    pub fn retain(&mut self, keep: &[bool]) {
        let mut out = Self::new();
        for i in 0..self.len {
            if keep.get(i).copied().unwrap_or(false) {
                out.push(self.get(i));
            }
        }
        *self = out;
    }

    /// The bits reordered by `order`.
    pub fn permute(&self, order: &[usize]) -> Self {
        let mut out = Self::new();
        for i in order {
            out.push(self.get(*i));
        }
        out
    }
}

/// A text column: every cell concatenated, with the end offset of each.
///
/// A `Vec<String>` would allocate once per cell; the wire wants a length prefix
/// and the bytes, so one buffer plus offsets is both smaller and what the
/// reference's varchar column already is.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct Utf8Column {
    data: Vec<u8>,
    /// `offsets[i]` is the end of cell `i`; the start is `offsets[i - 1]`.
    offsets: Vec<u32>,
}

impl Utf8Column {
    pub fn push(&mut self, cell: &str) {
        self.data.extend_from_slice(cell.as_bytes());
        self.offsets.push(self.data.len() as u32);
    }

    pub fn push_owned(&mut self, cell: String) {
        self.data.extend_from_slice(cell.as_bytes());
        self.offsets.push(self.data.len() as u32);
    }

    pub fn len(&self) -> usize {
        self.offsets.len()
    }

    pub fn is_empty(&self) -> bool {
        self.offsets.is_empty()
    }

    pub fn get(&self, i: usize) -> &str {
        let Some(end) = self.offsets.get(i).copied() else {
            return "";
        };
        let start = if i == 0 { 0 } else { self.offsets[i - 1] };
        std::str::from_utf8(&self.data[start as usize..end as usize]).unwrap_or("")
    }

    pub fn bytes(&self) -> &[u8] {
        &self.data
    }

    pub fn ends(&self) -> &[u32] {
        &self.offsets
    }

    fn retain(&mut self, keep: &[bool]) {
        let mut data = Vec::with_capacity(self.data.len());
        let mut offsets: Vec<u32> = Vec::with_capacity(self.offsets.len());
        for i in 0..self.offsets.len() {
            if keep.get(i).copied().unwrap_or(false) {
                data.extend_from_slice(self.get(i).as_bytes());
                offsets.push(data.len() as u32);
            }
        }
        self.data = data;
        self.offsets = offsets;
    }

    fn permute(&self, order: &[usize]) -> Self {
        let mut out = Self::default();
        for i in order {
            out.push(self.get(*i));
        }
        out
    }
}

/// The shape of a [`ColumnData`], without borrowing its buffer.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Kind {
    Empty,
    Int32,
    Int64,
    Float64,
    Bool,
    Utf8,
}

/// The bitmap of a column that has no cells yet.
static EMPTY_BITMAP: Bitmap = Bitmap {
    words: Vec::new(),
    len: 0,
};

/// One column of a [`Batch`]: a typed buffer plus its validity bitmap.
#[derive(Debug, Clone, PartialEq)]
pub enum ColumnData {
    /// No value yet, so no type either: the first `push_value` decides it.
    ///
    /// An operator allocates its output chunk before it knows what the
    /// expression produces (`SELECT x AND y` is a bool, `a + b` an integer), so
    /// the buffer starts here and adopts the first cell's type - which is how a
    /// boolean result keeps rendering as `true` instead of `1`.
    Empty {
        nulls: usize,
    },
    /// TINYINT .. INTEGER, and the 32-bit unsigned types.
    Int32 {
        values: Vec<i32>,
        valid: Bitmap,
    },
    /// BIGINT and the 64-bit unsigned types; also the engine's own `i64`.
    Int64 {
        values: Vec<i64>,
        valid: Bitmap,
    },
    /// FLOAT, DOUBLE, FLOAT16, BFLOAT16.
    Float64 {
        values: Vec<f64>,
        valid: Bitmap,
    },
    Bool {
        values: Vec<bool>,
        valid: Bitmap,
    },
    /// Everything whose cell is text on the wire: varchar, json, date, time,
    /// datetime.
    Utf8 {
        text: Utf8Column,
        valid: Bitmap,
    },
}

impl ColumnData {
    /// An empty buffer for a declared column type. Unknown and var-length types
    /// are text, which is how the wire carries them.
    pub fn new(declared: &str) -> Self {
        match declared_base(declared).as_str() {
            "BOOL" | "BOOLEAN" => ColumnData::Bool {
                values: Vec::new(),
                valid: Bitmap::new(),
            },
            "TINYINT" | "SMALLINT" | "INT" | "INTEGER" | "UINT8" | "UINT16" | "UINT32" => {
                ColumnData::Int32 {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                }
            }
            "BIGINT" | "UINT64" => ColumnData::Int64 {
                values: Vec::new(),
                valid: Bitmap::new(),
            },
            "FLOAT" | "FLOAT32" | "DOUBLE" | "FLOAT64" | "FLOAT16" | "BFLOAT16" => {
                ColumnData::Float64 {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                }
            }
            _ => ColumnData::Utf8 {
                text: Utf8Column::default(),
                valid: Bitmap::new(),
            },
        }
    }

    /// A column built from `values`, typed from what they hold (the projection
    /// of an expression: text if any cell is text, float if any is a float,
    /// otherwise `i64`).
    pub fn from_values(values: &[Value]) -> ColumnData {
        let rows: Vec<Vec<Value>> = values.iter().map(|v| vec![v.clone()]).collect();
        let mut column = column_for_rows(&rows, 0);
        for value in values {
            column.push_value(value);
        }
        column
    }

    /// A text column built from cells (`None` = SQL NULL), the shape the
    /// highlight pass rebuilds a column with.
    pub fn text_from_cells(cells: &[Option<String>]) -> ColumnData {
        let mut text = Utf8Column::default();
        let mut valid = Bitmap::new();
        for cell in cells {
            match cell {
                Some(cell) => text.push(cell),
                None => text.push(""),
            }
            valid.push(cell.is_some());
        }
        ColumnData::Utf8 { text, valid }
    }

    /// An untyped column: the first value pushed decides the type (see the
    /// [`ColumnData::Empty`] variant).
    pub fn empty() -> Self {
        ColumnData::Empty { nulls: 0 }
    }

    /// An empty buffer of the same shape as `self` (a typed column when `self`
    /// is typed, untyped when it is not).
    pub fn same_shape_empty(&self) -> ColumnData {
        match self {
            ColumnData::Empty { .. } => ColumnData::empty(),
            ColumnData::Int32 { .. } => ColumnData::Int32 {
                values: Vec::new(),
                valid: Bitmap::new(),
            },
            ColumnData::Int64 { .. } => ColumnData::Int64 {
                values: Vec::new(),
                valid: Bitmap::new(),
            },
            ColumnData::Float64 { .. } => ColumnData::Float64 {
                values: Vec::new(),
                valid: Bitmap::new(),
            },
            ColumnData::Bool { .. } => ColumnData::Bool {
                values: Vec::new(),
                valid: Bitmap::new(),
            },
            ColumnData::Utf8 { .. } => ColumnData::Utf8 {
                text: Utf8Column::default(),
                valid: Bitmap::new(),
            },
        }
    }

    /// An empty buffer of the same shape as `column` (see
    /// [`ColumnData::same_shape_empty`]).
    pub fn empty_like(column: &ColumnData) -> ColumnData {
        column.same_shape_empty()
    }

    fn kind(&self) -> Kind {
        match self {
            ColumnData::Empty { .. } => Kind::Empty,
            ColumnData::Int32 { .. } => Kind::Int32,
            ColumnData::Int64 { .. } => Kind::Int64,
            ColumnData::Float64 { .. } => Kind::Float64,
            ColumnData::Bool { .. } => Kind::Bool,
            ColumnData::Utf8 { .. } => Kind::Utf8,
        }
    }

    pub fn len(&self) -> usize {
        match self {
            ColumnData::Empty { nulls } => *nulls,
            ColumnData::Int32 { values, .. } => values.len(),
            ColumnData::Int64 { values, .. } => values.len(),
            ColumnData::Float64 { values, .. } => values.len(),
            ColumnData::Bool { values, .. } => values.len(),
            ColumnData::Utf8 { text, .. } => text.len(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    /// The column's validity bitmap.
    pub fn valid(&self) -> &Bitmap {
        match self {
            ColumnData::Empty { .. } => &EMPTY_BITMAP,
            ColumnData::Int32 { valid, .. } => valid,
            ColumnData::Int64 { valid, .. } => valid,
            ColumnData::Float64 { valid, .. } => valid,
            ColumnData::Bool { valid, .. } => valid,
            ColumnData::Utf8 { valid, .. } => valid,
        }
    }

    /// Appends a value, keeping the column's type and widening it when the value
    /// does not fit (an integer column that receives a float becomes a float
    /// column; anything that receives text becomes a text column). An untyped
    /// column adopts the type of the first value it sees.
    pub fn push_value(&mut self, value: &Value) {
        let pending = match self {
            ColumnData::Empty { nulls } => Some(*nulls),
            _ => None,
        };
        if let Some(nulls) = pending {
            let mut column = match value {
                Value::Null => {
                    *self = ColumnData::Empty { nulls: nulls + 1 };
                    return;
                }
                Value::Int(_) => ColumnData::Int64 {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                },
                Value::Float(_) => ColumnData::Float64 {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                },
                Value::Bool(_) => ColumnData::Bool {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                },
                Value::Str(_) => ColumnData::Utf8 {
                    text: Utf8Column::default(),
                    valid: Bitmap::new(),
                },
            };
            for _ in 0..nulls {
                column.push_null();
            }
            *self = column;
        }
        match value {
            Value::Null => self.push_null(),
            Value::Int(i) => self.push_int(*i),
            Value::Float(f) => self.push_float(*f),
            Value::Bool(b) => self.push_bool(*b),
            Value::Str(s) => self.push_text(s.clone()),
        }
    }

    /// [`ColumnData::push_value`] that also takes a text cell over instead of
    /// cloning it.
    pub fn push_value_owned(&mut self, value: Value) {
        match value {
            Value::Str(cell) => self.push_text(cell),
            other => self.push_value(&other),
        }
    }

    /// Appends SQL NULL, keeping the slot.
    pub fn push_null(&mut self) {
        match self {
            ColumnData::Empty { nulls } => {
                *nulls += 1;
            }
            ColumnData::Int32 { values, valid } => {
                values.push(0);
                valid.push(false);
            }
            ColumnData::Int64 { values, valid } => {
                values.push(0);
                valid.push(false);
            }
            ColumnData::Float64 { values, valid } => {
                values.push(0.0);
                valid.push(false);
            }
            ColumnData::Bool { values, valid } => {
                values.push(false);
                valid.push(false);
            }
            ColumnData::Utf8 { text, valid } => {
                text.push("");
                valid.push(false);
            }
        }
    }

    fn push_int(&mut self, i: i64) {
        match self {
            ColumnData::Int32 { values, valid } => {
                if let Ok(narrow) = i32::try_from(i) {
                    values.push(narrow);
                    valid.push(true);
                    return;
                }
            }
            ColumnData::Int64 { values, valid } => {
                values.push(i);
                valid.push(true);
                return;
            }
            ColumnData::Float64 { values, valid } => {
                values.push(i as f64);
                valid.push(true);
                return;
            }
            // A boolean column that receives an integer becomes an integer
            // column: storing `i != 0` would turn 5 into 1 and lose the value.
            // The widening below keeps the `true`s it already holds as `1`s.
            ColumnData::Bool { .. } => {}
            // A text column keeps the integer as text, the way `push_float`
            // does. Falling through to the widening below only recursed: a text
            // column has nothing left to widen to, so `push_int` called itself
            // forever (a stack overflow, not an error).
            ColumnData::Utf8 { .. } => {
                self.push_text(Value::Int(i).to_slt_string());
                return;
            }
            // An empty column adopts a type on its first value; `widen_to_int64`
            // below does that and keeps the nulls it already counted.
            ColumnData::Empty { .. } => {}
        }
        // Reached by an `Int32` the value does not fit in, by a boolean column
        // turning into an integer one, and by the empty column: all become
        // `Int64`, and the retry takes that arm above.
        self.widen_to_int64();
        self.push_int(i);
    }

    fn push_float(&mut self, f: f64) {
        match self {
            ColumnData::Float64 { values, valid } => {
                values.push(f);
                valid.push(true);
                return;
            }
            ColumnData::Int32 { .. } | ColumnData::Int64 { .. } | ColumnData::Bool { .. } => {}
            ColumnData::Empty { .. } => {
                *self = ColumnData::Float64 {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                };
            }
            ColumnData::Utf8 { .. } => {
                self.push_text(Value::Float(f).to_slt_string());
                return;
            }
        }
        self.widen_to_float();
        self.push_float(f);
    }

    fn push_bool(&mut self, b: bool) {
        match self {
            ColumnData::Bool { values, valid } => {
                values.push(b);
                valid.push(true);
            }
            ColumnData::Int32 { values, valid } => {
                values.push(i32::from(b));
                valid.push(true);
            }
            ColumnData::Int64 { values, valid } => {
                values.push(i64::from(b));
                valid.push(true);
            }
            ColumnData::Float64 { values, valid } => {
                values.push(if b { 1.0 } else { 0.0 });
                valid.push(true);
            }
            ColumnData::Empty { .. } => {
                *self = ColumnData::Bool {
                    values: Vec::new(),
                    valid: Bitmap::new(),
                };
                self.push_bool(b);
            }
            ColumnData::Utf8 { .. } => self.push_text(b.to_string()),
        }
    }

    /// Appends a text cell, moving the string when the column is text.
    pub fn push_text(&mut self, cell: String) {
        match self {
            ColumnData::Utf8 { text, valid } => {
                text.push_owned(cell);
                valid.push(true);
            }
            ColumnData::Empty { .. } => {
                *self = ColumnData::Utf8 {
                    text: Utf8Column::default(),
                    valid: Bitmap::new(),
                };
                self.push_text(cell);
            }
            _ => {
                self.widen_to_text();
                self.push_text(cell);
            }
        }
    }

    /// Appends one validity bit without a value.
    ///
    /// The block decoder writes a varchar cell straight into the text buffer
    /// (`decode_text_cell`) and only then learns whether the cell is the SQL
    /// NULL marker, so the bit trails the value. Overwriting the last bit would
    /// not work: [`Bitmap::set`] ignores an index the bitmap never recorded, so
    /// the bitmap would stay shorter than the buffer and every cell would read
    /// as present.
    pub fn push_valid(&mut self, valid: bool) {
        self.valid_mut().push(valid);
    }

    fn valid_mut(&mut self) -> &mut Bitmap {
        match self {
            ColumnData::Empty { .. } => panic!("an empty column has no bitmap"),
            ColumnData::Int32 { valid, .. } => valid,
            ColumnData::Int64 { valid, .. } => valid,
            ColumnData::Float64 { valid, .. } => valid,
            ColumnData::Bool { valid, .. } => valid,
            ColumnData::Utf8 { valid, .. } => valid,
        }
    }

    /// Rebuilds the column as text, keeping every existing cell.
    fn widen_to_text(&mut self) {
        let rows = self.len();
        let mut text = Utf8Column::default();
        let mut valid = Bitmap::new();
        for i in 0..rows {
            let present = self.valid().get(i);
            match self.value_at(i) {
                Value::Null => text.push(""),
                value => text.push(&value.to_slt_string()),
            }
            valid.push(present);
        }
        *self = ColumnData::Utf8 { text, valid };
    }

    /// Rebuilds the column as an `i64` buffer.
    fn widen_to_int64(&mut self) {
        let rows = self.len();
        let mut values = Vec::with_capacity(rows);
        let mut valid = Bitmap::new();
        for i in 0..rows {
            match self.value_at(i) {
                Value::Int(v) => values.push(v),
                Value::Float(f) => values.push(f as i64),
                Value::Bool(b) => values.push(i64::from(b)),
                _ => values.push(0),
            }
            valid.push(self.valid().get(i));
        }
        *self = ColumnData::Int64 { values, valid };
    }

    /// Rebuilds the column as an `f64` buffer.
    fn widen_to_float(&mut self) {
        let rows = self.len();
        let mut values = Vec::with_capacity(rows);
        let mut valid = Bitmap::new();
        for i in 0..rows {
            match self.value_at(i) {
                Value::Int(v) => values.push(v as f64),
                Value::Float(f) => values.push(f),
                Value::Bool(b) => values.push(if b { 1.0 } else { 0.0 }),
                _ => values.push(0.0),
            }
            valid.push(self.valid().get(i));
        }
        *self = ColumnData::Float64 { values, valid };
    }

    /// The cell at `i`, or SQL NULL when it is beyond the end or invalid.
    pub fn value_at(&self, i: usize) -> Value {
        if !self.valid().get(i) {
            return Value::Null;
        }
        match self {
            ColumnData::Empty { .. } => Value::Null,
            ColumnData::Int32 { values, .. } => values
                .get(i)
                .map(|v| Value::Int(i64::from(*v)))
                .unwrap_or(Value::Null),
            ColumnData::Int64 { values, .. } => {
                values.get(i).map(|v| Value::Int(*v)).unwrap_or(Value::Null)
            }
            ColumnData::Float64 { values, .. } => values
                .get(i)
                .map(|v| Value::Float(*v))
                .unwrap_or(Value::Null),
            ColumnData::Bool { values, .. } => values
                .get(i)
                .map(|v| Value::Bool(*v))
                .unwrap_or(Value::Null),
            ColumnData::Utf8 { text, .. } => {
                if i < text.len() {
                    Value::Str(text.get(i).to_string())
                } else {
                    Value::Null
                }
            }
        }
    }

    /// The rows whose value equals `value` (a NULL never matches).
    pub fn equality_mask(&self, value: &Value) -> Vec<bool> {
        let eq = |v: &Value| v.partial_cmp_value(value) == Some(std::cmp::Ordering::Equal);
        (0..self.len())
            .map(|i| self.valid().get(i) && eq(&self.value_at(i)))
            .collect()
    }

    /// Keeps the cells in `keep` (a mask over the current rows), compacting the
    /// buffer.
    pub fn retain(&mut self, keep: &[bool]) {
        match self {
            ColumnData::Empty { nulls } => {
                *nulls = keep.iter().filter(|k| **k).count();
            }
            ColumnData::Int32 { values, valid } => {
                let mut at = 0usize;
                values.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
                valid.retain(keep);
            }
            ColumnData::Int64 { values, valid } => {
                let mut at = 0usize;
                values.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
                valid.retain(keep);
            }
            ColumnData::Float64 { values, valid } => {
                let mut at = 0usize;
                values.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
                valid.retain(keep);
            }
            ColumnData::Bool { values, valid } => {
                let mut at = 0usize;
                values.retain(|_| {
                    let k = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    k
                });
                valid.retain(keep);
            }
            ColumnData::Utf8 { text, valid } => {
                text.retain(keep);
                valid.retain(keep);
            }
        }
    }

    /// The column reordered by `order`.
    pub fn permute(&self, order: &[usize]) -> ColumnData {
        let valid = self.valid().permute(order);
        match self {
            ColumnData::Empty { .. } => ColumnData::Empty { nulls: order.len() },
            ColumnData::Int32 { values, .. } => ColumnData::Int32 {
                values: order.iter().map(|i| values[*i]).collect(),
                valid,
            },
            ColumnData::Int64 { values, .. } => ColumnData::Int64 {
                values: order.iter().map(|i| values[*i]).collect(),
                valid,
            },
            ColumnData::Float64 { values, .. } => ColumnData::Float64 {
                values: order.iter().map(|i| values[*i]).collect(),
                valid,
            },
            ColumnData::Bool { values, .. } => ColumnData::Bool {
                values: order.iter().map(|i| values[*i]).collect(),
                valid,
            },
            ColumnData::Utf8 { text, .. } => ColumnData::Utf8 {
                text: text.permute(order),
                valid,
            },
        }
    }

    /// Appends every cell of `other` (the same column of a later block).
    pub fn append(&mut self, other: &ColumnData) {
        if self.kind() != other.kind() {
            // The two blocks disagree about the column; widening keeps the
            // values instead of dropping them.
            for i in 0..other.len() {
                let value = other.value_at(i);
                self.push_value(&value);
            }
            return;
        }
        match (self, other) {
            (ColumnData::Empty { nulls }, ColumnData::Empty { nulls: other_nulls }) => {
                *nulls += other_nulls;
            }
            (
                ColumnData::Int32 { values, valid },
                ColumnData::Int32 {
                    values: other_values,
                    valid: other_valid,
                },
            ) => {
                values.extend_from_slice(other_values);
                for i in 0..other_values.len() {
                    valid.push(other_valid.get(i));
                }
            }
            (
                ColumnData::Int64 { values, valid },
                ColumnData::Int64 {
                    values: other_values,
                    valid: other_valid,
                },
            ) => {
                values.extend_from_slice(other_values);
                for i in 0..other_values.len() {
                    valid.push(other_valid.get(i));
                }
            }
            (
                ColumnData::Float64 { values, valid },
                ColumnData::Float64 {
                    values: other_values,
                    valid: other_valid,
                },
            ) => {
                values.extend_from_slice(other_values);
                for i in 0..other_values.len() {
                    valid.push(other_valid.get(i));
                }
            }
            (
                ColumnData::Bool { values, valid },
                ColumnData::Bool {
                    values: other_values,
                    valid: other_valid,
                },
            ) => {
                values.extend_from_slice(other_values);
                for i in 0..other_values.len() {
                    valid.push(other_valid.get(i));
                }
            }
            (
                ColumnData::Utf8 { text, valid },
                ColumnData::Utf8 {
                    text: other_text,
                    valid: other_valid,
                },
            ) => {
                for i in 0..other_text.len() {
                    text.push(other_text.get(i));
                    valid.push(other_valid.get(i));
                }
            }
            _ => unreachable!("the kinds were compared above"),
        }
    }
}

/// A column-major block: the columns, their typed buffers and the row count.
///
/// This is the engine's `DataBlock` for results: `QueryResult::Rows` carries
/// one, the thrift writer packs it column by column, and everything that still
/// works on rows calls [`Batch::rows`].
#[derive(Debug, Clone, Default, PartialEq)]
pub struct Batch {
    pub columns: Vec<String>,
    pub data: Vec<ColumnData>,
    pub row_count: usize,
}

impl Batch {
    pub fn new(columns: Vec<String>, data: Vec<ColumnData>, row_count: usize) -> Self {
        Self {
            columns,
            data,
            row_count,
        }
    }

    /// An empty block with `width` untyped (integer) columns.
    pub fn empty(width: usize) -> Self {
        Self {
            columns: Vec::new(),
            data: (0..width).map(|_| ColumnData::empty()).collect(),
            row_count: 0,
        }
    }

    /// Builds a block from rows, picking each column's type from its values.
    pub fn from_rows(rows: Vec<Vec<Value>>) -> Self {
        let width = rows.iter().map(Vec::len).max().unwrap_or(0);
        let mut data: Vec<ColumnData> = (0..width).map(|c| column_for_rows(&rows, c)).collect();
        let row_count = rows.len();
        // The rows are consumed: a text cell moves into its buffer instead of
        // being cloned, which is one allocation per varchar cell.
        for row in rows {
            let width = row.len();
            for (c, value) in row.into_iter().enumerate() {
                data[c].push_value_owned(value);
            }
            for column in data.iter_mut().skip(width) {
                column.push_null();
            }
        }
        Self {
            columns: Vec::new(),
            data,
            row_count,
        }
    }

    pub fn len(&self) -> usize {
        self.row_count
    }

    pub fn is_empty(&self) -> bool {
        self.row_count == 0
    }

    pub fn width(&self) -> usize {
        self.data.len()
    }

    /// Appends one row; the block grows a column when the row is wider.
    pub fn push_row(&mut self, row: &[Value]) {
        while self.data.len() < row.len() {
            let mut column = ColumnData::empty();
            // The block already holds rows: the new column owes one cell per
            // row, or the cell pushed below lands on row 0.
            for _ in 0..self.row_count {
                column.push_null();
            }
            self.data.push(column);
        }
        for (c, value) in row.iter().enumerate() {
            self.data[c].push_value(value);
        }
        // A column the row did not fill still gets a cell.
        for column in self.data.iter_mut().skip(row.len()) {
            column.push_null();
        }
        self.row_count += 1;
    }

    /// Appends every row of `other`.
    pub fn append(&mut self, other: &Batch) {
        if other.row_count == 0 {
            return;
        }
        while self.data.len() < other.data.len() {
            let mut column = ColumnData::empty();
            // Same backfill as `push_row`: a column born next to existing rows
            // starts NULL in all of them, or `append` below would shift the new
            // cells up.
            for _ in 0..self.row_count {
                column.push_null();
            }
            self.data.push(column);
        }
        for (c, column) in other.data.iter().enumerate() {
            self.data[c].append(column);
        }
        for column in self.data.iter_mut().skip(other.data.len()) {
            for _ in 0..other.row_count {
                column.push_null();
            }
        }
        self.row_count += other.row_count;
    }

    /// Keeps the rows whose `keep` entry is true.
    pub fn retain(&mut self, keep: &[bool]) {
        for column in &mut self.data {
            column.retain(keep);
        }
        self.row_count = keep.iter().filter(|k| **k).count();
    }

    /// Reorders the rows.
    pub fn permute(&mut self, order: &[usize]) {
        for column in &mut self.data {
            *column = column.permute(order);
        }
    }

    /// Row `r`, materialised.
    pub fn row(&self, r: usize) -> Option<Vec<Value>> {
        if r >= self.row_count {
            return None;
        }
        Some(self.data.iter().map(|c| c.value_at(r)).collect())
    }

    /// Every row, materialised.
    pub fn rows(&self) -> Vec<Vec<Value>> {
        (0..self.row_count)
            .map(|r| self.data.iter().map(|c| c.value_at(r)).collect())
            .collect()
    }

    /// The rows, materialised, as an iterator.
    pub fn iter(&self) -> std::vec::IntoIter<Vec<Value>> {
        self.rows().into_iter()
    }

    /// Every row as display text (the CLI, `.slt`, HTTP and postgres exits).
    pub fn text_rows(&self) -> Vec<Vec<String>> {
        (0..self.row_count)
            .map(|r| {
                self.data
                    .iter()
                    .map(|c| c.value_at(r).to_slt_string())
                    .collect()
            })
            .collect()
    }
}

/// The declared type without its parameters (`VARCHAR(32)` → `VARCHAR`).
fn declared_base(declared: &str) -> String {
    declared
        .trim()
        .to_ascii_uppercase()
        .split('(')
        .next()
        .unwrap_or("")
        .split(',')
        .next()
        .unwrap_or("")
        .trim()
        .to_string()
}

/// The column type the rows ask for: text if any cell is text, float if any is
/// a float, bool only when every non-null cell is a bool, otherwise `i64`.
///
/// One pass, no per-column `Vec` of cell references: this runs over the whole
/// result set when the rows come out of the operator chain.
pub(crate) fn column_for_rows(rows: &[Vec<Value>], c: usize) -> ColumnData {
    let mut text = false;
    let mut float = false;
    let mut bools = true;
    let mut exact = true;
    let mut any = false;
    for cell in rows.iter().filter_map(|row| row.get(c)) {
        match cell {
            Value::Null => {}
            Value::Str(_) => {
                text = true;
                bools = false;
                any = true;
            }
            Value::Float(_) => {
                float = true;
                bools = false;
                any = true;
            }
            Value::Bool(_) => any = true,
            Value::Int(i) => {
                // An integer and a float in one column force the column to
                // `f64`, which holds `i` exactly only up to 2^53.
                if i.unsigned_abs() > (1u64 << 53) {
                    exact = false;
                }
                bools = false;
                any = true;
            }
        }
        if text {
            break;
        }
    }
    if text {
        return ColumnData::Utf8 {
            text: Utf8Column::default(),
            valid: Bitmap::new(),
        };
    }
    if float {
        // Mixed integers and floats whose integers do not survive `f64` exactly
        // go to text: `i as f64` would hand back a different number
        // (`9223372036854775807` becomes `...808`), and text is the only buffer
        // that keeps both kinds as they are.
        if exact {
            return ColumnData::Float64 {
                values: Vec::new(),
                valid: Bitmap::new(),
            };
        }
        return ColumnData::Utf8 {
            text: Utf8Column::default(),
            valid: Bitmap::new(),
        };
    }
    if any && bools {
        return ColumnData::Bool {
            values: Vec::new(),
            valid: Bitmap::new(),
        };
    }
    ColumnData::Int64 {
        values: Vec::new(),
        valid: Bitmap::new(),
    }
}

impl From<Vec<Vec<Value>>> for Batch {
    fn from(rows: Vec<Vec<Value>>) -> Self {
        Batch::from_rows(rows)
    }
}

impl IntoIterator for &Batch {
    type Item = Vec<Value>;
    type IntoIter = std::vec::IntoIter<Vec<Value>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

/// Lets a test compare a block with the rows it stands for.
impl PartialEq<Vec<Vec<Value>>> for Batch {
    fn eq(&self, other: &Vec<Vec<Value>>) -> bool {
        &self.rows() == other
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn bitmap_round_trips_and_survives_retain() {
        let mut bitmap = Bitmap::new();
        for i in 0..70 {
            bitmap.push(i % 3 != 0);
        }
        assert!(bitmap.get(1));
        assert!(!bitmap.get(3));
        assert!(bitmap.get(70), "past the end is valid");
        assert!(!bitmap.all_valid());
        let keep: Vec<bool> = (0..70).map(|i| i % 2 == 0).collect();
        bitmap.retain(&keep);
        assert_eq!(bitmap.len(), 35);
        // The kept cells were the even ones, so bit `k` is the old bit `2k`:
        // `0 % 3 == 0` is absent, `2 % 3` is present.
        assert!(!bitmap.get(0));
        assert!(bitmap.get(1));
        assert!(!bitmap.get(3));
    }

    #[test]
    fn a_text_column_is_one_buffer_plus_offsets() {
        let mut text = Utf8Column::default();
        text.push("ab");
        text.push_owned("cde".into());
        text.push("");
        assert_eq!(text.len(), 3);
        assert_eq!(text.get(0), "ab");
        assert_eq!(text.get(1), "cde");
        assert_eq!(text.get(2), "");
        assert_eq!(text.bytes(), b"abcde");
        assert_eq!(text.ends(), &[2, 5, 5]);
    }

    #[test]
    fn columns_widen_instead_of_truncating() {
        let mut column = ColumnData::empty();
        column.push_value(&Value::Int(1));
        column.push_value(&Value::Null);
        column.push_value(&Value::Float(2.5));
        assert_eq!(column.value_at(0), Value::Float(1.0));
        assert_eq!(column.value_at(1), Value::Null);
        assert_eq!(column.value_at(2), Value::Float(2.5));
        column.push_value(&Value::Str("x".into()));
        assert_eq!(column.value_at(3), Value::Str("x".into()));
        // The earlier cells were floats by then, so they render as floats.
        assert_eq!(column.value_at(0), Value::Str("1.000000".into()));
    }

    #[test]
    fn an_int32_column_widens_to_int64_when_the_value_does_not_fit() {
        let mut column = ColumnData::new("INT");
        column.push_value(&Value::Int(7));
        let big = i64::from(i32::MAX) + 1;
        column.push_value(&Value::Int(big));
        assert_eq!(column.value_at(0), Value::Int(7));
        assert_eq!(column.value_at(1), Value::Int(big));
    }

    #[test]
    fn a_column_born_late_keeps_the_rows_aligned() {
        // A wider row grows the block: the new column has to start with one
        // NULL per row already there, or the cell lands on row 0.
        let mut batch = Batch::empty(1);
        batch.push_row(&[Value::Int(1)]);
        batch.push_row(&[Value::Int(2)]);
        batch.push_row(&[Value::Int(3), Value::Str("c".into())]);
        assert_eq!(batch.row(0).unwrap(), vec![Value::Int(1), Value::Null]);
        assert_eq!(batch.row(1).unwrap(), vec![Value::Int(2), Value::Null]);
        assert_eq!(
            batch.row(2).unwrap(),
            vec![Value::Int(3), Value::Str("c".into())]
        );

        // The same for a wider block appended onto a narrower one.
        let mut wide = Batch::empty(2);
        wide.push_row(&[Value::Int(10), Value::Str("x".into())]);
        let mut narrow = Batch::empty(1);
        narrow.push_row(&[Value::Int(1)]);
        narrow.append(&wide);
        assert_eq!(narrow.row(0).unwrap(), vec![Value::Int(1), Value::Null]);
        assert_eq!(
            narrow.row(1).unwrap(),
            vec![Value::Int(10), Value::Str("x".into())]
        );
    }

    #[test]
    fn an_integer_keeps_its_value_in_a_boolean_column() {
        // `true`/`false` in the buffer, then an integer: the column turns into
        // integers, because `i != 0` would have stored 5 as 1.
        let mut column = ColumnData::new("BOOL");
        column.push_value(&Value::Bool(true));
        column.push_value(&Value::Int(5));
        assert_eq!(column.value_at(0), Value::Int(1));
        assert_eq!(column.value_at(1), Value::Int(5));
    }

    #[test]
    fn a_batch_materialises_the_rows_it_was_built_from() {
        let rows = vec![
            vec![Value::Int(1), Value::Str("a".into())],
            vec![Value::Null, Value::Str("bb".into())],
        ];
        let batch = Batch::from_rows(rows.clone());
        assert_eq!(batch.row_count, 2);
        assert_eq!(batch.rows(), rows);
        assert_eq!(batch, rows);
        assert_eq!(batch.row(2), None);
        assert_eq!(
            batch.text_rows(),
            vec![
                vec!["1".to_string(), "a".to_string()],
                vec!["null".to_string(), "bb".to_string()],
            ]
        );
    }

    #[test]
    fn a_bool_column_keeps_its_values_and_renders_as_text() {
        let rows = vec![vec![
            Value::Bool(false),
            Value::Bool(true),
            Value::Bool(false),
        ]];
        let batch = Batch::from_rows(rows.clone());
        assert!(matches!(batch.data[0], ColumnData::Bool { .. }));
        assert_eq!(batch.rows(), rows);
        assert_eq!(
            batch.text_rows(),
            vec![vec![
                "false".to_string(),
                "true".to_string(),
                "false".to_string()
            ]]
        );
    }

    #[test]
    fn appending_a_batch_keeps_the_types() {
        let mut first = Batch::from_rows(vec![vec![Value::Int(1)]]);
        first.append(&Batch::from_rows(vec![vec![Value::Int(2)]]));
        assert_eq!(first.rows(), vec![vec![Value::Int(1)], vec![Value::Int(2)]]);
        assert!(matches!(first.data[0], ColumnData::Int64 { .. }));
    }

    #[test]
    fn retaining_and_permuting_agree_with_the_rows() {
        let rows = vec![
            vec![Value::Int(1), Value::Str("a".into())],
            vec![Value::Int(2), Value::Str("b".into())],
            vec![Value::Int(3), Value::Str("c".into())],
        ];
        let mut batch = Batch::from_rows(rows.clone());
        batch.retain(&[true, false, true]);
        assert_eq!(batch.rows(), vec![rows[0].clone(), rows[2].clone()]);
        batch.permute(&[1, 0]);
        assert_eq!(batch.rows(), vec![rows[2].clone(), rows[0].clone()]);
    }
}
