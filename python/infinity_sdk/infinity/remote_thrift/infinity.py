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
import logging

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity import InfinityConnection
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.utils import name_validity_check, select_res_to_polars
from infinity.common import ConflictType, InfinityException


class RemoteThriftInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri, logger: logging.Logger = None):
        super().__init__(uri)
        self.db_name = "default_db"
        self._client = ThriftInfinityClient(uri, logger=logger)
        self._is_connected = True

    def __del__(self):
        if self._is_connected is True:
            self.disconnect()

    @name_validity_check("db_name", "DB")
    def create_database(self, db_name: str, conflict_type: ConflictType = ConflictType.Error, comment: str = None):
        create_database_conflict: ttypes.CreateConflict
        if conflict_type == ConflictType.Error:
            create_database_conflict = ttypes.CreateConflict.Error
        elif conflict_type == ConflictType.Ignore:
            create_database_conflict = ttypes.CreateConflict.Ignore
        elif conflict_type == ConflictType.Replace:
            create_database_conflict = ttypes.CreateConflict.Replace
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "Invalid conflict type")

        res = self._client.create_database(db_name=db_name, conflict_type=create_database_conflict, comment=comment)
        if res.error_code == ErrorCode.OK:
            return RemoteDatabase(self._client, db_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def list_databases(self):
        res = self._client.list_databases()
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("db_name", "DB")
    def show_database(self, db_name: str):
        res = self._client.show_database(db_name=db_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("db_name", "DB")
    def drop_database(self, db_name: str, conflict_type: ConflictType = ConflictType.Error):
        drop_database_conflict: ttypes.DropConflict
        if conflict_type == ConflictType.Error:
            drop_database_conflict = ttypes.DropConflict.Error
        elif conflict_type == ConflictType.Ignore:
            drop_database_conflict = ttypes.DropConflict.Ignore
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "Invalid conflict type")

        res = self._client.drop_database(db_name=db_name, conflict_type=drop_database_conflict)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("db_name", "DB")
    def get_database(self, db_name: str):
        res = self._client.get_database(db_name)
        if res.error_code == ErrorCode.OK:
            return RemoteDatabase(self._client, db_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_current_node(self):
        res = self._client.show_current_node()
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def cleanup(self):
        res = self._client.cleanup()
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def optimize(self, db_name: str, table_name: str, optimize_opt: ttypes.OptimizeOptions):
        res = self._client.optimize(db_name, table_name, optimize_opt)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def test_command(self, command_content: str):
        command = ttypes.CommandRequest()
        command.command_type = "test_command"
        command.test_command_content = command_content
        self._client.command(command)

    def flush_data(self):
        flush_request = ttypes.FlushRequest()
        flush_request.flush_type = "data"
        self._client.flush(flush_request)

    def flush_catalog(self):
        flush_request = ttypes.FlushRequest()
        flush_request.flush_type = "catalog"
        self._client.flush(flush_request)

    def disconnect(self):
        res = self._client.disconnect()
        if res.error_code == ErrorCode.OK:
            self._is_connected = False
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def create_database_snapshot(self, snapshot_name: str, db_name: str):
        res = self._client.create_database_snapshot(snapshot_name=snapshot_name, db_name=db_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def restore_database_snapshot(self, snapshot_name: str):
        res = self._client.restore_snapshot(snapshot_name=snapshot_name, scope="database")
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def create_system_snapshot(self, snapshot_name: str):
        res = self._client.create_system_snapshot(snapshot_name=snapshot_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def restore_system_snapshot(self, snapshot_name: str):
        res = self._client.restore_snapshot(snapshot_name=snapshot_name, scope="system")
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def list_snapshots(self):
        res = self._client.list_snapshots()
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        
    def show_snapshot(self, snapshot_name: str):
        res = self._client.show_snapshot(snapshot_name=snapshot_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
    
    def drop_snapshot(self, snapshot_name: str):
        res = self._client.drop_snapshot(snapshot_name=snapshot_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)
        

    @property
    def client(self):
        return self._client
