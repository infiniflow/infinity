import os
from infinity import index
import csv
import json
import numpy as np
import random
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

    def import_file() -> str:
        base_filepath = "test/data/jsonl/test_table.jsonl"
        filepath = "test/data/jsonl/test_table_gen.jsonl"
        if os.path.exists(filepath):
            return filepath
        if not os.path.exists("test/data/jsonl"):
            os.makedirs("test/data/jsonl")
        with open(base_filepath, "r") as base_file:
            base_data = base_file.readlines()
        with open(filepath, "w") as new_file:
            for _ in range(100):
                new_file.writelines(base_data)
        return filepath

    def import_size() -> int:
        return 1000


class MultiIndexTypesGenerator:
    """Generator for multiple index types test data"""

    DEFAULT_CSV_FILE = "test/data/csv/test_multi_index_types.csv"
    DEFAULT_NUM_ROWS = 10000000

    def columns():
        return {
            "id": {"type": "int"},
            "text": {"type": "varchar"},
            "category": {"type": "varchar"},
            "vector_col": {"type": "vector,4,float"},
            "multi_vector_col": {"type": "multivector,3,float"},
            "sparse_col": {"type": "sparse,100,float,int8"}
        }

    def _generate_sparse_vector(self, dim, density=0.3):
        indices = []
        values = []
        for i in range(dim):
            if random.random() < density:
                indices.append(i)
                values.append(round(random.random(), 6))
        if not indices:
            indices = [0]
            values = [round(random.random(), 6)]
        return indices, values

    def _generate_vector(self, dim):
        return [round(random.random(), 6) for _ in range(dim)]

    def _generate_multivector(self, sub_vec_dim, num_sub_vecs):
        result = []
        for _ in range(num_sub_vecs):
            for _ in range(sub_vec_dim):
                result.append(round(random.random(), 6))
        return result


    def _format_vector(self, vec):
        return "[" + ", ".join(str(v) for v in vec) + "]"

    def _format_multivector(self, mv):
        # multivector uses 1D flattened format like tensor: [e1, e2, ..., eN]
        return "[" + ", ".join(str(v) for v in mv) + "]"


    def _format_sparse(self, indices, values):
        return "[" + ", ".join(f"{idx}:{val}" for idx, val in zip(indices, values)) + "]"

    def _generate_csv_internal(self, output_file, num_rows, seed=42):
        random.seed(seed)
        categories = ["A", "B", "C", "D"]
        text_words = ["apple", "banana", "cherry", "date", "elder", "fig", "grape"]
        os.makedirs(os.path.dirname(output_file), exist_ok=True)

        with open(output_file, 'w', newline='') as f:
            writer = csv.writer(f)
            for i in range(num_rows):
                row_id = i
                text = f"test_text_{i}_{random.choice(text_words)}"
                category = categories[i % len(categories)]
                vector = self._generate_vector(4)
                multivec = self._generate_multivector(3, 3)  # dimension 3, 3 sub-vectors
                sparse_indices, sparse_values = self._generate_sparse_vector(100, density=0.3)

                writer.writerow([
                    row_id, text, category,
                    self._format_vector(vector),
                    self._format_multivector(multivec),
                    self._format_sparse(sparse_indices, sparse_values)
                ])
        print(f"Generated {num_rows} rows to {output_file}")

    def gen_factory(csv_path: str):
        if not csv_path.endswith(".csv"):
            raise ValueError("csv_path should be csv file")

        def gen(insert_n: int):
            i = 0
            while i < insert_n:
                f = open(csv_path, "r")
                reader = csv.reader(f)
                for row in reader:
                    yield [int(row[0]), row[1], row[2], row[3], row[4], row[5]]
                    i += 1
                    if i >= insert_n:
                        break
        return gen

    def index():
        return [
            index.IndexInfo("text", index.IndexType.FullText),
            index.IndexInfo("id", index.IndexType.Secondary, {"cardinality": "high"}),
            index.IndexInfo("vector_col", index.IndexType.Hnsw, {"M": "16", "ef_construction": "50", "metric": "l2"}),
            index.IndexInfo("multi_vector_col", index.IndexType.Hnsw, {"M": "16", "ef_construction": "50", "metric": "l2"}),
            index.IndexInfo("sparse_col", index.IndexType.BMP, {"block_size": "8", "compress_type": "compress"}),
            index.IndexInfo("category", index.IndexType.Secondary, {"cardinality": "low"}),
        ]

    def import_file() -> str:
        filepath = MultiIndexTypesGenerator.DEFAULT_CSV_FILE
        if os.path.exists(filepath):
            return filepath
        gen = MultiIndexTypesGenerator()
        gen._generate_csv_internal(filepath, MultiIndexTypesGenerator.DEFAULT_NUM_ROWS)
        return filepath

    def import_size() -> int:
        return MultiIndexTypesGenerator.DEFAULT_NUM_ROWS


if __name__ == "__main__":
    pass
