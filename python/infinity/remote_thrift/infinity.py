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
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase


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
        res = self._client.create_database(db_name=db_name)
        if res.success is True:
            return RemoteDatabase(self._client, db_name)
        else:
            raise Exception(res.error_msg)

    def list_databases(self):
        return self._client.list_databases()

    def describe_database(self, db_name: str):
        return self._client.describe_database(db_name=db_name).result()

    def drop_database(self, db_name: str, options=None):
        return self._client.drop_database(db_name=db_name)

    def get_database(self, db_name: str):
        res = self._client.get_database(db_name)
        if res.success is True:
            return RemoteDatabase(self._client, db_name)
        else:
            raise Exception("Get db error")

    def disconnect(self):
        res = self._client.disconnect()
        if res.success is True:
            self._is_connected = False
        else:
            raise Exception("Disconnect error")
        return res

    @property
    def client(self):
        return self._client
