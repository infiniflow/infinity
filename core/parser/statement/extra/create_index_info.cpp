//
// Created by jinhai on 23-3-3.
//

#include "create_index_info.h"

namespace infinity {

CreateIndexInfo::~CreateIndexInfo() {
    if (column_names_ != nullptr) {
        delete column_names_;
        column_names_ = nullptr;
    }
}

std::string CreateIndexInfo::ToString() const { return "create index info"; }

} // namespace infinity