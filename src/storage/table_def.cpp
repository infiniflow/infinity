//
// Created by jinhai on 22-12-25.
//

#include "table_def.h"

namespace infinity {

String
TableDef::ToString() const {
    std::stringstream ss;
    ss << name_ << '[';
    for(size_t i = 0; auto& column_ptr: this->columns_) {
        ss << column_ptr->name();
        ++ i;
        if(i != this->column_count()) {
            ss << ", ";
        }
    }
    ss << ']';
    return ss.str();
}

void
TableDef::UnionWith(const SharedPtr<TableDef>& other) {
    SizeT this_column_count = this->column_count();
    SizeT other_column_count = other->column_count();
    this->columns_.reserve(this_column_count + other_column_count);
    this->columns_.insert(this->columns_.end(), other->columns_.begin(), other->columns_.end());

    SizeT total_column_count = this_column_count + other_column_count;
    for(SizeT idx = this_column_count; idx < total_column_count; ++ idx) {
        column_name2id_[columns_[idx]->name()] = idx;
    }
}

}