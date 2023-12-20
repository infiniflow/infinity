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

import infinity
from infinity.common import NetworkAddress, REMOTE_HOST


class TestDatabase:

    def test_version(self):
        print(infinity.__version__)

    def test_infinity_thrift(self):
        """
        target: test table apis
        method:
        1. create databases
            - 'my_database'         √
            - 'my_database!@#'      ❌
            - 'my-database-dash'    ❌
            - '123_database'        ❌
            - ''                    ❌
        2. list databases
            - 'my_database'
            - 'default'
        3. drop databases
            - 'my_database'
        4. list tables:
            - 'default'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(REMOTE_HOST)

        # infinity
        infinity_obj.drop_database("my_database", None)
        db = infinity_obj.create_database("my_database")
        assert db

        try:
            db = infinity_obj.create_database("my_database!@#")
        except Exception as e:
            print(e)

        try:
            db = infinity_obj.create_database("my-database-dash")
        except Exception as e:
            print(e)

        try:
            db = infinity_obj.create_database("123_database")
        except Exception as e:
            print(e)

        try:
            db = infinity_obj.create_database("")
        except Exception as e:
            print(e)

        res = infinity_obj.list_databases()
        assert res is not None

        res.db_names.sort()

        assert res.db_names[0] == 'default'
        assert res.db_names[1] == 'my_database'

        res = infinity_obj.drop_database("my_database")
        assert res.success

        # res = infinity_obj.drop_database("default")
        # assert not res.success

        res = infinity_obj.list_databases()
        assert res.success

        for db in res.db_names:
            assert db == 'default'

        # disconnect
        res = infinity_obj.disconnect()

        assert res.success
