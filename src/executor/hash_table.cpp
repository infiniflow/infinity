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
import parser;
import column_vector;

import infinity_exception;

module hash_table;

#if 0
namespace infinity {

void HashTable::Init(const Vector<DataType> &types) {
    types_ = types;
    SizeT type_count = types.size();
    for (SizeT idx = 0; idx < type_count; ++idx) {
        const DataType &data_type = types[idx];
        switch (data_type.type()) {
            case kBoolean:
            case kTinyInt:
            case kSmallInt:
            case kInteger:
            case kBigInt:
            case kHugeInt:
            case kFloat:
            case kDouble:
            case kDecimal:
            case kVarchar:
            case kDate:
            case kTime:
            case kDateTime:
            case kMixed: {
                break; // All these type can be hashed.
            }
            case kTimestamp:
            case kInterval:
            case kArray:
            case kTuple:
            case kPoint:
            case kLine:
            case kLineSeg:
            case kBox:
//            case kPath:
//            case kPolygon:
            case kCircle:
//            case kBitmap:
            case kUuid:
//            case kBlob:
            case kEmbedding:
            case kRowID:
            case kNull:
            case kMissing:
            case kInvalid: {
                Error<ExecutorException>("Attempt to construct hash key for type: " + data_type.ToString());
            }
        }

        SizeT type_size = data_type.Size();
        key_size_ += type_size;
    }

    // Key layout: col1\0col2\0col3\0.
    key_size_ += type_count;
}

void HashTable::Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count) {
    UniquePtr<char[]> hash_key = MakeUnique<char[]>(key_size_);
    SizeT column_count = columns.size();
    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
        std::memset(hash_key.get(), 0, key_size_);
        SizeT offset = 0;

        for (SizeT column_id = 0; column_id < column_count; ++column_id) {
            char *target_ptr = hash_key.get() + offset;
            if (!columns[column_id]->nulls_ptr_->IsTrue(row_id)) {
                *(target_ptr) = '\0';
                offset += 2;
                continue;
            }

            DataType &data_type = types_[column_id];
            if (data_type.type() == kMixed) {
                // Only float/boolean/integer/string can be built as hash key. Array/Tuple will be treated as null
                Error<ExecutorException>("Attempt to construct hash key for heterogeneous type");
            }

            if (data_type.type() == kVarchar) {
                VarcharT *vchar_ptr = &((VarcharT *)(columns[column_id]->data_ptr_))[row_id];
                if (vchar_ptr->IsInlined()) {
                    std::memcpy(target_ptr, vchar_ptr->prefix, vchar_ptr->length);
                } else {
                    std::memcpy(target_ptr, vchar_ptr->ptr, vchar_ptr->length);
                }
                offset += (vchar_ptr->length + 1);
            } else {
                SizeT type_size = types_[column_id].Size();
                std::memcpy(target_ptr, columns[column_id]->data_ptr_ + type_size * row_id, type_size);
                offset += (type_size + 1);
            }
        }

        String key(hash_key.get(), key_size_);
        hash_table_[key][block_id].emplace_back(row_id);
    }
}

} // namespace infinity
#endif