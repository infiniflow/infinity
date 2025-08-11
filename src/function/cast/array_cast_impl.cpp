// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:array_cast.impl;

import :array_cast;
import :stl;
import :column_vector;
import :vector_buffer;
import :bound_cast_func;
import :column_vector_cast;
import third_party;
import :logger;
import :status;
import logical_type;
import internal_types;
import data_type;
import array_info;
import :infinity_exception;
import :value;
import :cast_function;
import :default_values;
import :bound_cast_func;

namespace infinity {

struct ArrayTryCastToArray {
    template <typename SourceT, typename TargetT>
    static bool Run(const SourceT &source,
                    const DataType &source_type,
                    const ColumnVector *source_vector_ptr,
                    TargetT &target,
                    const DataType &target_type,
                    ColumnVector *target_vector_ptr) {
        UnrecoverableError("Unreachable code");
        return false;
    }
};

template <>
bool ArrayTryCastToArray::Run<ArrayT, ArrayT>(const ArrayT &source,
                                              const DataType &source_type,
                                              const ColumnVector *source_vector_ptr,
                                              ArrayT &target,
                                              const DataType &target_type,
                                              ColumnVector *target_vector_ptr) {
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kVarBuffer) {
        UnrecoverableError(fmt::format("Array column vector should use kVarBuffer VectorBuffer."));
    }
    const auto src_value = source_vector_ptr->GetArrayValue(source);
    if (src_value.type() != source_type) {
        UnrecoverableError(fmt::format("Source Array type error: expected {}, got {}.", source_type.ToString(), src_value.type().ToString()));
    }
    const auto &src_elems = src_value.GetArray();
    Vector<Value> result_elements;
    if (!src_elems.empty()) {
        // non-empty array, need to cast each element
        const auto &src_elem_type = dynamic_cast<const ArrayInfo *>(src_value.type().type_info().get())->ElemType();
        const auto &target_elem_type = dynamic_cast<const ArrayInfo *>(target_type.type_info().get())->ElemType();
        if (src_elem_type == target_elem_type) {
            // no need to cast
            UnrecoverableError("Error: source and target element types are the same. No need to cast.");
        }
        const auto cast_func = CastFunction::GetBoundFunc(src_elem_type, target_elem_type);
        const auto src_elem_type_ptr = MakeShared<DataType>(src_elem_type);
        const auto target_elem_type_ptr = MakeShared<DataType>(target_elem_type);
        const u64 num_elements = src_elems.size();
        for (u64 next_job_idx = 0; next_job_idx < num_elements;) {
            const u64 job_size = std::min(num_elements - next_job_idx, static_cast<u64>(DEFAULT_VECTOR_SIZE));
            auto src_elem_column_vector = MakeShared<ColumnVector>(src_elem_type_ptr);
            src_elem_column_vector->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
            for (u64 i = 0; i < job_size; ++i) {
                src_elem_column_vector->AppendValue(src_elems[next_job_idx + i]);
            }
            auto cast_elem_column_vector = MakeShared<ColumnVector>(target_elem_type_ptr);
            cast_elem_column_vector->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
            CastParameters cast_parameters;
            cast_func.function(src_elem_column_vector, cast_elem_column_vector, job_size, cast_parameters);
            for (u64 i = 0; i < job_size; ++i) {
                result_elements.emplace_back(cast_elem_column_vector->GetValueByIndex(i));
            }
            next_job_idx += job_size;
        }
    }
    const auto result_value = Value::MakeArray(std::move(result_elements), target_type.type_info());
    target_vector_ptr->SetArrayValue(target, result_value);
    return true;
}

bool ArrayInfoCanConvertTo(const ArrayInfo *left, const ArrayInfo *right) {
    const auto &left_elem_type = left->ElemType();
    const auto &right_elem_type = right->ElemType();
    if (left_elem_type == right_elem_type) {
        return true;
    }
    if (left_elem_type.type() == LogicalType::kInvalid) {
        // empty array type can convert to any non-empty array type
        return true;
    }
    // try cast element type
    try {
        CastFunction::GetBoundFunc(left_elem_type, right_elem_type);
        return true;
    } catch (...) {
        // cast failed
        return false;
    }
}

BoundCastFunc BindArrayCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kArray || target.type() != LogicalType::kArray) {
        RecoverableError(Status::NotSupportedTypeConversion(source.ToString(), target.ToString()));
    }
    const auto *source_info = static_cast<const ArrayInfo *>(source.type_info().get());
    const auto *target_info = static_cast<const ArrayInfo *>(target.type_info().get());
    if (!ArrayInfoCanConvertTo(source_info, target_info)) {
        RecoverableError(Status::DataTypeMismatch(source.ToString(), target.ToString()));
    }
    return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorVarlenWithType<ArrayT, ArrayT, ArrayTryCastToArray>);
}

} // namespace infinity
