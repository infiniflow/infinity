#pragma once

/**
 * Refers to https://www.elastic.co/guide/en/elasticsearch/reference/current/search-search.html
 * knn  - (Optional, object or array of objects) Defines the kNN query to run.
 * query - (Optional, query object) Defines the search definition using the Query DSL.
 * rank - [preview] (Optional, object) Defines a method for combining and ranking result sets from a combination of query,
 * sub searches, and/or knn searches. Requires a minimum of 2 results sets for ranking from the specified sources.
 */
#include "parsed_expr.h"
#include <string>
#include <utility>
#include <vector>

namespace infinity {

class MatchExpr;
class QueryExpr;
class KnnExpr;
class MatchTensorExpr;
class FusionExpr;

class SearchExpr : public ParsedExpr {
public:
    explicit SearchExpr() : ParsedExpr(ParsedExprType::kSearch) {}

    ~SearchExpr() override;

    [[nodiscard]] std::string ToString() const override;

    void SetExprs(std::vector<infinity::ParsedExpr *> *exprs);
    void AddExpr(infinity::ParsedExpr *expr);
    void Validate() const;

public:
    std::vector<MatchExpr *> match_exprs_{};
    std::vector<KnnExpr *> knn_exprs_{};
    std::vector<MatchTensorExpr *> match_tensor_exprs_{};
    std::vector<FusionExpr *> fusion_exprs_{};

private:
    std::vector<infinity::ParsedExpr *> *exprs_{};
};

} // namespace infinity