import os
import numpy as np

def bvecs_mmap(fname):
    x = np.memmap(fname, dtype='uint8', mode='r')
    d = x[:4].view('int32')[0]
    return x.reshape(-1, d + 4)[:, 4:]

def sanitize(x):
    return np.ascontiguousarray(x, dtype='float32')


def get_queries(path):
    return sanitize(bvecs_mmap(path)[:])


print(get_queries('/home/infiniflow/Documents/data/bigann_query.bvecs'))