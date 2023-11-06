////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2021 ArangoDB GmbH, Cologne, Germany
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
/// @author Andrei Lobov
////////////////////////////////////////////////////////////////////////////////

#include "segmentation_token_stream.hpp"

#include <frozen/unordered_map.h>

#include <boost/text/case_mapping.hpp>
#include <boost/text/word_break.hpp>
#include <iostream>
#include <string_view>

#include "utils/hash_utils.hpp"
#include "utils/rst/strings/str_cat.h"
#include "utils/utf8_character_utils.hpp"

namespace {

using namespace irs;

using word_break_t =
  analysis::segmentation_token_stream::options_t::word_break_t;

template<typename Iterator>
bool accept_token(Iterator begin, Iterator end, word_break_t wb) {
  switch (wb) {
    case word_break_t::ALL:
      return true;
    case word_break_t::GRAPHIC:
      return std::find_if_not(begin, end, utf8_utils::char_is_white_space) !=
             end;
    case word_break_t::ALPHA:
      return std::find_if(begin, end, utf8_utils::char_is_alphanumeric) != end;
    default:
      IRS_ASSERT(false);
      return false;
  }
}

}  // namespace

namespace irs {
namespace analysis {

using namespace boost::text;

using data_t =
  decltype(as_graphemes(std::string_view{}.begin(), std::string_view{}.end()));
using iterator_t = decltype(next_word_break(data_t{}, data_t{}.begin()));

struct segmentation_token_stream::state_t {
  data_t data;
  iterator_t begin;
  iterator_t end;
};

void segmentation_token_stream::state_deleter_t::operator()(
  state_t* p) const noexcept {
  delete p;
}

void segmentation_token_stream::init() {}

segmentation_token_stream::segmentation_token_stream(
  segmentation_token_stream::options_t&& options)
  : state_{new state_t()}, options_{options} {}

bool segmentation_token_stream::next() {
  while (true) {
    const auto gr_begin = state_->begin;
    const auto gr_end = next_word_break(state_->data, gr_begin);

    const auto begin = gr_begin.base();
    const auto end = gr_end.base();

    const auto length =
      static_cast<size_t>(std::distance(begin.base(), end.base()));

    if (length == 0) {  // eof
      return false;
    }

    auto& offset = std::get<irs::offset>(attrs_);
    IRS_ASSERT(offset.end + length <= std::numeric_limits<uint32_t>::max());

    offset.start = offset.end;
    offset.end += static_cast<uint32_t>(length);
    state_->begin = gr_end;

    if (!accept_token(begin, end, options_.word_break)) {
      continue;
    }

    switch (auto& term = std::get<term_attribute>(attrs_);
            options_.case_convert) {
      case options_t::case_convert_t::NONE:
        IRS_ASSERT(length);
        // on *nix base returns pointer on msvc it return iterator
        term.value = {reinterpret_cast<const byte_type*>(&(*begin.base())),
                      length};
        break;
      case options_t::case_convert_t::LOWER:
        term_buf_.clear();
        to_lower(begin, begin, end, from_utf32_back_inserter(term_buf_));
        term.value = irs::ViewCast<byte_type>(std::string_view{term_buf_});
        std::cout << "term " << term_buf_ << std::endl;
        break;
      case options_t::case_convert_t::UPPER:
        term_buf_.clear();
        to_upper(begin, begin, end, from_utf32_back_inserter(term_buf_));
        term.value = irs::ViewCast<byte_type>(std::string_view{term_buf_});
        std::cout << "term " << term_buf_ << std::endl;
        break;
    }

    return true;
  }
}

bool segmentation_token_stream::reset(std::string_view data) {
  state_->data = as_graphemes(data.begin(), data.end());
  state_->begin = state_->data.begin();
  state_->end = state_->data.end();

  return true;
}

}  // namespace analysis
}  // namespace irs
