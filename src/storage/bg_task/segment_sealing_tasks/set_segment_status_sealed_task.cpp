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

module set_segment_status_sealed_task;

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

void SetSegmentStatusSealedTask::CreateAndSubmitTask(SegmentEntry *segment_entry, TableEntry *table_entry, TxnManager *txn_mgr) {
    if (SegmentStatus status = segment_entry->status(); status != SegmentStatus::kSealing and status != SegmentStatus::kUnsealed) {
        UnrecoverableError("Segment status error");
    }
    LOG_TRACE(fmt::format("SetSegmentStatusSealedTask: create task for segment: {}", segment_entry->segment_id()));
    auto sealing_task = MakeShared<SetSegmentStatusSealedTask>(segment_entry, table_entry, txn_mgr, 0);
    auto bg_processor = txn_mgr->bg_task_processor();
    bg_processor->Submit(std::move(sealing_task));
}

void SetSegmentStatusSealedTask::Execute() {
    // check segment status
    if (SegmentStatus status = segment_->status(); status != SegmentStatus::kSealing) {
        if (status == SegmentStatus::kUnsealed) {
            if (++delay_cnt_ >= 3) {
                LOG_TRACE(fmt::format("SetSegmentStatusSealedTask: segment is still unsealed, delay_cnt: {}", delay_cnt_));
            }
            auto sealing_task = MakeShared<SetSegmentStatusSealedTask>(segment_, table_entry_, txn_mgr_, delay_cnt_);
            auto bg_processor = txn_mgr_->bg_task_processor();
            bg_processor->Submit(std::move(sealing_task));
            return;
        } else {
            UnrecoverableError("Segment status error");
        }
    }
    ExecuteInner(segment_, txn_mgr_);
    // try to trigger compaction
    {
        LOG_TRACE("SetSegmentStatusSealedTask: add sealed segment to compact candidate");
        std::function<Txn *()> generate_txn = [txn_mgr_ = this->txn_mgr_]() { return txn_mgr_->CreateTxn(); };
        auto ret = table_entry_->AddSegment(segment_, generate_txn);
        if (ret.has_value()) {
            auto &[to_compacts, txn] = *ret;
            auto compact_task = CompactSegmentsTask::MakeTaskWithPickedSegments(table_entry_, std::move(to_compacts), txn);
            txn_mgr_->bg_task_processor()->Submit(std::move(compact_task));
        }
    }
}

void SetSegmentStatusSealedTask::ExecuteInner(SegmentEntry *segment, TxnManager *txn_mgr) {
    // create txn
    auto sealing_task_txn = txn_mgr->CreateTxn();
    // begin txn
    sealing_task_txn->Begin();
    auto buffer_manager = sealing_task_txn->buffer_manager();
    auto begin_ts = sealing_task_txn->BeginTS();
    LOG_TRACE(fmt::format("SetSegmentStatusSealedTask: BeginTS: {} begin task for segment: {}", begin_ts, segment->segment_id()));
    // build filter
    BuildFastRoughFilterTask::Execute(segment, buffer_manager, begin_ts, SegmentStatus::kSealing);
    // serialize filter
    String segment_filter_binary_data = segment->GetFastRoughFilter()->SerializeToString();
    Vector<Pair<BlockID, String>> block_filter_binary_data = segment->GetBlockFilterBinaryDataVector();
    // build WalCmd
    {
        auto table_entry = segment->GetTableEntry();
        String db_name = *table_entry->GetDBName();
        String table_name = *table_entry->GetTableName();
        SegmentID segment_id = segment->segment_id();
        String segment_filter_binary_data_copy = segment_filter_binary_data;
        Vector<Pair<BlockID, String>> block_filter_binary_data_copy = block_filter_binary_data;
        auto wal_cmd = MakeShared<WalCmdSetSegmentSealed>(std::move(db_name),
                                                          std::move(table_name),
                                                          segment_id,
                                                          std::move(segment_filter_binary_data_copy),
                                                          std::move(block_filter_binary_data_copy));
        sealing_task_txn->AddWalCmd(std::move(wal_cmd));
    }
    // build CatalogDeltaOperation
    {
        auto catalog_delta_op = MakeUnique<SetSegmentStatusSealedOp>(segment,
                                                                     std::move(segment_filter_binary_data),
                                                                     std::move(block_filter_binary_data),
                                                                     SegmentStatus::kSealing);
        sealing_task_txn->AddCatalogDeltaOperation(std::move(catalog_delta_op));
    }
    // segment status will be changed in SetSegmentStatusSealedOp::SaveSate(), in CommitBottom()
    auto commit_ts = txn_mgr->CommitTxn(sealing_task_txn);
    LOG_TRACE(fmt::format("SetSegmentStatusSealedTask: CommitTS: {} end task for segment: {}", commit_ts, segment->segment_id()));
}

} // namespace infinity
