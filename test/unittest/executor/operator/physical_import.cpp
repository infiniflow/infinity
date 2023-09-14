#include <gtest/gtest.h>
#include <fstream>

#include "executor/operator/physical_import.h"
#include "base_test.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "function/table/table_scan.h"
#include "main/infinity.h"
#include "main/query_context.h"
#include "main/stats/global_resource_usage.h"
#include "main/session.h"
#include "parser/statement/extra/create_table_info.h"
#include "storage/meta/catalog.h"
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
    auto catalog = MakeUnique<NewCatalog>(MakeShared<String>("/tmp/infinity"));
    RegisterTableScanFunction(catalog);
    
    Config config;
    config.Init(nullptr);

    Storage storage(&config);
    storage.Init();

    // create dummy query_context
    auto session_ptr = MakeUnique<Session>();

    auto buffer_mgr = storage.buffer_manager();
    auto txn_mgr = storage.txn_manager();
    auto txn = MakeUnique<Txn>(txn_mgr, catalog.get(), 0);
    session_ptr->txn_ = txn.get();

    UniquePtr<ResourceManager> resource_manager = MakeUnique<ResourceManager>(config.total_cpu_number(), config.total_memory_size());

    auto query_context = MakeUnique<QueryContext>();
    query_context->Init(session_ptr.get(), &config, nullptr, &storage, resource_manager.get());

    auto base_dir = MakeShared<String>("./table1");
    auto table_collection_name = MakeShared<String>("table1");

    Vector<SharedPtr<ColumnDef>> columns;
    Vector<SharedPtr<DataType>> column_types;
    auto col_type = MakeShared<DataType>(LogicalType::kBoolean);
    column_types.emplace_back(col_type);
    String col_name = "col1";
    auto col_def = MakeShared<ColumnDef>(0, col_type, col_name, HashSet<ConstraintType>());
    columns.emplace_back(col_def);

    SizeT dimension = 2;
    auto type_info = MakeShared<EmbeddingInfo>(kElemInt32, dimension);
    col_type = MakeShared<DataType>(LogicalType::kEmbedding, type_info);
    column_types.emplace_back(col_type);
    col_name = "col2";
    col_def = MakeShared<ColumnDef>(1, col_type, col_name, HashSet<ConstraintType>());
    columns.emplace_back(col_def);

    auto table_collection_meta = MakeUnique<TableCollectionMeta>(base_dir, table_collection_name, nullptr);

    auto table_collection_entry = MakeUnique<TableCollectionEntry>(
        base_dir, table_collection_name, columns, TableCollectionType::kTableEntry, table_collection_meta.get(), 0, 0);
    auto physical_import = MakeUnique<PhysicalImport>(
        0, table_collection_entry.get(), "./test/data/csv/embedding1.csv", false, ',', CopyFileType::kCSV);
    
    physical_import->ImportCSV(query_context.get());
    
    size_t row_num = 2;
    std::string col1_path = "/tmp/infinity/data/table1/0/0.col";
    std::string col2_path = "/tmp/infinity/data/table1/0/1.col";
    std::vector<bool> expect_col1{true, false, true};
    std::vector<std::array<int, 2>> expect_col2{{2, 3}, {5, 6}, {8, 9}};
    check_column(expect_col1, col1_path);
    check_column(expect_col2, col2_path);
}