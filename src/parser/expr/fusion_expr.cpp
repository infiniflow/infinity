#include "fusion_expr.h"
#include "search_parser.h"
#include "spdlog/fmt/fmt.h"
#include "sstream"
#include <cmath>
#include <utility>

namespace infinity {

std::string FusionExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    std::ostringstream oss;
    oss << "FUSION('";
    oss << method_;
    oss << "'";
    oss << SearchParser::OptionsToString(options_);
    oss << ")";
    return oss.str();
}

void FusionExpr::SetOptions(const std::string &options) { SearchParser::ParseOptions(options, options_); }

} // namespace infinity
