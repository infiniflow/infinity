// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include "unit_test/base_test.h"

import compilation_config;
import buffer_manager;
import buffer_obj;
import buffer_handle;
import parser;
import file_worker;

import block_column_iter;
import block_iter;
import segment_iter;
import catalog;

using namespace infinity;

class IteratorTest : public BaseTest {
public:
    class MockFileWorker : public FileWorker {
        void AllocateInMemory() override {}
        void FreeInMemory() override {}
        size_t GetMemoryCost() const override { return 0; }
        void WriteToFileImpl(bool &) override {}
        void ReadFromFileImpl() override {}

    public:
        MockFileWorker(void *data) : FileWorker(nullptr, nullptr) { data_ = data; }
    };

    class MockBufferObj : public BufferObj {

    public:
        MockBufferObj(BufferManager *buffer_mgr, void *data) : BufferObj(buffer_mgr, true, std::make_unique<MockFileWorker>(data)) {
            status_ = BufferStatus::kLoaded;
            rc_ = 1;
        }
    };

    class MockBlockColumnEntry : public BlockColumnEntry {
    public:
        MockBlockColumnEntry(uint64_t column_id, BufferObj *buffer_obj, std::shared_ptr<DataType> column_type)
            : BlockColumnEntry(nullptr, column_id, nullptr) {
            buffer_ = buffer_obj;
            column_type_ = column_type;
        }
    };

    class MockBlockEntry : public BlockEntry {
    public:
        MockBlockEntry(std::vector<std::unique_ptr<BlockColumnEntry>> columns, size_t row_cnt) : BlockEntry() {
            columns_ = std::move(columns);
            row_count_ = row_cnt;
        }
    };

    class MockSegmentEntry : public SegmentEntry {
    public:
        MockSegmentEntry(std::vector<std::shared_ptr<BlockEntry>> blocks) : SegmentEntry(nullptr) { block_entries_ = std::move(blocks); }
    };

    class MockTableEntry : public TableEntry {
    public:
        MockTableEntry(std::map<uint32_t, std::shared_ptr<SegmentEntry>> segments, SegmentEntry *unsealed) : TableEntry() {
            segment_map_ = std::move(segments);
            unsealed_segment_ = unsealed;
        }
    };
};

TEST_F(IteratorTest, block_column_iter_test) {
    size_t row_cnt = 10;
    std::unique_ptr<int[]> data(new int[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        data[i] = i;
    }
    std::unique_ptr<char[]> data_char(reinterpret_cast<char *>(data.release()));

    BufferManager buffer_mgr(0, std::make_shared<std::string>(tmp_data_path()), std::make_shared<std::string>(tmp_data_path()));

    MockBufferObj buffer_obj(&buffer_mgr, data_char.get());

    MockBlockColumnEntry entry(0, &buffer_obj, std::make_shared<DataType>(kInteger, nullptr));

    BlockColumnIter iter(&entry, 10);
    for (size_t i = 0; i < row_cnt; ++i) {
        auto ret = iter.Next();
        ASSERT_TRUE(ret.has_value());
        auto a = *reinterpret_cast<const int *>(ret.value());
        ASSERT_EQ(a, i);
    }
    ASSERT_FALSE(iter.Next().has_value());
}

TEST_F(IteratorTest, block_iter_test) {
    size_t row_cnt = 10;
    BufferManager buffer_mgr(0, std::make_shared<std::string>(tmp_data_path()), std::make_shared<std::string>(tmp_data_path()));

    std::unique_ptr<int[]> col1(new int[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        col1[i] = i;
    }
    std::unique_ptr<char[]> col1_char(reinterpret_cast<char *>(col1.release()));
    MockBufferObj buffer_obj1(&buffer_mgr, col1_char.get());

    std::unique_ptr<double[]> col2(new double[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        col2[i] = (double)i + 0.5;
    }
    std::unique_ptr<char[]> col2_char(reinterpret_cast<char *>(col2.release()));
    MockBufferObj buffer_obj2(&buffer_mgr, col2_char.get());

    std::vector<std::unique_ptr<BlockColumnEntry>> columns;
    columns.emplace_back(std::make_unique<MockBlockColumnEntry>(0, &buffer_obj1, std::make_shared<DataType>(kInteger, nullptr)));
    columns.emplace_back(std::make_unique<MockBlockColumnEntry>(1, &buffer_obj2, std::make_shared<DataType>(kDouble, nullptr)));

    MockBlockEntry entry(std::move(columns), row_cnt);

    BlockIter iter(&entry, {0, 1});
    for (size_t i = 0; i < row_cnt; ++i) {
        auto ret = iter.Next();
        ASSERT_TRUE(ret.has_value());
        auto a = *reinterpret_cast<const int *>(ret.value()[0]);
        auto b = *reinterpret_cast<const double *>(ret.value()[1]);
        ASSERT_EQ(a, i);
        ASSERT_EQ(b, (double)i + 0.5);
    }
    ASSERT_FALSE(iter.Next().has_value());
}

TEST_F(IteratorTest, segment_iter_test) {
    size_t row_cnt = 10;
    size_t block1_row = 5;
    BufferManager buffer_mgr(0, std::make_shared<std::string>(tmp_data_path()), std::make_shared<std::string>(tmp_data_path()));

    std::unique_ptr<int[]> col1(new int[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        col1[i] = i;
    }
    std::unique_ptr<char[]> col1_char(reinterpret_cast<char *>(col1.release()));

    std::unique_ptr<double[]> col2(new double[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        col2[i] = (double)i + 0.5;
    }
    std::unique_ptr<char[]> col2_char(reinterpret_cast<char *>(col2.release()));

    MockBufferObj buffer_obj_col1_block1(&buffer_mgr, col1_char.get());
    MockBufferObj buffer_obj_col2_block1(&buffer_mgr, col2_char.get());

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_block1;
    columns_block1.emplace_back(std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_block1, std::make_shared<DataType>(kInteger, nullptr)));
    columns_block1.emplace_back(std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_block1, std::make_shared<DataType>(kDouble, nullptr)));
    auto block1 = std::make_shared<MockBlockEntry>(std::move(columns_block1), block1_row);

    MockBufferObj buffer_obj_col1_block2(&buffer_mgr, col1_char.get() + sizeof(int) * block1_row);
    MockBufferObj buffer_obj_col2_block2(&buffer_mgr, col2_char.get() + sizeof(double) * block1_row);

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_block2;
    columns_block2.emplace_back(std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_block2, std::make_shared<DataType>(kInteger, nullptr)));
    columns_block2.emplace_back(std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_block2, std::make_shared<DataType>(kDouble, nullptr)));
    auto block2 = std::make_shared<MockBlockEntry>(std::move(columns_block2), row_cnt - block1_row);

    std::vector<std::shared_ptr<BlockEntry>> blocks{block1, block2};

    MockSegmentEntry entry(blocks);

    SegmentIter iter(&entry, std::make_shared<std::vector<size_t>>(std::vector<size_t>{0, 1}));
    for (size_t i = 0; i < row_cnt; ++i) {
        auto ret = iter.Next();
        ASSERT_TRUE(ret.has_value());
        auto a = *reinterpret_cast<const int *>(ret.value()[0]);
        auto b = *reinterpret_cast<const double *>(ret.value()[1]);
        ASSERT_EQ(a, i);
        ASSERT_EQ(b, (double)i + 0.5);
    }
    ASSERT_FALSE(iter.Next().has_value());
}

TEST_F(IteratorTest, table_iter_test) {
    size_t row_cnt = 25;
    size_t seal_row = 20;
    size_t segment1_row = 10;
    size_t block1_row = 5;
    size_t segment2_row = seal_row - segment1_row;

    BufferManager buffer_mgr(0, std::make_shared<std::string>(tmp_data_path()), std::make_shared<std::string>(tmp_data_path()));

    std::unique_ptr<int[]> col1(new int[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        col1[i] = i;
    }
    std::unique_ptr<char[]> col1_char(reinterpret_cast<char *>(col1.release()));

    std::unique_ptr<double[]> col2(new double[row_cnt]);
    for (size_t i = 0; i < row_cnt; ++i) {
        col2[i] = (double)i + 0.5;
    }
    std::unique_ptr<char[]> col2_char(reinterpret_cast<char *>(col2.release()));

    MockBufferObj buffer_obj_col1_blk1_seg1(&buffer_mgr, col1_char.get());
    MockBufferObj buffer_obj_col2_blk1_seg1(&buffer_mgr, col2_char.get());

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_blk1_seg1;
    columns_blk1_seg1.emplace_back(
        std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_blk1_seg1, std::make_shared<DataType>(kInteger, nullptr)));
    columns_blk1_seg1.emplace_back(
        std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_blk1_seg1, std::make_shared<DataType>(kDouble, nullptr)));

    auto block1_seg1 = std::make_shared<MockBlockEntry>(std::move(columns_blk1_seg1), block1_row);

    MockBufferObj buffer_obj_col1_blk2_seg1(&buffer_mgr, col1_char.get() + sizeof(int) * block1_row);
    MockBufferObj buffer_obj_col2_blk2_seg1(&buffer_mgr, col2_char.get() + sizeof(double) * block1_row);

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_blk2_seg1;
    columns_blk2_seg1.emplace_back(
        std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_blk2_seg1, std::make_shared<DataType>(kInteger, nullptr)));
    columns_blk2_seg1.emplace_back(
        std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_blk2_seg1, std::make_shared<DataType>(kDouble, nullptr)));

    auto block2_seg1 = std::make_shared<MockBlockEntry>(std::move(columns_blk2_seg1), segment1_row - block1_row);

    std::vector<std::shared_ptr<BlockEntry>> blocks_seg1{block1_seg1, block2_seg1};

    auto segment1 = std::make_shared<MockSegmentEntry>(blocks_seg1);

    //------------------------------------------

    MockBufferObj buffer_obj_col1_blk1_seg2(&buffer_mgr, col1_char.get() + sizeof(int) * segment1_row);
    MockBufferObj buffer_obj_col2_blk1_seg2(&buffer_mgr, col2_char.get() + sizeof(double) * segment1_row);

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_blk1_seg2;
    columns_blk1_seg2.emplace_back(
        std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_blk1_seg2, std::make_shared<DataType>(kInteger, nullptr)));
    columns_blk1_seg2.emplace_back(
        std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_blk1_seg2, std::make_shared<DataType>(kDouble, nullptr)));

    auto block1_seg2 = std::make_shared<MockBlockEntry>(std::move(columns_blk1_seg2), block1_row);

    MockBufferObj buffer_obj_col1_blk2_seg2(&buffer_mgr, col1_char.get() + sizeof(int) * (segment1_row + block1_row));
    MockBufferObj buffer_obj_col2_blk2_seg2(&buffer_mgr, col2_char.get() + sizeof(double) * (segment1_row + block1_row));

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_blk2_seg2;
    columns_blk2_seg2.emplace_back(
        std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_blk2_seg2, std::make_shared<DataType>(kInteger, nullptr)));
    columns_blk2_seg2.emplace_back(
        std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_blk2_seg2, std::make_shared<DataType>(kDouble, nullptr)));

    auto block2_seg2 = std::make_shared<MockBlockEntry>(std::move(columns_blk2_seg2), segment2_row - block1_row);

    std::vector<std::shared_ptr<BlockEntry>> blocks_seg2{block1_seg2, block2_seg2};

    auto segment2 = std::make_shared<MockSegmentEntry>(blocks_seg2);

    //------------------------------------------

    MockBufferObj buffer_obj_col1_blk_unsealed(&buffer_mgr, col1_char.get() + sizeof(int) * seal_row);
    MockBufferObj buffer_obj_col2_blk_unsealed(&buffer_mgr, col2_char.get() + sizeof(double) * seal_row);

    std::vector<std::unique_ptr<BlockColumnEntry>> columns_blk_unsealed;
    columns_blk_unsealed.emplace_back(
        std::make_unique<MockBlockColumnEntry>(0, &buffer_obj_col1_blk_unsealed, std::make_shared<DataType>(kInteger, nullptr)));
    columns_blk_unsealed.emplace_back(
        std::make_unique<MockBlockColumnEntry>(1, &buffer_obj_col2_blk_unsealed, std::make_shared<DataType>(kDouble, nullptr)));

    auto block_unsealed = std::make_shared<MockBlockEntry>(std::move(columns_blk_unsealed), row_cnt - seal_row);

    std::vector<std::shared_ptr<BlockEntry>> blocks_unsealed{block_unsealed};

    auto unsealed_segment = std::make_shared<MockSegmentEntry>(blocks_unsealed);

    //------------------------------------------

    std::map<uint32_t, std::shared_ptr<SegmentEntry>> segments;
    segments.emplace(0, segment1);
    segments.emplace(1, segment2);

    MockTableEntry entry(segments, unsealed_segment.get());
}
