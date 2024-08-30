from infinity import index
import csv
import numpy as np


class SimpleEmbeddingGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                yield [i, [0.1, 0.2, 0.3, 0.4]]

        return gen

    def index():
        return index.IndexInfo(
            "c2",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "l2"},
        )


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
        return index.IndexInfo("c2", index.IndexType.FullText)


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
        f = open(enwiki_path, "r")

        def gen(insert_n: int):
            i = 0
            while i < insert_n:
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
        return index.IndexInfo("body", index.IndexType.FullText)


class SiftGenerator:
    def columns():
        return {"vec": {"type": "vector,128,float"}}

    def gen_factory(sift_path: str):
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
        return index.IndexInfo(
            "vec",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "l2"},
        )
