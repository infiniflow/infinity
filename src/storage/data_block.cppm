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

export module infinity_core:data_block;

import :default_values;
import :selection;
import :column_vector;
import :value;
import :infinity_exception;
import :logger;

import internal_types;
import data_type;

namespace infinity {

// Intermediate data structure transferred between operator.
export struct DataBlock {

public:
    static inline std::shared_ptr<DataBlock> Make() { return std::make_shared<DataBlock>(); }
    static inline std::unique_ptr<DataBlock> MakeUniquePtr() { return std::make_unique<DataBlock>(); }

public:
    DataBlock() = default;

    bool AppendColumns(const DataBlock &other, const std::vector<size_t> &column_idxes);

    std::unique_ptr<DataBlock> Clone() const;

    void Init(const DataBlock *input, const std::shared_ptr<Selection> &input_select);

    static std::shared_ptr<DataBlock> MoveFrom(std::shared_ptr<DataBlock> &input);

    void Init(const std::vector<std::shared_ptr<DataType>> &types, size_t capacity = DEFAULT_VECTOR_SIZE);

    void Init(const std::vector<std::shared_ptr<ColumnVector>> &column_vectors);

    // void UnInit();

    [[nodiscard]] bool Initialized() const { return initialized; }

    // Reset to just initialized state.
    void Reset();

    // TODO: May cause error when capacity is larger than the originally allocated size
    void Reset(size_t capacity);

    [[nodiscard]] Value GetValue(size_t column_index, size_t row_index) const;

    void SetValue(size_t column_index, size_t row_index, const Value &val);

    void AppendValue(size_t column_index, const Value &value);

    void AppendValueByPtr(size_t column_index, const char *value_ptr);

    void Finalize();

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] std::string ToBriefString() const;

    [[nodiscard]] bool Finalized() const { return finalized; }

    void FillRowIDVector(std::shared_ptr<std::vector<RowID>> &row_ids, u32 block_id) const;

    void UnionWith(const std::shared_ptr<DataBlock> &other);

    void AppendWith(const std::shared_ptr<DataBlock> &other);

    void AppendWith(const DataBlock *other);

    void AppendWith(const DataBlock *other, size_t from, size_t count);

    void InsertVector(const std::shared_ptr<ColumnVector> &vector, size_t index);

public:
    [[nodiscard]] size_t column_count() const { return column_count_; }

    [[nodiscard]] u16 row_count() const { return row_count_; }

    [[nodiscard]] std::vector<std::shared_ptr<DataType>> types() const {
        std::vector<std::shared_ptr<DataType>> types;

        types.reserve(column_count());
        for (size_t colum_idx = 0; colum_idx < column_vectors.size(); ++colum_idx) {
            types.push_back(column_vectors[colum_idx]->data_type());
        }
        return types;
    }

    [[nodiscard]] size_t capacity() const { return capacity_; }
    [[nodiscard]] size_t available_capacity() const { return capacity_ - row_count_; }

    bool operator==(const DataBlock &other) const;
    bool operator!=(const DataBlock &other) const { return !(*this == other); }

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    i32 GetSizeInBytes() const;
    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static std::shared_ptr<DataBlock> ReadAdv(const char *&ptr, i32 maxbytes);

    std::vector<std::shared_ptr<ColumnVector>> column_vectors;

private:
    u16 row_count_{0};
    size_t column_count_{0};
    size_t capacity_{0};
    bool initialized = false;
    bool finalized = false;
};
} // namespace infinity
