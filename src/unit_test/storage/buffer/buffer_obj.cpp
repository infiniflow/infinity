#include "unit_test/base_test.h"

import infinity;
import infinity_exception;

import stl;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import data_file_worker;

class BufferObjTest : public BaseTest {
    void SetUp() override { system("rm -rf /tmp/infinity"); }
};

// Test status transfer of buffer handle.
// ?? status transfer in all
TEST_F(BufferObjTest, test1) {
    using namespace infinity;

    SizeT memory_limit = 1024;
    auto temp_dir = MakeShared<String>("/tmp/infinity/spill");
    auto base_dir = MakeShared<String>("/tmp/infinity/data");

    BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

    SizeT test_size1 = 1024;
    auto file_dir1 = MakeShared<String>("/tmp/infinity/data/dir1");
    auto test_fname1 = MakeShared<String>("test1");
    auto file_worker1 = MakeUnique<DataFileWorker>(file_dir1, test_fname1, test_size1);
    auto buf1 = buffer_manager.Allocate(std::move(file_worker1));

    SizeT test_size2 = 1024;
    auto file_dir2 = MakeShared<String>("/tmp/infinity/data/dir2");
    auto test_fname2 = MakeShared<String>("test2");
    auto file_worker2 = MakeUnique<DataFileWorker>(file_dir2, test_fname2, test_size2);
    auto buf2 = buffer_manager.Allocate(std::move(file_worker2));

    // kNew, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kNew);
    EXPECT_EQ(buf1->type(), BufferType::kEphemeral);

    {
        auto handle1 = buf1->Load();
        // kNew, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
    }

    // kLoaded, kEphemeral -> kUnloaded, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);

    {
        auto handle1 = buf1->Load();
        // kUnloaded, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
    }

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kEphemeral -> kFreed, kEphemeral
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);

    {
        auto handle1 = buf1->Load();
        auto data1 = handle1.GetDataMut();
        // kFreed, kEphemeral -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
    }
    { auto handle2 = buf2->Load(); }

    {
        auto handle1 = buf1->Load();
        // kFreed, kEphemeral -> kLoaded kTemp
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kTemp);
    }

    // kLoaded, kTemp -> kUnloaded, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);

    {
        auto handle1 = buf1->Load();
        // kUnloaded, kTemp -> kLoaded, kTemp
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
    }

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kTemp -> kFreed, kTemp
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);

    buf1->Save();
    // kFreed, kTemp -> kFreed, kPersistent
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);

    {
        auto handle1 = buf1->Load();
        // kFreed, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
    }

    // kLoaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);

    {
        auto handle1 = buf1->Load();
        // kUnloaded, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
    }

    { auto handle2 = buf2->Load(); }
    // kUnloaded, kPersistent -> kFreed, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kFreed);

    {
        auto handle1 = buf1->Load();
        auto data1 = handle1.GetDataMut();
        // kFreed, kPersistent -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kEphemeral);
    }

    buf1->Save();
    // kUnloadedModified, kEphemeral -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);

    {
        auto handle1 = buf1->Load();
        auto data1 = handle1.GetDataMut();
        // kUnloaded, kPersistent -> kLoaded, kEphemeral
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kEphemeral);

        buf1->Save();
        // kLoaded, kEphemeral -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kPersistent);
    }

    {
        auto handle1 = buf1->Load();
        auto data1 = handle1.GetDataMut();
    }

    {
        auto handle1 = buf1->Load();
        buf1->Save();
        // kLoaded, kEphemeral -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kPersistent);
    }

    {
        auto handle1 = buf1->Load();
        auto data1 = handle1.GetDataMut();
    }
    { auto handle2 = buf2->Load(); }
    {
        auto handle1 = buf1->Load();
        buf1->Save();
        // kLoaded, kPersistent -> kLoaded, kPersistent
        EXPECT_EQ(buf1->status(), BufferStatus::kLoaded);
        EXPECT_EQ(buf1->type(), BufferType::kPersistent);
    }
    {
        auto handle1 = buf1->Load();
        auto data1 = handle1.GetDataMut();
    }
    { auto handle2 = buf2->Load(); }
    { auto handle1 = buf1->Load(); }
    buf1->Save();
    // kUnloaded, kPersistent -> kUnloaded, kPersistent
    EXPECT_EQ(buf1->status(), BufferStatus::kUnloaded);
    EXPECT_EQ(buf1->type(), BufferType::kPersistent);
}
