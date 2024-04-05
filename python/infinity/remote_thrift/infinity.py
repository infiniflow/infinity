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

from abc import ABC

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity import InfinityConnection
from infinity.errors import ErrorCode
from infinity.infinity import ShowVariable
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.utils import name_validity_check, select_res_to_polars
from infinity.common import ConflictType


class RemoteThriftInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri):
        super().__init__(uri)
        self.db_name = "default"
        self._client = ThriftInfinityClient(uri)
        self._is_connected = True

    def __del__(self):
        if self._is_connected is True:
            self.disconnect()

    @name_validity_check("db_name", "DB")
    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.Error):
        create_database_conflict: ttypes.CreateConflict
        if conflict_type == ConflictType.Error:
            create_database_conflict = ttypes.CreateConflict.Error
        elif conflict_type == ConflictType.Ignore:
            create_database_conflict = ttypes.CreateConflict.Ignore
        elif conflict_type == ConflictType.Replace:
            create_database_conflict = ttypes.CreateConflict.Replace
        else:
            raise Exception(f"ERROR:3066, Invalid conflict type")

        res = self._client.create_database(db_name=db_name, conflict_type=create_database_conflict)
        if res.error_code == ErrorCode.OK:
            return RemoteDatabase(self._client, db_name)
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def list_databases(self):
        res = self._client.list_databases()
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    @name_validity_check("db_name", "DB")
    def show_database(self, db_name: str):
        res = self._client.show_database(db_name=db_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    @name_validity_check("db_name", "DB")
    def drop_database(self, db_name: str, conflict_type: ConflictType = ConflictType.Error):
        drop_database_conflict: ttypes.DropConflict
        if conflict_type == ConflictType.Error:
            drop_database_conflict = ttypes.DropConflict.Error
        elif conflict_type == ConflictType.Ignore:
            drop_database_conflict = ttypes.DropConflict.Ignore
        else:
            raise Exception(f"ERROR:3066, invalid conflict type")

        res = self._client.drop_database(db_name=db_name, conflict_type = drop_database_conflict)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    @name_validity_check("db_name", "DB")
    def get_database(self, db_name: str):
        res = self._client.get_database(db_name)
        if res.error_code == ErrorCode.OK:
            return RemoteDatabase(self._client, db_name)
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def disconnect(self):
        res = self._client.disconnect()
        if res.error_code == ErrorCode.OK:
            self._is_connected = False
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    @property
    def client(self):
        return self._client

    def show_variable(self, variable: ShowVariable):
        res = self._client.show_variable(variable)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")
