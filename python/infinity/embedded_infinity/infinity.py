from infinity import InfinityConnection
from abc import ABC
from infinity.common import ConflictType, EMBEDDED_INFINITY_PATH
from infinity.embedded_infinity.client import EmbeddedInfinityClient
from embedded_infinity import ConflictType as EmbeddedConflictType
class EmbeddedInfinityConnection(InfinityConnection, ABC):
    def __init__(self, path=EMBEDDED_INFINITY_PATH):
        self.db_name = "default_db"
        self._client = EmbeddedInfinityClient(path)
        self._is_connected = True

    def __del__(self):
        if self._is_connected is True:
            self.disconnect()

    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.Error):
        create_database_conflict: EmbeddedConflictType
        if conflict_type == ConflictType.Error:
            create_database_conflict = EmbeddedConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            create_database_conflict = EmbeddedConflictType.kIgnore
        elif conflict_type == ConflictType.Replace:
            create_database_conflict = EmbeddedConflictType.kReplace
        else:
            raise Exception(f"ERROR:3066, Invalid conflict type")
        self._client.create_database(db_name, create_database_conflict)
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def list_databases(self):
        res = self._client.list_databases()
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def show_database(self, db_name):
        res = self._client.show_database(db_name)
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def drop_database(self, db_name, options=None):
        res = self._client.drop_database(db_name, options)
        # if res.error_code == 0:
        #     return res
        # else:
        #     raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def get_database(self, db_name):
        return self._client.get_database(db_name)

    def disconnect(self):
        self._is_connected = False
        self._client.disconnect()