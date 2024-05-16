# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import functools
import threading
import time
import traceback
from datetime import datetime

import pandas as pd

import infinity
from infinity import NetworkAddress
from infinity.errors import ErrorCode


def trace_unhandled_exceptions(func):
    @functools.wraps(func)
    def wrapped_func(*args, **kwargs):
        try:
            func(*args, **kwargs)
        except:
            print('Exception in ' + func.__name__)
            traceback.print_exc()

    return wrapped_func


def worker_thread(thread_id, num_iterations, some_function, ip='127.0.0.1', port=9090):
    infinity_obj = infinity.connect(NetworkAddress(ip, port))
    try:
        for j in range(num_iterations):
            some_function(infinity_obj, port, thread_id, j)
    except Exception as e:
        print(f"Exception: {e}")
    finally:
        infinity_obj.disconnect()


def worker_internal_connection(num_threads, num_iterations, some_function, ip=None, port=None):
    threads = []
    for j in range(num_threads):
        thread = threading.Thread(target=worker_thread, args=(
            j, num_iterations, some_function, ip, port))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()


def measure_time_internal(num_threads, num_times, some_function, ip=None, port=None):
    # Calculate how many iterations each thread should do
    num_iterations = num_times // num_threads

    start_time = time.perf_counter()
    worker_internal_connection(
        num_threads, num_iterations, some_function, ip, port)
    end_time = time.perf_counter()

    elapsed_time = end_time - start_time
    return elapsed_time


def execute(some_functions: list, protocols: list, num_threads, num_times) -> pd.DataFrame:
    results = pd.DataFrame(
        columns=['rpc name', 'function', 'qps', 'elapsed_time', 'average_latency', 'num_threads',
                 'num_times'])
    print(f"\n")

    for (protocol, ip, port) in protocols:
        for some_function in some_functions:
            elapsed_time = measure_time_internal(
                num_threads, num_times, some_function, ip, port)
            qps = num_times / elapsed_time  # queries per second
            avg_latency = (elapsed_time / num_times) * 1000  # in ms

            results.loc[len(results)] = [protocol,
                                         some_function.__name__,
                                         qps,
                                         elapsed_time,
                                         avg_latency,
                                         num_threads,
                                         num_times]

    pd.set_option('display.max_columns', None)
    pd.set_option('display.width', None)
    return results


class TestBenchmark:

    def test_measure_time(self):
        @trace_unhandled_exceptions
        def create_database(infinity_obj, port, thread_id, num_iteration):
            res = infinity_obj.create_database(
                f"my_database_{port}_{thread_id}_{num_iteration}")
            if res.error_code != ErrorCode.OK:
                raise Exception(f"create_database failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def get_database(infinity_obj, port, thread_id, num_iteration):
            db_obj = infinity_obj.get_database(f"default_db")
            if db_obj is None:
                raise Exception(f"get_database failed")

        @trace_unhandled_exceptions
        def list_databases(infinity_obj, port, thread_id, num_iteration):
            res = infinity_obj.list_databases()
            if res.error_code != ErrorCode.OK:
                raise Exception(f"list_databases failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def drop_database(infinity_obj, port, thread_id, num_iteration):
            res = infinity_obj.drop_database(
                f"my_database_{port}_{thread_id}_{num_iteration}")
            if res.error_code != ErrorCode.OK:
                raise Exception(f"drop_database failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def create_table(infinity_obj, port, thread_id, num_iteration):
            res = infinity_obj.get_database(f"default_db").create_table(
                f"table_{port}_{thread_id}_{num_iteration}",
                {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}})
            if res.error_code != ErrorCode.OK:
                raise Exception(f"create_table failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def insert_table(infinity_obj, port, thread_id, num_iteration):
            res = (infinity_obj
                   .get_database(f"default_db")
                   .get_table(f"table_{port}_{thread_id}_{num_iteration}")
                   .insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}]))
            if res.error_code != ErrorCode.OK:
                raise Exception(f"insert_table failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def list_tables(infinity_obj, port, thread_id, num_iteration):
            (infinity_obj
             .get_database(f"default_db")
             .list_tables())

        @trace_unhandled_exceptions
        def select_table(infinity_obj, port, thread_id, num_iteration):
            res = (infinity_obj
                   .get_database(f"default_db")
                   .get_table(f"table_{port}_{thread_id}_{num_iteration}")
                   .query_builder()
                   .output(["*"])
                   .filter("c1 > 1").to_df())
            if res is None:
                raise Exception(f"select_table failed: {res}")

        @trace_unhandled_exceptions
        def drop_table(infinity_obj, port, thread_id, num_iteration):
            res = (infinity_obj
                   .get_database(f"default_db")
                   .drop_table(f"table_{port}_{thread_id}_{num_iteration}"))
            if res.error_code != ErrorCode.OK:
                raise Exception(f"drop_table failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def create_index(infinity_obj, port, thread_id, num_iteration):
            res = (infinity_obj
                   .get_database(f"default_db")
                   .get_table(f"table_{port}_{thread_id}_{num_iteration}")
                   .create_index("my_index", ["c1"], "IVF_FLAT", None))
            if res.error_code != ErrorCode.OK:
                raise Exception(f"create_index failed: {res.error_msg}")

        @trace_unhandled_exceptions
        def drop_index(infinity_obj, port, thread_id, num_iteration):
            res = (infinity_obj
                   .get_database(f"default_db")
                   .get_table(f"table_{port}_{thread_id}_{num_iteration}")
                   .drop_index("my_index"))
            if res.error_code != ErrorCode.OK:
                raise Exception(f"drop_index failed: {res.error_msg}")

        ############################################
        # Using the tune

        ip: str = '127.0.0.1'
        thrift = ("Thrift", ip, 23817)
        thread_pool_thrift = ("Thread Pool Thrift", ip, 23817)
        async_thrift = ("AsyncThrift", ip, 23817)
        num_threads = 1
        num_times = 10
        protocols = [thread_pool_thrift]

        database_functions = [create_database]

        db_df = execute(database_functions, protocols,
                        num_threads, num_times)

        table_functions = []
        tbl_df = execute(table_functions, protocols,
                         num_threads, num_times)

        # index_functions = []
        # idx_df = execute(index_functions, protocols, num_threads, num_times)

        df = pd.concat([db_df, tbl_df])
        print(df)
        df.to_excel(f"{datetime.now()}_benchmark.xlsx")


if __name__ == "__main__":
    TestBenchmark().test_measure_time()
