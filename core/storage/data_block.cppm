//
// Created by jinhai on 23-10-16.
//
module;

import stl;
import default_values;
import selection;
import parser;
import column_vector;
import value;
import infinity_assert;
import infinity_exception;

export module data_block;


namespace infinity {

// Intermediate data structure transferred between operator.
export struct DataBlock {

public:
    static inline SharedPtr<DataBlock> Make() { return MakeShared<DataBlock>(); }

public:
    DataBlock() = default;

    void Init(const DataBlock *input, const SharedPtr<Selection> &input_select);

    void Init(const SharedPtr<DataBlock> &input, const SharedPtr<Selection> &input_select);

    void Init(const SharedPtr<DataBlock> &input, SizeT start_idx, SizeT end_idx);

    void Init(const Vector<SharedPtr<DataType>> &types, SizeT capacity = DEFAULT_VECTOR_SIZE);

    void Init(const Vector<SharedPtr<ColumnVector>> &column_vectors);

    void UnInit();

    [[nodiscard]] inline bool Initialized() const { return initialized; }

    // Reset to just initialized state.
    void Reset();

    [[nodiscard]] Value GetValue(SizeT column_index, SizeT row_index) const;

    void SetValue(SizeT column_index, SizeT row_index, const Value &val);

    void AppendValue(SizeT column_index, const Value &value);

    void AppendValueByPtr(SizeT column_index, const ptr_t value_ptr);

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

    [[nodiscard]] inline i16 row_count() const {
        if (!finalized) {
            if (row_count_ == 0) {
                return 0;
            }
            Error<StorageException>("Not finalized data block", __FILE_NAME__, __LINE__);
        }
        return row_count_;
    }

    [[nodiscard]] inline SizeT capacity() const { return capacity_; }

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
    SizeT row_count_{0};
    SizeT column_count_{0};
    SizeT capacity_{0};
    bool initialized = false;
    bool finalized = false;
};
} // namespace infinity

