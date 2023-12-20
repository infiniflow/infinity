# Benchmark

**infinity** supply python script for sift1m and gist1m dataset benchmark.

## Get infinity binary file

```sh
git clone https://github.com/infiniflow/infinity.git
cd infinity
```

## download benchmark file

download via wget

```sh
#download sift benchmark
wget ftp://ftp.irisa.fr/local/texmex/corpus/sift.tar.gz
#download gist benchmark
wget ftp://ftp.irisa.fr/local/texmex/corpus/gist.tar.gz

```

or  
visit [http://corpus-texmex.irisa.fr/](http://corpus-texmex.irisa.fr/) to download manually.

```sh
#uncompress and move benchmark file
tar -zxvf sift.tar.gz
mv sift/sift_base.fvecs test/data/benchmark/sift_1m/sift_base.fvecs
mv sift/sift_query.fvecs test/data/benchmark/sift_1m/sift_query.fvecs
mv sift/sift_groundtruth.ivecs test/data/benchmark/sift_1m/sift_groundtruth.ivecs


tar -zxvf gist.tar.gz
mv gist/gist_base.fvecs test/data/benchmark/gist_1m/gist_base.fvecs
mv gist/gist_query.fvecs test/data/benchmark/gist_1m/gist_query.fvecs
mv gist/gist_groundtruth.ivecs test/data/benchmark/gist_1m/gist_groundtruth.ivecs

```

## Benchmark dependency

```sh
cd python

pip install -r requirements.txt
python setup.py bdist_wheel
pip install dist/infinity_sdk-0.1.0.dev1-py3-none-any.whl
```

## Start infinity

Read [README.md](https://github.com/infiniflow/infinity/blob/main/README.md) to start infinity.

## Import data

```sh
cd benchmark

options:
  -h, --help            show this help message and exit
  -d DATA_SET, --data DATA_SET

python remote_benchmark_import.py -d sift_1m
python remote_benchmark_import.py -d gist_1m
```

## run benchmark

```sh
options:
  -h, --help            show this help message and exit
  -t THREADS, --threads THREADS
  -r ROUNDS, --rounds ROUNDS
  -d DATA_SET, --data DATA_SET

# ROUNDS means how many times python runs the benchmark. The result is the average for each time.

# following command means run benchmark with 1 thread, for 1 time using sift dataset
python remote_benchmark.py -t 1 -r 1 -d sift_1m

python remote_benchmark.py -t 1 -r 1 -d gist_1m
```
