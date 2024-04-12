import argparse

from base_client import BaseClient

import argparse

from base_client import BaseClient

from qdrant_client import QdrantClient as QC
from qdrant_client import models
from qdrant_client.models import VectorParams, Distance
import os
import time
import json
from typing import Any, List, Optional

from base_client import BaseClient, FieldValue

class QdrantClient(BaseClient):
    def __init__(self,
                 mode: str,
                 options: argparse.Namespace,
                 drop_old: bool = True) -> None:
        with open(mode, 'r') as f:
            self.data = json.load(f)
        self.client = QC(self.data['connection_url'])
        self.collection_name = self.data['name']
        if self.data['distance'] == 'cosine':
            self.distance = Distance.COSINE
        elif self.data['distance'] == 'L2':
            self.distance = Distance.EUCLID
        if 'vector_index' in self.data:
            index_config = self.data['vector_index']
            if index_config['type'] == "HNSW":
                hnsw_params = index_config['index_params']
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
        
        self.client.recreate_collection(collection_name=self.collection_name,
                                        vectors_config=VectorParams(
                                            size=self.data['vector_size'],
                                            distance=self.distance),
                                            hnsw_config=hnsw_config)

        # set payload index
        if 'payload_index_schema' in self.data:
            for field_name, field_schema in self.data['payload_index_schema'].items():
                self.client.create_payload_index(collection_name=self.collection_name,
                                                field_name=field_name,
                                                field_schema=field_schema)
        
        # set full text index
        if 'full_text_index_schema' in self.data:
            for field_name, schema in self.data['full_text_index_schema'].items():
                field_schema = models.TextIndexParams(
                                        type="text",
                                        tokenizer=schema.get("tokenizer", None),
                                        min_token_len=schema.get("min_token_len", None),
                                        max_token_len=schema.get("max_token_len", None),
                                        lowercase=schema.get("lowercase", None),
                                    )
                self.client.create_payload_index(collection_name=self.collection_name,
                                                field_name=field_name,
                                                field_schema=field_schema)
    
    def upload_bach(self, ids: list[int], vectors, payloads):
        self.client.upsert(
            collection_name=self.collection_name,
            points=models.Batch(
                ids=ids,
                vectors=vectors,
                payloads=payloads
            ),
            wait=True
        )

    def upload(self):
        # get the dataset (downloading is completed in run.py)
        dataset_path = "/home/infinity/yj/codes/infinity/python/benchmark/datasets/dbpedia/dbpedia.json"
        vector_name = self.data['vector_name']
        batch_size=self.data['insert_batch_size']
        total_time = 0
        with open(dataset_path, 'r') as f:
            _, ext = os.path.splitext(dataset_path)
            if ext == '.json':
                vectors = []
                payloads = []
                for i, line in enumerate(f):
                    if i > 50000:
                        break
                    if i % batch_size == 0 and i > 0:
                        start_time = time.time()
                        self.upload_bach(list(range(i-batch_size, i)), vectors, payloads)
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
                    self.upload_bach(list(range(i-len(vectors)+1, i+1)), vectors, payloads)
                    end_time = time.time()
                    total_time += end_time - start_time
                    vectors = []
                    payloads = []
            
            print(f"Total time for uploading: {total_time:.2f} seconds")

    # build filter condition
    def build_condition(
        self, and_subfilters: Optional[List[Any]], or_subfilters: Optional[List[Any]]
    ) -> Optional[Any]:
        return models.Filter(
            should=or_subfilters,
            must=and_subfilters,
        )

    def build_value_match_filter(self, field_name: str, value: FieldValue) -> Any:
        return models.FieldCondition(
            key=field_name,
            match=models.MatchValue(value=value),
        )
    
    def build_text_match_filter(self, field_name: str, text: str) -> Any:
        return models.FieldCondition(
            key=field_name,
            match=models.MatchText(text=text),
        )

    def build_range_filter(
        self,
        field_name: str,
        lt: Optional[FieldValue],
        gt: Optional[FieldValue],
        lte: Optional[FieldValue],
        gte: Optional[FieldValue],
    ) -> Any:
        return models.FieldCondition(
            key=field_name,
            range=models.Range(
                lt=lt,
                gt=gt,
                gte=gte,
                lte=lte,
            ),
        )

    def search(self) -> list[list[Any]]:
        # get the queries path
        query_path = "/home/infinity/yj/codes/infinity/python/benchmark/datasets/dbpedia/hybrid_queries.json"
        results = []
        with open(query_path, 'r') as f:
            for line in f.readlines():
                query = json.loads(line)
                start = time.time()
                result = self.client.search(
                    collection_name=self.collection_name,
                    query_vector=query['vector'],
                    limit=self.data.get('topK', 10),
                    query_filter=self.parse(
                        self.data.get('query_filter', None)
                    ),
                    with_payload=False
                )
                end = time.time()
                print(f"latency of search with filter: {(end - start)*1000:.2f} milliseconds")
                print(result)
                results.append(result)
        return results
