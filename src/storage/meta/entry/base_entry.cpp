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

namespace infinity {

BaseEntry::BaseEntry(const BaseEntry &other) : deleted_(other.deleted_), entry_type_(other.entry_type_), encode_(other.encode_) {
    txn_id_ = other.txn_id_;
    begin_ts_ = other.begin_ts_;
    commit_ts_ = other.commit_ts_.load();
}

bool BaseEntry::CheckVisible(Txn *txn) const {
    TxnTimeStamp begin_ts = txn->BeginTS();
    if (txn_id_ == 0) {
        // could not check if the entry is visible accurately. log a warning and return true
        LOG_WARN(fmt::format("Entry {} txn id is not set, commit_ts: {}", *encode_, commit_ts_));
        return begin_ts >= commit_ts_;
    }
    if (begin_ts >= commit_ts_ || txn_id_ == txn->TxnID()) {
        return true;
    }
    TxnManager *txn_mgr = txn->txn_mgr();
    if (txn_mgr == nullptr) { // when replay
        String error_message = fmt::format("Replay should not reach here. begin_ts: {}, commit_ts_: {} txn_id: {}, txn_id_: {}",
                                           begin_ts,
                                           commit_ts_,
                                           txn->TxnID(),
                                           txn_id_);
        UnrecoverableError(error_message);
    }
    // Check if the entry is in committing process, because commit_ts of the base_entry is set in the Txn::CommitBottom
    return txn_mgr->CheckIfCommitting(txn_id_, begin_ts);
}

} // namespace infinity
