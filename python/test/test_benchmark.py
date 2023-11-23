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


def worker_internal_connection(thread_id, num_iterations, some_function):
    print(f">>> Thread ID: {thread_id} <<<")
    for j in range(num_iterations):
        # Simulate the "work" to be done during each iteration.
        infinity_obj = infinity.connect(NetworkAddress('0.0.0.0', 9090))
        some_function(infinity_obj, thread_id, j)
        infinity_obj.disconnect()


def measure_time_internal(num_threads, num_times, some_function):
    # Calculate how many iterations each thread should do
    num_iterations = num_times // num_threads

    start_time = time.perf_counter()
    threads = []
    for i in range(num_threads):
        thread = threading.Thread(target=worker_internal_connection, args=(i, num_iterations, some_function))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    end_time = time.perf_counter()

    elapsed_time = end_time - start_time
    return elapsed_time


class TestBenchmark:
    def test_connect_and_disconnect_ok(self):
        # create threads
        t1 = threading.Thread(target=print_numbers)
        t2 = threading.Thread(target=print_letters)

        # start threads
        t1.start()
        t2.start()

        # wait for threads to finish
        t1.join()
        t2.join()

    def test_measure_time(self):
        # Using the function
        num_threads = 16
        num_times = 16 * 1000
        result: list = []

        def create_database(infinity_obj, thread_id, num_iteration):
            infinity_obj.create_database(f"my_database_{thread_id}_{num_iteration}", None)

        elapsed_time = measure_time_internal(num_threads, num_times, create_database)
        print(f"QPS: {num_times / elapsed_time} records per second")
        print(f"Elapsed time: {elapsed_time} seconds")
