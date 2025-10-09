import infinity
from infinity.table import ExplainType
from infinity import index
import sys
import os
import time
# from common import common_values
from infinity.common import ConflictType
import pathlib
from infinity.errors import ErrorCode


class SimpleEmbeddingGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                yield [i, [0.1, 0.2, 0.3, 0.4]]

        return gen

    def index():
        return [
            index.IndexInfo(
                "c2",
                index.IndexType.Hnsw,
                {"M": "16", "ef_construction": "20", "metric": "l2"},
            )
        ]


class SimpleVarcharGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "varchar"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                if i % 2 == 0:
                    yield [i, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"]
                else:
                    yield [i, "test"]

        return gen

    def index():
        return [index.IndexInfo("c2", index.IndexType.FullText)]


class SimpleTensorGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "tensor,4,float"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                if i % 2 == 0:
                    yield [i, [0.1, 0.2, 0.3, 0.4]]
                else:
                    yield [i, [[0.1, 0.2, 0.3, 0.4], [0.5, 0.6, 0.7, 0.8]]]

        return gen


config = "test/data/config/restart_test/test_cleanup/1.toml"
data_dir = "/var/infinity/data"
table_name = "test_cleanup_index"
index_name = "idx0"
index_name2 = "idx1"
table_id = 0
index_id = 1
index2_id = 2

insert_n = 100

indexes = SimpleEmbeddingGenerator.index()
data_gen_factory = SimpleEmbeddingGenerator.gen_factory()
columns = SimpleEmbeddingGenerator.columns()

infinity_obj = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))

db_obj = infinity_obj.get_database("default_db")
table_obj = db_obj.get_table(table_name)
data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
count_star = data_dict["count(star)"][0]
assert count_star == insert_n

db_obj.drop_table(table_name, ConflictType.Error)

infinity_obj.cleanup()
