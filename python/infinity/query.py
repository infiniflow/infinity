from __future__ import annotations
from abc import ABC, abstractmethod
from typing import List, Literal, Optional, Type, Union
import pydantic

class Query(pydantic.BaseModel):
    filter: Optional[str] = None
    columns: Optional[List[str]] = None
    limit: Optional[int] = None
    offset: Optional[int] = None


class InfinityQueryBuilder(ABC):


    @classmethod
    def create(
            cls,
            table,
            query: Optional[Union[str]],
            vector_column_name: str,
    ) -> InfinityQueryBuilder:
        return InfinityVectorQueryBuilder(table, query, vector_column_name)

    def __init__(self, table):
        self._table = table
        self._limit = None
        self._columns = None
        self._filter = None
        self._offset = None

    def limit(self, limit: int) -> InfinityQueryBuilder:
        self._limit = limit
        return self

    def offset(self, offset: int) -> InfinityQueryBuilder:
        self._offset = offset
        return self

    def output(self, columns: list) -> InfinityQueryBuilder:
        self._columns = columns
        return self

    def filter(self, where: str) -> InfinityQueryBuilder:
        self._filter = where
        return self

    @property
    def columns(self):
        return self._columns


class InfinityVectorQueryBuilder(InfinityQueryBuilder):
    def __init__(
            self,
            table,
            query: Optional[Union[str]],
            vector_column: str,
    ):
        super().__init__(table)
        self._query = query
        self._vector_column = vector_column

    def filter(self, where: str) -> InfinityVectorQueryBuilder:
        self._filter = where
        return self

    def limit(self, limit: int) -> InfinityVectorQueryBuilder:
        self._limit = limit
        return self

    def offset(self, offset: int) -> InfinityVectorQueryBuilder:
        self._offset = offset
        return self

    def to_list(self) -> List[List]:
        query = Query(
            filter=self._filter,
            columns=self._columns,
            limit=self._limit,
            offset=self._offset
        )
        return self._table._execute_query(query)
