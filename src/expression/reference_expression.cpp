//
// Created by jinhai on 23-2-17.
//

module;

#include <cctype>

import stl;
import third_party;

module reference_expression;

namespace infinity {

String ReferenceExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        SizeT name_len = column_name_.size();
        for (SizeT idx = 0; idx < name_len; ++idx) {
            char a = column_name_[idx];
            if (!std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return Format("{}.{}", table_name_, column_name_);
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

} // namespace infinity