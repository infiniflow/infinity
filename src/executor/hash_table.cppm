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

export module infinity_core:hash_table;

import :stl;
import :column_vector;
import internal_types;
import data_type;

namespace infinity {

class HashTableBase {
public:
    virtual ~HashTableBase() = default;
    bool Initialized() const { return !types_.empty(); }

    void Init(Vector<SharedPtr<DataType>> types);

    void GetHashKey(const Vector<SharedPtr<ColumnVector>> &columns, SizeT row_id, String &hash_key) const;

public:
    Vector<SharedPtr<DataType>> types_{};
    SizeT key_size_{};
};

export class HashTable : public HashTableBase {
public:
    void Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count);

public:
    // Key -> (block id -> row array)
    HashMap<String, HashMap<SizeT, Vector<SizeT>>> hash_table_{};
};

export class MergeHashTable : public HashTableBase {
public:
    void Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count);

    bool GetOrInsert(const Vector<SharedPtr<ColumnVector>> &columns, SizeT row_id, Pair<SizeT, SizeT> &block_row_id);

public:
    // Key -> (block id, row id)
    HashMap<String, Pair<SizeT, SizeT>> hash_table_{};
};

} // namespace infinity
