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

#include <utility>
#include <vector>

export module block_iter;

import block_column_iter;
import stl;
import block_entry;
import block_column_iter;
import buffer_manager;
import infinity_exception;
import logger;
import column_vector;

namespace infinity {

export template <bool CheckTS = true>
class BlockIter {
public:
    BlockIter(const BlockEntry *entry, BufferManager *buffer_mgr, const Vector<ColumnID> &column_ids, TxnTimeStamp iterate_ts) {
        for (auto column_id : column_ids) {
            column_iters_.emplace_back(entry->GetColumnBlockEntry(column_id), buffer_mgr, iterate_ts);
        }
    }

    Optional<Pair<Vector<const void *>, BlockOffset>> Next() {
        Vector<const void *> rets;
        BlockOffset offset = 0;
        for (auto &column_iter : column_iters_) {
            auto ret_opt = column_iter.Next();
            if (!ret_opt.has_value()) {
                if (rets.empty()) {
                    return None;
                }
                String error_message = "ColumnIter return None, but other columns have data";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
            auto [ret, cur_offset] = *ret_opt;
            if (offset != cur_offset) {
                if (rets.empty()) {
                    offset = cur_offset;
                } else {
                    String error_message = "ColumnIter return different offset";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
            rets.emplace_back(ret);
        }
        return std::make_pair(std::move(rets), offset);
    }

    const SharedPtr<ColumnVector> &column_vector(SizeT col_id) const { return column_iters_[col_id].column_vector(); }

private:
    Vector<BlockColumnIter<CheckTS>> column_iters_;
};

} // namespace infinity