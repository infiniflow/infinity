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

export module empty_array_cast;

import stl;
import data_type;
import bound_cast_func;
import column_vector_cast;
import infinity_exception;
import internal_types;
import logical_type;
import third_party;

namespace infinity {

struct EmptyTryCastToFixlen;

export inline BoundCastFunc BindEmptyArrayCast(const DataType &source, const DataType &target) {
    switch (target.type()) {
        case LogicalType::kSparse: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVector<EmptyArrayT, SparseT, EmptyTryCastToFixlen>);
        }
        default: {
            UnrecoverableError("Not implemented");
        }
    }
    return BoundCastFunc(nullptr);
}

struct EmptyTryCastToFixlen {
    template <typename SourceType, typename TargetType>
    static bool Run(SourceType, TargetType &) {
        UnrecoverableError(fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template<>
bool EmptyTryCastToFixlen::Run(EmptyArrayT, SparseT &target) {
    target.nnz_ = 0;
    return true;
}

}