from qdrant_client import QdrantClient as QC
from qdrant_client import models
from qdrant_client.models import VectorParams, Distance

import json

from base_client import BaseClient

class QdrantClient(BaseClient):
    def __init__(self, mode: str, drop_old = True) -> None:
        data = json.load(mode)
        self.client = QC(data['connection_url'])
        self.collection_name = data['name']
        if data['distance'] == 'cosine':
            self.distance = Distance.COSINE
        elif data['distance'] == 'L2':
            self.distance = Distance.EUCLID
        hnsw_config = json.loads(data['vector_index'])
        self.client.recreate_collection(collection_name=self.collection_name,
                                        vectors_config=VectorParams(
                                            size=data['vector_size'],
                                            distance=self.distance,
                                            hnsw_config=models.HnswConfigDiff(m=data['vector_index'])))
    
