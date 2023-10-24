//
// Created by jinhai on 23-5-18.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import parser;
import file_system;
import local_file_system;
import file_writer;
import file_reader;
import infinity;

class FileWriteReadTest : public BaseTest {
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

TEST_F(FileWriteReadTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    LocalFileSystem local_file_system;
    String path = "/tmp/test_file1.abc";
    FileWriter file_writer(local_file_system, path, 128);

    for (SizeT i = 0; i < 128; ++i) {
        String buffer = "abc";
        file_writer.Write(buffer.c_str(), buffer.size());
    }
    file_writer.Flush();
    EXPECT_EQ(file_writer.GetFileSize(), 128 * 3);
    EXPECT_EQ(file_writer.total_written_, 128 * 3);

    FileReader file_reader(local_file_system, path, 128);
    String read_str;
    read_str.resize(4);
    file_reader.Read(read_str.data(), 4);
    EXPECT_STREQ(read_str.c_str(), "abca");
    EXPECT_FALSE(file_reader.Finished());
    local_file_system.DeleteFile(path);
}