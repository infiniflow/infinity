// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <string>
#include <algorithm>

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import data_block;

module physical_sort;

namespace infinity {

class Comparator {
public:
    explicit Comparator(const SharedPtr<DataTable> &order_by_table, const Vector<OrderType> &order_by_types)
        : order_by_table_(order_by_table), order_by_types_(order_by_types) {}

    bool operator()(RowID, RowID ) {
#if 0
        SizeT column_count = order_by_table_->ColumnCount();
        for(SizeT col_id = 0; col_id < column_count; ++col_id) {

            SharedPtr<DataType> type = order_by_table_->GetColumnTypeById(col_id);
            OrderType order_type = order_by_types_[col_id];

            const SharedPtr<DataBlock>& left_block = order_by_table_->GetDataBlockById(left.segment_id_);
            const SharedPtr<ColumnVector>& left_column = left_block->column_vectors[col_id];
            const SharedPtr<DataBlock>& right_block = order_by_table_->GetDataBlockById(right.segment_id_);
            const SharedPtr<ColumnVector>& right_column = right_block->column_vectors[col_id];

            switch(type->type()) {
                case kBoolean: {
                    BooleanT left_value = ((BooleanT*)(left_column->data()))[left.segment_offset_];
                    BooleanT right_value = ((BooleanT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kTinyInt: {
                    TinyIntT left_value = ((TinyIntT*)(left_column->data()))[left.segment_offset_];
                    TinyIntT right_value = ((TinyIntT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kSmallInt: {
                    SmallIntT left_value = ((SmallIntT*)(left_column->data()))[left.segment_offset_];
                    SmallIntT right_value = ((SmallIntT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kInteger: {
                    IntegerT left_value = ((IntegerT*)(left_column->data()))[left.segment_offset_];
                    IntegerT right_value = ((IntegerT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kBigInt: {
                    BigIntT left_value = ((BigIntT*)(left_column->data()))[left.segment_offset_];
                    BigIntT right_value = ((BigIntT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kHugeInt: {
                    NotImplementError("HugeInt comparation isn't implemented.")
                }
                case kFloat: {
                    FloatT left_value = ((FloatT*)(left_column->data()))[left.segment_offset_];
                    FloatT right_value = ((FloatT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kDouble: {
                    DoubleT left_value = ((DoubleT*)(left_column->data()))[left.segment_offset_];
                    DoubleT right_value = ((DoubleT*)(right_column->data()))[right.segment_offset_];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kDecimal: {
                    NotImplementError("Decimal comparation isn't implemented.")
                }
                case kVarchar: {
                    VarcharType& left_ref = ((VarcharType*)(left_column->data()))[left.segment_offset_];
                    VarcharType& right_ref = ((VarcharType*)(right_column->data()))[right.segment_offset_];
                    if(left_ref == right_ref) {
                        continue;
                    }

                    if(order_type == OrderType::kAsc) {
                        return left_ref < right_ref;
                    } else {
                        return left_ref > right_ref;
                    }
                }
                case kDate: {
                    NotImplementError("Date comparation isn't implemented.")
                }
                case kTime: {
                    NotImplementError("Time comparation isn't implemented.")
                }
                case kDateTime: {
                    NotImplementError("Datetime comparation isn't implemented.")
                }
                case kTimestamp: {
                    NotImplementError("Timestamp comparation isn't implemented.")
                }
                case kInterval: {
                    NotImplementError("Interval comparation isn't implemented.")
                }
                case kMixed: {
                    NotImplementError("Heterogeneous comparation isn't implemented.")
                }
                default: {
                    ExecutorError("Unexpected data type")
                }
            }
        }
#endif
        return true;
    }

private:
    const SharedPtr<DataTable> &order_by_table_;
    const Vector<OrderType> &order_by_types_;
};

void PhysicalSort::Init() {}

bool PhysicalSort::Execute(QueryContext *, OperatorState *) {
#if 0
    executor_.Init(this->expressions_);

    input_table_ = left_->output();

    ExecutorAssert(input_table_ != nullptr, "Input table count isn't matched.");

    // output table definition is same as input
    SizeT column_count = input_table_->ColumnCount();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SharedPtr<DataType> col_type = input_table_->GetColumnTypeById(idx);
        String col_name = input_table_->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, col_type, col_name, HashSet<ConstraintType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"),
                                                   MakeShared<String>("sort"),
                                                   columns);

    output_ = DataTable::Make(table_def, TableType::kIntermediate);

    // Generate table before getting the order of row id.
    SharedPtr<DataTable> order_by_table = GetOrderTable();

    // Fill the order by table
    this->executor_.Execute(input_table_, order_by_table);

    Sort(order_by_table, order_by_types_);
#endif
    return true;
}

SharedPtr<DataTable> PhysicalSort::GetOrderTable() const {
    SizeT column_count = this->expressions_.size();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SharedPtr<DataType> col_type = MakeShared<DataType>(this->expressions_[idx]->Type());
        String col_name = this->expressions_[idx]->Name();

        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, col_type, col_name, HashSet<ConstraintType>());
        columns.emplace_back(col_def);
    }

    //    // offset column is used to indicate which row this data belong to.
    //    SharedPtr<ColumnDef> offset_col = ColumnDef::Make("_offset",
    //                                                      column_count,
    //                                                      DataType(LogicalType::kInteger),
    //                                                      Set<ConstrainType>());
    //
    //    columns.emplace_back(offset_col);

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("order_by_key_table"), columns);

    return DataTable::Make(table_def, TableType::kIntermediate);
}

void PhysicalSort::Sort(const SharedPtr<DataTable> &order_by_table, const Vector<OrderType> &order_by_types) {
    // Generate row id vector
    SharedPtr<Vector<RowID>> rowid_vector = order_by_table->GetRowIDVector();

    std::sort(rowid_vector->begin(), rowid_vector->end(), Comparator(order_by_table, order_by_types));

    // Generate
    output_ = GenerateOutput(input_table_, rowid_vector);
}

SharedPtr<DataTable> PhysicalSort::GenerateOutput(const SharedPtr<DataTable> &input_table, const SharedPtr<Vector<RowID>> &rowid_vector) {
    // output table definition is same as input
    SizeT column_count = input_table->ColumnCount();
    Vector<SharedPtr<DataType>> types;
    types.reserve(column_count);
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SharedPtr<DataType> col_type = input_table->GetColumnTypeById(idx);
        types.emplace_back(col_type);

        String col_name = input_table->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, col_type, col_name, HashSet<ConstraintType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("sort"), columns);
    SharedPtr<DataTable> output_table = DataTable::Make(table_def, TableType::kIntermediate);

    const Vector<SharedPtr<DataBlock>> &input_datablocks = input_table->data_blocks_;

    //    SizeT vector_count = rowid_vector->size();
    SizeT vector_idx = 0;
#if 0
    SizeT block_count = input_table->data_blocks_.size();
    for(SizeT block_id = 0; block_id < block_count; ++block_id) {
        SharedPtr<DataBlock> output_datablock = DataBlock::Make();
        output_datablock->Init(types);
        const Vector<SharedPtr<ColumnVector>>& output_column_vectors = output_datablock->column_vectors;

        SizeT block_row_count = input_datablocks[block_id]->row_count();
        for(SizeT block_row_idx = 0; block_row_idx < block_row_count; ++block_row_idx) {
            RowID row_id = rowid_vector->at(vector_idx++);
            u32 input_block_id = row_id.segment_id_;
            u32 input_offset = row_id.segment_offset_;

            for(SizeT column_id = 0; column_id < column_count; ++column_id) {
                switch(types[column_id]->type()) {
                    case LogicalType::kBoolean: {
                        ((BooleanT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((BooleanT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case LogicalType::kTinyInt: {
                        ((TinyIntT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((TinyIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case LogicalType::kSmallInt: {
                        ((SmallIntT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((SmallIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case LogicalType::kInteger: {
                        ((IntegerT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((IntegerT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case LogicalType::kBigInt: {
                        ((BigIntT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((BigIntT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case kHugeInt: {
                        NotImplementError("HugeInt data shuffle isn't implemented.")
                    }
                    case kFloat: {
                        ((FloatT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((FloatT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case kDouble: {
                        ((DoubleT*)(output_column_vectors[column_id]->data()))[block_row_idx]
                                = ((DoubleT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        break;
                    }
                    case kDecimal: {
                        NotImplementError("Decimal data shuffle isn't implemented.")
                    }
                    case kVarchar: {
                        VarcharT& dst_ref = ((VarcharT*)(output_datablock->column_vectors[column_id]->data()))[block_row_idx];
                        VarcharT& src_ref = ((VarcharT*)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                        if(src_ref.IsInlined()) {
                            memcpy((char*)&dst_ref, (char*)&src_ref, sizeof(VarcharT));
                        } else {
                            dst_ref.length = src_ref.length;
                            memcpy(dst_ref.prefix, src_ref.prefix, VarcharT::PREFIX_LENGTH);

                            dst_ref.ptr = output_datablock->column_vectors[column_id]->buffer_->fix_heap_mgr_->Allocate(
                                    src_ref.length);
                            memcpy(dst_ref.ptr, src_ref.ptr, src_ref.length);
                        }
                        break;
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
        }

        for(SizeT column_id = 0; column_id < column_count; ++column_id) {
            output_column_vectors[column_id]->tail_index_ = block_row_count;
        }

        output_datablock->Finalize();

        output_table->Append(output_datablock);
    }
    return output_table;
#endif
    return nullptr;
}

} // namespace infinity
