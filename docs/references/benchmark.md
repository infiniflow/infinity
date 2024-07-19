---
sidebar_position: 1
slug: /benchmark
---
# Benchmark
This document compares the following key specifications of Elasticsearch, Qdrant, Quickwit and Infinity:

- Time to insert & build index
- Time to import & build index
- query latency
- QPS

You need to watch resource (persisted index size, peak memory, peak cpu, system load etc.) manually.

Keep the environment clean to ensure that the database under test is able to use up all resources of the system.

Avoid running multiple databases at the same time, as each one is a significant resource consumer.

Test environment:

- OS: OpenSUSE Tumbleweed x86_64
- CPU: Intel CORE i5-13500H 16vCPU
- RAM: 32GB
- Disk: 1TB

## Versions
|                   | Version |
| ----------------- |---------|
| **Elasticsearch** | v8.13.4 |
| **Qdrant**        | v1.9.2  |
| **Quickwit**      | v0.8.1  |
| **Infinity**      | v0.2.0  |

## Run Benchmark

1. Install necessary dependencies.

```bash
cd python/benchmark
pip install -r requirements.txt
```

2. Download the required Benchmark datasets to your **/datasets** folder：
   - [SIFT1M](http://ann-benchmarks.com/sift-128-euclidean.hdf5)
   - [GIST1M](http://ann-benchmarks.com/gist-960-euclidean.hdf5)
   - [Enwiki](https://home.apache.org/~mikemccand/enwiki-20120502-lines-1k.txt.lzma)

Preprocess dataset:

```bash
sed '1d' datasets/enwiki/enwiki-20120502-lines-1k.txt > datasets/enwiki/enwiki.csv
```

3. Start up the databases to compare:

```bash
mkdir -p $HOME/elasticsearch/data
docker run -d --name elasticsearch --network host -e "discovery.type=single-node" -e "ES_JAVA_OPTS=-Xms16384m -Xmx32000m" -e "xpack.security.enabled=false" -v $HOME/elasticsearch/data:/usr/share/elasticsearch/data elasticsearch:8.13.4

mkdir -p $HOME/qdrant/storage
docker run -d --name qdrant --network host -v $HOME/qdrant/storage:/qdrant/storage qdrant/qdrant:v1.9.2

mkdir -p $HOME/quickwit
docker run -d --name quickwit --network=host -v $HOME/quickwit/qwdata:/quickwit/qwdata quickwit/quickwit:0.8.1 run

mkdir -p $HOME/infinity
docker run -d --name infinity --network=host -v $HOME/infinity:/var/infinity --ulimit nofile=500000:500000 infiniflow/infinity:nightly
```

4. Run Benchmark:

Drop file cache before benchmark.

```bash
echo 3 | sudo tee /proc/sys/vm/drop_caches
```

Tasks of the Python script `run.py` include:
 - Generate fulltext query set.
 - Measure the time to import data and build index.
 - Measure the query latency.
 - Measure the QPS.

```bash
$ python run.py -h
usage: run.py [-h] [--generate] [--import] [--query QUERY] [--query-express QUERY_EXPRESS] [--concurrency CONCURRENCY] [--engine ENGINE] [--dataset DATASET]

RAG Database Benchmark

options:
  -h, --help            show this help message and exit
  --generate            Generate fulltext query set based on the dataset (default: False)
  --import              Import dataset into database engine (default: False)
  --query QUERY         Run the query set only once using given number of clients with recording the result and latency. This is for result validation and latency analysis (default: 0)
  --query-express QUERY_EXPRESS
                        Run the query set randomly using given number of clients without recording the result and latency. This is for QPS measurement. (default: 0)
  --concurrency CONCURRENCY
                        Choose concurrency mechanism, one of: mp - multiprocessing(recommended), mt - multithreading. (default: mp)
  --engine ENGINE       Choose database engine to benchmark, one of: infinity, qdrant, elasticsearch, quickwit (default: infinity)
  --dataset DATASET     Choose dataset to benchmark, one of: gist, sift, geonames, enwiki, tantivy (default: enwiki)
```

Following are commands for engine `infinity` and dataset `enwiki`:

```bash
python run.py --generate --engine infinity --dataset enwiki
python run.py --import --engine infinity --dataset enwiki
python run.py --query=16 --engine infinity --dataset enwiki
python run.py --query-express=16 --engine infinity --dataset enwiki
```

Following are commands to issue a single query so that you can compare results among several engines.

```base
curl -X GET "http://localhost:9200/elasticsearch_enwiki/_search" -H 'Content-Type: application/json' -d'{"size":10,"_source":"doctitle","query": {"match": { "body": "wraysbury istorijos" }}}'

curl -X GET "http://localhost:7280/api/v1/_elastic/qucikwit_enwiki/_search" -H 'Content-Type: application/json' -d'{"query": {"query_string": {"query": "wraysbury istorijos", "fields": [ "body" ] } },"sort": ["_score"],"size":10}'

psql -h 0.0.0.0 -p 5432 -c "SELECT doctitle, ROW_ID(), SCORE() FROM infinity_enwiki SEARCH MATCH TEXT ('body', 'wraysbury istorijos', 'topn=10');"
```

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



### Enwiki

> - 33000000 documents
> - 100000 `OR` queries generated based on the dataset. All terms are extracted from the dataset and very rare(occurrence < 100) terms are excluded. The number of terms of each query match the weight `[0.03, 0.15, 0.25, 0.25, 0.15, 0.08, 0.04, 0.03, 0.02]`.

|                   | Time to insert & build index | Time to import & build index | P95 Latency(ms)| QPS (16 python clients) |  Memory | vCPU  |
| ----------------- | ---------------------------- | ---------------------------- | ---------------| ------------------------| --------| ----- |
| **Elasticsearch** | 2289 s                       | N/A                          | 14.75          | 1340                    | 21.0GB  | 10.6  |
| **Quickwit**      | 3962 s                       | N/A                          | 65.55          | 179                     | 1.2GB   | 11.3  |
| **Infinity**      | 1562 s                       | 2244 s                       | 1.37           | 13731                   | 10.0GB  | 11.0  |

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
