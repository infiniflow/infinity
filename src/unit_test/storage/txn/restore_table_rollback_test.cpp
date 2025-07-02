

#include "gtest/gtest.h"
import base_test;

import infinity_context;
import stl;
import status;
import txn_state;
import new_txn_manager;
import new_txn;
import snapshot_info;
import data_type;
import logical_type;
import column_def;
import table_def;
import extra_ddl_info;


using namespace infinity;

class RestoreTableRollbackTest : public BaseTestNoParam {};

TEST_F(RestoreTableRollbackTest, test_restore_table_rollback_basic) {
    using namespace infinity;
    NewTxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();

    // Create database first
    NewTxn *create_db_txn = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
    Status status = create_db_txn->CreateDatabase("test_db", ConflictType::kError, MakeShared<String>());
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(create_db_txn);

    // Create a table to snapshot
    NewTxn *create_table_txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
    
    Vector<SharedPtr<ColumnDef>> columns;
    {
        SharedPtr<DataType> col_type = MakeShared<DataType>(LogicalType::kInteger);
        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0, col_type, "col1", std::set<ConstraintType>());
        columns.emplace_back(col_def);
    }
    
    SharedPtr<TableDef> table_def = MakeShared<TableDef>(MakeShared<String>("test_db"), 
                                                        MakeShared<String>("test_table"), 
                                                        MakeShared<String>(""), 
                                                        columns);
    
    status = create_table_txn->CreateTable("test_db", table_def, ConflictType::kError);
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(create_table_txn);

    // Create snapshot
    NewTxn *snapshot_txn = txn_mgr->BeginTxn(MakeUnique<String>("create snapshot"), TransactionType::kNormal);
    SharedPtr<TableSnapshotInfo> table_snapshot;
    std::tie(table_snapshot, status) = snapshot_txn->GetTableSnapshotInfo("test_db", "test_table");
    EXPECT_TRUE(status.ok());
    
    table_snapshot->snapshot_name_ = "test_snapshot";
    String snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    status = table_snapshot->Serialize(snapshot_dir, txn_mgr->GetReadCommitTS(snapshot_txn));
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(snapshot_txn);

    // Drop the original table
    NewTxn *drop_table_txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"), TransactionType::kNormal);
    status = drop_table_txn->DropTable("test_db", "test_table", ConflictType::kError);
    EXPECT_TRUE(status.ok());
    txn_mgr->CommitTxn(drop_table_txn);

    // Start restore table transaction
    NewTxn *restore_txn = txn_mgr->BeginTxn(MakeUnique<String>("restore table"), TransactionType::kRestoreTable);
    
    // Verify transaction type is set correctly
    // EXPECT_EQ(restore_txn->GetTxnType(), TransactionType::kRestoreTable);
    
    // Attempt to restore table
    status = restore_txn->RestoreTableSnapshot("test_db", table_snapshot);
    EXPECT_TRUE(status.ok());

    NewTxn *check_txn = txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);
    auto [table_info, check_status] = check_txn->GetTableInfo("test_db", "test_table");
    EXPECT_TRUE(check_status.ok()); // Table should exist after rollback
    txn_mgr->CommitTxn(check_txn);
    
    // Verify transaction store is created - Note: base_txn_store_ is private, so we can't access it directly
    // EXPECT_NE(restore_txn->base_txn_store_, nullptr);
    // EXPECT_EQ(restore_txn->base_txn_store_->GetTxnType(), TransactionType::kRestoreTable);
    
    // Now rollback the transaction
    status = restore_txn->Rollback();
    EXPECT_TRUE(status.ok());
    
    // Verify transaction state after rollback
    // EXPECT_EQ(restore_txn->GetTxnState(), TxnState::kRollbacked);
    
    // Verify that the table was not actually created (rollback worked)
    NewTxn *check_txn1 = txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);
    auto [table_info1, check_status1] = check_txn1->GetTableInfo("test_db", "test_table");
    EXPECT_FALSE(check_status1.ok()); // Table should not exist after rollback
    txn_mgr->CommitTxn(check_txn1);
    
    // Note: DestroyTxn doesn't exist in NewTxnManager, transactions are managed automatically
}

