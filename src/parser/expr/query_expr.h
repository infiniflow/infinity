#pragma once

/**
 * Refers to https://www.elastic.co/guide/en/elasticsearch/reference/current/sql-functions-search.html
 * MATCH('author', 'frank')
 * MATCH('author^2,name^5', 'frank dune')
 * MATCH('name', 'to the star', 'operator=OR;fuzziness=AUTO:1,5;minimum_should_match=1')
 *
 * QUERY('name:dune')
 * QUERY('_exists_:"author" AND page_count:>200 AND (name:/star./ OR name:duna~)')
 * QUERY('dune god', 'default_operator=and;default_field=name');
 */
#include "expr.h"
#include <memory>
#include <string>
#include <utility>

namespace irs {
class filter;
}

namespace infinity {

class SearchOptions;

class QueryExpr : public ParsedExpr {
public:
    explicit QueryExpr();

    ~QueryExpr() override;

    int SetFilter(const std::string &query_text);

    [[nodiscard]] std::string ToString() const override;
    void SetOptions(const std::string &options);

public:
    std::unique_ptr<irs::filter> flt_ = nullptr;
    std::string query_text_;
    std::shared_ptr<SearchOptions> options_ = nullptr;
};

} // namespace infinity