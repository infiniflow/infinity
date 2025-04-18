// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <gtest/gtest.h>
#include <numeric>

import base_test;
import stl;
import third_party;
import new_catalog;
import compilation_config;
import kv_store;
import config;
import status;
import infinity_context;
import new_txn_manager;
import db_meeta;
import txn_state;
import catalog;
import new_txn;
import extra_ddl_info;
import table_meeta;
import table_index_meeta;
import segment_meta;
import block_meta;

using namespace infinity;

class TransformMeta : public BaseTestWithParam<String> {
public:
    void SetUp() override {
        CleanupDbDirs();
        String config_path_str = GetParam();
        config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = std::make_shared<String>(std::filesystem::absolute(config_path_str));
        }
    }

    void TearDown() override {}

    void test_print() { std::cerr << "some OK\n"; }

    void Init() {
        std::string config_path_str = GetParam();
        config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = std::make_shared<std::string>(std::filesystem::absolute(config_path_str));
        }
        infinity::InfinityContext::instance().InitPhase1(config_path);
        infinity::InfinityContext::instance().InitPhase2();
        std::cerr << "init done!\n";
    }

    void UnInit() {
        infinity::InfinityContext::instance().UnInit();
        std::cerr << "uninit done!\n";
    }

protected:
    std::shared_ptr<String> config_path;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TransformMeta,
                         ::testing::Values(TransformMeta::NEW_CONFIG_PATH, TransformMeta::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TransformMeta, db_meta_transform_00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/db_meta_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();
    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();

    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
    {
        Optional<DBMeeta> db_meta;
        status = txn->GetDBMeta("db1", db_meta);
        EXPECT_TRUE(status.ok());
    }

    {
        Optional<DBMeeta> db_meta;
        status = txn->GetDBMeta("default_db", db_meta);
        EXPECT_TRUE(status.ok());
    }

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, db_meta_transform_01) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/db_meta_01.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();
    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
    {
        Optional<DBMeeta> db_meta;
        status = txn->GetDBMeta("db1", db_meta);
        EXPECT_TRUE(!status.ok());
    }

    {
        Optional<DBMeeta> db_meta;
        status = txn->GetDBMeta("default_db", db_meta);
        EXPECT_TRUE(status.ok());
    }

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, db_meta_transform_02) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/db_meta_02.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();
    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
    {
        Optional<DBMeeta> db_meta;
        status = txn->GetDBMeta("db1", db_meta);
        EXPECT_TRUE(status.ok());
    }

    {
        Optional<DBMeeta> db_meta;
        status = txn->GetDBMeta("default_db", db_meta);
        EXPECT_TRUE(status.ok());
    }

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, table_meta_transform_00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/table_meta_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();
    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
    {
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn->GetTableMeta("db1", "test_table", db_meta, table_meta);
        EXPECT_TRUE(status.ok());
    }

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, table_index_transform_00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/table_index_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);
    {
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn->GetTableMeta("default_db", "student", db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        // // Status GetTableMeta(const String &db_name,
        // //                     const String &table_name,
        // //                     Optional<DBMeeta> &db_meta,
        // //                     Optional<TableMeeta> &table_meta,
        // //                     String *table_key = nullptr);
        Optional<TableIndexMeeta> table_index_meta;

        status = txn->GetTableIndexMeta("idx_student_name", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        // if (table_meta.has_value()) {
        status = txn->GetTableIndexMeta("idx_student_id", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        // }
    }

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, table_index_transform_01) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/table_index_01.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check table"), TransactionType::kNormal);
    {
        Optional<DBMeeta> db_meta;
        Optional<TableMeeta> table_meta;
        status = txn->GetTableMeta("default_db", "my_table", db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        status = txn->GetTableMeta("default_db", "my_table", db_meta, table_meta);
        EXPECT_TRUE(status.ok());
        Optional<TableIndexMeeta> table_index_meta;

        status = txn->GetTableIndexMeta("age_secondary_index", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        status = txn->GetTableIndexMeta("sparse_vector_bmp_index", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        status = txn->GetTableIndexMeta("vector_ivf_index", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        status = txn->GetTableIndexMeta("doc_fulltext_index", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        status = txn->GetTableIndexMeta("vector_hnsw_index", table_meta.value(), table_index_meta);
        EXPECT_TRUE(status.ok());
        auto segmentindexmeta = SegmentIndexMeta(0, table_index_meta.value());
        txn->GetSegmentIndexInfo("default_db", "my_table", "age_secondary_index", 0);
        txn->GetChunkIndexInfo("default_db", "my_table", "age_secondary_index", 0, 0);
    }

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, segment_transform_00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/segment_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();

    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
    auto [table_info, get_status] = txn->GetTableInfo("db1", "test_table");
    EXPECT_TRUE(get_status.ok());

    UniquePtr<KVInstance> kv_instance = infinity::InfinityContext::instance().storage()->KVInstance();
    TableMeeta table_meta{table_info->db_id_, table_info->table_id_, *kv_instance, txn->BeginTS()};

    {
        auto [segment_ids_ptr, segment_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(segment_status.ok());
        EXPECT_EQ(segment_ids_ptr->size(), 1);
        EXPECT_EQ(segment_ids_ptr->at(0), 0);
    }

    new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    kv_instance->Commit();
    UnInit();
}

TEST_P(TransformMeta, block_transform_00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/segment_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    new_txn_mgr->PrintAllKeyValue();

    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
    auto [table_info, get_status] = txn->GetTableInfo("db1", "test_table");
    EXPECT_TRUE(get_status.ok());

    UniquePtr<KVInstance> kv_instance = infinity::InfinityContext::instance().storage()->KVInstance();
    TableMeeta table_meta{table_info->db_id_, table_info->table_id_, *kv_instance, txn->BeginTS()};

    {
        auto [segment_ids_ptr, segment_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(segment_status.ok());
        EXPECT_EQ(segment_ids_ptr->size(), 1);
        EXPECT_EQ(segment_ids_ptr->at(0), 0);
        {
            SegmentMeta segment_meta(0, table_meta);
            {
                auto [blocks, block_status] = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(block_status.ok());
                EXPECT_EQ(blocks->size(), 1);
                EXPECT_EQ(blocks->at(0), 0);

                BlockMeta block_meta{0, segment_meta};
                {
                    auto [block_columns, block_column_status] = block_meta.GetBlockColumnIDs1();
                    EXPECT_TRUE(block_column_status.ok());
                    EXPECT_EQ(block_columns->size(), 2);
                    EXPECT_EQ(block_columns->at(0), 0);
                    EXPECT_EQ(block_columns->at(1), 1);
                }
            }
        }
    }

    new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    kv_instance->Commit();
    UnInit();
}

TEST_P(TransformMeta, block_column_transform_00) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = String(test_data_path()) + "/json/segment_00.json";
    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
    {
        {
            auto [_, status] = txn->GetBlockColumnInfo("db1", "test_table", 0, 0, 0);
            EXPECT_TRUE(status.ok());
        }
        {
            auto [_, status] = txn->GetBlockColumnInfo("db1", "test_table", 0, 0, 1);
            EXPECT_TRUE(status.ok());
        }

        {
            auto [_, status] = txn->GetBlockColumnInfo("db1", "test_table", 0, 0, 11451);
            EXPECT_FALSE(status.ok());
        }
    }
    //     Tuple<SharedPtr<BlockColumnInfo>, Status>
    // GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id);

    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    UnInit();
}

TEST_P(TransformMeta, block_column_transform_01) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = "/home/inf/Downloads/FULL.2000764.json";

    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();

    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("get table"), TransactionType::kNormal);
    auto [table_info, get_status] = txn->GetTableInfo("default_db", "my_table");
    EXPECT_TRUE(get_status.ok());

    UniquePtr<KVInstance> kv_instance = infinity::InfinityContext::instance().storage()->KVInstance();
    TableMeeta table_meta{table_info->db_id_, table_info->table_id_, *kv_instance, txn->BeginTS()};

    {
        auto [segment_ids_ptr, segment_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(segment_status.ok());
        EXPECT_EQ(segment_ids_ptr->size(), 2);
        EXPECT_EQ(segment_ids_ptr->at(0), 0);
        EXPECT_EQ(segment_ids_ptr->at(1), 1);
        {
            SegmentMeta segment_meta{0, table_meta};
            {
                auto [blocks, block_status] = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(block_status.ok());
                EXPECT_EQ(blocks->size(), 1024);
                EXPECT_EQ(blocks->at(0), 0);

                BlockMeta block_meta{0, segment_meta};
                {
                    auto [block_columns, block_column_status] = block_meta.GetBlockColumnIDs1();
                    EXPECT_TRUE(block_column_status.ok());
                    EXPECT_EQ(block_columns->size(), 4);

                    EXPECT_EQ(block_columns->at(0), 0);
                    EXPECT_EQ(block_columns->at(1), 1);
                    EXPECT_EQ(block_columns->at(2), 2);
                    EXPECT_EQ(block_columns->at(3), 3);
                }
            }
        }
    }

    new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    kv_instance->Commit();
    UnInit();
}

TEST_P(TransformMeta, block_column_transform_02) {
    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(config_path, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> new_catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    String full_ckp_path = "/home/inf/Downloads/FULL.52.json";

    Vector<String> delta_ckp_path_array;
    new_catalog_ptr->TransformCatalog(config_ptr.get(), full_ckp_path, delta_ckp_path_array);

    status = kv_store_ptr->Flush();
    EXPECT_TRUE(status.ok());
    kv_store_ptr->Uninit();
    kv_store_ptr.reset();
    new_catalog_ptr.reset();

    Init();

    NewTxnManager *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn_mgr->SetNewSystemTS(
        std::numeric_limits<int>::max()); // due to WAL isn't replayed, and system timestamp is get from WAL. Set a huge timestamp here
    // new_txn_mgr->PrintAllKeyValue();
    auto *txn = new_txn_mgr->BeginTxn(MakeUnique<String>("check db"), TransactionType::kNormal);
    auto [table_info, get_status] = txn->GetTableInfo("default_db", "my_table_1");
    EXPECT_TRUE(get_status.ok());

    UniquePtr<KVInstance> kv_instance = infinity::InfinityContext::instance().storage()->KVInstance();
    TableMeeta table_meta{table_info->db_id_, table_info->table_id_, *kv_instance, txn->BeginTS()};

    {
        auto [segment_ids_ptr, segment_status] = table_meta.GetSegmentIDs1();
        EXPECT_TRUE(segment_status.ok());
        EXPECT_EQ(segment_ids_ptr->size(), 1);
        EXPECT_EQ(segment_ids_ptr->at(0), 0);
        {
            SegmentMeta segment_meta{0, table_meta};
            {
                auto [blocks, block_status] = segment_meta.GetBlockIDs1();
                EXPECT_TRUE(block_status.ok());
                EXPECT_EQ(blocks->size(), 1);
                EXPECT_EQ(blocks->at(0), 0);

                BlockMeta block_meta{0, segment_meta};
                {
                    auto [block_columns, block_column_status] = block_meta.GetBlockColumnIDs1();
                    EXPECT_TRUE(block_column_status.ok());
                    EXPECT_EQ(block_columns->size(), 10);

                    EXPECT_EQ(block_columns->at(0), 0);
                    EXPECT_EQ(block_columns->at(1), 1);
                    EXPECT_EQ(block_columns->at(2), 2);
                    EXPECT_EQ(block_columns->at(3), 3);
                    EXPECT_EQ(block_columns->at(4), 4);
                    EXPECT_EQ(block_columns->at(5), 5);
                    EXPECT_EQ(block_columns->at(6), 6);
                    EXPECT_EQ(block_columns->at(7), 7);
                    EXPECT_EQ(block_columns->at(8), 8);
                    EXPECT_EQ(block_columns->at(9), 9);
                }
            }
        }
    }
    //     Tuple<SharedPtr<BlockColumnInfo>, Status>
    // GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id);

    new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());

    kv_instance->Commit();
    UnInit();
}
