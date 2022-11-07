//
// Created by JinHai on 2022/10/30.
//

#include "column_vector.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

ColumnVector::ColumnVector(DataType type, i64 capacity) : data_type_(std::move(type)), capacity_(capacity) {}

ColumnVector::ColumnVector(Value &value) : data_type_(std::move(value.type_)) {

}

void ColumnVector::Initialize(bool fill_zero) {

}

String
ColumnVector::ToString() const {
    return infinity::String();
}

Value
ColumnVector::GetValue(idx_t index) const {
    TypeError("Not implemented");
}

void
ColumnVector::SetValue(idx_t index, const Value &Value) {

}

void
ColumnVector::ShallowCopy(const ColumnVector &other) {

}

}
