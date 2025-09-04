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

struct FlushDataEntry;
struct MetaKey;

export struct TxnInfo {
    TransactionID txn_id_;
    std::shared_ptr<std::string> txn_text_;
};

export struct TxnCheckpointInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    TxnTimeStamp checkpoint_ts_;
    bool committed_{false};
    bool auto_flush_{false};
    std::vector<std::shared_ptr<FlushDataEntry>> entries_{};
};

export struct TxnCompactInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
    std::vector<SegmentID> deprecated_segment_ids_;
    SegmentID new_segment_id_;
    u64 table_id_;
    std::string table_name_;
    u64 db_id_;
    std::string db_name_;
};

export struct TxnOptimizeInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
    SegmentID segment_id_;
    std::vector<ChunkID> deprecated_chunk_ids_;
    ChunkID new_chunk_id_;
    u64 table_id_;
    std::string table_name_;
    u64 db_id_;
    std::string db_name_;
    std::string index_name_;
    u64 index_id_;
};

export struct TxnImportInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
    std::string db_name_{};
    std::string table_name_{};
    u64 db_id_{};
    u64 table_id_{};
    std::vector<SegmentID> segment_ids_{};
    u64 row_count_{};
};

export struct TxnCleanInfo {
    TransactionID txn_id_;
    TxnTimeStamp begin_ts_;
    TxnTimeStamp commit_ts_;
    bool committed_{false};
    std::vector<std::string> dropped_keys_;
    std::vector<std::shared_ptr<MetaKey>> metas_;
};

} // namespace infinity
