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

import stl;
import default_values;
import selection;
import parser;
import column_vector;
import value;

import infinity_exception;

export module data_block;

namespace infinity {

// Intermediate data structure transferred between operator.
export struct DataBlock {

public:
    static inline SharedPtr<DataBlock> Make() { return MakeShared<DataBlock>(); }
    static inline UniquePtr<DataBlock> MakeUniquePtr() { return MakeUnique<DataBlock>(); }

public:
    DataBlock() = default;

    void Init(const DataBlock *input, const SharedPtr<Selection> &input_select);

    void Init(const SharedPtr<DataBlock> &input, const SharedPtr<Selection> &input_select);

    void Init(const SharedPtr<DataBlock> &input, SizeT start_idx, SizeT end_idx);

    static SharedPtr<DataBlock> MoveFrom(SharedPtr<DataBlock> &input);

    void Init(const Vector<SharedPtr<DataType>> &types, SizeT capacity = DEFAULT_VECTOR_SIZE);

    void Init(const Vector<SharedPtr<ColumnVector>> &column_vectors);

    void UnInit();

    [[nodiscard]] inline bool Initialized() const { return initialized; }

    // Reset to just initialized state.
    void Reset();

    [[nodiscard]] Value GetValue(SizeT column_index, SizeT row_index) const;

    void SetValue(SizeT column_index, SizeT row_index, const Value &val);

    void AppendValue(SizeT column_index, const Value &value);

    void AppendValueByPtr(SizeT column_index, const_ptr_t value_ptr);

    void Finalize();

    [[nodiscard]] String ToString() const;

    [[nodiscard]] bool Finalized() const { return finalized; }

    void FillRowIDVector(SharedPtr<Vector<RowID>> &row_ids, u32 block_id) const;

    void UnionWith(const SharedPtr<DataBlock> &other);

    void AppendWith(const SharedPtr<DataBlock> &other);

    void AppendWith(const DataBlock *other);

    void AppendWith(const SharedPtr<DataBlock> &other, SizeT from, SizeT count);

public:
    [[nodiscard]] inline SizeT column_count() const { return column_count_; }

    [[nodiscard]] inline u16 row_count() const {
        if (!finalized) {
            if (row_count_ == 0) {
                return 0;
            }
            Error<StorageException>("Not finalized data block");
        }
        return row_count_;
    }

    [[nodiscard]] inline SizeT capacity() const { return capacity_; }
    [[nodiscard]] inline SizeT available_capacity() const { return capacity_ - row_count_; }

    bool operator==(const DataBlock &other) const;
    bool operator!=(const DataBlock &other) const { return !(*this == other); }

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    i32 GetSizeInBytes() const;
    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static SharedPtr<DataBlock> ReadAdv(char *&ptr, i32 maxbytes);

    Vector<SharedPtr<ColumnVector>> column_vectors;

private:
    u16 row_count_{0};
    SizeT column_count_{0};
    SizeT capacity_{0};
    bool initialized = false;
    bool finalized = false;
};
} // namespace infinity
