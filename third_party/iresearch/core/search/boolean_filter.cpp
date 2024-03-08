////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 by EMC Corporation, All Rights Reserved
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
/// Copyright holder is EMC Corporation
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#include "boolean_filter.hpp"

#include <functional>

#include "conjunction.hpp"
#include "disjunction.hpp"
#include "exclusion.hpp"
#include "min_match_disjunction.hpp"
#include "prepared_state_visitor.hpp"

namespace {

template <typename SizeT>
inline void HashCombine(size_t &seed, SizeT value)  noexcept {
  std::hash<SizeT> hasher;
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// first - pointer to the innermost not "not" node
// second - collapsed negation mark
std::pair<const irs::filter*, bool> optimize_not(const irs::Not& node) {
  bool neg = true;
  const irs::filter* inner = node.filter();
  while (inner && inner->type() == irs::type<irs::Not>::id()) {
    neg = !neg;
    inner = static_cast<const irs::Not*>(inner)->filter();
  }

  return std::make_pair(inner, neg);
}

template<bool Conjunction, typename It>
irs::ScoreAdapters<irs::doc_iterator::ptr> MakeScoreAdapters(
  const irs::ExecutionContext& ctx, It begin, It end) {
  IRS_ASSERT(begin <= end);
  const size_t size = std::distance(begin, end);
  irs::ScoreAdapters<irs::doc_iterator::ptr> itrs;
  itrs.reserve(size);
  if (Conjunction || size > 1) {
    ctx.wand.root = false;
    // TODO(MBkkt) ctx.wand.strict = true;
    // We couldn't do this for few reasons:
    // 1. It's small chance that we will use just term iterator (or + eof)
    // 2. I'm not sure about precision
  }
  do {
    auto docs = (*begin)->execute(ctx);
    ++begin;

    // filter out empty iterators
    if (irs::doc_limits::eof(docs->value())) {
      if constexpr (Conjunction) {
        return {};
      } else {
        continue;
      }
    }

    itrs.emplace_back(std::move(docs));
  } while (begin != end);

  return itrs;
}

// Returns disjunction iterator created from the specified queries
template<typename QueryIterator, typename... Args>
irs::doc_iterator::ptr make_disjunction(const irs::ExecutionContext& ctx,
                                        irs::ScoreMergeType merge_type,
                                        QueryIterator begin, QueryIterator end,
                                        Args&&... args) {
  IRS_ASSERT(begin <= end);
  const size_t size = std::distance(begin, end);
  // check the size before the execution
  if (0 == size) {
    // empty or unreachable search criteria
    return irs::doc_iterator::empty();
  }

  auto itrs = MakeScoreAdapters<false>(ctx, begin, end);
  if (itrs.empty()) {
    return irs::doc_iterator::empty();
  }

  return irs::ResoveMergeType(
    merge_type, ctx.scorers.buckets().size(),
    [&]<typename A>(A&& aggregator) -> irs::doc_iterator::ptr {
      using disjunction_t =
        irs::disjunction_iterator<irs::doc_iterator::ptr, A>;

      return irs::MakeDisjunction<disjunction_t>(ctx.wand, std::move(itrs),
                                                 std::move(aggregator),
                                                 std::forward<Args>(args)...);
    });
}

// Returns conjunction iterator created from the specified queries
template<typename QueryIterator, typename... Args>
irs::doc_iterator::ptr make_conjunction(const irs::ExecutionContext& ctx,
                                        irs::ScoreMergeType merge_type,
                                        QueryIterator begin, QueryIterator end,
                                        Args&&... args) {
  IRS_ASSERT(begin <= end);
  const size_t size = std::distance(begin, end);
  // check size before the execution
  switch (size) {
    case 0:
      return irs::doc_iterator::empty();
    case 1:
      return (*begin)->execute(ctx);
  }

  auto itrs = MakeScoreAdapters<true>(ctx, begin, end);
  if (itrs.empty()) {
    return irs::doc_iterator::empty();
  }

  return irs::ResoveMergeType(
    merge_type, ctx.scorers.buckets().size(),
    [&]<typename A>(A&& aggregator) -> irs::doc_iterator::ptr {
      return irs::MakeConjunction(ctx.wand, std::move(aggregator),
                                  std::move(itrs), std::forward<Args>(args)...);
    });
}

}  // namespace

namespace irs {

// Base class for boolean queries
class BooleanQuery : public filter::prepared {
 public:
  using queries_t = ManagedVector<filter::prepared::ptr>;
  using iterator = queries_t::const_iterator;

  BooleanQuery() noexcept : excl_{0} {}

  doc_iterator::ptr execute(const ExecutionContext& ctx) const final {
    if (empty()) {
      return doc_iterator::empty();
    }

    IRS_ASSERT(excl_);
    const auto excl_begin = this->excl_begin();
    const auto end = this->end();

    auto incl = execute(ctx, begin(), excl_begin);

    if (excl_begin == end) {
      return incl;
    }

    // exclusion part does not affect scoring at all
    auto excl = ::make_disjunction(
      {.segment = ctx.segment, .scorers = Scorers::kUnordered, .ctx = ctx.ctx},
      irs::ScoreMergeType::kSum, excl_begin, end);

    // got empty iterator for excluded
    if (doc_limits::eof(excl->value())) {
      // pure conjunction/disjunction
      return incl;
    }

    return memory::make_managed<exclusion>(std::move(incl), std::move(excl));
  }

  void visit(const irs::SubReader& segment, irs::PreparedStateVisitor& visitor,
             score_t boost) const final {
    boost *= this->boost();

    if (!visitor.Visit(*this, boost)) {
      return;
    }

    // FIXME(gnusi): visit exclude group?
    for (auto it = begin(), end = excl_begin(); it != end; ++it) {
      (*it)->visit(segment, visitor, boost);
    }
  }

  void prepare(const PrepareContext& ctx, ScoreMergeType merge_type,
               std::span<const filter* const> incl,
               std::span<const filter* const> excl) {
    BooleanQuery::queries_t queries{{ctx.memory}};
    queries.reserve(incl.size() + excl.size());

    // apply boost to the current node
    this->boost(ctx.boost);

    // prepare included
    for (const auto* filter : incl) {
      queries.emplace_back(filter->prepare(ctx));
    }

    // prepare excluded
    for (const auto* filter : excl) {
      // exclusion part does not affect scoring at all
      queries.emplace_back(filter->prepare({
        .index = ctx.index,
        .memory = ctx.memory,
        .ctx = ctx.ctx,
      }));
    }

    // nothrow block
    queries_ = std::move(queries);
    excl_ = incl.size();
    merge_type_ = merge_type;
  }

  iterator begin() const { return queries_.begin(); }
  iterator excl_begin() const { return begin() + excl_; }
  iterator end() const { return queries_.end(); }

  bool empty() const { return queries_.empty(); }
  size_t size() const { return queries_.size(); }

 protected:
  virtual doc_iterator::ptr execute(const ExecutionContext& ctx, iterator begin,
                                    iterator end) const = 0;

  ScoreMergeType merge_type() const noexcept { return merge_type_; }

 private:
  // 0..excl_-1 - included queries
  // excl_..queries.end() - excluded queries
  queries_t queries_;
  // index of the first excluded query
  size_t excl_;
  ScoreMergeType merge_type_{ScoreMergeType::kSum};
};

// Represent a set of queries joint by "And"
class AndQuery : public BooleanQuery {
 public:
  doc_iterator::ptr execute(const ExecutionContext& ctx, iterator begin,
                            iterator end) const final {
    return ::make_conjunction(ctx, merge_type(), begin, end);
  }
};

// Represent a set of queries joint by "Or"
class OrQuery : public BooleanQuery {
 public:
  doc_iterator::ptr execute(const ExecutionContext& ctx, iterator begin,
                            iterator end) const final {
    return ::make_disjunction(ctx, merge_type(), begin, end);
  }
};

// Represent a set of queries joint by "Or" with the specified
// minimum number of clauses that should satisfy criteria
class MinMatchQuery : public BooleanQuery {
 public:
  explicit MinMatchQuery(size_t min_match_count) noexcept
    : min_match_count_{min_match_count} {
    IRS_ASSERT(min_match_count_ > 1);
  }

  doc_iterator::ptr execute(const ExecutionContext& ctx, iterator begin,
                            iterator end) const final {
    IRS_ASSERT(std::distance(begin, end) >= 0);
    const size_t size = size_t(std::distance(begin, end));

    // 1 <= min_match_count
    size_t min_match_count = std::max(size_t(1), min_match_count_);

    // check the size before the execution
    if (0 == size || min_match_count > size) {
      // empty or unreachable search criteria
      return doc_iterator::empty();
    } else if (min_match_count == size) {
      // pure conjunction
      return ::make_conjunction(ctx, merge_type(), begin, end);
    }

    // min_match_count <= size
    min_match_count = std::min(size, min_match_count);

    auto itrs = MakeScoreAdapters<false>(ctx, begin, end);
    if (itrs.empty()) {
      return irs::doc_iterator::empty();
    }

    return ResoveMergeType(
      merge_type(), ctx.scorers.buckets().size(),
      [&]<typename A>(A&& aggregator) -> doc_iterator::ptr {
        // FIXME(gnusi): use FAST version
        using disjunction_t = min_match_iterator<doc_iterator::ptr, A>;

        return MakeWeakDisjunction<disjunction_t, A>(
          ctx.wand, std::move(itrs), min_match_count, std::move(aggregator));
      });
  }

 private:
  size_t min_match_count_;
};

size_t boolean_filter::hash() const noexcept {
  size_t seed = 0;

  HashCombine<size_t>(seed, filter::hash());
  std::for_each(
    filters_.begin(), filters_.end(),
    [&seed](const filter::ptr& f) { HashCombine<size_t>(seed, f->hash()); });

  return seed;
}

bool boolean_filter::equals(const filter& rhs) const noexcept {
  if (!filter::equals(rhs)) {
    return false;
  }
  const auto& typed_rhs = DownCast<boolean_filter>(rhs);
  return filters_.size() == typed_rhs.size() &&
         std::equal(begin(), end(), typed_rhs.begin(),
                    [](const filter::ptr& lhs, const filter::ptr& rhs) {
                      return *lhs == *rhs;
                    });
}

filter::prepared::ptr boolean_filter::prepare(const PrepareContext& ctx) const {
  const auto size = filters_.size();

  if (IRS_UNLIKELY(size == 0)) {
    return prepared::empty();
  }

  if (size == 1) {
    auto* filter = filters_.front().get();
    IRS_ASSERT(filter);

    // FIXME(gnusi): let Not handle everything?
    if (filter->type() != irs::type<irs::Not>::id()) {
      return filter->prepare(ctx.Boost(boost()));
    }
  }

  // determine incl/excl parts
  std::vector<const filter*> incl;
  std::vector<const filter*> excl;

  irs::filter::ptr all_docs_zero_boost;
  irs::filter::ptr all_docs_no_boost;

  group_filters(all_docs_zero_boost, incl, excl);

  if (incl.empty() && !excl.empty()) {
    // single negative query case
    all_docs_no_boost = MakeAllDocsFilter(kNoBoost);
    incl.push_back(all_docs_no_boost.get());
  }

  return PrepareBoolean(incl, excl, ctx);
}

void boolean_filter::group_filters(filter::ptr& all_docs_zero_boost,
                                   std::vector<const filter*>& incl,
                                   std::vector<const filter*>& excl) const {
  incl.reserve(size() / 2);
  excl.reserve(incl.capacity());

  const irs::filter* empty_filter{nullptr};
  const auto is_or = type() == irs::type<Or>::id();
  for (auto begin = this->begin(), end = this->end(); begin != end; ++begin) {
    if (irs::type<irs::empty>::id() == (*begin)->type()) {
      empty_filter = begin->get();
      continue;
    }
    if (irs::type<Not>::id() == (*begin)->type()) {
      const auto res = optimize_not(DownCast<Not>(**begin));

      if (!res.first) {
        continue;
      }

      if (res.second) {
        if (!all_docs_zero_boost) {
          all_docs_zero_boost = MakeAllDocsFilter(0.f);
        }

        if (*all_docs_zero_boost == *res.first) {
          // not all -> empty result
          incl.clear();
          return;
        }
        excl.push_back(res.first);
        if (is_or) {
          // FIXME: this should have same boost as Not filter.
          // But for now we do not boost negation.
          incl.push_back(all_docs_zero_boost.get());
        }
      } else {
        incl.push_back(res.first);
      }
    } else {
      incl.push_back(begin->get());
    }
  }
  if (empty_filter != nullptr) {
    incl.push_back(empty_filter);
  }
}

filter::prepared::ptr And::PrepareBoolean(std::vector<const filter*>& incl,
                                          std::vector<const filter*>& excl,
                                          const PrepareContext& ctx) const {
  // optimization step
  //  if include group empty itself or has 'empty' -> this whole conjunction is
  //  empty
  if (incl.empty() || incl.back()->type() == irs::type<irs::empty>::id()) {
    return prepared::empty();
  }

  PrepareContext sub_ctx = ctx;

  // single node case
  if (1 == incl.size() && excl.empty()) {
    sub_ctx.boost *= boost();
    return incl.front()->prepare(sub_ctx);
  }

  auto cumulative_all = MakeAllDocsFilter(kNoBoost);
  score_t all_boost{0};
  size_t all_count{0};
  for (auto filter : incl) {
    if (*filter == *cumulative_all) {
      all_count++;
      all_boost += filter->boost();
    }
  }
  if (all_count != 0) {
    const auto non_all_count = incl.size() - all_count;
    auto it = std::remove_if(incl.begin(), incl.end(),
                             [&cumulative_all](const irs::filter* filter) {
                               return *cumulative_all == *filter;
                             });
    incl.erase(it, incl.end());
    // Here And differs from Or. Last 'All' should be left in include group only
    // if there is more than one filter of other type. Otherwise this another
    // filter could be container for boost from 'all' filters
    if (1 == non_all_count) {
      // let this last filter hold boost from all removed ones
      // so we aggregate in external boost values from removed all filters
      // If we will not optimize resulting boost will be:
      //   boost * OR_BOOST * ALL_BOOST + boost * OR_BOOST * LEFT_BOOST
      // We could adjust only 'boost' so we recalculate it as
      // new_boost =  ( boost * OR_BOOST * ALL_BOOST + boost * OR_BOOST *
      // LEFT_BOOST) / (OR_BOOST * LEFT_BOOST) so when filter will be executed
      // resulting boost will be: new_boost * OR_BOOST * LEFT_BOOST. If we
      // substitute new_boost back we will get ( boost * OR_BOOST * ALL_BOOST +
      // boost * OR_BOOST * LEFT_BOOST) - original non-optimized boost value
      auto left_boost = (*incl.begin())->boost();
      if (boost() != 0 && left_boost != 0 && !sub_ctx.scorers.empty()) {
        sub_ctx.boost = (sub_ctx.boost * boost() * all_boost +
                         sub_ctx.boost * boost() * left_boost) /
                        (left_boost * boost());
      } else {
        sub_ctx.boost = 0;
      }
    } else {
      // create new 'all' with boost from all removed
      cumulative_all->boost(all_boost);
      incl.push_back(cumulative_all.get());
    }
  }
  sub_ctx.boost *= this->boost();
  if (1 == incl.size() && excl.empty()) {
    // single node case
    return incl.front()->prepare(sub_ctx);
  }
  auto q = memory::make_tracked<AndQuery>(sub_ctx.memory);
  q->prepare(sub_ctx, merge_type(), incl, excl);
  return q;
}

filter::prepared::ptr Or::prepare(const PrepareContext& ctx) const {
  if (0 == min_match_count_) {  // only explicit 0 min match counts!
    // all conditions are satisfied
    return MakeAllDocsFilter(kNoBoost)->prepare(ctx.Boost(boost()));
  }

  return boolean_filter::prepare(ctx);
}

filter::prepared::ptr Or::PrepareBoolean(std::vector<const filter*>& incl,
                                         std::vector<const filter*>& excl,
                                         const PrepareContext& ctx) const {
  const PrepareContext sub_ctx = ctx.Boost(boost());

  if (0 == min_match_count_) {  // only explicit 0 min match counts!
    // all conditions are satisfied
    return MakeAllDocsFilter(kNoBoost)->prepare(sub_ctx);
  }

  if (!incl.empty() && incl.back()->type() == irs::type<irs::empty>::id()) {
    incl.pop_back();
  }

  if (incl.empty()) {
    return prepared::empty();
  }

  // single node case
  if (1 == incl.size() && excl.empty()) {
    return incl.front()->prepare(sub_ctx);
  }

  auto cumulative_all = MakeAllDocsFilter(kNoBoost);
  size_t optimized_match_count = 0;
  // Optimization steps

  score_t all_boost{0};
  size_t all_count{0};
  const irs::filter* incl_all{nullptr};
  for (auto filter : incl) {
    if (*filter == *cumulative_all) {
      all_count++;
      all_boost += filter->boost();
      incl_all = filter;
    }
  }
  if (all_count != 0) {
    if (sub_ctx.scorers.empty() && incl.size() > 1 &&
        min_match_count_ <= all_count) {
      // if we have at least one all in include group - all other filters are
      // not necessary in case there is no scoring and 'all' count satisfies
      // min_match
      IRS_ASSERT(incl_all != nullptr);
      incl.resize(1);
      incl.front() = incl_all;
      optimized_match_count = all_count - 1;
    } else {
      // Here Or differs from And. Last All should be left in include group
      auto it = std::remove_if(incl.begin(), incl.end(),
                               [&cumulative_all](const irs::filter* filter) {
                                 return *cumulative_all == *filter;
                               });
      incl.erase(it, incl.end());
      // create new 'all' with boost from all removed
      cumulative_all->boost(all_boost);
      incl.push_back(cumulative_all.get());
      optimized_match_count = all_count - 1;
    }
  }
  // check strictly less to not roll back to 0 min_match (we`ve handled this
  // case above!) single 'all' left -> it could contain boost we want to
  // preserve
  const auto adjusted_min_match = (optimized_match_count < min_match_count_)
                                    ? min_match_count_ - optimized_match_count
                                    : 1;

  if (adjusted_min_match > incl.size()) {
    // can't satisfy 'min_match_count' conditions
    // having only 'incl.size()' queries
    return prepared::empty();
  }

  if (1 == incl.size() && excl.empty()) {
    // single node case
    return incl.front()->prepare(sub_ctx);
  }

  IRS_ASSERT(adjusted_min_match > 0 && adjusted_min_match <= incl.size());

  memory::managed_ptr<BooleanQuery> q;
  if (adjusted_min_match == incl.size()) {
    q = memory::make_tracked<AndQuery>(sub_ctx.memory);
  } else if (1 == adjusted_min_match) {
    q = memory::make_tracked<OrQuery>(sub_ctx.memory);
  } else {  // min_match_count > 1 && min_match_count < incl.size()
    q = memory::make_tracked<MinMatchQuery>(sub_ctx.memory, adjusted_min_match);
  }

  q->prepare(sub_ctx, merge_type(), incl, excl);
  return q;
}

filter::prepared::ptr Not::prepare(const PrepareContext& ctx) const {
  const auto res = optimize_not(*this);

  if (!res.first) {
    return prepared::empty();
  }

  const PrepareContext sub_ctx = ctx.Boost(boost());

  if (res.second) {
    auto all_docs = MakeAllDocsFilter(kNoBoost);
    const std::array<const irs::filter*, 1> incl{all_docs.get()};
    const std::array<const irs::filter*, 1> excl{res.first};

    auto q = memory::make_tracked<AndQuery>(sub_ctx.memory);
    q->prepare(sub_ctx, ScoreMergeType::kSum, incl, excl);
    return q;
  }

  // negation has been optimized out
  return res.first->prepare(sub_ctx);
}

size_t Not::hash() const noexcept {
  size_t seed = 0;
  HashCombine<size_t>(seed, filter::hash());
  if (filter_) {
    HashCombine<size_t>(seed, filter_->hash());
  }
  return seed;
}

bool Not::equals(const irs::filter& rhs) const noexcept {
  if (!filter::equals(rhs)) {
    return false;
  }
  const auto& typed_rhs = DownCast<Not>(rhs);
  return (!empty() && !typed_rhs.empty() && *filter_ == *typed_rhs.filter_) ||
         (empty() && typed_rhs.empty());
}

}  // namespace irs
