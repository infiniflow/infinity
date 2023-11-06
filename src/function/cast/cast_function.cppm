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

import bound_cast_func;
import parser;

export module cast_function;

namespace infinity {

export class CastFunction {
public:
    static BoundCastFunc GetBoundFunc(const DataType &source, const DataType &target);

private:
    static BoundCastFunc NumericCast(const DataType &source, const DataType &target);

    static BoundCastFunc DecimalCast(const DataType &source, const DataType &target);

    static BoundCastFunc StringCast(const DataType &source, const DataType &target);
};

} // namespace infinity
