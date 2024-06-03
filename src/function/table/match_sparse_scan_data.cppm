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

export module match_sparse_scan_function_data;

import stl;
import table_function;
import global_block_id;
import block_index;
import merge_knn;
import data_block;

namespace infinity {

export class MatchSparseScanFunctionData : public TableFunctionData {
public:
    MatchSparseScanFunctionData() = default;

    MatchSparseScanFunctionData(const BlockIndex *block_index, const SharedPtr<Vector<GlobalBlockID>> &global_block_ids)
        : block_index_(block_index), global_block_ids_(global_block_ids), query_data_(DataBlock::Make()) {}

public:
    const BlockIndex *block_index_{};
    SharedPtr<Vector<GlobalBlockID>> global_block_ids_;

    bool evaluated_ = false;
    SharedPtr<DataBlock> query_data_{};

    u32 current_block_ids_idx_ = 0;
    UniquePtr<MergeKnnBase> merge_knn_base_{};
};

} // namespace infinity
