//
// Created by jinhai on 23-2-28.
//

#include "base_table_reference.h"

namespace infinity {

TableAlias::~TableAlias() {

    if (alias_ != nullptr) {
        free(alias_);
        alias_ = nullptr;
    }
    if (column_alias_array_ != nullptr) {
        delete column_alias_array_;
    }
}

BaseTableReference::~BaseTableReference() {
    if (alias_ != nullptr) {
        delete alias_;
        alias_ = nullptr;
    }
}

} // namespace infinity