---
sidebar_position: 1
slug: /benchmark
---
# Benchmark
This document compares the following key specifications of Elasticsearch, Qdrant, and Infinity:

- QPS
- Recall
- Time to insert & build index
- Time to import & build index
- Disk usage
- Peak memory usage

## Versions
|                   | Version |
| ----------------- |---------|
| **Elasticsearch** | v8.13.0 |
| **Qdrant**        | v1.8.2  |
| **Infinity**      | v0.1.0  |

## Run Benchmark

1. Install necessary dependencies. 

```python
pip install -r requirements.txt
```

2. Download the required Benchmark datasets to your **/datasets** folder：
   - [SIFT1M](http://ann-benchmarks.com/sift-128-euclidean.hdf5)
   - [GIST1M](http://ann-benchmarks.com/gist-960-euclidean.hdf5)
   - [Dbpedia](https://public.ukp.informatik.tu-darmstadt.de/thakur/BEIR/datasets/dbpedia-entity.zip)
   - [Enwiki](https://home.apache.org/~mikemccand/enwiki-20120502-lines-1k.txt.lzma)
3. Start up the databases to compare:
```bash
docker compose up -d
```
4. Run Benchmark: 

   > Tasks of this Python script include:
   >
   > - Delete the original data.
   > - Re-insert the data
   > - Calculate the time to insert data and build index
   > - Calculate QPS.
   > - Calculate query latencies. 
```bash
python run.py
```
5. Navigate to the **results** folder to view the results and latency of each query. 

## Benchmark Results
### SIFT1M

> - Metric: L2
> - 10000 queries

|                   | QPS   | Recall         | Time to insert & build index | Time to import & build index | Disk   | Peak memory |
| ----------------- | ----- | -------------- | ---------------------------- | ---------------------------- | ------ | ----------- |
| **Elasticsearch** | 934   | 0.992          | 131 s                        | N/A                          | 874 MB | 1.463 GB    |
| **Qdrant**        | 1303  | 0.979          | 46 s                         | N/A                          | 418 MB | 1.6 GB      |
| **Infinity**      | 16320 | 0.973          | 74 s                         | 28 s                         | 792 MB | 0.95 GB     |



### GIST1M

> - Metric: L2
> - 1000 queries

|                   | QPS  |  Recall        | Time to insert & build index | Time to import & build index | Disk   | Peak memory |
| ----------------- | ---- | -------------- | ---------------------------- | ---------------------------- | ------ | ----------- |
| **Elasticsearch** | 305  | 0.885          | 872 s                        | N/A                          | 13 GB  | 6.9 GB      |
| **Qdrant**        | 339  | 0.947          | 366 s                        | N/A                          | 4.4 GB | 7.3 GB      |
| **Infinity**      | 2200 | 0.946          | 463 s                        | 112 s                        | 4.7 GB | 6.0 GB      |



### Dbpedia

> -  4160000 documents
> - 467 queries

|                   | QPS  | Time to insert & build index | Time to import & build index | Disk   | Peak memory |
| ----------------- | ----------- | ---------------------------- | ---------------------------- | ---- | ------ |
| **Elasticsearch** | 777  | 291 s                        | N/A                          | 2 GB   | 1.7 GB      |
| **Infinity**      | 817  | 237 s                        | 123 s                        | 3.4 GB | 0.49 GB     |

### Enwiki

> - 33000000 documents
> - 100 queries

|                   | QPS  | Time to insert & build index | Time to import & build index | Disk  | Peak memory |
| ----------------- | ----------- | ---------------------------- | ---------------------------- | ---- | ----- |
| **Elasticsearch** | 484      | 2289 s                       | N/A                          | 28 GB  | 5.3 GB |
| **Infinity**      | 484      | 2321 s                       | 944 s                        | 54 GB  | 5.1 GB |


---

## Deprecated Benchmark

Infinity provides a Python script for benchmarking the SIFT1M and GIST1M datasets.

### Build and start Infinity

You have two options for building Infinity. Choose the option that best fits your needs:

- [Build Infinity using Docker](https://github.com/infiniflow/infinity/blob/main/README.md)
- [Build from source](../getstarted/build_from_source.md)

### Download the Benchmark datasets

To obtain the benchmark datasets, you have the option to download them using the wget command.

```sh
#download sift benchmark
wget ftp://ftp.irisa.fr/local/texmex/corpus/sift.tar.gz
#download gist benchmark
wget ftp://ftp.irisa.fr/local/texmex/corpus/gist.tar.gz

```

Alternatively, you can manually download the benchmark datasets by visiting [http://corpus-texmex.irisa.fr/](http://corpus-texmex.irisa.fr/).

```sh
# Unzip and move the SIFT1M benchmark file.
tar -zxvf sift.tar.gz
mv sift/sift_base.fvecs test/data/benchmark/sift_1m/sift_base.fvecs
mv sift/sift_query.fvecs test/data/benchmark/sift_1m/sift_query.fvecs
mv sift/sift_groundtruth.ivecs test/data/benchmark/sift_1m/sift_groundtruth.ivecs

# Unzip and move the GIST1M benchmark file.
tar -zxvf gist.tar.gz
mv gist/gist_base.fvecs test/data/benchmark/gist_1m/gist_base.fvecs
mv gist/gist_query.fvecs test/data/benchmark/gist_1m/gist_query.fvecs
mv gist/gist_groundtruth.ivecs test/data/benchmark/gist_1m/gist_groundtruth.ivecs

```

### Benchmark dependencies

```sh
cd python

pip install -r requirements.txt
pip install .
```

### Import the Benchmark datasets

```sh
cd benchmark

# options:
#   -h, --help            show this help message and exit
#   -d DATA_SET, --data DATA_SET

python remote_benchmark_knn_import.py -d sift_1m
python remote_benchmark_knn_import.py -d gist_1m
```

### Run Benchmark

```sh
# options:
#   -h, --help            show this help message and exit
#   -t THREADS, --threads THREADS
#   -r ROUNDS, --rounds ROUNDS
#   -d DATA_SET, --data DATA_SET

# ROUNDS indicates the number of times Python executes the benchmark, and the result represents the average duration for each run.

# Perform a latency benchmark on the SIFT1M dataset using a single thread, running it only once.
python remote_benchmark_knn.py -t 1 -r 1 -d sift_1m
# Perform a latency benchmark on the GIST1M dataset using a single thread, running it only once.
python remote_benchmark_knn.py -t 1 -r 1 -d gist_1m

# Perform a QPS benchmark on the SIFT1M dataset using a single thread, running it only once.
python remote_benchmark_knn.py -t 16 -r 1 -d sift_1m
# Perform a latency benchmark on the GIST1M dataset using a single thread, running it only once.
python remote_benchmark_knn.py -t 16 -r 1 -d gist_1m
```
### A SIFT1M Benchmark report

- **Hardware**: Intel i5-12500H, 16C, 16GB
- **Operating system**: Ubuntu 22.04
- **Dataset**: SIFT1M; **topk**: 100; **recall**: 97%+
- **P99 QPS**: 15,688 (16 clients)
- **P99 Latency**: 0.36 ms
- **Memory usage**: 408 MB
