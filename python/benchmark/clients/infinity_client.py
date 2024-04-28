import argparse
import json
import os
import h5py
import time
import numpy as np
from typing import Any, List

import infinity
import infinity.index as index
from infinity import NetworkAddress
from .base_client import BaseClient
import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
import csv

class InfinityClient(BaseClient):
    def __init__(self,
                 mode: str,
                 options: argparse.Namespace,
                 drop_old: bool = True) -> None:
        """
        The mode configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        with open(mode, 'r') as f:
            self.data = json.load(f)
        self.client = infinity.connect(NetworkAddress("127.0.0.1", 23817))
        self.collection_name = self.data['name']
        self.path_prefix = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    def _parse_index_schema(self, index_schema):
        indexs = []
        for key, value in index_schema.items():
            if value['type'] == 'text':
                indexs.append(index.IndexInfo(key, index.IndexType.FullText, []))
            elif value['type'] == 'HNSW':
                params = []
                for param, v in value['params'].items():uplo
                    params.append(index.InitParameter(param, str(v)))
                indexs.append(index.IndexInfo(key, index.IndexType.Hnsw, params))
        return indexs

    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        db_obj = self.client.get_database("default_db")
        db_obj.drop_table(self.collection_name)
        db_obj.create_table(self.collection_name, self.data["schema"])
        table_obj = db_obj.get_table(self.collection_name)
        dataset_path = os.path.join(self.path_prefix, self.data["data_path"])
        if not os.path.exists(dataset_path):
            self.download_data(self.data["data_link"], dataset_path)
        batch_size = self.data["insert_batch_size"]
        features = list(self.data["schema"].keys())
        _, ext = os.path.splitext(dataset_path)
        if ext == '.json':
            with open(dataset_path, 'r') as f:
                actions = []
                for i, line in enumerate(f):
                    if i % batch_size == 0 and i != 0:
                        table_obj.insert(actions)
                        actions = []
                    record = json.loads(line)
                    action = {}
                    for feature in features:
                        action[feature] = record.get(feature, "")
                    actions.append(action)
                if actions:
                    table_obj.insert(actions)
        elif ext == '.hdf5':
            with h5py.File(dataset_path, 'r') as f:
                actions = []
                # line is vector
                for i, line in enumerate(f['train']):
                    if i % batch_size == 0 and i != 0:
                        table_obj.insert(actions)
                        actions = []
                    record = {self.data['vector_name']: line.tolist()}
                    actions.append(record)
                if actions:
                    table_obj.insert(actions)
        elif ext == '.csv':
            if self.data["use_import"]:
               table_obj.import_data(dataset_path, import_options={"delimiter": "\t"})
            else:
                custom_headers = []
                headers = self.data["schema"]
                for key, value in headers.items():
                    custom_headers.append(key)

                with open(dataset_path, 'r', encoding='utf-8', errors='replace') as data_file:
                    current_batch = []
                    for i, line in enumerate(data_file):
                        row = line.strip().split('\t')
                        if (i % 100000 == 0):
                            print(f"row {i}")
                        if len(row) != len(headers):
                            print(f"row = {i}, row_len = {len(row)}, not equal headers len, skip")
                            continue
                        row_dict = {header: value for header, value in zip(headers, row)}
                        current_batch.append(row_dict)
                        if len(current_batch) >= batch_size:
                            table_obj.insert(current_batch)
                            current_batch = []

                    if current_batch:
                        table_obj.insert(current_batch)

        # create index
        indexs = self._parse_index_schema(self.data["index"])
        for i, idx in enumerate(indexs):
            table_obj.create_index(f"index{i}", [idx])

    def parse_fulltext_query(self, query: dict) -> Any:
        condition = query["body"]["query"]

        key, value = list(condition.items())[0]
        ret = f'{list(value.keys())[0]}:"{list(value.values())[0]}"'
        if key == 'and':
            ret = '&&'.join(f'{list(value.keys())[0]}:"{list(value.values())[0]}"')
        elif key == 'or':
            ret = '||'.join(f'{list(value.keys())[0]}:"{list(value.values())[0]}"')

        return ret

    def search(self) -> list[list[Any]]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters.
        The function returns id list.
        """
        db_obj = self.client.get_database("default_db")
        table_obj = db_obj.get_table(self.collection_name)
        query_path = os.path.join(self.path_prefix, self.data["query_path"])
        _, ext = os.path.splitext(query_path)
        results = []
        if ext == '.hdf5':
            with h5py.File(query_path, 'r') as f:
                for line in f['test']:
                    start = time.time()
                    res, _ = table_obj.output(["_row_id"]).knn(self.data["vector_name"], line.tolist(), "float", self.data['metric_type'], self.data["topK"]).to_result()
                    latency = (time.time() - start) * 1000
                    result = [[x[0] for x in res['ROW_ID']]]
                    result.append(latency)
                    results.append(result)
        elif ext == '.json':
            with open(query_path, 'r') as f:
                queries = json.load(f)
                for query in queries:
                    if self.data['mode'] == 'fulltext':
                        match_condition = self.parse_fulltext_query(query)
                    start = time.time()
                    res, _ = table_obj.output(["_row_id", "_score"]).match("", match_condition, f"topn={self.data['topK']}").to_result()
                    latency = (time.time() - start) * 1000
                    result = [(row_id[0], score) for row_id, score in zip(res['ROW_ID'], res['SCORE'])]
                    result.append(latency)
                    results.append(result)
        else:
            raise TypeError("Unsupported file type")
        return results

    def check_and_save_results(self, results: List[List[Any]]):
        for i, result in enumerate(results):
            print(f"result {i} = {result}")

            if len(result) == 1:
                print(f"not found term, cost time = {results[0]}")
            elif len(result) > 1:
                for row, score in result[:-1]:
                    print(f"row = {row}, score = {score}")
                print(f"query cost time: {result[-1]}ms")
        # ground_truth_path = self.data['ground_truth_path']
        # _, ext = os.path.splitext(ground_truth_path)
        # precisions = []
        # latencies = []
        # if ext == '.hdf5':
        #     with h5py.File(ground_truth_path, 'r') as f:
        #         expected_result = f['neighbors']
        #         assert len(expected_result) == len(results)
        #         for i, result in enumerate(results):
        #             ids = set(result[0])
        #             precision = len(ids.intersection(expected_result[i][:self.data['topK']])) / self.data['topK']
        #             precisions.append(precision)
        #             latencies.append(result[-1])
        # elif ext == '.json':
        #     with open(ground_truth_path, 'r') as f:
        #         expected_results = json.load(f)
        #         for i, result in enumerate(results):
        #             ids = set(x[0] for x in result[:-1])
        #             precision = len(ids.intersection(expected_results[i]['expected_results'][:self.data['topK']])) / self.data['topK']
        #             precisions.append(precision)
        #             latencies.append(result[-1])
        #
        # print(f"mean_time: {np.mean(latencies)}, mean_precisions: {np.mean(precisions)}, \
        #       std_time: {np.std(latencies)}, min_time: {np.min(latencies)}, \
        #       max_time: {np.max(latencies)}, p95_time: {np.percentile(latencies, 95)}, \
        #       p99_time: {np.percentile(latencies, 99)}")