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

/// C++ `RowID` (`src/common/types/row_id.h`): the segment and the row offset
/// inside it. The Rust engine mostly uses the packed `u64` alias from
/// `infinity_common`; a column needs the two fields to stay addressable.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct RowIdT {
    pub segment_id: u32,
    pub segment_offset: u32,
}

impl RowIdT {
    pub fn new(segment_id: u32, segment_offset: u32) -> Self {
        Self {
            segment_id,
            segment_offset,
        }
    }
}

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
    Json(ColumnVector<infinity_common::json::Json>),
    RowId(ColumnVector<RowIdT>),
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
            Column::Json(_) => LogicalType::Json,
            Column::RowId(_) => LogicalType::RowID,
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
            Column::Json(c) => c.len(),
            Column::RowId(c) => c.len(),
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
            Column::Json(c) => c.null_count(),
            Column::RowId(c) => c.null_count(),
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
            Column::Json(c) => c.clear(),
            Column::RowId(c) => c.clear(),
        }
    }

    /// C++ `ColumnVectorType` of the underlying vector.
    pub fn vector_type(&self) -> ColumnVectorType {
        match self {
            Column::Int(c) => c.vector_type(),
            Column::Float(c) => c.vector_type(),
            Column::Bool(c) => c.vector_type(),
            Column::Str(c) => c.vector_type(),
            Column::Bytes(c) => c.vector_type(),
            Column::TinyInt(c) => c.vector_type(),
            Column::SmallInt(c) => c.vector_type(),
            Column::Int32(c) => c.vector_type(),
            Column::HugeInt(c) => c.vector_type(),
            Column::Float32(c) => c.vector_type(),
            Column::Decimal(c) => c.vector_type(),
            Column::Date(c) => c.vector_type(),
            Column::Time(c) => c.vector_type(),
            Column::DateTime(c) => c.vector_type(),
            Column::Timestamp(c) => c.vector_type(),
            Column::Interval(c) => c.vector_type(),
            Column::Uuid(c) => c.vector_type(),
            Column::Embedding(c) => c.vector_type(),
            Column::Array(c) => c.vector_type(),
            Column::Mixed(c) => c.vector_type(),
            Column::Json(c) => c.vector_type(),
            Column::RowId(c) => c.vector_type(),
        }
    }

    /// C++ `ColumnVector::Size()`: the number of rows.
    pub fn size(&self) -> usize {
        match self {
            Column::Int(c) => c.size(),
            Column::Float(c) => c.size(),
            Column::Bool(c) => c.size(),
            Column::Str(c) => c.size(),
            Column::Bytes(c) => c.size(),
            Column::TinyInt(c) => c.size(),
            Column::SmallInt(c) => c.size(),
            Column::Int32(c) => c.size(),
            Column::HugeInt(c) => c.size(),
            Column::Float32(c) => c.size(),
            Column::Decimal(c) => c.size(),
            Column::Date(c) => c.size(),
            Column::Time(c) => c.size(),
            Column::DateTime(c) => c.size(),
            Column::Timestamp(c) => c.size(),
            Column::Interval(c) => c.size(),
            Column::Uuid(c) => c.size(),
            Column::Embedding(c) => c.size(),
            Column::Array(c) => c.size(),
            Column::Mixed(c) => c.size(),
            Column::Json(c) => c.size(),
            Column::RowId(c) => c.size(),
        }
    }

    /// C++ `ColumnVector::capacity()`.
    pub fn capacity(&self) -> usize {
        match self {
            Column::Int(c) => c.capacity(),
            Column::Float(c) => c.capacity(),
            Column::Bool(c) => c.capacity(),
            Column::Str(c) => c.capacity(),
            Column::Bytes(c) => c.capacity(),
            Column::TinyInt(c) => c.capacity(),
            Column::SmallInt(c) => c.capacity(),
            Column::Int32(c) => c.capacity(),
            Column::HugeInt(c) => c.capacity(),
            Column::Float32(c) => c.capacity(),
            Column::Decimal(c) => c.capacity(),
            Column::Date(c) => c.capacity(),
            Column::Time(c) => c.capacity(),
            Column::DateTime(c) => c.capacity(),
            Column::Timestamp(c) => c.capacity(),
            Column::Interval(c) => c.capacity(),
            Column::Uuid(c) => c.capacity(),
            Column::Embedding(c) => c.capacity(),
            Column::Array(c) => c.capacity(),
            Column::Mixed(c) => c.capacity(),
            Column::Json(c) => c.capacity(),
            Column::RowId(c) => c.capacity(),
        }
    }

    /// C++ `ColumnVector::Initialize(vector_type, capacity)`.
    pub fn initialize(&mut self, vector_type: ColumnVectorType, capacity: usize) {
        match self {
            Column::Int(c) => c.initialize(vector_type, capacity),
            Column::Float(c) => c.initialize(vector_type, capacity),
            Column::Bool(c) => c.initialize(vector_type, capacity),
            Column::Str(c) => c.initialize(vector_type, capacity),
            Column::Bytes(c) => c.initialize(vector_type, capacity),
            Column::TinyInt(c) => c.initialize(vector_type, capacity),
            Column::SmallInt(c) => c.initialize(vector_type, capacity),
            Column::Int32(c) => c.initialize(vector_type, capacity),
            Column::HugeInt(c) => c.initialize(vector_type, capacity),
            Column::Float32(c) => c.initialize(vector_type, capacity),
            Column::Decimal(c) => c.initialize(vector_type, capacity),
            Column::Date(c) => c.initialize(vector_type, capacity),
            Column::Time(c) => c.initialize(vector_type, capacity),
            Column::DateTime(c) => c.initialize(vector_type, capacity),
            Column::Timestamp(c) => c.initialize(vector_type, capacity),
            Column::Interval(c) => c.initialize(vector_type, capacity),
            Column::Uuid(c) => c.initialize(vector_type, capacity),
            Column::Embedding(c) => c.initialize(vector_type, capacity),
            Column::Array(c) => c.initialize(vector_type, capacity),
            Column::Mixed(c) => c.initialize(vector_type, capacity),
            Column::Json(c) => c.initialize(vector_type, capacity),
            Column::RowId(c) => c.initialize(vector_type, capacity),
        }
    }

    /// C++ `ColumnVector::Reset`.
    pub fn reset(&mut self) {
        match self {
            Column::Int(c) => c.reset(),
            Column::Float(c) => c.reset(),
            Column::Bool(c) => c.reset(),
            Column::Str(c) => c.reset(),
            Column::Bytes(c) => c.reset(),
            Column::TinyInt(c) => c.reset(),
            Column::SmallInt(c) => c.reset(),
            Column::Int32(c) => c.reset(),
            Column::HugeInt(c) => c.reset(),
            Column::Float32(c) => c.reset(),
            Column::Decimal(c) => c.reset(),
            Column::Date(c) => c.reset(),
            Column::Time(c) => c.reset(),
            Column::DateTime(c) => c.reset(),
            Column::Timestamp(c) => c.reset(),
            Column::Interval(c) => c.reset(),
            Column::Uuid(c) => c.reset(),
            Column::Embedding(c) => c.reset(),
            Column::Array(c) => c.reset(),
            Column::Mixed(c) => c.reset(),
            Column::Json(c) => c.reset(),
            Column::RowId(c) => c.reset(),
        }
    }

    /// C++ `ColumnVector::Make(DataType)`: an uninitialized column of the
    /// given logical type (`None` for types without a typed column here).
    pub fn new(logical_type: LogicalType) -> Option<Self> {
        Some(match logical_type {
            LogicalType::Boolean => Column::Bool(ColumnVector::new()),
            LogicalType::TinyInt => Column::TinyInt(ColumnVector::new()),
            LogicalType::SmallInt => Column::SmallInt(ColumnVector::new()),
            LogicalType::Integer => Column::Int32(ColumnVector::new()),
            LogicalType::BigInt => Column::Int(ColumnVector::new()),
            LogicalType::HugeInt => Column::HugeInt(ColumnVector::new()),
            LogicalType::Float => Column::Float32(ColumnVector::new()),
            LogicalType::Double => Column::Float(ColumnVector::new()),
            LogicalType::Decimal => Column::Decimal(ColumnVector::new()),
            LogicalType::Varchar => Column::Str(ColumnVector::new()),
            LogicalType::Date => Column::Date(ColumnVector::new()),
            LogicalType::Time => Column::Time(ColumnVector::new()),
            LogicalType::DateTime => Column::DateTime(ColumnVector::new()),
            LogicalType::Timestamp => Column::Timestamp(ColumnVector::new()),
            LogicalType::Interval => Column::Interval(ColumnVector::new()),
            LogicalType::Uuid => Column::Uuid(ColumnVector::new()),
            LogicalType::Array => Column::Array(ColumnVector::new()),
            LogicalType::Mixed => Column::Mixed(ColumnVector::new()),
            LogicalType::Json => Column::Json(ColumnVector::new()),
            LogicalType::RowID => Column::RowId(ColumnVector::new()),
            _ => return None,
        })
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
            Column::Mixed(c) => c.get(row).map(|v| v.text()),
            Column::Json(c) => c.get(row).map(|v| v.compact()),
            Column::RowId(c) => c
                .get(row)
                .map(|v| format!("{},{}", v.segment_id, v.segment_offset)),
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
                v.values().iter().map(|x| x.heap_len() + 24).sum::<usize>() + v.validity().len()
            }
            Column::Json(v) => {
                v.values()
                    .iter()
                    .map(|x| x.compact().len() + 24)
                    .sum::<usize>()
                    + v.validity().len()
            }
            Column::RowId(v) => v.values().len() * 8 + v.validity().len(),
        }
    }

    /// Parses one text slot (the block/decoder path). A slot whose text does
    /// not parse becomes NULL, as in the reference decoder.
    pub fn push_text(&mut self, text: Option<&str>) {
        // Bytes and Mixed keep their own text handling: `Bytes` is a byte
        // string (not UTF-8 typed) and `Mixed` stores the text itself.
        match self {
            Column::Bytes(c) => {
                c.push(text.map(|t| t.as_bytes().to_vec()));
                return;
            }
            Column::Mixed(c) => {
                c.push(text.map(infinity_common::MixedT::new));
                return;
            }
            _ => {}
        }
        let value = text
            .and_then(|t| BlockValue::parse_text(t, self.logical_type()))
            .unwrap_or(BlockValue::Null);
        let _ = self.push_block_value(value);
    }

    /// Appends one typed slot (`BlockValue::Null` appends a NULL).
    pub fn push_block_value(&mut self, value: BlockValue) -> Result<(), String> {
        if value == BlockValue::Null {
            match self {
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
                Column::Json(c) => c.push(None),
                Column::Bytes(c) => c.push(None),
                Column::RowId(c) => c.push(None),
            }
            return Ok(());
        }
        match (self, value) {
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
            (Column::Json(c), BlockValue::Json(v)) => c.push(Some(v)),
            (Column::RowId(c), BlockValue::RowId(v)) => c.push(Some(v)),
            _ => return Err("value type does not match column".to_string()),
        }
        Ok(())
    }

    /// C++ `ColumnVector::GetValueByIndex`: the typed slot (`None` for NULL).
    pub fn get_value_by_index(&self, row: usize) -> Option<BlockValue> {
        match self {
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
            Column::RowId(c) => c.get(row).copied().map(BlockValue::RowId),
            Column::Date(c) => c.get(row).copied().map(BlockValue::Date),
            Column::Time(c) => c.get(row).copied().map(BlockValue::Time),
            Column::DateTime(c) => c.get(row).copied().map(BlockValue::DateTime),
            Column::Timestamp(c) => c.get(row).copied().map(BlockValue::Timestamp),
            Column::Interval(c) => c.get(row).copied().map(BlockValue::Interval),
            Column::Uuid(c) => c.get(row).copied().map(BlockValue::Uuid),
            Column::Embedding(c) => c.get(row).cloned().map(BlockValue::Embedding),
            Column::Array(c) => c.get(row).cloned().map(BlockValue::Array),
            Column::Mixed(c) => c.get(row).cloned().map(BlockValue::Mixed),
            Column::Json(c) => c.get(row).cloned().map(BlockValue::Json),
            Column::Bytes(c) => c
                .get(row)
                .cloned()
                .map(|v| BlockValue::Str(String::from_utf8_lossy(&v).into_owned())),
        }
    }

    /// The `u8` discriminator used by [`Self::write_adv`].
    fn tag(&self) -> u8 {
        match self {
            Column::Bool(_) => 1,
            Column::TinyInt(_) => 2,
            Column::SmallInt(_) => 3,
            Column::Int32(_) => 4,
            Column::Int(_) => 5,
            Column::Float32(_) => 6,
            Column::Float(_) => 7,
            Column::Str(_) => 8,
            Column::Decimal(_) => 9,
            Column::Date(_) => 10,
            Column::Time(_) => 11,
            Column::DateTime(_) => 12,
            Column::Timestamp(_) => 13,
            Column::Interval(_) => 14,
            Column::Uuid(_) => 15,
            Column::RowId(_) => 16,
            Column::HugeInt(_) => 17,
            Column::Bytes(_) => 18,
            Column::Embedding(_) => 19,
            Column::Array(_) => 20,
            Column::Mixed(_) => 21,
            Column::Json(_) => 22,
        }
    }

    /// C++ `ColumnVector::GetSizeInBytes`: the exact byte count
    /// [`Self::write_adv`] emits.
    ///
    /// The reference's byte layout (its `DataType`/`VectorBuffer`/`Bitmask`
    /// encodings) is not reproduced; this is the storage layer's own
    /// self-describing frame, registered as a design change.
    pub fn get_size_in_bytes(&self) -> Result<usize, String> {
        self.check_serializable()?;
        let rows = self.size();
        let constant = self.vector_type() == ColumnVectorType::Constant;
        let value_rows = if constant { 1 } else { rows };
        let payload = match self {
            Column::Bool(_) | Column::TinyInt(_) => value_rows,
            Column::SmallInt(_) => value_rows * 2,
            Column::Int32(_) | Column::Float32(_) | Column::Date(_) | Column::Time(_) => {
                value_rows * 4
            }
            Column::Int(_)
            | Column::Float(_)
            | Column::DateTime(_)
            | Column::Timestamp(_)
            | Column::RowId(_) => value_rows * 8,
            Column::Interval(_) => value_rows * 5,
            Column::Uuid(_) => value_rows * 16,
            Column::Decimal(_) => value_rows * 20,
            Column::Str(c) => {
                if constant {
                    c.get(0).map_or(4, |v| v.len() + 4)
                } else {
                    c.values().iter().map(|v| v.len() + 4).sum()
                }
            }
            Column::Bytes(c) => {
                if constant {
                    c.get(0).map_or(4, |v| v.len() + 4)
                } else {
                    c.values().iter().map(|v| v.len() + 4).sum()
                }
            }
            Column::Json(c) => {
                if constant {
                    c.get(0).map_or(4, |v| v.compact().len() + 4)
                } else {
                    c.values().iter().map(|v| v.compact().len() + 4).sum()
                }
            }
            Column::HugeInt(_) | Column::Embedding(_) | Column::Array(_) | Column::Mixed(_) => {
                unreachable!("rejected by check_serializable")
            }
        };
        Ok(1 + 1 + 4 + rows.div_ceil(8) + payload)
    }

    fn check_serializable(&self) -> Result<(), String> {
        if !matches!(
            self.vector_type(),
            ColumnVectorType::Flat | ColumnVectorType::Constant
        ) {
            return Err(format!(
                "Not supported vector_type {:?}",
                self.vector_type()
            ));
        }
        if matches!(self, Column::HugeInt(_)) {
            // Same restriction as the reference `WriteAdv`.
            return Err("Attempt to serialize huge integer type".to_string());
        }
        if matches!(
            self,
            Column::Embedding(_) | Column::Array(_) | Column::Mixed(_)
        ) {
            return Err("Attempt to serialize a column with a variable-size payload".to_string());
        }
        Ok(())
    }

    /// Whether the row holds a value (the C++ null bitmap bit).
    fn row_is_valid(&self, row: usize) -> bool {
        match self {
            Column::Bool(c) => c.get(row).is_some(),
            Column::TinyInt(c) => c.get(row).is_some(),
            Column::SmallInt(c) => c.get(row).is_some(),
            Column::Int32(c) => c.get(row).is_some(),
            Column::Int(c) => c.get(row).is_some(),
            Column::HugeInt(c) => c.get(row).is_some(),
            Column::Float32(c) => c.get(row).is_some(),
            Column::Float(c) => c.get(row).is_some(),
            Column::Decimal(c) => c.get(row).is_some(),
            Column::Str(c) => c.get(row).is_some(),
            Column::Bytes(c) => c.get(row).is_some(),
            Column::Date(c) => c.get(row).is_some(),
            Column::Time(c) => c.get(row).is_some(),
            Column::DateTime(c) => c.get(row).is_some(),
            Column::Timestamp(c) => c.get(row).is_some(),
            Column::Interval(c) => c.get(row).is_some(),
            Column::Uuid(c) => c.get(row).is_some(),
            Column::Embedding(c) => c.get(row).is_some(),
            Column::Array(c) => c.get(row).is_some(),
            Column::Mixed(c) => c.get(row).is_some(),
            Column::Json(c) => c.get(row).is_some(),
            Column::RowId(c) => c.get(row).is_some(),
        }
    }

    /// C++ `ColumnVector::WriteAdv`: appends the frame to `out`.
    pub fn write_adv(&self, out: &mut Vec<u8>) -> Result<(), String> {
        self.check_serializable()?;
        let rows = self.size();
        let constant = self.vector_type() == ColumnVectorType::Constant;
        let value_rows = if constant { 1 } else { rows };
        out.push(self.tag());
        out.push(u8::from(constant));
        out.extend_from_slice(&(rows as u32).to_le_bytes());
        let mut bitmap = vec![0u8; rows.div_ceil(8)];
        for row in 0..rows {
            if self.row_is_valid(row) {
                bitmap[row / 8] |= 1 << (row % 8);
            }
        }
        out.extend_from_slice(&bitmap);

        macro_rules! write_pod {
            ($c:expr, $conv:expr) => {
                for row in 0..value_rows {
                    out.extend_from_slice(&$conv(value_at($c, row)));
                }
            };
        }
        match self {
            Column::Bool(c) => write_pod!(c, |v: bool| [u8::from(v)]),
            Column::TinyInt(c) => write_pod!(c, |v: i8| v.to_le_bytes()),
            Column::SmallInt(c) => write_pod!(c, |v: i16| v.to_le_bytes()),
            Column::Int32(c) => write_pod!(c, |v: i32| v.to_le_bytes()),
            Column::Int(c) => write_pod!(c, |v: i64| v.to_le_bytes()),
            Column::Float32(c) => write_pod!(c, |v: f32| v.to_bits().to_le_bytes()),
            Column::Float(c) => write_pod!(c, |v: f64| v.to_bits().to_le_bytes()),
            Column::Str(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&(value.len() as u32).to_le_bytes());
                    out.extend_from_slice(value.as_bytes());
                }
            }
            Column::Bytes(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&(value.len() as u32).to_le_bytes());
                    out.extend_from_slice(&value);
                }
            }
            Column::Json(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row).compact();
                    out.extend_from_slice(&(value.len() as u32).to_le_bytes());
                    out.extend_from_slice(value.as_bytes());
                }
            }
            Column::Decimal(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&value.value.to_le_bytes());
                    out.extend_from_slice(&value.scale.to_le_bytes());
                }
            }
            Column::Date(c) => write_pod!(c, |v: infinity_common::DateT| v.value.to_le_bytes()),
            Column::Time(c) => write_pod!(c, |v: infinity_common::TimeT| v.value.to_le_bytes()),
            Column::DateTime(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&value.date.to_le_bytes());
                    out.extend_from_slice(&value.time.to_le_bytes());
                }
            }
            Column::Timestamp(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&value.date.to_le_bytes());
                    out.extend_from_slice(&value.time.to_le_bytes());
                }
            }
            Column::Interval(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&value.value.to_le_bytes());
                    out.push(value.unit as u8);
                }
            }
            Column::Uuid(c) => {
                for row in 0..value_rows {
                    out.extend_from_slice(&value_at(c, row));
                }
            }
            Column::RowId(c) => {
                for row in 0..value_rows {
                    let value = value_at(c, row);
                    out.extend_from_slice(&value.segment_id.to_le_bytes());
                    out.extend_from_slice(&value.segment_offset.to_le_bytes());
                }
            }
            Column::HugeInt(_) | Column::Embedding(_) | Column::Array(_) | Column::Mixed(_) => {
                unreachable!("rejected by check_serializable")
            }
        }
        debug_assert_eq!(out.len(), self.get_size_in_bytes()?);
        Ok(())
    }

    /// C++ `ColumnVector::ReadAdv`: parses one frame written by
    /// [`Self::write_adv`]; returns the column and the consumed byte count.
    pub fn read_adv(input: &[u8]) -> Option<(Column, usize)> {
        let mut cursor = Cursor { buf: input, pos: 0 };
        let tag = cursor.u8()?;
        let constant = match cursor.u8()? {
            0 => false,
            1 => true,
            _ => return None,
        };
        let rows = cursor.u32()? as usize;
        let bitmap = cursor.take(rows.div_ceil(8))?.to_vec();
        let valid = |row: usize| {
            bitmap
                .get(row / 8)
                .is_some_and(|byte| byte & (1 << (row % 8)) != 0)
        };
        let mut column = match tag {
            1 => Column::Bool(ColumnVector::new()),
            2 => Column::TinyInt(ColumnVector::new()),
            3 => Column::SmallInt(ColumnVector::new()),
            4 => Column::Int32(ColumnVector::new()),
            5 => Column::Int(ColumnVector::new()),
            6 => Column::Float32(ColumnVector::new()),
            7 => Column::Float(ColumnVector::new()),
            8 => Column::Str(ColumnVector::new()),
            9 => Column::Decimal(ColumnVector::new()),
            10 => Column::Date(ColumnVector::new()),
            11 => Column::Time(ColumnVector::new()),
            12 => Column::DateTime(ColumnVector::new()),
            13 => Column::Timestamp(ColumnVector::new()),
            14 => Column::Interval(ColumnVector::new()),
            15 => Column::Uuid(ColumnVector::new()),
            16 => Column::RowId(ColumnVector::new()),
            18 => Column::Bytes(ColumnVector::new()),
            22 => Column::Json(ColumnVector::new()),
            _ => return None,
        };
        column.initialize(
            if constant {
                ColumnVectorType::Constant
            } else {
                ColumnVectorType::Flat
            },
            DEFAULT_VECTOR_SIZE,
        );
        let value_rows = if constant { 1 } else { rows };
        macro_rules! read_pod {
            ($c:expr, $t:ty) => {
                for row in 0..value_rows {
                    let bytes = cursor.take(std::mem::size_of::<$t>())?;
                    let value = <$t>::from_le_bytes(bytes.try_into().ok()?);
                    $c.push(valid(row).then_some(value));
                }
            };
        }
        match &mut column {
            Column::Bool(c) => {
                for row in 0..value_rows {
                    let value = cursor.u8()? != 0;
                    c.push(valid(row).then_some(value));
                }
            }
            Column::TinyInt(c) => read_pod!(c, i8),
            Column::SmallInt(c) => read_pod!(c, i16),
            Column::Int32(c) => read_pod!(c, i32),
            Column::Int(c) => read_pod!(c, i64),
            Column::Float32(c) => {
                for row in 0..value_rows {
                    let bits = u32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(f32::from_bits(bits)));
                }
            }
            Column::Float(c) => {
                for row in 0..value_rows {
                    let bits = u64::from_le_bytes(cursor.take(8)?.try_into().ok()?);
                    c.push(valid(row).then_some(f64::from_bits(bits)));
                }
            }
            Column::Str(c) => {
                for row in 0..value_rows {
                    let len = cursor.u32()? as usize;
                    let text = String::from_utf8(cursor.take(len)?.to_vec()).ok()?;
                    c.push(valid(row).then_some(text));
                }
            }
            Column::Bytes(c) => {
                for row in 0..value_rows {
                    let len = cursor.u32()? as usize;
                    c.push(valid(row).then_some(cursor.take(len)?.to_vec()));
                }
            }
            Column::Json(c) => {
                for row in 0..value_rows {
                    let len = cursor.u32()? as usize;
                    let text = std::str::from_utf8(cursor.take(len)?).ok()?;
                    let value = infinity_common::json::parse(text)?;
                    c.push(valid(row).then_some(value));
                }
            }
            Column::Decimal(c) => {
                for row in 0..value_rows {
                    let value = i128::from_le_bytes(cursor.take(16)?.try_into().ok()?);
                    let scale = u32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(infinity_common::DecimalT::new(value, scale)));
                }
            }
            Column::Date(c) => {
                for row in 0..value_rows {
                    let value = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(infinity_common::DateT::new(value)));
                }
            }
            Column::Time(c) => {
                for row in 0..value_rows {
                    let value = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(infinity_common::TimeT::new(value)));
                }
            }
            Column::DateTime(c) => {
                for row in 0..value_rows {
                    let date = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    let time = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(infinity_common::DateTimeT::new(date, time)));
                }
            }
            Column::Timestamp(c) => {
                for row in 0..value_rows {
                    let date = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    let time = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(infinity_common::TimestampT::new(date, time)));
                }
            }
            Column::Interval(c) => {
                for row in 0..value_rows {
                    let value = i32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    let unit = match cursor.u8()? {
                        1 => infinity_common::TimeUnit::Year,
                        2 => infinity_common::TimeUnit::Month,
                        3 => infinity_common::TimeUnit::Day,
                        4 => infinity_common::TimeUnit::Hour,
                        5 => infinity_common::TimeUnit::Minute,
                        6 => infinity_common::TimeUnit::Second,
                        _ => infinity_common::TimeUnit::Invalid,
                    };
                    c.push(
                        valid(row).then_some(infinity_common::IntervalT::with_unit(value, unit)),
                    );
                }
            }
            Column::Uuid(c) => {
                for row in 0..value_rows {
                    let mut value = [0u8; 16];
                    value.copy_from_slice(cursor.take(16)?);
                    c.push(valid(row).then_some(value));
                }
            }
            Column::RowId(c) => {
                for row in 0..value_rows {
                    let segment_id = u32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    let segment_offset = u32::from_le_bytes(cursor.take(4)?.try_into().ok()?);
                    c.push(valid(row).then_some(RowIdT::new(segment_id, segment_offset)));
                }
            }
            Column::HugeInt(_) | Column::Embedding(_) | Column::Array(_) | Column::Mixed(_) => {
                return None
            }
        }
        if constant {
            // `push` fills a flat tail; rebuild the constant vector from its
            // single stored row.
            macro_rules! to_constant {
                ($c:expr) => {{
                    let value = $c.values().first().cloned().filter(|_| valid(0));
                    $c.clear();
                    $c.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
                    $c.set_value(0, value).ok()?;
                    $c.finalize(rows);
                }};
            }
            match &mut column {
                Column::Bool(c) => to_constant!(c),
                Column::TinyInt(c) => to_constant!(c),
                Column::SmallInt(c) => to_constant!(c),
                Column::Int32(c) => to_constant!(c),
                Column::Int(c) => to_constant!(c),
                Column::Float32(c) => to_constant!(c),
                Column::Float(c) => to_constant!(c),
                Column::Str(c) => to_constant!(c),
                Column::Bytes(c) => to_constant!(c),
                Column::Decimal(c) => to_constant!(c),
                Column::Date(c) => to_constant!(c),
                Column::Time(c) => to_constant!(c),
                Column::DateTime(c) => to_constant!(c),
                Column::Timestamp(c) => to_constant!(c),
                Column::Interval(c) => to_constant!(c),
                Column::Uuid(c) => to_constant!(c),
                Column::Json(c) => to_constant!(c),
                Column::RowId(c) => to_constant!(c),
                Column::HugeInt(_) | Column::Embedding(_) | Column::Array(_) | Column::Mixed(_) => {
                    return None
                }
            }
        }
        Some((column, cursor.pos))
    }
}

/// The value at `row`, or the placeholder written for a NULL slot.
fn value_at<T: Clone + Default>(c: &ColumnVector<T>, row: usize) -> T {
    c.get(row).cloned().unwrap_or_default()
}

struct Cursor<'a> {
    buf: &'a [u8],
    pos: usize,
}

impl<'a> Cursor<'a> {
    fn u8(&mut self) -> Option<u8> {
        let value = *self.buf.get(self.pos)?;
        self.pos += 1;
        Some(value)
    }

    fn u32(&mut self) -> Option<u32> {
        Some(u32::from_le_bytes(self.take(4)?.try_into().ok()?))
    }

    fn take(&mut self, len: usize) -> Option<&'a [u8]> {
        let end = self.pos.checked_add(len)?;
        let slice = self.buf.get(self.pos..end)?;
        self.pos = end;
        Some(slice)
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
    /// A parsed JSON document (C++ `Value` of `LogicalType::kJson`).
    Json(infinity_common::json::Json),
    RowId(RowIdT),
    Null,
}

impl BlockValue {
    /// C++ `Value::StringToValue`: parses one text slot into the declared
    /// logical type. `None` for text that does not parse or for types without
    /// a text form here.
    pub fn parse_text(text: &str, logical_type: LogicalType) -> Option<BlockValue> {
        Some(match logical_type {
            LogicalType::Boolean => BlockValue::Bool(match text.trim() {
                "true" | "1" => true,
                "false" | "0" => false,
                _ => return None,
            }),
            LogicalType::TinyInt => BlockValue::TinyInt(text.trim().parse().ok()?),
            LogicalType::SmallInt => BlockValue::SmallInt(text.trim().parse().ok()?),
            LogicalType::Integer => BlockValue::Int32(text.trim().parse().ok()?),
            LogicalType::BigInt => BlockValue::Int(text.trim().parse().ok()?),
            LogicalType::HugeInt => BlockValue::HugeInt(text.trim().parse().ok()?),
            LogicalType::Float => BlockValue::Float(text.trim().parse().ok()?),
            LogicalType::Double => BlockValue::Double(text.trim().parse().ok()?),
            LogicalType::Varchar => BlockValue::Str(text.to_string()),
            LogicalType::Json => BlockValue::Json(infinity_common::json::parse(text)?),
            LogicalType::Date => {
                let mut date = infinity_common::DateT::default();
                date.from_string(text).ok()?;
                BlockValue::Date(date)
            }
            LogicalType::Time => BlockValue::Time(parse_time_text(text)?),
            LogicalType::DateTime => BlockValue::DateTime(parse_datetime_text(text)?),
            LogicalType::Timestamp => BlockValue::Timestamp(parse_timestamp_text(text)?),
            _ => return None,
        })
    }
}

/// C++ `TimeType::ConvertFromString` (`HH:MM:SS`): seconds since midnight.
fn parse_time_text(text: &str) -> Option<infinity_common::TimeT> {
    let mut parts = text.trim().split(':');
    let hour: i32 = parts.next()?.trim().parse().ok()?;
    let minute: i32 = parts.next()?.trim().parse().ok()?;
    let second: i32 = parts.next()?.trim().parse().ok()?;
    if parts.next().is_some() || !(0..=23).contains(&hour) || !(0..=59).contains(&minute) {
        return None;
    }
    if !(0..=59).contains(&second) {
        return None;
    }
    Some(infinity_common::TimeT::new(
        hour * 3600 + minute * 60 + second,
    ))
}

/// C++ `DateTimeType::FromString`: `YYYY-MM-DD HH:MM:SS`, with the time part
/// optional and defaulting to midnight.
fn parse_datetime_text(text: &str) -> Option<infinity_common::DateTimeT> {
    let (date_text, time_text) = split_datetime_text(text)?;
    let mut date = infinity_common::DateT::default();
    date.from_string(date_text).ok()?;
    let time = match time_text {
        Some(t) => parse_time_text(t)?,
        None => infinity_common::TimeT::new(0),
    };
    Some(infinity_common::DateTimeT::new(date.value, time.value))
}

/// C++ `TimestampType::FromString`: the same `YYYY-MM-DD HH:MM:SS` layout.
fn parse_timestamp_text(text: &str) -> Option<infinity_common::TimestampT> {
    let (date_text, time_text) = split_datetime_text(text)?;
    let mut date = infinity_common::DateT::default();
    date.from_string(date_text).ok()?;
    let time = match time_text {
        Some(t) => parse_time_text(t)?,
        None => infinity_common::TimeT::new(0),
    };
    Some(infinity_common::TimestampT::new(date.value, time.value))
}

fn split_datetime_text(text: &str) -> Option<(&str, Option<&str>)> {
    let text = text.trim();
    match text.find(' ') {
        Some(idx) => {
            let rest = text[idx + 1..].trim();
            Some((text[..idx].trim(), (!rest.is_empty()).then_some(rest)))
        }
        None => Some((text, None)),
    }
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

    /// C++: `DataBlock::AppendColumns` — appends the selected columns of
    /// `other`. The two blocks must hold the same number of rows.
    pub fn append_columns(&mut self, other: &DataBlock, indexes: &[usize]) -> bool {
        if self.row_count != other.row_count {
            return false;
        }
        let mut added = Vec::with_capacity(indexes.len());
        for &idx in indexes {
            match other.columns.get(idx) {
                Some(column) => added.push(column.clone()),
                None => return false,
            }
        }
        self.columns.extend(added);
        true
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
        col.push_block_value(value)
            .map_err(|_| format!("value type does not match column {column}"))
    }

    /// C++ `DataBlock::GetValue(column, row)`.
    pub fn get_value(&self, column: usize, row: usize) -> Option<BlockValue> {
        self.columns.get(column)?.get_value_by_index(row)
    }

    /// Approximate heap bytes, for the buffer-manager budget.
    pub fn mem_used(&self) -> usize {
        self.columns.iter().map(Column::mem_used).sum()
    }

    /// C++ `DataBlock::FillRowIDVector` (`data_block_impl.cpp:254`): appends one
    /// `(INVALID_SEGMENT_ID, block_id * DEFAULT_BLOCK_CAPACITY + offset)` id per
    /// row. The intermediate table uses these ids for its order-by result.
    pub fn fill_row_id_vector(&self, row_ids: &mut Vec<RowIdT>, block_id: u32) {
        let start = block_id * crate::cpp_format::DEFAULT_BLOCK_CAPACITY as u32;
        for offset in 0..self.row_count as u32 {
            row_ids.push(RowIdT::new(u32::MAX, start + offset));
        }
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
            BlockValue::Decimal(DecimalT::new(i as i128, 0)),
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

    /// Builds a flat column vector from owned values (test helper).
    fn cv<T: Clone + Default>(values: Vec<T>) -> ColumnVector<T> {
        let mut v = ColumnVector::new();
        v.initialize_flat();
        for value in values {
            v.push(Some(value));
        }
        v
    }

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

    // C++: `column_vector_decimal_ut_nimpl.cpp` (flat/constant/select/slice).
    #[test]
    fn decimal_flat_constant_select_slice() {
        flat_suite(|i| infinity_common::DecimalT::new(i as i128, 0));
        constant_suite(|i| infinity_common::DecimalT::new(i as i128, 0));
        select_suite(|i| infinity_common::DecimalT::new(i as i128, 0));
        slice_suite(|i| infinity_common::DecimalT::new(i as i128, 0));
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
    // C++: `expression_executor_select_ut.cpp::test1` — the selector call is
    // `#if 0` in the reference; only the boolean input vector is asserted.
    #[test]
    fn expression_select_alternating_input() {
        let mut v = ColumnVector::<bool>::new();
        v.initialize_flat();
        for i in 0..DEFAULT_VECTOR_SIZE {
            v.append_value(Some(i % 2 == 0)).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(v.get(i), Some(&(i % 2 == 0)));
        }
    }

    // C++: `expression_executor_select_ut.cpp::test2` (all true).
    #[test]
    fn expression_select_all_true_input() {
        let mut v = ColumnVector::<bool>::new();
        v.initialize_flat();
        for _ in 0..DEFAULT_VECTOR_SIZE {
            v.append_value(Some(true)).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(v.get(i), Some(&true));
        }
    }

    // C++: `expression_executor_select_ut.cpp::test3` (all false).
    #[test]
    fn expression_select_all_false_input() {
        let mut v = ColumnVector::<bool>::new();
        v.initialize_flat();
        for _ in 0..DEFAULT_VECTOR_SIZE {
            v.append_value(Some(false)).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(v.get(i), Some(&false));
        }
    }
    /// C++: `column_vector/value_ut.cpp::ValueStringTest/test_tostring` — the
    /// scalar value renderings (the JSON case has no `Column` variant; JSON is
    /// stored as text and rendered verbatim).
    #[test]
    #[allow(clippy::approx_constant)]
    fn value_to_string_matches_the_reference() {
        let cases: Vec<(Column, &str)> = vec![
            (Column::Bool(cv(vec![true])), "true"),
            (Column::Bool(cv(vec![false])), "false"),
            (Column::TinyInt(cv(vec![127])), "127"),
            (Column::TinyInt(cv(vec![-128])), "-128"),
            (Column::SmallInt(cv(vec![32767])), "32767"),
            (Column::SmallInt(cv(vec![-32768])), "-32768"),
            (Column::Int32(cv(vec![2147483647])), "2147483647"),
            (Column::Int32(cv(vec![-2147483648])), "-2147483648"),
            (Column::Int(cv(vec![i64::MAX])), "9223372036854775807"),
            (Column::Int(cv(vec![i64::MIN])), "-9223372036854775808"),
            (Column::Float32(cv(vec![3.14159f32])), "3.14159"),
            (Column::Float(cv(vec![-2.71828f64])), "-2.71828"),
            (
                Column::Float(cv(vec![3.141592653589793f64])),
                "3.141592653589793",
            ),
            (
                Column::Str(cv(vec!["Hello, World!".to_string()])),
                "Hello, World!",
            ),
            (Column::Str(cv(vec![String::new()])), ""),
        ];
        for (column, want) in cases {
            assert_eq!(column.value_text(0).as_deref(), Some(want));
        }
    }

    /// C++: `value_ut.cpp::ValueStringTest/test_stringtovalue` — text parses
    /// into the declared scalar type, including a parsed JSON document.
    #[test]
    #[allow(clippy::approx_constant)]
    fn string_to_value_roundtrips_the_scalar_types() {
        use infinity_common::{DateT, DateTimeT, TimeT, TimestampT};

        let parse = |text: &str, ty: LogicalType| {
            BlockValue::parse_text(text, ty).unwrap_or_else(|| panic!("{text} does not parse"))
        };
        assert_eq!(parse("true", LogicalType::Boolean), BlockValue::Bool(true));
        assert_eq!(
            parse("false", LogicalType::Boolean),
            BlockValue::Bool(false)
        );

        assert_eq!(parse("127", LogicalType::TinyInt), BlockValue::TinyInt(127));
        assert_eq!(
            parse("-128", LogicalType::TinyInt),
            BlockValue::TinyInt(-128)
        );
        assert_eq!(
            parse("32767", LogicalType::SmallInt),
            BlockValue::SmallInt(32767)
        );
        assert_eq!(
            parse("-32768", LogicalType::SmallInt),
            BlockValue::SmallInt(-32768)
        );
        assert_eq!(
            parse("2147483647", LogicalType::Integer),
            BlockValue::Int32(2147483647)
        );
        assert_eq!(
            parse("-2147483648", LogicalType::Integer),
            BlockValue::Int32(-2147483648)
        );
        assert_eq!(
            parse("9223372036854775807", LogicalType::BigInt),
            BlockValue::Int(i64::MAX)
        );
        assert_eq!(
            parse("-9223372036854775808", LogicalType::BigInt),
            BlockValue::Int(i64::MIN)
        );

        assert_eq!(
            parse("3.14159", LogicalType::Float),
            BlockValue::Float(3.14159)
        );
        assert_eq!(
            parse("-2.71828", LogicalType::Float),
            BlockValue::Float(-2.71828)
        );
        assert_eq!(
            parse("3.141592653589793", LogicalType::Double),
            BlockValue::Double(3.141592653589793)
        );
        assert_eq!(
            parse("-2.718281828459045", LogicalType::Double),
            BlockValue::Double(-2.718281828459045)
        );

        assert_eq!(
            parse("Hello, World!", LogicalType::Varchar),
            BlockValue::Str("Hello, World!".to_string())
        );
        assert_eq!(
            parse("", LogicalType::Varchar),
            BlockValue::Str(String::new())
        );

        // JSON: the reference compares the parsed value with
        // `Value::MakeJson(JsonManager::to_bson(nlohmann::json::parse(...)))`.
        {
            let text = "{\"key\": \"value\", \"number\": 42}";
            let expected = infinity_common::json::parse(text).unwrap();
            assert_eq!(
                parse(text, LogicalType::Json),
                BlockValue::Json(expected.clone())
            );
            let mut column = Column::new(LogicalType::Json).unwrap();
            column.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
            column.push_block_value(BlockValue::Json(expected.clone())).unwrap();
            assert_eq!(
                column.get_value_by_index(0),
                Some(BlockValue::Json(expected.clone()))
            );
            assert_eq!(column.value_text(0), Some(expected.compact()));
            assert_eq!(
                column.get_value_by_index(1),
                None,
                "unwritten JSON row stays NULL"
            );
        }

        let mut date = DateT::default();
        date.from_string("2023-12-25").unwrap();
        assert_eq!(
            parse("2023-12-25", LogicalType::Date),
            BlockValue::Date(date)
        );

        // `TimeType` keeps seconds since midnight.
        assert_eq!(
            parse("14:30:45", LogicalType::Time),
            BlockValue::Time(TimeT::new(14 * 3600 + 30 * 60 + 45))
        );

        let mut datetime = DateT::default();
        datetime.from_string("2023-12-25").unwrap();
        let time = TimeT::new(14 * 3600 + 30 * 60 + 45);
        assert_eq!(
            parse("2023-12-25 14:30:45", LogicalType::DateTime),
            BlockValue::DateTime(DateTimeT::new(datetime.value, time.value))
        );
        assert_eq!(
            parse("2023-12-25 14:30:45", LogicalType::Timestamp),
            BlockValue::Timestamp(TimestampT::new(datetime.value, time.value))
        );

        // The column decoder path stores the same parsed values.
        let mut column = Column::new(LogicalType::Time).unwrap();
        column.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        column.push_text(Some("14:30:45"));
        assert_eq!(
            column.push_block_value(BlockValue::Null),
            Ok(()),
            "NULL stays appendable"
        );
        assert_eq!(column.value_text(0).as_deref(), Some("52245"));
        assert_eq!(column.value_text(1), None);
    }

    // ---------------------------------------------------------------------
    // `src/unit_test/storage/column_vector/column_vector_row_ut.cpp`
    // ---------------------------------------------------------------------

    /// C++: `ColumnVectorRowTest.flat_row`.
    #[test]
    fn row_flat() {
        assert_eq!(std::mem::size_of::<RowIdT>(), 8);
        let mut v = ColumnVector::<RowIdT>::new();
        v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Flat).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Flat);
        assert_eq!(
            Column::RowId(ColumnVector::new()).logical_type(),
            LogicalType::RowID
        );
        assert_eq!(v.null_count(), 0);

        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            v.append_value(Some(RowIdT::new(i, i))).unwrap();
            let vx = v.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, i);
            assert_eq!(vx.segment_offset, i);
            assert!(v.get_value_by_index(i as usize + 1).is_none());
        }

        // C++ `ShallowCopy` shares the buffer; the Rust port copies it.
        let mut clone = ColumnVector::<RowIdT>::new();
        clone.shallow_copy(&v);
        assert_eq!(clone.size(), v.size());
        assert_eq!(clone.capacity(), v.capacity());
        assert_eq!(clone.vector_type(), v.vector_type());
        assert_eq!(clone.null_count(), v.null_count());
        assert_eq!(clone.values(), v.values());
        assert_eq!(clone.validity(), v.validity());
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            let vx = v.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, i);
            assert_eq!(vx.segment_offset, i);
        }

        v.reset();
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.size(), 0);
        assert_eq!(v.vector_type(), ColumnVectorType::Invalid);
        assert!(v.values().is_empty());

        // Second half: re-initialize and append through the pointer path
        // (`AppendByPtr` == `push`/`append_value` for a flat vector).
        v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Flat).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Flat);
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            v.append_value(Some(RowIdT::new(i, i))).unwrap();
            let vx = v.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, i);
            assert_eq!(vx.segment_offset, i);
            assert!(v.get_value_by_index(i as usize + 1).is_none());
        }

        // Constant tail: SetValueByIndex(0, ...) then Finalize(1) per row.
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            let mut c = ColumnVector::<RowIdT>::new();
            c.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
            c.set_value(0, v.get_value_by_index(i as usize).cloned())
                .unwrap();
            c.finalize(1);
            let vx = c.get_value_by_index(0).unwrap();
            assert_eq!(vx.segment_id, i);
            assert_eq!(vx.segment_offset, i);
            c.reset();
        }
    }

    /// C++: `ColumnVectorRowTest.contant_row`.
    #[test]
    fn row_constant() {
        let mut v = ColumnVector::<RowIdT>::new();
        v.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Constant).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.size(), 0);
        assert_eq!(v.vector_type(), ColumnVectorType::Constant);
        assert_eq!(v.null_count(), 0);

        v.append_value(Some(RowIdT::new(0, 0))).unwrap();
        assert_eq!(v.get_value_by_index(0), Some(&RowIdT::new(0, 0)));
        assert!(v.get_value_by_index(1).is_none());

        v.reset();
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.size(), 0);
        assert_eq!(v.vector_type(), ColumnVectorType::Invalid);

        v.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Constant).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Constant);

        v.append_value(Some(RowIdT::new(0, 0))).unwrap();
        assert_eq!(v.get_value_by_index(0), Some(&RowIdT::new(0, 0)));
        assert!(v.get_value_by_index(1).is_none());
    }

    /// C++: `ColumnVectorRowTest.row_column_vector_select`.
    #[test]
    fn row_column_vector_select() {
        let mut v = ColumnVector::<RowIdT>::new();
        v.initialize_flat();
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            v.append_value(Some(RowIdT::new(i, i))).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            let vx = v.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, i);
            assert_eq!(vx.segment_offset, i);
        }

        let selection: Vec<usize> = (0..DEFAULT_VECTOR_SIZE / 2).map(|i| i * 2).collect();
        let mut target = ColumnVector::<RowIdT>::new();
        target.initialize_selection(&v, &selection);
        assert_eq!(target.size(), DEFAULT_VECTOR_SIZE / 2);
        for i in 0..DEFAULT_VECTOR_SIZE as u32 / 2 {
            let vx = target.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, 2 * i);
            assert_eq!(vx.segment_offset, 2 * i);
        }
    }

    /// C++: `ColumnVectorRowTest.row_column_slice_init`.
    #[test]
    fn row_column_slice_init() {
        let mut v = ColumnVector::<RowIdT>::new();
        v.initialize_flat();
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            v.append_value(Some(RowIdT::new(i, i))).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE as u32 {
            let vx = v.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, i);
            assert_eq!(vx.segment_offset, i);
        }

        let start = DEFAULT_VECTOR_SIZE / 4;
        let end = 3 * DEFAULT_VECTOR_SIZE / 4;
        let count = end - start;
        let mut target = ColumnVector::<RowIdT>::new();
        target.initialize_slice(&v, start, end);
        assert_eq!(target.size(), DEFAULT_VECTOR_SIZE / 2);
        assert_eq!(count, DEFAULT_VECTOR_SIZE / 2);
        // The reference test inspects the source vector here; the target is
        // the vector under test, so assert both.
        for i in 0..count as u32 {
            let source = v.get_value_by_index(i as usize).unwrap();
            assert_eq!(source.segment_id, i);
            assert_eq!(source.segment_offset, i);
            let vx = target.get_value_by_index(i as usize).unwrap();
            assert_eq!(vx.segment_id, start as u32 + i);
            assert_eq!(vx.segment_offset, start as u32 + i);
        }
    }

    // ---------------------------------------------------------------------
    // `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp`
    //
    // The reference distinguishes an inline (<= 16 byte) payload from an
    // out-of-line one; a Rust `String` owns its bytes, so the four structural
    // cases reduce to the same API assertions with the reference's payloads.
    // ---------------------------------------------------------------------

    /// The flat / select / slice / constant body shared by the three varchar
    /// cases (`flat_inline_varchar` is covered by
    /// `varchar_flat_constant_select_slice`; these use the longer payloads).
    fn varchar_flat_suite(payload: impl Fn(i64) -> String) {
        let mut v = ColumnVector::<String>::new();
        v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Flat).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Flat);
        assert_eq!(
            Column::Str(ColumnVector::new()).logical_type(),
            LogicalType::Varchar
        );
        assert_eq!(v.null_count(), 0);

        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            let s = payload(i);
            v.append_value(Some(s.clone())).unwrap();
            assert_eq!(v.get_value_by_index(i as usize), Some(&s));
            assert!(v.get_value_by_index(i as usize + 1).is_none());
        }

        // C++ `ShallowCopy` shares the buffer; the Rust port copies it.
        let mut clone = ColumnVector::<String>::new();
        clone.shallow_copy(&v);
        assert_eq!(clone.size(), v.size());
        assert_eq!(clone.capacity(), v.capacity());
        assert_eq!(clone.vector_type(), v.vector_type());
        assert_eq!(clone.null_count(), v.null_count());
        assert_eq!(clone.values(), v.values());
        assert_eq!(clone.validity(), v.validity());
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            assert_eq!(v.get(i as usize), Some(&payload(i)));
        }

        v.reset();
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.size(), 0);
        assert_eq!(v.vector_type(), ColumnVectorType::Invalid);
        assert!(v.values().is_empty());

        // Second half: re-initialize, then append through
        // `AppendByStringView` (the same `push` path).
        v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Flat).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Flat);
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            let s = payload(i);
            v.append_value(Some(s.clone())).unwrap();
            assert_eq!(v.get_value_by_index(i as usize), Some(&s));
            assert!(v.get_value_by_index(i as usize + 1).is_none());
        }

        // Constant tail: SetValueByIndex(0, ...) then Finalize(1) per row.
        for i in 0..DEFAULT_VECTOR_SIZE as i64 {
            let s = payload(i);
            let mut c = ColumnVector::<String>::new();
            c.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
            c.set_value(0, v.get_value_by_index(i as usize).cloned())
                .unwrap();
            c.finalize(1);
            assert_eq!(c.get_value_by_index(0), Some(&s));
            c.reset();
        }
    }

    /// C++: `ColumnVectorVarcharTest.flat_not_inline_varchar`.
    #[test]
    fn varchar_flat_not_inline() {
        varchar_flat_suite(|i| format!("hellohellohello{i}"));
    }

    /// C++: `ColumnVectorVarcharTest.flat_mixed_inline_varchar`.
    #[test]
    fn varchar_flat_mixed_inline() {
        varchar_flat_suite(|i| format!("Professional{i}"));
    }

    /// C++: `ColumnVectorVarcharTest.constant_not_inline_varchar`.
    #[test]
    fn varchar_constant_not_inline() {
        let payload = |i: i64| format!("hellohellohello{i}");
        let mut v = ColumnVector::<String>::new();
        v.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Constant).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Constant);
        assert_eq!(v.null_count(), 0);

        let s = payload(0);
        v.append_value(Some(s.clone())).unwrap();
        assert!(v.append_value(Some(s.clone())).is_err());
        assert_eq!(v.get_value_by_index(0), Some(&s));
        assert!(v.get_value_by_index(1).is_none());
        assert_eq!(v.get_value_by_index(0), Some(&s));

        v.reset();
        assert_eq!(v.capacity(), 0);
        assert_eq!(v.size(), 0);
        assert_eq!(v.vector_type(), ColumnVectorType::Invalid);

        v.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        assert!(v.set_vector_type(ColumnVectorType::Constant).is_err());
        assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(v.size(), 0);
        assert!(v.get_value_by_index(0).is_none());
        assert_eq!(v.vector_type(), ColumnVectorType::Constant);

        let s = payload(0);
        v.append_value(Some(s.clone())).unwrap();
        assert!(v.append_value(Some(s.clone())).is_err());
        assert_eq!(v.get_value_by_index(0), Some(&s));
        assert!(v.get_value_by_index(1).is_none());
    }

    // ---------------------------------------------------------------------
    // `src/unit_test/storage/column_vector/column_vector_ut.cpp`
    // ---------------------------------------------------------------------

    /// C++: `ColumnVectorTest.ReadWrite` — every supported flat column
    /// serializes to exactly `GetSizeInBytes()` bytes and reads back equal.
    #[test]
    fn column_vector_read_write_roundtrips() {
        let vector_size = DEFAULT_VECTOR_SIZE / 3;
        let mut columns: Vec<Column> = Vec::new();

        let mut boolean = Column::new(LogicalType::Boolean).unwrap();
        boolean.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            boolean
                .push_block_value(BlockValue::Bool(i % 2 == 0))
                .unwrap();
        }
        columns.push(boolean);

        let mut tinyint = Column::new(LogicalType::TinyInt).unwrap();
        tinyint.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            tinyint
                .push_block_value(BlockValue::TinyInt(i as i8))
                .unwrap();
        }
        columns.push(tinyint);

        let mut smallint = Column::new(LogicalType::SmallInt).unwrap();
        smallint.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            smallint
                .push_block_value(BlockValue::SmallInt(i as i16))
                .unwrap();
        }
        columns.push(smallint);

        let mut integer = Column::new(LogicalType::Integer).unwrap();
        integer.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            integer
                .push_block_value(BlockValue::Int32(i as i32))
                .unwrap();
        }
        columns.push(integer);

        let mut bigint = Column::new(LogicalType::BigInt).unwrap();
        bigint.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            bigint.push_block_value(BlockValue::Int(i as i64)).unwrap();
        }
        columns.push(bigint);

        let mut float = Column::new(LogicalType::Float).unwrap();
        float.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            float
                .push_block_value(BlockValue::Float(i as f32 + 0.5))
                .unwrap();
        }
        columns.push(float);

        let mut double = Column::new(LogicalType::Double).unwrap();
        double.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            double
                .push_block_value(BlockValue::Double(i as f64 + 0.5))
                .unwrap();
        }
        columns.push(double);

        let mut varchar = Column::new(LogicalType::Varchar).unwrap();
        varchar.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        for i in 0..vector_size {
            let ch = (b'a' + (i % 26) as u8) as char;
            varchar
                .push_block_value(BlockValue::Str(ch.to_string().repeat(i)))
                .unwrap();
        }
        columns.push(varchar);

        for column in &columns {
            let expected = column.get_size_in_bytes().unwrap();
            let mut buf = Vec::with_capacity(expected);
            column.write_adv(&mut buf).unwrap();
            assert_eq!(buf.len(), expected);

            let (read, consumed) = Column::read_adv(&buf).unwrap();
            assert_eq!(consumed, expected);
            assert_eq!(&read, column);
        }
    }
}
