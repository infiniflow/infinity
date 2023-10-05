//
// Created by jinhai on 23-9-22.
//

#include "physical_knn_scan.h"
#include "planner/bound/base_table_ref.h"
#include "function/table/knn_scan.h"

namespace infinity {

void
PhysicalKnnScan::Init() {
}

void
PhysicalKnnScan::Execute(QueryContext* query_context) {

}

void
PhysicalKnnScan::Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) {
    auto* knn_scan_input_state = static_cast<KnnScanInputState*>(input_state);
    auto* knn_scan_output_state = static_cast<KnnScanOutputState*>(output_state);

//    Vector<u64>& segment_entry_index_array = *knn_scan_input_state->knn_scan_function_data_->segment_indexes_;
//    for(const u64 segment_idx: segment_entry_index_array) {
//        SegmentEntry* segment_entry = base_table_ref_->segment_entries_->at(segment_idx);
//        LOG_TRACE("Segment Entry ID: {}", segment_entry->segment_id_);
//    }

    KnnScanFunction* knn_scan_func = (KnnScanFunction*)base_table_ref_->table_func_.get();
    knn_scan_func->main_function_(query_context,
                                  knn_scan_input_state->knn_scan_function_data_.get(),
                                  *knn_scan_output_state->data_block_);
}

TableCollectionEntry*
PhysicalKnnScan::table_collection_ptr() const {
    return base_table_ref_->table_entry_ptr_;
}


String
PhysicalKnnScan::TableAlias() const {
    return base_table_ref_->alias_;
}

BlockIndex*
PhysicalKnnScan::GetBlockIndex() const {
    return base_table_ref_->block_index_.get();
}

Vector<SizeT>&
PhysicalKnnScan::ColumnIDs() const {
    return base_table_ref_->column_ids_;
}

Vector<SharedPtr<Vector<GlobalBlockID>>>
PhysicalKnnScan::PlanSegmentEntries(i64 parallel_count) const {
    BlockIndex* block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for(u64 task_id = 0, global_block_id = 0, residual_idx = 0; task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for(u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
        }
        if(residual_idx < residual) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

SizeT
PhysicalKnnScan::BlockEntryCount() const {
    return base_table_ref_->block_index_->BlockCount();
}

}
