from typing import Any
# from elasticsearch import Elasticsearch, helpers
import json
from typing import List
import os
import h5py
import uuid
import logging
import requests

from .base_client import BaseClient

class WrapQuickwitClient:
    def __init__(self, base_url):
        self.base_url = base_url

    def create_index(self, index_config):
        response = requests.post(
            f"{self.base_url}/api/v1/indexes",
            headers={'Content-Type': 'application/json'},
            data=json.dumps(index_config)
        )
        if response.status_code == 201:
            return response.json()
        else:
            response.raise_for_status()

    def index_exists(self, index):
        response = requests.get(f"{self.base_url}/api/v1/indexes/{index}")
        if response.status_code == 200:
            return True
        elif response.status_code == 404:
            return False
        else:
            response.raise_for_status()

    def delete_index(self, index):
        response = requests.delete(f"{self.base_url}/api/v1/indexes/{index}")
        if response.status_code == 200:
            return response.json()
        elif response.status_code == 404:
            return {"error": "Index not found"}
        else:
            response.raise_for_status()

    def upload_batch(self, index, data):
        bulk_url = f'{self.base_url}/api/v1/{index}/ingest'
        response = requests.post(
            # f"{self.base_url}/api/v1/_elastic/{index}/_bulk",
            # headers={'Content-Type': 'application/json'},
            bulk_url,
            headers={'Content-Type': 'application/json'},
            data=json.dumps(data)
        )
        if response.status_code == 200:
            return response.json()
        else:
            response.raise_for_status()

    def search(self, index, query):
        # search_url = f"{self.base_url}/api/v1/{index}/search"
        search_url = f"{self.base_url}/api/v1/_elastic/{index}/_search/"

        response = requests.get(
            search_url,
            headers={'Content-Type': 'application/json'},
            data=json.dumps(query)
        )
        if response.status_code == 200:
            return response.json()
        else:
            response.raise_for_status()

class QuickwitClient(BaseClient):
    def __init__(self, conf_path: str) -> None:
        """
        The mode configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        BaseClient.__init__(self, conf_path)
        with open(conf_path, "r") as f:
            self.data = json.load(f)
        self.client = WrapQuickwitClient(base_url=self.data["connection_url"])
        self.table_name = self.data["name"]
        self.path_prefix = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        logging.getLogger("quickwit_transport").setLevel(logging.WARNING)

    def upload_batch(self, actions: List):
        self.client.upload_batch(self.table_name, actions)

    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        if self.client.index_exists(index=self.table_name):
            self.client.delete_index(index=self.table_name)
        res = self.client.create_index(index_config=self.data["index"])
        print(f"res = {res}")
        print(f"after create index, exist={self.client.index_exists(index=self.table_name)}")
        print(f"quick wit created index {self.table_name}")
        # return
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
            print("quick wit csv upload")
            custom_headers = []
            headers = self.data["index"]["doc_mapping"]["field_mappings"]
            for key in headers:
                custom_headers.append(key["name"])
            with open(
                dataset_path, "r", encoding="utf-8", errors="replace"
            ) as data_file:
                current_batch = []
                bulk_request = ""
                cnt = 0
                for i, line in enumerate(data_file):
                    row = line.strip().split("\t")
                    if len(row) != len(headers):
                        logging.info(
                            f"row = {i}, row_len = {len(row)}, not equal headers len, skip"
                        )
                        continue
                    doc = {headers[j]["name"]: row[j] for j in range(len(headers))}
                    doc_id = doc.get("id", str(i))
                    # print(f"doc_id = {doc_id}")
                    # index_action = json.dumps({"index": {"_id": doc_id}})
                    create_action = json.dumps({"create": {"_index": self.table_name, "_id": doc_id}})

                    document = json.dumps(doc)
                    bulk_request += f"{create_action}\n{document}\n"
                    cnt += 1
                    if cnt >= batch_size:
                        self.upload_batch(bulk_request)
                        bulk_request = ""
                        cnt = 0

                if cnt != 0:
                    # print(f"quick wit upload batch, current batch = {bulk_request}")
                    print(f"index exist: {self.client.index_exists(index=self.table_name)}")
                    self.upload_batch(bulk_request)
                    cnt = 0
        else:
            raise TypeError("Unsupported file type")
        body = self.get_fulltext_query_content("every fictional")
        body["size"] = self.data["topK"]
        result = self.client.search(
            index=self.table_name,
            query=body,
        )
        print(result)
        print(result["hits"]["hits"])
        # self.client.indices.forcemerge(index=self.table_name, wait_for_completion=True)

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
            ret = {"query": {"match": {"body": query}}}
        return ret

    def setup_clients(self, num_threads=1):
        self.clients = list()
        for i in range(num_threads):
            client = WrapQuickwitClient(self.data["connection_url"])
            self.clients.append(client)

    def do_single_query(self, query_id, client_id) -> list[Any]:
        query = self.queries[query_id]
        client = self.clients[client_id]
        if self.data["mode"] == "fulltext":
            body = self.get_fulltext_query_content(query)
            body["size"] = self.data["topK"]

            result = client.search(
                index=self.table_name,
                query=body,
            )
            result = [
                (uuid.UUID(hex=hit["_id"]).int, hit["_score"])
                for hit in result["hits"]["hits"]
            ]
            return result
        else:
            raise TypeError("Unsupported data mode {}".format(self.data["mode"]))
