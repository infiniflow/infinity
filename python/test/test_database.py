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
import common_values

class TestDatabase:

    def test_version(self):
        print(infinity.__version__)

    def test_database(self):
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

    # create db with invalid names
    def test_create_database_invalid_name(self):
        """
        1. connect server

        2. create ' ' name db

        3. disconnect server
        """

        # 1. connect
        infinity_obj = infinity.connect(REMOTE_HOST)

        # 2. create db with invalid name
        for db_name in common_values.invalid_name_array:
            try:
                # print('db name: ', db_name)
                db = infinity_obj.create_database(db_name)
            except Exception as e:
                print(e)

        # 3. disconnect
        res = infinity_obj.disconnect()

        assert res.success

    # create same db in different thread to test conflict and show dbs

    # create too many db (1M) to reach the limitation, then drop these dbs. show dbs

    # create / drop same db 1000 times and show dbs

    # create db with invalid option
    # drop db with invalid option
    # drop db with invalid name

    # create db with ignore-if-existence option
    # create db with ignore-if-existence option when conflict repeatedly
    # drop non-existent db
    # drop non-existent db with ignore if non-existent option repeatedly

    # create / drop db name in chinese

    # describe non-existent db
    # describe existent db
    # describe dropped db
    # describe chinese named db

    # after disconnection, create/drop/describe/list databases
    # get non-exist db
    # one thread get db, another thread drop this db
    #