import argparse
import functools
import multiprocessing
import os
import struct
import time
import traceback

import infinity
from infinity import index
from infinity.common import LOCAL_HOST
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.table import RemoteTable


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
    infinity_obj = infinity.connect(LOCAL_HOST)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table("sift_benchmark")
    db_obj.create_table("sift_benchmark", {"col1": {"type": "vector,128,float"}})
    table_obj = db_obj.get_table("sift_benchmark")
    assert table_obj

    assert os.path.exists(path)

    print("Start insert data")

    count = 0
    start = time.time()

    loop_start = time.time()
    for idx, query_vec in enumerate(fvecs_read(path)):
        table_obj.insert([{"col1": query_vec}])
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
    infinity_obj = infinity.connect(LOCAL_HOST)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table("gist_benchmark")
    db_obj.create_table("gist_benchmark", {"col1": {"type": "vector,960,float"}})
    table_obj = db_obj.get_table("gist_benchmark")
    assert table_obj

    assert os.path.exists(path)

    print("Start insert data")

    count = 0
    start = time.time()

    loop_start = time.time()
    for idx, query_vec in enumerate(fvecs_read(path)):
        table_obj.insert([{"col1": query_vec}])
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

    # start = time.time()
    # create_index("gist_benchmark")
    # end = time.time()
    # dur = end - start
    # print(f"Create index on gist_1m cost time: {dur} s")


def create_index(table_name):
    conn = ThriftInfinityClient(LOCAL_HOST)
    table = RemoteTable(conn, "default_db", table_name)
    res = table.create_index("hnsw_index",
                             index.IndexInfo("col1",
                                             index.IndexType.Hnsw,
                                             {
                                                 "m": "16",
                                                 "ef_construction": "200",
                                                 "ef": "200",
                                                 "metric": "l2",
                                                 "encode": "lvq"
                                             }))

    assert res.error_code == ErrorCode.OK


def work(vectors, table_name, column_name):
    infinity_obj = infinity.connect(LOCAL_HOST)
    assert infinity_obj
    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    table_obj = db_obj.get_table(table_name)
    assert table_obj

    for vec in vectors:
        table_obj.insert([{column_name: vec}])


def process_pool(threads, path, table_name):
    if not os.path.exists(path):
        print(f"File: {path} doesn't exist")
        raise Exception(f"File: {path} doesn't exist")

    infinity_obj = infinity.connect(LOCAL_HOST)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table(table_name)
    db_obj.create_table(table_name, {"col1": {"type": "vector,128,float"}})
    infinity_obj.disconnect()

    results = []
    total_vectors = fvecs_read_all(path)
    total_count = len(total_vectors)
    print(f"Total Data Count: {total_count}")

    total_time = 0
    for i in range(10):
        batch = total_count / 10
        left = int(batch * i)
        right = int(batch * (i + 1))
        vectors = [[] for i in range(threads)]
        k = 0
        for j in range(left, right):
            vectors[k % threads].append(total_vectors[j])
            k += 1

        p = multiprocessing.Pool(threads)
        print("Start insert data.")

        start = time.time()
        for idx in range(threads):
            p.apply_async(work, args=(vectors[idx], table_name, "col1"))
        p.close()
        p.join()
        end = time.time()
        dur = end - start
        total_time += dur

    print(f"Import sift_1m cost time: {total_time} s, tps: {total_count * 1.0 / total_time}")

    start = time.time()
    create_index(table_name)
    end = time.time()
    dur = end - start
    print(f"Create index on sift_1m cost time: {dur} s")


def insert_data(threads, data_set, path):
    if not os.path.exists(path + "/sift_base.fvecs") and not os.path.exists(path + "/gist_base.fvecs"):
        raise Exception("Invalid data set")
    if data_set == "sift_1m":
        data_path = path + "/sift_base.fvecs"
        if threads > 1:
            print(f"Multi-threads: {threads}")
            process_pool(threads, data_path, "sift_benchmark")
        else:
            print(f"Single-thread")
            insert_sift_1m(data_path)
    elif data_set == "gist_1m":
        data_path = path + "/gist_base.fvecs"
        if threads > 1:
            print(f"Multi-threads: {threads}")
            process_pool(threads, data_path, "gist_benchmark")
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

    data_dir = parent_path + "/test/data/benchmark/" + parser.parse_args().data_set
    print(f"Data Dir: {data_dir}")

    args = parser.parse_args()

    insert_data(args.threads, args.data_set, path=data_dir)
