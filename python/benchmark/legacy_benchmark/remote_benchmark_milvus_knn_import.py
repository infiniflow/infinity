import argparse
import multiprocessing
import os
import struct
import time

from pymilvus import MilvusClient, DataType


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


def fvecs_read(filename):
    with open(filename, 'rb') as f:
        while True:
            try:
                dims = struct.unpack('i', f.read(4))[0]
                vec = struct.unpack('{}f'.format(dims), f.read(4 * dims))
                assert dims == len(vec)
                yield list(vec)
            except struct.error:
                break


def insert_sift_1m(path):
    client = MilvusClient(
        uri="http://localhost:19530",
        token="root:Milvus"
    )

    if client.has_collection(collection_name="sift_benchmark_collection"):
        client.drop_collection(collection_name="sift_benchmark_collection")

    schema = MilvusClient.create_schema(
        auto_id=False,
        enable_dynamic_field=True,
    )
    schema.add_field(field_name="id", datatype=DataType.INT64, is_primary=True)
    schema.add_field(field_name="vector", datatype=DataType.FLOAT_VECTOR, dim=128)

    index_params = client.prepare_index_params()
    index_params.add_index(
        field_name="vector",
        index_type="HNSW",
        metric_type="L2",
        params={"M": 16, "efConstruction": 200}
    )

    client.create_collection(
        collection_name="sift_benchmark_collection",
        schema=schema,
        index_params=index_params
    )

    assert os.path.exists(path)
    print("Start insert data")

    count = 0
    start = time.time()

    vectors = []
    loop_start = time.time()
    for idx, query_vec in enumerate(fvecs_read(path)):
        vectors.append({"id":idx, "vector":query_vec})
        if len(vectors) == 1000:
            client.insert(
                collection_name="sift_benchmark_collection",
                data=vectors,
            )
            vectors.clear()

        count += 1
        if count % 10000 == 0:
            loop_end = time.time()
            dur = loop_end - start
            # dur = loop_end - loop_start
            print(f"{count} rows inserted, cost time: {dur} s")
            # loop_start = time.time()

    end = time.time()
    dur = end - start
    print(f"Import sift_1m cost time: {dur} s, tps: {count * 1.0 / dur}")
    # start = time.time()
    # create_index("sift_benchmark")
    # end = time.time()
    # dur = end - start
    # print(f"Create index on sift_1m cost time: {dur} s")


def insert_gist_1m(path):
    client = MilvusClient(
        uri="http://localhost:19530",
        token="root:Milvus"
    )

    if client.has_collection(collection_name="sift_benchmark_collection"):
        client.drop_collection(collection_name="sift_benchmark_collection")

    schema = MilvusClient.create_schema(
        auto_id=False,
        enable_dynamic_field=True,
    )
    schema.add_field(field_name="id", datatype=DataType.INT64, is_primary=True)
    schema.add_field(field_name="vector", datatype=DataType.FLOAT_VECTOR, dim=960)

    index_params = client.prepare_index_params()
    index_params.add_index(
        field_name="vector",
        index_type="IVF_FLAT",
        metric_type="IP",
        #params={"nlist": 128}
    )

    client.create_collection(
        collection_name="sift_benchmark_collection",
        schema=schema,
        index_params=index_params
    )

    assert os.path.exists(path)

    print("Start insert data")

    count = 0
    start = time.time()

    vectors = []
    loop_start = time.time()
    for idx, query_vec in enumerate(fvecs_read(path)):
        vectors.append({"id":idx, "vector":query_vec})
        if len(vectors) == 1000:
            client.insert(
                collection_name="sift_benchmark_collection",
                data=vectors,
            )
            vectors.clear()

        count += 1
        if count % 10000 == 0:
            loop_end = time.time()
            dur = loop_end - start
            # dur = loop_end - loop_start
            print(f"{count} rows inserted, cost time: {dur} s")
            # loop_start = time.time()

    end = time.time()
    dur = end - start
    print(f"Import gist_1m cost time: {dur} s, tps: {count * 1.0 / dur}")


def insert_data(threads, data_set, path):
    if not os.path.exists(path + "/sift_base.fvecs") and not os.path.exists(path + "/gist_base.fvecs"):
        raise Exception("Invalid data set")
    if data_set == "sift_1m":
        data_path = path + "/sift_base.fvecs"
        if threads > 1:
            print(f"Multi-threads: {threads}")
            #process_pool(threads, data_path, "sift_benchmark")
        else:
            print(f"Single-thread")
            insert_sift_1m(data_path)
    elif data_set == "gist_1m":
        data_path = path + "/gist_base.fvecs"
        if threads > 1:
            print(f"Multi-threads: {threads}")
            #process_pool(threads, data_path, "gist_benchmark")
        else:
            print(f"Single-thread")
            insert_gist_1m(data_path)


if __name__ == '__main__':
    current_path = os.getcwd()
    parent_path = os.path.dirname(current_path)
    parent_path = os.path.dirname(parent_path)
    parent_path = os.path.dirname(parent_path)

    print(f"Current Path: {current_path}")
    print(f"Parent Path: {parent_path}")

    parser = argparse.ArgumentParser(description="Benchmark Infinity")

    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default='sift_1m',
        # default='gist_1m',
        dest="data_set",
    )

    parser.add_argument(
        "-t",
        "--thread",
        type=int,
        default='1',
        dest="threads",
    )

    data_dir = current_path + "/test/data/benchmark/" + parser.parse_args().data_set
    print(f"Data Dir: {data_dir}")

    args = parser.parse_args()

    insert_data(args.threads, args.data_set, path=data_dir)