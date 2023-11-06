////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017 ArangoDB GmbH, Cologne, Germany
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

#include "delimited_token_stream.hpp"

#include <string_view>

#include "utils/rst/strings/str_cat.h"

namespace {

irs::bytes_view eval_term(irs::bstring& buf, irs::bytes_view data) {
  if (!data.size() || '"' != data[0]) {
    return data;  // not a quoted term (even if quotes inside
  }

  buf.clear();

  bool escaped = false;
  size_t start = 1;

  for (size_t i = 1, count = data.size(); i < count; ++i) {
    if ('"' == data[i]) {
      if (escaped && start == i) {  // an escaped quote
        escaped = false;

        continue;
      }

      if (escaped) {
        break;  // mismatched quote
      }

      buf.append(&data[start], i - start);
      escaped = true;
      start = i + 1;
    }
  }

  return start != 1 && start == data.size()
           ? irs::bytes_view(buf)
           : data;  // return identity for mismatched quotes
}

size_t find_delimiter(irs::bytes_view data, irs::bytes_view delim) {
  if (irs::IsNull(delim)) {
    return data.size();
  }

  bool quoted = false;

  for (size_t i = 0, count = data.size(); i < count; ++i) {
    if (quoted) {
      if ('"' == data[i]) {
        quoted = false;
      }

      continue;
    }

    if (data.size() - i < delim.size()) {
      break;  // no more delimiters in data
    }

    if (0 == memcmp(data.data() + i, delim.data(), delim.size()) &&
        (i || delim.size())) {  // do not match empty delim at data start
      return i;  // delimiter match takes precedence over '"' match
    }

    if ('"' == data[i]) {
      quoted = true;
    }
  }

  return data.size();
}

}  // namespace

namespace irs {
namespace analysis {

delimited_token_stream::delimited_token_stream(std::string_view delimiter)
  : delim_(ViewCast<byte_type>(delimiter)) {
  if (!irs::IsNull(delim_)) {
    delim_buf_ = delim_;  // keep a local copy of the delimiter
    delim_ = delim_buf_;  // update the delimter to point at the local copy
  }
}

void delimited_token_stream::init() {}

bool delimited_token_stream::next() {
  if (irs::IsNull(data_)) {
    return false;
  }

  auto& offset = std::get<irs::offset>(attrs_);

  auto size = find_delimiter(data_, delim_);
  auto next = std::max(size_t(1), size + delim_.size());
  auto start =
    offset.end + uint32_t(delim_.size());  // value is allowed to overflow, will
                                           // only produce invalid result
  auto end = start + size;

  if (std::numeric_limits<uint32_t>::max() < end) {
    return false;  // cannot fit the next token into offset calculation
  }

  auto& term = std::get<term_attribute>(attrs_);

  offset.start = start;
  offset.end = uint32_t(end);
  term.value = irs::IsNull(delim_)
                 ? bytes_view{data_.data(), size}
                 : eval_term(term_buf_, bytes_view(data_.data(), size));
  data_ = size >= data_.size()
            ? bytes_view{}
            : bytes_view{data_.data() + next, data_.size() - next};

  return true;
}

bool delimited_token_stream::reset(std::string_view data) {
  data_ = ViewCast<byte_type>(data);

  auto& offset = std::get<irs::offset>(attrs_);
  offset.start = 0;
  offset.end =
    0 - uint32_t(delim_.size());  // counterpart to computation in next() above

  return true;
}

}  // namespace analysis
}  // namespace irs
