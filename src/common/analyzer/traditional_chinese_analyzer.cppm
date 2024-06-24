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

export module traditional_chinese_analyzer;

import stl;
import status;
import chinese_analyzer;

class OpenCC;

namespace infinity {

export class TraditionalChineseAnalyzer : public ChineseAnalyzer {
public:
    TraditionalChineseAnalyzer(const String &path);

    TraditionalChineseAnalyzer(const TraditionalChineseAnalyzer &other);

    ~TraditionalChineseAnalyzer();

    Status Load();

protected:
    void Parse(const String &input) override;

private:
    OpenCC *opencc_{nullptr};
    bool own_opencc_{false};
};
} // namespace infinity
