//
// Created by jinhai on 23-2-28.
//

#include "table_reference.h"

namespace infinity {

String TableReference::ToString() { return "TableReference"; }

String TableReference::GetTableName() const {
    if (alias_ != nullptr) {
        return String(alias_->alias_);
    }
    return table_name_;
}

}; // namespace infinity
