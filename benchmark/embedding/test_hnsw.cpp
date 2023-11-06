import stl;
import std;
import local_file_system;
import knn_knsw;
import dist_func;

using namespace infinity;

int main() {

    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real;

    int dim = 16;
    int element_size = 300;

    auto data = MakeUnique<float[]>(dim * element_size);
    for (int i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    int M = 16;
    int ef_construction = 200;
    DistFuncL2<float> space(dim);
    {
        auto hnsw_index = MakeUnique<KnnHnsw<float>>(element_size, dim, space, M, ef_construction);

        for (int i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            hnsw_index->Insert(query, unsigned(i));
            hnsw_index->CheckGraph();
        }

        hnsw_index->SaveIndex("./tmp/test.index", MakeUnique<LocalFileSystem>());
    }

    {
        auto hnsw_index = KnnHnsw<float>::LoadIndex("./tmp/test.index", MakeUnique<LocalFileSystem>(), space);
        hnsw_index->CheckGraph();
        hnsw_index->SetEf(ef_construction);
        int correct = 0;
        for (int i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            MaxHeap<Pair<float, u32>> result = hnsw_index->KnnSearch(query, 1);
            if (result.top().second == i) {
                ++correct;
            }
        }
        std::cout << "correct: " << float(correct) / element_size << std::endl;
    }
}