from pathlib import Path
from typing import Union
from abc import ABC, abstractmethod

URI = Union[str, Path]


# abstract class
class InfinityConnection(ABC):
    def __init__(self, URI):
        self.uri = URI

    @abstractmethod
    def create_database(self, db_name, options=None):
        pass

    @abstractmethod
    def list_databases(self):
        pass

    @abstractmethod
    def describe_database(self, db_name):
        pass

    @abstractmethod
    def drop_database(self, db_name, options=None):
        pass

    @abstractmethod
    def get_database(self, db_name):
        pass
