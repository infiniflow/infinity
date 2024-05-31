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

module physical_scan_base;

import stl;
import physical_operator;
import physical_operator_type;
import global_block_id;
import base_table_ref;
import table_entry;
import block_index;

namespace infinity {

PhysicalScanBase::PhysicalScanBase(u64 id,
                 PhysicalOperatorType type,
                 UniquePtr<PhysicalOperator> left,
                 UniquePtr<PhysicalOperator> right,
                 SharedPtr<BaseTableRef> base_table_ref,
                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(type, std::move(left), std::move(right), id, load_metas), base_table_ref_(base_table_ref) {}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalScanBase::PlanBlockEntries(i64 parallel_count) const {
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<GlobalBlockID> global_blocks;
    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        for (const auto *block_entry : segment_info.block_map_) {
            global_blocks.emplace_back(segment_id, block_entry->block_id());
        }
    }
    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (SizeT task_id = 0, global_block_id = 0, residual_idx = 0; (i64)task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->push_back(global_blocks[global_block_id++]);
        }
        if (residual_idx < residual) {
            result[task_id]->push_back(global_blocks[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

SizeT PhysicalScanBase::TaskletCount() { return base_table_ref_->block_index_->BlockCount(); }

BlockIndex *PhysicalScanBase::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

} // namespace infinity
