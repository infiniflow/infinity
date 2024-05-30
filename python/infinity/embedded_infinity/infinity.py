from embedded_infinity import Infinity
from infinity import InfinityConnection
from abc import ABC

class EmbeddedInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri, path="/var/infinity"):
        Infinity.LocalInit(path)

        self.db_name = "default_db"
        self._client = Infinity.LocalConnect()
        self._is_connected = True

    def __del__(self):
        if self._is_connected is True:
            self.disconnect()

    def create_database(self, db_name, options=None):
        res = self._client.CreateDatabase(db_name, options)
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def list_databases(self):
        res = self._client.ListDatabases()
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def show_database(self, db_name):
        res = self._client.ShowDatabase(db_name)
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def drop_database(self, db_name, options=None):
        res = self._client.DropDatabase(db_name, options)
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def get_database(self, db_name):
        return self._client.GetDatabase(db_name)

    def disconnect(self):
        Infinity.LocalUnInit()
        self._is_connected = False