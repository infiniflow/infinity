# vector-benchmark

Today there are many open-source vector databases and search engines, each offering unique functionalities and demonstrates varied performance metrics. Notably, with the increasing integration of full-text and vector searches, objectively and fairly evaluating their performance becomes crucial. To fill the gap in benchmarking the performance of hybrid queries that combine both search types, we have developed a framework. This framework is designed to run various vector databases/search engines on the same hardware configuration to compare their performance differences in handling mixed queries. For greater extensibility, we also provide evaluation capabilities for standalone full-text and vector searches, enabling users to specify and run according to their requirements.

## Goals:

1.  A user-friendly front-end that facilitates users in configuring settings.
2.  An extensible testing environment that handle different scenarios, such as full-text search and vector search.
3.  Generate interactive test reports for comparison by the users.

## Principles:

1. We test different databases on the same hardware.
2. We purge OS cache before each test.
3. We disable the internal caches of each database. 
4. We restart the database before query.
5. The usage of CPU and memory could be limited.
6. The results must be reproducible.
7. All the benchmarks and vector databases(search engines) are open-sourced and representative.

## Engines:

- Qdrant
- ElasticSearch
- Infinity

## Datasets:

- Vector Search
  - GIST(1M, 960 dimensions)
  - SIFT(1M, 128 dimensions)
  - Cohere(1M/10M, 768 dimensions)
- Sparse Vector Search
  - SPLADE(1M)
- Full-text Search
  - ES benchmark(rally-tracks)
    - Wikipedia(~21GB)
    - PMC
- Hybrid Search(Full-text Search + Vector Search)
  - dbpedia(1M, 1536 dimensions)

## How to run the benchmark?
### Options

```
Usage: run.py [OPTIONS]

Options:
    --generate
        default=False
        whether generate fulltext query set based on the dataset
    --import
        default=False
        whether import dataset into database engine
    --query
        default=0
        Run the query set only once using given number of clients with recording the result and latency. This is for result validation and latency analysis.
    --query-express    
        default=0
        Run the query set randomly using given number of clients without recording the result and latency. This is for QPS measurement.
    --concurrency
       default="mp"
       Choose concurrency mechanism, one of: mp - multiprocessing(recommended), mt - multithreading.
    --engine
       default="infinity" 
       Choose database engine to benchmark:
          infinity,
          qdrant,
          elasticsearch,
          quickwit
    --dataset
      default="enwiki"  
      Choose dataset to benchmark:
          gist,
          sift,
          geonames,
          enwiki,
          tantivy,
          splade
    --help
        show this message.
```
*Notice that the arguments set for options [--query][--query-express] are not the number of rounds for query set or the number of query. Instead, they indicate the number of client/process/thread.*

### Configurations
The mode configurations are saved as JSON files in the "configs" folder. The mode configuration is for setting up the benchmark being used, which includes the dataset, index parameters, queries, query parameters and etc. Users can customize these configurations, saving them as JSON files, for example, "elasticsearch_sift.json".  
For example, if you want to run a qdrant benchmark on sift data set, with:  
- Euclidean distance to measure similarities among vectors,
- HNSW index type with M = 16, ef_construct = 200,
- top 100 most similar results,
- precision of the results
```
qdrant_sift.json
{
    "name": "qdrant_sift",
    "app": "qdrant",
    "app_path": "servers/qdrant/",
    "connection_url": "http://localhost:6333",
    "vector_size": 128,
    "distance": "L2",
    "mode": "vector", 
    "data_path": "datasets/sift/sift-128-euclidean.hdf5",
    "data_link": "http://ann-benchmarks.com/sift-128-euclidean.hdf5",
    "vector_name": "embeddings",
    "topK": 100,
    "vector_index": {
       "type": "HNSW",
       "index_params":{
           "M": 16,
           "ef_construct": 200
       }
    },
    "query_type": "json",
    "query_path": "datasets/sift/sift-128-euclidean.hdf5",
    "query_link": "http://ann-benchmarks.com/sift-128-euclidean.hdf5",
    "insert_batch_size": 8192,
    "ground_truth_path": "datasets/sift/sift-128-euclidean.hdf5"
}
```

Or, if you want to run an infinity benchmark on splade data set, with:  
- IP to measure similarities among vectors,
- BMP index type with block_size = 16, compress_type = compress,
- alpha = 0.92, beta = 0.8,
- top 10 most similar results,
- precision of the results
```
infinity_spade.json
{
    "name": "infinity_splade",
    "app": "infinity",
    "host": "127.0.0.1:23817",
    "data_path": "datasets/SPLADE/base_1M.csr",
    "use_import": "sparse_vector_csr",
    "topK": 10,
    "mode": "sparse_vector",
    "schema": {
        "col1": {"type": "sparse,30109,float,int16"}
    },
    "vector_name": "embeddings",
    "metric_type": "ip",
    "index": {
        "col1": {
            "type": "BMP",
            "params": {
                "block_size":8,
                "compress_type":"compress"
            }
        }
    },
    "alpha": 0.92,
    "beta": 0.8,
    "query_path": "datasets/SPLADE/queries.dev.csr",
    "batch_size": 8192,
    "ground_truth_path": "datasets/SPLADE/base_1M.dev.gt"
}
```

### Steps to run a benchmark

#### Step 1: Prepare the data set
Download the data set for import, query set for query and the ground truth set if you want to validate the results and calculate precision and save them in the "datasets" folder. The benchmark framework also support automatically downloading data set if you give the url link in "data_link" field in your configuration files.
#### Step 2: Customize your configuration files
Customize your configuration files for the specific benchmark you want to run, just like the previous configuration section did. Your configuration should also include the correct data path where you saved data set and be named correctly in pattern "\<engine_name\>_\<dataset_name\>.json".  
For example, if you want to run an infinity benchmark on splade data set, then the configuration file name should be "infinity_splade.json".
#### Step 3: Run the database engine
Download and run the database engine, make sure it is accessible when you run the benchmark.
#### Step 4: Import the data set
Import the data set into database engine using the option [--import].  
For example, if you want to import splade data set into qdrant, then you shall use:
```commandline
python3 run.py --engine qdrant --dataset splade --import
```
#### Step 5: Run the query set
Finally, you could run the query set using the option [--query] or [--query-express].
For example, if you want to run a qdrant benchmark on splade data set with single process for result validation and latency analysis, then you shall use:
```commandline
python3 run.py --engine qdrant --dataset splade --query 1 
```
Or, if you want to run an infinity benchmark on sift data set with 16 processes for QPS measurement, then you shall use:
```commandline
python3 run.py --engine  infinity --dataset sift --query-express 16
```