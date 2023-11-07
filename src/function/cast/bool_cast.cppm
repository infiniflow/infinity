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

import parser;
import stl;
import bound_cast_func;
import column_vector_cast;
import infinity_exception;
import third_party;
import column_vector;

export module bool_cast;

namespace infinity {

export struct TryCastBoolean {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType input, TargetType &target) {
        Error<FunctionException>("No implementation to cast from " + DataType::TypeToString<SourceType>() + " to " +
                                 DataType::TypeToString<TargetType>());
    }
};

// Cast BooleanT to other VarcharT type
template <>
inline bool TryCastBoolean::Run(BooleanT source, VarcharT &target) {
    if (source) {
        constexpr u16 TRUE_LEN = 4;
        Memcpy(target.prefix, "true", TRUE_LEN);
        Memset(target.prefix + TRUE_LEN, 0, VarcharT::INLINE_LENGTH - TRUE_LEN);
        target.length = TRUE_LEN;
    } else {
        constexpr u16 FALSE_LEN = 5;
        Memcpy(target.prefix, "false", FALSE_LEN);
        Memset(target.prefix + FALSE_LEN, 0, VarcharT::INLINE_LENGTH - FALSE_LEN);
        target.length = FALSE_LEN;
    }
    return true;
}

export inline BoundCastFunc BindBoolCast(const DataType &source, const DataType &target) {
    Assert<TypeException>(source.type() == LogicalType::kBoolean, Format("Expect boolean type, but it is {}", source.ToString()));

    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<BooleanT, VarcharT, TryCastBoolean>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Boolean to {}", target.ToString()));
        }
    }
}

} // namespace infinity