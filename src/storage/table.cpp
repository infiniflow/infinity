//
// Created by JinHai on 2022/7/29.
//

#include "table.h"

namespace infinity {

static String
TableTypeToString(TableType type) {
    switch (type) {
        case TableType::kInvalid:
            TypeError("Unexpected table type: Invalid")
        case TableType::kDataTable:
            return "DataTable";
        case TableType::kIntermediate:
            return "Intermediate";
        case TableType::kResult:
            return "Result";
        case TableType::kOrderBy:
            return "OrderBy";
        case TableType::kGroupBy:
            return "GroupBy";
        case TableType::kAggregate:
            return "Aggregate";
        case TableType::kCrossProduct:
            return "CrossProduct";
    }
    TypeError("Unexpected error.")
}

String
Table::ToString() const {
    std::stringstream ss;
    ss << definition_ptr_->ToString();
    ss << "Table type: " << TableTypeToString(type_) << " Row count: " << row_count_ << std::endl;

    SizeT block_count = data_blocks_.size();
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        ss << "Block " << idx << std::endl;
        ss << data_blocks_[idx]->ToString();
    }

    return ss.str();
}

SharedPtr<Vector<RowID>>
Table::GetRowIDVector() const {
    SizeT block_count = data_blocks_.size();
    SharedPtr<Vector<RowID>> result = MakeShared<Vector<RowID>>();
    result->reserve(row_count_);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        data_blocks_[idx]->FillRowIDVector(result, idx);
    }
    return result;
}

void
Table::UnionWith(const SharedPtr<Table>& other) {
    StorageAssert(this->row_count_ == other->row_count_,
                  fmt::format("Can't union two table with different row count {}:{}",
                              this->row_count_,
                              other->row_count_));

    StorageAssert(this->data_blocks_.size() == other->data_blocks_.size(),
                  fmt::format("Can't union two table with different block count {}:{}",
                              this->data_blocks_.size(),
                              other->data_blocks_.size()));

    SizeT block_count = this->data_blocks_.size();
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        this->data_blocks_[idx]->UnionWith(other->data_blocks_[idx]);
    }

    this->definition_ptr_->UnionWith(other->definition_ptr_);
}

}
