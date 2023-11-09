#include "query_expr.h"
#include "search_parser.h"
#include "spdlog/fmt/fmt.h"
#include <cmath>
#include <sstream>
#include <utility>

namespace infinity {

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

void QueryExpr::SetOptions(const std::string &options) { SearchParser::ParseOptions(options, options_); }

} // namespace infinity
