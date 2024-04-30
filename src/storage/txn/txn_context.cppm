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
import txn_state;

import infinity_exception;

export module txn_context;

namespace infinity {

class Txn;

export class TxnContext {
public:
    friend class Txn;

    TxnContext(TxnTimeStamp begin_ts) : begin_ts_(begin_ts) {}

    inline TxnTimeStamp GetBeginTS() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return begin_ts_;
    }

    inline TxnTimeStamp GetCommitTS() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return commit_ts_;
    }

    inline TxnState GetTxnState() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return state_;
    }

    inline void SetTxnRollbacking(TxnTimeStamp rollback_ts) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kCommitting && state_ != TxnState::kStarted) {
            UnrecoverableError("Transaction isn't in TO_ROLLBACK status.");
        }
        state_ = TxnState::kRollbacking;
        commit_ts_ = rollback_ts; // update commit_ts ?
    }

    inline void SetTxnRollbacked() {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kRollbacking) {
            UnrecoverableError("Transaction isn't in ROLLBACKING status.");
        }
        state_ = TxnState::kRollbacked;
    }

    inline void SetTxnCommitted() {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kCommitting) {
            UnrecoverableError("Transaction isn't in COMMITTING status.");
        }
        state_ = TxnState::kCommitted;
    }

    inline void SetTxnCommitting(TxnTimeStamp commit_ts) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kStarted) {
            UnrecoverableError("Transaction isn't in STARTED status.");
        }
        state_ = TxnState::kCommitting;
        commit_ts_ = commit_ts;
    }

    void SetTxnType(TxnType type) { type_ = type; }

    TxnType GetTxnType() const { return type_; }

private:
    std::shared_mutex rw_locker_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};
    TxnState state_{TxnState::kStarted};
    TxnType type_{TxnType::kInvalid};
};

} // namespace infinity
