from abc import ABC

from python.infinity import InfinityConnection
from python.infinity.remote.client import GrpcInfinityClient



class RemoteInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri):
        self.db_name = "default"
        self._client = GrpcInfinityClient(uri)

    def create_database(self, db_name: str, options=None):
        # new database will be created if not exist
        # show error if database already exist
        res = self._client.create_database(db_name=db_name)
        return res

    def list_databases(self):
        return self._client.list_databases()

    def describe_database(self, db_name: str):
        return self._client.describe_database(db_name=db_name).result()

    def drop_database(self, db_name: str, options=None):
        return self._client.drop_database(db_name=db_name)

    def get_database(self, db_name: str):
        from python.infinity.remote.db import RemoteDatabase
        return RemoteDatabase(self, name=db_name)

    def disconnect(self):
        return self._client.disconnect()

    @property
    def client(self):
        return self._client
