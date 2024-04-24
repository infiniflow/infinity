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
import polars as pl

from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode
import infinity
from test_sdkbase import TestSdk


class TestDescribe(TestSdk):

    def test_show_table(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table", ConflictType.Ignore)
        db_obj.create_table(
            "test_show_table", {"num": "integer", "body": "varchar", "vec": "vector,5,float"}, ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = db_obj.show_table("test_show_table")
            print(res)

        res = db_obj.drop_table("test_show_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_show_columns(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_columns", ConflictType.Ignore)
        db_obj.create_table(
            "test_show_columns", {"num": "integer", "body": "varchar", "vec": "vector,5,float"}, ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = db_obj.show_columns("test_show_columns")
            print(res)
            # check the polars dataframe
            assert res.columns == ["column_name", "column_type", "constraint"]

        res = db_obj.drop_table("test_show_columns", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_show_big_databases(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)

        for i in range(8193):
            infinity_obj.drop_database(f"test_show_big_databases_{i}", ConflictType.Ignore)

        for i in range(8193):
            infinity_obj.create_database(f"test_show_big_databases_{i}", ConflictType.Ignore)

        res = infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        for i in range(8193):
            infinity_obj.drop_database(f"test_show_big_databases_{i}", ConflictType.Ignore)
