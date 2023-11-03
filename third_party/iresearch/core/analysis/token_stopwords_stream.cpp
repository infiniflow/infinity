////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2019 ArangoDB GmbH, Cologne, Germany
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
/// @author Vasiliy Nabatchikov
////////////////////////////////////////////////////////////////////////////////

#include "token_stopwords_stream.hpp"

#include <cctype>  // for std::isspace(...)
#include <string_view>

#include "utils/rst/strings/str_cat.h"

namespace irs {
namespace analysis {

token_stopwords_stream::token_stopwords_stream(
  token_stopwords_stream::stopwords_set&& stopwords)
  : stopwords_(std::move(stopwords)), term_eof_(true) {}

void token_stopwords_stream::init() {}

bool token_stopwords_stream::next() {
  if (term_eof_) {
    return false;
  }

  term_eof_ = true;

  return true;
}

bool token_stopwords_stream::reset(std::string_view data) {
  auto& offset = std::get<irs::offset>(attrs_);
  offset.start = 0;
  offset.end = uint32_t(data.size());
  auto& term = std::get<term_attribute>(attrs_);
  term.value = irs::ViewCast<irs::byte_type>(data);
  term_eof_ = stopwords_.contains(data);
  return true;
}

}  // namespace analysis
}  // namespace irs
