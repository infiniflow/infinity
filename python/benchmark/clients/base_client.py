import sys
from abc import abstractmethod
from typing import Any
import subprocess
import os
import time
import logging
import random
import json
import h5py
import numpy as np
import threading
import multiprocessing

from .utils import csr_read_all, gt_read_all, calculate_recall

class BaseClient:
    """
    Base class for all clients(Qdrant, ES, infinity).
    mode is a string that corresponds to a JSON file's address in the configurations.
    Each client reads the required parameters from the JSON configuration file.
    """

    def __init__(self, conf_path: str) -> None:
        """
        The conf_path configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        self.data = None
        self.queries = list()
        self.clients = list()
        # Following are for multithreading
        self.mt_lock = threading.Lock()
        self.mt_query_batch = 10
        self.mt_next_begin = 0
        self.mt_done_queries = 0
        self.mt_active_workers = 0
        self.mt_results = []
        # Following are for multiprocessing
        self.mp_manager = multiprocessing.Manager()
        self.mp_lock = multiprocessing.Lock()
        self.mp_query_batch = multiprocessing.Value("i", 10, lock=False)
        self.mp_next_begin = multiprocessing.Value("i", 0, lock=False)
        self.mp_done_queries = multiprocessing.Value("i", 0, lock=False)
        self.mp_active_workers = multiprocessing.Value("i", 0, lock=False)
        self.mp_results = self.mp_manager.list()

    @abstractmethod
    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        pass

    @abstractmethod
    def setup_clients(self, num_workers=1):
        pass

    @abstractmethod
    def do_single_query(self, query_id, client_id) -> list[Any]:
        pass

    def download_data(self, url, target_path):
        """
        Download dataset and extract it into path.
        """
        #Ask for downloading data in taeget path
        print(f"Dataset doesn't exist: {target_path},\n"
              f"downloading dataset in {target_path}?\n"
              f"[Y/n]")
        while(True):
            user_input = input()
            if user_input == 'Y' or user_input == 'y' :
                print("Downloading...")
                break
            elif user_input == 'N' or user_input == 'n':
                print("Exit")
                sys.exit(0)
            else :
                print("Invalid input!")

        _, ext = os.path.splitext(url)
        if ext == ".bz2":
            bz2_path = target_path + ".bz2"
            subprocess.run(["wget", "-O", bz2_path, url], check=True)
            subprocess.run(["bunzip2", bz2_path], check=True)
            extracted_path = os.path.splitext(bz2_path)[0]
            os.rename(extracted_path, target_path)
        else:
            subprocess.run(["wget", "-O", target_path, url], check=True)

    def search_mt(self, is_express=False, num_workers=1):
        self.setup_clients(num_workers)

        query_path = os.path.join(self.path_prefix, self.data["query_path"])
        _, ext = os.path.splitext(query_path)
        if self.data["mode"] == "fulltext":
            assert ext == ".txt"
            for line in open(query_path, "r"):
                line = line.strip()
                self.queries.append(line)
        elif self.data["mode"] == "vector":
            if ext == ".hdf5":
                with h5py.File(query_path, "r") as f:
                    self.queries = list(f["test"])
            else:
                assert ext == "jsonl"
                for line in open(query_path, "r"):
                    query = json.loads(line)["vector"]
                    self.queries.append(query)
        elif self.data["mode"] == "sparse_vector":
            assert ext == ".csr"
            query_mat = csr_read_all(query_path)
            for query_id in range(query_mat.nrow):
                indices, values = query_mat.at(query_id)
                self.queries.append((indices, values))
        else:
            raise TypeError("Unsupported type")

        self.mt_active_workers = num_workers
        threads = []
        for i in range(num_workers):
            threads.append(
                threading.Thread(
                    target=self.search_thread_mainloop,
                    args=[is_express, i],
                    daemon=True,
                )
            )
        for i in range(num_workers):
            threads[i].start()

        report_qps_sec = 60
        sleep_sec = 10
        sleep_cnt = 0
        done_warm_up = True
        if is_express:
            logging.info(f"Let database warm-up for {report_qps_sec} seconds")
            done_warm_up = False
        start = time.time()
        start_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(start))
        report_prev = start
        done_queries_prev = 0
        done_queries_curr = 0

        while self.mt_active_workers > 0:
            time.sleep(sleep_sec)
            sleep_cnt += 1
            if sleep_cnt < report_qps_sec / sleep_sec:
                continue
            sleep_cnt = 0
            now = time.time()
            if done_warm_up:
                with self.mt_lock:
                    done_queries_curr = self.mt_done_queries
                avg_start = done_queries_curr / (now - start)
                avg_interval = (done_queries_curr - done_queries_prev) / (
                    now - report_prev
                )
                done_queries_prev = done_queries_curr
                report_prev = now
                logging.info(
                    f"average QPS since {start_str}: {avg_start}, average QPS of last interval: {avg_interval}"
                )
            else:
                query_batch = 10
                with self.mt_lock:
                    # how many queries a single worker can do in 10ms
                    query_batch = int(
                        self.mt_done_queries / (num_workers * report_qps_sec * 100)
                    )
                    query_batch = min(100, query_batch)
                    query_batch = max(query_batch, 10)
                    self.mt_query_batch = query_batch
                    self.mt_done_queries = 0
                start = now
                start_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(start))
                report_prev = now
                done_warm_up = True
                logging.info(
                    f"Collecting statistics for 30 minutes and print QPS so far every minute. Workers will report every {query_batch} queries. Type Ctrl+C to quit."
                )

        for i in range(num_workers):
            threads[i].join()
        if not is_express:
            self.save_and_check_results(self.mt_results)

    def search_thread_mainloop(self, is_express: bool, client_id: int):
        num_queries = len(self.queries)
        if is_express:
            query_batch = 10
            with self.mt_lock:
                query_batch = self.mt_query_batch
            local_rng = random.Random()  # random number generator per thread
            deadline = time.time() + 30 * 60  # 30 minutes
            while time.time() < deadline:
                for i in range(query_batch):
                    query_id = local_rng.randrange(0, num_queries)
                    _ = self.do_single_query(query_id, client_id)
                with self.mt_lock:
                    self.mt_done_queries += query_batch
                    query_batch = self.mt_query_batch
        else:
            begin = 0
            end = 0
            local_results = list()
            while end < num_queries:
                with self.mt_lock:
                    self.mt_done_queries += end - begin
                    begin = self.mt_next_begin
                    end = begin + self.mt_query_batch
                    if end > num_queries:
                        end = num_queries
                    self.mt_next_begin = end
                for query_id in range(begin, end):
                    start = time.time()
                    result = self.do_single_query(query_id, client_id)
                    latency = (time.time() - start) * 1000
                    result = [(query_id, latency)] + result
                    local_results.append(result)
            with self.mt_lock:
                self.mt_done_queries += end - begin
                self.mt_results += local_results
        with self.mt_lock:
            self.mt_active_workers -= 1

    def search_mp(self, is_express=False, num_workers=1):
        query_path = os.path.join(self.path_prefix, self.data["query_path"])
        _, ext = os.path.splitext(query_path)
        if self.data["mode"] == "fulltext":
            assert ext == ".txt"
            for line in open(query_path, "r"):
                line = line.strip()
                self.queries.append(line)
        elif self.data["mode"] == "vector":
            if ext == ".hdf5":
                with h5py.File(query_path, "r") as f:
                    self.queries = list(f["test"])
        elif self.data["mode"] == "sparse_vector":
            assert ext == ".csr"
            query_mat = csr_read_all(query_path)
            for query_id in range(query_mat.nrow):
                indices, values = query_mat.at(query_id)
                self.queries.append((indices, values))
        else:
            raise TypeError("Unsupported type")

        self.mp_active_workers.value = num_workers
        workers = []
        for i in range(num_workers):
            workers.append(
                multiprocessing.Process(
                    target=self.search_process_mainloop,
                    args=[is_express],
                    daemon=True,
                )
            )
        for i in range(num_workers):
            workers[i].start()

        report_qps_sec = 60
        sleep_sec = 10
        sleep_cnt = 0
        done_warm_up = True
        if is_express:
            logging.info(f"Let database warm-up for {report_qps_sec} seconds")
            done_warm_up = False
        start = time.time()
        start_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(start))
        report_prev = start
        done_queries_prev = 0
        done_queries_curr = 0

        while True:
            active_workers = 0
            with self.mp_lock:
                active_workers = self.mp_active_workers.value
            if active_workers <= 0:
                break
            time.sleep(sleep_sec)
            sleep_cnt += 1
            if sleep_cnt < report_qps_sec / sleep_sec:
                continue
            sleep_cnt = 0
            now = time.time()
            if done_warm_up:
                with self.mp_lock:
                    done_queries_curr = self.mp_done_queries.value
                avg_start = done_queries_curr / (now - start)
                avg_interval = (done_queries_curr - done_queries_prev) / (
                    now - report_prev
                )
                done_queries_prev = done_queries_curr
                report_prev = now
                logging.info(
                    f"average QPS since {start_str}: {avg_start}, average QPS of last interval: {avg_interval}"
                )
            else:
                query_batch = 10
                with self.mp_lock:
                    # how many queries a single worker can do in 10ms
                    query_batch = int(
                        self.mp_done_queries.value
                        / (num_workers * report_qps_sec * 100)
                    )
                    query_batch = min(100, query_batch)
                    query_batch = max(query_batch, 10)
                    self.mp_query_batch.value = query_batch
                    self.mp_done_queries.value = 0
                start = now
                start_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(start))
                report_prev = now
                done_warm_up = True
                logging.info(
                    f"Collecting statistics for 30 minutes and print QPS so far every minute. Workers will report every {query_batch} queries. Type Ctrl+C to quit."
                )

        for i in range(num_workers):
            workers[i].join()
        if not is_express:
            self.save_and_check_results(self.mp_results)

    def search_process_mainloop(self, is_express: bool):
        self.setup_clients(1)  # socket is unsafe to share among workers
        num_queries = len(self.queries)
        if is_express:
            query_batch = 10
            with self.mp_lock:
                query_batch = self.mp_query_batch.value
            local_rng = random.Random()  # random number generator per thread
            deadline = time.time() + 30 * 60  # 30 minutes
            while time.time() < deadline:
                for i in range(query_batch):
                    query_id = local_rng.randrange(0, num_queries)
                    _ = self.do_single_query(query_id, 0)
                with self.mp_lock:
                    self.mp_done_queries.value += query_batch
                    query_batch = self.mp_query_batch.value
        else:
            begin = 0
            end = 0
            local_results = list()
            while end < num_queries:
                with self.mp_lock:
                    self.mp_done_queries.value += end - begin
                    begin = self.mp_next_begin.value
                    end = begin + self.mp_query_batch.value
                    if end > num_queries:
                        end = num_queries
                    self.mp_next_begin.value = end
                for query_id in range(begin, end):
                    start = time.time()
                    result = self.do_single_query(query_id, 0)
                    latency = (time.time() - start) * 1000
                    result = [(query_id, latency)] + result
                    local_results.append(result)
            with self.mp_lock:
                self.mp_done_queries.value += end - begin
            self.mp_results += local_results
        with self.mp_lock:
            self.mp_active_workers.value -= 1

    def save_and_check_results(self, results: list[list[Any]]):
        """
        Compare the search results with ground truth to calculate recall.
        """
        results = sorted(results, key=lambda x: x[0][0])
        if "result_path" in self.data:
            result_path = self.data["result_path"]
            with open(result_path, "w") as f:
                for result in results:
                    line = json.dumps(result)
                    f.write(line + "\n")
            logging.info("query_result_path: {0}".format(result_path))
        latencies = []
        for result in results:
            latencies.append(result[0][1])
        logging.info(
            f"""mean_time: {np.mean(latencies)}, std_time: {np.std(latencies)},
                max_time: {np.max(latencies)}, min_time: {np.min(latencies)},
                p95_time: {np.percentile(latencies, 95)}, p99_time: {np.percentile(latencies, 99)}"""
        )
        if "ground_truth_path" in self.data:
            ground_truth_path = self.data["ground_truth_path"]
            _, ext = os.path.splitext(ground_truth_path)
            precisions = []
            if self.data["mode"] == "vector":
                assert ext == ".hdf5"
                with h5py.File(ground_truth_path, "r") as f:
                    expected_result = f["neighbors"]
                    for i, result in enumerate(results):
                        ids = []
                        if self.data["app"] == "qdrant" :
                            for ScoredPoint in result[1:]:
                                ids.append(((ScoredPoint.id >> 32) << 23) + (ScoredPoint.id & 0xFFFFFFFF))
                        else :#elasticsearch & infinity
                            ids = [((x >> 32) << 23) + (x & 0xFFFFFFFF) for x in result[1:]]
                        precision = (
                            len(set(ids).intersection(expected_result[i][1:]))
                            / self.data["topK"]
                        )
                        precisions.append(precision)
            elif self.data["mode"] == "sparse_vector":
                assert ext == ".gt"
                topk, gt = gt_read_all(ground_truth_path)
                assert topk == self.data["topK"]
                query_results = [[] for _ in range(len(self.queries))]
                if self.data["app"] == "qdrant":
                    for query_id, result in enumerate(results):
                        for res in result[1:]:
                            query_results[query_id].append(res.id)
                elif self.data["app"] == "infinity":
                    for query_id, result in enumerate(results):
                        for res in result[1:]:
                            query_results[query_id].append(res)

                recall = calculate_recall(gt, query_results)
                precisions.append(recall)
            else:
                assert ext == ".json" or ext == ".jsonl"
                with open(ground_truth_path, "r") as f:
                    for i, line in enumerate(f):
                        expected_result = json.loads(line)
                        exp_ids = [
                            ((x[0] >> 32) << 23) + (x[0] & 0xFFFFFFFF)
                            for x in expected_result[1:]
                        ]
                        result = results[i]
                        ids = [
                            ((x[0] >> 32) << 23) + (x[0] & 0xFFFFFFFF)
                            for x in result[1:]
                        ]
                        precision = (
                            len(set(ids).intersection(exp_ids)) / self.data["topK"]
                        )
                        precisions.append(precision)
            logging.info(f"""mean_precisions: {np.mean(precisions)}""")

    def run_experiment(self, args):
        """
        run experiment and save results.
        """
        if args.import_data:
            start_time = time.time()
            self.upload()
            finish_time = time.time()
            logging.info(f"upload finish, cost time = {finish_time - start_time}")
        elif args.query >= 1 or args.query_express >= 1:
            is_express = True if args.query_express >= 1 else False
            search_func = self.search_mp if args.concurrency == "mp" else self.search_mt
            num_workers = max(args.query, args.query_express)
            search_func(is_express, num_workers)
