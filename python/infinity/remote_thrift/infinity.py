from abc import ABC

from infinity import InfinityConnection
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteThriftDatabase


class RemoteThriftInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri):
        self.db_name = "default"
        self._client = ThriftInfinityClient(uri)

    def create_database(self, db_name: str, options=None):
        return self._client.create_database(db_name=db_name)

    def list_databases(self):
        return self._client.list_databases()

    def describe_database(self, db_name: str):
        return self._client.describe_database(db_name=db_name).result()

    def drop_database(self, db_name: str, options=None):
        return self._client.drop_database(db_name=db_name)

    def get_database(self, db_name: str):
        return RemoteThriftDatabase(self, name=db_name)

    def disconnect(self):
        return self._client.disconnect()

    @property
    def client(self):
        return self._client
