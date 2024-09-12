// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module user_defined_analyzer;
#if 0
#include "Python.h"
import stl;
import term;
import stemmer;
import analyzer;
import tokenizer;
import third_party;
import status;

namespace infinity {
export class UserDefinedAnalyzer : public Analyzer {
public:
    UserDefinedAnalyzer(const String &analyzer_path) : analyzer_path_(analyzer_path) {}

    ~UserDefinedAnalyzer() = default;

    Tuple<Vector<String>, Status> Analyze(const String &text);

protected:
private:
    const String analyzer_path_;
};
} // namespace infinity
#endif