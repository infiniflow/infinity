#include "gtest/gtest.h"
import base_test;
import stl;
import persistence_manager;
import virtual_storage;
import virtual_storage_type;
import abstract_file_handle;
import file_system_type;
import third_party;
import persist_result_handler;

using namespace infinity;
namespace fs = std::filesystem;

class PersistenceManagerTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::RemoveDbDirs();
        workspace_ = String(GetFullTmpDir()) + "/persistence";
        file_dir_ = String(GetFullTmpDir()) + "/persistence_src";
        system(("mkdir -p " + workspace_).c_str());
        system(("mkdir -p " + file_dir_).c_str());
        pm_ = MakeUnique<PersistenceManager>(workspace_, file_dir_, ObjSizeLimit);
        handler_ = MakeUnique<PersistResultHandler>(pm_.get());
    }
    void CheckObjData(const String& obj_addr, const String& data);

protected:
    String workspace_{};
    String file_dir_{};
    UniquePtr<PersistenceManager> pm_{};
    static constexpr int ObjSizeLimit = 128;
    UniquePtr<PersistResultHandler> handler_;
};

void PersistenceManagerTest::CheckObjData(const String& local_file_path, const String& data) {
    PersistReadResult result = pm_->GetObjCache(local_file_path);
    const ObjAddr &obj_addr = handler_->HandleReadResult(result);
    String obj_path = pm_->GetObjPath(obj_addr.obj_key_);
    fs::path obj_fp(obj_path);
    ASSERT_TRUE(fs::exists(obj_fp));
    ASSERT_EQ(obj_addr.part_size_, data.size());
    SizeT obj_file_size = fs::file_size(obj_fp);
    ASSERT_LE(obj_file_size, ObjSizeLimit);

    VirtualStorage virtual_storage;
    Map<String, String> configs;
    virtual_storage.Init(StorageType::kLocal, configs);
    auto [pm_file_handle, status] = virtual_storage.BuildFileHandle();
    EXPECT_TRUE(status.ok());

    status = pm_file_handle->Open(obj_path, FileAccessMode::kRead);
    EXPECT_TRUE(status.ok());
    status = pm_file_handle->Seek(obj_addr.part_offset_);
    EXPECT_TRUE(status.ok());
    auto file_size = obj_addr.part_size_;
    auto buffer = std::make_unique<char[]>(file_size);
    auto [nread, read_status] = pm_file_handle->Read(buffer.get(), file_size);
    EXPECT_TRUE(read_status.ok());
    ASSERT_EQ(String(buffer.get(), file_size), data);
    pm_file_handle->Close();

    pm_->PutObjCache(local_file_path);
}

TEST_F(PersistenceManagerTest, PersistFileBasic) {
    String file_path = file_dir_ + "/persist_file";
    std::ofstream out_file(file_path);
    String persist_str = "Persistence Manager Test";
    out_file << persist_str;
    out_file.close();
    PersistWriteResult result = pm_->Persist(file_path, file_path);
    handler_->HandleWriteResult(result);
    const ObjAddr &obj_addr = result.obj_addr_;
    ASSERT_TRUE(obj_addr.Valid());
    ASSERT_EQ(obj_addr.part_size_, persist_str.size());
    PersistWriteResult result2 = pm_->CurrentObjFinalize();
    handler_->HandleWriteResult(result2);

    CheckObjData(file_path, persist_str);
}

TEST_F(PersistenceManagerTest, PersistMultiFile) {
    String file_path_base = file_dir_ + "/persist_file";
    Vector<String> file_paths;
    Vector<String> persist_strs;
    Vector<ObjAddr> obj_addrs;
    for (SizeT i = 0; i < 10; ++i) {
        String file_path = file_path_base + std::to_string(i);
        std::ofstream out_file(file_path);
        String persist_str = "Persistence Manager Test " + std::to_string(i);
        out_file << persist_str;
        out_file.close();
        file_paths.push_back(file_path);
        persist_strs.push_back(persist_str);

        PersistWriteResult result = pm_->Persist(file_path, file_path);
        handler_->HandleWriteResult(result);
        const ObjAddr &obj_addr = result.obj_addr_;
        ASSERT_TRUE(obj_addr.Valid());
        ASSERT_EQ(obj_addr.part_size_, persist_str.size());
        obj_addrs.push_back(obj_addr);
    }
    ASSERT_EQ(file_paths.size(), persist_strs.size());
    ASSERT_EQ(file_paths.size(), obj_addrs.size());
    PersistWriteResult result = pm_->CurrentObjFinalize();
    handler_->HandleWriteResult(result);

    for (SizeT i = 0; i < file_paths.size(); ++i) {
        CheckObjData(file_paths[i], persist_strs[i]);
    }
}

TEST_F(PersistenceManagerTest, PersistFileMultiThread) {
    String file_path_base = file_dir_ + "/persist_file";
    Vector<String> file_paths;
    Vector<String> persist_strs;
    HashMap<String, ObjAddr> obj_addrs;
    Vector<std::thread> threads;
    std::mutex obj_mutex;

    for (SizeT i = 0; i < 10; ++i) {
        String file_path = file_path_base + std::to_string(i);
        std::ofstream out_file(file_path);
        String persist_str = "Persistence Manager Test " + std::to_string(i);
        out_file << persist_str;
        out_file.close();
        file_paths.push_back(file_path);
        persist_strs.push_back(persist_str);

        threads.emplace_back([this, file_path, persist_str, &obj_addrs, &obj_mutex]() {
            PersistWriteResult result = pm_->Persist(file_path, file_path);
            handler_->HandleWriteResult(result);
            const ObjAddr &obj_addr = result.obj_addr_;
            ASSERT_TRUE(obj_addr.Valid());
            ASSERT_EQ(obj_addr.part_size_, persist_str.size());
            std::unique_lock<std::mutex> lock(obj_mutex);
            obj_addrs[file_path] = obj_addr;
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    ASSERT_EQ(file_paths.size(), persist_strs.size());
    ASSERT_EQ(file_paths.size(), obj_addrs.size());
    PersistWriteResult result = pm_->CurrentObjFinalize();
    handler_->HandleWriteResult(result);

    for (SizeT i = 0; i < file_paths.size(); ++i) {
        CheckObjData(file_paths[i], persist_strs[i]);
    }
}

TEST_F(PersistenceManagerTest, CleanupBasic) {
    String file_path_base = file_dir_ + "/persist_file";
    Vector<String> file_paths;
    Vector<String> persist_strs;
    Vector<ObjAddr> obj_addrs;
    Set<String> obj_paths;

    for (SizeT i = 0; i < 10; ++i) {
        String file_path = file_path_base + std::to_string(i);
        std::ofstream out_file(file_path);
        String persist_str = "Persistence Manager Test " + std::to_string(i);
        out_file << persist_str;
        out_file.close();
        file_paths.push_back(file_path);
        persist_strs.push_back(persist_str);

        PersistWriteResult result = pm_->Persist(file_path, file_path);
        handler_->HandleWriteResult(result);
        const ObjAddr &obj_addr = result.obj_addr_;
        ASSERT_TRUE(obj_addr.Valid());
        ASSERT_EQ(obj_addr.part_size_, persist_str.size());
        obj_addrs.push_back(obj_addr);
        obj_paths.insert(workspace_ + "/" + obj_addr.obj_key_);
    }
    ASSERT_EQ(file_paths.size(), persist_strs.size());
    ASSERT_EQ(file_paths.size(), obj_addrs.size());
    PersistWriteResult result = pm_->CurrentObjFinalize();
    handler_->HandleWriteResult(result);

    for (SizeT i = 0; i < file_paths.size(); ++i) {
        CheckObjData(file_paths[i], persist_strs[i]);
    }

    for (const auto& obj_path : obj_paths) {
        ASSERT_TRUE(fs::exists(obj_path));
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(file_paths.begin(), file_paths.end(), g);

    for (auto& file_path : file_paths) {
        PersistWriteResult result = pm_->Cleanup(file_path);
        handler_->HandleWriteResult(result);
    }
    for (const auto& obj_path : obj_paths) {
        ASSERT_FALSE(fs::exists(obj_path));
    }
}