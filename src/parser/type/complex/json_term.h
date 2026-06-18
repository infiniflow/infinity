// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
#include <limits>

namespace infinity {

/// JsonTermT is an alias for std::string.
/// It represents a flattened JSON term used for JSON index.
/// Term format: {path}:{type_tag}:{encoded_value}
/// The string is kept ordered (dictionary order = value order).
/// Using std::string removes the arbitrary size limit.
using JsonTermT = std::string;

/// Returns a "min" term that sorts before all valid terms.
/// @return Empty string.
constexpr JsonTermT JsonTermTMin() { return {}; }

/// Returns a "max" term that sorts after all valid terms.
/// Uses a single 0xFF byte, which is greater than any valid JSON term
/// (valid terms are ASCII / UTF‑8, where bytes < 0xFF).
/// @return String with a single 0xFF byte.
constexpr JsonTermT JsonTermTMax() { return std::string(1, '\xFF'); }

} // namespace infinity
