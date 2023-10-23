//
// Created by tangdonghai on 23-9-6.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;
import new_catalog;
import scalar_function;
import scalar_function_set;
import function_set;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import base_expression;
import column_vector;
import value_expression;
import table_scan;
import config;
import session;
import storage;
import resource_manager;
import query_context;

class TableScanTest : public BaseTest {
    void SetUp() override {
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

TEST_F(TableScanTest, block_read_test) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    auto catalog = MakeUnique<NewCatalog>(MakeShared<String>("/tmp/infinity"));
//    RegisterSeqScanFunction(catalog);
    RegisterTableScanFunction(catalog);

    Config config;
    config.Init(nullptr);

    Storage storage(&config);
    storage.Init();

    // create dummy query_context
    UniquePtr<Session> session_ptr = MakeUnique<Session>();

    UniquePtr<ResourceManager> resource_manager = MakeUnique<ResourceManager>(config.total_cpu_number(), config.total_memory_size());

    UniquePtr<QueryContext> query_context = MakeUnique<QueryContext>();
    query_context->Init(session_ptr.get(), &config, nullptr, &storage, resource_manager.get());

    Vector<SharedPtr<DataType>> column_types;
    column_types.push_back(MakeShared<DataType>(LogicalType::kInteger));
    // column_types.push_back(MakeShared<DataType>(LogicalType::kDouble));
    // column_types.push_back(MakeShared<DataType>(LogicalType::kCircle));
#if 0
    // mock segment
    auto segment_entry1 = MakeShared<SegmentEntry>(nullptr);
    segment_entry1->segment_id_ = 1;
    segment_entry1->row_capacity_ = 1000;
    segment_entry1->current_row_ = 1000;

    segment_entry1->segment_dir_ = MakeShared<String>("segment1/tx1");
    segment_entry1->columns_.push_back(SegmentColumnEntry::MakeNewColumnDataEntry(segment_entry1.get(),
                                                                                  0,
                                                                                  1000,
                                                                                  MakeShared<DataType>(LogicalType::kInteger),
                                                                                  storage.buffer_manager()));

    auto block_entry1 = MakeShared<BlockEntry>(segment_entry1.get(),
                                               1, // block id
                                               1, // column count
                                               0, // start row
                                               storage.buffer_manager());

    auto segment_entry2 = MakeShared<SegmentEntry>(nullptr);
    segment_entry2->segment_id_ = 10;
    segment_entry2->row_capacity_ = 10000;
    segment_entry2->current_row_ = 8000;

    segment_entry2->base_dir_ = MakeShared<String>("segment2/tx1");
    segment_entry2->columns_.push_back(SegmentColumnEntry::MakeNewColumnDataEntry(segment_entry2.get(),
                                                                                  0,
                                                                                  8000,
                                                                                  MakeShared<DataType>(LogicalType::kInteger),
                                                                                  storage.buffer_manager()));

    auto segment_entry3 = MakeShared<SegmentEntry>(nullptr);
    segment_entry3->segment_id_ = 30;
    segment_entry3->row_capacity_ = 10000;
    segment_entry3->current_row_ = 8000;
    segment_entry3->base_dir_ = MakeShared<String>("segment3/tx1");
    segment_entry3->columns_.push_back(SegmentColumnEntry::MakeNewColumnDataEntry(segment_entry3.get(),
                                                                                  0,
                                                                                  8000,
                                                                                  MakeShared<DataType>(LogicalType::kInteger),
                                                                                  storage.buffer_manager()));

    auto segment_entry4 = MakeShared<SegmentEntry>(nullptr);
    segment_entry4->segment_id_ = 20;
    segment_entry4->row_capacity_ = 1000;
    segment_entry4->current_row_ = 408;
    segment_entry4->base_dir_ = MakeShared<String>("segment4/tx1");
    segment_entry4->columns_.push_back(SegmentColumnEntry::MakeNewColumnDataEntry(segment_entry4.get(),
                                                                                  0,
                                                                                  1000,
                                                                                  MakeShared<DataType>(LogicalType::kInteger),
                                                                                  storage.buffer_manager()));



    auto db_entry_dir = MakeShared<String>("/tmp/infinity/data/default/tx_0");
    auto table_name = MakeShared<String>("table1");
    // total_row smaller than block size
    {
        TableCollectionEntry entry(db_entry_dir, table_name, {}, TableCollectionType::kTableEntry,
                                   nullptr, 0, 0);
        entry.segments_[segment_entry1->segment_id_] = segment_entry1;

        SizeT total_row = 0;
        for(auto& [_, segment_entry]: entry.segments_) {
            total_row += segment_entry->current_row_;
        }

        Vector<SizeT> column_ids{0};
        auto table_scan_func = MakeShared<SeqScanFunctionData>(&entry,
                                                               nullptr,
                                                               column_ids);
        int times = 0;
        while(true) {

            DataBlock output;
            output.Init(column_types, 1024);
            auto func = NewCatalog::GetTableFunctionByName(catalog.get(), "seq_scan");
            func->main_function_(query_context.get(), table_scan_func.get(), output);
            if(output.row_count() == 0) {
                break;
            }
            times += 1;
        }

        EXPECT_EQ(times, ceil(total_row / 1024.0));
    }


    // total row larger than block size
    {
        TableCollectionEntry entry(db_entry_dir, table_name, {}, TableCollectionType::kTableEntry,
                                   nullptr, 0, 0);
        entry.segments_[segment_entry1->segment_id_] = segment_entry1;
        entry.segments_[segment_entry2->segment_id_] = segment_entry2;
        entry.segments_[segment_entry3->segment_id_] = segment_entry3;

        SizeT total_row = 0;
        for(auto& [_, segment_entry]: entry.segments_) {
            total_row += segment_entry->current_row_;
        }

        Vector<SizeT> column_ids{0};
        auto table_scan_func = MakeShared<SeqScanFunctionData>(&entry,
                                                               nullptr,
                                                               column_ids);
        int times = 0;
        while(true) {

            DataBlock output;
            output.Init(column_types, 1024);
            auto func = NewCatalog::GetTableFunctionByName(catalog.get(), "seq_scan");
            func->main_function_(query_context.get(), table_scan_func.get(), output);
            if(output.row_count() == 0) {
                break;
            }
            times += 1;
        }
        EXPECT_EQ(times, ceil(total_row / 1024.0));
    }

    // total row is multipy of block size
    {
        TableCollectionEntry entry(db_entry_dir, table_name, {}, TableCollectionType::kTableEntry,
                                   nullptr, 0, 0);
        entry.segments_[segment_entry1->segment_id_] = segment_entry1;
        entry.segments_[segment_entry2->segment_id_] = segment_entry2;
        entry.segments_[segment_entry3->segment_id_] = segment_entry3;
        entry.segments_[segment_entry4->segment_id_] = segment_entry4;

        SizeT total_row = 0;
        for(auto& [_, segment_entry]: entry.segments_) {
            total_row += segment_entry->current_row_;
        }

        Vector<SizeT> column_ids{0};
        auto table_scan_func = MakeShared<SeqScanFunctionData>(&entry,
                                                               nullptr,
                                                               column_ids);
        int times = 0;
        while(true) {

            DataBlock output;
            output.Init(column_types, 1024);
            auto func = NewCatalog::GetTableFunctionByName(catalog.get(), "seq_scan");
            func->main_function_(query_context.get(), table_scan_func.get(), output);
            if(output.row_count() == 0) {
                break;
            }
            times += 1;
        }
        EXPECT_EQ(times, ceil(total_row / 1024.0));
    }
#endif
}