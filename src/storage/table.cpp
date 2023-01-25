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

Vector<SharedPtr<Vector<i32>>>
Table::GetOffsetVectors() const {
    SizeT block_count = data_blocks_.size();
    Vector<SharedPtr<Vector<i32>>> result(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        result[idx] = data_blocks_[idx]->GenerateOffset();
    }
    return result;
}

}
