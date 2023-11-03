#include "jieba_analyzer.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

#include "utils/hash_utils.hpp"
#include "utils/rst/strings/str_cat.h"
#include "utils/vpack_utils.hpp"
#include "velocypack/Builder.h"
#include "velocypack/Parser.h"
#include "velocypack/Slice.h"
#include "velocypack/velocypack-aliases.h"

static const std::string DICT_PATH = "dict/jieba.dict.utf8";
static const std::string HMM_PATH = "dict/hmm_model.utf8";
static const std::string USER_DICT_PATH = "dict/user.dict.utf8";
static const std::string IDF_PATH = "dict/idf.utf8";
static const std::string STOP_WORD_PATH = "dict/stop_words.utf8";

static const std::string PATH_KEY = "dictpath";

namespace {

namespace fs = std::filesystem;
using namespace irs;

bool parse_vpack_options(const VPackSlice slice,
                         irs::analysis::jieba_analyzer::options_t& options) {
  if (!slice.isObject()) {
    IRS_LOG_ERROR("Slice for jieba_analyzer is not an object");
    return false;
  }
  if (auto path_slice = slice.get(PATH_KEY); !path_slice.isNone()) {
    if (!path_slice.isString()) {
      IRS_LOG_WARN(rst::StrCat({"Invalid type '", PATH_KEY,
                                "' (string expected) for jieba_analyzer from "
                                "VPack arguments"}));
      return false;
    }

    auto path_str = path_slice.stringView();
    fs::path root(path_str);
    fs::path dict_path(root / DICT_PATH);
    fs::path hmm_path(root / HMM_PATH);
    fs::path userdict_path(root / USER_DICT_PATH);
    fs::path idf_path(root / IDF_PATH);
    fs::path stopwords_path(root / STOP_WORD_PATH);

    if (!fs::exists(dict_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", DICT_PATH,
                                "' dict for jieba_analyzer does not exist"}));
      return false;
    }
    if (!fs::exists(hmm_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", HMM_PATH,
                                "' hmm for jieba_analyzer does not exist"}));
      return false;
    }
    if (!fs::exists(userdict_path)) {
      IRS_LOG_WARN(
        rst::StrCat({"Invalid jieba config '", USER_DICT_PATH,
                     "' user_dict for jieba_analyzer does not exist"}));
      return false;
    }
    if (!fs::exists(idf_path)) {
      IRS_LOG_WARN(rst::StrCat({"Invalid jieba config '", IDF_PATH,
                                "' idf for jieba_analyzer does not exist"}));
      return false;
    }
    if (!fs::exists(stopwords_path)) {
      IRS_LOG_WARN(
        rst::StrCat({"Invalid jieba config '", STOP_WORD_PATH,
                     "' stopword for jieba_analyzer does not exist"}));
      return false;
    }
    options.dict_path_ = dict_path.string();
    options.hmm_path_ = hmm_path.string();
    options.user_dict_path_ = userdict_path.string();
    options.idf_path_ = idf_path.string();
    options.stopwords_path_ = stopwords_path.string();
  }
  return true;
}

bool make_vpack_config(const analysis::jieba_analyzer::options_t& options,
                       VPackBuilder* builder) {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief args is a Vpack slice object with the following attributes:
///        "case"(string enum): modify token case
///        "break"(string enum): word breaking method
////////////////////////////////////////////////////////////////////////////////
analysis::analyzer::ptr make_vpack(const VPackSlice slice) {
  try {
    analysis::jieba_analyzer::options_t options;
    if (!parse_vpack_options(slice, options)) {
      return nullptr;
    }
    return std::make_unique<analysis::jieba_analyzer>(std::move(options));
  } catch (const VPackException& ex) {
    IRS_LOG_ERROR(rst::StrCat(
      {"Caught error '", ex.what(),
       "' while constructing jieba_analyzer from VPack arguments"}));
  } catch (...) {
    IRS_LOG_ERROR(
      "Caught error while constructing jieba_analyzer from VPack "
      "arguments");
  }
  return nullptr;
}

analysis::analyzer::ptr make_vpack(std::string_view args) {
  VPackSlice slice(reinterpret_cast<const uint8_t*>(args.data()));
  return make_vpack(slice);
}

bool normalize_vpack_config(const VPackSlice slice,
                            VPackBuilder* vpack_builder) {
  analysis::jieba_analyzer::options_t options;
  try {
    if (parse_vpack_options(slice, options)) {
      return make_vpack_config(options, vpack_builder);
    } else {
      return false;
    }
  } catch (const VPackException& ex) {
    IRS_LOG_ERROR(
      rst::StrCat({"Caught error '", ex.what(),
                   "' while normalizing jieba_analyzer from VPack arguments"}));
  } catch (...) {
    IRS_LOG_ERROR(
      "Caught error while normalizing jieba_analyzer from VPack "
      "arguments");
  }
  return false;
}

bool normalize_vpack_config(std::string_view args, std::string& config) {
  VPackSlice slice(reinterpret_cast<const uint8_t*>(args.data()));
  VPackBuilder builder;
  if (normalize_vpack_config(slice, &builder)) {
    config.assign(builder.slice().startAs<char>(), builder.slice().byteSize());
    return true;
  }
  return false;
}

analysis::analyzer::ptr make_json(std::string_view args) {
  try {
    if (IsNull(args)) {
      IRS_LOG_ERROR("Null arguments while constructing jieba_analyzer");
      return nullptr;
    }
    auto vpack = VPackParser::fromJson(args.data(), args.size());
    return make_vpack(vpack->slice());
  } catch (const VPackException& ex) {
    IRS_LOG_ERROR(
      rst::StrCat({"Caught error '", ex.what(),
                   "' while constructing jieba_analyzer from JSON"}));
  } catch (...) {
    IRS_LOG_ERROR("Caught error while constructing jieba_analyzer from JSON");
  }
  return nullptr;
}

bool normalize_json_config(std::string_view args, std::string& definition) {
  try {
    if (IsNull(args)) {
      IRS_LOG_ERROR("Null arguments while normalizing jieba_analyzer");
      return false;
    }
    auto vpack = VPackParser::fromJson(args.data(), args.size());
    VPackBuilder builder;
    if (normalize_vpack_config(vpack->slice(), &builder)) {
      definition = builder.toString();
      return !definition.empty();
    }
  } catch (const VPackException& ex) {
    IRS_LOG_ERROR(
      rst::StrCat({"Caught error '", ex.what(),
                   "' while normalizing jieba_analyzer from JSON"}));
  } catch (...) {
    IRS_LOG_ERROR("Caught error while normalizing jieba_analyzer from JSON");
  }
  return false;
}
}  // namespace

namespace irs {
namespace analysis {

REGISTER_ANALYZER_VPACK(jieba_analyzer, make_vpack, normalize_vpack_config);
REGISTER_ANALYZER_JSON(jieba_analyzer, make_json, normalize_json_config);

void jieba_analyzer::init() {}

jieba_analyzer::jieba_analyzer(jieba_analyzer::options_t&& options) {
  jieba_ = new cppjieba::Jieba(options.dict_path_, options.hmm_path_,
                               options.user_dict_path_, options.idf_path_,
                               options.stopwords_path_);
  load_stopwords_dict(options.stopwords_path_);
}

jieba_analyzer::~jieba_analyzer() {
  if (jieba_) delete jieba_;
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

}  // namespace analysis
}  // namespace irs
