//
// Created by jinhai on 23-2-28.
//

#include "table_reference.h"

namespace infinity {

std::string TableReference::ToString() { return "TableReference"; }

std::string TableReference::GetTableName() const {
    if (alias_ != nullptr) {
        return std::string(alias_->alias_);
    }
    return table_name_;
}

}; // namespace infinity
