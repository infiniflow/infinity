#include "query_expr.h"
#include "parser_assert.h"
#include "query_driver.h"
#include "search/filter.hpp"
#include "search_parser.h"
#include "spdlog/fmt/fmt.h"
#include <cmath>
#include <sstream>
#include <utility>

namespace infinity {

QueryExpr::QueryExpr() : ParsedExpr(ParsedExprType::kQuery) {}

QueryExpr::~QueryExpr() {}

std::string QueryExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    std::ostringstream oss;
    oss << "QUERY('";
    oss << query_text_;
    oss << "'";
    oss << SearchParser::OptionsToString(options_);
    oss << ")";
    return oss.str();
}

int QueryExpr::SetFilter(const std::string &query_text) {
    query_text_ = query_text;
    QueryDriver driver;
    int rc = driver.ParseSingle(query_text);
    flt_ = std::move(driver.result);
    return rc;
}

void QueryExpr::SetOptions(const std::string &options) { SearchParser::ParseOptions(options, options_); }

} // namespace infinity
