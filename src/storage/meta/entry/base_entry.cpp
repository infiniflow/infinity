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

module base_entry;

import stl;
import txn_manager;
import txn_state;
import txn;
import infinity_exception;
import third_party;

namespace infinity {

bool BaseEntry::CheckVisible(Txn *txn) const {
    TxnTimeStamp begin_ts = txn->BeginTS();
    TransactionID txn_id = txn_id_.load();
    if (begin_ts >= commit_ts_ || txn_id == txn->TxnID()) {
        return true;
    }
    TxnManager *txn_mgr = txn->txn_mgr();
    if (txn_mgr == nullptr) { // when replay
        UnrecoverableError(fmt::format("Replay should not reach here. begin_ts: {}, commit_ts_: {} txn_id: {}, txn_id_: {}",
                                       begin_ts,
                                       commit_ts_,
                                       txn->TxnID(),
                                       txn_id));
    }
    return txn_mgr->CheckIfCommitting(txn_id, begin_ts);
}

} // namespace infinity