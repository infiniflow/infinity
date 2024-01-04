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

#include <vector>

export module block_iter;

import stl;
import catalog;
import block_column_iter;

namespace infinity {

export class BlockIter {
public:
    BlockIter(const BlockEntry *entry, const Vector<SizeT> &column_ids) {
        Vector<BlockColumnIter> column_iters;
        for (auto column_id : column_ids) {
            column_iters.emplace_back(entry->GetColumnDataByID(column_id), entry->row_count());
        }
        column_iters_ = column_iters;
    }

    Optional<Vector<const void *>> Next() {
        Vector<const void *> rets;
        for (auto &column_iter : column_iters_) {
            auto ret = column_iter.Next();
            if (!ret) {
                return None;
            }
            rets.emplace_back(*ret);
        }
        return rets;
    }

private:
    Vector<BlockColumnIter> column_iters_{};
};

} // namespace infinity