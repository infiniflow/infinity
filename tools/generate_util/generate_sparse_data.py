import random
import numpy as np


def generate_sparse_data(row_n: int, max_dim: int, sparsity: float):
    nnz = int(row_n * max_dim * sparsity)
    indptr = np.zeros(row_n + 1).astype(np.int64)
    indptr[0] = 0
    for i, r in zip(range(1, row_n), random.sample(range(nnz - 1), row_n - 1)):
        indptr[i] = r + 1
    indptr[row_n] = nnz
    indptr.sort()

    indices = np.zeros(nnz).astype(np.int32)
    data = np.random.random(nnz).astype(np.float32)
    for row_id in range(row_n):
        start, end = indptr[row_id], indptr[row_id + 1]
        for i, r in zip(range(start, end), random.sample(range(max_dim), end - start)):
            indices[i] = r
        indices[start:end] = np.sort(indices[start:end])
    return indptr, indices, data
