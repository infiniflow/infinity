//
// Created by JinHai on 2022/7/28.
//

#include "physical_sort.h"

namespace infinity {

class Comparator {
public:
    explicit
    Comparator(const SharedPtr<Table>& order_by_table, const Vector<OrderByType>& order_by_types)
            : order_by_table_(order_by_table), order_by_types_(order_by_types)
    {}

    bool
    operator() (RowID left, RowID right) {
        SizeT column_count = order_by_table_->ColumnCount();
        for(SizeT col_id = 0; col_id < column_count; ++ col_id) {

            DataType type = order_by_table_->GetColumnTypeById(col_id);
            OrderByType order_type = order_by_types_[col_id];

            const SharedPtr<DataBlock>& left_block = order_by_table_->GetDataBlockById(left.block);
            const SharedPtr<ColumnVector>& left_column = left_block->column_vectors[col_id];
            const SharedPtr<DataBlock>& right_block = order_by_table_->GetDataBlockById(right.block);
            const SharedPtr<ColumnVector>& right_column = right_block->column_vectors[col_id];

            switch(type.type()) {
                case kBoolean: {
                    BooleanT left_value = ((BooleanT*)(left_column->data()))[left.offset];
                    BooleanT right_value = ((BooleanT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kTinyInt: {
                    TinyIntT left_value = ((TinyIntT*)(left_column->data()))[left.offset];
                    TinyIntT right_value = ((TinyIntT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kSmallInt: {
                    SmallIntT left_value = ((SmallIntT*)(left_column->data()))[left.offset];
                    SmallIntT right_value = ((SmallIntT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kInteger: {
                    IntegerT left_value = ((IntegerT *)(left_column->data()))[left.offset];
                    IntegerT right_value = ((IntegerT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kBigInt: {
                    BigIntT left_value = ((BigIntT *)(left_column->data()))[left.offset];
                    BigIntT right_value = ((BigIntT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kHugeInt: {
                    NotImplementError("HugeInt comparation isn't implemented.")
                }
                case kFloat: {
                    FloatT left_value = ((FloatT *)(left_column->data()))[left.offset];
                    FloatT right_value = ((FloatT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kDouble: {
                    DoubleT left_value = ((DoubleT *)(left_column->data()))[left.offset];
                    DoubleT right_value = ((DoubleT*)(right_column->data()))[right.offset];
                    if(left_value == right_value) {
                        continue;
                    }

                    if(order_type == OrderByType::kAscending) {
                        return left_value < right_value;
                    } else {
                        return left_value > right_value;
                    }
                }
                case kDecimal16: {
                    NotImplementError("Decimal16 comparation isn't implemented.")
                }
                case kDecimal32: {
                    NotImplementError("Decimal32 comparation isn't implemented.")
                }
                case kDecimal64: {
                    NotImplementError("Decimal64 comparation isn't implemented.")
                }
                case kDecimal128: {
                    NotImplementError("Decimal128 comparation isn't implemented.")
                }
                case kVarchar: {
                    NotImplementError("Varchar comparation isn't implemented.")
                }
                case kChar: {
                    NotImplementError("Char comparation isn't implemented.")
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
                case kTimestampTZ: {
                    NotImplementError("TimestampTZ comparation isn't implemented.")
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
        return true;
    }

private:
    const SharedPtr<Table>& order_by_table_;
    const Vector<OrderByType>& order_by_types_;
};

void
PhysicalSort::Init() {
    executor.Init(this->expressions_);

    // output table definition is same as input
    SharedPtr<Table> output_table = left_->output();
    SizeT column_count = output_table->ColumnCount();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = output_table->GetColumnTypeById(idx);
        String col_name = output_table->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make("sort", columns, false);

    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalSort::Execute(SharedPtr<QueryContext>& query_context) {

    // Generate table before getting the order of row id.
    SharedPtr<Table> order_by_table = GetOrderTable();

    // Fill the order by table
    this->executor.Execute(left_->output(), order_by_table);

    Sort(order_by_table, order_by_types_);
    // Sort by each table data block and get sorted offset

    // merge each sorted data block and generated row id selection

    // Scatter the input table as the row id selection into output table

    output_ = left_->output();
}

void
PhysicalSort::Sort(const SharedPtr<Table>& order_by_table,
                   const Vector<OrderByType>& order_by_types) {
    // Generate row id vector
    SharedPtr<Vector<RowID>> rowid_vector = order_by_table->GetRowIDVector();

    std::sort(rowid_vector->begin(), rowid_vector->end(), Comparator(order_by_table, order_by_types));

    // Generate
}

SharedPtr<Table>
PhysicalSort::GetOrderTable() const {
    SizeT column_count = this->expressions_.size();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = this->expressions_[idx]->Type();
        String col_name = this->expressions_[idx]->ToString();

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

//    // offset column is used to indicate which row this data belong to.
//    SharedPtr<ColumnDef> offset_col = ColumnDef::Make("_offset",
//                                                      column_count,
//                                                      DataType(LogicalType::kInteger),
//                                                      Set<ConstrainType>());
//
//    columns.emplace_back(offset_col);

    SharedPtr<TableDef> table_def = TableDef::Make("order_by_key_table", columns, false);

    return Table::Make(table_def, TableType::kIntermediate);
}

}

