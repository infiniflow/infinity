import argparse
from typing import Any
from elasticsearch import Elasticsearch, helpers
import json
import time
from typing import List, Optional
import os
import h5py

from .base_client import BaseClient, FieldValue


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

    def upload_bach(self, actions:List):
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
                        self.upload_bach(actions)
                        actions = []
                    record = json.loads(line)
                    actions.append({"_index": self.collection_name, "_source": record})
                if actions:
                    self.upload_bach(actions)
        elif ext == '.hdf5' and self.data['mode'] == 'vector':
            with h5py.File(dataset_path, 'r') as f:
                actions = []
                for i, line in enumerate(f['train']):
                    if i % batch_size == 0 and i != 0:
                        self.upload_bach(actions)
                        actions = []
                    record = {self.data['vector_name']: line}
                    actions.append({"_index": self.collection_name, "_source": record})
                if actions:
                    self.upload_bach(actions)
        else:
            raise TypeError("Unsupported file type")

    def build_condition(
        self, and_subfilters: Optional[List[Any]], or_subfilters: Optional[List[Any]]
    ) -> Optional[Any]:
        return {
            "bool": {
                "must": and_subfilters,
                "should": or_subfilters,
            }
        }

    def build_exact_match_filter(self, field_name: str, value: FieldValue) -> Any:
        return {"term": {field_name: value}}
    
    def build_text_match_filter(self, field_name: str, text: FieldValue) -> Any:
        return {"match": {field_name: text}}

    def build_range_filter(
        self,
        field_name: str,
        lt: Optional[FieldValue],
        gt: Optional[FieldValue],
        lte: Optional[FieldValue],
        gte: Optional[FieldValue],
    ) -> Any:
        return {"range": {field_name: {"lt": lt, "gt": gt, "lte": lte, "gte": gte}}}

    def search(self) -> list[list[Any]]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters.
        The function returns id list.
        """
        query_path = os.path.join(self.path_prefix, self.data["query_path"])

        results = []
        _, ext = os.path.splitext(query_path)
        if ext == '.json':
            with open(query_path, 'r') as f:
                queries = json.load(f)
                start = time.time()
                if self.data['mode'] == 'fulltext':
                    for query in queries:
                        result = self.client.search(index=self.collection_name,
                                                    source=["_id"],
                                                    body=query['body'])
                        results.append(result)
                end = time.time()
            print("latency:", (end-start)*1000/len(queries))
        elif ext == '.hdf5' and self.data['mode'] == 'vector':
            with h5py.File(query_path, 'r') as f:
                start = time.time()
                for query in f['test']:
                    knn = {
                        "field": self.data["vector_name"],
                        "query_vector": query,
                        "k": self.data["topK"],
                        "num_candidates": 200
                    }
                    result = self.client.search(index=self.collection_name,
                                                source=["_id"],
                                                knn=knn,
                                                size = self.data["topK"])
                    results.append(result)
                end = time.time()
                print("latency: ", (end-start)*1000/len(f['test']))
        else:
            raise TypeError("Unsupported file type")
        return results
