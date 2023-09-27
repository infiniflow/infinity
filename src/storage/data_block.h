//
// Created by JinHai on 2022/11/12.
//

#pragma once

#include "common/types/alias/db_type.h"
#include "common/column_vector/column_vector.h"
#include "common/column_vector/selection.h"

namespace infinity {

// Intermediate data structure transferred between operator.
struct DataBlock {

public:
    static inline SharedPtr<DataBlock>
    Make() {
        return MakeShared<DataBlock>();
    }

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    int32_t
    GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char* &ptr) const;
    // Read from a serialized version
    static SharedPtr<DataBlock> ReadAdv(char* &ptr, int32_t maxbytes);

public:

    DataBlock() = default;

    void
    Init(const DataBlock* input, const SharedPtr<Selection>& input_select);

    void
    Init(const SharedPtr<DataBlock>& input, const SharedPtr<Selection>& input_select);

    void
    Init(const SharedPtr<DataBlock>& input, SizeT start_idx, SizeT end_idx);

    void
    Init(const Vector<SharedPtr<DataType>>& types, SizeT capacity = DEFAULT_VECTOR_SIZE);

    void
    Init(const Vector<SharedPtr<ColumnVector>>& column_vectors);

    void
    UnInit();

    [[nodiscard]] inline bool
    Initialized() const {
        return initialized;
    }

    // Reset to just initialized state.
    void
    Reset();

    [[nodiscard]] Value
    GetValue(SizeT column_index, SizeT row_index) const;

    void
    SetValue(SizeT column_index, SizeT row_index, const Value& val);

    void
    AppendValue(SizeT column_index, const Value& value);

    void
    Finalize();

    [[nodiscard]] String
    ToString() const;

    [[nodiscard]] bool
    Finalized() const {
        return finalized;
    }

    void
    FillRowIDVector(SharedPtr<Vector<RowID>>& row_ids, u32 block_id) const;

    void
    UnionWith(const SharedPtr<DataBlock>& other);

    void
    AppendWith(const SharedPtr<DataBlock>& other);

    void
    AppendWith(const SharedPtr<DataBlock>& other, SizeT from, SizeT count);

public:
    [[nodiscard]] inline SizeT
    column_count() const {
        return column_count_;
    }

    [[nodiscard]] inline SizeT
    row_count() const {
        if(!finalized) {
            if(row_count_ == 0)
                return 0;
            StorageError("Not finalized data block")
        }
        return row_count_;
    }

    [[nodiscard]] inline SizeT
    capacity() const {
        return capacity_;
    }

    Vector<SharedPtr<ColumnVector>> column_vectors;

private:

    SizeT row_count_{0};
    SizeT column_count_{0};
    SizeT capacity_{0};
    bool initialized = false;
    bool finalized = false;
};
}
