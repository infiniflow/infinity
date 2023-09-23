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

Vector<SegmentEntry*>*
PhysicalKnnScan::SegmentEntriesPtr() const {
    return base_table_ref_->segment_entries_.get();
}

Vector<SizeT>&
PhysicalKnnScan::ColumnIDs() const {
    return base_table_ref_->column_ids_;
}

Vector<SharedPtr<Vector<u64>>>
PhysicalKnnScan::PlanSegmentEntries(i64 parallel_count) const {
    SizeT segment_count = base_table_ref_->segment_entries_->size();
    Vector<SharedPtr<Vector<u64>>> result(parallel_count, nullptr);
    for(SizeT task_id = 0; task_id < parallel_count; ++ task_id) {
        result[task_id] = MakeShared<Vector<u64>>();
    }

    for(SizeT idx = 0; idx < segment_count; ++ idx) {
        u64 task_id = idx % parallel_count;
        result[task_id]->emplace_back(idx);
    }
    return result;
}

SizeT
PhysicalKnnScan::SegmentEntryCount() const {
    return base_table_ref_->segment_entries_->size();
}
}
