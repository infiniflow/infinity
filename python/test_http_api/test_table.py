import os
import sys
import pytest
import concurrent.futures
from httpapibase import HttpTest
from common.common_values import *


class TestTable(HttpTest):
    def test_http_verison(self):
        return

    def test_http_table(self):
        db_name = "default_db"
        self.show_database(db_name)
        self.drop_table(db_name, "my_table")
        self.create_table(
            db_name,
            "my_table",
            {
                "name":
                    {
                        "type": "varchar",
                        "constraints": ["not null"]
                    },
                "age":
                    {
                        "type": "integer",
                        "constraints": ["not null"]
                    },
                "score":
                    {
                        "type": "integer",
                        "constraints": ["not null"]
                    },
            },
        )
        self.create_table(
            db_name,
            "mytable!@#",
            {
                "c1":
                    {
                        "type": "integer",
                        "constraints": ["primary key"]
                    },
                "c2":
                    {
                        "type": "float",
                    }
            },
            {},
            {
                "status_code": 500,
                "error_code": 3004,
                # "error_message": "Duplicate table: mytable!@# in {db_name}.",
            },
        )

        self.create_table(
            db_name,
            "123_table",
            {
                "c1":
                    {
                        "type": "integer",
                        "constraints": ["primary key"]
                    },
                "c2":
                    {
                        "type": "float",
                    }
            },
            {},
            {
                "status_code": 500,
                "error_code": 3004,
                # "error_message": "Duplicate table: 123_table in {db_name}.",
            }
        )

        self.create_table(
            db_name,
            "my-table-dash",
            {
                "c1":
                    {
                        "type": "integer",
                        "constraints": ["primary key"]
                    },
                "c2":
                    {
                        "type": "float",
                    }
            },
            {},
            {
                "status_code": 500,
                "error_code": 3004,
                # "error_message": "Duplicate table: my-table-dash in {db_name}.",
            }
        )

        self.create_table(
            db_name,
            "bad_column",
            {
                "123":
                    {
                        "type": "integer",
                        "constraints": ["int", "primary key"]
                    },
                "c2":
                    {
                        "type": "float",
                    }
            },
            {},
            {
                "status_code": 500,
                "error_code": 3005,
                # "error_message": "Duplicate table: bad_column in {db_name}.",
            }
        )

        self.list_tables("default_db")
        self.drop_table("default_db", "my_table")
        self.list_tables("default_db")
        return

    def test_http_show_tables(self):
        self.show_database("default_db")
        tblist = self.get_all_tables("default_db")
        print(tblist)
        return

    def test_http_create_varchar_table(self):
        '''
            test create table with varchar column
        '''
        table_name = "test_create_varchar_table"
        self.show_database("default_db")
        self.drop_table("default_db", table_name)
        self.create_table(
            "default_db",
            table_name,
            {
                "c1":
                    {
                        "type": "varchar",
                        "constraints": ["primary key"]
                    },
                "c2":
                    {
                        "type": "float",
                    }
            },
        )
        self.drop_table("default_db", table_name)
        return

    def test_http_create_embedding_table(self):
        """
            test create table with embedding column
        """
        table_name = "test_create_embedding_table"
        self.show_database("default_db")
        self.drop_table("default_db", table_name)
        self.create_table(
            "default_db",
            table_name,
            {
                "c1":
                    {
                        "type": "integer",
                        "constraints": []
                    },
            },
        )
        self.drop_table("default_db", table_name)
        return

    def test_http_create_table_with_invalid_column_name(self):
        """
            test create table with invalid column_name
        """
        db_name = "default_db"
        table_name = "test_create_invalid_column_name"

        invalid_clumn_name = ["", " ", "12", "[]", "()", "{}", "10", 1.1]
        for i in invalid_clumn_name:
            self.show_database(db_name)
            self.drop_table("default_db", table_name)
            if i == "":
                self.create_table(
                    db_name,
                    table_name,
                    {i: {
                        "type": "float",
                    }},
                    {},
                    {
                        "status_code": 500,
                        "error_code": 3042,  # empty column name
                        # "error_message": "Duplicate table: {table_name} in {db_name}.",
                    }
                )
            else:
                self.create_table(
                    db_name,
                    table_name,
                    {i: {
                        "type": "float",
                    }},
                    {},
                    {
                        "status_code": 500,
                        "error_code": 3005,  # invalid column name
                        # "error_message": "Duplicate table: {table_name} in {db_name}.",
                    }
                )
            self.drop_table(db_name, table_name)
        return

    def test_http_create_table_with_invalid_column_name_python(self):
        db_name = "default_db"
        table_name = "my_table"

        self.show_database(db_name)
        self.drop_table("default_db", table_name)

        for column_name in invalid_name_array:
            if column_name == "":
                self.create_table(
                    db_name,
                    table_name,
                    {str(column_name): {
                        "type": "float",
                    }},
                    {},
                    {
                        "status_code": 500,
                        "error_code": 3042,  # empty column name
                        # "error_message": "Duplicate table: {table_name} in {db_name}.",
                    }
                )
            else:
                self.create_table(
                    db_name,
                    table_name,
                    {
                        str(column_name):
                            {
                                "type": "integer",
                                "constraints": []
                            },
                    },
                    {},
                    {
                        "status_code": 500,
                        "error_code": 3005,
                        # "error_message": "Duplicate table: {table_name} in {db_name}.",
                    }
                )

        self.drop_table(db_name, table_name)
        return

    def test_http_table_with_different_column_name(self):
        """
        test create/drop/show/get valid table name with different column names
        """
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table("default_db", table_name)
        for column_name in invalid_name_array:
            # create table
            if column_name == '':
                continue
            self.create_table(
                db_name,
                table_name,
                {
                    str(column_name):
                        {
                            "type": "integer"
                        }
                },
                {},
                {
                    "status_code": 500,
                    "error_code": 3005,
                    # "error_message": "Duplicate table: {table_name} in {db_name}.",
                }
            )
            # list table
            self.list_tables(db_name)
            # get tables
            self.show_table(db_name, table_name, expect={
                "status_code": 500,
                "error_code": 3022,
            })
            # drop table
            self.drop_table(db_name, table_name)
        return

    def test_http_table_with_different_column_types(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "c1": {"type": "BOOLEAN", },
                "c2": {"type": "INTEGER", },
                "c3": {"type": "TINYINT", },
                "c4": {"type": "SMALLINT", },
                "c5": {"type": "BIGINT", },
                "c6": {"type": "FLOAT", },
                "c7": {"type": "DOUBLE", },
                "c8": {"type": "VARCHAR", },
                # "c10":{"type":"ARRAY",},
            }
        )
        self.list_tables(db_name)
        self.show_table(db_name, table_name, expect={
            "error_code": 0,
            "database_name": db_name,
            "table_name": table_name,
            "column_count": 8,
            "segment_count": 0,
            "row_count": 0
        })
        self.drop_table(db_name, table_name)
        return

    def test_http_table_with_various_column_types(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        c_count = 100
        types = [
            "integer", "tinyint", "smallint", "bigint", "float",
            "double", "varchar", "boolean"
        ]
        sz = len(types)
        # make params
        params = {}
        for i in range(c_count - sz):
            params.update({
                "c" + str(i): {"type": types[i % sz]}
            })

        # create tb with 10000 columns with various column types
        self.create_table(db_name, table_name, params)
        # list tables
        self.list_tables(db_name, {
            "error_code": 0,
        })
        # show tables
        self.show_table(db_name, table_name, {
            "error_code": 0,
            "database_name": db_name,
            "table_name": table_name,
            "column_count": c_count - sz,
        })
        # drop tables
        self.drop_table(db_name, table_name)
        return

    # INFO:if use invalid options, then infinity will use default options: kInvalid
    def test_http_table_with_different_invalid_options(self):
        db_name = "default_db"
        table_name = "test_table_with_different_invalid_options"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        # create tables
        url = f"databases/{db_name}/tables/{table_name}"
        h = self.set_up_header(['accept', 'content-type'])
        idx = 0
        for name in invalid_name_array:
            if idx == 0:
                d = self.set_up_data([], {'fields': {"c1": {"type": "integer", }, }, 'properties': {},
                                          'create_option': str(name)})
                r = self.request(url, "post", h, d)
                # print(r)
                expect = {"status_code": 200, "error_code": 0}
                self.tear_down(r, expect)
            else:
                d = self.set_up_data([], {'fields': {"c1": {"type": "integer", }, }, 'properties': {},
                                          'create_option': str(name)})
                r = self.request(url, "post", h, d)
                # print(r)
                expect = {"status_code": 500, "error_code": 3017}
                self.tear_down(r, expect)
            idx += 1
        self.drop_table(db_name, table_name)
        return

    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("SKIPTIMECOST")!="0", reason="Cost too much times,and may cause the serve to terminate")
    def test_http_various_tables_with_various_columns(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        tb_count = 1000
        column_count = 10000

        types = [
            "integer", "tinyint", "smallint", "bigint", "float",
            "double", "varchar", "boolean",
        ]
        sz = len(types)
        # make params
        params = {}
        for i in range(column_count - sz):
            params.update({
                "c" + str(i): {"type": types[i % sz]}
            })

        for i in range(tb_count):
            self.create_table(
                db_name,
                "my_table" + str(i),
                params,
            )
        for i in range(tb_count):
            self.drop_table(
                db_name,
                "my_table" + str(i),
        )
        return

    def test_http_after_disconnect_use_table(self):
        pass

    def test_http_create_drop_table(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {"c1": {"type": "integer", "constraints": ["primary key"], }, "c2": {"type": "float", }}
        )
        self.create_table(
            db_name,
            table_name,
            {"c1": {"type": "integer", "constraints": ["primary key"], }, "c2": {"type": "float", }},
            {},
            {"status_code": 200, "error_code": 0, },
            "kIgnore",
        )

        self.create_table(
            db_name,
            table_name,
            {"c1": {"type": "integer", "constraints": ["primary key"], }, "c2": {"type": "float", }
             },
            {},
            {"status_code": 500, "error_code": 3017, },
            "kError",
        )

        self.drop_table(db_name, table_name)

    @pytest.mark.skip(reason="Feature request")
    def test_http_describe_various_table(self):
        pass

    def test_http_create_1k_table(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        tb_count = 100
        for i in range(tb_count):
            self.create_table(
                db_name,
                table_name + str(i),
                {"c1": {"type": "integer"}}
            )
            # get all tables
        res = self.get_all_tables(db_name)
        print("table nums: " + str(len(res)))
        # assert len(res) == tb_count

        # show table
        for i in range(tb_count):
            self.show_table(db_name, table_name + str(i), expect={
                "error_code": 0,
                "database_name": db_name,
                "table_name": table_name + str(i),
                # "column_count" : 1,
                "segment_count": 0,
                "row_count": 0
            })
            # drop table
        for i in range(tb_count):
            self.drop_table(db_name, table_name + str(i))
        return

    # create/drop/list/get 1M table to reach the limit
    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("SKIPTIMECOST")!="0", reason="Cost too much times")
    def test_http_create_1m_table(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        tb_count = 1000000
        for i in range(tb_count):
            self.create_table(
                db_name,
                table_name + str(i),
                {"c1": {"type": "integer"}}
            )
            # get all tables
        res = self.get_all_tables(db_name)
        print("table nums: " + str(len(res)))
        # assert len(res) == tb_count

        # show table
        for i in range(tb_count):
            self.show_table(db_name, table_name + str(i), expect={
                "error_code": 0,
                "database_name": db_name,
                "table_name": table_name + str(i),
                # "column_count" : 1,
                "segment_count": 0,
                "row_count": 0
            })
            # drop table
        for i in range(tb_count):
            self.drop_table(db_name, table_name + str(i))
        return

    def test_http_create_or_drop_same_table_in_different_thread(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into processpool
            futures = [executor.submit(self.create_table(
                db_name,
                table_name,
                {
                    "c1": {
                        "type": "integer",
                    }
                }
            ), i) for i in range(16)]
            # wait all processes finished
            concurrent.futures.wait(futures)

        # drop table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into threadpool
            futures = [executor.submit(self.drop_table(db_name, table_name), i) for i in range(16)]
            # wait all threads finished
            concurrent.futures.wait(futures)
        return

    def test_http_create_empty_column_table(self):
        db_name = "default_db"
        table_name = "my_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, None, None, expect={
            "status_code": 500,
            "error_code": 3048,
        })
        self.drop_table(db_name, table_name)
        return

        # @pytest.mark.parametrize("types", [

    #     "int", "int8", "int16", "int32", "int64", "integer",
    #     "float", "float32", "double", "float64",
    #     "varchar",
    #     "bool",
    #     "vector, 3, float"])
    # def test_http_create_valid_option(self, types):
    #     db_name = "default_db"
    #     table_name = "test_valid_option"
    #     self.showdb(db_name)
    #     self.dropTable(db_name,table_name)
    #     self.createTable(db_name,table_name,{"c1":types})
    #     return

    def test_http_drop_option(self):
        db_name = "default_db"
        table_name = "test_drop_option"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, {"c1": {"type": "integer"}})
        self.drop_table(db_name, table_name)
        self.drop_table(
            db_name,
            table_name,
            {
                "status_code": 500,
                "error_code": 3022,
            },
            "kError"
        )
        return

    def test_http_create_same_name_table(self):
        db_name = "default_db"
        table_name = "test_create_same_name"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {'type': 'integer'}})
        self.create_table(db_name, table_name, {"c1": {'type': 'integer'}})
        self.create_table(db_name, table_name, {"c1": {'type': 'integer'}}, {}, {
            "status_code": 500,
            "error_code": 3017,
        }, "kError")
        self.drop_table(db_name, table_name)
        return

    def test_http_drop_same_name_table(self):
        db_name = "default_db"
        table_name = "test_drop_same_name"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.drop_table(db_name, table_name)
        self.drop_table(db_name, table_name, {
            "status_code": 500,
            "error_code": 3022,
        }, "kError")
        return

    def test_http_same_column_name(self):
        db_name = "default_db"
        table_name = "test_same_column_name"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        # self.createTable(db_name,table_name,{"c1": {'type':"integer"},"c1": {"type":"integer"}})
        url = f"databases/{db_name}/tables/{table_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = {'create_option': 'ignore_if_exists', 'fields': {'c1': {'type': 'integer'}, 'c1': {'type': 'integer'}},
             'properties': []}
        r = self.request(url, "post", h, d)
        self.tear_down(r, {})
        return

    def test_http_column_numbers(self):
        db_name = "default_db"
        table_name = "test_same_column_name"

        column_number = [0, 1, pow(2, 63) - 1]
        params = {}
        for i in column_number:
            params.update({
                "c" + str(i): {"type": types[i % len(types)]}
            })
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, params)
        self.drop_table(db_name, table_name)
        return

    def test_http_table_create_valid_option(self):
        db_name = "default_db"
        table_name = "test_table_create_valid_option"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in create_valid_option:
            url = f"databases/{db_name}/tables/{table_name}"
            h = self.set_up_header(['accept', 'content-type'])
            d = self.set_up_data(['create_option'],
                                 {'fields': {"c1": {"type": "integer"}}, 'properties': {}, 'create_option': str(i)})
            r = self.request(url, "post", h, d)
            print(r)
            self.tear_down(r, {
                "status_code": 200,
                "error_code": 0,
            })
            self.drop_table(db_name, table_name)
        return

    def test_http_table_create_invalid_option(self):
        db_name = "default_db"
        table_name = "test_table_create_invalid_option"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in create_invalid_option:
            url = f"databases/{db_name}/tables/{table_name}"
            h = self.set_up_header(['accept', 'content-type'])
            d = self.set_up_data(['create_option'],
                                 {'fields': {"c1": {"type": "integer"}}, 'properties': {}, 'create_option': str(i)})
            r = self.request(url, "post", h, d)
            print(r)
            self.tear_down(r, {
                "status_code": 200,
                "error_code": 0,
            })
            self.drop_table(db_name, table_name)
        return

    def test_http_table_drop_valid_option(self):
        db_name = "default_db"
        table_name = "test_table_drop_valid_option"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in drop_valid_option:
            self.create_table(db_name, table_name + str(i), {"c1": {"type": "integer"}}, {}, {})
            self.drop_table(db_name, table_name + str(i), {
                "status_code": 200,
                "error_code": 0,
            }, i)
        return

    def test_http_table_drop_invalid_option(self):
        db_name = "default_db"
        table_name = "test_table_drop_invalid_option"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in drop_invalid_option:
            self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, {}, {})

            url = f"databases/{db_name}/tables/{table_name}"
            h = self.set_up_header(['accept', 'content-type'])
            d = self.set_up_data(['drop_option'], {'drop_option': str(i)})
            r = self.request(url, "delete", h, d)
            self.tear_down(r, {
                "status_code": 200,
                "error_code": 0,
            })
        return

    def test_http_create_duplicated_table_with_ignore_option(self):
        db_name = "default_db"
        table_name = "test_create_duplicated_table_with_error_option"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in range(100):
            self.create_table(db_name, table_name, {"c1": {"type": "integer"}})
        self.drop_table(db_name, table_name)

    def test_http_create_duplicated_table_with_error_option(self):
        db_name = "default_db"
        table_name = "test_create_duplicated_table_with_error_option"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in range(100):
            if i == 0:
                self.create_table(db_name, table_name, {"c1": {"type": "integer"}})
            else:
                self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, {}, {
                    "status_code": 500,
                    "error_code": 3017
                }, "kError")
        self.drop_table(db_name, table_name)
        return

        # TODO: Replace options

    def test_http_create_duplicated_table_with_replace_option(self):
        db_name = "default_db"
        table_name = "test_create_duplicated_table_with_error_option"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for i in range(100):
            self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, {}, {
                "status_code": 200,
                "error_code": 0
            }, "kIgnore")
        self.drop_table(db_name, table_name)
        return
