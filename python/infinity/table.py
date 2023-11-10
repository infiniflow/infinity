from abc import ABC, abstractmethod
import typing as tp
from typing import Literal, Optional, Type, Union



class Table(ABC):

    @abstractmethod
    def create_index(self, index_name: str, column_names: list[str], method_type: str, index_para_list: list[dict],
                     options=None):
        pass

    @abstractmethod
    def drop_index(self, index_name: str):
        pass

    @abstractmethod
    def insert(self, data: list[dict[str, tp.Union[str, int, float]]]):
        pass

    @abstractmethod
    def import_data(self, file_path: str, options=None):
        pass

    @abstractmethod
    def delete(self, condition):
        pass

    @abstractmethod
    def update(self, condition, data):
        pass

    from python.infinity.query import Query, InfinityQueryBuilder

    @abstractmethod
    def search(
            self,
            query: Optional[Union[str]] = None,
    ) -> InfinityQueryBuilder:
        raise NotImplementedError

    @abstractmethod
    def _execute_query(self, query: Query) -> list[dict]:
        pass
