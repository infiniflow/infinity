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

import stl;
import singleton;
import third_party;

export module iresearch_analyzer;

namespace infinity {

export constexpr StringView JIEBA = "jieba";
export constexpr StringView SEGMENT = "segmentation";

export class AnalyzerPool : public Singleton<AnalyzerPool> {
public:
    using CacheType = FlatHashMap<StringView, UniquePtr<IRSAnalyzer>>;

    UniquePtr<IRSAnalyzer> Get(const StringView &name);

    void Set(const StringView &name);

private:
    CacheType cache_;
};

} // namespace infinity