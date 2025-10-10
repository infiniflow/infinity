module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.upload_meta_to_s3;

import :ut.base_test;
import :new_txn_manager;
import :new_txn;
import :kv_store;
import :utility;
import :table_meta;
import :segment_meta;
import :block_meta;
import :default_values;

import third_party;

import column_def;

using namespace infinity;
using namespace std::chrono_literals;

class UploadMetaToS3 : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, UploadMetaToS3, ::testing::Values(BaseTestParamStr::S3_STORAGE));

TEST_P(UploadMetaToS3, DISABLED_MINIO_test1) {
    auto *txn_mgr_ = infinity::InfinityContext::instance().storage()->new_txn_manager();
    auto db_name = std::make_shared<std::string>("db1");
    auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kInteger), "col1", std::set<ConstraintType>());
    auto column_def2 = std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kVarchar), "col2", std::set<ConstraintType>());
    auto table_name = std::make_shared<std::string>("tb1");
    auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1, column_def2});

    {
        auto *txn = txn_mgr_->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
        auto status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
        EXPECT_TRUE(status.ok());
        status = txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    {
        auto *txn = txn_mgr_->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        auto status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
        EXPECT_TRUE(status.ok());
        status = txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto MakeInputBlock = [&](const Value &v1, const Value &v2, size_t row_cnt) {
        auto make_column = [&](std::shared_ptr<ColumnDef> &column_def, const Value &v) {
            auto col = ColumnVector::Make(column_def->type());
            col->Initialize();
            for (size_t i = 0; i < row_cnt; ++i) {
                col->AppendValue(v);
            }
            return col;
        };
        auto input_block = std::make_shared<DataBlock>();
        {
            auto col1 = make_column(column_def1, v1);
            input_block->InsertVector(col1, 0);
        }
        {
            auto col2 = make_column(column_def2, v2);
            input_block->InsertVector(col2, 1);
        }
        input_block->Finalize();
        return input_block;
    };

    {
        auto *txn = txn_mgr_->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
        auto input_block_ptr = MakeInputBlock(Value::MakeInt(1), Value::MakeVarchar("abcdefghijklmnopqrstuvwxyz"), 1);
        std::vector<std::shared_ptr<DataBlock>> input_blocks = {input_block_ptr};
        auto status = txn->Import(*db_name, *table_name, input_blocks);
        EXPECT_TRUE(status.ok());
        status = txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
        auto *txn = txn_mgr_->BeginTxn(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
        auto status = txn->Checkpoint(wal_manager->LastCheckpointTS(), false);
        EXPECT_TRUE(status.ok());
        status = txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }

    auto kv_store = infinity::InfinityContext::instance().storage()->kv_store();

    std::vector<std::string> remote_files;
    VirtualStore::ListObjects(S3_DEFAULT_BUCKET, "", remote_files);

    std::this_thread::sleep_for(1s); // fake async, may cause some fuck issue

    auto catalog_dir = GetCatalogDir();

    for (auto &file : remote_files) {
        std::string local_file_path;
        auto v = infinity::Partition(file, '/');

        std::string file_name = v.back();
        if (file.find(S3_META_SST_PREFIX) != std::string::npos) {
            local_file_path = fmt::format("{}/{}", catalog_dir, file_name);
        } else if (file.find(S3_META_PREFIX) != std::string::npos) {
            local_file_path = fmt::format("{}/{}", catalog_dir, file_name);
        } else {
            continue;
        }
        VirtualStore::DownloadObject(local_file_path, file);
    }

    kv_store->Init(catalog_dir);

    {
        auto *txn = txn_mgr_->BeginTxn(std::make_unique<std::string>("check"), TransactionType::kRead);

        std::shared_ptr<DBMeta> db_meta;
        std::shared_ptr<TableMeta> table_meta;
        TxnTimeStamp create_timestamp;
        auto status = txn->GetTableMeta(*db_name, *table_name, db_meta, table_meta, create_timestamp);
        EXPECT_TRUE(status.ok());

        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta->GetSegmentIDs1();
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(*segment_ids_ptr, std::vector<SegmentID>({0}));

        SegmentID segment_id = segment_ids_ptr->back();

        SegmentMeta segment_meta(segment_id, *table_meta);

        {
            size_t segment_row_cnt = 0;
            std::tie(segment_row_cnt, status) = segment_meta.GetRowCnt1();
            EXPECT_EQ(segment_row_cnt, 1);
        }

        {
            std::vector<BlockID> *block_ids_ptr = nullptr;
            std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(*block_ids_ptr, std::vector<BlockID>({0}));

            BlockID block_id = block_ids_ptr->back();
            BlockMeta block_meta(block_id, segment_meta);
            size_t block_row_cnt = 0;
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            EXPECT_EQ(block_row_cnt, 1);
        }

        status = txn_mgr_->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
}
