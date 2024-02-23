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

import pytest

import common_values
import infinity

from infinity.errors import ErrorCode

class TestCreateTable:

    def test_create_varchar_table(self):
        """
        target: test create table with varchar column
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_varchar_table", True)
        table_obj = db_obj.create_table("test_create_varchar_table", {
            "c1": "varchar, primary key", "c2": "float"}, None)
        assert table_obj

        db_obj.drop_table("test_create_varchar_table")

    def test_create_embedding_table(self):
        """
        target: test create table with embedding column
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_create_embedding_table", True)
        table_obj = db_obj.create_table("test_create_embedding_table", {
            "c1": "vector,128,float"}, None)
        assert table_obj

        db_obj.drop_table("test_create_embedding_table")

    def test_create_table_with_invalid_column_name(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                " ": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "12": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "[]": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "()": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "{}": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "1": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

        with pytest.raises(Exception):
            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("test_create_invalid_column_name", True)
            table_obj = db_obj.create_table("test_create_invalid_column_name", {
                "1.1": "vector,128,float"}, None)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name")

    @pytest.mark.parametrize("column_name", common_values.invalid_name_array)
    def test_create_table_with_invalid_column_name_python(self, column_name):
        """
        target: create with invalid column name
        methods: create table with invalid column name
        expect: all operations throw exception on python side
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("my_table")

        try:
            tb = db_obj.create_table("my_table", {column_name: "int"}, None)
        except Exception as e:
            print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK