//! Intermediate result tables.
//!
//! Port of the C++ `DataTable` (`src/storage/data_table.cppm`). The reference
//! uses one table type for scan results, order-by buffers and aggregate
//! output; the Rust executor materialises rows directly, so only the parts the
//! order-by table needs are reproduced here: the definition, the block list,
//! the row count and the synthetic row-id vector.

use infinity_catalog::value_format::TableDef;
use infinity_storage::data_block::{DataBlock, RowIdT};

/// C++ `TableType`.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum TableType {
    #[default]
    Invalid,
    DataTable,
    Intermediate,
    OrderBy,
    GroupBy,
    Aggregate,
    CrossProduct,
    Result,
}

/// C++ `DataTable`: a definition plus row-aligned data blocks.
#[derive(Clone, Debug, Default)]
pub struct DataTable {
    pub definition: TableDef,
    pub row_count: usize,
    pub table_type: TableType,
    pub data_blocks: Vec<DataBlock>,
}

impl DataTable {
    /// C++ `DataTable::Make`.
    pub fn make(definition: TableDef, table_type: TableType) -> Self {
        Self {
            definition,
            row_count: 0,
            table_type,
            data_blocks: Vec::new(),
        }
    }

    pub fn column_count(&self) -> usize {
        self.definition.column_count()
    }

    pub fn table_name(&self) -> &str {
        &self.definition.table_name
    }

    pub fn data_block_count(&self) -> usize {
        self.data_blocks.len()
    }

    /// C++ `DataTable::GetDataBlockById`. The reference raises an unrecoverable
    /// error for a bad index; `None` keeps the same contract at the type level.
    pub fn get_data_block_by_id(&self, idx: usize) -> Option<&DataBlock> {
        self.data_blocks.get(idx)
    }

    /// C++ `DataTable::Append`: append a block and accumulate its rows.
    pub fn append(&mut self, block: DataBlock) {
        self.row_count += block.row_count();
        self.data_blocks.push(block);
    }

    /// C++ `DataTable::GetRowIDVector`: the synthetic `(INVALID_SEGMENT_ID,
    /// block_id * DEFAULT_BLOCK_CAPACITY + offset)` ids of every row.
    pub fn get_row_id_vector(&self) -> Vec<RowIdT> {
        let mut row_ids = Vec::with_capacity(self.row_count);
        for (block_id, block) in self.data_blocks.iter().enumerate() {
            block.fill_row_id_vector(&mut row_ids, block_id as u32);
        }
        row_ids
    }
}
