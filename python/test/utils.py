import functools
import os
import subprocess
import time
import traceback
from shutil import copyfile
import numpy as np
import pytest

from common import common_values

current_path = os.getcwd()
data_dir = current_path + common_values.TEST_DATA_DIR
copy_dir = common_values.TEST_TMP_DIR


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
    shell = os.getcwd() + "/build/src/infinity"
    with open("./tmp.txt", "w") as f:
        infinity = subprocess.Popen(shell, stdout=f)
    time.sleep(1)
    return infinity


@pytest.fixture(scope="class")
def check_data(request):
    dir_name = request.param["dir_name"]
    file_name = request.param["file_name"]
    data_dir = request.param["data_dir"]
    # path not exists
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)
    if not os.path.exists(data_dir + file_name):
        for dirpath, dirnames, filenames in os.walk(dir_name):
            for filename in filenames:
                if filename == file_name:
                    src_path = os.path.join(dirpath, filename)
                    dest_path = os.path.join(copy_dir, filename)
                    copyfile(src_path, dest_path)
