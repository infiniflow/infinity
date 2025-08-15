// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:txn_context;

import :stl;
import :txn_state;
import :default_values;

namespace infinity {

export struct TxnContext {
    // This struct is used to store the operation history of a transaction. This history can be used for debugging purposes.
    static UniquePtr<TxnContext> Make() { return MakeUnique<TxnContext>(); }

    void AddOperation(const SharedPtr<String> &operation_text) { operations_.push_back(operation_text); }
    Vector<SharedPtr<String>> GetOperations() const { return operations_; }
    String ToString();

    TransactionID txn_id_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{UNCOMMIT_TS};
    TxnTimeStamp kv_commit_ts_{UNCOMMIT_TS};
    TxnTimeStamp last_kv_commit_ts_{};
    TxnTimeStamp last_commit_ts_{};
    TxnState state_{TxnState::kStarted};
    TransactionType txn_type_{TransactionType::kRead};

    bool is_write_transaction_{false};
    bool replay_{false};

    SharedPtr<String> text_{};
    Vector<SharedPtr<String>> operations_;
};

} // namespace infinity
