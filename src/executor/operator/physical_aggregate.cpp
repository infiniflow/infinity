//
// Created by JinHai on 2022/7/28.
//

#include "physical_aggregate.h"
#include "executor/expression/expression_executor.h"
#include "executor/hash_table.h"

namespace infinity {

void
PhysicalAggregate::Init() {
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

    SharedPtr<TableDef> table_def = TableDef::Make("aggregate", columns, false);

    output_ = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalAggregate::Execute(SharedPtr<QueryContext>& query_context) {
    this->output_ = this->left_->output();

    // 1. Execute group-by expressions to generate unique key.
    ExpressionExecutor groupby_executor;
    groupby_executor.Init(groups_);

    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(groups_.size());

    Vector<DataType> types;
    types.reserve(groups_.size());

    for(i64 idx = 0; auto& expr: groups_) {
        SharedPtr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
        columns.emplace_back(col_def);
        types.emplace_back(expr->Type());
        ++ idx;
    }

    SharedPtr<TableDef> table_def = TableDef::Make("groupby", columns, false);
    SharedPtr<Table> groupby_table = Table::Make(table_def, TableType::kIntermediate);

    auto input_table = left_->output();
    groupby_executor.Execute(input_table, groupby_table);

    // 2. Use the unique key to get the row list of the same key.
    HashTable hash_table;
    hash_table.Init(types);

    SizeT block_count = groupby_table->DataBlockCount();
    for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
        const SharedPtr<DataBlock>& block_ptr = groupby_table->GetDataBlockById(block_id);
        hash_table.Append(block_ptr->column_vectors, block_id, block_ptr->row_count());
    }

    // 3. forlop each aggregates function on each group by bucket, to calculate the result according to the row list

    // 4. generate the result to output

    this->output_ = left_->output();
}

}
