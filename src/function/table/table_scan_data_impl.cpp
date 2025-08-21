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

module infinity_core:table_scan_data.impl;

import :table_scan_function_data;
import :global_block_id;
import :block_index;

namespace infinity {

TableScanFunctionData::TableScanFunctionData(const BlockIndex *block_index,
                                             const std::shared_ptr<std::vector<GlobalBlockID>> &global_block_ids,
                                             const std::vector<size_t> &column_ids)
    : block_index_(block_index), global_block_ids_(global_block_ids), column_ids_(column_ids) {}

} // namespace infinity
