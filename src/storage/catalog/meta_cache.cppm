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

export module infinity_core:meta_cache;

import :stl;
import :default_values;
import internal_types;
import :third_party;
import :status;
import column_def;

namespace infinity {

class KVInstance;

export enum class MetaCacheType {
    kInvalid,
    kCreateDB,
    kCreateTable,
    kCreateIndex,
};

export String ToString(MetaCacheType type);

export enum class EraseCacheType {
    kInvalid,
    kEraseDB,
    kEraseTable,
    kEraseIndex,
};

class MetaBaseCache {
public:
    explicit MetaBaseCache(MetaCacheType type, bool is_dropped, TransactionID reader_txn_id)
        : type_(type), is_dropped_(is_dropped), reader_txn_id_(reader_txn_id) {}
    virtual ~MetaBaseCache() = default;

    MetaCacheType type() const;
    virtual String name() const = 0;
    virtual u64 commit_ts() const = 0;
    virtual String detail() const = 0;
    virtual bool is_dropped() const = 0;

protected:
    mutable std::mutex mtx_{};
    MetaCacheType type_{MetaCacheType::kInvalid};
    bool is_dropped_{false};
    TransactionID reader_txn_id_{MAX_TXN_ID};
};

export class MetaDbCache final : public MetaBaseCache {
public:
    MetaDbCache(const String &db_name, u64 db_id, u64 commit_ts, const String &db_key, bool is_dropped, TransactionID reader_txn_id)
        : MetaBaseCache(MetaCacheType::kCreateDB, is_dropped, reader_txn_id), db_name_(db_name), db_id_(db_id), commit_ts_(commit_ts),
          db_key_(db_key) {}
    ~MetaDbCache() final = default;

    String name() const final;
    u64 db_id() const;
    const String &db_key() const;
    u64 commit_ts() const final;
    String detail() const final;
    bool is_dropped() const final;

    void set_comment(const SharedPtr<String> &comment);
    bool get_comment() const;
    SharedPtr<String> comment() const;

private:
    String db_name_{};
    u64 db_id_{};
    u64 commit_ts_{};
    String db_key_{};

    bool get_comment_{false};
    SharedPtr<String> comment_{};

    bool get_tables_{false};
    Vector<String> table_ids_{};
    Vector<String> table_names_{};
};

export class MetaTableCache final : public MetaBaseCache {
public:
    MetaTableCache(u64 db_id,
                   const String &table_name,
                   u64 table_id,
                   u64 commit_ts,
                   const String &table_key,
                   bool is_dropped,
                   TransactionID reader_txn_id)
        : MetaBaseCache(MetaCacheType::kCreateTable, is_dropped, reader_txn_id), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          commit_ts_(commit_ts), table_key_(table_key) {}
    ~MetaTableCache() final = default;

    String name() const final;
    u64 db_id() const;
    u64 table_id() const;
    const String &table_key() const;
    u64 commit_ts() const final;
    String detail() const final;
    bool is_dropped() const final;

    SharedPtr<Vector<SharedPtr<ColumnDef>>> get_columns() const;
    void set_columns(const SharedPtr<Vector<SharedPtr<ColumnDef>>> &columns);

    SharedPtr<Vector<SegmentID>> get_segments() const;
    void set_segments(const SharedPtr<Vector<SegmentID>> &segments);

    Tuple<SharedPtr<Vector<String>>, SharedPtr<Vector<String>>> get_index_ids() const;
    void set_index_ids(const SharedPtr<Vector<String>> &index_names_ptr, const SharedPtr<Vector<String>> &index_ids_ptr);

private:
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};
    u64 commit_ts_{};
    String table_key_{};
    SharedPtr<Vector<SharedPtr<ColumnDef>>> columns_{};
    SharedPtr<Vector<SegmentID>> segment_ids_{};
    SharedPtr<Vector<String>> index_names_ptr_{};
    SharedPtr<Vector<String>> index_ids_ptr_{};
};

export class MetaIndexCache final : public MetaBaseCache {
public:
    MetaIndexCache(u64 db_id,
                   u64 table_id,
                   const String &index_name,
                   u64 index_id,
                   u64 commit_ts,
                   const String &index_key,
                   bool is_dropped,
                   TransactionID reader_txn_id)
        : MetaBaseCache(MetaCacheType::kCreateIndex, is_dropped, reader_txn_id), db_id_(db_id), table_id_(table_id), index_name_(index_name),
          index_id_(index_id), index_key_(index_key), commit_ts_(commit_ts) {};
    ~MetaIndexCache() final = default;

    String name() const final;
    u64 db_id() const;
    u64 table_id() const;
    u64 commit_ts() const final;
    String detail() const final;
    bool is_dropped() const final;

private:
    u64 db_id_{};
    u64 table_id_{};
    String index_name_{};
    u64 index_id_{};
    String index_key_{};
    u64 commit_ts_{};
};

export struct EraseBaseCache {
    explicit EraseBaseCache(EraseCacheType type) : type_(type) {}
    virtual ~EraseBaseCache() = default;
    EraseCacheType type_{EraseCacheType::kInvalid};
};

export struct MetaEraseDbCache : public EraseBaseCache {
    explicit MetaEraseDbCache(const String &db_name) : EraseBaseCache(EraseCacheType::kEraseDB), db_name_(db_name) {}
    String db_name_{};
};

export struct MetaEraseTableCache : public EraseBaseCache {
    explicit MetaEraseTableCache(u64 db_id, const String &table_name)
        : EraseBaseCache(EraseCacheType::kEraseTable), db_id_(db_id), table_name_(table_name) {}
    u64 db_id_{};
    String table_name_{};
};

export struct MetaEraseIndexCache : public EraseBaseCache {
    explicit MetaEraseIndexCache(u64 db_id, u64 table_id, const String &index_name)
        : EraseBaseCache(EraseCacheType::kEraseIndex), db_id_(db_id), table_id_(table_id), index_name_(index_name) {}
    u64 db_id_{};
    u64 table_id_{};
    String index_name_{};
};

struct CacheItem {
    String name_;
    SharedPtr<MetaBaseCache> meta_cache_;
};

struct TableNameID {
    String name_{};
    TxnTimeStamp commit_ts_{};
    MetaCacheType meta_type_{MetaCacheType::kInvalid};
};

export struct CacheStatus {
    u64 item_count_;
    u64 request_count_;
    u64 hit_count_;
};

export class MetaCache {
private:
    mutable std::mutex cache_mtx_{};
    HashMap<String, Map<u64, List<CacheItem>::iterator>> dbs_;     // db_name -> (commit_ts -> MetaDbCache)
    HashMap<String, Map<u64, List<CacheItem>::iterator>> tables_;  // table_name -> (commit_ts -> MetaTableCache)
    HashMap<String, Map<u64, List<CacheItem>::iterator>> indexes_; // index_name -> (commit_ts -> MetaIndexCache)

    u64 db_request_count_{};
    u64 table_request_count_{};
    u64 index_request_count_{};

    u64 db_hit_count_{};
    u64 table_hit_count_{};
    u64 index_hit_count_{};

    SizeT capacity_{0};
    List<CacheItem> lru_{};
    TxnTimeStamp latest_erased_ts_{};

public:
    explicit MetaCache(SizeT capacity) : capacity_(capacity) {};

    void Put(const Vector<SharedPtr<MetaBaseCache>> &cache_items, TxnTimeStamp begin_ts);

    Status Erase(const Vector<SharedPtr<EraseBaseCache>> &cache_items, KVInstance *kv_instance, TxnTimeStamp commit_ts);

    Status PutOrErase(const Vector<SharedPtr<MetaBaseCache>> &cache_items, KVInstance *kv_instance);

    SharedPtr<MetaDbCache> GetDb(const String &db_name, TxnTimeStamp begin_ts);

    SharedPtr<MetaTableCache> GetTable(u64 db_id, const String &table_name, TxnTimeStamp begin_ts);

    SharedPtr<MetaIndexCache> GetIndex(u64 db_id, u64 table_id, const String &index_name, TxnTimeStamp begin_ts);

    CacheStatus GetCacheStatus(MetaCacheType type) const;

    void PrintLRU() const;

    SizeT Size() const;

    Vector<SharedPtr<MetaBaseCache>> GetAllCacheItems() const;

private:
    void PutOrEraseNolock(const SharedPtr<MetaBaseCache> &meta_base_cache);
    void PutDbNolock(const SharedPtr<MetaDbCache> &db_cache);
    void EraseDbNolock(const String &db_name);
    void PutTableNolock(const SharedPtr<MetaTableCache> &table_cache);
    void EraseTableNolock(u64 db_id, const String &table_name);
    void PutIndexNolock(const SharedPtr<MetaIndexCache> &index_cache);
    void EraseIndexNolock(u64 db_id, u64 table_id, const String &index_name);
    void TrimCacheNolock();
    void TouchNolock(List<CacheItem>::iterator iter);
};

} // namespace infinity
