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

#include <string>
import stl;
import logical_type;
import column_vector;
import status;
import infinity_exception;
import third_party;
import internal_types;

module hash_table;

namespace infinity {


void HashTableBase::Init(Vector<SharedPtr<DataType>> types) {
    types_ = std::move(types);
    SizeT key_size = 0;
    SizeT type_count = types_.size();
    for (SizeT idx = 0; idx < type_count; ++idx) {
        const DataType &data_type = *types_[idx];
        switch (data_type.type()) {
            case LogicalType::kBoolean:
            case LogicalType::kTinyInt:
            case LogicalType::kSmallInt:
            case LogicalType::kInteger:
            case LogicalType::kBigInt:
            case LogicalType::kFloat:
            case LogicalType::kDouble:
            case LogicalType::kDate:
            case LogicalType::kTime:
            case LogicalType::kDateTime: {
                SizeT type_size = data_type.Size();
                key_size += type_size;
                break; // All these type can be hashed.
            }
            case LogicalType::kVarchar: {
                key_size = 0;
                break; // Varchar can be hashed.
            }
            default: {
                RecoverableError(Status::NotSupport(fmt::format("Attempt to construct hash key for type: {}", data_type.ToString())));
            }
        }
        if (key_size == 0) {
            break;
        }
    }

    if (key_size) {
        // Key layout: col1\0col2\0col3\0.
        key_size += type_count;
    }
    key_size_ = key_size;
}

void HashTableBase::GetHashKey(const Vector<SharedPtr<ColumnVector>> &columns, SizeT row_id, String &hash_key) const {
    SizeT column_count = columns.size();
    hash_key.clear();
    bool has_null = false;
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        if (!columns[column_id]->nulls_ptr_->IsTrue(row_id)) {
            hash_key += "\0\0";
            has_null = true;
            continue;
        }

        const DataType &data_type = *types_[column_id];

        if (data_type.type() == LogicalType::kVarchar) {
            Span<const char> text = columns[column_id]->GetVarchar(row_id);
            hash_key.append(text.begin(), text.end());
        } else {
            SizeT type_size = types_[column_id]->Size();
            Span<const char> binary(reinterpret_cast<const char *>(columns[column_id]->data() + type_size * row_id), type_size);
            hash_key.append(binary.begin(), binary.end());
        }
        hash_key += '\0';
    }
    if (!has_null && key_size_ && hash_key.size() != key_size_) {
        UnrecoverableError(fmt::format("Hash key size mismatch: {} vs {}", hash_key.size(), key_size_));
    }
}

void HashTable::Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count) {
    String hash_key;
    if (key_size_) {
        hash_key.reserve(key_size_);
    }
    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
        GetHashKey(columns, row_id, hash_key);
        hash_table_[std::move(hash_key)][block_id].emplace_back(row_id);
    }
}

void MergeHashTable::Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count) {
    String hash_key;
    if (key_size_) {
        hash_key.reserve(key_size_);
    }
    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
        GetHashKey(columns, row_id, hash_key);
        if (auto iter = hash_table_.find(hash_key); iter != hash_table_.end()) {
            UnrecoverableError("Duplicate key in merge hash table");
        } else {
            hash_table_.emplace_hint(iter, std::move(hash_key), Pair<SizeT, SizeT>(block_id, row_id));
        }
    }
}

bool MergeHashTable::GetOrInsert(const Vector<SharedPtr<ColumnVector>> &columns, SizeT row_id, Pair<SizeT, SizeT> &block_row_id) {
    String hash_key;
    if (key_size_) {
        hash_key.reserve(key_size_);
    }
    GetHashKey(columns, row_id, hash_key);
    auto iter = hash_table_.find(hash_key);
    if (iter == hash_table_.end()) {
        hash_table_.emplace_hint(iter, std::move(hash_key), block_row_id);
        return false;
    }
    block_row_id = iter->second;
    return true;
}

} // namespace infinity