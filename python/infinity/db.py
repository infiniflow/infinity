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

from abc import ABC, abstractmethod

class Database(ABC):

    @abstractmethod
    def create_table(self, table_name, schema, options):
        pass  # implement create table logic here

    @abstractmethod
    def drop_table(self, table_name):
        pass  # implement drop table logic here

    @abstractmethod
    def list_tables(self):
        pass  # implement list tables logic here

    @abstractmethod
    def show_table(self, table_name):
        pass  # implement describe table logic here

    @abstractmethod
    def show_columns(self, table_name):
        pass  # implement describe table logic here

    @abstractmethod
    def get_table(self, table_name):
        pass  # implement get table logic here

    @abstractmethod
    def show_tables(self):
        pass
