#include "gtest/gtest.h"
import base_test;
import stl;
import persistence_manager;
import local_file_system;
import file_system_type;
import third_party;

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
    }
    void CheckObjData(const String& obj_addr, const String& data);

protected:
    String workspace_{};
    String file_dir_{};
    UniquePtr<PersistenceManager> pm_{};
    static constexpr int ObjSizeLimit = 128;
};

void PersistenceManagerTest::CheckObjData(const String& local_file_path, const String& data) {
    auto obj_addr = pm_->GetObjCache(local_file_path);
    String obj_path = pm_->GetObjPath(obj_addr.obj_key_);
    fs::path obj_fp(obj_path);
    ASSERT_TRUE(fs::exists(obj_fp));
    ASSERT_EQ(obj_addr.part_size_, data.size());
    SizeT obj_file_size = fs::file_size(obj_fp);
    ASSERT_LE(obj_file_size, ObjSizeLimit);

    LocalFileSystem local_fs;
    auto [file_handler, status] = local_fs.OpenFile(obj_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    ASSERT_TRUE(status.ok());
    local_fs.Seek(*file_handler, obj_addr.part_offset_);
    auto file_size = obj_addr.part_size_;
    auto buffer = std::make_unique<char[]>(file_size);
    local_fs.Read(*file_handler, buffer.get(), file_size);
    ASSERT_EQ(String(buffer.get(), file_size), data);
    local_fs.Close(*file_handler);

    pm_->PutObjCache(local_file_path);
}

TEST_F(PersistenceManagerTest, PersistFileBasic) {
    String file_path = file_dir_ + "/persist_file";
    std::ofstream out_file(file_path);
    String persist_str = "Persistence Manager Test";
    out_file << persist_str;
    out_file.close();
    ObjAddr obj_addr = pm_->Persist(file_path, file_path);
    ASSERT_TRUE(obj_addr.Valid());
    ASSERT_EQ(obj_addr.part_size_, persist_str.size());
    pm_->CurrentObjFinalize();

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

        ObjAddr obj_addr = pm_->Persist(file_path, file_path);
        ASSERT_TRUE(obj_addr.Valid());
        ASSERT_EQ(obj_addr.part_size_, persist_str.size());
        obj_addrs.push_back(obj_addr);
    }
    ASSERT_EQ(file_paths.size(), persist_strs.size());
    ASSERT_EQ(file_paths.size(), obj_addrs.size());
    pm_->CurrentObjFinalize();

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
            ObjAddr obj_addr = pm_->Persist(file_path, file_path);
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
    pm_->CurrentObjFinalize();

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

        ObjAddr obj_addr = pm_->Persist(file_path, file_path);
        ASSERT_TRUE(obj_addr.Valid());
        ASSERT_EQ(obj_addr.part_size_, persist_str.size());
        obj_addrs.push_back(obj_addr);
        obj_paths.insert(workspace_ + "/" + obj_addr.obj_key_);
    }
    ASSERT_EQ(file_paths.size(), persist_strs.size());
    ASSERT_EQ(file_paths.size(), obj_addrs.size());
    pm_->CurrentObjFinalize();

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
        pm_->Cleanup(file_path);
    }
    for (const auto& obj_path : obj_paths) {
        ASSERT_FALSE(fs::exists(obj_path));
    }
}