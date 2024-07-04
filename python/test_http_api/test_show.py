import sys
import pytest
import time
from httpapibase import HttpTest
from common.common_values import *


class TestShow(HttpTest):
    def test_http_show_table(self):
        db_name = "default_db"
        table_name = "test_http_test_show_table"
        self.drop_table(db_name, table_name)
        self.show_table(db_name, table_name, {
            "status_code": 500,
            "error_code": 3022,
        })
        self.create_table(
            db_name, table_name, [
                {
                    "name": "num",
                    "type": "integer",
                },
                {
                    "name": "body",
                    "type": "varchar",
                },
                {
                    "name": "vec",
                    "type": "vector",
                    "dimension": 5,
                    "element_type": "float",
                }
            ]
        )
        self.show_table(db_name, table_name, {
            "error_code": 0,
            "database_name": db_name,
            "table_name": table_name,
            "column_count": 3,
            "row_count": 0
        })
        self.drop_table(db_name, table_name)
        return

    def test_http_show_columns(self):
        db_name = "default_db"
        table_name = "test_http_test_show_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(
            db_name, table_name, [
                {
                    "name": "num",
                    "type": "integer",
                },
                {
                    "name": "body",
                    "type": "varchar",
                },
                {
                    "name": "vec",
                    "type": "vector",
                    "dimension": 5,
                    "element_type": "float",
                }
            ]
        )

        self.show_table_columns(db_name, table_name, {
            "error_code": 0,
            "columns": [
                {'column_name': 'num', 'column_type': 'Integer', 'constraint': '', 'default': 'Null'},
                {'column_name': 'body', 'column_type': 'Varchar', 'constraint': '', 'default': 'Null'},
                {'column_name': 'vec', 'column_type': 'Embedding(float,5)', 'constraint': '', 'default': 'Null'}
            ]
        })

        self.drop_table(db_name, table_name)
        return
