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

export module sparse_vector_distance;

import stl;

namespace infinity {

export template <typename DataType, typename IndexType>
DataType SparseIPDistance(const DataType *data1, const IndexType *index1, SizeT nnz1, const DataType *data2, const IndexType *index2, SizeT nnz2) {
    DataType distance{};
    SizeT i = 0, j = 0;
    while (i < nnz1 && j < nnz2) {
        if (index1[i] == index2[j]) {
            distance += data1[i] * data2[j];
            ++i;
            ++j;
        } else if (index1[i] < index2[j]) {
            ++i;
        } else {
            ++j;
        }
    }
    return distance;
}

} // namespace infinity
