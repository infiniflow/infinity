//
// Created by jinhai on 23-6-4.
//

#include "txn.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"
#include "common/utility/defer_op.h"

namespace infinity {

EntryResult
Txn::CreateDatabase(const String& db_name) {

    TxnTimeStamp begin_ts;
    TxnState txn_state;
    {
        txn_context_.RLock();
        DeferFn defer_fn([&]() {
            txn_context_.RUnLock();
        });
        begin_ts = txn_context_.begin_ts_;
        txn_state = txn_context_.state_;
    }

    if(txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.")
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    EntryResult res = catalog_->CreateDatabase(db_name, this->txn_id_, begin_ts, &txn_context_);
    if(res.entry_ == nullptr) {
        return res;
    }

    if(res.entry_->entry_type_ != EntryType::kDatabase) {
        return {nullptr, MakeUnique<String>("Invalid database type")};
    }

    auto* db_entry = static_cast<DBEntry*>(res.entry_);
    txn_dbs_.insert(db_entry);
    db_names_.insert(db_name);
    return res;
}

EntryResult
Txn::DropDatabase(const String& db_name) {

    TxnTimeStamp begin_ts;
    TxnState txn_state;
    {
        txn_context_.RLock();
        DeferFn defer_fn([&]() {
            txn_context_.RUnLock();
        });
        begin_ts = txn_context_.begin_ts_;
        txn_state = txn_context_.state_;
    }

    if(txn_state != TxnState::kStarted) {
        LOG_TRACE("Transaction isn't started.")
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    EntryResult res = catalog_->DropDatabase(db_name, txn_id_, begin_ts, &txn_context_);

    if(res.entry_ == nullptr) {
        return res;
    }

    DBEntry* dropped_db_entry = static_cast<DBEntry*>(res.entry_);

    if(txn_dbs_.contains(dropped_db_entry)) {
        txn_dbs_.erase(dropped_db_entry);
    } else {
        txn_dbs_.insert(dropped_db_entry);
    }

    if(db_names_.contains(db_name)) {
        db_names_.erase(db_name);
    } else {
        db_names_.insert(db_name);
    }

    return res;
}

EntryResult
Txn::GetDatabase(const String& db_name) {
    TxnTimeStamp begin_ts;
    TxnState txn_state;
    {
        txn_context_.RLock();
        DeferFn defer_fn([&]() {
            txn_context_.RUnLock();
        });
        begin_ts = txn_context_.begin_ts_;
        txn_state = txn_context_.state_;
    }

    if(txn_state != TxnState::kStarted) {
        StorageError("Transaction isn't in STARTED status.")
    }

    return catalog_->GetDatabase(db_name, this->txn_id_, begin_ts);
}

void
Txn::BeginTxn(TxnTimeStamp begin_ts) {
    txn_context_.Lock();
    DeferFn defer_fn([&]() {
        txn_context_.UnLock();
    });

    if(txn_context_.state_ != TxnState::kNotStarted) {
        StorageError("Transaction isn't in NOT_STARTED status.")
    }
    txn_context_.begin_ts_ = begin_ts;
    txn_context_.state_ = TxnState::kStarted;
}

void
Txn::CommitTxn(TxnTimeStamp commit_ts) {
    {
        txn_context_.Lock();
        DeferFn defer_fn([&]() {
            txn_context_.UnLock();
        });
        if(txn_context_.state_ != TxnState::kStarted) {
            StorageError("Transaction isn't in STARTED status.")
        }
        txn_context_.state_ = TxnState::kCommitting;
    }

    for(auto* db_entry: txn_dbs_) {
        db_entry->Commit(commit_ts);
    }

    {
        txn_context_.Lock();
        DeferFn defer_fn([&]() {
            txn_context_.UnLock();
        });
        if(txn_context_.state_ != TxnState::kCommitting) {
            StorageError("Transaction isn't in COMMITTING status.")
        }
        txn_context_.state_ = TxnState::kCommitted;
    }
}

void
Txn::RollbackTxn(TxnTimeStamp abort_ts) {

    {
        txn_context_.Lock();
        DeferFn defer_fn([&]() {
            txn_context_.UnLock();
        });
        if(txn_context_.state_ == TxnState::kStarted) {
            txn_context_.state_ = TxnState::kRollbacking;
        } else if(txn_context_.state_ == TxnState::kRollbacking) {
            ;
        } else {
            StorageError("Transaction isn't in STARTED or ROLLBACKING status.")
        }
    }


    for(const auto& db_name: db_names_) {
        catalog_->RemoveDBEntry(db_name, this->txn_id_, &txn_context_);
    }

    {
        txn_context_.Lock();
        DeferFn defer_fn([&]() {
            txn_context_.UnLock();
        });
        if(txn_context_.state_ != TxnState::kRollbacking) {
            StorageError("Transaction isn't in ROLLBACKING status.")
        }
        txn_context_.state_ = TxnState::kRollbacked;
    }
}

}
