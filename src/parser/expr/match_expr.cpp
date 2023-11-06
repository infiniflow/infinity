#include "match_expr.h"
#include "search_parser.h"
#include "spdlog/fmt/fmt.h"
#include <cmath>
#include <sstream>
#include <utility>

namespace infinity {

std::string MatchExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    std::ostringstream oss;
    oss << "MATCH(";
    oss << SearchParser::FieldsToString(fields_);
    oss << ", '" << matching_text_ << "'";
    oss << SearchParser::OptionsToString(options_);
    oss << ")";
    return oss.str();
}

void MatchExpr::SetFields(const std::string &fields) { SearchParser::ParseFields(fields, fields_); }

void MatchExpr::SetOptions(const std::string &options) { SearchParser::ParseOptions(options, options_); }

} // namespace infinity
