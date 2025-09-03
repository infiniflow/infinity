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

export module infinity_core:txn_info;

import std;
import :infinity_type;

namespace infinity {

export struct TxnInfo {
    TransactionID txn_id_;
    std::shared_ptr<std::string> txn_text_;
};

export struct TxnCheckpointInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
};

export struct TxnCompactInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
};

export struct TxnOptimizeInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
};

export struct TxnImportInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
};

export struct TxnCleanInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
};

} // namespace infinity
