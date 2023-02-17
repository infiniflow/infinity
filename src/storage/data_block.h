//
// Created by JinHai on 2022/11/12.
//

#pragma once

#include "common/types/internal_types.h"
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

public:

    DataBlock() = default;

    void
    Init(const SharedPtr<DataBlock>& input, const SharedPtr<Selection>& input_select);

    void
    Init(const SharedPtr<DataBlock>& input, SizeT start_idx, SizeT end_idx);

    void
    Init(const Vector<DataType>& types, SizeT capacity = DEFAULT_VECTOR_SIZE);

    void
    Init(const Vector<SharedPtr<ColumnVector>>& column_vectors);

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

public:
    [[nodiscard]] inline SizeT
    column_count() const {
        return column_count_;
    }

    [[nodiscard]] inline SizeT
    row_count() const {
        if(!finalized) {
            if(row_count_ == 0) return 0;
            StorageError("Not finalized data block")
        }

        if(row_count_ == 0) return 0;

        return row_count_;
    }

    [[nodiscard]] inline SizeT
    capacity() const {
        return capacity_;
    }

    Vector<SharedPtr<ColumnVector>> column_vectors;

private:

    SizeT row_count_ {0};
    SizeT column_count_ {0};
    SizeT capacity_ {0};
    bool initialized = false;
    bool finalized = false;
};
}
