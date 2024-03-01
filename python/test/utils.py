import functools
import os
import subprocess
import traceback

import numpy as np


# catch the expected exception
def trace_expected_exceptions(func):
    @functools.wraps(func)
    def wrapped_func(*args, **kwargs):
        try:
            func(*args, **kwargs)
        except:
            print('Expected exception in ' + func.__name__)
            traceback.print_exc()

    return wrapped_func


# read fvecs file
def read_fvecs_file(filename):
    file_int32 = np.fromfile(filename, dtype='int32')
    d = file_int32[0]
    ivecs = file_int32.reshape(-1, d + 1)[:, 1:].copy()
    fvecs = ivecs.view('float32')
    return fvecs


def start_infinity_service_in_subporcess():
    # os.system("rm -fr /tmp/infinity")
    shell = "/home/fann/infinity/build/src/infinity"
    with open("./tmp.txt", "w") as f:
        infinity = subprocess.Popen(shell, stdout=f)
    return infinity
