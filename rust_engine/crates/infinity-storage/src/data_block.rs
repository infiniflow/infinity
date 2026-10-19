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

/// C++ `ColumnVector<T>`: `data_` plus the null bitmap.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct ColumnVector<T> {
    data: Vec<T>,
    /// `true` for a non-NULL row (the C++ `null_bitmap_` bit).
    validity: Vec<bool>,
}

impl<T> ColumnVector<T> {
    /// C++ `ColumnVector::Make`: an empty vector.
    pub fn new() -> Self {
        Self {
            data: Vec::new(),
            validity: Vec::new(),
        }
    }

    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            data: Vec::with_capacity(capacity),
            validity: Vec::with_capacity(capacity),
        }
    }

    /// C++ `ColumnVector::GetValue`: `None` for a NULL slot.
    pub fn get(&self, row: usize) -> Option<&T> {
        if self.validity.get(row).copied().unwrap_or(false) {
            self.data.get(row)
        } else {
            None
        }
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

    pub fn clear(&mut self) {
        self.data.clear();
        self.validity.clear();
    }
}

impl<T: Default> ColumnVector<T> {
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
    }

    /// C++ `ColumnVector::SetValue`.
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
    }
}

impl<T: Clone + Default> ColumnVector<T> {
    /// C++ `ColumnVector::AppendWith(T, count)`.
    pub fn extend_value(&mut self, value: T, count: usize) {
        self.data.extend(std::iter::repeat_n(value, count));
        self.validity.extend(std::iter::repeat_n(true, count));
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
