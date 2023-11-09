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
import db_entry;
import db_meta;
import txn_manager;
import function;
import function_set;
import table_function;
import third_party;
import buffer_manager;
import profiler;

export module new_catalog;

namespace infinity {

template<typename T>
class LimitedVector {
public:
    explicit LimitedVector(SizeT max_size) : max_size_(max_size) {
        vector_.reserve(max_size);
    }

    void PushBack(const T& value) {
        if (vector_.size() >= max_size_) {
            vector_.erase(vector_.begin());
        }
        vector_.push_back(value);
    }

    const T& operator[](SizeT index) const {
        return vector_[index];
    }

    T& operator[](SizeT index) {
        return vector_[index];
    }

    [[nodiscard]] SizeT Size() const {
        return vector_.size();
    }

private:
    Vector<T> vector_;
    SizeT max_size_;
};

export struct NewCatalog {
public:
    explicit NewCatalog(SharedPtr<String> dir, bool create_default_db = false);

public:
    static EntryResult CreateDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult DropDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult GetDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts);

    static void RemoveDBEntry(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnManager *txn_mgr);

    static Vector<DBEntry *> Databases(NewCatalog *catalog, u64 txn_id, TxnTimeStamp begin_ts);

    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, String function_name);

    static void AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void DeleteFunctionSet(NewCatalog *catalog, String function_name);

    // Table Function related methods
    static SharedPtr<TableFunction> GetTableFunctionByName(NewCatalog *catalog, String function_name);

    static void AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function);

    static void DeleteTableFunction(NewCatalog *catalog, String function_name);

    static Json Serialize(NewCatalog *catalog, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static void Deserialize(const Json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog);

    static UniquePtr<NewCatalog> LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr);

    static UniquePtr<NewCatalog> LoadFromFile(const String &catalog_path, BufferManager *buffer_mgr);

    static String SaveAsFile(NewCatalog *catalog_ptr, const String &dir, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    void MergeFrom(NewCatalog &other);

    void AppendProfilerRecord(SharedPtr<QueryProfiler> profiler) {
        records_.PushBack(Move(profiler));
    }

    const QueryProfiler *GetProfilerRecord(SizeT index) {
        if (index >= records_.Size()) {
            return nullptr;
        }
        return records_[index].get();
    }
public:
    SharedPtr<String> current_dir_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    u64 next_txn_id_{};
    u64 catalog_version_{};
    RWMutex rw_locker_;

    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_;
    HashMap<String, SharedPtr<TableFunction>> table_functions_;
    LimitedVector<SharedPtr<QueryProfiler>> records_{ 100 };
};

} // namespace infinity
