from infinity import index
import csv
import json
import numpy as np
from infinity.common import SparseVector


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


class EnwikiGenerator:
    def columns():
        return {
            "title": {"type": "varchar"},
            "date": {"type": "varchar"},
            "body": {"type": "varchar"},
        }

    def gen_factory(enwiki_path: str):
        if not enwiki_path.endswith(".csv"):
            raise ValueError("enwiki_path should be csv file")

        def gen(insert_n: int):
            i = 0
            while i < insert_n:
                f = open(enwiki_path, "r")
                reader = csv.reader(f, delimiter="\t")
                for row in reader:
                    title = row[0]
                    date = row[1]
                    body = row[2]
                    yield [title, date, body]
                    i += 1
                    if i >= insert_n:
                        break

        return gen

    def index():
        return [index.IndexInfo("body", index.IndexType.FullText)]


class SiftGenerator:
    def columns():
        return {"vec": {"type": "vector,128,float"}}

    def gen_factory(sift_path: str):
        if not sift_path.endswith(".fvecs"):
            raise ValueError("sift_path should be fvecs file")

        def gen(insert_n: int):
            i = 0
            while i < insert_n:
                f = open(sift_path, "rb")
                while i < insert_n:
                    buf = f.read(4)
                    if len(buf) == 0:
                        break
                    dim = int.from_bytes(buf, byteorder="little")
                    assert dim == 128
                    buf = f.read(128 * 4)
                    vec = np.frombuffer(buf, dtype=np.float32).tolist()
                    yield [vec]
                    i += 1

        return gen

    def index():
        return [
            index.IndexInfo(
                "vec",
                index.IndexType.Hnsw,
                {"M": "16", "ef_construction": "20", "metric": "l2"},
            )
        ]


class LChYDataGenerato:
    def columns():
        return {
            "id": {"type": "varchar"},
            "content": {"type": "varchar"},
            "dense_vec": {"type": "vector, 1024, float"},
            "sparse_vec": {"type": "sparse,250002,float,int"},
        }

    def gen_factory(data_path: str):
        # check data_path is jsonl file
        if not data_path.endswith(".jsonl"):
            raise ValueError("data_path should be jsonl file")

        def gen(insert_n: int):
            def convert_sparse(sparse_data: dict[str, float]):
                indices = []
                values = []
                for k, v in sparse_data.items():
                    indices.append(int(k))
                    values.append(v)
                return SparseVector(indices, values)

            i = 0
            while i < insert_n:
                f = open(data_path, "r")
                for line in f:
                    data = json.loads(line)

                    yield [
                        data["id"],
                        data["content"],
                        data["dense_vec"],
                        convert_sparse(data["sparse_vec"]),
                    ]
                    i += 1
                    if i >= insert_n:
                        break

        return gen

    def index():
        return [
            index.IndexInfo("content", index.IndexType.FullText),
            index.IndexInfo(
                "dense_vec",
                index.IndexType.Hnsw,
                {
                    "encode": "lvq",
                    "m": "16",
                    "ef_construction": "200",
                    "metric": "l2",
                },
            ),
            index.IndexInfo(
                "sparse_vec",
                index.IndexType.BMP,
                {"block_size": "8", "compress_type": "compress"},
            ),
        ]


if __name__ == "__main__":
    pass
