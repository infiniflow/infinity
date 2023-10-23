//
// Created by jinhai on 23-2-22.
//

#include "show_statement.h"
#include <sstream>

namespace infinity {

std::string ShowStatement::ToString() const {
    std::stringstream ss;
    switch (show_type_) {
        case ShowStmtType::kTables: {
            ss << "Show tables, schema: " << schema_name_;
            break;
        }
        case ShowStmtType::kCollections: {
            ss << "Show collections, schema: " << schema_name_;
            break;
        }
        case ShowStmtType::kViews: {
            ss << "Show views, schema: " << schema_name_;
            break;
        }
        case ShowStmtType::kColumns: {
            ss << "Describe table: " << schema_name_ << "." << table_name_;
            break;
        }
    }
    return ss.str();
}

} // namespace infinity
