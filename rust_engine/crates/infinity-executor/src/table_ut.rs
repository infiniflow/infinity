//! Port of `TableTest.test1`
//! (`src/unit_test/storage/definition/table_ut.cpp`): an order-by
//! `DataTable` built from three 8192-row blocks preserves both column values
//! and the synthetic row-id vector.

use crate::data_table::{DataTable, TableType};
use infinity_catalog::value_format::{data_type_json, logical, ColumnDefValue, TableDef};
use infinity_common::LogicalType;
use infinity_storage::cpp_format::DEFAULT_BLOCK_CAPACITY;
use infinity_storage::data_block::{
    BlockValue, Column, ColumnVectorType, DataBlock, DEFAULT_VECTOR_SIZE,
};

#[test]
fn test1() {
    let column_count = 2;
    let block_count = 3;
    let row_count = DEFAULT_VECTOR_SIZE;

    let mut columns = Vec::with_capacity(column_count);
    columns.push(ColumnDefValue::new(
        data_type_json(logical::BOOLEAN, None),
        0,
        "col1",
    ));
    columns.push(ColumnDefValue::new(
        data_type_json(logical::BIGINT, None),
        1,
        "col2",
    ));
    assert_eq!(columns.len(), column_count);

    let table_def = TableDef::make("default_db", "order_by_table", "comment", columns);
    let mut order_by_table = DataTable::make(table_def, TableType::OrderBy);

    for _ in 0..block_count {
        let mut col1 = Column::new(LogicalType::Boolean).expect("boolean column");
        col1.initialize(ColumnVectorType::CompactBit, DEFAULT_VECTOR_SIZE);
        let mut col2 = Column::new(LogicalType::BigInt).expect("bigint column");
        col2.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
        let mut data_block = DataBlock::new(vec![col1, col2], DEFAULT_VECTOR_SIZE);

        for row_id in 0..row_count {
            data_block
                .append_value(0, BlockValue::Bool(row_id % 2 == 0))
                .expect("append bool");
            data_block
                .append_value(1, BlockValue::Int(row_id as i64))
                .expect("append bigint");
        }
        data_block.finalize();
        order_by_table.append(data_block);
    }

    let offset_column_vector = order_by_table.get_row_id_vector();
    assert_eq!(
        offset_column_vector.len(),
        block_count * DEFAULT_VECTOR_SIZE
    );

    for block_id in 0..block_count {
        let block = order_by_table
            .get_data_block_by_id(block_id)
            .expect("block exists");

        // Check Column1 data (the C++ `buffer_->GetCompactBit`).
        let column1 = block.column(0).expect("column 1");
        assert_eq!(column1.logical_type(), LogicalType::Boolean);
        for row_id in 0..row_count {
            assert_eq!(
                column1.get_value_by_index(row_id),
                Some(BlockValue::Bool(row_id % 2 == 0))
            );
        }

        // Check Column2 data (the C++ `((BigIntT *)column2->data())[row_id]`).
        let column2 = block.column(1).expect("column 2");
        assert_eq!(column2.logical_type(), LogicalType::BigInt);
        for row_id in 0..row_count {
            assert_eq!(
                column2.get_value_by_index(row_id),
                Some(BlockValue::Int(row_id as i64))
            );
        }

        // Check offset.
        for row_id in 0..row_count {
            let row = &offset_column_vector[block_id * DEFAULT_VECTOR_SIZE + row_id];
            assert_eq!(row.segment_id, u32::MAX);
            assert_eq!(
                row.segment_offset as usize,
                block_id * DEFAULT_BLOCK_CAPACITY + row_id
            );
        }
    }
}
