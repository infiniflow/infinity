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

void HashTable::Init(const Vector<SharedPtr<DataType>> &types) {
    types_ = types;
    SizeT type_count = types.size();
    for (SizeT idx = 0; idx < type_count; ++idx) {
        const DataType &data_type = *types[idx];
        switch (data_type.type()) {
            case LogicalType::kBoolean:
            case LogicalType::kTinyInt:
            case LogicalType::kSmallInt:
            case LogicalType::kInteger:
            case LogicalType::kBigInt:
            case LogicalType::kHugeInt:
            case LogicalType::kFloat:
            case LogicalType::kDouble:
            case LogicalType::kDecimal:
            case LogicalType::kVarchar:
            case LogicalType::kDate:
            case LogicalType::kTime:
            case LogicalType::kDateTime:
            case LogicalType::kMixed: {
                break; // All these type can be hashed.
            }
            case LogicalType::kTimestamp:
            case LogicalType::kInterval:
            case LogicalType::kArray:
            case LogicalType::kTuple:
            case LogicalType::kPoint:
            case LogicalType::kLine:
            case LogicalType::kLineSeg:
            case LogicalType::kBox:
                //            case kPath:
                //            case kPolygon:
            case LogicalType::kCircle:
                //            case kBitmap:
            case LogicalType::kUuid:
                //            case kBlob:
            case LogicalType::kEmbedding:
            case LogicalType::kRowID:
            case LogicalType::kNull:
            case LogicalType::kMissing:
            case LogicalType::kInvalid:
            default: {
                RecoverableError(Status::NotSupport(fmt::format("Attempt to construct hash key for type: {}", data_type.ToString())));
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

            const DataType &data_type = *types_[column_id];
            if (data_type.type() == LogicalType::kMixed) {
                // Only float/boolean/integer/string can be built as hash key. Array/Tuple will be treated as null
                RecoverableError(Status::NotSupport("Attempt to construct hash key for heterogeneous type"));
            }

            if (data_type.type() == LogicalType::kVarchar) {
                VarcharT *vchar_ptr = &((VarcharT *)(columns[column_id]->data()))[row_id];
                if (vchar_ptr->IsInlined()) {
                    std::memcpy(target_ptr, vchar_ptr->short_.data_, vchar_ptr->length_);
                    offset += (vchar_ptr->length_ + 1);
                } else {
                    std::memcpy(target_ptr, vchar_ptr->vector_.prefix_, VARCHAR_PREFIX_LEN);
                    offset += (VARCHAR_PREFIX_LEN + 1);
                }
            } else {
                SizeT type_size = types_[column_id]->Size();
                std::memcpy(target_ptr, columns[column_id]->data() + type_size * row_id, type_size);
                offset += (type_size + 1);
            }
        }

        String key(hash_key.get(), key_size_);
        hash_table_[key][block_id].emplace_back(row_id);
    }
}

} // namespace infinity