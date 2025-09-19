#include "match_expr.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {

MatchExpr::MatchExpr() : ParsedExpr(ParsedExprType::kMatch) {}

MatchExpr::~MatchExpr() {}

std::string MatchExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    std::ostringstream oss;
    oss << "MATCH TEXT ('";
    oss << fields_;
    oss << "', '" << matching_text_ << "'";
    oss << ", '" << options_text_ << "'";
    oss << ", '" << index_names_ << "'";
    if (filter_expr_) {
        oss << ", WHERE " << filter_expr_->ToString();
    }
    oss << ")";
    return std::move(oss).str();
}

} // namespace infinity
