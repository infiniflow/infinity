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

export module infinity_core:parse_fulltext_options;

import :infinity_type;

import std;

namespace infinity {

// parse minimum_should_match
struct MinimumShouldMatchCount {
    i32 value_ = 0;
};

struct MinimumShouldMatchPercentage {
    i8 value_ = 0;
};

using SingleMinimumShouldMatchOption = std::pair<u32, std::variant<MinimumShouldMatchCount, MinimumShouldMatchPercentage>>;

export using MinimumShouldMatchOption = std::vector<SingleMinimumShouldMatchOption>;

export MinimumShouldMatchOption ParseMinimumShouldMatchOption(std::string_view input_str);

export u32 GetMinimumShouldMatchParameter(const MinimumShouldMatchOption &option_vec, u32 leaf_count);

export enum class FulltextSimilarity {
    kBM25,
    kBoolean,
};

export struct BM25Params {
    float k1 = 1.2F;
    float b = 0.75F;
    float delta_term = 0.0F;
    float delta_phrase = 0.0F;
};

export struct RankFeatureOption {
    std::string field_;
    std::string feature_;
    float boost_;
};

export using RankFeaturesOption = std::vector<RankFeatureOption>;

export RankFeaturesOption ParseRankFeaturesOption(std::string_view input_str);

} // namespace infinity
