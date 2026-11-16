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

export module infinity_core:bool_cast;

import :bound_cast_func;
import :column_vector_cast;
import :infinity_exception;
import :status;
import :column_vector;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

export struct TryCastBoolean {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &) {
        std::string error_msg =
            fmt::format("No implementation to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>());
        RecoverableError(Status::NotSupport(error_msg));
        return false;
    }
};

// Cast BooleanT to other VarcharT type
template <>
inline bool TryCastBoolean::Run(BooleanT source, VarcharT &target) {
    const std::string_view text = source ? "true" : "false";
    target.length_ = static_cast<u32>(text.size());
    std::memcpy(target.short_.data_, text.data(), target.length_);
    // Clear the rest of the inline buffer. `Varchar` is a union: leaving stale
    // bytes past the text means a reader of `vector_.prefix_`/`file_offset_`
    // sees whatever the slot held before.
    std::memset(target.short_.data_ + target.length_, 0, VARCHAR_INLINE_LEN - target.length_);
    return true;
}

/// `CAST(<bool> AS VARCHAR)` on a column.
///
/// A VARCHAR column is var-length: the value must be written through the
/// column's own varchar appender so the outline buffer holds the text. Filling
/// the `VarcharT` slot directly (what `TryCastColumnVector` would do) leaves
/// the outline empty, and every later read of the cell then resolves the
/// slot's offset against a buffer that holds nothing.
export struct BooleanTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType, TargetType &, ColumnVector *) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool BooleanTryCastToVarlen::Run(BooleanT source, VarcharT &target, ColumnVector *result_vector) {
    const std::string_view text = source ? "true" : "false";
    result_vector->AppendVarcharInner(std::span<const char>(text.data(), text.size()), target);
    return true;
}

export inline BoundCastFunc BindBoolCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kBoolean) {
        UnrecoverableError(fmt::format("Expect boolean type, but it is {}", source.ToString()));
    }

    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BooleanT, VarcharT, BooleanTryCastToVarlen>);
        }
        default: {
            std::string error_msg = fmt::format("Can't cast from Boolean to {}", target.ToString());
            RecoverableError(Status::NotSupport(error_msg));
        }
    }
    return BoundCastFunc(nullptr);
}

} // namespace infinity
