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


def copy_data(file_name):
    data_dir = os.getcwd() + common_values.TEST_DATA_DIR
    for dirpath, dirnames, filenames in os.walk(data_dir):
        for filename in filenames:
            if filename == file_name:
                src_path = os.path.join(dirpath, filename)
                dest_path = os.path.join(common_values.TEST_TMP_DIR, filename)
                copyfile(src_path, dest_path)


def generate_big_int_csv(num, filename):
    with open(os.getcwd() + common_values.TEST_DATA_DIR + "csv/" + filename, "w") as f:
        for i in range(num):
            f.write(str(i) + "," + str(i) + '\n')
    f.close()


def generate_big_rows_csv(num, filename):
    with open(os.getcwd() + common_values.TEST_DATA_DIR + "csv/" + filename, "w") as f:
        for i in range(num):
            f.write(str(i) + ",asdasdlkfjio@!#!@asd #$%$23\n")
    f.close()


def generate_big_columns_csv(num, filename):
    with open(os.getcwd() + common_values.TEST_DATA_DIR + "csv/" + filename, "w") as f:
        data = ''.join(str(i) + ',' for i in range(num - 1))
        data += str(num - 1)
        f.write(data)
    f.close()
