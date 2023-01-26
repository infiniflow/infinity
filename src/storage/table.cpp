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
    }
    TypeError("Unexpected error.")
}

String
Table::ToString() const {
    std::stringstream ss;
    ss << definition_ptr_->ToString();
    ss << "Table type: " << TableTypeToString(type_) << " Row count: " << row_count_;

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

}
