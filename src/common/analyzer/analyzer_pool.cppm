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

export module analyzer_pool;

import stl;
import singleton;
import third_party;
import analyzer;
import status;

namespace infinity {

export class AnalyzerPool : public Singleton<AnalyzerPool> {
public:
    using CacheType = FlatHashMap<std::string_view, UniquePtr<Analyzer>>;

    Tuple<UniquePtr<Analyzer>, Status> GetAnalyzer(const std::string_view &name);

    void Set(const std::string_view &name);

public:
    static constexpr std::string_view CHINESE = "chinese";
    static constexpr std::string_view TRADITIONALCHINESE = "traditional";
    static constexpr std::string_view JAPANESE = "japanese";
    static constexpr std::string_view KOREAN = "korean";
    static constexpr std::string_view STANDARD = "standard";
    static constexpr std::string_view NGRAM = "ngram";

private:
    CacheType cache_{};
};

} // namespace infinity