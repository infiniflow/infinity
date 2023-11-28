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
import os
import shutil

import pytest
import threading
import time
import infinity
from infinity.infinity import NetworkAddress
import multiprocessing
from datetime import datetime
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def worker_external_connection(thread_id, num_iterations, infinity_obj):
    print(f">>> Thread ID: {thread_id} <<<")
    for j in range(num_iterations):
        # Simulate the "work" to be done during each iteration.
        infinity_obj.create_database(f"my_database_{thread_id}_{j}")


def measure_time_external(num_threads, num_times):
    infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))

    start_time = time.perf_counter()

    # Calculate how many iterations each thread should do
    num_iterations = num_times // num_threads

    threads = []
    for i in range(num_threads):
        thread = threading.Thread(target=worker_external_connection, args=(i, num_iterations, infinity_obj))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    end_time = time.perf_counter()
    infinity_obj.disconnect()

    elapsed_time = end_time - start_time
    return elapsed_time


def worker_thread(process_id, thread_id, num_iterations, some_function, ip='0.0.0.0', port=9090):
    # print(f">>>Process ID: {process_id} Thread ID: {thread_id} <<<")
    for j in range(num_iterations):
        infinity_obj = infinity.connect(NetworkAddress(ip, port))
        some_function(infinity_obj, port, process_id, thread_id, j)
        infinity_obj.disconnect()


def worker_internal_connection(process_id, num_threads, num_iterations, some_function, ip=None, port=None):
    threads = []
    for j in range(num_threads):
        thread = threading.Thread(target=worker_thread, args=(process_id, j, num_iterations, some_function, ip, port))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()


def measure_time_internal(num_processes, num_threads, num_times, some_function, ip=None, port=None):
    # Calculate how many iterations each process should do
    num_iterations = num_times // num_processes // num_threads

    start_time = time.perf_counter()
    processes = []
    for i in range(num_processes):
        process = multiprocessing.Process(target=worker_internal_connection,
                                          args=(i, num_threads, num_iterations, some_function, ip, port))
        processes.append(process)
        process.start()

    # Wait for all threads to finish
    for process in processes:
        process.join()

    end_time = time.perf_counter()

    elapsed_time = end_time - start_time
    return elapsed_time


def execute(some_functions: list, protocols: list, num_processes, num_threads, num_times) -> pd.DataFrame:
    results = pd.DataFrame(
        columns=['rpc name', 'function', 'qps', 'elapsed_time', 'average_latency', 'num_processes', 'num_threads',
                 'num_times'])
    print(f"\n")

    for (protocol, ip, port) in protocols:
        for some_function in some_functions:
            elapsed_time = measure_time_internal(num_processes, num_threads, num_times, some_function, ip, port)
            qps = num_times / elapsed_time  # queries per second
            avg_latency = (elapsed_time / num_times) * 1000  # in ms

            results.loc[len(results)] = [protocol,
                                         some_function.__name__,
                                         qps,
                                         elapsed_time,
                                         avg_latency,
                                         num_processes,
                                         num_threads,
                                         num_times]

            # ## kill the infinity server
            # os.system("pkill -f infinity")
            # time.sleep(1)
            # ## remove the database directory
            # shutil.rmtree("/tmp/infinity")
            # ## start the infinity server
            # os.system("infinity --data_path /tmp/infinity &")
            pd.set_option('display.max_columns', None)
            pd.set_option('display.width', None)

    return results


class TestBenchmark:

    def test_measure_time(self):
        def create_database(infinity_obj, port, process_id, thread_id, num_iteration):
            infinity_obj.create_database(f"my_database_{port}_{process_id}_{thread_id}_{num_iteration}", None)

        def get_database(infinity_obj, port, process_id, thread_id, num_iteration):
            db_obj = infinity_obj.get_database(f"default")

        def list_databases(infinity_obj, port, process_id, thread_id, num_iteration):
            infinity_obj.list_databases()

        def drop_database(infinity_obj, port, process_id, thread_id, num_iteration):
            infinity_obj.drop_database(f"my_database_{port}_{process_id}_{thread_id}_{num_iteration}")

        def create_table(infinity_obj, port, process_id, thread_id, num_iteration):
            (infinity_obj
            .get_database(f"default")
            .create_table(
                f"table_{port}_{process_id}_{thread_id}_{num_iteration}",
                {"c1": "int, primary key", "c2": "float"}, None))

        def insert_table(infinity_obj, port, process_id, thread_id, num_iteration):
            (infinity_obj
             .get_database(f"default")
             .get_table(f"table_{port}_{process_id}_{thread_id}_{num_iteration}")
             .insert([{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}]))

        def list_tables(infinity_obj, port, process_id, thread_id, num_iteration):
            (infinity_obj
             .get_database(f"default")
             .list_tables())

        def select_table(infinity_obj, port, process_id, thread_id, num_iteration):
            (infinity_obj
             .get_database(f"default")
             .get_table(f"table_{port}_{process_id}_{thread_id}_{num_iteration}")
             .search()
             .output(["*"])
             .filter("c1 > 1").to_list())

        def drop_table(infinity_obj, port, process_id, thread_id, num_iteration):
            (infinity_obj
             .get_database(f"default")
             .drop_table(f"table_{port}_{process_id}_{thread_id}_{num_iteration}"))

        ############################################
        # Using the function
        ip: str = '0.0.0.0'
        grpc = ("G RPC", ip, 50052)
        brpc = ("B RPC", ip, 50051)
        thrift = ("Thrift", ip, 9090)
        num_processes = 16
        num_threads = 16
        num_times = 16 * 16 * 1
        protocols = [thrift, brpc, grpc]

        database_functions = [create_database, get_database, list_databases, drop_database]
        db_df = execute(database_functions, protocols, num_processes, num_threads, num_times)

        table_functions = [create_table, insert_table, select_table, list_tables, drop_table]
        tbl_df = execute(table_functions, protocols, num_processes, num_threads, num_times)

        df = pd.concat([db_df, tbl_df])
        print(df)
        df.to_excel(f"{datetime.now()}_benchmark.xlsx")
