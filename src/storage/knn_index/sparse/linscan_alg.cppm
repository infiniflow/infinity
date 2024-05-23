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

export module linscan_alg;

import stl;

namespace infinity {

export struct SparseVecRef {
    const f32 *data_;
    const u32 *indices_;
    i32 nnz_;
};

struct Posting {
    u32 doc_id_;
    f32 val_;
};

export class LinScan {
public:
    void Insert(const SparseVecRef &vec, u32 doc_id);

    Pair<Vector<u32>, Vector<f32>> Query(const SparseVecRef &query, u32 top_k) const;

    SizeT num_docs() const { return num_docs_; }

private:
    HashMap<u32, Vector<Posting>> inverted_idx_;
    SizeT num_docs_{};
};

} // namespace infinity