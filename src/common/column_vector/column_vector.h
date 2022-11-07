//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "vector_buffer.h"
#include "bitmap.h"
#include "common/types/value.h"
#include "common/default_values.h"

namespace infinity {

enum class ColumnVectorType : i8 {
    kInvalid,
    kFlat,  // Stand without any encode
    kConstant, // All vector has same type and value
    kDictionary, // There is a dictionary zone
    kRLE, // Run length encoding
    kSequence,
    kBias,

    kCollectionFlat,
    kCollectionDictionary,
    kHeterogeneous,
};

// Basic unit of column data vector
struct ColumnVector {
public:
    // Member variable
    ColumnVectorType vector_type_ {ColumnVectorType::kInvalid};
    DataType data_type_;

    i64  capacity_{0};

    // Only a pointer to the real data in vector buffer
    ptr_t data_ptr_;

    // A bitmap to indicate the null information
    UniquePtr<Bitmap> nulls_ptr_;

    // this buffer is holding the data
    SharedPtr<VectorBuffer> buffer_;

public:
    // Construct a column vector without initialization;
    explicit
    ColumnVector(DataType type, i64 capacity = DEFAULT_VECTOR_SIZE);

    // Construct a column vector from a value;
    explicit
    ColumnVector(Value& value);

    void
    Initialize(bool fill_zero = false);

    [[nodiscard]] String
    ToString() const;

    // Return the <index> of the vector.
    [[nodiscard]] Value
    GetValue(idx_t index) const;

    // Set the <index> element of the vector to the specified value.
    void
    SetValue(idx_t index, const Value &Value);

    void
    ShallowCopy(const ColumnVector &other);

    // Getter
    [[nodiscard]] const inline ColumnVectorType&
    vector_type() const {
        return vector_type_;
    }

    [[nodiscard]] const inline DataType&
    data_type() const {
        return data_type_;
    }

    [[nodiscard]] inline ptr_t data() const {
        return data_ptr_;
    }
};

}
