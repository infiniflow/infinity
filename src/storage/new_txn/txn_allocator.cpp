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

module txn_allocator;

import stl;
import txn_allocator_task;
import new_txn;
import logger;
import storage;
import third_party;
import base_txn_store;
import txn_state;
import infinity_exception;
import new_catalog;
import catalog_cache;
import status;

namespace infinity {

TxnAllocator::TxnAllocator(Storage *storage) : storage_(storage) {}
TxnAllocator::~TxnAllocator() = default;

void TxnAllocator::SetSystemCache(const SharedPtr<SystemCache> &system_cache) { system_cache_ = system_cache; }

void TxnAllocator::Start() {
    processor_thread_ = Thread([this] { Process(); });
    LOG_INFO("Transaction allocator is started.");
}

void TxnAllocator::Stop() {
    LOG_INFO("Transaction allocator is stopping.");
    SharedPtr<TxnAllocatorTask> stop_task = MakeShared<TxnAllocatorTask>(nullptr, true);
    task_queue_.Enqueue(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    system_cache_.reset();
    LOG_INFO("Transaction allocator is stopped.");
}

void TxnAllocator::Submit(SharedPtr<TxnAllocatorTask> task) {
    ++task_count_;
    task_queue_.Enqueue(std::move(task));
}

void TxnAllocator::Process() {
    bool running{true};
    Deque<SharedPtr<TxnAllocatorTask>> tasks;
    NewCatalog *catalog = storage_->new_catalog();
    while (running) {
        task_queue_.DequeueBulk(tasks);
        // LOG_INFO(fmt::format("Receive tasks: {}", tasks.size()));

        for (const auto &txn_allocator_task : tasks) {
            if (txn_allocator_task->IsStopTask()) {
                running = false;
            } else {
                NewTxn *txn = txn_allocator_task->txn_ptr();
                TransactionType txn_type = txn->GetTxnType();
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                switch (txn_type) {
                    case TransactionType::kAppend: {
                        AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(base_txn_store);
                        LOG_INFO(fmt::format("TxnAllocator: Append txn: db: {}, {}, table: {}, {}, data size: {}",
                                             append_txn_store->db_name_,
                                             append_txn_store->db_id_,
                                             append_txn_store->table_name_,
                                             append_txn_store->table_id_,
                                             append_txn_store->input_block_->row_count()));
                        SharedPtr<TableCache> table_cache = catalog->GetTableCache(append_txn_store->db_id_, append_txn_store->table_id_);
                        if (table_cache.get() == nullptr) {
                            Status status;
                            std::tie(table_cache, status) = catalog->AddNewTableCache(append_txn_store->db_id_, append_txn_store->table_id_);
                        }
                        append_txn_store->row_ranges_ = table_cache->PrepareAppendRanges(append_txn_store->input_block_->row_count(), txn->TxnID());
                        break;
                    }
                    case TransactionType::kUpdate: {
                        break;
                    }
                    case TransactionType::kImport: {
                        break;
                    }
                    case TransactionType::kDumpMemIndex: {
                        break;
                    }
                    case TransactionType::kOptimizeIndex: {
                        break;
                    }
                    case TransactionType::kCompact: {
                        break;
                    }
                    case TransactionType::kCreateDB: {
                        CreateDBTxnStore *txn_store = static_cast<CreateDBTxnStore *>(base_txn_store);
                        std::tie(txn_store->db_id_, txn_allocator_task->status_) = system_cache_->AddNewDbCache(txn_store->db_name_);
                        if (txn_allocator_task->status_.ok()) {
                            LOG_DEBUG(fmt::format("CreateDB txn: db: {}, {}", txn_store->db_name_, txn_store->db_id_));
                        } else {
                            LOG_DEBUG(txn_allocator_task->status_.message());
                        }

                        break;
                    }
                    case TransactionType::kCreateTable: {
                        CreateTableTxnStore *txn_store = static_cast<CreateTableTxnStore *>(base_txn_store);
                        txn_store->table_id_ = system_cache_->AddNewTableCache(txn_store->db_id_);
                        LOG_INFO(fmt::format("CreateTable txn: db: {}, {}, table: {}, {}",
                                             txn_store->db_name_,
                                             txn_store->db_id_,
                                             txn_store->table_name_,
                                             txn_store->table_id_));
                        break;
                    }
                    case TransactionType::kCreateIndex: {
                        break;
                    }
                    default: {
                        UnrecoverableError(fmt::format("Transaction type {} is not supported", TransactionType2Str(txn_type)));
                    }
                }
                LOG_INFO(*txn->GetTxnText());
            }
            txn_allocator_task->Complete();
        }
    }
}

} // namespace infinity
