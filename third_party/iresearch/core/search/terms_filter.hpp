////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2020 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <set>

#include "search/all_docs_provider.hpp"
#include "search/filter.hpp"
#include "utils/string.hpp"

namespace irs {

class by_terms;
struct filter_visitor;

/// Options for terms filter
struct by_terms_options {
  struct search_term {
    bstring term;
    score_t boost;

    search_term() = default;

    explicit search_term(bstring&& term, score_t boost = kNoBoost) noexcept
      : term(std::move(term)), boost(boost) {}

    explicit search_term(bytes_view term, score_t boost = kNoBoost)
      : term(term.data(), term.size()), boost(boost) {}

    bool operator==(const search_term& rhs) const noexcept {
      return term == rhs.term && boost == rhs.boost;
    }

    bool operator<(const search_term& rhs) const noexcept {
      return term < rhs.term;
    }

    size_t hash() const noexcept {
      return hash_combine(std::hash<decltype(boost)>()(boost), term);
    }
  };

  using filter_type = by_terms;
  using search_terms = std::set<search_term>;

  search_terms terms;
  size_t min_match{1};
  ScoreMergeType merge_type{ScoreMergeType::kSum};

  bool operator==(const by_terms_options& rhs) const noexcept {
    return min_match == rhs.min_match && merge_type == rhs.merge_type &&
           terms == rhs.terms;
  }

  size_t hash() const noexcept {
    size_t hash = hash_combine(0, min_match);
    for (auto& term : terms) {
      hash = hash_combine(hash, term.hash());
    }
    return hash_combine(hash, merge_type);
  }
};

// Filter by a set of terms
class by_terms final : public filter_base<by_terms_options>,
                       public AllDocsProvider {
 public:
  static ptr make();

  static void visit(const SubReader& segment, const term_reader& field,
                    const by_terms_options::search_terms& terms,
                    filter_visitor& visitor);

  filter::prepared::ptr prepare(const PrepareContext& ctx) const final;
};

}  // namespace irs

namespace std {

template<>
struct hash<::irs::by_terms_options> {
  size_t operator()(const ::irs::by_terms_options& v) const noexcept {
    return v.hash();
  }
};

}  // namespace std
