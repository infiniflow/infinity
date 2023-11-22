#include "match_expr.h"
#include "parser_assert.h"
#include "query_driver.h"
#include "search/filter.hpp"
#include "search_parser.h"
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
    oss << "MATCH(";
    oss << fields_;
    oss << ", '" << matching_text_ << "'";
    oss << SearchParser::OptionsToString(options_);
    oss << ")";
    return oss.str();
}

void MatchExpr::SetOptions(const std::string &options) { SearchParser::ParseOptions(options, options_); }

int MatchExpr::SetFilter(const std::string &fields, const std::string &matching_text) {
    matching_text_ = matching_text;
    fields_ = fields;
    QueryDriver driver;
    int rc = driver.ParseSingleWithFields(fields, matching_text);
    flt_ = std::move(driver.result);
    return rc;
}

} // namespace infinity
