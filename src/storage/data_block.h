//
// Created by JinHai on 2022/11/12.
//

#include "common/types/internal_types.h"
#include "common/column_vector/column_vector.h"

namespace infinity {

struct DataBlock {

    DataBlock() = default;

    void
    Init(const std::vector<DataType>& types);

    // Reset to just initialized state.
    void
    Reset();

    [[nodiscard]] Value
    GetValue(size_t column_index, size_t row_index) const;

//    void
//    SetValue(size_t column_index, size_t row_index, const Value& val);

    void
    AppendValue(size_t column_index, const Value& value);

public:
    [[nodiscard]] inline size_t
    column_count() const {
        return column_count_;
    }

    [[nodiscard]] inline size_t
    row_count() const {
        return row_count_;
    }

    [[nodiscard]] inline size_t
    row_capacity() const {
        return row_capacity_;
    }


    Vector<ColumnVector> columns;

private:

    size_t row_count_ {0};
    size_t row_capacity_ {DEFAULT_VECTOR_SIZE};
    size_t column_count_ {0};
    bool initialized = false;
};
}
