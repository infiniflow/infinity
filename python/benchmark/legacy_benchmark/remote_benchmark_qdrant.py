import argparse
import os
import time
import multiprocessing
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


def gt_read_all(filename: str):
    if not os.path.exists(filename):
        raise FileNotFoundError(f"{filename} not found")
    with open(filename, "rb") as f:
        num = struct.unpack("i", f.read(4))[0]
        topk = struct.unpack("i", f.read(4))[0]
        gt = []
        for _ in range(num):
            row = []
            for _ in range(topk):
                row.append(struct.unpack("i", f.read(4))[0])
            gt.append(row)
        # score = []
        # for _ in range(num):
        #     row = []
        #     for _ in range(topk):
        #         row.append(struct.unpack("f", f.read(4))[0])
        #     score.append(row)
    return topk, gt


def work(remote, queries, topk, alpha, beta):
    if remote:
        client = QdrantClient(host="localhost", port=6333)

    for indices, values in queries:
        res = client.search(
            collection_name="splade_benchmark_collection",
            query_vector=models.NamedSparseVector(
                name="text",
                vector=models.SparseVector(
                    indices=indices,
                    values=values
                ),
            ),
            limit = topk
        )


def process_pool(remote, threads, rounds, query_path, topk, alpha, beta):
    query_mat = csr_read_all(query_path)

    queries = [[] for _ in range(threads)]
    for i in range(query_mat.nrow):
        queries[i % threads].append(query_mat.at(i))

    results = []
    total_queries_count = query_mat.nrow

    for i in range(rounds):
        p = multiprocessing.Pool(threads)
        start = time.time()
        for idx in range(threads):
            p.apply_async(work, args=(remote, queries[idx], topk, alpha, beta))
        p.close()
        p.join()
        end = time.time()
        dur = end - start

        results.append(f"Round {i + 1}:")
        results.append(f"Total Dur: {dur:.2f} s")
        results.append(f"Query Count: {total_queries_count}")
        results.append(f"QPS: {(total_queries_count / dur):.2f}")
    for res in results:
        print(res)


def calculate_recall(gt, query_res):
    correct = 0
    for gt_row, query_row in zip(gt, query_res):
        for gt_val in gt_row:
            if gt_val in query_row:
                correct += 1
    return correct / (len(gt) * len(gt[0]))


def one_thread(remote, rounds, query_path, gt_path, alpha, beta):
    if remote:
        client = QdrantClient(host="localhost", port=6333)

    query_mat = csr_read_all(query_path)
    topk, gt = gt_read_all(gt_path)

    results = []
    dur_sum = 0

    for i in range(rounds):
        query_results = [[] for _ in range(query_mat.nrow)]
        dur = 0.0
        for query_id in range(query_mat.nrow):
            indices, values = query_mat.at(query_id)
            start = time.time()
            res_list  = client.search(
                collection_name="splade_benchmark_collection",
                query_vector=models.NamedSparseVector(
                    name="text",
                    vector=models.SparseVector(
                        indices=indices,
                        values=values
                    ),
                ),
                limit=topk
            )

            end = time.time()
            dur += end - start

            for res in res_list:
                query_results[query_id].append(res.id)

        topk1, gt = gt_read_all(gt_path)
        assert topk == topk1
        assert len(res_list) == topk
        recall = calculate_recall(gt, query_results)
        results.append(f"Round {i + 1}:")
        results.append(f"Total Dur: {dur:.2f} s")
        results.append(f"Query Count: {query_mat.nrow}")
        results.append(f"QPS: {(query_mat.nrow / dur):.2f}")
        results.append(f"Recall@{topk}: {recall}")

        dur_sum += dur
    dur_sum /= rounds
    results.append(f"Avg Dur: {dur_sum:.2f} s")
    results.append(f"Avg Qps: {(query_mat.nrow / dur_sum):.2f}")

    #client.delete_collection("splade_benchmark_collection")

    for res in results:
        print(res)


def benchmark(
    threads: int,
    data_set: str,
    rounds: int,
    topk: int,
    alpha: float,
    beta: float,
    remote: bool,
):
    current_path = os.getcwd()
    data_dir = current_path + "/test/data/benchmark/splade"
    print(f"Data Dir: {data_dir}")

    query_path = data_dir + "/queries.dev.csr"
    if data_set == "splade_small":
        gt_path = data_dir + "/base_small.dev.gt"
    elif data_set == "splade_1M":
        gt_path = data_dir + "/base_1M.dev.gt"
    elif data_set == "splade_full":
        gt_path = data_dir + "/base_full.dev.gt"
    else:
        raise Exception("Invalid data set")

    if threads > 1:
        print(f"Multi-thread: {threads}")
        print(f"Rounds: {rounds}")
        process_pool(remote, threads, rounds, query_path, topk, alpha, beta)
    else:
        print(f"Single-thread")
        print(f"Rounds: {rounds}")
        one_thread(remote, rounds, query_path, gt_path, alpha, beta)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Benchmark Infinity")

    parser.add_argument(
        "-t",
        "--threads",
        type=int,
        default=1,
        dest="threads",
    )
    parser.add_argument(
        "-d",
        "--data_set",
        type=str,
        default="splade_small",
        dest="data_set",
    )
    parser.add_argument(
        "--rounds",
        type=int,
        default=3,
        dest="rounds",
    )
    parser.add_argument(
        "--topk",
        type=int,
        default=10,
        dest="topk",
    )
    parser.add_argument(
        "--alpha",
        type=float,
        default=0.92,
        dest="alpha",
    )
    parser.add_argument(
        "--beta",
        type=float,
        default=0.5,
        dest="beta",
    )
    parser.add_argument(
        "--remote",
        action="store_true",
        default=True,
        dest="remote",
    )
    args = parser.parse_args()

    benchmark(
        args.threads,
        args.data_set,
        args.rounds,
        args.topk,
        args.alpha,
        args.beta,
        args.remote,
    )