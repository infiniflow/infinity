// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:txn_context.impl;

import :txn_context;
import :stl;

import std;

namespace infinity {

String TxnContext::ToString() {
    std::stringstream ss;
    ss << "Txn ID: " << txn_id_;
    if (text_ != nullptr) {
        ss << ", Text: " << *text_;
    }
    ss << ", Begin TS: " << begin_ts_ << ", Commit TS: " << commit_ts_ << ", KV Commit TS: " << kv_commit_ts_ << ", State: " << TxnState2Str(state_)
       << "\n";
    for (const auto &operation : operations_) {
        ss << "    " << *operation << "\n";
    }
    return ss.str();
}

} // namespace infinity