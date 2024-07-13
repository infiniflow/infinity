from typing import Any
from elasticsearch import Elasticsearch, helpers
import json
from typing import List
import os
import h5py
import uuid
import logging
import re

from .base_client import BaseClient


class ElasticsearchClient(BaseClient):
    def __init__(self, conf_path: str) -> None:
        """
        The mode configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        BaseClient.__init__(self, conf_path)
        with open(conf_path, "r") as f:
            self.data = json.load(f)
        self.client = Elasticsearch(self.data["connection_url"])
        self.table_name = self.data["name"]
        self.path_prefix = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        logging.getLogger("elastic_transport").setLevel(logging.WARNING)

    def upload_batch(self, actions: List):
        helpers.bulk(self.client, actions)

    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        if self.client.indices.exists(index=self.table_name):
            self.client.indices.delete(index=self.table_name)
        self.client.indices.create(index=self.table_name, body=self.data["index"])
        batch_size = self.data["insert_batch_size"]
        dataset_path = os.path.join(self.path_prefix, self.data["data_path"])
        if not os.path.exists(dataset_path):
            self.download_data(self.data["data_link"], dataset_path)
        _, ext = os.path.splitext(dataset_path)
        if ext == ".json":
            with open(dataset_path, "r") as f:
                actions = []
                for i, line in enumerate(f):
                    if i % batch_size == 0 and i != 0:
                        self.upload_batch(actions)
                        actions = []
                    record = json.loads(line)
                    actions.append(
                        {
                            "_index": self.table_name,
                            "_id": uuid.UUID(int=i).hex,
                            "_source": record,
                        }
                    )
                if actions:
                    self.upload_batch(actions)
        elif ext == ".hdf5" and self.data["mode"] == "vector":
            with h5py.File(dataset_path, "r") as f:
                actions = []
                for i, line in enumerate(f["train"]):
                    if i % batch_size == 0 and i != 0:
                        self.upload_batch(actions)
                        actions = []
                    record = {self.data["vector_name"]: line}
                    actions.append(
                        {
                            "_index": self.table_name,
                            "_id": uuid.UUID(int=i).hex,
                            "_source": record,
                        }
                    )
                if actions:
                    self.upload_batch(actions)
        elif ext == ".csv":
            custom_headers = []
            headers = self.data["index"]["mappings"]["properties"]
            for key, value in headers.items():
                custom_headers.append(key)
            with open(
                dataset_path, "r", encoding="utf-8", errors="replace"
            ) as data_file:
                current_batch = []
                for i, line in enumerate(data_file):
                    row = line.strip().split("\t")
                    if len(row) != len(headers):
                        logging.info(
                            f"row = {i}, row_len = {len(row)}, not equal headers len, skip"
                        )
                        continue
                    row_dict = {header: value for header, value in zip(headers, row)}
                    current_batch.append(
                        {
                            "_index": self.table_name,
                            "_id": uuid.UUID(int=i).hex,
                            "_source": row_dict,
                        }
                    )
                    if len(current_batch) >= batch_size:
                        self.upload_batch(current_batch)
                        current_batch = []

                if current_batch:
                    self.upload_batch(current_batch)
        else:
            raise TypeError("Unsupported file type")

        self.client.indices.forcemerge(index=self.table_name, wait_for_completion=True)

    # https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-match-query.html
    def get_fulltext_query_content(self, query: str, is_and: bool = False) -> Any:
        ret = None
        if is_and:
            terms = query.split()
            ret = {
                "query": {
                    "bool": {"must": [{"match": {"body": term}} for term in terms]}
                }
            }
        else:
            is_phrase = False
            is_slop_phrase = False
            slop = 0
            query = query.lstrip()
            if query.startswith('"'):
                is_phrase = True
                match = re.search(r'~(\d+)$', query)
                if match:
                    is_slop_phrase = True
                    slop = int(match.group(1))
            if is_phrase:
                if is_slop_phrase:
                    ret = {"query": {"match_phrase": {"body": {"query": query, "slop": slop}}}}
                else:
                    ret = {"query": {"match_phrase": {"body": query}}}
            else:
                ret = {"query": {"match": {"body": query}}}
        return ret

    def setup_clients(self, num_threads=1):
        self.clients = list()
        for i in range(num_threads):
            client = Elasticsearch(self.data["connection_url"])
            self.clients.append(client)

    def do_single_query(self, query_id, client_id) -> list[Any]:
        query = self.queries[query_id]
        client = self.clients[client_id]
        if self.data["mode"] == "vector":
            knn = {
                "field": self.data["vector_name"],
                "query_vector": query,
                "k": self.data["topK"],
                "num_candidates": 200,
            }
            result = client.search(
                index=self.table_name,
                source=["_id", "_score"],
                knn=knn,
                size=self.data["topK"],
            )
            result = [
                (uuid.UUID(hex=hit["_id"]).int, hit["_score"])
                for hit in result["hits"]["hits"]
            ]
            return result
        elif self.data["mode"] == "fulltext":
            body = self.get_fulltext_query_content(query)
            result = self.client.search(
                index=self.table_name,
                body=body,
                size=self.data["topK"],
            )
            result = [
                (uuid.UUID(hex=hit["_id"]).int, hit["_score"])
                for hit in result["hits"]["hits"]
            ]
            return result
        else:
            raise TypeError("Unsupported data mode {}".format(self.data["mode"]))
