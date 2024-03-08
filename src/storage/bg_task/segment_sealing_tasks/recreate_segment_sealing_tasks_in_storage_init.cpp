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

#include <unordered_map>
module recreate_segment_sealing_tasks_in_storage_init;

import stl;
import catalog;
import meta_map;
import db_meta;
import db_entry;
import table_entry;
import segment_entry;
import entry_list;
import txn;
import txn_manager;
import set_segment_status_sealed_task;
import logger;

namespace infinity {

void RecreateSegmentSealingTasksInStorageInit(Catalog *catalog, TxnManager *txn_mgr, TxnTimeStamp system_start_ts) {
    LOG_TRACE("RecreateSegmentSealingTasksInStorageInit: Begin job");
    auto txn = txn_mgr->CreateTxn();
    txn->Begin();
    MetaMap<DBMeta> &db_meta_map = catalog->db_meta_map_;
    for (auto &[name, db_meta] : db_meta_map.meta_map_) {
        List<SharedPtr<DBEntry>> &db_entry_list = db_meta->db_entry_list();
        // front: newest, back: oldest
        for (auto iter = db_entry_list.begin(); iter != db_entry_list.end(); ++iter) {
            SharedPtr<DBEntry> &db_entry = *iter;
            if (db_entry->commit_ts_ < system_start_ts) {
                // find entry, break after job done
                if (!db_entry->deleted_) {
                    // find valid entry
                    Vector<TableEntry *> table_entries = db_entry->TableCollections(txn->TxnID(), system_start_ts);
                    for (auto table_entry : table_entries) {
                        auto &segment_map = table_entry->segment_map();
                        for (auto &[segment_id, segment_entry] : segment_map) {
                            if (segment_entry->status() == SegmentStatus::kSealing) {
                                SetSegmentStatusSealedTask::CreateAndSubmitTask(segment_entry.get(), table_entry, txn_mgr);
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    txn_mgr->CommitTxn(txn);
    LOG_TRACE("RecreateSegmentSealingTasksInStorageInit: End job");
}

} // namespace infinity