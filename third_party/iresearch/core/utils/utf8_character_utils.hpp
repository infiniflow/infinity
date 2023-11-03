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
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utils/utf8_character_tables.hpp"

namespace irs {
namespace utf8_utils {

// Returns true if a specified character 'c' is a whitespace according to
// unicode standard, for details see https://unicode.org/reports
constexpr bool char_is_white_space(uint32_t c) noexcept {
  return 0 != WHITE_SPACE_TABLE.count(c);
}

constexpr uint16_t char_general_category(uint32_t c) noexcept {
  const auto it = frozen::bits::lower_bound<GENERAL_CATEGORY_TABLE.size()>(
    GENERAL_CATEGORY_TABLE.begin(), c, GENERAL_CATEGORY_TABLE.key_comp());

  if (it != GENERAL_CATEGORY_TABLE.begin() && it->first != c) {
    return std::prev(it)->second;
  }
  return it->second;
}

constexpr char char_primary_category(uint32_t c) noexcept {
  return char(char_general_category(c) >> 8);
}

constexpr bool char_is_alphanumeric(uint32_t c) noexcept {
  const auto g = char_primary_category(c);

  return g == 'L' || g == 'N';
}

}  // namespace utf8_utils
}  // namespace irs
