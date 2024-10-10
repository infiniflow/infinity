// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module;

export module parse_fulltext_options;

import stl;

namespace infinity {

// parse minimum_should_match
struct MinimumShouldMatchCount {
    i32 value_ = 0;
};

struct MinimumShouldMatchPercentage {
    i8 value_ = 0;
};

using SingleMinimumShouldMatchOption = Pair<u32, std::variant<MinimumShouldMatchCount, MinimumShouldMatchPercentage>>;

export using MinimumShouldMatchOption = Vector<SingleMinimumShouldMatchOption>;

export MinimumShouldMatchOption ParseMinimumShouldMatchOption(std::string_view input_str);

export u32 GetMinimumShouldMatchParameter(const MinimumShouldMatchOption &option_vec, u32 leaf_count);

} // namespace infinity
