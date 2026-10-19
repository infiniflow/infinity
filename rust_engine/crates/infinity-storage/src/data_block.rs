//! Typed in-memory blocks — the counterpart of the reference's
//! `ColumnVector<T>` and `DataBlock`
//! (`src/storage/column_vector/column_vector.cppm`,
//! `src/storage/column_vector/data_block.cppm`).
//!
//! The executor still evaluates expressions over `Value` rows; this module
//! provides the typed column container the block decoder fills and the class
//! boundary the reference uses for its memory blocks. A `ColumnVector<T>` is a
//! value array plus a validity vector (the Rust form of the C++ null bitmap);
//! a [`DataBlock`] is a row-aligned set of typed columns.

use infinity_common::LogicalType;

/// `DEFAULT_VECTOR_SIZE` (`src/common/default_values.cppm:58`): the initial
/// capacity of a freshly initialized flat vector.
pub const DEFAULT_VECTOR_SIZE: usize = 8192;

/// C++: `ColumnVectorType` (`column_vector.cppm:49-62`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum ColumnVectorType {
    #[default]
    Invalid,
    /// One value per row.
    Flat,
    /// Every row has the same value.
    Constant,
    /// Compact bit encoding.
    CompactBit,
    /// Rows may have missing values.
    Heterogeneous,
}

/// C++ `ColumnVector<T>`: `data_` plus the null bitmap, the vector type and
/// (for constant vectors) the single stored value.
///
/// The reference's `VectorBuffer`/`BufferObj` plumbing is not reproduced: the
/// executor's memory blocks are plain Rust vectors, and a slice/selection is a
/// copy instead of a shared buffer (registered as a design change).
#[derive(Clone, Debug, Default, PartialEq)]
pub struct ColumnVector<T> {
    data: Vec<T>,
    /// `true` for a non-NULL row (the C++ `null_bitmap_` bit).
    validity: Vec<bool>,
    vector_type: ColumnVectorType,
    capacity: usize,
    /// `tail_index_`: the row count of a constant vector (flat vectors use
    /// `data.len()`).
    size: usize,
    /// The single value of a constant vector.
    constant: Option<T>,
}

impl<T> ColumnVector<T> {
    /// C++ `ColumnVector::Make`: an empty vector.
    pub fn new() -> Self {
        Self {
            data: Vec::new(),
            validity: Vec::new(),
            vector_type: ColumnVectorType::Invalid,
            capacity: 0,
            size: 0,
            constant: None,
        }
    }

    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            data: Vec::with_capacity(capacity),
            validity: Vec::with_capacity(capacity),
            vector_type: ColumnVectorType::Flat,
            capacity,
            size: 0,
            constant: None,
        }
    }

    /// C++ `ColumnVector::Initialize(vector_type, capacity)`. `capacity`
    /// defaults to [`DEFAULT_VECTOR_SIZE`].
    pub fn initialize(&mut self, vector_type: ColumnVectorType, capacity: usize) {
        self.vector_type = vector_type;
        self.capacity = capacity;
        self.data.clear();
        self.validity.clear();
        self.size = 0;
        self.constant = None;
    }

    /// C++ `ColumnVector::Initialize()`: a flat vector with the default
    /// capacity.
    pub fn initialize_flat(&mut self) {
        self.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
    }

    /// C++ `ColumnVector::SetVectorType`: an already initialized vector cannot
    /// change its type (the reference throws).
    pub fn set_vector_type(&mut self, vector_type: ColumnVectorType) -> Result<(), String> {
        if self.vector_type != ColumnVectorType::Invalid {
            return Err("column vector is already initialized".to_string());
        }
        self.vector_type = vector_type;
        Ok(())
    }

    pub fn vector_type(&self) -> ColumnVectorType {
        self.vector_type
    }

    /// C++ `capacity()`.
    pub fn capacity(&self) -> usize {
        self.capacity
    }

    /// C++ `Size()`: the number of rows (including deleted slots).
    pub fn size(&self) -> usize {
        match self.vector_type {
            ColumnVectorType::Constant => self.size,
            _ => self.data.len(),
        }
    }

    /// C++ `ColumnVector::GetValue`: `None` for a NULL slot or an index past
    /// the vector's size.
    pub fn get(&self, row: usize) -> Option<&T> {
        if row >= self.size() {
            return None;
        }
        match self.vector_type {
            ColumnVectorType::Constant => {
                if self.validity.first().copied().unwrap_or(true) {
                    self.constant.as_ref()
                } else {
                    None
                }
            }
            _ => {
                if self.validity.get(row).copied().unwrap_or(false) {
                    self.data.get(row)
                } else {
                    None
                }
            }
        }
    }

    /// C++ `ColumnVector::GetValueByIndex` (same as [`Self::get`]).
    pub fn get_value_by_index(&self, row: usize) -> Option<&T> {
        self.get(row)
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    /// C++ `ColumnVector::nulls()`: NULL slots in the vector.
    pub fn null_count(&self) -> usize {
        self.validity.iter().filter(|v| !**v).count()
    }

    pub fn values(&self) -> &[T] {
        &self.data
    }

    pub fn validity(&self) -> &[bool] {
        &self.validity
    }

    /// C++ `ColumnVector::Reset`: back to the uninitialized state.
    pub fn reset(&mut self) {
        self.vector_type = ColumnVectorType::Invalid;
        self.capacity = 0;
        self.data.clear();
        self.validity.clear();
        self.size = 0;
        self.constant = None;
    }

    pub fn clear(&mut self) {
        self.data.clear();
        self.validity.clear();
        self.size = 0;
        self.constant = None;
    }

    /// C++ `ColumnVector::Finalize(index)`: fixes the vector's row count.
    pub fn finalize(&mut self, index: usize) {
        self.size = index;
    }

    /// C++ `ColumnVector::IsNullAt`.
    pub fn is_null_at(&self, row: usize) -> bool {
        row >= self.size() || self.get(row).is_none()
    }

    /// C++ `ColumnVector::SetNullAt`.
    pub fn set_null_at(&mut self, row: usize) {
        if row < self.validity.len() {
            self.validity[row] = false;
        }
    }
}

impl<T: Clone + Default> ColumnVector<T> {
    /// C++ `ColumnVector::ShallowCopy`: shares the other vector's storage.
    /// The Rust port copies (ownership is per-vector), registered as a design
    /// change.
    pub fn shallow_copy(&mut self, other: &Self) {
        *self = other.clone();
    }

    /// C++ `ColumnVector::Initialize(other, start_idx, end_idx)`.
    pub fn initialize_slice(&mut self, other: &Self, start: usize, end: usize) {
        self.vector_type = ColumnVectorType::Flat;
        self.capacity = end.saturating_sub(start);
        self.data.clear();
        self.validity.clear();
        for row in start..end {
            self.data.push(other.get(row).cloned().unwrap_or_default());
            self.validity.push(other.get(row).is_some());
        }
        self.size = self.data.len();
        self.constant = None;
    }

    /// C++ `ColumnVector::Initialize(other, const Selection &)`.
    pub fn initialize_selection(&mut self, other: &Self, selection: &[usize]) {
        self.vector_type = ColumnVectorType::Flat;
        self.capacity = selection.len();
        self.data.clear();
        self.validity.clear();
        for &row in selection {
            self.data.push(other.get(row).cloned().unwrap_or_default());
            self.validity.push(other.get(row).is_some());
        }
        self.size = self.data.len();
        self.constant = None;
    }
}

impl<T: Clone + Default> ColumnVector<T> {
    /// C++ `ColumnVector::AppendValue` / `AppendByPtr`: a `None` appends a
    /// NULL slot.
    pub fn push(&mut self, value: Option<T>) {
        match value {
            Some(v) => {
                self.data.push(v);
                self.validity.push(true);
            }
            None => {
                self.data.push(T::default());
                self.validity.push(false);
            }
        }
        self.size = self.data.len();
    }

    /// C++ `ColumnVector::AppendValue` with the reference's capacity rules: a
    /// constant vector holds exactly one value.
    pub fn append_value(&mut self, value: Option<T>) -> Result<(), String> {
        if self.vector_type == ColumnVectorType::Constant {
            if self.size > 0 {
                return Err("constant vector already holds its value".to_string());
            }
            self.constant = Some(value.as_ref().cloned().unwrap_or_default());
            self.validity.push(value.is_some());
            self.size = 1;
            return Ok(());
        }
        self.push(value);
        Ok(())
    }

    /// C++ `ColumnVector::SetValue(index, value)`: the index must be inside
    /// the written region.
    pub fn set_value(&mut self, row: usize, value: Option<T>) -> Result<(), String> {
        if self.vector_type == ColumnVectorType::Constant {
            // The reference writes the single constant value at index 0 even
            // before `Finalize` fixes the row count.
            if row != 0 {
                return Err("index out of range".to_string());
            }
            let valid = value.is_some();
            self.constant = value;
            if self.validity.is_empty() {
                self.validity.push(valid);
            } else {
                self.validity[0] = valid;
            }
            return Ok(());
        }
        if row >= self.size() {
            return Err("index out of range".to_string());
        }
        match value {
            Some(v) => {
                self.data[row] = v;
                self.validity[row] = true;
            }
            None => {
                self.data[row] = T::default();
                self.validity[row] = false;
            }
        }
        Ok(())
    }

    /// C++ `ColumnVector::SetValue`: grows the flat vector as needed (used by
    /// the block decoder).
    pub fn set(&mut self, row: usize, value: Option<T>) {
        if row >= self.data.len() {
            self.data.resize_with(row + 1, T::default);
            self.validity.resize(row + 1, false);
        }
        match value {
            Some(v) => {
                self.data[row] = v;
                self.validity[row] = true;
            }
            None => {
                self.data[row] = T::default();
                self.validity[row] = false;
            }
        }
        self.size = self.data.len();
    }
}

impl<T: Clone + Default> ColumnVector<T> {
    /// C++ `ColumnVector::AppendWith(T, count)`.
    pub fn extend_value(&mut self, value: T, count: usize) {
        self.data.extend(std::iter::repeat_n(value, count));
        self.validity.extend(std::iter::repeat_n(true, count));
        self.size = self.data.len();
    }
}

#[derive(Clone, Debug, PartialEq)]
pub enum Column {
    Int(ColumnVector<i64>),
    Float(ColumnVector<f64>),
    Bool(ColumnVector<bool>),
    Str(ColumnVector<String>),
    Bytes(ColumnVector<Vec<u8>>),
    TinyInt(ColumnVector<i8>),
    SmallInt(ColumnVector<i16>),
    Int32(ColumnVector<i32>),
    HugeInt(ColumnVector<i128>),
    Float32(ColumnVector<f32>),
    Decimal(ColumnVector<infinity_common::DecimalT>),
    Date(ColumnVector<infinity_common::DateT>),
    Time(ColumnVector<infinity_common::TimeT>),
    DateTime(ColumnVector<infinity_common::DateTimeT>),
    Timestamp(ColumnVector<infinity_common::TimestampT>),
    Interval(ColumnVector<infinity_common::IntervalT>),
    Uuid(ColumnVector<[u8; 16]>),
    Embedding(ColumnVector<Vec<f32>>),
    Array(ColumnVector<Vec<i64>>),
    Mixed(ColumnVector<infinity_common::MixedT>),
}

impl Column {
    /// The C++ logical type this column stores.
    pub fn logical_type(&self) -> LogicalType {
        match self {
            Column::Int(_) => LogicalType::BigInt,
            Column::Float(_) => LogicalType::Double,
            Column::Bool(_) => LogicalType::Boolean,
            Column::Str(_) => LogicalType::Varchar,
            Column::Bytes(_) => LogicalType::Varchar,
            Column::TinyInt(_) => LogicalType::TinyInt,
            Column::SmallInt(_) => LogicalType::SmallInt,
            Column::Int32(_) => LogicalType::Integer,
            Column::HugeInt(_) => LogicalType::HugeInt,
            Column::Float32(_) => LogicalType::Float,
            Column::Decimal(_) => LogicalType::Decimal,
            Column::Date(_) => LogicalType::Date,
            Column::Time(_) => LogicalType::Time,
            Column::DateTime(_) => LogicalType::DateTime,
            Column::Timestamp(_) => LogicalType::Timestamp,
            Column::Interval(_) => LogicalType::Interval,
            Column::Uuid(_) => LogicalType::Uuid,
            Column::Embedding(_) => LogicalType::Embedding(0),
            Column::Array(_) => LogicalType::Array,
            Column::Mixed(_) => LogicalType::Mixed,
        }
    }

    pub fn len(&self) -> usize {
        match self {
            Column::Int(c) => c.len(),
            Column::Float(c) => c.len(),
            Column::Bool(c) => c.len(),
            Column::Str(c) => c.len(),
            Column::Bytes(c) => c.len(),
            Column::TinyInt(c) => c.len(),
            Column::SmallInt(c) => c.len(),
            Column::Int32(c) => c.len(),
            Column::HugeInt(c) => c.len(),
            Column::Float32(c) => c.len(),
            Column::Decimal(c) => c.len(),
            Column::Date(c) => c.len(),
            Column::Time(c) => c.len(),
            Column::DateTime(c) => c.len(),
            Column::Timestamp(c) => c.len(),
            Column::Interval(c) => c.len(),
            Column::Uuid(c) => c.len(),
            Column::Embedding(c) => c.len(),
            Column::Array(c) => c.len(),
            Column::Mixed(c) => c.len(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    pub fn null_count(&self) -> usize {
        match self {
            Column::Int(c) => c.null_count(),
            Column::Float(c) => c.null_count(),
            Column::Bool(c) => c.null_count(),
            Column::Str(c) => c.null_count(),
            Column::Bytes(c) => c.null_count(),
            Column::TinyInt(c) => c.null_count(),
            Column::SmallInt(c) => c.null_count(),
            Column::Int32(c) => c.null_count(),
            Column::HugeInt(c) => c.null_count(),
            Column::Float32(c) => c.null_count(),
            Column::Decimal(c) => c.null_count(),
            Column::Date(c) => c.null_count(),
            Column::Time(c) => c.null_count(),
            Column::DateTime(c) => c.null_count(),
            Column::Timestamp(c) => c.null_count(),
            Column::Interval(c) => c.null_count(),
            Column::Uuid(c) => c.null_count(),
            Column::Embedding(c) => c.null_count(),
            Column::Array(c) => c.null_count(),
            Column::Mixed(c) => c.null_count(),
        }
    }

    pub fn clear(&mut self) {
        match self {
            Column::Int(c) => c.clear(),
            Column::Float(c) => c.clear(),
            Column::Bool(c) => c.clear(),
            Column::Str(c) => c.clear(),
            Column::Bytes(c) => c.clear(),
            Column::TinyInt(c) => c.clear(),
            Column::SmallInt(c) => c.clear(),
            Column::Int32(c) => c.clear(),
            Column::HugeInt(c) => c.clear(),
            Column::Float32(c) => c.clear(),
            Column::Decimal(c) => c.clear(),
            Column::Date(c) => c.clear(),
            Column::Time(c) => c.clear(),
            Column::DateTime(c) => c.clear(),
            Column::Timestamp(c) => c.clear(),
            Column::Interval(c) => c.clear(),
            Column::Uuid(c) => c.clear(),
            Column::Embedding(c) => c.clear(),
            Column::Array(c) => c.clear(),
            Column::Mixed(c) => c.clear(),
        }
    }

    /// The rendered text of a slot (`None` for NULL).
    pub fn value_text(&self, row: usize) -> Option<String> {
        match self {
            Column::Int(c) => c.get(row).map(|v| v.to_string()),
            Column::Float(c) => c.get(row).map(|v| v.to_string()),
            Column::Bool(c) => c.get(row).map(|v| v.to_string()),
            Column::Str(c) => c.get(row).cloned(),
            Column::Bytes(c) => c.get(row).map(|v| String::from_utf8_lossy(v).into_owned()),
            Column::TinyInt(c) => c.get(row).map(|v| v.to_string()),
            Column::SmallInt(c) => c.get(row).map(|v| v.to_string()),
            Column::Int32(c) => c.get(row).map(|v| v.to_string()),
            Column::HugeInt(c) => c.get(row).map(|v| v.to_string()),
            Column::Float32(c) => c.get(row).map(|v| v.to_string()),
            Column::Decimal(c) => c.get(row).map(|v| format!("{}e-{}", v.value, v.scale)),
            Column::Date(c) => c.get(row).map(|v| v.value.to_string()),
            Column::Time(c) => c.get(row).map(|v| v.value.to_string()),
            Column::DateTime(c) => c.get(row).map(|v| format!("{} {}", v.date, v.time)),
            Column::Timestamp(c) => c.get(row).map(|v| format!("{} {}", v.date, v.time)),
            Column::Interval(c) => c.get(row).map(|v| v.value.to_string()),
            Column::Uuid(c) => c
                .get(row)
                .map(|v| v.iter().map(|b| format!("{b:02x}")).collect::<String>()),
            Column::Embedding(c) => c.get(row).map(|v| {
                format!(
                    "[{}]",
                    v.iter()
                        .map(|x| x.to_string())
                        .collect::<Vec<_>>()
                        .join(",")
                )
            }),
            Column::Array(c) => c.get(row).map(|v| {
                format!(
                    "[{}]",
                    v.iter()
                        .map(|x| x.to_string())
                        .collect::<Vec<_>>()
                        .join(",")
                )
            }),
            Column::Mixed(c) => c.get(row).map(|v| v.0.clone()),
        }
    }

    /// Approximate heap bytes held by this column.
    pub fn mem_used(&self) -> usize {
        match self {
            Column::Int(v) => v.values().len() * 8 + v.validity().len(),
            Column::Float(v) => v.values().len() * 8 + v.validity().len(),
            Column::Bool(v) => v.values().len() + v.validity().len(),
            Column::Str(v) => {
                v.values().iter().map(|s| s.len() + 24).sum::<usize>() + v.validity().len()
            }
            Column::Bytes(v) => {
                v.values().iter().map(|s| s.len() + 24).sum::<usize>() + v.validity().len()
            }
            Column::TinyInt(v) => v.values().len() + v.validity().len(),
            Column::SmallInt(v) => v.values().len() * 2 + v.validity().len(),
            Column::Int32(v) => v.values().len() * 4 + v.validity().len(),
            Column::HugeInt(v) => v.values().len() * 16 + v.validity().len(),
            Column::Float32(v) => v.values().len() * 4 + v.validity().len(),
            Column::Decimal(v) => v.values().len() * 16 + v.validity().len(),
            Column::Date(v) => v.values().len() * 4 + v.validity().len(),
            Column::Time(v) => v.values().len() * 4 + v.validity().len(),
            Column::DateTime(v) => v.values().len() * 8 + v.validity().len(),
            Column::Timestamp(v) => v.values().len() * 8 + v.validity().len(),
            Column::Interval(v) => v.values().len() * 4 + v.validity().len(),
            Column::Uuid(v) => v.values().len() * 16 + v.validity().len(),
            Column::Embedding(v) => {
                v.values().iter().map(|x| x.len() * 4 + 24).sum::<usize>() + v.validity().len()
            }
            Column::Array(v) => {
                v.values().iter().map(|x| x.len() * 8 + 24).sum::<usize>() + v.validity().len()
            }
            Column::Mixed(v) => {
                v.values().iter().map(|x| x.0.len() + 24).sum::<usize>() + v.validity().len()
            }
        }
    }

    /// Parses one text slot (the block/decoder path).
    pub fn push_text(&mut self, text: Option<&str>) {
        match self {
            Column::Int(c) => c.push(text.and_then(|t| t.trim().parse::<i64>().ok())),
            Column::Float(c) => c.push(text.and_then(|t| t.trim().parse::<f64>().ok())),
            Column::Bool(c) => c.push(text.and_then(|t| match t.trim() {
                "true" | "1" => Some(true),
                "false" | "0" => Some(false),
                _ => None,
            })),
            Column::Str(c) => c.push(text.map(str::to_string)),
            Column::Bytes(c) => c.push(text.map(|t| t.as_bytes().to_vec())),
            Column::TinyInt(c) => c.push(text.and_then(|t| t.trim().parse::<i8>().ok())),
            Column::SmallInt(c) => c.push(text.and_then(|t| t.trim().parse::<i16>().ok())),
            Column::Int32(c) => c.push(text.and_then(|t| t.trim().parse::<i32>().ok())),
            Column::HugeInt(c) => c.push(text.and_then(|t| t.trim().parse::<i128>().ok())),
            Column::Float32(c) => c.push(text.and_then(|t| t.trim().parse::<f32>().ok())),
            Column::Decimal(_) => {}
            Column::Date(_) => {}
            Column::Time(_) => {}
            Column::DateTime(_) => {}
            Column::Timestamp(_) => {}
            Column::Interval(_) => {}
            Column::Uuid(_) => {}
            Column::Embedding(_) => {}
            Column::Array(_) => {}
            Column::Mixed(c) => c.push(text.map(infinity_common::MixedT::new)),
        }
    }
}

/// One typed slot value, the Rust counterpart of the reference's `Value` at
/// the `DataBlock` boundary.
#[derive(Clone, Debug, PartialEq)]
pub enum BlockValue {
    Bool(bool),
    TinyInt(i8),
    SmallInt(i16),
    Int(i64),
    Int32(i32),
    HugeInt(i128),
    Float(f32),
    Double(f64),
    Decimal(infinity_common::DecimalT),
    Str(String),
    Date(infinity_common::DateT),
    Time(infinity_common::TimeT),
    DateTime(infinity_common::DateTimeT),
    Timestamp(infinity_common::TimestampT),
    Interval(infinity_common::IntervalT),
    Uuid([u8; 16]),
    Embedding(Vec<f32>),
    Array(Vec<i64>),
    Mixed(infinity_common::MixedT),
    Null,
}

/// C++ `DataBlock`: a row-aligned set of typed columns.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct DataBlock {
    columns: Vec<Column>,
    row_count: usize,
    capacity: usize,
}

impl DataBlock {
    /// C++ `DataBlock::Make(column_defs, capacity)`. The column vectors start
    /// empty; rows are appended with [`DataBlock::push_row`].
    pub fn new(columns: Vec<Column>, capacity: usize) -> Self {
        Self {
            columns,
            row_count: 0,
            capacity,
        }
    }

    pub fn column_count(&self) -> usize {
        self.columns.len()
    }

    pub fn columns(&self) -> &[Column] {
        &self.columns
    }

    pub fn column(&self, index: usize) -> Option<&Column> {
        self.columns.get(index)
    }

    /// C++ `DataBlock::row_count()`.
    pub fn row_count(&self) -> usize {
        self.row_count
    }

    /// C++ `DataBlock::capacity()`.
    pub fn capacity(&self) -> usize {
        self.capacity
    }

    pub fn is_full(&self) -> bool {
        self.row_count >= self.capacity
    }

    /// C++ `DataBlock::AppendValue`/`AppendRow` for one slot. Every column must
    /// receive a value (or NULL) per row, as in the reference.
    pub fn push_slot(&mut self, column: usize, text: Option<&str>) {
        let Some(col) = self.columns.get_mut(column) else {
            return;
        };
        col.push_text(text);
    }

    /// C++ `DataBlock::Finalize`: all columns have the same length.
    pub fn finalize(&mut self) {
        self.row_count = self.columns.iter().map(Column::len).max().unwrap_or(0);
    }

    /// C++ `DataBlock::GetValue(column, row)`, rendered as text.
    pub fn value_text(&self, column: usize, row: usize) -> Option<String> {
        self.columns.get(column)?.value_text(row)
    }

    /// C++ `DataBlock::Reset`.
    pub fn reset(&mut self) {
        for column in &mut self.columns {
            column.clear();
        }
        self.row_count = 0;
    }

    /// C++ `DataBlock::AppendValue(column, value)`.
    pub fn append_value(&mut self, column: usize, value: BlockValue) -> Result<(), String> {
        let col = self
            .columns
            .get_mut(column)
            .ok_or_else(|| format!("column {column} out of range"))?;
        if value == BlockValue::Null {
            match col {
                Column::Bool(c) => c.push(None),
                Column::TinyInt(c) => c.push(None),
                Column::SmallInt(c) => c.push(None),
                Column::Int(c) => c.push(None),
                Column::Int32(c) => c.push(None),
                Column::HugeInt(c) => c.push(None),
                Column::Float32(c) => c.push(None),
                Column::Float(c) => c.push(None),
                Column::Decimal(c) => c.push(None),
                Column::Str(c) => c.push(None),
                Column::Date(c) => c.push(None),
                Column::Time(c) => c.push(None),
                Column::DateTime(c) => c.push(None),
                Column::Timestamp(c) => c.push(None),
                Column::Interval(c) => c.push(None),
                Column::Uuid(c) => c.push(None),
                Column::Embedding(c) => c.push(None),
                Column::Array(c) => c.push(None),
                Column::Mixed(c) => c.push(None),
                Column::Bytes(c) => c.push(None),
            }
            return Ok(());
        }
        match (col, value) {
            (Column::Bool(c), BlockValue::Bool(v)) => c.push(Some(v)),
            (Column::TinyInt(c), BlockValue::TinyInt(v)) => c.push(Some(v)),
            (Column::SmallInt(c), BlockValue::SmallInt(v)) => c.push(Some(v)),
            (Column::Int(c), BlockValue::Int(v)) => c.push(Some(v)),
            (Column::Int32(c), BlockValue::Int32(v)) => c.push(Some(v)),
            (Column::HugeInt(c), BlockValue::HugeInt(v)) => c.push(Some(v)),
            (Column::Float32(c), BlockValue::Float(v)) => c.push(Some(v)),
            (Column::Float(c), BlockValue::Double(v)) => c.push(Some(v)),
            (Column::Decimal(c), BlockValue::Decimal(v)) => c.push(Some(v)),
            (Column::Str(c), BlockValue::Str(v)) => c.push(Some(v)),
            (Column::Date(c), BlockValue::Date(v)) => c.push(Some(v)),
            (Column::Time(c), BlockValue::Time(v)) => c.push(Some(v)),
            (Column::DateTime(c), BlockValue::DateTime(v)) => c.push(Some(v)),
            (Column::Timestamp(c), BlockValue::Timestamp(v)) => c.push(Some(v)),
            (Column::Interval(c), BlockValue::Interval(v)) => c.push(Some(v)),
            (Column::Uuid(c), BlockValue::Uuid(v)) => c.push(Some(v)),
            (Column::Embedding(c), BlockValue::Embedding(v)) => c.push(Some(v)),
            (Column::Array(c), BlockValue::Array(v)) => c.push(Some(v)),
            (Column::Mixed(c), BlockValue::Mixed(v)) => c.push(Some(v)),
            _ => return Err(format!("value type does not match column {column}")),
        }
        Ok(())
    }

    /// C++ `DataBlock::GetValue(column, row)`.
    pub fn get_value(&self, column: usize, row: usize) -> Option<BlockValue> {
        let col = self.columns.get(column)?;
        match col {
            Column::Bool(c) => c.get(row).copied().map(BlockValue::Bool),
            Column::TinyInt(c) => c.get(row).copied().map(BlockValue::TinyInt),
            Column::SmallInt(c) => c.get(row).copied().map(BlockValue::SmallInt),
            Column::Int(c) => c.get(row).copied().map(BlockValue::Int),
            Column::Int32(c) => c.get(row).copied().map(BlockValue::Int32),
            Column::HugeInt(c) => c.get(row).copied().map(BlockValue::HugeInt),
            Column::Float32(c) => c.get(row).copied().map(BlockValue::Float),
            Column::Float(c) => c.get(row).copied().map(BlockValue::Double),
            Column::Decimal(c) => c.get(row).copied().map(BlockValue::Decimal),
            Column::Str(c) => c.get(row).cloned().map(BlockValue::Str),
            Column::Date(c) => c.get(row).copied().map(BlockValue::Date),
            Column::Time(c) => c.get(row).copied().map(BlockValue::Time),
            Column::DateTime(c) => c.get(row).copied().map(BlockValue::DateTime),
            Column::Timestamp(c) => c.get(row).copied().map(BlockValue::Timestamp),
            Column::Interval(c) => c.get(row).copied().map(BlockValue::Interval),
            Column::Uuid(c) => c.get(row).copied().map(BlockValue::Uuid),
            Column::Embedding(c) => c.get(row).cloned().map(BlockValue::Embedding),
            Column::Array(c) => c.get(row).cloned().map(BlockValue::Array),
            Column::Mixed(c) => c.get(row).cloned().map(BlockValue::Mixed),
            Column::Bytes(c) => c
                .get(row)
                .cloned()
                .map(|v| BlockValue::Str(String::from_utf8_lossy(&v).into_owned())),
        }
    }

    /// Approximate heap bytes, for the buffer-manager budget.
    pub fn mem_used(&self) -> usize {
        self.columns.iter().map(Column::mem_used).sum()
    }
}

/// C++: `DataBlockTest.test1` (`storage/definition/data_block_ut.cpp`):
/// every supported column type appends and reads back in one row block. The
/// geo columns of the reference test are excluded by scope.
#[cfg(test)]
mod data_block_reference_suite {
    use super::*;
    use infinity_common::{DateT, DateTimeT, DecimalT, IntervalT, MixedT, TimeT, TimestampT};

    fn columns() -> Vec<Column> {
        vec![
            Column::Bool(ColumnVector::new()),
            Column::TinyInt(ColumnVector::new()),
            Column::SmallInt(ColumnVector::new()),
            Column::Int32(ColumnVector::new()),
            Column::Int(ColumnVector::new()),
            Column::HugeInt(ColumnVector::new()),
            Column::Float32(ColumnVector::new()),
            Column::Float(ColumnVector::new()),
            Column::Decimal(ColumnVector::new()),
            Column::Str(ColumnVector::new()),
            Column::Date(ColumnVector::new()),
            Column::Time(ColumnVector::new()),
            Column::DateTime(ColumnVector::new()),
            Column::Timestamp(ColumnVector::new()),
            Column::Interval(ColumnVector::new()),
            Column::Array(ColumnVector::new()),
            Column::Uuid(ColumnVector::new()),
            Column::Embedding(ColumnVector::new()),
            Column::Mixed(ColumnVector::new()),
        ]
    }

    fn row(i: i64) -> Vec<BlockValue> {
        vec![
            BlockValue::Bool(i % 2 == 0),
            BlockValue::TinyInt(i as i8),
            BlockValue::SmallInt(i as i16),
            BlockValue::Int32(i as i32),
            BlockValue::Int(i),
            BlockValue::HugeInt(i as i128),
            BlockValue::Float(i as f32),
            BlockValue::Double(i as f64),
            BlockValue::Decimal(DecimalT::new(i as i128, 2)),
            BlockValue::Str(format!("str_{i}")),
            BlockValue::Date(DateT::new(i as i32)),
            BlockValue::Time(TimeT::new(i as i32)),
            BlockValue::DateTime(DateTimeT::new(i as i32, i as i32)),
            BlockValue::Timestamp(TimestampT::new(i as i32, i as i32)),
            BlockValue::Interval(IntervalT::new(i as i32)),
            BlockValue::Array(vec![i, i + 1]),
            BlockValue::Uuid([i as u8; 16]),
            BlockValue::Embedding(vec![i as f32; 32]),
            BlockValue::Mixed(MixedT::new(format!("mixed_{i}"))),
        ]
    }

    #[test]
    fn all_types_append_and_read_back() {
        let mut block = DataBlock::new(columns(), DEFAULT_VECTOR_SIZE);
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            for (column, value) in row(i).into_iter().enumerate() {
                block.append_value(column, value).unwrap();
            }
            block.finalize();
        }
        assert_eq!(block.row_count(), DEFAULT_VECTOR_SIZE);
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            for (column, value) in row(i).into_iter().enumerate() {
                assert_eq!(
                    block.get_value(column, i as usize),
                    Some(value),
                    "col {column}"
                );
            }
        }
        block.reset();
        assert_eq!(block.row_count(), 0);
        assert!(block.mem_used() == 0);
    }

    #[test]
    fn nulls_and_type_mismatches() {
        let mut block = DataBlock::new(columns(), 4);
        block.append_value(0, BlockValue::Null).unwrap();
        block.append_value(1, BlockValue::Null).unwrap();
        block.finalize();
        assert_eq!(block.get_value(0, 0), None);
        assert_eq!(
            block.append_value(2, BlockValue::Str("bad".into())),
            Err("value type does not match column 2".to_string())
        );
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// C++ `column_vector_ut.cpp`: append values and NULLs, read them back.
    #[test]
    fn column_vector_keeps_values_and_nulls() {
        let mut c = ColumnVector::with_capacity(4);
        c.push(Some(1i64));
        c.push(None);
        c.push(Some(3));
        assert_eq!(c.len(), 3);
        assert_eq!(c.get(0), Some(&1));
        assert_eq!(c.get(1), None);
        assert_eq!(c.get(2), Some(&3));
        assert_eq!(c.null_count(), 1);
        assert_eq!(c.validity(), &[true, false, true]);
        assert_eq!(c.get(9), None);
    }

    /// C++ `DataBlock::AppendRow` / `GetValue`: rows span every column and
    /// NULLs are per-slot.
    #[test]
    fn data_block_appends_aligned_rows() {
        let columns = vec![
            Column::Int(ColumnVector::new()),
            Column::Str(ColumnVector::new()),
            Column::Float(ColumnVector::new()),
        ];
        let mut block = DataBlock::new(columns, 4);
        for (i, (id, name, score)) in [
            ("1", Some("alice"), "1.5"),
            ("2", None, "2.5"),
            ("3", Some("carol"), "3.5"),
        ]
        .into_iter()
        .enumerate()
        {
            block.push_slot(0, Some(id));
            block.push_slot(1, name);
            block.push_slot(2, Some(score));
            block.finalize();
            assert_eq!(block.row_count(), i + 1);
        }
        assert_eq!(block.row_count(), 3);
        assert_eq!(block.column_count(), 3);
        assert_eq!(block.value_text(0, 1).as_deref(), Some("2"));
        assert_eq!(block.value_text(1, 1), None);
        assert_eq!(block.value_text(2, 2).as_deref(), Some("3.5"));
        assert!(block.mem_used() > 0);
        block.reset();
        assert_eq!(block.row_count(), 0);
        assert_eq!(block.column(0).unwrap().len(), 0);
    }

    #[test]
    fn block_reports_the_declared_logical_types() {
        let columns = [
            Column::Int(ColumnVector::new()),
            Column::Float(ColumnVector::new()),
            Column::Bool(ColumnVector::new()),
            Column::Str(ColumnVector::new()),
        ];
        let types: Vec<LogicalType> = columns.iter().map(Column::logical_type).collect();
        assert_eq!(
            types,
            vec![
                LogicalType::BigInt,
                LogicalType::Double,
                LogicalType::Boolean,
                LogicalType::Varchar
            ]
        );
    }
}

#[cfg(test)]
mod reference_suite {
    //! Ports of `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp`
    //! (`ColumnVectorIntegerTest`), generically over the integer widths.

    use super::*;
    use std::fmt::Debug;

    fn flat_suite<T>(conv: impl Fn(i64) -> T)
    where
        T: Clone + Default + PartialEq + Debug,
    {
        let mut v = ColumnVector::<T>::new();
        v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert!(v.set_vector_type(ColumnVectorType::Flat).is_err());
        assert!(v.set_value(3, Some(conv(3))).is_err());

        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            v.append_value(Some(conv(i))).unwrap();
            assert_eq!(v.get_value_by_index(i as usize), Some(&conv(i)));
            assert!(v.get_value_by_index(i as usize + 1).is_none());
        }

        let mut clone = ColumnVector::<T>::new();
        clone.shallow_copy(&v);
        assert_eq!(clone.size(), v.size());
        assert_eq!(clone.capacity(), v.capacity());
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            assert_eq!(clone.get(i as usize), Some(&conv(i)));
        }

        v.reset();
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.size(), 0);
        v.initialize_flat();
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
    }

    fn constant_suite<T>(conv: impl Fn(i64) -> T)
    where
        T: Clone + Default + PartialEq + Debug,
    {
        let mut v = ColumnVector::<T>::new();
        v.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert!(v.set_vector_type(ColumnVectorType::Constant).is_err());

        v.append_value(Some(conv(0))).unwrap();
        assert!(v.append_value(Some(conv(0))).is_err());
        assert_eq!(v.get_value_by_index(0), Some(&conv(0)));
        assert!(v.get_value_by_index(1).is_none());

        v.reset();
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.size(), 0);
        v.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
    }

    fn select_suite<T>(conv: impl Fn(i64) -> T)
    where
        T: Clone + Default + PartialEq + Debug,
    {
        let mut v = ColumnVector::<T>::new();
        v.initialize_flat();
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            v.append_value(Some(conv(i))).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            assert_eq!(v.get(i as usize), Some(&conv(i)));
        }
        let selection: Vec<usize> = (0..DEFAULT_VECTOR_SIZE / 2).map(|i| i * 2).collect();
        let mut target = ColumnVector::<T>::new();
        target.initialize_selection(&v, &selection);
        assert_eq!(target.size(), DEFAULT_VECTOR_SIZE / 2);
        for i in 0..DEFAULT_VECTOR_SIZE as i64 / 2 {
            assert_eq!(target.get(i as usize), Some(&conv(2 * i)));
        }
    }

    fn slice_suite<T>(conv: impl Fn(i64) -> T)
    where
        T: Clone + Default + PartialEq + Debug,
    {
        let mut v = ColumnVector::<T>::new();
        v.initialize_flat();
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            v.append_value(Some(conv(i))).unwrap();
        }
        let start = DEFAULT_VECTOR_SIZE / 4;
        let end = 3 * DEFAULT_VECTOR_SIZE / 4;
        let mut target = ColumnVector::<T>::new();
        target.initialize_slice(&v, start, end);
        assert_eq!(target.size(), DEFAULT_VECTOR_SIZE / 2);
        for i in 0..(end - start) as i64 {
            assert_eq!(target.get(i as usize), Some(&conv(start as i64 + i)));
        }
    }

    // C++: ColumnVectorIntegerTest, tinyint.
    #[test]
    fn tinyint_flat_constant_select_slice() {
        flat_suite(|i| i as i8);
        constant_suite(|i| i as i8);
        select_suite(|i| i as i8);
        slice_suite(|i| i as i8);
    }

    // C++: ColumnVectorIntegerTest, smallint.
    #[test]
    fn smallint_flat_constant_select_slice() {
        flat_suite(|i| i as i16);
        constant_suite(|i| i as i16);
        select_suite(|i| i as i16);
        slice_suite(|i| i as i16);
    }

    // C++: ColumnVectorIntegerTest, int.
    #[test]
    fn int_flat_constant_select_slice() {
        flat_suite(|i| i as i32);
        constant_suite(|i| i as i32);
        select_suite(|i| i as i32);
        slice_suite(|i| i as i32);
    }

    // C++: ColumnVectorIntegerTest, bigint.
    #[test]
    fn bigint_flat_constant_select_slice() {
        flat_suite(|i| i);
        constant_suite(|i| i);
        select_suite(|i| i);
        slice_suite(|i| i);
    }

    // C++: ColumnVectorIntegerTest, hugeint.
    #[test]
    fn hugeint_flat_constant_select_slice() {
        flat_suite(|i| i as i128);
        constant_suite(|i| i as i128);
        select_suite(|i| i as i128);
        slice_suite(|i| i as i128);
    }

    // C++: `column_vector_date_time_ut.cpp`, date.
    #[test]
    fn date_flat_constant_select_slice() {
        flat_suite(|i| infinity_common::DateT::new(i as i32));
        constant_suite(|i| infinity_common::DateT::new(i as i32));
        select_suite(|i| infinity_common::DateT::new(i as i32));
        slice_suite(|i| infinity_common::DateT::new(i as i32));
    }

    // C++: `column_vector_date_time_ut.cpp`, time.
    #[test]
    fn time_flat_constant_select_slice() {
        flat_suite(|i| infinity_common::TimeT::new(i as i32));
        constant_suite(|i| infinity_common::TimeT::new(i as i32));
        select_suite(|i| infinity_common::TimeT::new(i as i32));
        slice_suite(|i| infinity_common::TimeT::new(i as i32));
    }

    // C++: `column_vector_date_time_ut.cpp`, datetime.
    #[test]
    fn datetime_flat_constant_select_slice() {
        flat_suite(|i| infinity_common::DateTimeT::new(i as i32, i as i32));
        constant_suite(|i| infinity_common::DateTimeT::new(i as i32, i as i32));
        select_suite(|i| infinity_common::DateTimeT::new(i as i32, i as i32));
        slice_suite(|i| infinity_common::DateTimeT::new(i as i32, i as i32));
    }

    // C++: `column_vector_date_time_ut.cpp`, timestamp.
    #[test]
    fn timestamp_flat_constant_select_slice() {
        flat_suite(|i| infinity_common::TimestampT::new(i as i32, i as i32));
        constant_suite(|i| infinity_common::TimestampT::new(i as i32, i as i32));
        select_suite(|i| infinity_common::TimestampT::new(i as i32, i as i32));
        slice_suite(|i| infinity_common::TimestampT::new(i as i32, i as i32));
    }

    // C++: `column_vector_date_time_ut.cpp`, interval.
    #[test]
    fn interval_flat_constant_select_slice() {
        flat_suite(|i| infinity_common::IntervalT::new(i as i32));
        constant_suite(|i| infinity_common::IntervalT::new(i as i32));
        select_suite(|i| infinity_common::IntervalT::new(i as i32));
        slice_suite(|i| infinity_common::IntervalT::new(i as i32));
    }

    /// C++: `flat_interval`'s constant tail: `SetValueByIndex(0, …)` then
    /// `Finalize(1)` for every source row.
    #[test]
    fn interval_constant_set_value_before_finalize() {
        let mut source = ColumnVector::<infinity_common::IntervalT>::new();
        source.initialize_flat();
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            source
                .append_value(Some(infinity_common::IntervalT::new(i as i32)))
                .unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            let mut c = ColumnVector::new();
            c.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
            c.set_value(0, source.get(i as usize).cloned()).unwrap();
            c.finalize(1);
            assert_eq!(c.get(0), Some(&infinity_common::IntervalT::new(i as i32)));
            c.reset();
        }
    }

    // C++: `column_vector_bool_ut.cpp` (flat/constant/select/slice).
    #[test]
    fn bool_flat_constant_select_slice() {
        let conv = |i: i64| i % 2 == 0;
        flat_suite(conv);
        constant_suite(conv);
        select_suite(conv);
        slice_suite(conv);
    }

    // C++: `column_vector_float_ut.cpp`, float.
    #[test]
    fn float_flat_constant_select_slice() {
        flat_suite(|i| i as f32);
        constant_suite(|i| i as f32);
        select_suite(|i| i as f32);
        slice_suite(|i| i as f32);
    }

    // C++: `column_vector_float_ut.cpp`, double.
    #[test]
    fn double_flat_constant_select_slice() {
        flat_suite(|i| i as f64);
        constant_suite(|i| i as f64);
        select_suite(|i| i as f64);
        slice_suite(|i| i as f64);
    }

    // C++: `column_vector_uuid_ut.cpp`.
    #[test]
    fn uuid_flat_constant_select_slice() {
        let conv = |i: i64| {
            let mut bytes = [0u8; 16];
            bytes[..8].copy_from_slice(&(i as u64).to_le_bytes());
            bytes
        };
        flat_suite(conv);
        constant_suite(conv);
        select_suite(conv);
        slice_suite(conv);
    }

    // C++: `column_vector_embedding_ut.cpp`.
    #[test]
    fn embedding_flat_constant_select_slice() {
        let conv = |i: i64| vec![i as f32, (i * 2) as f32, (i * 3) as f32];
        flat_suite(conv);
        constant_suite(conv);
        select_suite(conv);
        slice_suite(conv);
    }

    /// C++: `column_vector_varchar_ut.cpp` — the inline/out-of-line buffer
    /// distinction does not exist here (a row owns its `String`), so the four
    /// structural cases carry over directly.
    #[test]
    fn varchar_flat_constant_select_slice() {
        let conv = |i: i64| format!("v{i}");
        flat_suite(conv);
        constant_suite(conv);
        select_suite(conv);
        slice_suite(conv);
    }
}
