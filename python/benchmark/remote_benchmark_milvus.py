import argparse
import concurrent
import os
import struct
import time

from pymilvus import (
    connections,
    utility,
    FieldSchema, CollectionSchema, DataType,
    Collection,
)

from benchmark.remote_benchmark import calculate_recall_all, read_groundtruth


def fvecs_read_all(filename):
    vectors = []
    with open(filename, 'rb') as f:
        while True:
            try:
                dims = struct.unpack('i', f.read(4))[0]
                vec = struct.unpack('{}f'.format(dims), f.read(4 * dims))
                assert dims == len(vec)
                vectors.append(list(vec))
            except struct.error:
                break
    return vectors


def import_sift_1m_milvus(path):

    has = utility.has_collection("sift_benchmark")
    print(f"Does collection sift_benchmark exist in Milvus: {has}")

    if has is False:
        print("Start creating collection")

        num_entities, dim = 1000000, 128
        fields = [
            FieldSchema(name="pk", dtype=DataType.INT64, is_primary=True, auto_id=False),
            FieldSchema(name="col1", dtype=DataType.FLOAT_VECTOR, dim=dim)
        ]
        schema = CollectionSchema(fields, "sift_1m is a collection for benchmark")
        sift_collection = Collection("sift_benchmark", schema)

        # import data
        print("Start importing data")
        vectors = fvecs_read_all(path)
        print(f"Number of vectors entities: {len(vectors)}")
        pk_list = [i for i in range(num_entities)]
        print(f"Number of pk_list entities:{len(pk_list)}")

        total_entities = [
            pk_list,
            vectors
        ]

        start = time.time()

        for i in range(len(total_entities[0])):
            # insert one row
            entity = [
                [total_entities[0][i]],
                [total_entities[1][i]]
            ]

            sift_collection.insert(entity)

            if i % 1000 == 0:
                print(f"Number of entities in Milvus: {i}")

        sift_collection.flush()
        end = time.time()
        print(f"Time to insert {num_entities} entities: {end - start}")

        print("Start Creating index HNSW")
        index = {
            "index_type": "HNSW",
            "metric_type": "L2",
            "params": {"M": 16, "efConstruction": 200, "ef": 200},
        }

        print(f"Number of entities in Milvus: {sift_collection.num_entities}")

        start = time.time()

        sift_collection.create_index("col1", index)
        end = time.time()
        print(f"Time to create index: {end - start}")

        print("End Creating index HNSW")


def import_gist_1m_milvus(path):
    has = utility.has_collection("gist_benchmark")
    print(f"Does collection gist_benchmark exist in Milvus: {has}")
    if has is False:
        print("Start creating collection")

        num_entities, dim = 1000000, 960
        fields = [
            FieldSchema(name="pk", dtype=DataType.INT64, is_primary=True, auto_id=False),
            FieldSchema(name="col1", dtype=DataType.FLOAT_VECTOR, dim=dim)
        ]
        schema = CollectionSchema(fields, "gist_1m is a collection for benchmark")
        gist_collection = Collection("gist_benchmark", schema)

        # import data
        print("Start importing data")

        vectors = fvecs_read_all(path)
        print(f"Number of vectors entities: {len(vectors)}")
        pk_list = [i for i in range(num_entities)]
        print(f"Number of pk_list entities:{len(pk_list)}")

        total_entities = [
            pk_list,
            vectors
        ]

        start = time.time()

        for i in range(len(total_entities[0])):
            # insert one row
            entity = [
                [total_entities[0][i]],
                [total_entities[1][i]]
            ]

            gist_collection.insert(entity)

            if i % 1000 == 0:
                print(f"Number of entities in Milvus: {i}")

        gist_collection.flush()
        end = time.time()
        print(f"Time to insert {num_entities} entities: {end - start}")

        print("Start Creating index HNSW")
        index = {
            "index_type": "HNSW",
            "metric_type": "L2",
            "params": {"M": 16, "efConstruction": 200, "ef": 200},
        }

        print(f"Number of entities in Milvus: {gist_collection.num_entities}")

        start = time.time()

        gist_collection.create_index("col1", index)
        end = time.time()
        print(f"Time to create index: {end - start}")

        print("End Creating index HNSW")


def import_data(path):
    if os.path.exists(path + "/sift_base.fvecs"):
        import_sift_1m_milvus(path + "/sift_base.fvecs")
    elif os.path.exists(path + "/gist_base.fvecs"):
        import_gist_1m_milvus(path + "/gist_base.fvecs")
    else:
        raise Exception("Invalid data set")


def benchmark(threads, rounds, data_set, path):
    import_data(path)
    if not os.path.exists(path):
        print(f"Path: {path} doesn't exist")
        raise Exception(f"Path: {path} doesn't exist")
    if data_set == "sift_1m":
        query_path = path + "/sift_query.fvecs"
        ground_truth_path = path + "/sift_groundtruth.ivecs"
        if threads == 1:
            one_thread(rounds, query_path, ground_truth_path, "sift_benchmark")
        else:
            multithreading(query_path, ground_truth_path, rounds, threads, "sift_benchmark")

    elif data_set == "gist_1m":
        query_path = path + "/gist_query.fvecs"
        ground_truth_path = path + "/gist_groundtruth.ivecs"
        if threads == 1:
            one_thread(rounds, query_path, ground_truth_path, "gist_benchmark")
        else:
            multithreading(query_path, ground_truth_path, rounds, threads, "gist_benchmark")
    else:
        raise Exception("Invalid data set")


def search_query(query, table_name):
    connections.connect("default", host="192.168.200.151", port="19530")
    sift_collection = Collection(table_name)
    search_params = {
        "metric_type": "L2",
        "params": {"ef": 200},
    }
    return sift_collection.search([query], "col1", search_params, limit=100)


def multithreading(query_path, ground_truth_path, rounds, threads, table_name):
    # Before conducting a search or a query, you need to load the data in `hello_milvus` into memory.
    print(f"Start Querying")
    sift_collection = Collection(table_name)
    sift_collection.load()

    # search based on vector similarity

    queries = fvecs_read_all(query_path)
    print(f"Number of queries: {len(queries)}")

    for i in range(rounds):
        query_results = [[] for _ in range(len(queries))]
        start_time = time.time()
        with concurrent.futures.ThreadPoolExecutor(max_workers=threads) as executor:
            results = list(executor.map(search_query, queries, [table_name] * len(queries)))
            for idx, hits in enumerate(results):
                for hit in hits:
                    query_results[idx].extend(hit.ids)

            # assert len(results) == 10000
            if table_name == "sift_benchmark":
                assert len(results) == 10000
            elif table_name == "gist_benchmark":
                assert len(results) == 1000
            end_time = time.time()

            recall_1, recall_10, recall_100 = calculate_recall(ground_truth_path, query_results)
            print(f"Thread: {threads}")
            print(f"Round {i + 1}:")
            print(f"Time of search = {end_time - start_time}s")
            qps = len(queries) / (end_time - start_time)
            print(f"QPS = {qps}")
            print(f"Recall@1: {recall_1}")
            print(f"Recall@10: {recall_10}")
            print(f"Recall@100: {recall_100}")


def one_thread(rounds, query_path, ground_truth_path, table_name):
    if not os.path.exists(query_path):
        print(f"File: {query_path} doesn't exist")
        raise Exception(f"File: {query_path} doesn't exist")

    results = []
    queries = fvecs_read_all(query_path)

    for i in range(rounds):

        query_results = [[] for _ in range(len(queries))]

        dur = 0.0
        for idx, query_vec in enumerate(queries):
            start = time.time()

            sift_collection = Collection(table_name)
            search_params = {
                "metric_type": "L2",
                "params": {"ef": 200},
            }
            res = sift_collection.search([query_vec], "col1", search_params, limit=100)

            end = time.time()

            diff = end - start
            dur += diff

            res_list = res[0]

            query_results[idx].extend(res_list.ids)

        recall_1, recall_10, recall_100 = calculate_recall(ground_truth_path, query_results)

        results.append(f"Round {i + 1}:")
        results.append(f"Total Dur: {dur} s")
        results.append(f"Query Count: {len(queries)}")
        results.append(f"QPS: {len(queries) / dur}")
        results.append(f"Recall@1: {recall_1}")
        results.append(f"Recall@10: {recall_10}")
        results.append(f"Recall@100: {recall_100}")

    for result in results:
        print(result)


def calculate_recall(ground_truth_path, query_results):
    ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(ground_truth_path)
    recall_1, recall_10, recall_100 = calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10,
                                                           ground_truth_sets_100, query_results)
    return recall_1, recall_10, recall_100


if __name__ == '__main__':
    connections.connect("default", host="192.168.200.151", port="19530")
    current_path = os.getcwd()
    parent_path = os.path.dirname(current_path)
    parent_path = os.path.dirname(parent_path)

    print(f"Current Path: {current_path}")
    print(f"Parent Path: {parent_path}")

    parser = argparse.ArgumentParser(description="Benchmark Infinity")

    parser.add_argument(
        "-t",
        "--threads",
        type=int,
        default=32,
        dest="threads",
    )
    parser.add_argument(
        "-r",
        "--rounds",
        type=int,
        default=1,
        dest="rounds",
    )
    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default='sift_1m',  # gist_1m
        dest="data_set",
    )

    data_dir = parent_path + "/test/data/benchmark/" + parser.parse_args().data_set
    print(f"Data Dir: {data_dir}")

    args = parser.parse_args()


    benchmark(args.threads, args.rounds, args.data_set, path=data_dir)
