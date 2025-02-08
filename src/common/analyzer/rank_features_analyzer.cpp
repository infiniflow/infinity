// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include <string>
module rank_features_analyzer;
import stl;
import term;
import analyzer;
import smallfloat;
import third_party;

namespace infinity {

int RankFeaturesAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    nlohmann::json line_json = nlohmann::json::parse(input.text_);
    u32 offset = 0;
    for (const auto &element : line_json) {
        if (element.is_object()) {
            for (auto it = element.begin(); it != element.end(); ++it) {
                std::string key = it.key();
                float value = it.value();
                u16 weight = SmallFloat::Float122ToUInt16(value);
                func(data, key.data(), key.size(), offset++, 0, false, weight);
            }
        }
    }

    return 0;
}

} // namespace infinity
