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

export module hash_table;

import stl;
import column_vector;
import internal_types;
import data_type;

namespace infinity {

export class HashTable {
public:
    void Init(const Vector<SharedPtr<DataType>> &types);

    void Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count);

public:
    Vector<SharedPtr<DataType>> types_{};
    SizeT key_size_{};

    // Key -> (block id -> row array)
    HashMap<String, HashMap<SizeT, Vector<SizeT>>> hash_table_{};
};

} // namespace infinity
