# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import os
import time

import infinity
from infinity import index
from infinity.common import LOCAL_HOST, LOCAL_INFINITY_PATH, ConflictType
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.table import RemoteTable


def import_data(path, dataset: str, m: int, ef_construction: int, build_type: str, encode_type: str, compress_type: str, remote: bool):
    print(
        f"dataset: {dataset}, m: {m}, ef_construction: {ef_construction}, remote: {remote}"
    )
    if dataset == "sift_1m":
        import_sift_1m(path + "/sift_base.fvecs", m, ef_construction, build_type, encode_type, compress_type, remote)
    elif dataset == "sift_10k":
        import_sift_1m(path + "/sift10k_base.fvecs", m, ef_construction, build_type, encode_type, compress_type, remote)
    elif dataset == "gist_1m":
        import_gist_1m(path + "/gist_base.fvecs", m, ef_construction, build_type, encode_type, compress_type, remote)
    elif dataset == "msmarco_1m":
        import_msmarco_1m(path + "/msmarco_base.fvecs", m, ef_construction, build_type, encode_type, compress_type, remote)
    else:
        raise Exception("Invalid data set")


def import_sift_1m(path, m: int, ef_construction: int, build_type: str, encode_type: str, compress_type: str, remote: bool):
    infinity_obj = None
    if remote:
        infinity_obj = infinity.connect(LOCAL_HOST)
    else:
        infinity_obj = infinity.connect(LOCAL_INFINITY_PATH)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    table_name = f"sift_benchmark"
    db_obj.drop_table(table_name)
    db_obj.create_table(table_name, {"col1": {"type": "vector,128,float"}})
    table_obj = db_obj.get_table(table_name)
    assert table_obj

    assert os.path.exists(path)

    start = time.time()
    res = table_obj.import_data(path, {"file_type": "fvecs"})
    end = time.time()
    dur = end - start
    print(f"Import sift_1m cost time: {dur} s")

    assert res.error_code == ErrorCode.OK

    start = time.time()
    create_index(table_obj, m, ef_construction, build_type, encode_type, compress_type)
    end = time.time()
    dur = end - start
    print(f"Create index on sift_1m cost time: {dur} s")


def import_gist_1m(path, m: int, ef_construction: int, build_type: str, encode_type: str, compress_type: str, remote: bool):
    infinity_obj = None
    if remote:
        infinity_obj = infinity.connect(LOCAL_HOST)
    else:
        infinity_obj = infinity.connect(LOCAL_INFINITY_PATH)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    table_name = f"gist_benchmark"
    db_obj.drop_table(table_name)
    db_obj.create_table(table_name, {"col1": {"type": "vector,960,float"}})
    table_obj = db_obj.get_table(table_name)
    assert table_obj

    assert os.path.exists(path)

    start = time.time()
    res = table_obj.import_data(path, {"file_type": "fvecs"})
    end = time.time()
    dur = end - start
    print(f"Import gist_1m cost time: {dur} s")

    assert res.error_code == ErrorCode.OK

    start = time.time()
    create_index(table_obj, m, ef_construction, build_type, encode_type, compress_type)
    end = time.time()
    dur = end - start
    print(f"Create index on gist_1m cost time: {dur} s")


def import_msmarco_1m(path, m: int, ef_construction: int, build_type: str, encode_type: str, compress_type: str, remote: bool):
    infinity_obj = None
    if remote:
        infinity_obj = infinity.connect(LOCAL_HOST)
    else:
        infinity_obj = infinity.connect(LOCAL_INFINITY_PATH)
    assert infinity_obj

    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    table_name = f"msmarco_benchmark"
    db_obj.drop_table(table_name)
    db_obj.create_table(table_name, {"col1": {"type": "vector,1024,float"}})
    table_obj = db_obj.get_table(table_name)
    assert table_obj

    assert os.path.exists(path)

    start = time.time()
    res = table_obj.import_data(path, {"file_type": "fvecs"})
    end = time.time()
    dur = end - start
    print(f"Import msmarco_1m cost time: {dur} s")

    assert res.error_code == ErrorCode.OK

    start = time.time()
    create_index(table_obj, m, ef_construction, build_type, encode_type, compress_type)
    end = time.time()
    dur = end - start
    print(f"Create index on msmarco_1m cost time: {dur} s")


def create_index(table_obj, m: int, ef_construction: int, build_type: str, encode_type: str, compress_type: str):
    res = table_obj.drop_index("hnsw_index", ConflictType.Ignore)

    assert res.error_code == ErrorCode.OK

    res = table_obj.create_index(
        "hnsw_index",
        index.IndexInfo(
            "col1",
            index.IndexType.Hnsw,
            {
                "m": str(m),
                "ef_construction": str(ef_construction),
                "metric": "l2",
                "build_type": build_type,
                "encode": encode_type
            },
        )
    )

    assert res.error_code == ErrorCode.OK

    if compress_type == "lvq":
        res = table_obj.optimize("hnsw_index", {"compress_to_lvq": "true"})

    assert res.error_code == ErrorCode.OK


def str2bool(value):
    if isinstance(value, bool):
        return value
    if value.lower() in ("yes", "true", "t", "y", "1"):
        return True
    elif value.lower() in ("no", "false", "f", "n", "0"):
        return False
    else:
        raise argparse.ArgumentTypeError("Boolean value expected")


if __name__ == "__main__":
    current_path = os.getcwd()

    parser = argparse.ArgumentParser(description="Benchmark Infinity")

    parser.add_argument("-d", "--data", type=str, default="sift_1m", dest="data_set")
    parser.add_argument("--m", type=int, default=16, dest="m")
    parser.add_argument("--efc", type=int, default=200, dest="ef_construction")
    parser.add_argument("-b", "--build", type=str, default="plain", dest="build_type")  # plain, lsg
    parser.add_argument("-e", "--encode", type=str, default="lvq", dest="encode_type")  # plain, lvq
    parser.add_argument("-C", "--compress", type=str, default="plain", dest="compress_type")  # plain, lvq
    parser.add_argument("-R", "--remote", type=str2bool, default=True, dest="remote")

    args = parser.parse_args()

    data_dir = current_path + "/test/data/benchmark/" + args.data_set
    print(f"Data Dir: {data_dir}")

    import_data(data_dir, args.data_set, args.m, args.ef_construction, args.build_type, args.encode_type, args.compress_type, args.remote)
