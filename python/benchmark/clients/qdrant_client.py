from qdrant_client import QdrantClient as QC
from qdrant_client import models
from qdrant_client.models import VectorParams, Distance
import os
import time
import json
import h5py
from typing import Any
import logging

from .base_client import BaseClient
from .utils import  csr_read_all

class QdrantClient(BaseClient):
    def __init__(self, conf_path: str) -> None:
        BaseClient.__init__(self, conf_path)
        with open(conf_path, "r") as f:
            self.data = json.load(f)
        self.client = QC(self.data["connection_url"])
        self.table_name = self.data["name"]
        if "distance" in self.data:
            if self.data["distance"] == "cosine":
                self.distance = Distance.COSINE
            elif self.data["distance"] == "L2":
                self.distance = Distance.EUCLID

        self.path_prefix = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        logging.getLogger("httpx").setLevel(logging.WARNING)

    def upload_bach(self, ids: list[int], vectors, payloads=None):
        self.client.upsert(
            collection_name=self.table_name,
            points=models.Batch(ids=ids, vectors=vectors, payloads=payloads),
            wait=True,
        )

    def upload(self):
        # get the dataset (downloading is completed in run.py)
        if "vector_index" in self.data:
            index_config = self.data["vector_index"]
            if index_config["type"] == "HNSW":
                hnsw_params = index_config["index_params"]
                hnsw_config = models.HnswConfigDiff(
                    m=hnsw_params.get("M", None),
                    ef_construct=hnsw_params.get("ef_construct", None),
                    full_scan_threshold=hnsw_params.get("full_scan_threshold", None),
                    max_indexing_threads=hnsw_params.get("max_indexing_threads", None),
                    on_disk=hnsw_params.get("on_disk", None),
                    payload_m=hnsw_params.get("payload_m", None),
                )
        else:
            hnsw_config = None

        if self.data["mode"] == "sparse_vector":
            self.client.recreate_collection(
                collection_name=self.table_name,
                vectors_config={},
                sparse_vectors_config={
                    "text": models.SparseVectorParams(),
                },
            )
        else:
            self.client.recreate_collection(
                collection_name=self.table_name,
                vectors_config=VectorParams(
                    size=self.data["vector_size"], distance=self.distance
                ),
                hnsw_config=hnsw_config,
            )

        # set payload index
        if "payload_index_schema" in self.data:
            for field_name, field_schema in self.data["payload_index_schema"].items():
                self.client.create_payload_index(
                    collection_name=self.table_name,
                    field_name=field_name,
                    field_schema=field_schema,
                )

        # set full text index
        if "full_text_index_schema" in self.data:
            for field_name, schema in self.data["full_text_index_schema"].items():
                field_schema = models.TextIndexParams(
                    type="text",
                    tokenizer=schema.get("tokenizer", None),
                    min_token_len=schema.get("min_token_len", None),
                    max_token_len=schema.get("max_token_len", None),
                    lowercase=schema.get("lowercase", None),
                )
                self.client.create_payload_index(
                    collection_name=self.table_name,
                    field_name=field_name,
                    field_schema=field_schema,
                )
        dataset_path = os.path.join(self.path_prefix, self.data["data_path"])
        if not os.path.exists(dataset_path):
            self.download_data(self.data["data_link"], dataset_path)
        vector_name = self.data["vector_name"]
        batch_size = self.data["insert_batch_size"]
        total_time = 0
        _, ext = os.path.splitext(dataset_path)
        if ext == ".json":
            with open(dataset_path, "r") as f:
                vectors = []
                payloads = []
                for i, line in enumerate(f):
                    if i % batch_size == 0 and i != 0:
                        start_time = time.time()
                        self.upload_bach(
                            list(range(i - batch_size, i)), vectors, payloads
                        )
                        end_time = time.time()
                        total_time += end_time - start_time
                        # reset vectors and payloads for the next batch
                        vectors = []
                        payloads = []
                    record = json.loads(line)
                    vectors.append(record[vector_name])
                    del record[vector_name]
                    payloads.append(record)
                if vectors:
                    start_time = time.time()
                    self.upload_bach(
                        list(range(i - len(vectors) + 1, i + 1)), vectors, payloads
                    )
                    end_time = time.time()
                    total_time += end_time - start_time
        elif ext == ".hdf5":
            with h5py.File(dataset_path) as f:
                vectors = []
                for i, line in enumerate(f["train"]):
                    if i % batch_size == 0 and i != 0:
                        self.upload_bach(list(range(i - batch_size, i)), vectors)
                        vectors = []
                    vectors.append(line)
                if vectors:
                    self.upload_bach(list(range(i - len(vectors) + 1, i + 1)), vectors)
        elif ext == ".csr":
            data_mat = csr_read_all(dataset_path)
            points = []
            for data_id in range(data_mat.nrow):
                indices, values = data_mat.at(data_id)
                if data_id % batch_size == 0 and data_id != 0:
                    self.client.upsert(
                        collection_name=self.table_name,
                        points=points,
                    )
                    points.clear()
                    print(data_id)
                else:
                    points.append(models.PointStruct(
                        id=data_id,
                        payload={},  # Add any additional payload if necessary
                        vector={
                            "text": models.SparseVector(
                                indices=indices,
                                values=values
                            )
                        },
                    )
                )
            if points:
                self.client.upsert(
                    collection_name=self.table_name,
                    points=points,
                )
        else:
            raise TypeError("Unsupported file type")

    def setup_clients(self, num_threads=1):
        self.clients = list()
        for i in range(num_threads):
            client = QC(self.data["connection_url"])
            self.clients.append(client)

    def do_single_query(self, query_id, client_id) -> list[Any]:
        query = self.queries[query_id]
        client = self.clients[client_id]
        if self.data["mode"] == "vector":
            res = client.search(
                collection_name=self.table_name,
                query_vector=query,
                limit=self.data.get("topK", 10),
                with_payload=False,
            )
        elif self.data["mode"] == "sparse_vector":
            indices, values = self.queries[query_id]
            res = client.search(
                collection_name=self.table_name,
                query_vector=models.NamedSparseVector(
                    name="text",
                    vector=models.SparseVector(
                        indices=indices,
                        values=values
                    ),
                ),
                limit=self.data.get("topK", 10),
            )
        else:
            raise TypeError("Unsupported type")
        return res
