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

/// One typed column of a [`DataBlock`].
#[derive(Clone, Debug, PartialEq)]
pub enum Column {
    Int(ColumnVector<i64>),
    Float(ColumnVector<f64>),
    Bool(ColumnVector<bool>),
    Str(ColumnVector<String>),
    Bytes(ColumnVector<Vec<u8>>),
}

impl Column {
    /// The C++ logical type this column stores (unsigned widths are folded
    /// into the signed vector; the declared type stays in the catalog).
    pub fn logical_type(&self) -> LogicalType {
        match self {
            Column::Int(_) => LogicalType::BigInt,
            Column::Float(_) => LogicalType::Double,
            Column::Bool(_) => LogicalType::Boolean,
            Column::Str(_) => LogicalType::Varchar,
            Column::Bytes(_) => LogicalType::Varchar,
        }
    }

    pub fn len(&self) -> usize {
        match self {
            Column::Int(c) => c.len(),
            Column::Float(c) => c.len(),
            Column::Bool(c) => c.len(),
            Column::Str(c) => c.len(),
            Column::Bytes(c) => c.len(),
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
        }
    }

    pub fn clear(&mut self) {
        match self {
            Column::Int(c) => c.clear(),
            Column::Float(c) => c.clear(),
            Column::Bool(c) => c.clear(),
            Column::Str(c) => c.clear(),
            Column::Bytes(c) => c.clear(),
        }
    }

    /// The rendered text of a slot (`None` for NULL), the form the executor
    /// uses when it converts a decoded block into `Value` rows.
    pub fn value_text(&self, row: usize) -> Option<String> {
        match self {
            Column::Int(c) => c.get(row).map(|v| v.to_string()),
            Column::Float(c) => c.get(row).map(|v| v.to_string()),
            Column::Bool(c) => c.get(row).map(|v| v.to_string()),
            Column::Str(c) => c.get(row).cloned(),
            Column::Bytes(c) => c.get(row).map(|v| String::from_utf8_lossy(v).into_owned()),
        }
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
        match col {
            Column::Int(c) => c.push(text.and_then(|t| t.trim().parse::<i64>().ok())),
            Column::Float(c) => c.push(text.and_then(|t| t.trim().parse::<f64>().ok())),
            Column::Bool(c) => c.push(text.and_then(|t| match t.trim() {
                "true" | "1" => Some(true),
                "false" | "0" => Some(false),
                _ => None,
            })),
            Column::Str(c) => c.push(text.map(str::to_string)),
            Column::Bytes(c) => c.push(text.map(|t| t.as_bytes().to_vec())),
        }
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

    /// Approximate heap bytes, for the buffer-manager budget.
    pub fn mem_used(&self) -> usize {
        self.columns
            .iter()
            .map(|c| match c {
                Column::Int(v) => v.values().len() * 8 + v.validity().len(),
                Column::Float(v) => v.values().len() * 8 + v.validity().len(),
                Column::Bool(v) => v.values().len() + v.validity().len(),
                Column::Str(v) => {
                    v.values().iter().map(|s| s.len() + 24).sum::<usize>() + v.validity().len()
                }
                Column::Bytes(v) => {
                    v.values().iter().map(|s| s.len() + 24).sum::<usize>() + v.validity().len()
                }
            })
            .sum()
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
            assert_eq!(
                c.get(0),
                Some(&infinity_common::IntervalT::new(i as i32))
            );
            c.reset();
        }
    }
}
