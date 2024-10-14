import os
from infinity_embedded import InfinityConnection
from abc import ABC
from infinity_embedded.common import ConflictType, LOCAL_INFINITY_PATH, InfinityException
from infinity_embedded.local_infinity.client import LocalInfinityClient
from infinity_embedded.embedded_infinity_ext import ConflictType as LocalConflictType
from infinity_embedded.errors import ErrorCode
from infinity_embedded.local_infinity.db import LocalDatabase
from infinity_embedded.local_infinity.utils import name_validity_check
import logging


class LocalInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri=LOCAL_INFINITY_PATH):
        if not os.path.exists(uri):
            try:
                logging.warning(f"Directory {uri} not found, try to create it")
                os.makedirs(uri)
            except OSError as e:
                raise InfinityException(ErrorCode.DIR_NOT_FOUND, f"Directory {uri} not found and create failed: {e}")
        if os.path.isdir(uri):
            if os.access(uri, os.R_OK | os.W_OK):
                self._client = LocalInfinityClient(uri)
                self._is_connected = True
            else:
                raise InfinityException(ErrorCode.UNEXPECTED_ERROR,
                                        f"Directory {uri} does not have read or write permissions")
        else:
            raise InfinityException(ErrorCode.DIR_NOT_FOUND, f"Directory {uri} not found")

        self.db_name = "default_db"

    def __del__(self):
        if self._is_connected is True:
            self.disconnect()

    def check_connect(self):
        if (self._is_connected is False):
            raise Exception("Local infinity is not connected")

    @name_validity_check("db_name", "DB")
    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.Error):
        self.check_connect()
        create_database_conflict: LocalConflictType
        if conflict_type == ConflictType.Error:
            create_database_conflict = LocalConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            create_database_conflict = LocalConflictType.kIgnore
        elif conflict_type == ConflictType.Replace:
            create_database_conflict = LocalConflictType.kReplace
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "Invalid conflict type")
        res = self._client.create_database(db_name, create_database_conflict)

        if res.error_code == ErrorCode.OK:
            return LocalDatabase(self._client, db_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def list_databases(self):
        self.check_connect()
        res = self._client.list_databases()
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("db_name", "DB")
    def show_database(self, db_name):
        self.check_connect()
        res = self._client.show_database(db_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_info(self, info_name):
        self.check_connect()
        res = self._client.show_info(info_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def search(self, db_name, table_name):
        self.check_connect()
        res = self._client.search(db_name, table_name, [])
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("db_name", "DB")
    def drop_database(self, db_name, conflict_type: ConflictType = ConflictType.Error):
        self.check_connect()
        drop_database_conflict: LocalConflictType
        if conflict_type == ConflictType.Error:
            drop_database_conflict = LocalConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            drop_database_conflict = LocalConflictType.kIgnore
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "Invalid conflict type")

        res = self._client.drop_database(db_name, drop_database_conflict)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("db_name", "DB")
    def get_database(self, db_name):
        self.check_connect()
        res = self._client.get_database(db_name)
        if res.error_code == ErrorCode.OK:
            return LocalDatabase(self._client, db_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def disconnect(self):
        res = self._client.disconnect()
        if res.error_code == ErrorCode.OK:
            self._is_connected = False
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def client(self):
        return self._client
