#include "search_expr.h"
#include "fusion_expr.h"
#include "knn_expr.h"
#include "match_expr.h"
#include "spdlog/fmt/fmt.h"
#include <cmath>
#include <sstream>
#include <utility>

namespace infinity {

SearchExpr::~SearchExpr() {
    if (exprs_ != nullptr) {
        for (auto& expr : *exprs_) {
            delete expr;
            expr = nullptr;
        }
        delete exprs_;
        exprs_ = nullptr;
    }
}

std::string SearchExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    std::ostringstream oss;
    oss << "SEARCH ";
    bool is_first = true;
    for (auto &expr : match_exprs_) {
        if (!is_first)
            oss << ", ";
        oss << expr->ToString();
        is_first = false;
    }
    for (auto &expr : knn_exprs_) {
        if (!is_first)
            oss << ", ";
        oss << expr->ToString();
        is_first = false;
    }
    if (fusion_expr_ != nullptr) {
        oss << ", " << fusion_expr_->ToString();
    }
    return oss.str();
}

void SearchExpr::SetExprs(std::vector<infinity::ParsedExpr *> *exprs) {
    exprs_ = exprs;
    for (ParsedExpr *expr : *exprs) {
        switch (expr->type_) {
            case ParsedExprType::kKnn:
                knn_exprs_.push_back(dynamic_cast<KnnExpr *>(expr));
                break;
            case ParsedExprType::kMatch:
                match_exprs_.push_back(dynamic_cast<MatchExpr *>(expr));
                break;
            case ParsedExprType::kFusion:
                if (fusion_expr_ != nullptr) {
                    ParserError("More than one FUSION expr");
                }
                fusion_expr_ = dynamic_cast<FusionExpr *>(expr);
                break;
            default:
                ParserError("Invalid expr type for SEARCH");
        }
    }
    size_t num_sub_expr = knn_exprs_.size() + match_exprs_.size();
    if (num_sub_expr <= 0) {
        ParserError("Need at least one KNN/MATCH/QUERY expression");
    } else if (num_sub_expr >= 2) {
        if (fusion_expr_ == nullptr)
            ParserError("Need FUSION expr since there are multiple KNN/MATCH/QUERY expressions");
    }
}

} // namespace infinity
