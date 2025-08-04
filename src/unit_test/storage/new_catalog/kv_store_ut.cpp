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

#ifndef CI
module;

#include "gtest/gtest.h"

module infinity_core:ut.kv_store;

import :ut.base_test;
import :stl;
import :third_party;
import :status;
import :kv_store;
#else
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#endif

using namespace infinity;

class TestTxnKVStoreTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTxnKVStoreTest,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTxnKVStoreTest, kv_store0) {
    using namespace infinity;
    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_transaction_example", GetFullTmpDir());
    UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
    Status status = kv_store->Init(rocksdb_tmp_path);
    EXPECT_TRUE(status.ok());

    std::string value;
    status = kv_store->Get("key1", value);
    EXPECT_FALSE(status.ok());

    status = kv_store->Put("key1", "1");
    EXPECT_TRUE(status.ok());

    status = kv_store->Get("key1", value);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(value, "1");

    status = kv_store->Merge("key1", "2");
    EXPECT_TRUE(status.ok());

    status = kv_store->Get("key1", value);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(value, "3");

    status = kv_store->Delete("key1");
    EXPECT_TRUE(status.ok());

    status = kv_store->Get("key1", value);
    EXPECT_FALSE(status.ok());

    status = kv_store->Uninit();
    EXPECT_TRUE(status.ok());

    status = kv_store->Destroy(rocksdb_tmp_path);
    EXPECT_TRUE(status.ok());
}

TEST_P(TestTxnKVStoreTest, kv_store1) {
    using namespace infinity;
    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_transaction_example", GetFullTmpDir());
    // Test multi-version
    UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
    Status status = kv_store->Init(rocksdb_tmp_path);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

    std::string value;

    status = kv_instance1->Get("abc", value);
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

    kv_instance1.reset();
    kv_instance2.reset();
    kv_instance3.reset();
    kv_instance4.reset();

    status = kv_store->Uninit();
    EXPECT_TRUE(status.ok());
    status = kv_store->Destroy(rocksdb_tmp_path);
    EXPECT_TRUE(status.ok());
}

TEST_P(TestTxnKVStoreTest, kv_store2) {
    using namespace infinity;
    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_transaction_example", GetFullTmpDir());
    // Test disable WAL
    {
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        Status status = kv_store->Init(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
        UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

        std::string value;

        status = kv_instance1->Get("abc", value);
        EXPECT_FALSE(status.ok());

        status = kv_instance1->Put("abc", "def");
        EXPECT_TRUE(status.ok());

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        kv_instance1.reset();

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());

        kv_store = MakeUnique<KVStore>();
        status = kv_store->Init(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
        kv_instance1 = kv_store->GetInstance();

        status = kv_instance1->Get("abc", value);
        EXPECT_FALSE(status.ok());

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        kv_instance1.reset();

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
    }

    // Test disable WAL with Flush
    {
        {
            UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
            Status status = kv_store->Init("/tmp/rocksdb_transaction_example");
            EXPECT_TRUE(status.ok());
            UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

            std::string value;

            status = kv_instance1->Get("abc", value);
            EXPECT_FALSE(status.ok());

            status = kv_instance1->Put("abc", "def");
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Commit();
            EXPECT_TRUE(status.ok());

            status = kv_store->Flush();
            EXPECT_TRUE(status.ok());

            kv_instance1.reset();

            status = kv_store->Uninit();
            EXPECT_TRUE(status.ok());
        }

        {
            UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
            Status status = kv_store->Init("/tmp/rocksdb_transaction_example");
            EXPECT_TRUE(status.ok());
            UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

            std::string value;

            status = kv_instance1->Get("abc", value);
            EXPECT_TRUE(status.ok());

            status = kv_instance1->Commit();
            EXPECT_TRUE(status.ok());

            kv_instance1.reset();

            status = kv_store->Uninit();
            EXPECT_TRUE(status.ok());

            status = kv_store->Destroy("/tmp/rocksdb_transaction_example");
            EXPECT_TRUE(status.ok());
        }
    }
}

TEST_P(TestTxnKVStoreTest, kv_store3) {
    using namespace infinity;
    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_transaction_example", GetFullTmpDir());
    // Test disable WAL
    {
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        Status status = kv_store->Init(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());

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

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnKVStoreTest, kv_store4) {
    using namespace infinity;
    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_transaction_example", GetFullTmpDir());
    const auto rocksdb_backup_path = fmt::format("{}/rocksdb_example_backup", GetFullTmpDir());
    // Backup
    {
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        Status status = kv_store->Init(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
        UniquePtr<KVInstance> kv_instance1 = kv_store->GetInstance();

        status = kv_instance1->Put("db|db1|10", "10");
        EXPECT_TRUE(status.ok());

        status = kv_instance1->Put("abc", "def");
        EXPECT_TRUE(status.ok());

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        kv_store->Flush();

        Vector<rocksdb::BackupInfo> backup_info_list;
        status = kv_store->CreateBackup(rocksdb_backup_path, backup_info_list);
        EXPECT_TRUE(status.ok());

        for (const auto &backup_info : backup_info_list) {
            std::cout << backup_info.number_files << std::endl;
        }

        kv_instance1.reset();

        status = kv_store->Uninit();
        EXPECT_TRUE(status.ok());
        status = kv_store->Destroy(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
    }
    {
        Status status = KVStore::RestoreFromBackup(rocksdb_backup_path, rocksdb_tmp_path);
        UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
        status = kv_store->Init(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
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
        status = kv_store->Destroy(rocksdb_tmp_path);
        EXPECT_TRUE(status.ok());
    }
}

TEST_P(TestTxnKVStoreTest, kv_store5) {
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

    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_two_phases_transaction", GetFullTmpDir());

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

        Status s = TransactionDB::Open(options, txn_db_options, rocksdb_tmp_path, &txn_db);
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
        KVStore::Destroy(rocksdb_tmp_path);
    }
}

TEST_P(TestTxnKVStoreTest, kv_store6) {
    using namespace infinity;
    UniquePtr<KVStore> kv_store = MakeUnique<KVStore>();
    const auto rocksdb_tmp_path = fmt::format("{}/rocksdb_transaction_example", GetFullTmpDir());
    Status status = kv_store->Init(rocksdb_tmp_path);
    EXPECT_TRUE(status.ok());
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

        //        auto iter2 = kv_instance1->GetIterator("key2", "key4");
        auto iter2 = kv_instance1->GetIterator();

        String prefix = "";
        iter2->Seek(prefix);
        while (iter2->Valid() && iter2->Key().starts_with(prefix)) {
            std::cout << iter2->Key().ToString() << ": " << iter2->Value().ToString() << std::endl;
            iter2->Next();
        }

        status = kv_instance1->Commit();
        EXPECT_TRUE(status.ok());

        kv_store->Flush();
    }
    status = kv_store->Uninit();
    EXPECT_TRUE(status.ok());
    status = kv_store->Destroy(rocksdb_tmp_path);
    EXPECT_TRUE(status.ok());
}

