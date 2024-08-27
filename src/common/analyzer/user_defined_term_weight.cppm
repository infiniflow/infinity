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

export module user_defined_term_weight;
#if 0
import stl;
import term;
import stemmer;
import analyzer;
import tokenizer;
import third_party;
import status;

namespace infinity {
export class UserDefinedTermWeight {
public:
    UserDefinedTermWeight(const String &tw_path) : tw_path_(tw_path) {}

    ~UserDefinedTermWeight() = default;

    Tuple<HashMap<String, double>, Status> Run(const Vector<String> &text);

protected:
private:
    const String tw_path_;


};
} // namespace infinityz
#endif
