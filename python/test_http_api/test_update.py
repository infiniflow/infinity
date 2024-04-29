import os
import sys
import pytest
import time
from httpapibase import HttpTest
from common.common_values import *


class TestUpdate(HttpTest):
    def test_http_version(self):
        return

    # PASS
    def test_http_update(self):
        db_name = "default_db"
        table_name = "test_update"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
                "constraints": ["primary key", "not null"],
            },
            "c2": {
                "type": "integer",
            },
            "c3": {
                "type": "integer",
            }
        })
        self.insert(db_name, table_name,
                    [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
                     {"c1": 4, "c2": 40, "c3": 400}]
                    )
        self.update(db_name, table_name, {"c2": 90, "c3": 900}, "c1 = 1")
        self.select(db_name, table_name,
                    ["c1", "c2", ], "", {}, {},
                    {
                        "error_code": 0,
                        "output": [{'c1': '2', 'c2': '20'}, {'c1': '3', 'c2': '30'}, {'c1': '4', 'c2': '40'},
                                   {'c1': '1', 'c2': '90'}]
                    })
        self.update(db_name, table_name, {"c2": 90, "c3": 900}, None, {
            "status_code": 500,
            "error_code": 3067,
        })
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_update_empty_table(self):
        db_name = "default_db"
        table_name = "test_update_empty_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {}, {}, {
            "status_code": 500,
            "error_code": 3048,
        })
        self.show_table(db_name, table_name, {
            "status_code": 500,
            "error_code": 3022,
        })
        self.update(db_name, table_name, {"c2": 90, "c3": 900}, "c1 = 1", expect={
            "status_code": 500,
            "error_code": 3022,
        })
        self.drop_table(db_name, table_name)
        return

    def test_http_update_non_existent_table(self):
        db_name = "default_db"
        table_name = "test_update_non_existent_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", },
            "c3": {"type": "integer", }
        })
        self.show_table(db_name, table_name, {
            "error_code": 0,
            "database_name": db_name,
            "table_name": table_name,
            "column_count": 3,
        })
        self.update(db_name, table_name, {"c2": 90, "c3": 900}, "c1 = 1")
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_update_no_row_is_met_the_condition(self):
        db_name = "default_db"
        table_name = "test_update_no_row_is_met_the_condition"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        types = [
            "integer", "tinyint", "smallint", "bigint", "float", "double",  # "hugeint"
        ]
        types_example = [
            1, 127, 32767, 10, 10.10, 10.1010,  # 10
        ]

        for i in range(len(types)):
            self.create_table(db_name, table_name, {
                "c1": {"type": types[i]},
                "c2": {"type": types[i]}
            })
            self.show_table(db_name, table_name, {
                "error_code": 0,
                "database_name": db_name,
                "table_name": table_name,
                "column_count": 2,
            })
            self.insert(db_name, table_name,
                        [{"c1": types_example[i], "c2": types_example[i]}]
                        )
            self.update(db_name, table_name, {"c2": types_example[i]}, "c1 = 2")
            self.drop_table(db_name, table_name)

        return

    def test_http_update_all_row_is_met_the_condition(self):
        db_name = "default_db"
        table_name = "test_update_all_row_is_met_the_condition"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        types = [
            "integer", "tinyint", "smallint", "bigint", "float", "double",  # "hugeint"
        ]
        types_example = [
            1, 127, 32767, 10, 10.10, 10.1010,  # 10
        ]

        for i in range(len(types)):
            self.create_table(db_name, table_name + str(i), {
                "c1": {"type": types[i]},
                "c2": {"type": types[i]}
            })
            self.show_table(db_name, table_name + str(i), {
                "error_code": 0,
                "database_name": db_name,
                "table_name": table_name + str(i),
                "column_count": 2,
            })
            self.insert(db_name, table_name + str(i),
                        [{"c1": types_example[i], "c2": types_example[i]}]
                        )
            self.update(db_name, table_name + str(i), {"c2": types_example[i]}, "c1 = " + str(types_example_array[i]))
            self.drop_table(db_name, table_name + str(i))
        return

    def test_http_update_table_with_one_block(self):
        db_name = "default_db"
        table_name = "test_update_all_row_is_met_the_condition"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer"},
            "c2": {"type": "integer"}
        })
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        self.insert(db_name, table_name, values)
        self.update(db_name, table_name, {"c2": 20}, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_update_table_with_one_segment(self):
        db_name = "default_db"
        table_name = "test_update_all_row_is_met_the_condition"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer"},
            "c2": {"type": "integer"}
        })
        for _ in range(1024):
            values = [{"c1": 1, "c2": 2} for _ in range(8)]
            self.insert(db_name, table_name, values)
        self.update(db_name, table_name, {"c2": 20}, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_update_before_delete(self):
        db_name = "default_db"
        table_name = "test_update_before_delete"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })

        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        self.insert(db_name, table_name, values)

        self.delete(db_name, table_name, "c1 = 1")
        self.update(db_name, table_name, {"c2": 20}, "c1 = 1")
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_update_inserted_data(self):
        db_name = "default_db"
        table_name = "test_update_before_delete"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        self.insert(db_name, table_name, values)
        self.update(db_name, table_name, {"c2": 21}, "c1 = 1")
        self.drop_table(db_name, table_name)

        return

    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST")!="1", reason="Taking too much time.")
    def test_http_update_inserted_long_before(self):
        db_name = "default_db"
        table_name = "test_update_before_delete"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        self.insert(db_name, table_name, values)

        time.sleep(60)

        self.update(db_name, table_name, {"c2": 21}, "c1 = 1")
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_update_dropped_table(self):
        db_name = "default_db"
        table_name = "test_update_before_delete"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        self.update(db_name, table_name, {"c2": 21}, "c1 = 1")
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_update_invalid_value(self):
        db_name = "default_db"
        table_name = "test_update_before_delete"
        types = [
            "varchar"
        ]
        types_example = [
            [1, 2, 3]
        ]
        for i in range(len(types)):
            self.create_table(db_name, table_name, {
                "c1": {
                    "type": "integer",
                },
                "c2": {
                    "type": str(types[i]),
                }
            })
            self.update(db_name, table_name, {"c2": types_example[i]}, "c1 = 1", {
                "status_code": 500,
                "error_code": 3067,
            })
            self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_update_new_value(self):
        types = ["integer", "float"]
        types_example = [1, 1.333, "1"]
        db_name = "default_db"
        table_name = "test_update_before_delete"
        self.drop_table(db_name, table_name)
        for i in range(len(types)):
            self.create_table(db_name, table_name, {
                "c1": {
                    "type": "integer",
                },
                "c2": {
                    "type": str(types[i]),
                }
            })
            self.update(db_name, table_name, {"c2": types_example[i]}, "c1 = 1")
            self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_valid_filter_expression(self):
        filter_list = [
            "c1 > 10",
            "c2 > 1",
            "c1 > 0.1 && c2 < 3.0",
            "c1 > 0.1 && c2 < 1.0",
            "c1 < 0.1 && c2 < 1.0",
            "c1 < 0.1 && c1 > 1.0",
            "c1 = 0",
        ]
        types_example = ["1", "1.333"]

        db_name = "default_db"
        table_name = "test_valid_filter_expression"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
            },
            "c2": {
                "type": "float",
            }
        })
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(db_name, table_name, values)

        for i in range(len(filter_list)):
            self.update(db_name, table_name, {"c2": 1}, filter_list[i])
        self.drop_table(db_name, table_name)
        return

    def test_http_invalid_filter_expression(self):
        filter_list = [
            ("_row_id", 3063),
            ("#@$%@#f", 3063),
            ("c1 > 0.1 %@#$sf c2 < 1.0", 3063),
            ("c1 + 0.1 and c2 - 1.0", 3030),
            ("c1 * 0.1 and c2 / 1.0", 3030),
        ]
        types_example = [1, 1.333]

        db_name = "default_db"
        table_name = "test_valid_filter_expression"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
            },
            "c2": {
                "type": "float",
            }
        })
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            self.insert(db_name, table_name, values)
        for i in range(len(filter_list)):
            self.update(db_name, table_name, {"c2": types_example[i % 2]}, filter_list[i][0], {
                "status_code": 500,
                "error_code": filter_list[i][1],
            })
        self.drop_table(db_name, table_name)
        return
