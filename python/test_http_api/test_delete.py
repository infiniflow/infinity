import sys
import pytest
import time
from httpapibase import HttpTest
from common.common_values import *


class TestDelete(HttpTest):
    def test_http_version(self):

        return
        # pass

    def test_http_delete(self):
        db_name = "default"
        table_name = "test_delete"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
                "constraints": ["primary key", "not null"]
            },
            "c2": {
                "type": "integer",
            },
            "c3": {
                "type": "integer",
            }
        })
        self.show_table(db_name, table_name, {
            "error_code": 0,
        })
        self.insert(db_name, table_name,
                    [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
                     {"c1": 4, "c2": 40, "c3": 400}])
        self.delete(db_name, table_name, "c1 = 1")
        # self.select(db_name,table_name,[
        #     "c1,c2",
        # ],"",{},{
        #     "error_code":0,
        #     "output":[
        #         {'c1': (), 'c2': (), 'c3': ()}
        #     ]
        # })
        self.drop_table(db_name, table_name)
        return

        # PASS

    def test_http_delete_empty_table(self):
        db_name = "default"
        table_name = "test_delete_empty_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
                "constraints": ["primary key", "not null"]
            },
        })
        self.show_table(db_name, table_name, {
            "error_code": 0,
        })
        self.delete(db_name, table_name, "c1=1", {
            "status_code": 200,
            "error_code": 0,
        })
        self.drop_table(db_name, table_name)
        return
        # pass

    def test_http_delete_non_existent_table(self):
        db_name = "default"
        table_name = "test_delete_non_existent_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.delete(db_name, table_name, "", {
            "status_code": 500,
            "error_code": 3063,
        })
        return
        # PASS

    def test_http_delete_table_all_row_met_the_condition(self):
        db_name = "default"
        table_name = "test_delete_table_all_row_met_the_condition"
        types = [
            "integer", "tinyint", "smallint", "bigint", "float", "double",  # "hugeint"
        ]
        types_example = [
            1, 127, 32767, 10, 10.10, 10.1010,  # 10
        ]
        self.show_database(db_name)
        i = 0
        for t in types:
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, {
                "c1": {"type": str(t)
                       }
            })
            self.show_table(db_name, table_name, {
                "error_code": 0,
            })
            self.insert(db_name, table_name, [
                {"c1": str(types_example[i])}
            ])
            self.delete(db_name, table_name, "c1=" + str(types_example[i] - 1))
            i += 1
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_delete_table_no_rows_met_condition(self):
        db_name = "default"
        table_name = "test_delete_table_no_rows_met_condition"
        types = [
            "integer", "tinyint", "smallint", "bigint", "float", "double",  # "hugeint"
        ]
        types_example = [
            1, 127, 32767, 10, 10.10, 10.1010,  # 10
        ]
        for i in range(len(types)):
            self.drop_table(db_name, table_name + str(i))
        for i in range(len(types)):
            self.create_table(db_name, table_name + str(i), {
                "c1": {"type": str(types[i])
                       }
            })
            self.show_table(db_name, table_name + str(i), {
                "error_code": 0,
            })
            self.insert(db_name, table_name + str(i), [
                {"c1": str(types_example[i])}
            ])
            self.delete(db_name, table_name + str(i), "c1 = 0")
        for i in range(len(types)):
            self.drop_table(db_name, table_name + str(i))
        return

    # PASS
    def test_http_delete_table_with_one_block(self):
        db_name = "default"
        table_name = "test_delete_table_with_one_block"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, )
        values = [{"c1": 1} for _ in range(8192)]
        self.insert(db_name, table_name, values)
        self.delete(db_name, table_name, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_delete_table_with_one_segment(self):
        db_name = "default"
        table_name = "test_delete_table_with_one_segment"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, )
        for i in range(1024):
            values = [{"c1": i} for _ in range(10)]
            self.insert(db_name, table_name, values)
        for i in range(1024):
            self.delete(db_name, table_name, "c1 = " + str(i))
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_select_before_after_delete(self):
        db_name = "default"
        table_name = "test_select_before_after_delete"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, )

        for i in range(10):
            values = [{"c1": i} for _ in range(10)]
            self.insert(db_name, table_name, values)
        self.delete(db_name, table_name, "c1=1")
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_delete_insert_data(self):
        db_name = "default"
        table_name = "test_delete_insert_data"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, )

        values = [{"c1": 1} for _ in range(10)]
        self.insert(db_name, table_name, values)

        self.delete(db_name, table_name, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

    @pytest.mark.slow
    def test_http_delete_inserted_long_before_data(self):
        db_name = "default"
        table_name = "test_delete_inserted_long_before_data"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {"type": "integer"}}, )

        for i in range(1024):
            values = [{"c1": i} for _ in range(5)]
            self.insert(db_name, table_name, values)
        time.sleep(5)
        self.delete(db_name, table_name, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

        # Make Critical Error

    def test_http_delete_dropped_table(self):
        db_name = "default"
        table_name = "test_delete_dropped_table"
        self.drop_table(db_name, table_name)
        self.show_table(db_name, table_name, {
            "status_code": 500,
            "error_code": 3022,
        })
        self.delete(db_name, table_name, "c1 = 0", {
            "status_code": 500,
            "error_code": 3022,
        })
        self.drop_table(db_name, table_name)
        return

    def test_http_various_expression_in_where_clause(self):
        db_name = "default"
        table_name = "test_various_expression_in_where_clause"
        self.drop_table(db_name, table_name)
        types = [
            "integer", "tinyint", "smallint", "bigint", "float", "double"
        ]
        types_example = [
            1, 127, 32767, 10, 10.10, 10.1010,
        ]
        for i in range(len(types)):
            self.create_table(db_name, table_name, {
                "c1": {
                    "type": str(types[i]),
                }
            })
            self.show_table(db_name, table_name, {
                "error_code": 0,
            })
            values = [{"c1": types_example[i]} for _ in range(5)]
            self.insert(db_name, table_name, values)
            self.delete(db_name, table_name, "c1 = " + str(types_example[i]))
            self.drop_table(db_name, table_name)
        return

    def test_http_delete_one_block_without_expression(self):
        db_name = "default"
        table_name = "test_delete_one_block_without_expression"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {"c1": {"type": "integer", }})
        values = [{"c1": 1} for _ in range(8192)]
        self.insert(db_name, table_name, values)
        self.delete(db_name, table_name, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

    def test_http_delete_one_segment_without_expression(self):
        db_name = "default"
        table_name = "test_delete_one_segment_without_expression"
        self.create_table(db_name, table_name, {"c1": {"type": "integer", }})
        for i in range(1024):
            values = [{"c1": i} for _ in range(10)]
            self.insert(db_name, table_name, values)
        self.drop_table(db_name, table_name)
        return

    def test_http_filter_with_valid_expression(self):
        filter_list = [
            "c1 > 10",
            "c2 > 1",
            "c1 > 0.1 && c2 < 3.0",
            "c1 > 0.1 && c2 < 1.0",
            "c1 < 0.1 && c2 < 1.0",
            "c1 < 0.1 && c1 > 1.0",
            "c1 = 0",
        ]
        db_name = "default"
        table_name = "test_delete_one_segment_without_expression"
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "float"}}
                          )
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(db_name, table_name, values)
        for i in range(len(filter_list)):
            self.delete(db_name, table_name, filter_list[i])
        self.drop_table(db_name, table_name)
        return

    @pytest.mark.parametrize("filters", [("c1", 3063),
                                         ("_row_id", 3063),
                                         ("*", 3013),
                                         ("#@$%@#f", 3063),
                                         # ("c1 + 0.1 and c2 - 1.0", 0),  FIXME
                                         # ("c1 * 0.1 and c2 / 1.0", 0),  FIXME
                                         ("c1 > 0.1 %@#$sf c2 < 1.0", 3063)])
    def test_http_filter_with_invalid_expression(self, filters):
        db_name = "default"
        table_name = "test_filter_with_invalid_expression"
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "float"}}
                          )
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(db_name, table_name, values)

        self.delete(db_name, table_name, filters[0], {
            "status_code": 500,
            "error_code": filters[1],
        })
        self.drop_table(db_name, table_name)
        return
