#include "check_statement.h"
#include <sstream>

namespace infinity {

std::string CheckStatement::ToString() const {
    std::stringstream ss;
    switch (check_type_) {
        case CheckStmtType::kInvalid: {
            ParserError("Invalid check statement");
            break;
        }
        case CheckStmtType::kSystem: {
            ss << "Check system";
            break;
        }
        case CheckStmtType::kTable: {
            ss << "Check table";
            break;
        }
    }
    return ss.str();
}

}; // namespace infinity
