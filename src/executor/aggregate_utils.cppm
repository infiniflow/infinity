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

export module infinity_core:aggregate_utils;

import std.compat;

import third_party;

import internal_types;
import data_type;

namespace infinity {

class ColumnVector;
// Type aliases for hash tables used in GROUP BY aggregation
//
//                                           Outer key                      Value
//                               (serialized groupby column values)     (block_id -> row ids)
export using GroupByHashTable = FlatHashMap<std::string, FlatHashMap<size_t, std::vector<size_t>>>;

//                                           Outer key                      Value
//                               (serialized groupby column values)     (block_id, row_id)
export using MergeGroupByHashTable = FlatHashMap<std::string, std::pair<size_t, size_t>>;

export void BuildHashKey(const std::vector<std::shared_ptr<ColumnVector>> &columns,
                         size_t row_id,
                         const std::vector<std::shared_ptr<DataType>> &types,
                         std::string &hash_key);

export size_t CalculateHashKeySize(const std::vector<std::shared_ptr<DataType>> &types);
} // namespace infinity
