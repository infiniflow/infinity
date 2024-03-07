#include "fusion_expr.h"
#include "search_options.h"
#include "spdlog/fmt/fmt.h"
#include "sstream"
#include <cmath>
#include <utility>

namespace infinity {

FusionExpr::FusionExpr() : ParsedExpr(ParsedExprType::kFusion) {}

FusionExpr::~FusionExpr(){};

std::string FusionExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    std::ostringstream oss;
    oss << "FUSION('";
    oss << method_;
    oss << "', '";
    oss << options_->ToString();
    oss << "')";
    return oss.str();
}

void FusionExpr::SetOptions(const std::string &options) { options_ = std::make_shared<SearchOptions>(options); }

} // namespace infinity
