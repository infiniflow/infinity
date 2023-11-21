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
} // namespace irs

namespace infinity {

class SearchOptions;

class MatchExpr : public ParsedExpr {
public:
    explicit MatchExpr();

    ~MatchExpr() override;

    [[nodiscard]] std::string ToString() const override;

    int SetFilter(const std::string &fields, const std::string &matching_text);
    void SetOptions(const std::string &options);

public:
    std::unique_ptr<irs::filter> flt_{};
    std::string fields_;
    std::string matching_text_;
    std::shared_ptr<SearchOptions> options_{};
};

} // namespace infinity