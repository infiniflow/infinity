//
// Created by JinHai on 2022/7/28.
//

#include "physical_aggregate.h"
#include "executor/expression/expression_executor.h"
#include "executor/expression/expression_evaluator.h"
#include "common/utility/utility.h"

namespace infinity {

void
PhysicalAggregate::Init() {
}

void
PhysicalAggregate::Execute(SharedPtr<QueryContext>& query_context) {
    input_table_ = left_->output();
    ExecutorAssert(input_table_ != nullptr, "No left input.");

    // 1. Execute group-by expressions to generate unique key.
    ExpressionExecutor groupby_executor;
    groupby_executor.Init(groups_);

    Vector<SharedPtr<ColumnDef>> groupby_columns;
    Vector<SharedPtr<ColumnDef>> output_columns;
    groupby_columns.reserve(groups_.size());
    output_columns.reserve(groups_.size());

    Vector<DataType> types;
    types.reserve(groups_.size());

    for(i64 idx = 0; auto& expr: groups_) {
        SharedPtr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
        groupby_columns.emplace_back(col_def);
        output_columns.emplace_back(col_def);
        types.emplace_back(expr->Type());
        ++ idx;
    }

    SharedPtr<TableDef> groupby_tabledef = TableDef::Make("groupby", groupby_columns, false);
    SharedPtr<Table> groupby_table = Table::Make(groupby_tabledef, TableType::kIntermediate);

    groupby_executor.Execute(input_table_, groupby_table);

    // 2. Use the unique key to get the row list of the same key.
    hash_table_.Init(types);

    SizeT block_count = groupby_table->DataBlockCount();
    for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
        const SharedPtr<DataBlock>& block_ptr = groupby_table->GetDataBlockById(block_id);
        hash_table_.Append(block_ptr->column_vectors, block_id, block_ptr->row_count());
    }

    // 3. forlop each aggregates function on each group by bucket, to calculate the result according to the row list
    SharedPtr<Table> output_groupby_table = Table::Make(groupby_tabledef, TableType::kIntermediate);
    GenerateGroupByResult(groupby_table, output_groupby_table);


    // input table after group by, each block belong to one group. This is the prerequisites to execute aggregate function.
    SharedPtr<Table> grouped_input_table;
    {
        SizeT column_count = input_table_->ColumnCount();
        Vector<SharedPtr<ColumnDef>> columns;
        columns.reserve(column_count);
        for(SizeT idx = 0; idx < column_count; ++ idx) {
            DataType col_type = input_table_->GetColumnTypeById(idx);
            String col_name = input_table_->GetColumnNameById(idx);

            SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
            columns.emplace_back(col_def);
        }

        SharedPtr<TableDef> table_def = TableDef::Make("grouped_input", columns, false);

        grouped_input_table = Table::Make(table_def, TableType::kGroupBy);
    }
    GroupByInputTable(input_table_, grouped_input_table);

    // generate output aggregate table
    SizeT aggregates_count = aggregates_.size();
    if(aggregates_count > 0) {
        SharedPtr<Table> output_aggregate_table{};

        // Prepare the output table columns
        Vector<SharedPtr<ColumnDef>> aggregate_columns;
        aggregate_columns.reserve(aggregates_count);

        // Prepare the expression states
        Vector<SharedPtr<ExpressionState>> expr_states;
        expr_states.reserve(aggregates_.size());

        // Prepare the output block
        Vector<DataType> output_types;
        output_types.reserve(aggregates_count);

        for(i64 idx = 0; auto& expr: aggregates_) {
            // expression state
            expr_states.emplace_back(ExpressionState::CreateState(expr));

            // column definition
            SharedPtr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
            aggregate_columns.emplace_back(col_def);

            // for output block
            output_types.emplace_back(expr->Type());

            ++ idx;
        }

        // output aggregate table definition
        SharedPtr<TableDef> aggregate_tabledef = TableDef::Make("aggregate", groupby_columns, false);
        output_aggregate_table = Table::Make(aggregate_tabledef, TableType::kAggregate);

        // Loop blocks
        HashMap<u64, SharedPtr<DataBlock>> block_map;
        SizeT input_data_block_count = grouped_input_table->DataBlockCount();
        for(SizeT block_idx = 0; block_idx < input_data_block_count; ++ block_idx) {
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types);

            const SharedPtr<DataBlock>& block_ptr = grouped_input_table->GetDataBlockById(block_idx);
//            block_map[groupby_index_] = block_ptr;
//            block_map[input_table_index_] = block_ptr;
            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            for(SizeT expr_idx = 0; expr_idx < aggregates_count; ++ expr_idx) {
                evaluator.Execute(aggregates_[expr_idx],
                                  expr_states[expr_idx],
                                  block_ptr,
                                  block_ptr->row_count(),
                                  output_data_block->column_vectors[expr_idx]);
            }

            output_data_block->Finalize();
            output_aggregate_table->Append(output_data_block);
        }

        // Union output group by table with aggregate table
        output_groupby_table->UnionWith(output_aggregate_table);
    }

    // 4. generate the result to output
    this->output_ = output_groupby_table;
}

void
PhysicalAggregate::GroupByInputTable(const SharedPtr<Table>& input_table, SharedPtr<Table>& grouped_input_table) {
    SizeT column_count = input_table->ColumnCount();

    // 1. Get output table column types.
    Vector<DataType> types;
    types.reserve(column_count);
    for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
        DataType input_type = input_table->GetColumnTypeById(column_id);
        DataType output_type = grouped_input_table->GetColumnTypeById(column_id);
        ExecutorAssert(input_type == output_type,
                       "Column type doesn't matched: " + input_type.ToString() + " and " + output_type.ToString());
        types.emplace_back(input_type);
    }

    // 2. Generate data blocks and append it into output table according to the group by hash table.
    const Vector<SharedPtr<DataBlock>>& input_datablocks = input_table->data_blocks_;
    for(const auto& item: hash_table_.hash_table_) {

        // 2.1 Each hash bucket will be insert in to one data block
        SharedPtr<DataBlock> output_datablock = DataBlock::Make();
        SizeT datablock_size = 0;
        for(const auto& vec_pair: item.second) {
            datablock_size += vec_pair.second.size();
        }
        SizeT datablock_capacity = NextPowerOfTwo(datablock_size);
        output_datablock->Init(types, datablock_capacity);

        // Loop each block
        SizeT output_row_idx = 0;
        for(const auto& vec_pair: item.second) {
            SizeT input_block_id = vec_pair.first;

            // Loop each row of same block
            for(const auto input_offset: vec_pair.second) {

                // Forloop each column
                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    switch (types[column_id].type()) {
                        case LogicalType::kBoolean: {
                            ((BooleanT *) (output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((BooleanT *) (input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kTinyInt: {
                            ((TinyIntT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((TinyIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kSmallInt: {
                            ((SmallIntT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((SmallIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kInteger: {
                            ((IntegerT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((IntegerT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kBigInt: {
                            ((BigIntT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((BigIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case kHugeInt: {
                            NotImplementError("HugeInt data shuffle isn't implemented.")
                        }
                        case kFloat: {
                            ((FloatT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((FloatT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case kDouble: {
                            ((DoubleT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx]
                                    = ((DoubleT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case kDecimal16: {
                            NotImplementError("Decimal16 data shuffle isn't implemented.")
                        }
                        case kDecimal32: {
                            NotImplementError("Decimal32 data shuffle isn't implemented.")
                        }
                        case kDecimal64: {
                            NotImplementError("Decimal64 data shuffle isn't implemented.")
                        }
                        case kDecimal128: {
                            NotImplementError("Decimal128 data shuffle isn't implemented.")
                        }
                        case kVarchar: {
                            NotImplementError("Varchar data shuffle isn't implemented.")
                        }
                        case kChar: {
                            NotImplementError("Char data shuffle isn't implemented.")
                        }
                        case kDate: {
                            NotImplementError("Date data shuffle isn't implemented.")
                        }
                        case kTime: {
                            NotImplementError("Time data shuffle isn't implemented.")
                        }
                        case kDateTime: {
                            NotImplementError("Datetime data shuffle isn't implemented.")
                        }
                        case kTimestamp: {
                            NotImplementError("Timestamp data shuffle isn't implemented.")
                        }
                        case kTimestampTZ: {
                            NotImplementError("TimestampTZ data shuffle isn't implemented.")
                        }
                        case kInterval: {
                            NotImplementError("Interval data shuffle isn't implemented.")
                        }
                        case kMixed: {
                            NotImplementError("Heterogeneous data shuffle isn't implemented.")
                        }
                        default: {
                            ExecutorError("Unexpected data type")
                        }
                    }
                }

                ++ output_row_idx;
            }
        }

        ExecutorAssert(output_row_idx == datablock_size,
                       "Expected block size: " + std::to_string(datablock_size)
                       + ", but only copied data size: " + std::to_string(output_row_idx));

        for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
            output_datablock->column_vectors[column_id]->tail_index_ = datablock_size;
        }

        output_datablock->Finalize();
        grouped_input_table->Append(output_datablock);
    }
}

void
PhysicalAggregate::GenerateGroupByResult(const SharedPtr<Table>& input_table, SharedPtr<Table>& output_table) {

    SizeT column_count = input_table->ColumnCount();
    Vector<DataType> types;
    types.reserve(column_count);
    for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
        DataType input_type = input_table->GetColumnTypeById(column_id);
        DataType output_type = output_table->GetColumnTypeById(column_id);
        ExecutorAssert(input_type == output_type,
                       "Column type doesn't matched: " + input_type.ToString() + " and " + output_type.ToString());
        types.emplace_back(input_type);
    }

    SharedPtr<DataBlock> output_datablock = nullptr;
    const Vector<SharedPtr<DataBlock>>& input_datablocks = input_table->data_blocks_;
    SizeT row_count = hash_table_.hash_table_.size();
#if 1
    for(SizeT block_row_idx = 0; const auto& item: hash_table_.hash_table_) {
        // Each hash bucket will generate one data block.
        output_datablock = DataBlock::Make();
        output_datablock->Init(types);

        // Only get the first row(block id and row offset of the block) of the bucket
        SizeT input_block_id = item.second.begin()->first;
        SizeT input_offset = item.second.begin()->second.front();

        // Only the first position of the column vector has value.
        for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
            switch (types[column_id].type()) {
                case LogicalType::kBoolean: {
                    ((BooleanT *) (output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((BooleanT *) (input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kTinyInt: {
                    ((TinyIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((TinyIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kSmallInt: {
                    ((SmallIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((SmallIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kInteger: {
                    ((IntegerT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((IntegerT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kBigInt: {
                    ((BigIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((BigIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case kHugeInt: {
                    NotImplementError("HugeInt data shuffle isn't implemented.")
                }
                case kFloat: {
                    ((FloatT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((FloatT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case kDouble: {
                    ((DoubleT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((DoubleT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case kDecimal16: {
                    NotImplementError("Decimal16 data shuffle isn't implemented.")
                }
                case kDecimal32: {
                    NotImplementError("Decimal32 data shuffle isn't implemented.")
                }
                case kDecimal64: {
                    NotImplementError("Decimal64 data shuffle isn't implemented.")
                }
                case kDecimal128: {
                    NotImplementError("Decimal128 data shuffle isn't implemented.")
                }
                case kVarchar: {
                    NotImplementError("Varchar data shuffle isn't implemented.")
                }
                case kChar: {
                    NotImplementError("Char data shuffle isn't implemented.")
                }
                case kDate: {
                    NotImplementError("Date data shuffle isn't implemented.")
                }
                case kTime: {
                    NotImplementError("Time data shuffle isn't implemented.")
                }
                case kDateTime: {
                    NotImplementError("Datetime data shuffle isn't implemented.")
                }
                case kTimestamp: {
                    NotImplementError("Timestamp data shuffle isn't implemented.")
                }
                case kTimestampTZ: {
                    NotImplementError("TimestampTZ data shuffle isn't implemented.")
                }
                case kInterval: {
                    NotImplementError("Interval data shuffle isn't implemented.")
                }
                case kMixed: {
                    NotImplementError("Heterogeneous data shuffle isn't implemented.")
                }
                default: {
                    ExecutorError("Unexpected data type")
                }
            }

            output_datablock->column_vectors[column_id]->tail_index_ = block_row_idx + 1;
        }

        output_datablock->Finalize();
        output_table->Append(output_datablock);
    }
#else
    for(SizeT row_id = 0, block_row_idx = 0; const auto& item: hash_table_.hash_table_) {
        // DEFAULT VECTOR SIZE buckets will generate one data block.
        if(row_id % DEFAULT_VECTOR_SIZE == 0) {
            if(output_datablock != nullptr) {
                for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                    output_datablock->column_vectors[column_id]->tail_index_ = block_row_idx;
                }

                output_datablock->Finalize();
                output_table->Append(output_datablock);
                block_row_idx = 0;
            }

            output_datablock = DataBlock::Make();
            output_datablock->Init(types);


        }

        SizeT input_block_id = item.second.begin()->first;
        SizeT input_offset = item.second.begin()->second.front();

        for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
            switch (types[column_id].type()) {
                case LogicalType::kBoolean: {
                    ((BooleanT *) (output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((BooleanT *) (input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kTinyInt: {
                    ((TinyIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((TinyIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kSmallInt: {
                    ((SmallIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((SmallIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kInteger: {
                    ((IntegerT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((IntegerT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kBigInt: {
                    ((BigIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((BigIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case kHugeInt: {
                    NotImplementError("HugeInt data shuffle isn't implemented.")
                }
                case kFloat: {
                    ((FloatT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((FloatT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case kDouble: {
                    ((DoubleT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx]
                            = ((DoubleT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case kDecimal16: {
                    NotImplementError("Decimal16 data shuffle isn't implemented.")
                }
                case kDecimal32: {
                    NotImplementError("Decimal32 data shuffle isn't implemented.")
                }
                case kDecimal64: {
                    NotImplementError("Decimal64 data shuffle isn't implemented.")
                }
                case kDecimal128: {
                    NotImplementError("Decimal128 data shuffle isn't implemented.")
                }
                case kVarchar: {
                    NotImplementError("Varchar data shuffle isn't implemented.")
                }
                case kChar: {
                    NotImplementError("Char data shuffle isn't implemented.")
                }
                case kDate: {
                    NotImplementError("Date data shuffle isn't implemented.")
                }
                case kTime: {
                    NotImplementError("Time data shuffle isn't implemented.")
                }
                case kDateTime: {
                    NotImplementError("Datetime data shuffle isn't implemented.")
                }
                case kTimestamp: {
                    NotImplementError("Timestamp data shuffle isn't implemented.")
                }
                case kTimestampTZ: {
                    NotImplementError("TimestampTZ data shuffle isn't implemented.")
                }
                case kInterval: {
                    NotImplementError("Interval data shuffle isn't implemented.")
                }
                case kMixed: {
                    NotImplementError("Heterogeneous data shuffle isn't implemented.")
                }
                default: {
                    ExecutorError("Unexpected data type")
                }
            }
        }

        ++ block_row_idx;
        ++ row_id;

        if(row_id == row_count) {
            // All hash table data are checked
            for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
                output_datablock->column_vectors[column_id]->tail_index_ = block_row_idx;
            }

            output_datablock->Finalize();
            output_table->Append(output_datablock);
            break;
        }
    }
#endif
}

}
