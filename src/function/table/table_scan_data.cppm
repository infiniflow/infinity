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

export module infinity_core:table_scan_function_data;

import :function_data;
import :table_function;
import :global_block_id;
import :new_catalog;

namespace infinity {

class BlockIndex;

export class TableScanFunctionData : public TableFunctionData {
public:
    TableScanFunctionData(const BlockIndex *block_index,
                          const std::shared_ptr<std::vector<GlobalBlockID>> &global_block_ids,
                          const std::vector<size_t> &column_ids);

    const BlockIndex *block_index_{};
    const std::shared_ptr<std::vector<GlobalBlockID>> &global_block_ids_{};
    const std::vector<size_t> &column_ids_{};

    u64 current_block_ids_idx_{0};
    size_t current_read_offset_{0};

    std::optional<NewTxnGetVisibleRangeState> get_visible_range_state_{};
};

} // namespace infinity
