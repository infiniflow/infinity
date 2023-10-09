//
// Created by jinhai on 23-9-21.
//

#include "logical_knn_scan.h"
#include "planner/bound/base_table_ref.h"

namespace infinity {

LogicalKnnScan::LogicalKnnScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref)
    : LogicalNode(node_id, LogicalNodeType::kKnnScan), base_table_ref_(std::move(base_table_ref)) {}

Vector<ColumnBinding> LogicalKnnScan::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT column_count = base_table_ref_->column_names_->size();
    result.reserve(column_count);
    for (SizeT i = 0; i < column_count; ++i) {
        result.emplace_back(base_table_ref_->table_index_, i);
    }

    column_count = knn_expressions_.size();
    for (SizeT i = 0; i < column_count; ++i) {
        result.emplace_back(knn_table_index_, i);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalKnnScan::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();

    for (const auto &column_name : *base_table_ref_->column_names_) {
        result_names->emplace_back(column_name);
    }

    for (const auto &knn_expr : knn_expressions_) {
        result_names->emplace_back(knn_expr->Name());
    }

    result_names->emplace_back("_row_id");
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalKnnScan::GetOutputTypes() const {
    Vector<SharedPtr<DataType>> result_types = *base_table_ref_->column_types_;
    for (const auto &knn_expr : knn_expressions_) {
        result_types.emplace_back(MakeShared<DataType>(knn_expr->Type()));
    }
    return MakeShared<Vector<SharedPtr<DataType>>>(result_types);
}

TableCollectionEntry *LogicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String LogicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalKnnScan::TableIndex() const { return base_table_ref_->table_index_; }

String LogicalKnnScan::ToString(i64 &space) {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "KnnScan: " << base_table_ref_->table_entry_ptr_->table_collection_name_ << ", on: ";
    size_t column_count = base_table_ref_->column_names_->size();
    for (size_t i = 0; i < column_count - 1; ++i) {
        ss << base_table_ref_->column_names_->at(i) << " ";
    }
    ss << base_table_ref_->column_names_->back();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
