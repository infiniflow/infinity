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


def print_numbers():
    for i in range(10):
        print(i)


def print_letters():
    for letter in 'abcdefghij':
        print(letter)


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


def worker_thread(process_id, thread_id, num_iterations, some_function, port=None):
    # print(f">>>Process ID: {process_id} Thread ID: {thread_id} <<<")
    for j in range(num_iterations):
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', port))
        some_function(infinity_obj, port, process_id, thread_id, j)
        infinity_obj.disconnect()


def worker_internal_connection(process_id, num_threads, num_iterations, some_function, port=None):
    threads = []
    for j in range(num_threads):
        thread = threading.Thread(target=worker_thread, args=(process_id, j, num_iterations, some_function, port))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()


def measure_time_internal(num_processes, num_threads, num_times, some_function, port=None):
    # Calculate how many iterations each process should do
    num_iterations = num_times // num_processes // num_threads

    start_time = time.perf_counter()
    processes = []
    for i in range(num_processes):
        process = multiprocessing.Process(target=worker_internal_connection,
                                          args=(i, num_threads, num_iterations, some_function, port))
        processes.append(process)
        process.start()

    # Wait for all threads to finish
    for process in processes:
        process.join()

    end_time = time.perf_counter()

    elapsed_time = end_time - start_time
    return elapsed_time


def execute(some_functions: list, num_processes, num_threads, num_times):
    for some_function in some_functions:
        print(f"\n")
        print(f"[{some_function.__name__}]")
        print(f"Number of processes: {num_processes}")
        print(f"Number of threads: {num_threads}")
        print(f"Number of times: {num_times}")
        ports = [('THRIFT  ', 9090)]
        for (name, port) in ports:
            elapsed_time = measure_time_internal(num_processes, num_threads, num_times, some_function, port)
            print(f"{name} {some_function.__name__} QPS: {num_times / elapsed_time} records per second")


class TestBenchmark:

    def test_measure_time(self):
        # Using the function
        num_processes = 16
        num_threads = 16
        num_times = 16 * 16 * 100

        def create_database(infinity_obj, port, process_id, thread_id, num_iteration):
            res = infinity_obj.create_database(f"my_database_{port}_{process_id}_{thread_id}_{num_iteration}", None)
            assert res.success

        def get_database(infinity_obj, port, process_id, thread_id, num_iteration):
            db_obj = infinity_obj.get_database(f"default")
            assert db_obj.db_name == "default"

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

        functions = [create_database, get_database, create_table, insert_table]
        execute(functions, num_processes, num_threads, num_times)
