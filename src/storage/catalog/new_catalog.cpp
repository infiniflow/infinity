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

#include <string>

module new_catalog;

import stl;
import new_txn;
import status;
import extra_ddl_info;
import kv_store;
import third_party;
import logger;
import infinity_exception;
import default_values;
import mem_index;
import column_index_reader;
import meta_key;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

NewCatalog::~NewCatalog() = default;

String NewCatalog::GetPathNameTail(const String &path) {
    SizeT delimiter_i = path.rfind('/');
    if (delimiter_i == String::npos) {
        return path;
    }
    return path.substr(delimiter_i + 1);
}

Status NewCatalog::LockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kImmutable: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in immutable", table_key));
        }
        case LockType::kUnlocked: {
            table_memory_context->locker = LockType::kLocking;
            break;
        }
    }
    table_memory_context->locked_txn_ = txn_id;
    return Status::OK();
}

Status NewCatalog::CommitLockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is locked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kLocking) {
        table_memory_context->locker = LockType::kLocked;
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in locking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::RollbackLockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is locked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kLocking) {
        table_memory_context_map_.erase(table_key);
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in locking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::UnlockTable(const String &table_key, TransactionID txn_id) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {}", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            table_memory_context->locker = LockType::kUnlocking;
            break;
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kUnlocked: {
            return Status::NotLocked(fmt::format("Table key: {} wasn't locked before", table_key));
        }
        case LockType::kImmutable: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in immutable", table_key));
        }
    }
    table_memory_context->locked_txn_ = txn_id;
    return Status::OK();
}

Status NewCatalog::CommitUnlockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is being unlocked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kUnlocking) {
        table_memory_context_map_.erase(table_key);
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in unlocking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::RollbackUnlockTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {} not found in table locker", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is being unlocked by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    if (table_memory_context->locker == LockType::kUnlocking) {
        table_memory_context->locker = LockType::kLocked;
    } else {
        return Status::UnexpectedError(fmt::format("Table key: {} isn't in unlocking state.", table_key));
    }
    return Status::OK();
}

Status NewCatalog::ImmutateTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kImmutable: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in immutable", table_key));
        }
        case LockType::kUnlocked: {
            table_memory_context->locker = LockType::kImmutable;
            break;
        }
    }

    table_memory_context->locked_txn_ = txn_id;
    return Status::OK();
}

Status NewCatalog::MutateTable(const String &table_key, TransactionID txn_id) {
    std::unique_lock lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    SizeT write_txn_num = table_memory_context->write_txn_num_;
    if (write_txn_num > 0) {
        return Status::TableIsUsing(fmt::format("Table key: {} is using write transactions, count: {}", table_key, write_txn_num));
    }

    if (txn_id != table_memory_context->locked_txn_) {
        return Status::UnexpectedError(
            fmt::format("Table key: {} is immutated by txn: {}, but not: {}", table_key, table_memory_context->locked_txn_, txn_id));
    }

    switch (table_memory_context->locker) {
        case LockType::kLocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
        }
        case LockType::kLocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is locking", table_key));
        }
        case LockType::kUnlocking: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is unlocking", table_key));
        }
        case LockType::kImmutable: {
            table_memory_context_map_.erase(table_key);
            break;
        }
        case LockType::kUnlocked: {
            return Status::AlreadyLocked(fmt::format("Table key: {} is in unlocked state", table_key));
        }
    }

    return Status::OK();
}

Status NewCatalog::IncreaseTableWriteCount(const String &table_key) {
    std::lock_guard lock(mtx_);
    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        table_memory_context_map_[table_key] = MakeShared<TableMemoryContext>();
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    if (table_memory_context->locker != LockType::kUnlocked) {
        return Status::AlreadyLocked(fmt::format("Table key: {} is already locked", table_key));
    }

    ++table_memory_context->write_txn_num_;
    return Status::OK();
}

Status NewCatalog::DecreaseTableWriteCount(const String &table_key) {
    std::lock_guard lock(mtx_);

    auto iter = table_memory_context_map_.find(table_key);
    if (iter == table_memory_context_map_.end()) {
        return Status::NotFound(fmt::format("Table key: {}", table_key));
    }

    TableMemoryContext *table_memory_context = table_memory_context_map_[table_key].get();
    if (table_memory_context->locker != LockType::kUnlocked) {
        UnrecoverableError(fmt::format("Table key: {} is already locked", table_key));
    }

    SizeT &write_txn_num = iter->second->write_txn_num_;
    if (write_txn_num > 0) {
        --write_txn_num;
    }

    if (write_txn_num == 0) {
        table_memory_context_map_.erase(table_key);
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(String block_key) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>());
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(String block_key, TxnTimeStamp checkpoint_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>(checkpoint_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetBlockLock(const String &block_key, SharedPtr<BlockLock> &block_lock) {
    block_lock = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(block_lock_mtx_);
        if (auto iter = block_lock_map_.find(block_key); iter != block_lock_map_.end()) {
            block_lock = iter->second;
        }
    }
    if (block_lock == nullptr) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::DropBlockLockByBlockKey(const String &block_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(block_lock_mtx_);
        delete_success = block_lock_map_.erase(block_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::AddMemIndex(String mem_index_key, SharedPtr<MemIndex> mem_index) {
    bool insert_success = false;
    HashMap<String, SharedPtr<MemIndex>>::iterator iter;
    {
        std::unique_lock lock(mem_index_mtx_);
        std::tie(iter, insert_success) = mem_index_map_.emplace(std::move(mem_index_key), std::move(mem_index));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("MemIndex key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetMemIndex(const String &mem_index_key, SharedPtr<MemIndex> &mem_index) {
    mem_index = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(mem_index_mtx_);
        if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
            mem_index = iter->second;
        }
    }
    if (mem_index == nullptr) {
        return Status::CatalogError(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Status NewCatalog::DropMemIndexByMemIndexKey(const String &mem_index_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(mem_index_mtx_);
        delete_success = mem_index_map_.erase(mem_index_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Status NewCatalog::AddFtIndexCache(String ft_index_cache_key, SharedPtr<TableIndexReaderCache> ft_index_cache) {
    bool insert_success = false;
    HashMap<String, SharedPtr<TableIndexReaderCache>>::iterator iter;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        std::tie(iter, insert_success) = ft_index_cache_map_.emplace(std::move(ft_index_cache_key), std::move(ft_index_cache));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetFtIndexCache(const String &ft_index_cache_key, SharedPtr<TableIndexReaderCache> &ft_index_cache) {
    ft_index_cache = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(ft_index_cache_mtx_);
        if (auto iter = ft_index_cache_map_.find(ft_index_cache_key); iter != ft_index_cache_map_.end()) {
            ft_index_cache = iter->second;
        }
    }
    if (ft_index_cache == nullptr) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::DropFtIndexCacheByFtIndexCacheKey(const String &ft_index_cache_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        delete_success = ft_index_cache_map_.erase(ft_index_cache_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

void NewCatalog::AddCleanedMeta(TxnTimeStamp ts, UniquePtr<MetaKey> meta) {
    std::unique_lock lock(cleaned_meta_mtx_);

    cleaned_meta_.emplace(ts, std::move(meta));
}

void NewCatalog::GetCleanedMeta(TxnTimeStamp ts, Vector<UniquePtr<MetaKey>> &metas) {
    std::unique_lock lock(cleaned_meta_mtx_);

    auto iter = cleaned_meta_.lower_bound(ts);
    for (auto it = cleaned_meta_.begin(); it != iter; ++it) {
        metas.push_back(std::move(it->second));
    }
    cleaned_meta_.erase(cleaned_meta_.begin(), iter);
}

} // namespace infinity
