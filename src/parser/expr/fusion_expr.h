#pragma once

/**
 * Refers to https://www.elastic.co/guide/en/elasticsearch/reference/current/sql-functions-search.html
 * MATCH(author, 'frank')
 * MATCH('author^2,name^5', 'frank dune')
 * MATCH(name, 'to the star', 'operator=OR;fuzziness=AUTO:1,5;minimum_should_match=1')
 *
 * QUERY('name:dune')
 * QUERY('_exists_:"author" AND page_count:>200 AND (name:/star./ OR name:duna~)')
 * QUERY('dune god', 'default_operator=and;default_field=name');
 */
#include "match_tensor_expr.h"
#include "parsed_expr.h"
#include "search_options.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace infinity {

class FusionExpr : public ParsedExpr {
public:
    explicit FusionExpr();

    ~FusionExpr() override;

    [[nodiscard]] std::string ToString() const override;

    void SetOptions(const std::string &options);
    void JobAfterParser();

public:
    std::string method_;
    std::shared_ptr<SearchOptions> options_;
    std::unique_ptr<MatchTensorExpr> match_tensor_expr_;
};

} // namespace infinity