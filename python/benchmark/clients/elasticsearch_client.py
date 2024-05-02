import argparse
from typing import Any
from elasticsearch import Elasticsearch, helpers
import json
import time
from typing import List
import os
import h5py
import uuid
import numpy as np
import logging

from .base_client import BaseClient


class ElasticsearchClient(BaseClient):
    def __init__(self,
                 mode: str,
                 options: argparse.Namespace,
                 drop_old: bool = True) -> None:
        """
        The mode configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        with open(mode, 'r') as f:
            self.data = json.load(f)
        self.client = Elasticsearch(self.data['connection_url'])
        self.collection_name = self.data['name']
        self.path_prefix = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    def upload_batch(self, actions: List):
        helpers.bulk(self.client, actions)

    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        if self.client.indices.exists(index=self.collection_name):
            self.client.indices.delete(index=self.collection_name)
        self.client.indices.create(index=self.collection_name, body=self.data['index'])
        batch_size = self.data["insert_batch_size"]
        dataset_path = os.path.join(self.path_prefix, self.data["data_path"])
        if not os.path.exists(dataset_path):
            self.download_data(self.data["data_link"], dataset_path)
        _, ext = os.path.splitext(dataset_path)
        if ext == '.json':
            with open(dataset_path, 'r') as f:
                actions = []
                for i, line in enumerate(f):
                    if i % batch_size == 0 and i != 0:
                        self.upload_batch(actions)
                        actions = []
                    record = json.loads(line)
                    actions.append({"_index": self.collection_name, "_id": uuid.UUID(int=i).hex, "_source": record})
                if actions:
                    self.upload_batch(actions)
        elif ext == '.hdf5' and self.data['mode'] == 'vector':
            with h5py.File(dataset_path, 'r') as f:
                actions = []
                for i, line in enumerate(f['train']):
                    if i % batch_size == 0 and i != 0:
                        self.upload_batch(actions)
                        actions = []
                    record = {self.data['vector_name']: line}
                    actions.append({"_index": self.collection_name, "_id": uuid.UUID(int=i).hex, "_source": record})
                if actions:
                    self.upload_batch(actions)
        elif ext == '.csv':
            custom_headers = []
            headers = self.data["index"]["mappings"]["properties"]
            for key, value in headers.items():
                custom_headers.append(key)
            with open(dataset_path, 'r', encoding='utf-8', errors='replace') as data_file:
                current_batch = []
                for i, line in enumerate(data_file):
                    row = line.strip().split('\t')
                    if len(row) != len(headers):
                        logging.info(f"row = {i}, row_len = {len(row)}, not equal headers len, skip")
                        continue
                    row_dict = {header: value for header, value in zip(headers, row)}
                    current_batch.append({"_index": self.collection_name, "_id": uuid.UUID(int=i).hex, "_source": row_dict})
                    if len(current_batch) >= batch_size:
                        self.upload_batch(current_batch)
                        current_batch = []

                if current_batch:
                    self.upload_batch(current_batch)
        else:
            raise TypeError("Unsupported file type")
        
        self.client.indices.forcemerge(index=self.collection_name, wait_for_completion=True)

    def parse_fulltext_query(self, query: dict) -> Any:
        condition = query["body"]["query"]
        key, value = list(condition.items())[0]
        ret = {}
        if key == 'and':
            ret = {
                "query": {
                    "bool": {
                        "must": [{"match": item} for item in value]
                    }
                }
            }
        elif key == 'or':
            ret = {
                "query": {
                    "bool": {
                        "should": [{"match": item} for item in value]
                    }
                }
            }
        elif key == 'match':
            ret = {
                "query": {
                    "match": {
                        list(value.keys())[0]: list(value.values())[0]
                    }
                }
            }
        return ret

    def parse_fulltext_query_content(self, query_content: dict) -> Any:
        ret = {
            "query": {
                "match": query_content
            }
        }
        return ret

    def search(self) -> list[list[Any]]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters.
        The function returns id list.
        """
        query_path = os.path.join(self.path_prefix, self.data["query_path"])
        logging.info(query_path)
        results = []
        _, ext = os.path.splitext(query_path)
        if ext == '.json' or ext == '.jsonl':
            with open(query_path, 'r') as f:
                queries = json.load(f)
                if self.data['mode'] == 'fulltext':
                    for query in queries:
                        body = self.parse_fulltext_query(query)
                        topk = self.data['topK']
                        start = time.time()
                        result = self.client.search(index=self.collection_name,
                                                    body=body)
                        end = time.time()
                        latency = (end - start) * 1000
                        result = [(uuid.UUID(hex=hit['_id']).int, hit['_score']) for hit in result['hits']['hits']]
                        result.append(latency)
                        results.append(result)
        elif ext == '.hdf5' and self.data['mode'] == 'vector':
            with h5py.File(query_path, 'r') as f:
                for query in f['test']:
                    knn = {
                        "field": self.data["vector_name"],
                        "query_vector": query,
                        "k": self.data["topK"],
                        "num_candidates": 200
                    }
                    start = time.time()
                    result = self.client.search(index=self.collection_name,
                                                source=["_id", "_score"],
                                                knn=knn,
                                                size=self.data["topK"])
                    end = time.time()
                    latency = (end - start) * 1000
                    result = [(uuid.UUID(hex=hit['_id']).int, hit['_score']) for hit in result['hits']['hits']]
                    result.append(latency)
                    results.append(result)
        elif ext == '.txt':
            with open(query_path, 'r') as f:
                if self.data['mode'] == 'fulltext':
                    for line in f:
                        query = {"body" : line[:-1]}
                        body = self.parse_fulltext_query_content(query)
                        topk = self.data['topK']
                        start = time.time()
                        result = self.client.search(index=self.collection_name,
                                                    body=body)
                        end = time.time()
                        latency = (end - start) * 1000
                        result = [(uuid.UUID(hex=hit['_id']).int, hit['_score']) for hit in result['hits']['hits']]
                        result.append(latency)
                        logging.info(f"{line[:-1]}, {latency}")
                        results.append(result)
        else:
            raise TypeError("Unsupported file type")
        return results
    
    def check_and_save_results(self, results: List[List[Any]]):
        if 'ground_truth_path' in self.data:
            ground_truth_path = self.data['ground_truth_path']
            _, ext = os.path.splitext(ground_truth_path)
            precisions = []
            latencies = []
            if ext == '.hdf5':
                with h5py.File(ground_truth_path, 'r') as f:
                    expected_result = f['neighbors']
                    for i, result in enumerate(results):
                        ids = set(x[0] for x in result[:-1])
                        precision = len(ids.intersection(expected_result[i][:self.data['topK']])) / self.data['topK']
                        precisions.append(precision)
                        latencies.append(result[-1])
            elif ext == '.json' or ext == '.jsonl':
                with open(ground_truth_path, 'r') as f:
                    for i, line in enumerate(f):
                        expected_result = json.loads(line)
                        result = results[i]
                        ids = set(x[0] for x in result[:-1])
                        precision = len(ids.intersection(expected_result['expected_results'][:self.data['topK']])) / self.data['topK']
                        precisions.append(precision)
                        latencies.append(result[-1])

            logging.info(
                f'''mean_time: {np.mean(latencies)}, mean_precisions: {np.mean(precisions)},
                    std_time: {np.std(latencies)}, min_time: {np.min(latencies)}, \n
                    max_time: {np.max(latencies)}, p95_time: {np.percentile(latencies, 95)},
                    p99_time: {np.percentile(latencies, 99)}''')
        else:
            latencies = []
            for result in results:
                latencies.append(result[-1])
            logging.info(
                f'''mean_time: {np.mean(latencies)}, std_time: {np.std(latencies)},
                    max_time: {np.max(latencies)}, min_time: {np.min(latencies)}, 
                    p95_time: {np.percentile(latencies, 95)}, p99_time: {np.percentile(latencies, 99)}''')
