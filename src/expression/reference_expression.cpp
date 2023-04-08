//
// Created by jinhai on 23-2-17.
//

#include "reference_expression.h"

namespace infinity {

String
ReferenceExpression::ToString() const {
    if(alias_.empty()) {
        bool all_digits{true};
        for(char a: column_name_) {
            if(!std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if(all_digits) {
            return table_name_ + "." + column_name_;
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

}