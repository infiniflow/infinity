# Infinity Benchmark scripts

## Benchmark on vector search

1. Clone infinity source code in _**infinity_source_path**_.
2. Copy or create symbolic link of sift 1m(gist 1m) dataset into **_infinity_source_path_**/test/data/benchmark/sift_1m(or gist_1m)
3. Start infinity server and run **remote_benchmark_knn_import.py** to import the sift or gist dataset into infinity and create index on the data.
4. Run **remote_benchmark_knn.py** to run the searching benchmark.
