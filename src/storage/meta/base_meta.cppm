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

import stl;
import base_entry;
import parser;

export module base_meta;

namespace infinity {

class TxnManager;

export class BaseMeta {
public:
    static EntryResult DropNewEntry(BaseMeta *meta, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static void DeleteNewEntry(BaseMeta *meta, u64 txn_id, TxnManager *txn_mgr);

    static EntryResult GetEntry(BaseMeta *meta, u64 txn_id, TxnTimeStamp begin_ts);

protected:
    enum EntryStatus : u8 {
        kExisted = 0,
        kNotExisted = 1,
        kConflict = 2,
    };

    // The write lock must be held before calling this function.
    // It return the status useful when to add new/delete entry
    EntryStatus AddEntryInternal(u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) const;

    RWMutex rw_locker_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};

} // namespace infinity