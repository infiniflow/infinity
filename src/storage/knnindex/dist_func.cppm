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

export module dist_func;

namespace infinity {

export template <typename DataType, typename DataType2 = const DataType *>
using DistFunc = DataType(*)(const DataType *, DataType2, SizeT);

export template<typename DataType, typename DataType2>
using DistFunc2 = DataType(*)(DataType2, DataType2, SizeT);

export template <typename DataType, typename DataType2 = const DataType *>
class SpaceBase {
protected:
    DistFunc<DataType, DataType2> fstdistfunc_ = nullptr;

    DistFunc2<DataType, DataType2> fstdistfunc2_ = nullptr;

public:
    DistFunc<DataType, DataType2> DistFuncPtr() const { return fstdistfunc_; }

    DistFunc2<DataType, DataType2> DistFuncPtr2() const { return fstdistfunc2_; }

    virtual ~SpaceBase() = default;
};

} // namespace infinity
