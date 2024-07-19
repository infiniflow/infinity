import os
import struct
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
    return topk, gt

def calculate_recall(gt, query_res):
    correct = 0
    for gt_row, query_row in zip(gt, query_res):
        for gt_val in gt_row:
            if gt_val in query_row:
                correct += 1
    return correct / (len(gt) * len(gt[0]))