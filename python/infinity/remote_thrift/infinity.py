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

from infinity import InfinityConnection
from infinity.infinity import ShowVariable
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.utils import check_valid_name, select_res_to_polars


class RemoteThriftInfinityConnection(InfinityConnection, ABC):
    def __init__(self, uri):
        super().__init__(uri)
        self.db_name = "default"
        self._client = ThriftInfinityClient(uri)
        self._is_connected = True

    def __del__(self):
        if self._is_connected is True:
            self.disconnect()

    def create_database(self, db_name: str, options=None):
        check_valid_name(db_name)
        res = self._client.create_database(db_name=db_name)
        if res.success is True:
            return RemoteDatabase(self._client, db_name)
        else:
            raise Exception(res.error_msg)

    def list_databases(self):
        res = self._client.list_databases()
        if res.success is True:
            return res
        else:
            raise Exception(res.error_msg)

    def describe_database(self, db_name: str):
        check_valid_name(db_name)
        res = self._client.describe_database(db_name=db_name)
        if res.success is True:
            return res
        else:
            raise Exception(res.error_msg)

    def drop_database(self, db_name: str, options=None):
        check_valid_name(db_name)
        res = self._client.drop_database(db_name=db_name)
        if res.success is True:
            return res
        else:
            raise Exception(res.error_msg)

    def get_database(self, db_name: str):
        check_valid_name(db_name)
        res = self._client.get_database(db_name)
        if res.success is True:
            return RemoteDatabase(self._client, db_name)
        else:
            raise Exception("Get db error")

    def disconnect(self):
        res = self._client.disconnect()
        if res.success is True:
            self._is_connected = False
            return res
        else:
            raise Exception("Disconnect error")

    @property
    def client(self):
        return self._client

    def show_variable(self, variable: ShowVariable):
        res = self._client.show_variable(variable)
        if res.success is True:
            return select_res_to_polars(res)
        else:
            raise Exception(res.error_msg)
