#include "check_statement.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

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
