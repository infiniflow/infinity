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
from enum import Enum

from infinity import URI

# abstract class
class InfinityConnection(ABC):
    def __init__(self, uri):
        self.uri = uri

    @abstractmethod
    def create_database(self, db_name, options=None):
        pass

    @abstractmethod
    def list_databases(self):
        pass

    @abstractmethod
    def show_database(self, db_name):
        pass

    @abstractmethod
    def drop_database(self, db_name, options=None):
        pass

    @abstractmethod
    def get_database(self, db_name):
        pass

    @abstractmethod
    def disconnect(self):
        pass
