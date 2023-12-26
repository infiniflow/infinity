# Benchmark

Infinity provides a Python script for benchmarking the SIFT1M and GIST1M datasets.

## Build and start Infinity

You have two options for building Infinity. Choose the option that best fits your needs:

- [Build Infinity using Docker](../README.md)
- [Build from source](./build_from_source.md)

## Download the Benchmark datasets

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

## Benchmark dependencies

```sh
cd python

pip install -r requirements.txt
python setup.py bdist_wheel
pip install dist/infinity_sdk-0.1.0.dev1-py3-none-any.whl
```

## Import the Benchmark datasets

```sh
cd benchmark

# options:
#   -h, --help            show this help message and exit
#   -d DATA_SET, --data DATA_SET

python remote_benchmark_import.py -d sift_1m
python remote_benchmark_import.py -d gist_1m
```

## Run Benchmark

```sh
# options:
#   -h, --help            show this help message and exit
#   -t THREADS, --threads THREADS
#   -r ROUNDS, --rounds ROUNDS
#   -d DATA_SET, --data DATA_SET

# ROUNDS indicates the number of times Python executes the benchmark, and the result represents the average duration for each run.

# Perform a benchmark on the SIFT1M dataset using a single thread, running it only once.
python remote_benchmark.py -t 1 -r 1 -d sift_1m
# Perform a benchmark on the GIST1M dataset using a single thread, running it only once.
python remote_benchmark.py -t 1 -r 1 -d gist_1m
```
## A SIFT1M Benchmark report

- **Hardware**: Intel i5-12500H, 16C, 16GB
- **Operating system**: Ubuntu 22.04
- **Dataset**: SIFT1M; **topk**: 100; **recall**: 97%+
- **QPS**: 10,305
- **P99 Latency**: < 0.4 ms