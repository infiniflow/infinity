#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <random>

import dist_func;
import dist_func_l2_2;
import hnsw_alg;
import file_system;
import file_system_type;
import local_file_system;
import file_system_type;
import plain_store;
import lvq_store;

using namespace infinity;

int main() {
    using LabelT = uint64_t;
    using RetHeap = std::priority_queue<std::pair<float, LabelT>>;

    // using Hnsw = KnnHnsw<float, LabelT, PlainStore<float>>;
    using Hnsw = KnnHnsw<float, LabelT, LVQStore<float, uint8_t>>;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real;

    std::string save_dir = "/home/shenyushi/Documents/Code/infiniflow/infinity/tmp";

    int dim = 1;
    int element_size = 50;

    auto data = std::make_unique<float[]>(dim * element_size);
    for (int i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    LocalFileSystem fs;

    int M = 16;
    int ef_construction = 200;

    // FloatL2Space space(dim);
    FloatLVQ8L2Space space(dim);

    {
        auto hnsw_index = Hnsw::Make(element_size, dim, space, M, ef_construction, {});

        if (false) {
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                hnsw_index->Insert(query, unsigned(i));
                // hnsw_index->Dump(std::cout);
                // hnsw_index->Check();
            }
        } else {
            auto labels = std::make_unique<LabelT[]>(element_size);
            std::iota(labels.get(), labels.get() + element_size, 0);
            hnsw_index->Insert(data.get(), labels.get(), element_size);
            // hnsw_index->Dump(std::cout);
            // hnsw_index->Check();
        }

        hnsw_index->Dump(std::cout);
        hnsw_index->Check();

        hnsw_index->SetEf(ef_construction);
        int correct = 0;
        for (int i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            RetHeap result = hnsw_index->KnnSearch(query, 1);
            if (result.top().second == i) {
                ++correct;
            }
        }
        printf("correct rage: %f\n", float(correct) / element_size);

        uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kWriteLock);
        hnsw_index->SaveIndex(*file_handler);
        file_handler->Close();
    }

    {
        uint8_t file_flags = FileFlags::READ_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kReadLock);

        auto hnsw_index = Hnsw::LoadIndex(*file_handler, space, {});
        // hnsw_index->Dump(std::cout);
        hnsw_index->Check();
        int correct = 0;
        for (int i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            std::priority_queue<std::pair<float, LabelT>> result = hnsw_index->KnnSearch(query, 1);
            if (result.top().second == i) {
                ++correct;
            }
        }
        printf("correct rage: %f\n", float(correct) / element_size);
        file_handler->Close();
    }
}