#pragma once
#include <parallel_hashmap/phmap.h>

#include <cppjieba/Jieba.hpp>

#include "analyzers.hpp"
#include "shared.hpp"
#include "token_attributes.hpp"
#include "token_stream.hpp"
#include "utils/attribute_helper.hpp"

namespace irs {
namespace analysis {
class jieba_analyzer final : public TypedAnalyzer<jieba_analyzer>,
                             private util::noncopyable {
 public:
  static constexpr std::string_view type_name() noexcept { return "jieba"; }
  static void init() {} // for triggering registration in a static build

  struct options_t {
      std::string path_;
  };

  attribute* get_mutable(irs::type_info::type_id type) noexcept final {
    return irs::get_mutable(attrs_, type);
  }
  explicit jieba_analyzer(options_t &&opts);
  jieba_analyzer(const jieba_analyzer &other);
  ~jieba_analyzer();
  bool load();
  bool next() final;
  bool reset(std::string_view data) final;

 private:
  using stopwords_set = phmap::flat_hash_set<std::string>;

  void load_stopwords_dict(const std::string& stopwords_path);
  bool accept_token(const std::string& term) {
    return !stopwords_.contains(term);
  }
  using attributes = std::tuple<increment, offset, term_attribute>;
  cppjieba::Jieba* jieba_ = nullptr;
  std::string dict_path_;
  bool own_jieba_;
  size_t cursor_;
  std::vector<cppjieba::Word> cut_words_;
  std::string term_buf_;
  attributes attrs_;
  stopwords_set stopwords_;
};

}  // namespace analysis
}  // namespace irs
