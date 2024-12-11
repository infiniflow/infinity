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

#include <sstream>
#include <string>
module whitespace_analyzer;
import stl;
import term;
import analyzer;

namespace infinity {

int WhitespaceAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    std::istringstream is(input.text_);
    std::string t;
    u32 offset = 0;
    while (is >> t) {
        func(data, t.data(), t.size(), offset++, 0, Term::AND, 0, false);
    }
    return 0;
}

} // namespace infinity
