//
// Created by jinhai on 23-1-22.
//

#include "parsed_column_expression.h"

namespace infinity {
String
ParsedColumnExpression::ToString() const {
    String result;
    if(!db_name_.empty()) {
        result = db_name_;
    }

    if(!schema_name_.empty()) {
        if(!result.empty()) {
            result += ".";
        }
        result += schema_name_;
    }

    if(!table_name_.empty()) {
        if(!result.empty()) {
            result += ".";
        }
        result += table_name_;
    }

    if(!result.empty()) {
        result += ".";
    }
    result += column_name_;
    return result;
}

}
