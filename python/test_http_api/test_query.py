import sys
import pytest
from httpapibase import HttpTest
from common.common_values import *
import infinity.index as index


class TestQuery(HttpTest):
    def test_http_query(self):
        db_name = "default"
        table_name = "my_table"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name, table_name, {
                "num": {
                    "type": "integer",
                },
                "body": {
                    "type": "varchar",
                },
                "vec": {
                    "type": "varchar",
                    "dimension": 5,
                    "element_type": "float"
                }
            }
        )
        self.insert(db_name, table_name,
                    [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
        self.insert(db_name, table_name,
                    [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
        self.insert(db_name, table_name,
                    [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])
        self.create_index(db_name, table_name, idxname, fields=["body"],
                          index={"column_name": "body", "type": "FULLTEXT"})
        self.drop_table(db_name, table_name)
        return

    def test_http_query_builder(self):
        db_name = "default"
        table_name = "my_table"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name, table_name, {
                "c1": {
                    "type": "integer",
                },
            }
        )
        self.select(db_name, table_name,
                    ["*"])
        self.drop_table(db_name, table_name)
