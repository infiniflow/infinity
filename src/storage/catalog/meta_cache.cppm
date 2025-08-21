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

import std;
import :default_values;
import internal_types;
import third_party;
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

export std::string ToString(MetaCacheType type);

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
    virtual std::string name() const = 0;
    virtual u64 commit_ts() const = 0;
    virtual std::string detail() const = 0;
    virtual bool is_dropped() const = 0;

protected:
    mutable std::mutex mtx_{};
    MetaCacheType type_{MetaCacheType::kInvalid};
    bool is_dropped_{false};
    TransactionID reader_txn_id_{MAX_TXN_ID};
};

export class MetaDbCache final : public MetaBaseCache {
public:
    MetaDbCache(const std::string &db_name, u64 db_id, u64 commit_ts, const std::string &db_key, bool is_dropped, TransactionID reader_txn_id)
        : MetaBaseCache(MetaCacheType::kCreateDB, is_dropped, reader_txn_id), db_name_(db_name), db_id_(db_id), commit_ts_(commit_ts),
          db_key_(db_key) {}
    virtual ~MetaDbCache();

    std::string name() const final;
    u64 db_id() const;
    const std::string &db_key() const;
    u64 commit_ts() const final;
    std::string detail() const final;
    bool is_dropped() const final;

    void set_comment(const std::shared_ptr<std::string> &comment);
    bool get_comment() const;
    std::shared_ptr<std::string> comment() const;

private:
    std::string db_name_{};
    u64 db_id_{};
    u64 commit_ts_{};
    std::string db_key_{};

    bool get_comment_{false};
    std::shared_ptr<std::string> comment_{};

    bool get_tables_{false};
    std::vector<std::string> table_ids_{};
    std::vector<std::string> table_names_{};
};

export class MetaTableCache final : public MetaBaseCache {
public:
    MetaTableCache(u64 db_id,
                   const std::string &table_name,
                   u64 table_id,
                   u64 commit_ts,
                   const std::string &table_key,
                   bool is_dropped,
                   TransactionID reader_txn_id)
        : MetaBaseCache(MetaCacheType::kCreateTable, is_dropped, reader_txn_id), db_id_(db_id), table_name_(table_name), table_id_(table_id),
          commit_ts_(commit_ts), table_key_(table_key) {}
    ~MetaTableCache() override = default;

    std::string name() const final;
    u64 db_id() const;
    u64 table_id() const;
    const std::string &table_key() const;
    u64 commit_ts() const final;
    std::string detail() const final;
    bool is_dropped() const final;

    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> get_columns() const;
    void set_columns(const std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> &columns);

    std::shared_ptr<std::vector<SegmentID>> get_segments() const;
    void set_segments(const std::shared_ptr<std::vector<SegmentID>> &segments);

    std::tuple<std::shared_ptr<std::vector<std::string>>, std::shared_ptr<std::vector<std::string>>> get_index_ids() const;
    void set_index_ids(const std::shared_ptr<std::vector<std::string>> &index_names_ptr,
                       const std::shared_ptr<std::vector<std::string>> &index_ids_ptr);

private:
    u64 db_id_{};
    std::string table_name_{};
    u64 table_id_{};
    u64 commit_ts_{};
    std::string table_key_{};
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> columns_{};
    std::shared_ptr<std::vector<SegmentID>> segment_ids_{};
    std::shared_ptr<std::vector<std::string>> index_names_ptr_{};
    std::shared_ptr<std::vector<std::string>> index_ids_ptr_{};
};

export class MetaIndexCache final : public MetaBaseCache {
public:
    MetaIndexCache(u64 db_id,
                   u64 table_id,
                   const std::string &index_name,
                   u64 index_id,
                   u64 commit_ts,
                   const std::string &index_key,
                   bool is_dropped,
                   TransactionID reader_txn_id)
        : MetaBaseCache(MetaCacheType::kCreateIndex, is_dropped, reader_txn_id), db_id_(db_id), table_id_(table_id), index_name_(index_name),
          index_id_(index_id), index_key_(index_key), commit_ts_(commit_ts) {};
    ~MetaIndexCache() override = default;

    std::string name() const final;
    u64 db_id() const;
    u64 table_id() const;
    u64 commit_ts() const final;
    std::string detail() const final;
    bool is_dropped() const final;

private:
    u64 db_id_{};
    u64 table_id_{};
    std::string index_name_{};
    u64 index_id_{};
    std::string index_key_{};
    u64 commit_ts_{};
};

export struct EraseBaseCache {
    explicit EraseBaseCache(EraseCacheType type) : type_(type) {}
    virtual ~EraseBaseCache() = default;
    EraseCacheType type_{EraseCacheType::kInvalid};
};

export struct MetaEraseDbCache : public EraseBaseCache {
    explicit MetaEraseDbCache(const std::string &db_name) : EraseBaseCache(EraseCacheType::kEraseDB), db_name_(db_name) {}
    std::string db_name_{};
};

export struct MetaEraseTableCache : public EraseBaseCache {
    explicit MetaEraseTableCache(u64 db_id, const std::string &table_name)
        : EraseBaseCache(EraseCacheType::kEraseTable), db_id_(db_id), table_name_(table_name) {}
    u64 db_id_{};
    std::string table_name_{};
};

export struct MetaEraseIndexCache : public EraseBaseCache {
    explicit MetaEraseIndexCache(u64 db_id, u64 table_id, const std::string &index_name)
        : EraseBaseCache(EraseCacheType::kEraseIndex), db_id_(db_id), table_id_(table_id), index_name_(index_name) {}
    u64 db_id_{};
    u64 table_id_{};
    std::string index_name_{};
};

struct CacheItem {
    std::string name_;
    std::shared_ptr<MetaBaseCache> meta_cache_;
};

struct TableNameID {
    std::string name_{};
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
    std::unordered_map<std::string, std::map<u64, std::list<CacheItem>::iterator>> dbs_;     // db_name -> (commit_ts -> MetaDbCache)
    std::unordered_map<std::string, std::map<u64, std::list<CacheItem>::iterator>> tables_;  // table_name -> (commit_ts -> MetaTableCache)
    std::unordered_map<std::string, std::map<u64, std::list<CacheItem>::iterator>> indexes_; // index_name -> (commit_ts -> MetaIndexCache)

    u64 db_request_count_{};
    u64 table_request_count_{};
    u64 index_request_count_{};

    u64 db_hit_count_{};
    u64 table_hit_count_{};
    u64 index_hit_count_{};

    size_t capacity_{0};
    std::list<CacheItem> lru_{};
    TxnTimeStamp latest_erased_ts_{};

public:
    explicit MetaCache(size_t capacity) : capacity_(capacity) {};

    void Put(const std::vector<std::shared_ptr<MetaBaseCache>> &cache_items, TxnTimeStamp begin_ts);

    Status Erase(const std::vector<std::shared_ptr<EraseBaseCache>> &cache_items, KVInstance *kv_instance, TxnTimeStamp commit_ts);

    Status PutOrErase(const std::vector<std::shared_ptr<MetaBaseCache>> &cache_items, KVInstance *kv_instance);

    std::shared_ptr<MetaDbCache> GetDb(const std::string &db_name, TxnTimeStamp begin_ts);

    std::shared_ptr<MetaTableCache> GetTable(u64 db_id, const std::string &table_name, TxnTimeStamp begin_ts);

    std::shared_ptr<MetaIndexCache> GetIndex(u64 db_id, u64 table_id, const std::string &index_name, TxnTimeStamp begin_ts);

    CacheStatus GetCacheStatus(MetaCacheType type) const;

    void PrintLRU() const;

    size_t Size() const;

    std::vector<std::shared_ptr<MetaBaseCache>> GetAllCacheItems() const;

private:
    void PutOrEraseNolock(const std::shared_ptr<MetaBaseCache> &meta_base_cache);
    void PutDbNolock(const std::shared_ptr<MetaDbCache> &db_cache);
    void EraseDbNolock(const std::string &db_name);
    void PutTableNolock(const std::shared_ptr<MetaTableCache> &table_cache);
    void EraseTableNolock(u64 db_id, const std::string &table_name);
    void PutIndexNolock(const std::shared_ptr<MetaIndexCache> &index_cache);
    void EraseIndexNolock(u64 db_id, u64 table_id, const std::string &index_name);
    void TrimCacheNolock();
    void TouchNolock(std::list<CacheItem>::iterator iter);
};

} // namespace infinity
