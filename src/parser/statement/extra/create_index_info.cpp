//
// Created by jinhai on 23-3-3.
//

#include "create_index_info.h"
#include "common/utility/infinity_assert.h"
#include <sstream>

namespace infinity {

CreateIndexInfo::~CreateIndexInfo() {
    if (column_names_ != nullptr) {
        delete column_names_;
        column_names_ = nullptr;
    }
}

String CreateIndexInfo::ToString() const {
    std::stringstream ss;
    ss << "CREATE INDEX ";
    switch (conflict_type_) {
        case ConflictType::kIgnore: {
            ss << "IF NOT EXISTS ";
        }
        case ConflictType::kError: {
            break;
        }
        case ConflictType::kReplace: {
            NotImplementError("Not implemented.");
        }
        case ConflictType::kInvalid: {
            ParserError("Invalid conflict type.");
        }
    }
    if (index_name_.empty()) {
        ss << "DEFAULT_INDEX_NAME ";
    } else {
        ss << index_name_;
    }
    ss << "ON " << table_name_ << "(";
    SizeT column_count = column_names_->size();
    if (column_count == 0) {
        ParserError("No column name specified.");
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        ss << (*column_names_)[idx] << ", ";
    }
    ss << (*column_names_)[column_count - 1] << ")";
    ss << "USING " << method_type_;
    if (index_para_list_ != nullptr && !index_para_list_->empty()) {
        ss << "WITH(";
        SizeT para_count = index_para_list_->size();
        for (SizeT idx = 0; idx < para_count; ++idx) {
            ss << (*index_para_list_)[idx]->para_name_ << " " << (*index_para_list_)[idx]->para_value_;
            if (idx != para_count - 1) {
                ss << ", ";
            }
        }
        ss << ")";
    }
    ss << ";";
    return ss.str();
}

} // namespace infinity