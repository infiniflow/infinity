#include "executor/operator/physical_import.h"
#include <gtest/gtest.h>
#include <fstream>

#include "base_test.h"
#include "common/types/internal_types.h"
#include "main/infinity.h"
#include "main/query_context.h"
#include "main/session.h"
#include "main/stats/global_resource_usage.h"
#include "parser/statement/statement_common.h"
#include "storage/meta/entry/db_entry.h"
#include "storage/meta/table_collection_meta.h"
#include "storage/txn/txn.h"


class PhysicalImportTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

template<typename T>
void check_column(const std::vector<T> &expect, const std::string &file_path) {
    std::ifstream file(file_path, std::ios::binary);
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> file_data(file_size);
    file.read(file_data.data(), file_size);
    auto ptr = file_data.data();

    uint64_t magic_number, check_sum;
    std::size_t buffer_length;
    std::size_t meta_length = sizeof(magic_number) + sizeof(buffer_length) + sizeof(check_sum);
    EXPECT_GE(file_size, meta_length);

    magic_number = *(uint64_t *)ptr;
    EXPECT_EQ(magic_number, 0x00dd3344);
    ptr += sizeof(magic_number);

    buffer_length = *(size_t *)ptr;
    EXPECT_EQ(buffer_length / sizeof(T), expect.size());
    EXPECT_EQ(meta_length + buffer_length, file_size);
    ptr += sizeof(buffer_length);

    for (size_t i = 0; i < expect.size(); i++) {
        T value = *(T *)ptr;
        EXPECT_EQ(value, expect[i]);
        ptr += sizeof(T);
    }

    check_sum = *(uint64_t *)ptr;
    EXPECT_EQ(check_sum, 0);
}

TEST_F(PhysicalImportTest, test1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    Config config;
    config.Init(nullptr); // default configuration

    Storage storage(&config);
    storage.Init();

    auto resource_manager = MakeUnique<ResourceManager>(
        config.total_cpu_number(), config.total_memory_size()
    );

    auto session_ptr = MakeUnique<Session>();
    auto txn_mgr = storage.txn_manager();
    auto txn = MakeUnique<Txn>(txn_mgr, nullptr, 0);
    session_ptr->txn_ = txn.get();

    auto query_context = MakeUnique<QueryContext>();
    query_context->Init(
        session_ptr.get(), &config, nullptr, &storage, resource_manager.get()
    );

    //----------------------//

    auto db_name = MakeShared<String>("default");
    auto base_dir1 = MakeShared<String>("/tmp/infinity/data/default");
    u64 txn_id = 0;
    TxnTimeStamp timestamp = 0;
    auto db_entry = MakeUnique<DBEntry>(
        base_dir1, db_name, txn_id, timestamp
    );

    auto table_name = MakeShared<String>("t1");
    auto base_dir2 = MakeShared<String>("/tmp/infinity/data/default/txn_2");
    auto table_meta = MakeUnique<TableCollectionMeta>(
        base_dir2, table_name, db_entry.get()
    );

    Vector<SharedPtr<ColumnDef>> columns;
    {
        auto col_type = MakeShared<DataType>(LogicalType::kInteger);
        String col_name = "col1";
        auto col_def = MakeShared<ColumnDef>(0, col_type, col_name, HashSet<ConstraintType>());
        columns.emplace_back(col_def);

        SizeT dimension = 3;
        auto type_info = MakeShared<EmbeddingInfo>(kElemInt32, dimension);
        col_type = MakeShared<DataType>(LogicalType::kEmbedding, type_info);
        col_name = "col2";
        col_def = MakeShared<ColumnDef>(1, col_type, col_name, HashSet<ConstraintType>());
        columns.emplace_back(col_def);
    }

    auto table_entry = MakeUnique<TableCollectionEntry>(
        base_dir2, table_name, columns, TableCollectionType::kCollectionEntry,
        table_meta.get(), txn_id, timestamp
    );

    uint64_t id = 0;
    // String file_path = "test/data/csv/embedding1.csv";
    String file_path = "../../../test/data/csv/embedding_int_dim3.csv";
    bool header = false;
    char delimiter = ',';
    auto file_type = CopyFileType::kCSV;
    auto physical_import = MakeUnique<PhysicalImport>(
        id, table_entry.get(), std::move(file_path),
        header, delimiter, file_type
    );

    physical_import->ImportCSV(query_context.get());
    return;
}