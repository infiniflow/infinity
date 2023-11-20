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

#pragma once

#include <functional>

#include "index/index_reader_options.hpp"
#include "index/iterators.hpp"
#include "utils/hash_utils.hpp"

namespace irs {

struct IndexReader;
struct PreparedStateVisitor;

struct PrepareContext {
  const IndexReader& index;
  IResourceManager& memory = IResourceManager::kNoop;
  const Scorers& scorers = Scorers::kUnordered;
  const attribute_provider* ctx = nullptr;
  score_t boost = kNoBoost;

  PrepareContext Boost(score_t boost) const noexcept {
    auto ctx = *this;
    ctx.boost *= boost;
    return ctx;
  }
};

struct ExecutionContext {
  const SubReader& segment;
  IResourceManager& memory = IResourceManager::kNoop;
  const Scorers& scorers = Scorers::kUnordered;
  const attribute_provider* ctx = nullptr;
  // If enabled, wand would use first scorer from scorers
  WandContext wand;
};

// Base class for all user-side filters
class filter {
 public:
  // Base class for all prepared(compiled) queries
  class prepared : public memory::Managed {
   public:
    using ptr = memory::managed_ptr<const prepared>;

    static prepared::ptr empty();

    explicit prepared(score_t boost = kNoBoost) noexcept : boost_(boost) {}

    virtual doc_iterator::ptr execute(const ExecutionContext& ctx) const = 0;

    virtual void visit(const SubReader& segment, PreparedStateVisitor& visitor,
                       score_t boost) const = 0;

    score_t boost() const noexcept { return boost_; }

   protected:
    void boost(score_t boost) noexcept { boost_ *= boost; }

   private:
    score_t boost_;
  };

  using ptr = std::unique_ptr<filter>;

  virtual ~filter() = default;

  virtual size_t hash() const noexcept {
    return std::hash<type_info::type_id>()(type());
  }

  bool operator==(const filter& rhs) const noexcept { return equals(rhs); }

  virtual void PopulateDefaultFieldRecursive(const std::string &default_field) {}

  virtual filter::prepared::ptr prepare(const PrepareContext& ctx) const = 0;

  score_t boost() const noexcept { return boost_; }

  filter& boost(score_t boost) noexcept {
    boost_ = boost;
    return *this;
  }

  virtual type_info::type_id type() const noexcept = 0;

 protected:
  virtual bool equals(const filter& rhs) const noexcept {
    return type() == rhs.type();
  }

 private:
  score_t boost_{kNoBoost};
};

// boost::hash_combine support
inline size_t hash_value(const filter& q) noexcept { return q.hash(); }

// Convenient base class filters with options
template<typename Options>
class filter_with_options : public filter {
 public:
  using options_type = Options;
  using filter_type = typename options_type::filter_type;

  const options_type& options() const noexcept { return options_; }
  options_type* mutable_options() noexcept { return &options_; }

  size_t hash() const noexcept override {
    return hash_combine(filter::hash(), options_.hash());
  }

  type_info::type_id type() const noexcept final {
    return irs::type<filter_type>::id();
  }

 protected:
  bool equals(const filter& rhs) const noexcept override {
    return filter::equals(rhs) &&
           options_ == DownCast<filter_type>(rhs).options_;
  }

 private:
  options_type options_;
};

// Convenient base class for single field filters
template<typename Options>
class filter_base : public filter_with_options<Options> {
 public:
  using options_type = typename filter_with_options<Options>::options_type;
  using filter_type = typename options_type::filter_type;

  std::string_view field() const noexcept { return field_; }
  std::string* mutable_field() noexcept { return &field_; }

  void PopulateDefaultFieldRecursive(const std::string &default_field) override {
      if (field_.empty())
          field_ = default_field;
  }

  size_t hash() const noexcept final {
    return hash_combine(hash_utils::Hash(field_),
                        filter_with_options<options_type>::hash());
  }

 protected:
  bool equals(const filter& rhs) const noexcept final {
    return filter_with_options<options_type>::equals(rhs) &&
           field_ == DownCast<filter_type>(rhs).field_;
  }

 private:
  std::string field_;
};

// Filter which returns no documents
class empty final : public filter {
 public:
  filter::prepared::ptr prepare(const PrepareContext& ctx) const final;

  type_info::type_id type() const noexcept final {
    return irs::type<empty>::id();
  }
};

struct filter_visitor;
using field_visitor =
  std::function<void(const SubReader&, const term_reader&, filter_visitor&)>;

}  // namespace irs

namespace std {

template<>
struct hash<irs::filter> {
  typedef irs::filter argument_type;
  typedef size_t result_type;

  result_type operator()(const argument_type& key) const { return key.hash(); }
};

}  // namespace std
