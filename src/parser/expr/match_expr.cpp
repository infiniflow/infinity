#include "match_expr.h"
#include "parser_assert.h"
#include "search_options.h"
#include "spdlog/fmt/fmt.h"

#include <cmath>
#include <sstream>
#include <utility>

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
    if (filter_expr_) {
        oss << ", WHERE " << filter_expr_->ToString();
    }
    oss << ")";
    return std::move(oss).str();
}

} // namespace infinity
