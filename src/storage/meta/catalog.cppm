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
import special_function;
import third_party;
import buffer_manager;
import profiler;
import status;

export module new_catalog;

namespace infinity {

class ProfileHistory {
private:
    Mutex lock_{};
    Vector<SharedPtr<QueryProfiler>> queue;
    SizeT front;
    SizeT rear;
    SizeT max_size;

public:
    ProfileHistory(SizeT size) {
        max_size = size + 1;
        queue.resize(max_size);
        front = 0;
        rear = 0;
    }

    void Enqueue(SharedPtr<QueryProfiler> &&profiler) {
        UniqueLock<Mutex> lk(lock_);
        if ((rear + 1) % max_size == front) {
            return;
        }
        queue[rear] = profiler;
        rear = (rear + 1) % max_size;
    }

    QueryProfiler *GetElement(SizeT index) {
        UniqueLock<Mutex> lk(lock_);
        if (index < 0 || index >= (rear - front + max_size) % max_size) {
            return nullptr;
        }
        SizeT actualIndex = (front + index) % max_size;
        return queue[actualIndex].get();
    }

    Vector<SharedPtr<QueryProfiler>> GetElements() {
        Vector<SharedPtr<QueryProfiler>> elements;
        elements.reserve(max_size);

        UniqueLock<Mutex> lk(lock_);
        for (SizeT i = 0; i < queue.size(); ++i) {
            if (queue[i].get() != nullptr) {
                elements.push_back(queue[i]);
            }
        }
        return elements;
    }
};

export struct NewCatalog {
public:
    explicit NewCatalog(SharedPtr<String> dir, bool create_default_db = false);

public:
    static Status
    CreateDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, BaseEntry *&db_entry);

    static Status
    DropDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, BaseEntry *&db_entry);

    static Status GetDatabase(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry *&new_db_entry);

    static void RemoveDBEntry(NewCatalog *catalog, const String &db_name, u64 txn_id, TxnManager *txn_mgr);

    static Vector<DBEntry *> Databases(NewCatalog *catalog, u64 txn_id, TxnTimeStamp begin_ts);

    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, String function_name);

    static void AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void DeleteFunctionSet(NewCatalog *catalog, String function_name);

    // Table Function related methods
    static SharedPtr<TableFunction> GetTableFunctionByName(NewCatalog *catalog, String function_name);

    static void AddTableFunction(NewCatalog *catalog, const SharedPtr<TableFunction> &table_function);

    static void AddSpecialFunction(NewCatalog *catalog, const SharedPtr<SpecialFunction> &special_function);

    static SharedPtr<SpecialFunction> GetSpecialFunctionByNameNoExcept(NewCatalog *catalog, String function_name);

    static void DeleteTableFunction(NewCatalog *catalog, String function_name);

    static Json Serialize(NewCatalog *catalog, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static void Deserialize(const Json &catalog_json, BufferManager *buffer_mgr, UniquePtr<NewCatalog> &catalog);

    static UniquePtr<NewCatalog> LoadFromFiles(const Vector<String> &catalog_paths, BufferManager *buffer_mgr);

    static UniquePtr<NewCatalog> LoadFromFile(const String &catalog_path, BufferManager *buffer_mgr);

    static String SaveAsFile(NewCatalog *catalog_ptr, const String &dir, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    void MergeFrom(NewCatalog &other);

    void AppendProfilerRecord(SharedPtr<QueryProfiler> profiler) { history.Enqueue(Move(profiler)); }

    const QueryProfiler *GetProfilerRecord(SizeT index) { return history.GetElement(index); }

    const Vector<SharedPtr<QueryProfiler>> GetProfilerRecords() { return history.GetElements(); }

    void CheckCatalog();

public:
    SharedPtr<String> current_dir_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    u64 next_txn_id_{};
    u64 catalog_version_{};
    RWMutex rw_locker_;

    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_;
    HashMap<String, SharedPtr<TableFunction>> table_functions_;
    HashMap<String, SharedPtr<SpecialFunction>> special_functions_;

    ProfileHistory history{128};
};

} // namespace infinity
