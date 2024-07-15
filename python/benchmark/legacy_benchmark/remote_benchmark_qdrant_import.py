import argparse
import os
import time
import struct

from qdrant_client import QdrantClient, models
from qdrant_client.models import PointStruct
from qdrant_client.models import Distance, VectorParams

class SparseMatrix:
    nrow: int
    ncol: int
    nnz: int
    indptr: list[int]
    indices: list[int]
    data: list[float]

    def __init__(self, nrow, ncol, nnz, indptr, indices, data):
        self.nrow = nrow
        self.ncol = ncol
        self.nnz = nnz
        self.indptr = indptr
        self.indices = indices
        self.data = data

    def at(self, i: int):
        start, end = self.indptr[i], self.indptr[i + 1]
        return self.indices[start:end], self.data[start:end]


def csr_read_all(filename: str) -> SparseMatrix:
    if not os.path.exists(filename):
        raise FileNotFoundError(f"{filename} not found")
    with open(filename, "rb") as f:
        nrow = struct.unpack("q", f.read(8))[0]
        ncol = struct.unpack("q", f.read(8))[0]
        nnz = struct.unpack("q", f.read(8))[0]
        indptr = []
        for _ in range(nrow + 1):
            indptr.append(struct.unpack("q", f.read(8))[0])
        indices = []
        for _ in range(nnz):
            indices.append(struct.unpack("i", f.read(4))[0])
        data = []
        for _ in range(nnz):
            data.append(struct.unpack("f", f.read(4))[0])
    return SparseMatrix(nrow, ncol, nnz, indptr, indices, data)

def import_data(
    data_set: str,
    block_size: int,
    compress: bool,
    opt_topk: int,
    bp_reorder: bool,
    remote: bool,
):
    current_path = os.getcwd()
    data_dir = current_path + "/test/data/benchmark/splade"
    print(f"Data Dir: {data_dir}")

    if data_set == "splade_small":
        import_path = data_dir + "/base_small.csr"
    elif data_set == "splade_1M":
        import_path = data_dir + "/base_1M.csr"
    elif data_set == "splade_full":
        import_path = data_dir + "/base_full.csr"
    else:
        raise Exception("Invalid data set")
    print(f"Data Dir: {import_path}")

    if remote:
        client = QdrantClient(host="localhost", port=6333)

    if client.collection_exists("splade_benchmark_collection"):
        client.delete_collection("splade_benchmark_collection")
    #print(client.collection_exists("splade_benchmark_collection"))

    client.create_collection(
        collection_name="splade_benchmark_collection",
        vectors_config={},
        sparse_vectors_config={
            "text": models.SparseVectorParams(),
        },
        on_disk_payload=True,
    )
    #print(client.collection_exists("splade_benchmark_collection"))

    assert os.path.exists(import_path)
    start = time.time()

    data_mat = csr_read_all(import_path)

    points = []

    for data_id in range(data_mat.nrow):
        indices, values = data_mat.at(data_id)
        if len(points) == 1024:
            client.upsert(
                collection_name="splade_benchmark_collection",
                points=points,
            )
            points.clear()
            print(data_id)
        else:
            points.append(models.PointStruct(
                        id=data_id,
                        payload={},  # Add any additional payload if necessary
                        vector={
                            "text": models.SparseVector(
                                indices=indices,
                                values=values
                            )
                        },
                    )
            )

    end = time.time()
    dur = end - start
    print(f"Import splade cost time: {dur} s")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Benchmark Infinity")
    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default="splade_small",
        dest="data_set",
    )
    parser.add_argument("--block_size", type=int, default=8, dest="block_size")
    parser.add_argument("--compress", type=bool, default=True, dest="compress")
    parser.add_argument("--opt_topk", type=int, default=10, dest="opt_topk")
    parser.add_argument("--bp_reorder", type=bool, default=True, dest="bp_reorder")
    parser.add_argument("-R", "--remote", type=bool, default=True, dest="remote")

    args = parser.parse_args()

    import_data(
        args.data_set,
        args.block_size,
        args.compress,
        args.opt_topk,
        args.bp_reorder,
        args.remote,
    )