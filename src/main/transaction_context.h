//
// Created by JinHai on 2022/9/5.
//

#pragma once

namespace infinity {

class Txn;

class TransactionContext {
public:
    void BeginTxn();

    void CommitTxn();

    void RollbackTxn();

    void ClearTxn();

    void SetAutoCommit(bool auto_commit);

    inline bool IsAutoCommit() const { return auto_commit_; }

private:
    bool auto_commit_;
    Txn *current_txn_;
};

} // namespace infinity
