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

export module set_segment_status_sealed_task;

import stl;
import bg_task;
import segment_entry;
import table_entry;
import txn;
import txn_manager;

namespace infinity {

export class SetSegmentStatusSealedTask final : public BGTask {
public:
    static void CreateAndSubmitTask(SegmentEntry *segment_entry, TableEntry *table_entry, TxnManager *txn_mgr);

    explicit SetSegmentStatusSealedTask(SegmentEntry *segment, TableEntry *table_entry, TxnManager *txn_mgr, u32 delay_cnt)
        : BGTask(BGTaskType::kSetSegmentStatusSealed, false), segment_(segment), table_entry_(table_entry), txn_mgr_(txn_mgr), delay_cnt_(delay_cnt) {
    }

    String ToString() const override { return "SetSegmentStatusSealedTask"; }

    void Execute();

    static void ExecuteInner(SegmentEntry *segment, TxnManager *txn_mgr);

private:
    SegmentEntry *segment_{};
    TableEntry *table_entry_{};
    TxnManager *txn_mgr_{};
    u32 delay_cnt_{}; // insert back to bg task queue if segment is unsealed yet
};

} // namespace infinity
