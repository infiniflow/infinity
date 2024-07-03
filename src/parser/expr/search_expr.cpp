#include "search_expr.h"
#include "fusion_expr.h"
#include "knn_expr.h"
#include "match_expr.h"
#include "match_tensor_expr.h"
#include "match_sparse_expr.h"
#include "spdlog/fmt/fmt.h"
#include <cmath>
#include <sstream>
#include <utility>

namespace infinity {

SearchExpr::~SearchExpr() {
    if (exprs_ != nullptr) {
        for (auto &expr : *exprs_) {
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
    for (auto &expr : fusion_exprs_) {
        if (!is_first)
            oss << ", ";
        oss << expr->ToString();
        is_first = false;
    }
    return oss.str();
}

void SearchExpr::SetExprs(std::vector<infinity::ParsedExpr *> *exprs) {
    if (exprs == nullptr) {
        ParserError("SearchExpr::SetExprs parameter is nullptr");
    }
    if (exprs_ != nullptr) {
        ParserError("SearchExpr::SetExprs member exprs_ is not nullptr");
    }
    exprs_ = exprs;
    for (ParsedExpr *expr : *exprs) {
        AddExpr(expr);
    }
    Validate();
}

void SearchExpr::Validate() const {
    size_t num_sub_expr = match_exprs_.size();
    if (num_sub_expr <= 0) {
        ParserError("Need at least one MATCH VECTOR / MATCH TENSOR / MATCH TEXT / MATCH SPARSE / QUERY expression");
    } else if (num_sub_expr >= 2) {
        if (fusion_exprs_.empty()) {
            ParserError("Need FUSION expr since there are multiple MATCH VECTOR / MATCH TENSOR / MATCH TEXT / MATCH SPARSE / QUERY expressions");
        }
    }
}

void SearchExpr::AddExpr(infinity::ParsedExpr *expr) {
    switch (expr->type_) {
        case ParsedExprType::kKnn:
        case ParsedExprType::kMatch:
        case ParsedExprType::kMatchTensor:
        case ParsedExprType::kMatchSparse:
            match_exprs_.push_back(expr);
            break;
        case ParsedExprType::kFusion:
            fusion_exprs_.push_back(static_cast<FusionExpr *>(expr));
            break;
        default:
            ParserError("Invalid expr type for SEARCH");
    }
}

} // namespace infinity
