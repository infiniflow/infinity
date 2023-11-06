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

#include "short_string_mixed_type.h"

namespace infinity {

bool ShortStrMixedType::Equal(const std::string &input) {
    if (length != input.length())
        return false;
    for (size_t i = 0; const auto &c : input) {
        if (this->ptr[i++] != c)
            return false;
    }
    return true;
}

} // namespace infinity