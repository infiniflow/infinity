// #include "unit_test/base_test.h"

// import infinity;
// import infinity_exception;
// import infinity_assert;
// import stl;
// import buffer_manager;
// import buffer_handle;
// import buffer_obj;
// import data_file_worker;

// class BufferObjTest : public BaseTest {};

// // Test status transfer of buffer handle.
// // ?? status transfer in all
// TEST_F(BufferObjTest, test1) {
//     using namespace infinity;

//     SizeT memory_limit = 1024;
//     auto temp_dir = MakeShared<String>("/tmp/infinity/spill");
//     auto base_dir = MakeShared<String>("/tmp/infinity/data");

//     BufferManager buffer_manager(memory_limit, base_dir, temp_dir);

//     SizeT test_size1 = 1024;
//     auto relative_dir1 = MakeShared<String>("dir1");
//     auto test_fname1 = MakeShared<String>("test1");
//     auto file_worker1 = MakeUnique<DataFileWorker>(relative_dir1, test_fname1, test_size1);
//     auto buf1 = buffer_manager.Allocate(std::move(file_worker1));

//     SizeT test_size2 = 1024;
//     auto relative_dir2 = MakeShared<String>("dir2");
//     auto test_fname2 = MakeShared<String>("test2");
//     auto file_worker2 = MakeUnique<DataFileWorker>(relative_dir2, test_fname2, test_size2);
//     auto buf2 = buffer_manager.Allocate(std::move(file_worker2));

//     // kNew, kEphemeral
//     EXPECT_EQ(buf1->status_, BufferStatus::kNew);
//     EXPECT_EQ(buf1->type_, BufferType::kEphemeral);

//     {
//         auto handle1 = buf1->LoadMut();
//         // kNew, kEphemeral -> kLoadedMutable, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoadedMutable);
//     }

//     // kLoadedMutable, kEphemeral -> kUnloadedModified, kEphemeral
//     EXPECT_EQ(buf1->status_, BufferStatus::kUnloadedModified);

//     {
//         auto handle1 = buf1->LoadMut();
//         // kUnloadedModified, kEphemeral -> kLoadedMutable, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoadedMutable);
//     }

//     {
//         auto handle1 = buf1->Load();
//         // kLoadedMutable, kEphemeral -> kLoadedUnsaved, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoadedUnsaved);
//     }

//     // kLoadedUnsaved, kEphemeral -> kUnloadedModified, kEphemeral
//     EXPECT_EQ(buf1->status_, BufferStatus::kUnloadedModified);

//     { auto handle2 = buf2->LoadMut(); }
//     // kUnloadedModified, kEphemeral -> kFreed, kEphemeral
//     EXPECT_EQ(buf1->status_, BufferStatus::kFreed);

//     {
//         auto handle1 = buf1->LoadMut();
//         // kFreed, kEphemeral -> kLoadedMutable, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoadedMutable);
//     }
//     { auto handle2 = buf2->Load(); }

//     {
//         auto handle1 = buf1->Load();
//         // kFreed, kEphemeral -> kLoaded kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//     }

//     // kLoaded, kEphemeral -> kUnloaded, kEphemeral
//     EXPECT_EQ(buf1->status_, BufferStatus::kUnloaded);

//     {
//         auto handle1 = buf1->Load();
//         // kUnloaded, kEphemeral -> kLoaded, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//     }

//     { auto handle2 = buf2->Load(); }
//     // kUnloaded, kEphemeral -> kFreed, kEphemeral
//     EXPECT_EQ(buf1->status_, BufferStatus::kFreed);

//     buf1->Save();
//     // kFreed, kEphemeral -> kFreed, kPersistent
//     EXPECT_EQ(buf1->type_, BufferType::kPersistent);

//     {
//         auto handle1 = buf1->Load();
//         // kFreed, kPersistent -> kLoaded, kPersistent
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//     }

//     // kLoaded, kPersistent -> kUnloaded, kPersistent
//     EXPECT_EQ(buf1->status_, BufferStatus::kUnloaded);

//     {
//         auto handle1 = buf1->Load();
//         // kUnloaded, kPersistent -> kLoaded, kPersistent
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//     }

//     { auto handle2 = buf2->Load(); }
//     // kUnloaded, kPersistent -> kFreed, kPersistent
//     EXPECT_EQ(buf1->status_, BufferStatus::kFreed);

//     {
//         auto handle1 = buf1->LoadMut();
//         // kFreed, kPersistent -> kLoadedMutable, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoadedMutable);
//         EXPECT_EQ(buf1->type_, BufferType::kEphemeral);
//     }

//     buf1->Save();
//     // kUnloadedModified, kEphemeral -> kUnloaded, kPersistent
//     EXPECT_EQ(buf1->status_, BufferStatus::kUnloaded);
//     EXPECT_EQ(buf1->type_, BufferType::kPersistent);

//     {
//         auto handle1 = buf1->LoadMut();
//         // kUnloaded, kPersistent -> kLoadedMutable, kEphemeral
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoadedMutable);
//         EXPECT_EQ(buf1->type_, BufferType::kEphemeral);

//         buf1->Save();
//         // kLoadedMutable, kEphemeral -> kLoaded, kPersistent
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//         EXPECT_EQ(buf1->type_, BufferType::kPersistent);
//     }

//     { auto handle1 = buf1->LoadMut(); }

//     {
//         auto handle1 = buf1->Load();
//         buf1->Save();
//         // kLoadedUnsaved, kEphemeral -> kLoaded, kPersistent
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//         EXPECT_EQ(buf1->type_, BufferType::kPersistent);
//     }

//     { auto handle1 = buf1->LoadMut(); }
//     { auto handle2 = buf2->Load(); }
//     {
//         auto handle1 = buf1->Load();
//         buf1->Save();
//         // kLoaded, kPersistent -> kLoaded, kPersistent
//         EXPECT_EQ(buf1->status_, BufferStatus::kLoaded);
//         EXPECT_EQ(buf1->type_, BufferType::kPersistent);
//     }
//     { auto handle1 = buf1->LoadMut(); }
//     { auto handle2 = buf2->Load(); }
//     { auto handle1 = buf1->Load(); }
//     buf1->Save();
//     // kUnloaded, kPersistent -> kUnloaded, kPersistent
//     EXPECT_EQ(buf1->status_, BufferStatus::kUnloaded);
//     EXPECT_EQ(buf1->type_, BufferType::kPersistent);
// }
