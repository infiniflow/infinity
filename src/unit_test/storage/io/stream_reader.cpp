#include <gtest/gtest.h>

import base_test;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;

import file_writer;
import file_reader;
import infinity_context;
import stream_reader;
import virtual_store;

using namespace infinity;

class StreamReaderTest : public BaseTest {};

TEST_F(StreamReaderTest, TestBasicStreamIO) {
    String path = String(GetFullTmpDir()) + "/test_streamio.abc";
    FileWriter file_writer(path, 128);

    String lines[5];
    lines[0] = "hahahahha";
    lines[1] = "xixixixiix";
    lines[2] = "huhuhuhu";
    lines[3] = "xuxuxuxuxxu";
    lines[4] = "ddddd";

    for (i64 i = 0; i < 5; i++) {
        file_writer.Write(lines[i].c_str(), lines[i].size());
        file_writer.Write("\n", 1);
    }
    file_writer.Sync();

    UniquePtr<StreamReader> stream = LocalStore::OpenStreamReader(path);
    i32 i = 0;
    String line;
    while (stream->ReadLine(line)) {
        EXPECT_STREQ(line.c_str(), lines[i].c_str());
        i++;
    }
    EXPECT_EQ(i, 5);
}