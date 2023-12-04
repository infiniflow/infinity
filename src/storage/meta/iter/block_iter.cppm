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

import stl;
import block_entry;
import block_column_iter;

export module block_iter;

namespace infinity {

export class BlockIter {
private:
    static Vector<BlockColumnIter> MakeColumnIters(const BlockEntry *entry, const std::vector<SizeT> &column_ids) {
        Vector<BlockColumnIter> ret;
        for (auto column_id : column_ids) {
            ret.emplace_back(BlockEntry::GetColumnDataByID(entry, column_id), entry->row_count_);
        }
        return ret;
    }

public:
    BlockIter(const BlockEntry *entry, const Vector<SizeT> &column_ids) : column_iters_(MakeColumnIters(entry, column_ids)) {}

    Optional<Vector<const void*>> Next() {
        Vector<const void*> rets;
        for (auto &column_iter : column_iters_) {
            auto ret = column_iter.Next();
            if (!ret) {
                return None;
            }
            rets.emplace_back(*ret);
        }
        return rets;
    }

    Vector<SizeT> GetColumnIds() const {
        Vector<SizeT> ret;
        for (auto &column_iter : column_iters_) {
            ret.emplace_back(column_iter.column_id_);
        }
        return ret;
    }

private:
    Vector<BlockColumnIter> column_iters_{};
};

} // namespace infinity