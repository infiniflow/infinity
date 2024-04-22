import argparse
import json
import os
import h5py
import time
from typing import Any

import infinity
import infinity.index as index
from infinity import NetworkAddress
from .base_client import BaseClient


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
                for param, v in value['params'].items():
                    params.append(index.InitParameter(param, str(v)))
                indexs.append(index.IndexInfo(key, index.IndexType.Hnsw, params))
        return indexs

    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        db_obj = self.client.get_database("default")
        db_obj.drop_table(self.collection_name)
        db_obj.create_table(self.collection_name, self.data["schema"])
        table_obj = db_obj.get_table(self.collection_name)
        dataset_path = os.path.join(self.path_prefix, self.data["data_path"])
        if not os.path.exists(dataset_path):
            self.download_data(self.data["data_link"], dataset_path)
        batch_size = self.data["batch_size"]
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
        
        # create index
        indexs = self._parse_index_schema(self.data["index"])
        for i, idx in enumerate(indexs):
            table_obj.create_index(f"index{i}", [idx])

    def search(self) -> list[list[Any]]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters.
        The function returns id list.
        """
        db_obj = self.client.get_database("default")
        table_obj = db_obj.get_table(self.collection_name)
        query_path = os.path.join(self.path_prefix, self.data["query_path"])
        _, ext = os.path.splitext(query_path)
        results = []
        if ext == '.hdf5':
            with h5py.File(query_path, 'r') as f:
                start = time.time()
                for line in f['test']:
                    res = table_obj.output(["_row_id"]).knn(self.data["vector_name"], line.tolist(), "float", self.data['metric_type'], self.data["topK"]).to_pl()
                    results.append(res)
                end = time.time()
                print(f"search latency: {(end-start)*1000/len(f['test'])} ms")
        return results