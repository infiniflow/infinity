//
// Created by jinhai on 23-5-1.
//

#include <thread>

#include "base_profiler.h"
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/IndexScalarQuantizer.h"
#include "faiss/index_io.h"
#include "helper.h"
#include "hnswlib/hnswlib.h"
#include "scheduler.h"

using namespace infinity;

enum class IndexType { kFlat, kIVFFlat, kIVFSQ8, kHnsw };

static const char *sift1m_base = "/home/jinhai/Documents/data/sift1M/sift_base.fvecs";
static const char *sift1m_train = "/home/jinhai/Documents/data/sift1M/sift_learn.fvecs";
static const char *sift1m_query = "/home/jinhai/Documents/data/sift1M/sift_query.fvecs";
static const char *sift1m_ground_truth = "/home/jinhai/Documents/data/sift1M/sift_groundtruth.ivecs";
static const char *ivf_index_name = "ivf_index.bin";
static const char *ivfsq8_index_name = "ivfsq8_index.bin";
static const char *flat_index_name = "flat_index.bin";
static const char *hnsw_index_l2_name = "hnsw_index_l2.bin";

void batch_run_on_cpu(faiss::Index *index,
                      IndexType index_type,
                      float *query_vectors,
                      size_t query_count,
                      size_t dimension,
                      size_t top_k,
                      const int *ground_truth) {
    faiss::idx_t *I = new faiss::idx_t[query_count * top_k];
    float *D = new float[query_count * top_k];

    switch (index_type) {
        case IndexType::kIVFFlat: {
            faiss::IndexIVFFlat *ivf_flat_index = (faiss::IndexIVFFlat *)index;
            ivf_flat_index->nprobe = 32; // Default value is 1;
            ivf_flat_index->search(query_count, query_vectors, top_k, D, I);
            break;
        }
        case IndexType::kHnsw: {
            assert(false || !"HNSW index doesn't support batch search");
            break;
        }
        case IndexType::kFlat: {
            index->search(query_count, query_vectors, top_k, D, I);
            break;
        }
        default:
            break;
    }

    // evaluate result by hand.
    int n_1 = 0, n_10 = 0, n_100 = 0;
    for (int i = 0; i < query_count; i++) {
        int gt_nn = ground_truth[i * top_k];
        for (int j = 0; j < top_k; j++) {
            if (I[i * top_k + j] == gt_nn) {
                if (j < 1)
                    n_1++;
                if (j < 10)
                    n_10++;
                if (j < 100)
                    n_100++;
            }
        }
    }
    printf("R@1 = %.4f\n", n_1 / float(query_count));
    printf("R@10 = %.4f\n", n_10 / float(query_count));
    printf("R@100 = %.4f\n", n_100 / float(query_count));

    delete[] I;
    delete[] D;
}

void single_run_on_cpu(void *index,
                       IndexType index_type,
                       float *query_vectors,
                       size_t query_count,
                       size_t dimension,
                       size_t top_k,
                       const int *ground_truth) {
    std::cout << "Begin to search " << query_count << " query, memory: " << get_current_rss() / 1000000 << " MB" << std::endl;

    infinity::BaseProfiler profiler;
    profiler.Begin();

    faiss::idx_t *I = new faiss::idx_t[top_k];
    float *D = new float[top_k];

    // evaluate result by hand.
    int n_1 = 0, n_10 = 0, n_100 = 0;
    for (size_t query_index = 0; query_index < query_count; ++query_index) {
        float *query_vector = &query_vectors[dimension * query_index];

        switch (index_type) {
            case IndexType::kIVFFlat: {
                faiss::IndexIVFFlat *ivf_flat_index = (faiss::IndexIVFFlat *)index;
                ivf_flat_index->nprobe = 32; // Default value is 1;
                ivf_flat_index->search(1, query_vector, top_k, D, I);
                break;
            }
            case IndexType::kHnsw: {
                size_t ef_construction = 200;
                hnswlib::HierarchicalNSW<float> *hnsw_index = (hnswlib::HierarchicalNSW<float> *)index;
                hnsw_index->setEf(ef_construction);
                std::priority_queue<std::pair<float, hnswlib::labeltype>> result = hnsw_index->searchKnn(query_vector, top_k);
                for (size_t idx = 0; idx < top_k; ++idx) {
                    I[idx] = result.top().second;
                    result.pop();
                }
                break;
            }
            case IndexType::kFlat: {
                faiss::IndexFlatL2 *flat_index = (faiss::IndexFlatL2 *)index;
                flat_index->search(1, query_vector, top_k, D, I);
                break;
            }
            default:
                break;
        }

        int gt_nn = ground_truth[query_index * top_k];
        for (int j = 0; j < top_k; j++) {
            if (I[j] == gt_nn) {
                if (j < 1)
                    n_1++;
                if (j < 10)
                    n_10++;
                if (j < 100)
                    n_100++;
            }
        }

        if (query_index % 1000 == 0) {
            std::cout << query_index << ", " << get_current_rss() / 1000000 << " MB, " << profiler.ElapsedToString() << std::endl;
            //            std::cout << n_1 << " " << n_10 << " " << n_100 << std::endl;
        }
    }

    profiler.End();
    std::cout << "Search: " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;

    printf("R@1 = %.4f\n", n_1 / float(query_count));
    printf("R@10 = %.4f\n", n_10 / float(query_count));
    printf("R@100 = %.4f\n", n_100 / float(query_count));

    delete[] I;
    delete[] D;
}

struct AnnFlatTask : public Task {
    inline explicit AnnFlatTask(faiss::Index *index,
                                size_t dimension,
                                size_t query_count,
                                float *query_vectors,
                                size_t top_k,
                                int64_t *&result_id,
                                float *&result_distance)
        : Task(TaskType::kAnnFlat), index_(index), dimension_(dimension), query_count_(query_count), query_vectors_(query_vectors), top_k_(top_k),
          result_id_(result_id), result_distance_(result_distance) {}

    void run(int64_t worker_id) override {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        //        for(size_t idx = 0; idx < query_count_; ++ idx) {
        //            float* one_query_ptr = &query_vectors_[idx * dimension_];
        //            int64_t* one_result_ids = &result_id_[idx * top_k_];
        //            float* one_result_distances = &result_distance_[idx * top_k_];
        //            index_->search(1, one_query_ptr, top_k_, one_result_distances, one_result_ids);
        ////            printf("worker id: %ld, query: %f, result: %ld\n", worker_id, *one_query_ptr, *one_result_ids);
        //        }

        index_->search(query_count_, query_vectors_, top_k_, result_distance_, result_id_);

        profiler.End();
        //        std::cout << "Search: " << profiler.ElapsedToString() << ", "
        //                  << get_current_rss() / 1000000 << " MB" << std::endl;
        printf("Run AnnFlat by worker: %ld, spend: %s\n", (long)worker_id, profiler.ElapsedToString().c_str());
        //        usleep(1000 * 1000);
    }

private:
    faiss::Index *index_{nullptr};
    size_t dimension_{0};
    size_t query_count_{0};
    float *query_vectors_{nullptr};
    size_t top_k_{0};
    int64_t *result_id_{nullptr};       // I
    float *result_distance_{nullptr}; // D
};

struct AnnIVFFlatTask : public Task {
    inline explicit AnnIVFFlatTask(faiss::Index *index,
                                   size_t dimension,
                                   size_t query_count,
                                   float *query_vectors,
                                   size_t top_k,
                                   int64_t *&result_id,
                                   float *&result_distance)
        : Task(TaskType::kAnnIVFFlat), index_(index), dimension_(dimension), query_count_(query_count), query_vectors_(query_vectors), top_k_(top_k),
          result_id_(result_id), result_distance_(result_distance) {}

    void run(int64_t worker_id) override {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        //        for(size_t idx = 0; idx < query_count_; ++ idx) {
        //            float* one_query_ptr = &query_vectors_[idx * dimension_];
        //            int64_t* one_result_ids = &result_id_[idx * top_k_];
        //            float* one_result_distances = &result_distance_[idx * top_k_];
        //            index_->search(1, one_query_ptr, top_k_, one_result_distances, one_result_ids);
        ////            printf("worker id: %ld, query: %f, result: %ld\n", worker_id, *one_query_ptr, *one_result_ids);
        //        }

        index_->search(query_count_, query_vectors_, top_k_, result_distance_, result_id_);

        profiler.End();
        //        std::cout << "Search: " << profiler.ElapsedToString() << ", "
        //                  << get_current_rss() / 1000000 << " MB" << std::endl;
        //        printf("Run AnnIVFFlat by worker: %ld, spend: %s\n", worker_id, profiler.ElapsedToString().c_str());
        //        usleep(1000 * 1000);
    }

private:
    faiss::Index *index_{nullptr};
    size_t dimension_{0};
    size_t query_count_{0};
    float *query_vectors_{nullptr};
    size_t top_k_{0};
    int64_t *result_id_{nullptr};       // I
    float *result_distance_{nullptr}; // D
};

struct AnnIVFSQ8Task : public Task {
    inline explicit AnnIVFSQ8Task(faiss::Index *index,
                                  size_t dimension,
                                  size_t query_count,
                                  float *query_vectors,
                                  size_t top_k,
                                  int64_t *&result_id,
                                  float *&result_distance)
        : Task(TaskType::kAnnIVFSQ8), index_(index), dimension_(dimension), query_count_(query_count), query_vectors_(query_vectors), top_k_(top_k),
          result_id_(result_id), result_distance_(result_distance) {}

    void run(int64_t worker_id) override {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        for (size_t idx = 0; idx < query_count_; ++idx) {
            float *one_query_ptr = &query_vectors_[idx * dimension_];
            int64_t *one_result_ids = &result_id_[idx * top_k_];
            float *one_result_distances = &result_distance_[idx * top_k_];
            index_->search(1, one_query_ptr, top_k_, one_result_distances, one_result_ids);
            //            printf("worker id: %ld, query: %f, result: %ld\n", worker_id, *one_query_ptr, *one_result_ids);
        }

        //        index_->search(query_count_, query_vectors_, top_k_, result_distance_, result_id_);

        profiler.End();
        //        std::cout << "Search: " << profiler.ElapsedToString() << ", "
        //                  << get_current_rss() / 1000000 << " MB" << std::endl;
        //        printf("Run AnnIVFFlat by worker: %ld, spend: %s\n", worker_id, profiler.ElapsedToString().c_str());
        //        usleep(1000 * 1000);
    }

private:
    faiss::Index *index_{nullptr};
    size_t dimension_{0};
    size_t query_count_{0};
    float *query_vectors_{nullptr};
    size_t top_k_{0};
    int64_t *result_id_{nullptr};       // I
    float *result_distance_{nullptr}; // D
};

struct AnnHNSWTask : public Task {
    inline explicit AnnHNSWTask(hnswlib::HierarchicalNSW<float> *index,
                                size_t dimension,
                                size_t query_count,
                                float *query_vectors,
                                size_t top_k,
                                int64_t *&result_id,
                                float *&result_distance)
        : Task(TaskType::kAnnHNSW), index_(index), dimension_(dimension), query_count_(query_count), query_vectors_(query_vectors), top_k_(top_k),
          result_id_(result_id), result_distance_(result_distance) {}

    void run(int64_t worker_id) override {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        for (size_t idx = 0; idx < query_count_; ++idx) {
            float *one_query_ptr = &query_vectors_[idx * dimension_];
            int64_t *one_result_ids = &result_id_[idx * top_k_];
            float *one_result_distances = &result_distance_[idx * top_k_];
            std::priority_queue<std::pair<float, hnswlib::labeltype>> result = index_->searchKnn(one_query_ptr, top_k_);
            for (size_t k = 0; k < top_k_; ++k) {
                one_result_ids[k] = result.top().second;
                result.pop();
            }
            //            printf("worker id: %ld, query: %f, result: %ld\n", worker_id, *one_query_ptr, *one_result_ids);
        }

        //        index_->search(query_count_, query_vectors_, top_k_, result_distance_, result_id_);

        profiler.End();
        //        std::cout << "Search: " << profiler.ElapsedToString() << ", "
        //                  << get_current_rss() / 1000000 << " MB" << std::endl;
        //        printf("Run AnnIVFFlat by worker: %ld, spend: %s\n", worker_id, profiler.ElapsedToString().c_str());
        //        usleep(1000 * 1000);
    }

private:
    hnswlib::HierarchicalNSW<float> *index_{nullptr};
    size_t dimension_{0};
    size_t query_count_{0};
    float *query_vectors_{nullptr};
    size_t top_k_{0};
    int64_t *result_id_{nullptr};       // I
    float *result_distance_{nullptr}; // D
};

void scheduler_run_on_cpu(void *index,
                          IndexType index_type,
                          float *total_query_vectors,
                          size_t total_query_count,
                          size_t dimension,
                          size_t top_k,
                          const int32_t *ground_truth) {
    //    const std::unordered_set<int64_t> cpu_mask{1, 3, 5, 7, 9, 11, 13, 15};
    //    const std::unordered_set<int64_t> cpu_mask{1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15};
    //    const std::unordered_set<int64_t> cpu_mask{1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15};
    //    const std::unordered_set<int64_t> cpu_mask{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    //    const std::unordered_set<int64_t> cpu_mask{1, 3, 5, 7};
    const std::unordered_set<int64_t> cpu_mask;
    //    total_query_count = 16;

    int64_t cpu_count = std::thread::hardware_concurrency();
    std::unordered_set<int64_t> cpu_set;
    for (int64_t idx = 0; idx < cpu_count; ++idx) {
        if (!cpu_mask.contains(idx)) {
            cpu_set.insert(idx);
        }
    }

    size_t task_count = cpu_set.size();
    if (task_count == 0) {
        assert(false || !"No cpu available");
    }
    size_t base_task_size = total_query_count / task_count;
    size_t reminder_size = total_query_count % task_count;
    size_t query_offset = 0;
    std::vector<std::unique_ptr<Task>> tasks;
    tasks.reserve(task_count);

    int64_t *total_result_ids = new faiss::idx_t[total_query_count * top_k];
    float *total_result_distances = new float[total_query_count * top_k];

    for (size_t idx = 0; idx < task_count; ++idx) {
        size_t query_count = base_task_size;
        if (reminder_size > 0) {
            ++query_count;
            --reminder_size;
        }
        float *query_vectors = &total_query_vectors[query_offset * dimension];

        int64_t *result_ids = &total_result_ids[query_offset * top_k];
        float *result_distances = &total_result_distances[query_offset * top_k];

        switch (index_type) {

            case IndexType::kIVFFlat: {
                faiss::IndexIVFFlat *ivf_flat_index = (faiss::IndexIVFFlat *)index;
                ivf_flat_index->nprobe = 32; // Default value is 1;
                tasks.emplace_back(
                    std::make_unique<AnnIVFFlatTask>(ivf_flat_index, dimension, query_count, query_vectors, top_k, result_ids, result_distances));
                break;
            }
            case IndexType::kIVFSQ8: {
                faiss::IndexIVFScalarQuantizer *ivf_sq8_index = (faiss::IndexIVFScalarQuantizer *)index;
                ivf_sq8_index->nprobe = 32; // Default value is 1;
                tasks.emplace_back(
                    std::make_unique<AnnIVFSQ8Task>(ivf_sq8_index, dimension, query_count, query_vectors, top_k, result_ids, result_distances));
                break;
            }
            case IndexType::kHnsw: {
                size_t ef_construction = 200;
                hnswlib::HierarchicalNSW<float> *hnsw_index = (hnswlib::HierarchicalNSW<float> *)index;
                hnsw_index->setEf(ef_construction);
                tasks.emplace_back(std::make_unique<AnnHNSWTask>(hnsw_index, dimension, query_count, query_vectors, top_k, result_ids, result_distances));
                break;
            }
            case IndexType::kFlat: {
                faiss::IndexFlatL2 *flat_index = (faiss::IndexFlatL2 *)index;
                tasks.emplace_back(std::make_unique<AnnFlatTask>(flat_index, dimension, query_count, query_vectors, top_k, result_ids, result_distances));
                break;
            }
            default:
                break;
        }

        query_offset += query_count;
    }

    Scheduler scheduler;
    scheduler.Init(cpu_set);
    int64_t task_id{0};
    for (int64_t cpu_id : cpu_set) {
        scheduler.ScheduleTask(cpu_id, tasks[task_id].get());
        ++task_id;
    }

    scheduler.Uninit();

    int n_1 = 0, n_10 = 0, n_100 = 0;
    std::unordered_set<int32_t> gt1, gt10, gt100;
    for (int i = 0; i < total_query_count; i++) {
        for (int j = 0; j < top_k; ++j) {
            int32_t gt_id = ground_truth[i * top_k + j];
            if (j < 1) {
                gt1.insert(gt_id);
            }
            if (j < 10) {
                gt10.insert(gt_id);
            }
            if (j < 100) {
                gt100.insert(gt_id);
            }
        }
        for (int j = 0; j < top_k; j++) {
            int32_t result_id = total_result_ids[i * top_k + j];
            if (j < 1 && gt1.contains(result_id)) {
                ++n_1;
            }
            if (j < 10 && gt10.contains(result_id)) {
                ++n_10;
            }
            if (j < 100 && gt100.contains(result_id)) {
                ++n_100;
            }
        }
    }
    printf("R@1 = %.4f\n", n_1 / float(total_query_count));
    printf("R@10 = %.4f\n", n_10 / float(total_query_count * 10));
    printf("R@100 = %.4f\n", n_100 / float(total_query_count * 100));

    delete[] total_result_ids;
    delete[] total_result_distances;
}

static float *float_vector_read(const char *fname, size_t *dimension, size_t *row_count, const char *comment) {
    infinity::BaseProfiler profiler;
    profiler.Begin();
    float *float_vec = fvecs_read(fname, dimension, row_count);
    profiler.End();
    if (comment != nullptr) {
        std::cout << "Load " << comment << ": " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;
    }
    return float_vec;
}

static int *integer_vector_read(const char *fname, size_t *dimension, size_t *row_count, const char *comment) {
    infinity::BaseProfiler profiler;
    profiler.Begin();
    int *float_vec = ivecs_read(fname, dimension, row_count);
    profiler.End();
    if (comment != nullptr) {
        std::cout << "Load " << comment << ": " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;
    }
    return float_vec;
}

void benchmark_flat() {
    std::cout << "Before query, current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;

    faiss::Index *index;
    int dimension = 128;
    std::ifstream f(flat_index_name);
    if (f.good()) {
        // Found index file
        std::cout << "Found flat index file ... " << std::endl;
        index = faiss::read_index(flat_index_name);
    } else {
        {
            // Construct index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index = new faiss::IndexFlatL2(dimension);
            profiler.End();
            std::cout << "Create Flat index: " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;
        }

        // Read base vector data
        size_t base_vector_row_count{0};
        size_t base_vector_dimension{0};
        float *base_vectors{nullptr};
        { base_vectors = float_vector_read(sift1m_base, &base_vector_dimension, &base_vector_row_count, "Base std::vector"); }

        {
            // Build index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index->add(base_vector_row_count, base_vectors);
            profiler.End();
            std::cout << "Insert Base std::vector into index: " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;
        }

        delete[] base_vectors;

        faiss::write_index(index, flat_index_name);
    }

    // Read query vector data
    size_t query_vector_row_count{0};
    size_t query_vector_dimension{0};
    float *query_vectors;
    {
        query_vectors = float_vector_read(sift1m_query, &query_vector_dimension, &query_vector_row_count, "Query std::vector");
        assert(query_vector_dimension == dimension || !"query does not have same dimension as base set");
    }

    // Read ground truth
    size_t top_k{0};             // number of results per query in the GT
    int32_t *ground_truth{nullptr}; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    size_t ground_truth_row_count{0};
    {
        // load ground-truth
        ground_truth = ivecs_read(sift1m_ground_truth, &top_k, &ground_truth_row_count);
        assert(query_vector_row_count == ground_truth_row_count || !"incorrect nb of ground truth entries");
    }

    infinity::BaseProfiler profiler;
    profiler.Begin();

    std::cout << "Start to query: " << query_vector_row_count << " vectors, Current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;
    //    single_run_on_cpu(index, IndexType::kFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    //    batch_run_on_cpu(index, IndexType::kFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    scheduler_run_on_cpu(index, IndexType::kFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    profiler.End();
    std::cout << "Spend total: " << profiler.ElapsedToString() << std::endl;

    {
        delete[] query_vectors;
        delete[] ground_truth;
        delete index;
    }
    std::cout << "Memory: " << get_current_rss() / 1000000 << " MB" << std::endl;
}

void benchmark_ivfflat() {
    std::cout << "Before query, current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;

    int dimension = 128;
    size_t centroids_count = 4096;
    faiss::IndexFlatL2 quantizer(dimension);
    faiss::Index *index = nullptr;

    std::ifstream f(ivf_index_name);
    if (f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        index = faiss::read_index(ivf_index_name);
    } else {
        {
            // Construct index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index = new faiss::IndexIVFFlat(&quantizer, dimension, centroids_count, faiss::MetricType::METRIC_L2);
            profiler.End();
            std::cout << "Create IVF Flat index: " << profiler.ElapsedToString() << std::endl;
        }

        // Read train vector data
        size_t train_vector_row_count{0};
        size_t train_vector_dimension{0};
        {
            infinity::BaseProfiler profiler;
            profiler.Begin();
            float *train_vectors = float_vector_read(sift1m_train, &train_vector_dimension, &train_vector_row_count, "Train std::vector");
            profiler.End();

            profiler.Begin();
            index->train(train_vector_row_count, train_vectors);
            profiler.End();
            std::cout << "Train sift1M learn data into quantizer: " << profiler.ElapsedToString() << std::endl;
            delete[] train_vectors;
        }

        // Read base vector data
        size_t base_vector_row_count{0};
        size_t base_vector_dimension{0};
        float *base_vectors{nullptr};
        { base_vectors = float_vector_read(sift1m_base, &base_vector_dimension, &base_vector_row_count, "Base std::vector"); }

        {
            // Build index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index->add(base_vector_row_count, base_vectors);
            profiler.End();
            std::cout << "Insert Base std::vector into index: " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;
        }

        delete[] base_vectors;

        faiss::write_index(index, ivf_index_name);
    }

    // Read query vector data
    size_t query_vector_row_count{0};
    size_t query_vector_dimension{0};
    float *query_vectors;
    {
        query_vectors = float_vector_read(sift1m_query, &query_vector_dimension, &query_vector_row_count, "Query std::vector");
        assert(query_vector_dimension == dimension || !"query does not have same dimension as base set");
    }

    // Read ground truth
    size_t top_k{0};             // number of results per query in the GT
    int32_t *ground_truth{nullptr}; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    size_t ground_truth_row_count{0};
    {
        // load ground-truth
        ground_truth = ivecs_read(sift1m_ground_truth, &top_k, &ground_truth_row_count);
        assert(query_vector_row_count == ground_truth_row_count || !"incorrect nb of ground truth entries");
    }

    infinity::BaseProfiler profiler;
    profiler.Begin();

    std::cout << "Start to query: " << query_vector_row_count << " vectors, Current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;
    //    batch_run_on_cpu(index, IndexType::kIVFFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    //    single_run_on_cpu(index, IndexType::kIVFFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    scheduler_run_on_cpu(index, IndexType::kIVFFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    profiler.End();
    std::cout << "Spend total: " << profiler.ElapsedToString() << std::endl;

    {
        delete[] query_vectors;
        delete[] ground_truth;
        delete index;
    }
    std::cout << "Memory: " << get_current_rss() / 1000000 << " MB" << std::endl;
}

void benchmark_ivfsq8() {
    std::cout << "Before query, current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;

    int dimension = 128;
    size_t centroids_count = 4096;
    faiss::IndexFlatL2 quantizer(dimension);
    faiss::Index *index = nullptr;

    std::ifstream f(ivfsq8_index_name);
    if (f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        index = faiss::read_index(ivfsq8_index_name);
    } else {
        {
            // Construct index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index = new faiss::IndexIVFScalarQuantizer(&quantizer,
                                                       dimension,
                                                       centroids_count,
                                                       faiss::ScalarQuantizer::QuantizerType::QT_8bit,
                                                       faiss::MetricType::METRIC_L2);
            profiler.End();
            std::cout << "Create IVF SQ8 index: " << profiler.ElapsedToString() << std::endl;
        }

        // Read train vector data
        size_t train_vector_row_count{0};
        size_t train_vector_dimension{0};
        {
            infinity::BaseProfiler profiler;
            profiler.Begin();
            float *train_vectors = float_vector_read(sift1m_train, &train_vector_dimension, &train_vector_row_count, "Train std::vector");
            profiler.End();

            profiler.Begin();
            index->train(train_vector_row_count, train_vectors);
            profiler.End();
            std::cout << "Train sift1M learn data into quantizer: " << profiler.ElapsedToString() << std::endl;
            delete[] train_vectors;
        }

        // Read base vector data
        size_t base_vector_row_count{0};
        size_t base_vector_dimension{0};
        float *base_vectors{nullptr};
        { base_vectors = float_vector_read(sift1m_base, &base_vector_dimension, &base_vector_row_count, "Base std::vector"); }

        {
            // Build index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index->add(base_vector_row_count, base_vectors);
            profiler.End();
            std::cout << "Insert Base std::vector into index: " << profiler.ElapsedToString() << ", " << get_current_rss() / 1000000 << " MB" << std::endl;
        }

        delete[] base_vectors;

        faiss::write_index(index, ivfsq8_index_name);
    }

    // Read query vector data
    size_t query_vector_row_count{0};
    size_t query_vector_dimension{0};
    float *query_vectors;
    {
        query_vectors = float_vector_read(sift1m_query, &query_vector_dimension, &query_vector_row_count, "Query std::vector");
        assert(query_vector_dimension == dimension || !"query does not have same dimension as base set");
    }

    // Read ground truth
    size_t top_k{0};             // number of results per query in the GT
    int32_t *ground_truth{nullptr}; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    size_t ground_truth_row_count{0};
    {
        // load ground-truth
        ground_truth = ivecs_read(sift1m_ground_truth, &top_k, &ground_truth_row_count);
        assert(query_vector_row_count == ground_truth_row_count || !"incorrect nb of ground truth entries");
    }

    infinity::BaseProfiler profiler;
    profiler.Begin();

    std::cout << "Start to query: " << query_vector_row_count << " vectors, Current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;
    //    batch_run_on_cpu(index, IndexType::kIVFFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    //    single_run_on_cpu(index, IndexType::kIVFFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    scheduler_run_on_cpu(index, IndexType::kIVFSQ8, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    profiler.End();
    std::cout << "Spend total: " << profiler.ElapsedToString() << std::endl;

    {
        delete[] query_vectors;
        delete[] ground_truth;
        delete index;
    }
    std::cout << "Memory: " << get_current_rss() / 1000000 << " MB" << std::endl;
}

void benchmark_hnsw() {
    std::cout << "Before query, current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;

    size_t dimension = 128;
    size_t M = 16;
    size_t ef_construction = 200;
    assert(dimension == 128 || !"embedding dimension isn't 128");
    hnswlib::L2Space l2space(dimension);
    hnswlib::HierarchicalNSW<float> *hnsw_index = nullptr;

    std::ifstream f(hnsw_index_l2_name);
    if (f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&l2space, hnsw_index_l2_name);
    } else {
        size_t max_elements = 10000000; // create index
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&l2space, max_elements, M, ef_construction);

        // Read base vector data
        size_t base_vector_row_count{0};
        size_t base_vector_dimension{0};
        float *base_vectors{nullptr};
        { base_vectors = float_vector_read(sift1m_base, &base_vector_dimension, &base_vector_row_count, "Base std::vector"); }

        {
            // Insert data into index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            // insert data into index
            for (size_t idx = 0; idx < base_vector_row_count; ++idx) {
                hnsw_index->addPoint(base_vectors + idx * dimension, idx);

                if (idx % 100000 == 0) {
                    std::cout << idx << ", " << get_current_rss() / 1000000 << " MB, " << profiler.ElapsedToString() << std::endl;
                }
            }
            profiler.End();
            std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
            hnsw_index->saveIndex(hnsw_index_l2_name);
        }
        delete[] base_vectors;
    }

    // Read query vector data
    size_t query_vector_row_count{0};
    size_t query_vector_dimension{0};
    float *query_vectors;
    {
        query_vectors = float_vector_read(sift1m_query, &query_vector_dimension, &query_vector_row_count, "Query std::vector");
        assert(query_vector_dimension == dimension || !"query does not have same dimension as base set");
    }

    // Read ground truth
    size_t top_k{0};             // number of results per query in the GT
    int32_t *ground_truth{nullptr}; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    size_t ground_truth_row_count{0};
    {
        // load ground-truth
        ground_truth = ivecs_read(sift1m_ground_truth, &top_k, &ground_truth_row_count);
        assert(query_vector_row_count == ground_truth_row_count || !"incorrect nb of ground truth entries");
    }

    infinity::BaseProfiler profiler;
    profiler.Begin();

    std::cout << "Start to query: " << query_vector_row_count << " vectors, Current memory: " << get_current_rss() / 1000000 << "MB" << std::endl;

    //    batch_run_on_cpu(index, IndexType::kIVFFlat, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    //    single_run_on_cpu(hnsw_index, IndexType::kHnsw, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    scheduler_run_on_cpu(hnsw_index, IndexType::kHnsw, query_vectors, query_vector_row_count, dimension, top_k, ground_truth);
    profiler.End();
    std::cout << "Spend total: " << profiler.ElapsedToString() << std::endl;

    {
        delete[] query_vectors;
        delete[] ground_truth;
        delete hnsw_index;
    }
    std::cout << "Memory: " << get_current_rss() / 1000000 << " MB" << std::endl;
}

void scheduler_test() {
    Scheduler scheduler;
    uint32_t thread_count = std::thread::hardware_concurrency();
    std::vector<std::unique_ptr<DummyTask>> tasks;
    tasks.reserve(thread_count);
    std::unordered_set<int64_t> cpu_set;
    for (int64_t idx = 0; idx < thread_count; ++idx) {
        cpu_set.insert(idx);
        tasks.emplace_back(std::move(std::make_unique<DummyTask>()));
    }

    scheduler.Init(cpu_set);
    for (int64_t idx = 0; idx < 5; ++idx) {
        for (int64_t cpu_id : cpu_set) {
            scheduler.ScheduleTask(cpu_id, tasks[cpu_id].get());
        }
    }

    scheduler.Uninit();
}

auto main() -> int {
    //    benchmark_flat();
    //    benchmark_flat();
    //    benchmark_ivfflat();
    benchmark_ivfsq8();
    //    benchmark_hnsw();
    //    scheduler_test();
    return 0;
}
