//! The chunk the physical operators exchange.
//!
//! Mirrors the reference `DataBlock` (`src/storage/data_block.cppm`): a
//! column-major batch of at most [`DEFAULT_VECTOR_SIZE`] rows that is filled by
//! one operator and consumed by the next.
//!
//! The cells live in typed column buffers ([`ColumnData`]), not in `Value`
//! rows: a scan decodes a block once, an operator that needs a row (filter,
//! sort, aggregate) materialises the cells it reads, and a fragment's output is
//! the concatenation of its chunks into a [`crate::columnar::Batch`] - no step
//! builds a `Value` per cell the query does not look at.

use ::common::{Error, Result};
use expression::Value;

use crate::columnar::{column_for_rows, Batch, ColumnData};

/// `DEFAULT_BLOCK_CAPACITY` / `DEFAULT_VECTOR_SIZE`
/// (`src/common/default_values.cppm:58`): the row count of one chunk.
pub use storage::data_block::DEFAULT_VECTOR_SIZE;

/// A column-major batch of rows.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct DataChunk {
    /// One typed buffer per column, each with `row_count` cells.
    columns: Vec<ColumnData>,
    capacity: usize,
    row_count: usize,
}

impl DataChunk {
    /// An empty chunk with `column_count` columns, sized for
    /// [`DEFAULT_VECTOR_SIZE`] rows.
    pub fn new(column_count: usize) -> Self {
        Self::with_capacity(column_count, DEFAULT_VECTOR_SIZE)
    }

    /// An empty chunk with `column_count` columns that can hold `capacity` rows.
    ///
    /// The buffers start as `i64` and widen as rows arrive (see
    /// [`ColumnData::push_value`]); [`DataChunk::from_rows`] picks the types up
    /// front when the rows are already known.
    pub fn with_capacity(column_count: usize, capacity: usize) -> Self {
        Self {
            columns: (0..column_count).map(|_| ColumnData::empty()).collect(),
            capacity,
            row_count: 0,
        }
    }

    /// Number of columns.
    pub fn column_count(&self) -> usize {
        self.columns.len()
    }

    /// The column buffers (C++ `DataBlock::column_vectors_`).
    pub fn columns(&self) -> &[ColumnData] {
        &self.columns
    }

    /// Number of rows currently in the chunk.
    pub fn row_count(&self) -> usize {
        self.row_count
    }

    /// Maximum row count (C++ `capacity_`).
    pub fn capacity(&self) -> usize {
        self.capacity
    }

    /// Rows that can still be appended before [`DataChunk::is_full`].
    pub fn available_capacity(&self) -> usize {
        self.capacity - self.row_count
    }

    /// Whether the chunk holds [`DataChunk::capacity`] rows.
    pub fn is_full(&self) -> bool {
        self.row_count >= self.capacity
    }

    /// Appends one row. Fails when the chunk is full or the row's width does not
    /// match the chunk's column count.
    pub fn push_row(&mut self, row: &[Value]) -> Result<()> {
        if row.len() != self.columns.len() {
            return Err(Error::Invalid(format!(
                "data chunk has {} columns, row has {}",
                self.columns.len(),
                row.len()
            )));
        }
        if self.is_full() {
            return Err(Error::Invalid(format!(
                "data chunk is full ({} rows)",
                self.capacity
            )));
        }
        for (column, value) in self.columns.iter_mut().zip(row) {
            column.push_value(value);
        }
        self.row_count += 1;
        Ok(())
    }

    /// Appends `count` rows of `other`, starting at row `offset`
    /// (C++ `DataBlock::AppendWith`). The count is clamped to what `other` still
    /// has and what this chunk can still hold.
    pub fn append_with(&mut self, other: &DataChunk, offset: usize, count: usize) {
        if offset >= other.row_count {
            return;
        }
        let count = count
            .min(other.row_count - offset)
            .min(self.available_capacity());
        if count == 0 {
            return;
        }
        let order: Vec<usize> = (offset..offset + count).collect();
        for (dst, src) in self.columns.iter_mut().zip(&other.columns) {
            let part = src.permute(&order);
            dst.append(&part);
        }
        self.row_count += count;
    }

    /// The cell at `(row, column)`.
    pub fn get(&self, row: usize, column: usize) -> Option<Value> {
        if row >= self.row_count {
            return None;
        }
        self.columns.get(column).map(|c| c.value_at(row))
    }

    /// Row `row` as an owned slice, in column order.
    pub fn row(&self, row: usize) -> Option<Vec<Value>> {
        if row >= self.row_count {
            return None;
        }
        Some(self.columns.iter().map(|c| c.value_at(row)).collect())
    }

    /// Marks the chunk ready for consumption (C++ `DataBlock::Finalize`). The
    /// chunk is already consistent after every append, so this is a no-op kept
    /// for parity with the operator calls.
    pub fn finalize(&mut self) {}

    /// Drops every row, keeping the columns and their capacity.
    pub fn reset(&mut self) {
        for column in &mut self.columns {
            // Every variant, not just the one this buffer happens to hold: a
            // reused chunk hands out its old cells when a column keeps its
            // values, and a pending `Empty { nulls }` would count as a row.
            *column = column.same_shape_empty();
        }
        self.row_count = 0;
    }

    /// Row-major view of the chunk.
    pub fn into_rows(self) -> Vec<Vec<Value>> {
        (0..self.row_count)
            .map(|r| self.columns.iter().map(|c| c.value_at(r)).collect())
            .collect()
    }

    /// The chunk as a result block: the same columns, no copying.
    pub fn into_batch(self, columns: Vec<String>) -> Batch {
        Batch::new(columns, self.columns, self.row_count)
    }

    /// Splits `rows` into chunks of [`DEFAULT_VECTOR_SIZE`] rows. An empty input
    /// produces no chunk.
    pub fn from_rows(column_count: usize, rows: Vec<Vec<Value>>) -> Vec<DataChunk> {
        let mut chunks: Vec<DataChunk> = Vec::new();
        let mut current = Self::typed(column_count, &rows);
        for row in rows {
            if current.is_full() {
                current.finalize();
                chunks.push(std::mem::replace(
                    &mut current,
                    DataChunk::new(column_count),
                ));
            }
            // A row of the wrong width would silently desynchronise the chunk;
            // pad/truncate it instead (the source rows always come from one
            // materialised relation, so this is a defensive guard only).
            let mut row = row;
            row.resize(column_count, Value::Null);
            // push_row cannot fail here: the width is fixed and the chunk is not
            // full.
            let _ = current.push_row(&row);
        }
        if current.row_count() > 0 {
            current.finalize();
            chunks.push(current);
        }
        chunks
    }

    /// A chunk whose columns already carry the types the rows ask for, so the
    /// first `push_row` does not have to widen them.
    fn typed(column_count: usize, rows: &[Vec<Value>]) -> DataChunk {
        // Every column is typed from all the rows, not from the first one: a
        // leading NULL says nothing about the column, and a probe that saw only
        // NULL typed it `i64`, so later `true`s were stored as `1`.
        let columns = (0..column_count)
            .map(|c| ColumnData::empty_like(&column_for_rows(rows, c)))
            .collect();
        DataChunk {
            columns,
            capacity: DEFAULT_VECTOR_SIZE,
            row_count: 0,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn row(i: i64) -> Vec<Value> {
        vec![Value::Int(i), Value::Str(format!("r{i}"))]
    }

    #[test]
    fn splits_8193_rows_at_the_chunk_boundary() {
        let rows: Vec<Vec<Value>> = (0..8193).map(row).collect();
        let chunks = DataChunk::from_rows(2, rows);
        assert_eq!(chunks.len(), 2);
        assert_eq!(chunks[0].row_count(), DEFAULT_VECTOR_SIZE);
        assert_eq!(chunks[1].row_count(), 1);
        assert_eq!(
            chunks[0].get(8191, 0),
            Some(Value::Int(8191)),
            "the cells are typed in the buffer, not stringified"
        );
        assert_eq!(chunks[1].get(0, 1), Some(Value::Str("r8192".into())));
        assert_eq!(chunks.iter().map(|c| c.row_count()).sum::<usize>(), 8193);
    }

    #[test]
    fn append_with_crosses_the_chunk_boundary() {
        let rows: Vec<Vec<Value>> = (0..8193).map(row).collect();
        let chunks = DataChunk::from_rows(2, rows);
        let mut target = DataChunk::new(2);
        // Copy rows 8180..8193, which straddle the two chunks: 8180..8192 come
        // from the first one, the last row from the second.
        target.append_with(&chunks[0], 8180, 13);
        assert_eq!(target.row_count(), 12);
        target.append_with(&chunks[1], 0, 13);
        assert_eq!(target.row_count(), 13);
        assert_eq!(target.get(11, 0), Some(Value::Int(8191)));
        assert_eq!(target.get(12, 0), Some(Value::Int(8192)));
        // A full chunk refuses the append instead of overflowing.
        let mut full = DataChunk::new(2);
        full.append_with(&chunks[0], 0, 9000);
        assert!(full.is_full());
        full.append_with(&chunks[1], 0, 1);
        assert_eq!(full.row_count(), DEFAULT_VECTOR_SIZE);
    }

    #[test]
    fn append_with_never_overflows_a_full_chunk() {
        let chunks = DataChunk::from_rows(2, (0..8193).map(row).collect());
        let mut target = DataChunk::new(2);
        target.append_with(&chunks[0], 0, 8180);
        assert_eq!(target.row_count(), 8180);
        // 8180 + 13 does not fit into the 8192-row chunk, so only the 12 rows
        // the chunk still has room for are copied.
        target.append_with(&chunks[0], 8180, 13);
        assert_eq!(target.row_count(), DEFAULT_VECTOR_SIZE);
        assert!(target.is_full());
        assert_eq!(target.get(8179, 0), Some(Value::Int(8179)));
        assert_eq!(target.get(8180, 0), Some(Value::Int(8180)));
    }

    #[test]
    fn append_with_clamps_to_the_source_remainder() {
        let chunks = DataChunk::from_rows(2, (0..5).map(row).collect());
        let mut target = DataChunk::new(2);
        target.append_with(&chunks[0], 3, 100);
        assert_eq!(target.row_count(), 2);
        assert_eq!(
            target.row(0),
            Some(vec![Value::Int(3), Value::Str("r3".into())])
        );
        assert_eq!(target.row(2), None);
    }

    #[test]
    fn push_row_rejects_a_width_mismatch() {
        let mut chunk = DataChunk::new(2);
        assert!(chunk.push_row(&[Value::Int(1), Value::Int(2)]).is_ok());
        assert!(chunk.push_row(&[Value::Int(1)]).is_err());
        assert_eq!(chunk.row_count(), 1);
    }

    #[test]
    fn into_rows_round_trips_from_rows() {
        let rows: Vec<Vec<Value>> = (0..3).map(row).collect();
        let chunks = DataChunk::from_rows(2, rows.clone());
        let back: Vec<Vec<Value>> = chunks.into_iter().flat_map(DataChunk::into_rows).collect();
        assert_eq!(back, rows);
    }

    #[test]
    fn a_chunk_becomes_a_batch_without_touching_the_rows() {
        let rows: Vec<Vec<Value>> = (0..4).map(row).collect();
        let chunk = DataChunk::from_rows(2, rows.clone()).pop().unwrap();
        let batch = chunk.into_batch(vec!["id".into(), "name".into()]);
        assert_eq!(batch.row_count, 4);
        assert_eq!(batch.rows(), rows);
        assert_eq!(batch.columns, vec!["id".to_string(), "name".to_string()]);
    }

    #[test]
    fn reset_keeps_the_capacity() {
        let mut chunk = DataChunk::new(2);
        for i in 0..10 {
            chunk.push_row(&row(i)).unwrap();
        }
        chunk.reset();
        assert_eq!(chunk.row_count(), 0);
        assert_eq!(chunk.available_capacity(), DEFAULT_VECTOR_SIZE);
        assert_eq!(chunk.column_count(), 2);
    }

    #[test]
    fn reset_clears_every_column_before_reuse() {
        let mut chunk = DataChunk::new(2);
        chunk.push_row(&row(0)).unwrap();
        chunk.push_row(&row(1)).unwrap();
        chunk.reset();
        chunk
            .push_row(&[Value::Int(9), Value::Str("new".into())])
            .unwrap();
        assert_eq!(chunk.row_count(), 1);
        assert_eq!(
            chunk.row(0),
            Some(vec![Value::Int(9), Value::Str("new".into())]),
            "a reused chunk must not expose the rows it held before"
        );
    }

    #[test]
    fn reset_drops_the_pending_nulls_of_an_untyped_column() {
        let mut chunk = DataChunk::new(1);
        chunk.push_row(&[Value::Null]).unwrap();
        chunk.push_row(&[Value::Null]).unwrap();
        chunk.reset();
        chunk.push_row(&[Value::Str("a".into())]).unwrap();
        assert_eq!(chunk.row_count(), 1);
        assert_eq!(chunk.get(0, 0), Some(Value::Str("a".into())));
    }

    #[test]
    fn a_leading_null_does_not_type_the_column() {
        // Typing from the first row alone read all-NULL as `i64`, and the
        // booleans after it were stored as `1`/`0`.
        let rows = vec![
            vec![Value::Null],
            vec![Value::Bool(true)],
            vec![Value::Bool(false)],
        ];
        let chunk = DataChunk::from_rows(1, rows).pop().unwrap();
        assert_eq!(chunk.get(0, 0), Some(Value::Null));
        assert_eq!(chunk.get(1, 0), Some(Value::Bool(true)));
        assert_eq!(chunk.get(2, 0), Some(Value::Bool(false)));
    }

    #[test]
    fn an_empty_input_produces_no_chunk() {
        assert!(DataChunk::from_rows(3, Vec::new()).is_empty());
        let empty = DataChunk::new(3);
        assert_eq!(empty.row_count(), 0);
        assert_eq!(empty.column_count(), 3);
        assert_eq!(empty.available_capacity(), DEFAULT_VECTOR_SIZE);
        assert!(!empty.is_full());
    }
}
