#include "unit_test/base_test.h"
#include <gtest/gtest.h>

import stl;
import buffer_manager;
import data_file_worker;
import buffer_obj;
import infinity_exception;

class BufferHandleTest : public BaseTest {};

TEST_F(BufferHandleTest, test1) {
    using namespace infinity;

    SizeT memory_limit = 1024;
    auto temp_dir = MakeShared<String>("/tmp/infinity/spill");
    auto base_dir = MakeShared<String>("/tmp/infinity/data");

    BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

    SizeT test_size1 = 512;
    auto relative_dir1 = MakeShared<String>("dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(relative_dir1, test_fname1, test_size1);
    auto buf1 = buffer_manager.Allocate(std::move(file_worker1));

    SizeT test_size2 = 512;
    auto relative_dir2 = MakeShared<String>("dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(relative_dir2, test_fname2, test_size2);
    auto buf2 = buffer_manager.Allocate(std::move(file_worker2));

    SizeT test_size3 = 512;
    auto relative_dir3 = MakeShared<String>("dir3");
    auto test_fname3 = MakeShared<String>("test3");
    auto file_worker3 = MakeUnique<DataFileWorker>(relative_dir3, test_fname3, test_size3);
    auto buf3 = buffer_manager.Allocate(std::move(file_worker3));

    // Use load mut to initialize
    EXPECT_THROW({ auto buf_handle1 = buf1->Load(); }, StorageException);

    {
        auto buf_handle1 = buf1->LoadMut();
        EXPECT_EQ(buf1->rc(), 1);

        auto buf_handle2 = buf2->LoadMut();

        // out of memory
        EXPECT_THROW({ auto buf_handle3 = buf3->LoadMut(); }, StorageException);
    }

    EXPECT_EQ(buf1->rc(), 0);

    {
        auto buf_handle1 = buf1->Load();
        auto buf_handle1_1 = buf1->Load();
        EXPECT_EQ(buf1->rc(), 2);
    }

    {
        auto buf_handle1 = buf1->LoadMut();
        EXPECT_EQ(buf1->rc(), 1);

        // cannot load to mutable handler simutanously
        EXPECT_THROW({ auto buf_handle1_1 = buf1->LoadMut(); }, StorageException);
    }
}