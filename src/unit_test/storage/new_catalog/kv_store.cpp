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

#include "gtest/gtest.h"

import base_test;
import stl;
import third_party;
import status;
import kv_store;

using namespace infinity;

class KVStoreTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         KVStoreTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(KVStoreTest, kv_store1) {
    using namespace infinity;

    // Test multi-version
    UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
    kv_store->Init("/tmp/rocksdb_transaction_example");
    UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

    std::string value;

    Status status = kv_instance1->Get("abc", value);
    EXPECT_FALSE(status.ok());

    status = kv_instance1->Put("abc", "def");
    EXPECT_TRUE(status.ok());

    UniquePtr<KVInstance> kv_instance2 = kv_store->GetInstance();

    status = kv_instance2->Get("abc", value);
    EXPECT_FALSE(status.ok());

    status = kv_instance2->Put("xyz", "zzz");
    EXPECT_TRUE(status.ok());

    status = kv_instance2->Put("abc", "def");
    EXPECT_FALSE(status.ok()); // transaction conflict before commit

    status = kv_instance2->Get("xyz", value);
    EXPECT_TRUE(status.ok());
    EXPECT_STREQ(value.c_str(), "zzz");

    UniquePtr<KVInstance> kv_instance4 = kv_store->GetInstance();

    status = kv_instance1->Commit();
    EXPECT_TRUE(status.ok());

    status = kv_instance2->Commit();
    EXPECT_TRUE(status.ok());

    status = kv_instance4->Put("abc", "xxx");
    EXPECT_FALSE(status.ok()); // transaction conflict after commit

    status = kv_instance4->Rollback();
    EXPECT_TRUE(status.ok());

    UniquePtr<KVInstance> kv_instance3 = kv_store->GetInstance();

    status = kv_instance3->Get("xyz", value);
    EXPECT_TRUE(status.ok());
    EXPECT_STREQ(value.c_str(), "zzz");

    status = kv_instance3->Get("abc", value);
    EXPECT_TRUE(status.ok());
    EXPECT_STREQ(value.c_str(), "def");

    status = kv_instance3->Commit();
    EXPECT_TRUE(status.ok());

    status = kv_store->Uninit();
    EXPECT_TRUE(status.ok());
    status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
    EXPECT_TRUE(status.ok());
}

TEST_P(KVStoreTest, kv_store2) {
    using namespace infinity;

    // Test disable WAL
    {
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        kv_store->Init("/tmp/rocksdb_transaction_example");
        UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

        std::string value;

        Status status = kv_instance1->Get("abc", value);
        EXPECT_FALSE(status.ok());

        status = kv_instance1->Put("abc", "def");
        EXPECT_TRUE(status.ok());

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());

        kv_store = MakeUnique<KVStore>();
        kv_store->Init("/tmp/rocksdb_transaction_example");
        kv_instance1 = kv_store->GetInstance();

        status = kv_instance1->Get("abc", value);
        EXPECT_FALSE(status.ok());

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
        EXPECT_TRUE(status.ok());
    }

    // Test disable WAL with Flush
    {
        {
            UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
            kv_store->Init("/tmp/rocksdb_transaction_example");
            UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

            std::string value;

            Status status = kv_instance1->Get("abc", value);
            EXPECT_FALSE(status.ok());

            status = kv_instance1->Put("abc", "def");
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Commit();
            EXPECT_TRUE(status.ok());

            status = kv_store->Flush();
            EXPECT_TRUE(status.ok());

            status = kv_store->Uninit();
            EXPECT_TRUE(status.ok());
        }

        {
            UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
            kv_store->Init("/tmp/rocksdb_transaction_example");
            UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

            std::string value;

            Status status = kv_instance1->Get("abc", value);
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Commit();
            EXPECT_TRUE(status.ok());

            status = kv_store->Uninit();
            EXPECT_TRUE(status.ok());

            status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
            EXPECT_TRUE(status.ok());
        }
    }
}

TEST_P(KVStoreTest, kv_store3) {
    using namespace infinity;

    // Test disable WAL
    {
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        kv_store->Init("/tmp/rocksdb_transaction_example");

        {
            UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

            std::string value;

            Status status = kv_instance1->Get("abc", value);
            EXPECT_FALSE(status.ok());

            status = kv_instance1->Put("db|db1|10", "10");
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Commit();
            EXPECT_TRUE(status.ok());

            UniquePtr<KVInstance> kv_instance2 = kv_store->GetInstance();
            kv_instance1 = kv_store->GetInstance();

            status = kv_instance1->Delete("db|db1|10");
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Put("db|db1|20", "20");
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Commit();
            EXPECT_TRUE(status.ok());

            const String prefix = "db|db1|";
            auto iter2 = kv_instance2->GetIterator();
            iter2->Seek(prefix);
            while (iter2->Valid() && iter2->Key().starts_with(prefix)) {
                EXPECT_STREQ(iter2->Key().ToString().c_str(), "db|db1|10");
                EXPECT_STREQ(iter2->Value().ToString().c_str(), "10");
                //            std::cout << iter2->key().ToString() << ": " << iter2->value().ToString() << std::endl;
                iter2->Next();
            }

            kv_instance2->Commit();
        }

        Status status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(KVStoreTest, kv_store4) {
    using namespace infinity;

    // Backup
    {
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        kv_store->Init("/tmp/rocksdb_transaction_example");
        UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

        Status status = kv_instance1->Put("db|db1|10", "10");
        EXPECT_TRUE(status.ok());

        status = kv_instance1->Put("abc", "def");
        EXPECT_TRUE(status.ok());

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        kv_store->Flush();

        Vector<rocksdb::BackupInfo> backup_info_list;
        status = kv_store->CreateBackup("/tmp/rocksdb_example_backup", backup_info_list);
        EXPECT_TRUE(status.ok());

        for (const auto &backup_info : backup_info_list) {
            std::cout << backup_info.number_files << std::endl;
        }

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = KVStore::RestoreFromBackup("/tmp/rocksdb_example_backup", "/tmp/rocksdb_transaction_example");
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        kv_store->Init("/tmp/rocksdb_transaction_example");
        {
            UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();
            const String prefix = "db|db1|";
            auto iter2 = kv_instance1->GetIterator();
            iter2->Seek(prefix);
            while (iter2->Valid() && iter2->Key().starts_with(prefix)) {
                EXPECT_STREQ(iter2->Key().ToString().c_str(), "db|db1|10");
                EXPECT_STREQ(iter2->Value().ToString().c_str(), "10");
                //            std::cout << iter2->key().ToString() << ": " << iter2->value().ToString() << std::endl;
                iter2->Next();
            }

            std::string value;
            status = kv_instance1->Get("abc", value);
            EXPECT_TRUE(status.ok());
            EXPECT_STREQ(value.c_str(), "def");

            kv_instance1->Commit();
        }
        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(KVStoreTest, kv_store5) {
    using namespace infinity;

    //    using rocksdb::DestroyDB;
    using rocksdb::FlushOptions;
    using rocksdb::Options;
    using rocksdb::ReadOptions;
    using rocksdb::Slice;
    using rocksdb::Snapshot;
    using rocksdb::Status;
    using rocksdb::Transaction;
    using rocksdb::TransactionDB;
    using rocksdb::TransactionDBOptions;
    using rocksdb::TransactionOptions;
    using rocksdb::WriteOptions;

    const std::string db_path = "/tmp/rocksdb_two_phases_transaction";

    {
        // open DB
        Options options;
        TransactionDBOptions txn_db_options;
        options.create_if_missing = true;
        options.manual_wal_flush = true;
        options.avoid_flush_during_shutdown = true;
        TransactionDB *txn_db;
        TransactionOptions txn_options;
        txn_options.set_snapshot = true;

        Status s = TransactionDB::Open(options, txn_db_options, db_path, &txn_db);
        EXPECT_TRUE(s.ok());

        WriteOptions write_options;
        write_options.disableWAL = true;
        ReadOptions read_options;
        std::string value;

        Transaction *txn1 = txn_db->BeginTransaction(write_options, txn_options);
        EXPECT_TRUE(txn1 != nullptr);

        s = txn1->Put("key1", "value1");
        EXPECT_TRUE(s.ok());
        s = txn1->Put("key2", "value2");
        EXPECT_TRUE(s.ok());
        s = txn1->Put("key3", "value3");
        EXPECT_TRUE(s.ok());
        s = txn1->Put("key4", "value4");
        EXPECT_TRUE(s.ok());
        s = txn1->Put("key5", "value5");
        EXPECT_TRUE(s.ok());

        s = txn1->Commit();
        EXPECT_TRUE(s.ok());
        delete txn1;

        Transaction *txn2 = txn_db->BeginTransaction(write_options, txn_options);
        EXPECT_TRUE(txn2 != nullptr);
        txn2->SetSnapshot();
        const Snapshot *snapshot = txn2->GetSnapshot();

        const String prefix = "key";
        read_options.snapshot = snapshot;
        read_options.iterate_lower_bound = new Slice("key1");
        read_options.iterate_upper_bound = new Slice("key4");
        auto iter2 = txn2->GetIterator(read_options);
        iter2->Seek(prefix);
        while (iter2->Valid() && iter2->key().starts_with(prefix)) {
            //            EXPECT_STREQ(iter2->key().ToString().c_str(), "db|db1|10");
            //            EXPECT_STREQ(iter2->value().ToString().c_str(), "10");
            std::cout << iter2->key().ToString() << ": " << iter2->value().ToString() << std::endl;
            iter2->Next();
        }
        delete iter2;
        txn2->Commit();
        read_options.snapshot = nullptr;

        delete read_options.iterate_lower_bound;
        delete read_options.iterate_upper_bound;

        delete txn2;

        // Cleanup
        delete txn_db;
        KVStore::Destroy(db_path);
    }
}

TEST_P(KVStoreTest, kv_store6) {
    using namespace infinity;
    UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
    kv_store->Init("/tmp/rocksdb_transaction_example");
    {

        UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

        Status status = kv_instance1->Put("key1", "value1");
        EXPECT_TRUE(status.ok());
        status = kv_instance1->Put("key2", "value2");
        EXPECT_TRUE(status.ok());
        status = kv_instance1->Put("key3", "value3");
        EXPECT_TRUE(status.ok());
        status = kv_instance1->Put("key4", "value4");
        EXPECT_TRUE(status.ok());
        status = kv_instance1->Put("key5", "value5");
        EXPECT_TRUE(status.ok());
        status = kv_instance1->Put("foo", "value5");
        EXPECT_TRUE(status.ok());
        status = kv_instance1->Put("loop", "value5");
        EXPECT_TRUE(status.ok());

        auto iter2 = kv_instance1->GetIterator("key2", "key4");

        String prefix = "key";
        iter2->Seek(prefix);
        while (iter2->Valid() && iter2->Key().starts_with(prefix)) {
            std::cout << iter2->Key().ToString() << ": " << iter2->Value().ToString() << std::endl;
            iter2->Next();
        }

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        kv_store->Flush();
    }
    Status status = kv_store->Uninit();
    EXPECT_TRUE(status.ok());
    status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
    EXPECT_TRUE(status.ok());
}

//
//// txn1: create db1, commit.
//// txn2: start,              get db1, commit
//// txn3:                     start, get db1, delete db1, commit
// TEST_P(CatalogTest, simple_test2) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//
//     // start txn2
//     auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);
//
//     // create db in empty catalog should be success
//     {
//         Status status = txn1->CreateDatabase(MakeShared<String>("db1"), ConflictType::kError, MakeShared<String>());
//         EXPECT_TRUE(status.ok());
//     }
//
//     u64 txn1_id = txn1->TxnID();
//     TxnTimeStamp txn1_begin_ts = txn1->BeginTS();
//     txn_mgr->CommitTxn(txn1);
//
//     // should not be visible to txn2
//     {
//         auto [db_entry, status1] = catalog->GetDatabase("db1", txn1_id, txn1_begin_ts);
//         // should not be visible to same txn
//         EXPECT_TRUE(!status1.ok());
//     }
//
//     txn_mgr->CommitTxn(txn2);
//
//     auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//
//     // should be visible to txn3
//     {
//         auto [db_entry1, status1] = catalog->GetDatabase("db1", txn3->TxnID(), txn3->BeginTS());
//         EXPECT_TRUE(status1.ok());
//         EXPECT_NE(db_entry1, nullptr);
//
//         Status status = txn3->DropDatabase("db1", ConflictType::kError);
//         EXPECT_TRUE(status.ok());
//
//         // should not be visible to other txn
//         auto [db_entry2, status2] = catalog->Catalog::GetDatabase("db1", txn3->TxnID(), txn3->BeginTS());
//         EXPECT_TRUE(!status2.ok());
//     }
//
//     txn_mgr->CommitTxn(txn3);
// }
//
// TEST_P(CatalogTest, concurrent_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//
//     for (int loop = 0; loop < 1; ++loop) {
//         // start txn1 && txn2
//         auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//         auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//
//         auto write_routine = [&](int start, Txn *txn) {
//             for (int db_id = start; db_id < 1000; db_id += 2) {
//                 String db_name = "db" + std::to_string(db_id);
//                 Status status = txn->CreateDatabase(MakeShared<String>(db_name), ConflictType::kError, MakeShared<String>());
//                 EXPECT_TRUE(status.ok());
//             }
//         };
//
//         Thread write_thread1(write_routine, 0, txn1);
//         Thread write_thread2(write_routine, 1, txn2);
//
//         write_thread1.join();
//         write_thread2.join();
//
//         txn_mgr->CommitTxn(txn1);
//         txn_mgr->CommitTxn(txn2);
//
//         // start txn3 && txn4
//         auto *txn3 = txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);
//         auto *txn4 = txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);
//
//         auto read_routine = [&](Txn *txn) {
//             for (int db_id = 0; db_id < 1000; ++db_id) {
//                 String db_name = "db" + std::to_string(db_id);
//                 auto [db_entry, status] = catalog->Catalog::GetDatabase(db_name, txn->TxnID(), txn->BeginTS());
//                 EXPECT_TRUE(status.ok());
//                 // only read, don't need lock
//                 EXPECT_NE(db_entry, nullptr);
//             }
//         };
//
//         std::thread read_thread1(read_routine, txn3);
//         std::thread read_thread2(read_routine, txn4);
//         read_thread1.join();
//         read_thread2.join();
//
//         txn_mgr->CommitTxn(txn3);
//         txn_mgr->CommitTxn(txn4);
//
//         // start txn5 && txn6
//         auto *txn5 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//         auto *txn6 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//
//         auto drop_routine = [&](int start, Txn *txn) {
//             for (int db_id = start; db_id < 1000; db_id += 2) {
//                 String db_name = "db" + std::to_string(db_id);
//                 Status status = txn->DropDatabase(db_name, ConflictType::kError);
//                 EXPECT_TRUE(status.ok());
//             }
//         };
//
//         std::thread drop_thread1(drop_routine, 0, txn5);
//         std::thread drop_thread2(drop_routine, 1, txn6);
//         drop_thread1.join();
//         drop_thread2.join();
//
//         txn_mgr->CommitTxn(txn5);
//         txn_mgr->CommitTxn(txn6);
//
//         // start txn7
//         auto *txn7 = txn_mgr->BeginTxn(MakeUnique<String>("drop db"), TransactionType::kNormal);
//
//         // check all has been dropped
//         for (int db_id = 0; db_id < 1000; ++db_id) {
//             String db_name = "db" + std::to_string(db_id);
//             auto [db_entry, status] = catalog->Catalog::GetDatabase(db_name, txn7->TxnID(), txn7->BeginTS());
//             EXPECT_TRUE(!status.ok());
//         }
//         txn_mgr->CommitTxn(txn7);
//     }
// }
//
//// txn1: create db1, get db1 info, delete db1, get db1, commit
//// txn2:             get db1 info,             get db1, commit
// TEST_P(CatalogTest, get_db_info_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create db"), TransactionType::kNormal);
//
//     // start txn2
//     auto *txn2 = txn_mgr->BeginTxn(MakeUnique<String>("get db"), TransactionType::kRead);
//
//     // create db in empty catalog should be success
//     {
//         auto [base_entry, status] = catalog->CreateDatabase(MakeShared<String>("db1"), MakeShared<String>(), txn1->TxnID(), txn1->BeginTS(),
//         txn_mgr); EXPECT_TRUE(status.ok());
//     }
//
//     {
//         auto [db_info1, status1] = catalog->GetDatabaseInfo("db1", txn1->TxnID(), txn1->BeginTS());
//         // should be visible to same txn
//         EXPECT_TRUE(status1.ok());
//         std::cout << *(db_info1->db_name_) << std::endl;
//         EXPECT_STREQ("db1", db_info1->db_name_->c_str());
//
//         // should not be visible to other txn
//         auto [db_info2, status2] = catalog->GetDatabaseInfo("db1", txn2->TxnID(), txn2->BeginTS());
//         EXPECT_TRUE(!status2.ok());
//     }
//
//     // drop db should be success
//     {
//         auto [base_entry, status] = catalog->DropDatabase("db1", txn1->TxnID(), txn1->BeginTS(), txn_mgr);
//         EXPECT_TRUE(status.ok());
//
//         auto [db_info1, status1] = catalog->GetDatabaseInfo("db1", txn1->TxnID(), txn1->BeginTS());
//         // should not be visible to same txn
//         EXPECT_TRUE(!status1.ok());
//
//         // should not be visible to other txn
//         auto [db_info2, status2] = catalog->GetDatabaseInfo("db1", txn2->TxnID(), txn2->BeginTS());
//         EXPECT_TRUE(!status2.ok());
//     }
//
//     txn_mgr->CommitTxn(txn1);
//     txn_mgr->CommitTxn(txn2);
// }
//
// TEST_P(CatalogTest, get_table_info_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//
//     // create table, get table info, drop table
//     {
//         Vector<SharedPtr<ColumnDef>> columns;
//         {
//             std::set<ConstraintType> constraints;
//             constraints.insert(ConstraintType::kNotNull);
//             i64 column_id = 0;
//             auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
//             auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1",
//             constraints); columns.emplace_back(column_def_ptr);
//         }
//
//         auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
//         auto [table_entry, status] =
//             catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
//         EXPECT_TRUE(status.ok());
//
//         auto [table_info, status1] = catalog->GetTableInfo("default_db", "tbl1", txn1);
//         EXPECT_TRUE(status1.ok());
//         EXPECT_STREQ("tbl1", table_info->table_name_->c_str());
//
//         auto [table_entry1, status2] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(),
//         txn_mgr); EXPECT_TRUE(status2.ok());
//     }
//
//     txn_mgr->CommitTxn(txn1);
// }
//
// TEST_P(CatalogTest, get_table_index_info_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
//
//     // create table
//     {
//         Vector<SharedPtr<ColumnDef>> columns;
//         {
//             std::set<ConstraintType> constraints;
//             constraints.insert(ConstraintType::kNotNull);
//             i64 column_id = 0;
//             auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
//             auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1",
//             constraints); columns.emplace_back(column_def_ptr);
//         }
//
//         auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
//         auto [table_entry, status] =
//             catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
//         EXPECT_TRUE(status.ok());
//     }
//
//     // CreateIndex
//     {
//         Vector<String> columns1{"col1"};
//         Vector<InitParameter *> parameters1;
//         parameters1.emplace_back(new InitParameter("metric", "l2"));
//         parameters1.emplace_back(new InitParameter("encode", "plain"));
//         parameters1.emplace_back(new InitParameter("m", "16"));
//         parameters1.emplace_back(new InitParameter("ef_construction", "200"));
//
//         SharedPtr<String> index_name = MakeShared<String>("hnsw_index");
//         auto index_base_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test comment"), "hnsw_index_test_hnsw", columns1, parameters1);
//         for (auto *init_parameter : parameters1) {
//             delete init_parameter;
//         }
//
//         const String &db_name = "default_db";
//         const String &table_name = "tbl1";
//         ConflictType conflict_type = ConflictType::kError;
//         auto [table_entry, table_status] = catalog->GetTableByName(db_name, table_name, txn1->TxnID(), txn1->BeginTS());
//         EXPECT_EQ(table_status.ok(), true);
//         auto [index_entry, index_status] = catalog->CreateIndex(table_entry, index_base_hnsw, conflict_type, txn1->TxnID(), txn1->BeginTS(),
//         txn_mgr); EXPECT_EQ(index_status.ok(), true);
//     }
//
//     // get index info
//     {
//         auto [index_info, status] = catalog->GetTableIndexInfo("default_db", "tbl1", "hnsw_index", txn1->TxnID(), txn1->BeginTS());
//         EXPECT_TRUE(status.ok());
//         EXPECT_STREQ("hnsw_index", index_info->index_name_->c_str());
//     }
//
//     // drop index
//     {
//         auto [index_entry, status] =
//             catalog->DropIndex("default_db", "tbl1", "hnsw_index", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
//         EXPECT_TRUE(status.ok());
//     }
//
//     // drop table
//     {
//         auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
//         EXPECT_TRUE(status.ok());
//     }
//
//     txn_mgr->CommitTxn(txn1);
// }
//
// TEST_P(CatalogTest, remove_index_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create index"), TransactionType::kNormal);
//
//     // create table
//     {
//         Vector<SharedPtr<ColumnDef>> columns;
//         {
//             std::set<ConstraintType> constraints;
//             constraints.insert(ConstraintType::kNotNull);
//             i64 column_id = 0;
//             auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
//             auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1",
//             constraints); columns.emplace_back(column_def_ptr);
//         }
//
//         auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
//         auto [table_entry, status] =
//             catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
//
//         EXPECT_TRUE(status.ok());
//     }
//
//     // CreateIndex
//     {
//         Vector<String> columns1{"col1"};
//         Vector<InitParameter *> parameters1;
//         parameters1.emplace_back(new InitParameter("metric", "l2"));
//         parameters1.emplace_back(new InitParameter("encode", "plain"));
//         parameters1.emplace_back(new InitParameter("m", "16"));
//         parameters1.emplace_back(new InitParameter("ef_construction", "200"));
//
//         SharedPtr<String> index_name = MakeShared<String>("hnsw_index");
//         auto index_base_hnsw = IndexHnsw::Make(index_name, MakeShared<String>("test comment"), "hnsw_index_test_hnsw", columns1, parameters1);
//         for (auto *init_parameter : parameters1) {
//             delete init_parameter;
//         }
//
//         const String &db_name = "default_db";
//         const String &table_name = "tbl1";
//         ConflictType conflict_type = ConflictType::kError;
//         auto [table_entry, table_status] = catalog->GetTableByName(db_name, table_name, txn1->TxnID(), txn1->BeginTS());
//         EXPECT_EQ(table_status.ok(), true);
//         auto [index_entry, index_status] = catalog->CreateIndex(table_entry, index_base_hnsw, conflict_type, txn1->TxnID(), txn1->BeginTS(),
//         txn_mgr); EXPECT_EQ(index_status.ok(), true);
//     }
//
//     // remove index
//     {
//         auto [index_entry, index_status] = catalog->GetIndexByName("default_db", "tbl1", "hnsw_index", txn1->TxnID(), txn1->BeginTS());
//         EXPECT_TRUE(index_status.ok());
//         auto status = catalog->RemoveIndexEntry(index_entry, txn1->TxnID());
//         EXPECT_TRUE(status.ok());
//     }
//
//     // drop table
//     {
//         auto [table_entry, status] = catalog->DropTableByName("default_db", "tbl1", ConflictType::kError, txn1->TxnID(), txn1->BeginTS(), txn_mgr);
//         EXPECT_TRUE(status.ok());
//     }
//
//     txn_mgr->CommitTxn(txn1);
// }
//
// TEST_P(CatalogTest, roll_back_append_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//     BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//
//     // create table
//     {
//         Vector<SharedPtr<ColumnDef>> columns;
//         {
//             std::set<ConstraintType> constraints;
//             constraints.insert(ConstraintType::kNotNull);
//             i64 column_id = 0;
//             auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
//             auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1",
//             constraints); columns.emplace_back(column_def_ptr);
//         }
//
//         auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
//         auto [table_entry, status] =
//             catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
//
//         EXPECT_TRUE(status.ok());
//     }
//
//     {
//         auto [table_entry, status] = catalog->GetTableByName("default_db", "tbl1", txn1->TxnID(), txn1->BeginTS());
//         EXPECT_TRUE(status.ok());
//
//         auto txn_store = txn1->GetTxnTableStore(table_entry);
//         txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
//         Catalog::Append(table_entry, txn1->TxnID(), (void *)txn_store, txn1->CommitTS(), buffer_mgr);
//         EXPECT_THROW(Catalog::RollbackAppend(table_entry, txn1->TxnID(), txn1->CommitTS(), (void *)txn_store), UnrecoverableException);
//     }
// }
//
// TEST_P(CatalogTest, roll_back_delete_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//     BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//
//     // create table
//     {
//         Vector<SharedPtr<ColumnDef>> columns;
//         {
//             std::set<ConstraintType> constraints;
//             constraints.insert(ConstraintType::kNotNull);
//             i64 column_id = 0;
//             auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
//             auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1",
//             constraints); columns.emplace_back(column_def_ptr);
//         }
//
//         auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
//         auto [table_entry, status] =
//             catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
//
//         EXPECT_TRUE(status.ok());
//     }
//
//     {
//         auto [table_entry, status] = catalog->GetTableByName("default_db", "tbl1", txn1->TxnID(), txn1->BeginTS());
//         EXPECT_TRUE(status.ok());
//
//         auto txn_store = txn1->GetTxnTableStore(table_entry);
//         Catalog::Delete(table_entry, txn1->TxnID(), (void *)txn_store, txn1->CommitTS(), txn_store->GetDeleteStateRef());
//         EXPECT_THROW(Catalog::RollbackDelete(table_entry, txn1->TxnID(), txn_store->GetDeleteStateRef(), buffer_mgr), UnrecoverableException);
//     }
// }
//
// TEST_P(CatalogTest, roll_back_write_test) {
//     using namespace infinity;
//
//     TxnManager *txn_mgr = infinity::InfinityContext::instance().storage()->txn_manager();
//     Catalog *catalog = infinity::InfinityContext::instance().storage()->catalog();
//     BufferManager *buffer_mgr = infinity::InfinityContext::instance().storage()->buffer_manager();
//
//     // start txn1
//     auto *txn1 = txn_mgr->BeginTxn(MakeUnique<String>("create table"), TransactionType::kNormal);
//
//     // create table
//     {
//         Vector<SharedPtr<ColumnDef>> columns;
//         {
//             std::set<ConstraintType> constraints;
//             constraints.insert(ConstraintType::kNotNull);
//             i64 column_id = 0;
//             auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128);
//             auto column_def_ptr = MakeShared<ColumnDef>(column_id, MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo), "col1",
//             constraints); columns.emplace_back(column_def_ptr);
//         }
//
//         auto tbl1_def = MakeUnique<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("tbl1"), MakeShared<String>(), columns);
//         auto [table_entry, status] =
//             catalog->CreateTable("default_db", txn1->TxnID(), txn1->BeginTS(), std::move(tbl1_def), ConflictType::kError, txn_mgr);
//
//         EXPECT_TRUE(status.ok());
//     }
//
//     {
//         auto [table_entry, status] = catalog->GetTableByName("default_db", "tbl1", txn1->TxnID(), txn1->BeginTS());
//         EXPECT_TRUE(status.ok());
//
//         auto txn_store = txn1->GetTxnTableStore(table_entry);
//         txn_store->SetAppendState(MakeUnique<AppendState>(txn_store->GetBlocks()));
//         Catalog::Append(table_entry, txn1->TxnID(), (void *)txn_store, txn1->CommitTS(), buffer_mgr);
//         Catalog::CommitWrite(table_entry, txn1->TxnID(), txn1->CommitTS(), txn_store->txn_segments(), nullptr);
//         Vector<TxnSegmentStore> segment_stores;
//         auto status1 = Catalog::RollbackWrite(table_entry, txn1->CommitTS(), segment_stores);
//         EXPECT_TRUE(status1.ok());
//     }
// }
