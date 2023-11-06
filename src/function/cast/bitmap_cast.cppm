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

import stl;
import parser;
import column_vector_cast;

import infinity_exception;
import bound_cast_func;
import column_vector;
import third_party;

export module bitmap_cast;

namespace infinity {

export struct BitmapTryCastToVarlen;

export inline BoundCastFunc BindBitmapCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BitmapT, VarcharT, BitmapTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Time type to {}", target.ToString()));
        }
    }
}

struct BitmapTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

template <>
inline bool BitmapTryCastToVarlen::Run(const BitmapT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    Error<FunctionException>("Not implemented");
}

} // namespace infinity
