//
// Created by jinhai on 23-7-1.
//

#include "data_segment.h"

namespace infinity {

UniquePtr<String>
DataSegment::Append(void* txn_ptr, AppendState& append_state) {

}

UniquePtr<String>
DataSegment::Delete(void* txn_ptr, DeleteState& delete_state) {

}

UniquePtr<String>
DataSegment::InitScan(void* txn_ptr, ScanState& scan_state) {

}

UniquePtr<String>
DataSegment::Scan(void* txn_ptr, ScanState scan_state) {

}

UniquePtr<String>
DataSegment::CommitAppend(void* txn_ptr, u32 block_id, u32 block_offset, u32 row_count) {

}

UniquePtr<String>
DataSegment::RevertAppend(void* txn_ptr, u32 block_id, u32 block_offset, u32 row_count) {

}

UniquePtr<String>
DataSegment::CommitDelete(void* txn_ptr, const Vector<RowID>& row_ids) {

}

}