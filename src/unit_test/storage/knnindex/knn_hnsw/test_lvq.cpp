#include "unit_test/base_test.h"
#include <iostream>
#include <random>

import lvq_store;
import local_file_system;
import file_system;
import file_system_type;

using namespace infinity;

class HnswLVQTest : public BaseTest {
public:
    using LVQ8Store = LVQStore<float, uint8_t>;
    using LVQ8 = LVQ<float, uint8_t>;

    static constexpr size_t dim_ = 16;
    static constexpr size_t vec_n_ = 20;
    static constexpr size_t buffer_size_ = 4;
    const std::string file_dir_ = "/home/shenyushi/Documents/Code/infiniflow/infinity/tmp";

    bool dump_ = false;

    void DumpStore(const LVQ8Store &store, const float *data) {
        if (!dump_) {
            return;
        }
        std::cout.widen(8);
        std::cout.fill('0');
        for (size_t i = 0; i < store.cur_vec_num(); ++i) {
            std::cout << i << std::endl;
            LVQ8 lvq = store.GetVec(i);
            auto res = std::make_unique<float[]>(dim_);
            lvq.DecompressForTest(res.get());

            std::cout << "LVQ:\t";
            for (size_t j = 0; j < 16; ++j) {
                std::cout << res[j] << " ";
            }
            std::cout << std::endl;

            std::cout << "Old:\t";
            for (size_t j = 0; j < 16; ++j) {
                std::cout << data[i * dim_ + j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "---------------" << std::endl;
    }
};

TEST_F(HnswLVQTest, test1) {
    using namespace infinity;

    auto data = std::make_unique<float[]>(dim_ * vec_n_);

    // randomize the data from 0 to 1
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real;
    for (size_t i = 0; i < dim_ * vec_n_; ++i) {
        data[i] = distrib_real(rng);
    }

    {
        LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
        auto ret = lvq_store.AddBatchVec(data.get(), vec_n_);
        EXPECT_NE(ret, LVQ8Store::err_idx());
        DumpStore(lvq_store, data.get());
    }

    {
        size_t idx = 0;
        LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
        auto ret = lvq_store.AddBatchVec(data.get(), vec_n_ / 2);
        EXPECT_NE(ret, LVQ8Store::err_idx());
        idx += vec_n_ / 2;

        ret = lvq_store.AddBatchVec(data.get() + idx * dim_, vec_n_ - idx);
        EXPECT_NE(ret, LVQ8Store::err_idx());
        DumpStore(lvq_store, data.get());
    }

    {
        size_t idx = 0;
        LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
        auto ret = lvq_store.AddBatchVec(data.get(), vec_n_ / 2);
        EXPECT_NE(ret, LVQ8Store::err_idx());
        idx += vec_n_ / 2;

        for (size_t i = 0; i < buffer_size_ && idx < vec_n_; ++i) {
            ret = lvq_store.AddVec(data.get() + idx * dim_);
            EXPECT_NE(ret, LVQ8Store::err_idx());
            ++idx;
        }
        ret = lvq_store.AddBatchVec(data.get() + idx * dim_, vec_n_ - idx);
        EXPECT_NE(ret, LVQ8Store::err_idx());

        DumpStore(lvq_store, data.get());
    }

    {
        std::string file_path = file_dir_ + "/lvq_store1.bin";
        LocalFileSystem fs;

        size_t idx = 0;
        {
            uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kWriteLock);

            LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
            auto ret = lvq_store.AddBatchVec(data.get(), vec_n_ / 2);
            EXPECT_NE(ret, LVQ8Store::err_idx());
            idx += vec_n_ / 2;

            lvq_store.Save(*file_handler);
        }
        {
            uint8_t file_flags = FileFlags::READ_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kReadLock);

            LVQ8Store lvq_store = LVQ8Store::Load(*file_handler, buffer_size_);

            DumpStore(lvq_store, data.get());
        }
    }

    {
        std::string file_path = file_dir_ + "/lvq_store2.bin";
        LocalFileSystem fs;

        size_t idx = 0;
        {
            uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kWriteLock);

            LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
            auto ret = lvq_store.AddBatchVec(data.get(), vec_n_ / 2);
            EXPECT_NE(ret, LVQ8Store::err_idx());
            idx += vec_n_ / 2;

            for (size_t i = 0; i < buffer_size_ && idx < vec_n_; ++i) {
                ret = lvq_store.AddVec(data.get() + idx * dim_);
                EXPECT_NE(ret, LVQ8Store::err_idx());
                ++idx;
            }
            lvq_store.Save(*file_handler);
        }
        {
            uint8_t file_flags = FileFlags::READ_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kReadLock);

            LVQ8Store lvq_store = LVQ8Store::Load(*file_handler, buffer_size_);

            auto ret = lvq_store.AddBatchVec(data.get() + idx * dim_, vec_n_ - idx);
            EXPECT_NE(ret, LVQ8Store::err_idx());

            DumpStore(lvq_store, data.get());
        }
    }
}