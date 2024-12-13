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

export module blockmax_leaf_iterator;

import stl;
import internal_types;
import doc_iterator;

namespace infinity {

export class BlockMaxLeafIterator : public DocIterator {
public:
    virtual RowID BlockMinPossibleDocID() const = 0;

    virtual RowID BlockLastDocID() const = 0;

    virtual float BlockMaxBM25Score() = 0;

    // Move block cursor to ensure its last_doc_id is no less than given doc_id.
    // Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
    // Note that this routine decode skip_list only, and doesn't update doc_id_ when returns true.
    // Caller may invoke BlockMaxBM25Score() after this routine.
    virtual bool NextShallow(RowID doc_id) = 0;

    virtual float BM25Score() = 0;
};

} // namespace infinity
