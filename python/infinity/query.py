# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import annotations

from abc import ABC
from typing import List, Optional, Union

import numpy as np
import pandas as pd
import pydantic

from infinity.common import VEC


class Query(pydantic.BaseModel):
    embedding: Optional[list]
    vector_column_name: Optional[str] = "vector"
    distance: Optional[str] = "L2"
    threshold: Optional[float] = None
    columns: Optional[List[str]] = None
    filter: Optional[str] = None
    limit: Optional[int] = None
    offset: Optional[int] = None


class InfinityQueryBuilder(ABC):

    @classmethod
    def create(
            cls,
            table,
            embedding: Optional[VEC],
            vector_column_name: str,
            distance: str = "L2",
            threshold: Optional[float] = None,
    ) -> InfinityQueryBuilder:
        if isinstance(embedding, list):
            embedding = embedding
        if isinstance(embedding, np.ndarray):
            embedding = embedding.tolist()
        return InfinityVectorQueryBuilder(table, embedding, vector_column_name, distance, threshold)

    def __init__(self, table):
        self._table = table
        self._limit = None
        self._columns = None
        self._filter = None
        self._offset = None

    def limit(self, limit: Optional[int]) -> InfinityQueryBuilder:
        self._limit = limit
        return self

    def offset(self, offset: Optional[int]) -> InfinityQueryBuilder:
        self._offset = offset
        return self

    def output(self, columns: Optional[list]) -> InfinityQueryBuilder:
        self._columns = columns
        return self

    def filter(self, where: Optional[str]) -> InfinityQueryBuilder:
        self._filter = where
        return self


class InfinityVectorQueryBuilder(InfinityQueryBuilder):
    def __init__(
            self,
            table,
            embedding: Optional[VEC],
            vector_column_name: str,
            distance: str = "L2",
            threshold: Optional[float] = None,

    ):
        super().__init__(table)
        self._embedding = embedding
        self._vector_column = vector_column_name
        self._distance = distance
        self._threshold = threshold

    def filter(self, where: Optional[str]) -> InfinityVectorQueryBuilder:
        self._filter = where
        return self

    def limit(self, limit: Optional[int]) -> InfinityVectorQueryBuilder:
        self._limit = limit
        return self

    def offset(self, offset: Optional[int]) -> InfinityVectorQueryBuilder:
        self._offset = offset
        return self

    def output(self, columns: Optional[list]) -> InfinityVectorQueryBuilder:
        self._columns = columns
        return self

    def to_df(self) -> pd.DataFrame:
        query = Query(
            embedding=self._embedding,
            vector_column_name=self._vector_column,
            distance=self._distance,
            threshold=self._threshold,

            filter=self._filter,
            columns=self._columns,
            limit=self._limit,
            offset=self._offset
        )
        return self._table._execute_query(query)
