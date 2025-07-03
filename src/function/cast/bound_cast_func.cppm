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

export module bound_cast_func;

import stl;

namespace infinity {

class ColumnVector;

export struct CastParameters {
    bool strict{false};
};

using cast_function_t = bool (*)(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &result, SizeT count, CastParameters &parameters);

export struct BoundCastFunc {
    explicit inline BoundCastFunc(cast_function_t func) : function(func) {};

    cast_function_t function = nullptr;
};

} // namespace infinity
