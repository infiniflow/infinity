import random
import numpy as np


def generate_sparse_data(row_n: int, max_dim: int, sparsity: float, max_col: int = 4096):
    nnz = int(row_n * max_dim * sparsity)
    indptr = np.zeros(row_n + 1).astype(np.int64)
    while True:
        indptr[0] = 0
        for i, r in zip(range(1, row_n), random.sample(range(nnz - 1), row_n - 1)):
            indptr[i] = r + 1
        indptr[row_n] = nnz
        indptr.sort()

        if all(indptr[i + 1] - indptr[i] <= max_col for i in range(row_n)):
            break

    indices = np.zeros(nnz).astype(np.int32)
    data = np.random.random(nnz).astype(np.float32)
    for row_id in range(row_n):
        start, end = indptr[row_id], indptr[row_id + 1]
        for i, r in zip(range(start, end), random.sample(range(max_dim), end - start)):
            indices[i] = r
        indices[start:end] = np.sort(indices[start:end])
    return indptr, indices, data

def find_topk(indptr: list[int], indices: list[int], data: list[float], topk: int, qindices: list[int], qdata: list[float]):
    row_n = len(indptr) - 1
    dist = []
    qnnz = len(qindices)
    for row_id in range(row_n):
        start, end = indptr[row_id], indptr[row_id + 1]
        vindices = indices[start:end]
        vdata = data[start:end]

        ip = 0.0
        j = 0
        for i, d in zip(vindices, vdata):
            while j < qnnz and i > qindices[j]:
                j += 1
            if j < qnnz and i == qindices[j]:
                ip += d * qdata[j]
        dist.append((row_id, ip))
    dist.sort(key=lambda x: x[1], reverse=True)
    return [i for i, _ in dist[:topk]]
