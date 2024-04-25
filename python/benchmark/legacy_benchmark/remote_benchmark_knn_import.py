import argparse
import os
import time

import infinity
from infinity import index
from infinity.common import REMOTE_HOST
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.table import RemoteTable


def import_data(path):
    if os.path.exists(path + "/sift_base.fvecs"):
        import_sift_1m(path + "/sift_base.fvecs")
    elif os.path.exists(path + "/gist_base.fvecs"):
        import_gist_1m(path + "/gist_base.fvecs")
    else:
        raise Exception("Invalid data set")


def import_sift_1m(path):
    infinity_obj = infinity.connect(REMOTE_HOST)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table("sift_benchmark")
    db_obj.create_table("sift_benchmark", {"col1": {"type": "vector,128,float"}})
    table_obj = db_obj.get_table("sift_benchmark")
    assert table_obj

    assert os.path.exists(path)

    start = time.time()
    res = table_obj.import_data(path, {'file_type': 'fvecs'})
    end = time.time()
    dur = end - start
    print(f"Import sift_1m cost time: {dur} s")

    assert res.error_code == ErrorCode.OK

    start = time.time()
    create_index("sift_benchmark")
    end = time.time()
    dur = end - start
    print(f"Create index on sift_1m cost time: {dur} s")


def import_gist_1m(path):
    infinity_obj = infinity.connect(REMOTE_HOST)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table("gist_benchmark")
    db_obj.create_table("gist_benchmark", {"col1": {"type": "vector,960,float"}})
    table_obj = db_obj.get_table("gist_benchmark")
    assert table_obj

    assert os.path.exists(path)

    start = time.time()
    res = table_obj.import_data(path, {'file_type': 'fvecs'})
    end = time.time()
    dur = end - start
    print(f"Import gist_1m cost time: {dur} s")

    assert res.error_code == ErrorCode.OK

    start = time.time()
    create_index("gist_benchmark")
    end = time.time()
    dur = end - start
    print(f"Create index on gist_1m cost time: {dur} s")


def create_index(table_name):
    conn = ThriftInfinityClient(REMOTE_HOST)
    table = RemoteTable(conn, "default_db", table_name)
    res = table.create_index("hnsw_index",
                             [index.IndexInfo("col1",
                                              index.IndexType.Hnsw,
                                              [
                                                  index.InitParameter("M", "16"),
                                                  index.InitParameter("ef_construction", "200"),
                                                  index.InitParameter("ef", "200"),
                                                  index.InitParameter("metric", "l2"),
                                                  index.InitParameter("encode", "lvq")
                                              ])])

    assert res.error_code == ErrorCode.OK


if __name__ == '__main__':
    current_path = os.getcwd()
    parent_path = os.path.dirname(current_path)
    parent_path = os.path.dirname(parent_path)

    print(f"Current Path: {current_path}")
    print(f"Parent Path: {parent_path}")

    parser = argparse.ArgumentParser(description="Benchmark Infinity")

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

    import_data(path=data_dir)
