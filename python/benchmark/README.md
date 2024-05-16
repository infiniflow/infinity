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
- Full-text Search
  - ES benchmark(rally-tracks)
    - Wikipedia(~21GB)
    - PMC
- Hybrid Search(Full-text Search + Vector Search)
  - dbpedia(1M, 1536 dimensions)

## How to run the benchmark?

```
Usage: run_benchmark.py [OPTIONS]

Options:
    --engines
        all(default), qdrant, elasticSearch, infinity
    --hardware
        8c_16g(default)
        4c_32g
    --mode
        test mode:  all(default)
        gist,
        sift,
        cohere_1M,
        cohere_10M,
        geonames,
        pmc
    --limit-ram
        25Gb(default)
    --limit-cpu
        8(default), 1~your cpu core
    --import
        perform import data operation
    --generate
        whether to generate a query
    --generate-query-num
        1(default)
    --generate-term-num
        4(default)
    --query
        perform query operation
    --help
        show this message.
```

The hardware and mode configurations are saved as JSON files in the "configs" folder. The hardware configuration is for setting up the database server's hardware parameters, while the mode configuration is for setting up the benchmark being used, which includes the dataset, index parameters, queries, and query parameters. Users can customize these configurations, saving them as JSON files, for example, "elasticsearch_sift.json". To run a test, simply use `--engines elasticsesarch --mode sift`.
