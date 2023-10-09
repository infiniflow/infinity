//
// Created by jinhai on 23-3-3.
//

#include "create_view_info.h"

namespace infinity {

CreateViewInfo::~CreateViewInfo() {
    if (select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }
    if (view_columns_ != nullptr) {
        delete view_columns_;
        view_columns_ = nullptr;
    }
}

String CreateViewInfo::ToString() const { return "Create view info"; }

} // namespace infinity
