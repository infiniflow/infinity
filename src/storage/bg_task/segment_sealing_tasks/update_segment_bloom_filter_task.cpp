//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

module update_segment_bloom_filter_task;

import stl;
import bg_task;
import background_process;
import txn_manager;
import segment_entry;
import table_entry;
import buffer_manager;
import infinity_exception;
import logger;
import wal_entry;
import third_party;
import build_fast_rough_filter_task;
import compact_segments_task;
import catalog_delta_entry;

namespace infinity {

void UpdateSegmentBloomFilterTask::CreateAndSubmitTask(SegmentEntry *segment_entry, TableEntry *table_entry, TxnManager *txn_mgr) {
    if (SegmentStatus status = segment_entry->status(); status != SegmentStatus::kSealed) {
        LOG_WARN("UpdateSegmentBloomFilterTask::CreateAndSubmitTask: Segment status not sealed, skip job");
        return;
    }
    LOG_TRACE(fmt::format("UpdateSegmentBloomFilterTask: create task for segment: {}", segment_entry->segment_id()));
    auto update_bloom_filter_task = MakeShared<UpdateSegmentBloomFilterTask>(segment_entry, table_entry, txn_mgr);
    auto bg_processor = txn_mgr->bg_task_processor();
    bg_processor->Submit(std::move(update_bloom_filter_task));
}

void UpdateSegmentBloomFilterTask::Execute() {
    // check segment status
    if (SegmentStatus status = segment_->status(); status != SegmentStatus::kSealed) {
        LOG_WARN("UpdateSegmentBloomFilterTask::Execute: Segment status not sealed, abort task");
        return;
    }
    ExecuteInner(segment_, txn_mgr_);
}

// deprecated temporary
void UpdateSegmentBloomFilterTask::ExecuteInner(SegmentEntry *segment, TxnManager *txn_mgr) {
    // create txn
    // auto update_bloom_filter_txn = txn_mgr->BeginTxn();
    // // begin txn
    // // need a txn to keep segment from being deleted
    // update_bloom_filter_txn->Begin();
    // auto *buffer_mgr = update_bloom_filter_txn->buffer_mgr();
    // auto begin_ts = update_bloom_filter_txn->BeginTS();
    // LOG_TRACE(fmt::format("SetSegmentStatusSealedTask: BeginTS: {} begin task for segment: {}", begin_ts, segment->segment_id()));
    // // build filter
    // // TODO: check segment visibility?
    // BuildFastRoughFilterTask::ExecuteUpdateSegmentBloomFilter(segment, buffer_mgr, begin_ts);
    // // serialize filter
    // String segment_filter_binary_data = segment->GetFastRoughFilter()->SerializeToString();
    // Vector<Pair<BlockID, String>> block_filter_binary_data = segment->GetBlockFilterBinaryDataVector();
    // // build WalCmd
    // {
    //     auto table_entry = segment->GetTableEntry();
    //     String db_name = *table_entry->GetDBName();
    //     String table_name = *table_entry->GetTableName();
    //     SegmentID segment_id = segment->segment_id();
    //     auto wal_cmd = MakeShared<WalCmdUpdateSegmentBloomFilterData>(std::move(db_name),
    //                                                                   std::move(table_name),
    //                                                                   segment_id,
    //                                                                   segment_filter_binary_data,
    //                                                                   block_filter_binary_data);
    //     update_bloom_filter_txn->AddWalCmd(std::move(wal_cmd));
    // }
    // // build CatalogDeltaOperation
    // {
    //     auto catalog_delta_op =
    //         MakeUnique<UpdateSegmentBloomFilterDataOp>(segment, std::move(segment_filter_binary_data), std::move(block_filter_binary_data));
    //     update_bloom_filter_txn->AddCatalogDeltaOperation(std::move(catalog_delta_op));
    // }
    // // segment status will be changed in SetSegmentStatusSealedOp::SaveSate(), in CommitBottom()
    // auto commit_ts = txn_mgr->CommitTxn(update_bloom_filter_txn);
    // LOG_TRACE(fmt::format("SetSegmentStatusSealedTask: CommitTS: {} end task for segment: {}", commit_ts, segment->segment_id()));
}

} // namespace infinity
