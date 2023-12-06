#include "jieba_analyzer.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

#include "utils/hash_utils.hpp"
#include "utils/rst/strings/str_cat.h"

static const std::string DICT_PATH = "jieba/dict/jieba.dict.utf8";
static const std::string HMM_PATH = "jieba/dict/hmm_model.utf8";
static const std::string USER_DICT_PATH = "jieba/dict/user.dict.utf8";
static const std::string IDF_PATH = "jieba/dict/idf.utf8";
static const std::string STOP_WORD_PATH = "jieba/dict/stop_words.utf8";

namespace fs = std::filesystem;

namespace irs {
namespace analysis {

jieba_analyzer::jieba_analyzer(jieba_analyzer::options_t &&options) : dict_path_(options.path_), jieba_(nullptr) {}

bool jieba_analyzer::load() {
    fs::path root(dict_path_);
    fs::path dict_path(root / DICT_PATH);
    fs::path hmm_path(root / HMM_PATH);
    fs::path userdict_path(root / USER_DICT_PATH);
    fs::path idf_path(root / IDF_PATH);
    fs::path stopwords_path(root / STOP_WORD_PATH);

    if (!fs::exists(dict_path)) {
        IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", dict_path.string(), "' dict for jieba_analyzer does not exist"}));
        return false;
    }
  if (!fs::exists(hmm_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", hmm_path.string(), "' hmm for jieba_analyzer does not exist"}));
      return false;
  }
  if (!fs::exists(userdict_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", userdict_path.string(), "' user_dict for jieba_analyzer does not exist"}));
      return false;
  }
  if (!fs::exists(idf_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", idf_path.string(), "' idf for jieba_analyzer does not exist"}));
      return false;
  }
  if (!fs::exists(stopwords_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", stopwords_path.string(), "' stopword for jieba_analyzer does not exist"}));
      return false;
  }

  try {
      jieba_ = new cppjieba::Jieba(dict_path.string(), hmm_path.string(), userdict_path.string(), idf_path.string(), stopwords_path.string());
  } catch (const std::exception &e) {
      return false;
  }
  own_jieba_ = true;
  load_stopwords_dict(stopwords_path.string());
  return true;
}

jieba_analyzer::jieba_analyzer(const jieba_analyzer &other) {
    own_jieba_ = false;
    jieba_ = other.jieba_;
}
jieba_analyzer::~jieba_analyzer() {
    if (own_jieba_ && jieba_)
        delete jieba_;
}

void jieba_analyzer::load_stopwords_dict(const std::string& stopwords_path) {
  std::ifstream ifs(stopwords_path);
  std::string line;
  while (getline(ifs, line)) {
    stopwords_.insert(line);
  }
}

bool jieba_analyzer::next() {
  if (cut_words_.size() == 0) {
    return false;
  }

  while (true) {
    if (cursor_ == cut_words_.size()) {
      return false;
    }
    if (!accept_token(cut_words_[cursor_].word)) {
      cursor_++;
      continue;
    }
    term_buf_ = cut_words_[cursor_].word;
    auto& term = std::get<term_attribute>(attrs_);
    term.value = irs::ViewCast<byte_type>(std::string_view{term_buf_});
    auto& offset = std::get<irs::offset>(attrs_);
    offset.start = cut_words_[cursor_].offset;
    offset.end = offset.start + term_buf_.size();
    cursor_++;
    return true;
  }
}

bool jieba_analyzer::reset(std::string_view data) {
  cursor_ = 0;
  auto& term = std::get<term_attribute>(attrs_);
  auto& offset = std::get<irs::offset>(attrs_);
  // reset term attribute
  term.value = {};

  // reset offset attribute
  offset.start = 0;
  offset.end = 0;

  cut_words_.clear();

  std::string str = std::string(data);
  jieba_->CutForSearch(str, cut_words_, true);
  return true;
}

} // namespace analysis
} // namespace irs
