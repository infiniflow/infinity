import json
import os
import h5py
from typing import Any
import logging

import infinity
import infinity.index as index
from infinity import NetworkAddress
from .base_client import BaseClient


class InfinityClient(BaseClient):
    def __init__(self, conf_path: str) -> None:
        """
        The conf_path configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        BaseClient.__init__(self, conf_path)
        with open(conf_path, "r") as f:
            self.data = json.load(f)
        host, port = self.data["host"].split(":")
        self.client = infinity.connect(NetworkAddress(host, int(port)))
        self.table_name = self.data["name"]
        self.data_mode = self.data["mode"]
        self.path_prefix = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        self.table_objs = list()

    def _parse_index_schema(self, index_schema):
        indexs = []
        for key, value in index_schema.items():
            if value["type"] == "text":
                indexs.append(index.IndexInfo(key, index.IndexType.FullText, []))
            elif value["type"] == "HNSW":
                params = []
                for param, v in value["params"].items():
                    params.append(index.InitParameter(param, str(v)))
                indexs.append(index.IndexInfo(key, index.IndexType.Hnsw, params))
        return indexs

    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        db_obj = self.client.get_database("default_db")
        db_obj.drop_table(self.table_name)
        db_obj.create_table(self.table_name, self.data["schema"])
        table_obj = db_obj.get_table(self.table_name)
        dataset_path = os.path.join(self.path_prefix, self.data["data_path"])
        if not os.path.exists(dataset_path):
            self.download_data(self.data["data_link"], dataset_path)
        batch_size = self.data["insert_batch_size"]
        features = list(self.data["schema"].keys())
        _, ext = os.path.splitext(dataset_path)
        if ext == ".json":
            with open(dataset_path, "r") as f:
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
        elif ext == ".hdf5":
            with h5py.File(dataset_path, "r") as f:
                actions = []
                # line is vector
                for i, line in enumerate(f["train"]):
                    if i % batch_size == 0 and i != 0:
                        table_obj.insert(actions)
                        actions = []
                    record = {self.data["vector_name"]: line.tolist()}
                    actions.append(record)
                if actions:
                    table_obj.insert(actions)
        elif ext == ".csv":
            if self.data["use_import"]:
                table_obj.import_data(dataset_path, import_options={"delimiter": "\t"})
            else:
                custom_headers = []
                headers = self.data["schema"]
                for key, value in headers.items():
                    custom_headers.append(key)

                with open(
                    dataset_path, "r", encoding="utf-8", errors="replace"
                ) as data_file:
                    current_batch = []
                    for i, line in enumerate(data_file):
                        row = line.strip().split("\t")
                        if i % 100000 == 0:
                            logging.info(f"row {i}")
                        if len(row) != len(headers):
                            logging.info(
                                f"row = {i}, row_len = {len(row)}, not equal headers len, skip"
                            )
                            continue
                        row_dict = {
                            header: value for header, value in zip(headers, row)
                        }
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

    def setup_clients(self, num_threads=1):
        host, port = self.data["host"].split(":")
        self.clients = list()
        for i in range(num_threads):
            client = infinity.connect(NetworkAddress(host, int(port)))
            db_obj = client.get_database("default_db")
            table_obj = db_obj.get_table(self.table_name)
            self.clients.append(client)
            self.table_objs.append(table_obj)

    def do_single_query(self, query_id, client_id) -> list[Any]:
        result = None
        query = self.queries[query_id]
        table_obj = self.table_objs[client_id]
        if self.data_mode == "vector":
            res, _ = (
                table_obj.output(["_row_id"])
                .knn(
                    self.data["vector_name"],
                    query,
                    "float",
                    self.data["metric_type"],
                    self.data["topK"],
                )
                .to_result()
            )
            result = res["ROW_ID"]
        elif self.data_mode == "fulltext":
            res, _ = (
                table_obj.output(["_row_id", "_score"])
                .match(
                    "",
                    query,
                    f"topn={self.data['topK']};default_field=body",
                )
                .to_result()
            )
            result = list(zip(res["ROW_ID"], res["SCORE"]))
        return result
