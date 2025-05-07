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

export module base_txn_store;

import stl;
import internal_types;

namespace infinity {

class DataBlock;

enum class TxnStoreType {
    kInvalid,
    kAppend,
    kImport,
    kDelete,
    kUpdate,
    kCreateIndex,
    kDumpMemIndex,
    kOptimizeIndex,
    kCompact,
};

export struct BaseTxnStore {
    explicit BaseTxnStore(TxnStoreType type) : type_(type) {};

    TxnStoreType type_{TxnStoreType::kInvalid};
};

export struct AppendTxnStore : public BaseTxnStore {
    explicit AppendTxnStore() : BaseTxnStore(TxnStoreType::kAppend) {}

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    SharedPtr<DataBlock> input_block_{};
    Vector<Pair<RowID, u64>> row_ranges_{};
};

export struct ImportTxnStore : public BaseTxnStore {
    explicit ImportTxnStore() : BaseTxnStore(TxnStoreType::kImport) {}

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SharedPtr<DataBlock>> input_blocks_{};
    SegmentID segment_id_{};
};

} // namespace infinity