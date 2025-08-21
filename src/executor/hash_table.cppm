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

export module infinity_core:hash_table;

import :column_vector;

import internal_types;
import data_type;

namespace infinity {

class HashTableBase {
public:
    virtual ~HashTableBase() = default;
    bool Initialized() const { return !types_.empty(); }

    void Init(std::vector<std::shared_ptr<DataType>> types);

    void GetHashKey(const std::vector<std::shared_ptr<ColumnVector>> &columns, size_t row_id, std::string &hash_key) const;

public:
    std::vector<std::shared_ptr<DataType>> types_{};
    size_t key_size_{};
};

export class HashTable : public HashTableBase {
public:
    void Append(const std::vector<std::shared_ptr<ColumnVector>> &columns, size_t block_id, size_t row_count);

public:
    // Key -> (block id -> row array)
    std::unordered_map<std::string, std::unordered_map<size_t, std::vector<size_t>>> hash_table_{};
};

export class MergeHashTable : public HashTableBase {
public:
    void Append(const std::vector<std::shared_ptr<ColumnVector>> &columns, size_t block_id, size_t row_count);

    bool GetOrInsert(const std::vector<std::shared_ptr<ColumnVector>> &columns, size_t row_id, std::pair<size_t, size_t> &block_row_id);

public:
    // Key -> (block id, row id)
    std::unordered_map<std::string, std::pair<size_t, size_t>> hash_table_{};
};

} // namespace infinity
