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

#include <cassert>
#include <cstdlib>

module infinity_core;

import :stl;
import :third_party;
import :logger;
import :infinity_exception;
import :rocksdb_merge_operator;

namespace infinity {

bool String2UInt64AddOperator::Merge(const rocksdb::Slice & /*key*/,
                                     const rocksdb::Slice *existing_value,
                                     const rocksdb::Slice &value,
                                     std::string *new_value,
                                     rocksdb::Logger *logger) const {
    u64 orig_value = 0;
    if (existing_value) {
        orig_value = DecodeInteger(*existing_value, logger);
    }
    u64 operand = DecodeInteger(value, logger);

    assert(new_value);
    new_value->clear();
    *new_value = std::to_string(orig_value + operand);

    return true; // Return true always since corruption will be treated as 0
}

u64 String2UInt64AddOperator::DecodeInteger(const rocksdb::Slice &value, rocksdb::Logger *logger) const {
    String value_str = value.ToString();
    return std::stoull(value_str);
}

} // namespace infinity