import sys
import pytest
import os
import signal
import time
import concurrent.futures
from httpapibase import HttpTest
from common.common_values import *


class TestInsert(HttpTest):
    def test_http_version(self):
        return

    def test_http_insert_basic(self):
        db_name = "default"
        table_name = "table_2"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
                "constraints": ["primary key", "not null"],
            },
            "c2": {
                "type": "integer",
                "constraints": ["not null"],
            }
        })

        self.insert(db_name, table_name, [{"c1": 0, "c2": 0}])
        self.insert(db_name, table_name, [{"c1": 1, "c2": 1}])
        self.insert(db_name, table_name, [{"c1": 2, "c2": 2}])
        self.insert(db_name, table_name, [
                    {"c1": 3, "c2": 3}, {"c1": 4, "c2": 4}])

        self.drop_table(db_name, table_name)
        return

    def test_http_insert_varchar(self):
        db_name = "default"
        table_name = "test_insert_varchar"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "varchar",
            }
        })
        self.insert(db_name, table_name, [{"c1": "aaa"}])
        self.insert(db_name, table_name, [{"c1": " test_insert_varchar "}])
        self.insert(db_name, table_name, [{"c1": "^789$ test insert varchar"}])

        self.drop_table(db_name, table_name)
        return

    def test_http_insert_big_varchar(self):
        db_name = "default"
        table_name = "test_insert_big_varchar"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "varchar",
            }
        })
        for i in range(100):
            self.insert(db_name, table_name, [
                        {"c1": "test_insert_big_varchar" * 1000}])
        self.drop_table(db_name, table_name)
        return

    def test_http_insert_embedding(self):
        db_name = "default"
        table_name = "test_insert_big_embedding"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        })
        self.insert(db_name, table_name, [{"c1": [1, 2, 3]}])
        self.insert(db_name, table_name, [{"c1": [4, 5, 6]}])
        self.insert(db_name, table_name, [{"c1": [7, 8, 9]}])
        self.insert(db_name, table_name, [{"c1": [-7, -8, -9]}])

        self.drop_table(db_name, table_name)
        return

    def test_http_insert_big_embedding(self):
        db_name = "default"
        table_name = "test_insert_big_embedding"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {

                "type": "vector",
                "dimension": 65535,
                "element_type": "integer",
            }
        })
        self.insert(db_name, table_name, [{"c1": [1] * 65535}])
        self.insert(db_name, table_name, [{"c1": [4] * 65535}])
        self.insert(db_name, table_name, [{"c1": [7] * 65535}])
        self.insert(db_name, table_name, [{"c1": [-999999] * 65535}])

        self.drop_table(db_name, table_name)
        return

    def test_http_insert_big_embedding_float(self):
        db_name = "default"
        table_name = "test_insert_big_embedding_float"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 65535,
                "element_type": "float",
            }
        })
        self.insert(db_name, table_name, [{"c1": [1] * 65535}])
        self.insert(db_name, table_name, [{"c1": [-999999] * 65535}])
        self.insert(db_name, table_name, [{"c1": [1.1] * 65535}])
        self.insert(db_name, table_name, [{"c1": [-999999.988] * 65535}])

        self.drop_table(db_name, table_name)
        return

    def test_http_insert_big_embedding_various_type(self):
        self.test_http_insert_big_embedding()
        self.test_http_insert_big_embedding_float()

    def test_http_insert_exceed_block_size(self):
        db_name = "default"
        table_name = "test_insert_exceed_block_size"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "float"
            }
        })
        values = [{"c1": 1} for _ in range(8193)]
        self.insert(db_name, table_name, values, expect={
            "status_code": 500,
            "error_code": 7011
        })
        self.drop_table(db_name, table_name)
        return

    @pytest.mark.parametrize("examples", list(zip(types, types_example)))
    def test_http_insert_data_not_aligned_with_table_definition(self, examples):
        db_name = "default"
        table_name = "test_insert_data_not_aligned_with_table_definition"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "integer",
            },
            "c2": {
                "type": examples[0],
            }
        })
        values = [{"c1": 1, "c2": examples[1]}]
        self.insert(db_name, table_name, values)
        self.drop_table(db_name, table_name)

        return

    def test_http_insert_data_into_non_existent_table(self):
        db_name = "default"
        table_name = "test_insert_data_into_non_existent_table"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
                          "c1": {"type": "integer", }, "c2": {"type": "integer", }})
        self.drop_table(db_name, table_name)

        values = [{"c1": 1, "c2": 1}]
        self.insert(db_name, table_name, values, expect={
            "status_code": 500,
            "error_code": 3022
        })
        return

    def test_http_insert_empty_into_table(self):
        db_name = "default"
        table_name = "test_insert_empty_into_table"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        for t in types:
            self.create_table(db_name, table_name, {
                "c1": {
                    "type": "float"
                },
                "c2": {
                    "type": t
                }
            })
            values = [{}]
            self.insert(db_name, table_name, values, expect={
                "status_code": 500,
                "error_code": 3013
            })
            self.drop_table(db_name, table_name)
        return

    # IVFFlat realtime index is not supported yet
    def test_http_insert_data_into_index_created_table(self):
        db_name = "default"
        table_name = "test_insert_data_into_index_created_table"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        # create table
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            }
        })
        # create index
        self.create_index(db_name, table_name, "my_index_1", ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })

        self.create_index(db_name, table_name, "my_index_2", ["c1"], {
            "type": "IVFFlat",
            "centroids_count": "128",
            "metric": "l2",
        })
        # insert
        values = [{"c1": [1.1 for _ in range(1024)]}]
        self.insert(db_name, table_name, values)
        # delete
        self.drop_table(db_name, table_name)
        return

    def test_http_insert_table_with_10000_columns(self):
        db_name = "default"
        table_name = "test_insert_table_with_10000_columns"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        # create table
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        # insert
        for i in range(100):
            values = [{"c1": i * 100 + j, "c2": i * 100 + j + 1}
                      for j in range(100)]
            self.insert(db_name, table_name, values)
        return

    def test_http_read_after_shutdown(self):
        db_name = "default"
        table_name = "test_insert_table_with_10000_columns"
        self.show_database(db_name)
        self.show_table(db_name, table_name, expect={
            "status_code": 200,
            "error_code": 0,
        })
        values = [{"c1": 1, "c2": 1}]
        self.insert(db_name, table_name, values, expect={
            "status_code": 200,
            "error_code": 0,
        })
        self.drop_table(db_name, table_name)
        return

    def test_http_insert_with_not_matched_columns(self):
        values = [[{"c1": 1}], [{"c1": 1, "c2": 1, "c3": 1}]]
        db_name = "default"
        table_name = "test_insert_with_not_matched_columns"
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        self.insert(db_name, table_name, values, {
            "status_code": 500,
            "error_code": 3057,
        })
        self.drop_table(db_name, table_name)
        return

    # @pytest.mark.parametrize("values", [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]])
    def test_http_insert_with_exceeding_invalid_value_range(self):
        values = [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]]
        db_name = "default"
        table_name = "test_insert_with_exceeding_invalid_value_range"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        # insert
        self.insert(db_name, table_name, values, expect={
            "status_code": 500,
            "error_code": 3057,
        })
        self.drop_table(db_name, table_name)
        return
        # PASS

    # @pytest.mark.parametrize("batch", [10, 1024, 2048])
    def test_http_batch_insert_within_limit(self):
        batch = [10, 1024, 2048]
        db_name = "default"
        table_name = "test_batch_insert_within_limit"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })
        for len in batch:
            values = [{"c1": 1, "c2": 2} for _ in range(len)]
            self.insert(db_name, table_name, values)
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_batch_insert(self):
        db_name = "default"
        table_name = "test_batch_insert"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {"type": "integer", }
        })

        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        self.insert(db_name, table_name, values)

        self.drop_table(db_name, table_name)
        return

    @pytest.mark.skip(reason="error")
    @pytest.mark.parametrize("batch", [10, 1024])
    @pytest.mark.parametrize("types", [(1, False), (1.1, False), ("1#$@!adf", False), ([1, 2, 3], True)])
    def test_http_insert_with_invalid_data_type(self, batch, types):
        db_name = "default"
        table_name = "test_insert_with_invalid_data_type"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
            "c2": {
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        })
        for i in range(5):
            values = [{"c1": 1, "c2": types[0]} for _ in range(batch)]
            if not types[1]:
                self.insert(db_name, table_name, values, expect={
                    "status_code": 500,
                    "error_code": 3032,
                })
            else:
                self.insert(db_name, table_name, values)
        self.drop_table(db_name, table_name)
        return

    def test_http_batch_insert_with_invalid_column_count(self):
        batch = [10, 1024]
        for batch in batch:
            db_name = "default"
            table_name = "test_batch_insert_with_invalid_column_count"
            self.show_database(db_name)
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, {
                "c1": {"type": "integer", },
            })
            for i in range(5):
                values = [{"c1": 1, "c2": 1} for _ in range(batch)]
                self.insert(db_name, table_name, values, expect={
                    "status_code": 500,
                    "error_code": 3013
                })
            self.drop_table(db_name, table_name)
        return

    # @pytest.mark.parametrize('column_types_example', [[1, 2, 3], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]])
    def test_http_various_insert_types(self):
        db_name = "default"
        table_name = "test_various_insert_types"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "varchar", },
        })
        self.show_table(db_name, table_name, expect={
            "error_code": 0,
        })

        values = [{"c1": [1, 2, 3]} for _ in range(5)]
        self.insert(db_name, table_name, values, {
            "status_code": 500,
        })

        self.drop_table(db_name, table_name)
        return

    @pytest.mark.complex
    @pytest.mark.skip(reason="TODO")
    def test_http_insert_and_shutdown_output(self):
        os.system("rm -fr /var/infinity")
        db_name = "default"
        table_name = "test_insert_and_shutdown_output"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
        })
        for i in range(10):
            values = [{"c1": 1} for _ in range(100)]
            self.insert(db_name, table_name, values)
        os.kill(os.getpid(), signal.SIGINT)
        time.sleep(1)

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.show_table(db_name, table_name, {
            "status_code": 404
        })
        os.kill(os.getpid(), signal.SIGINT)
        return

    def test_http_insert_zero_column(self):
        db_name = "default"
        table_name = "test_insert_zero_colum"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer", },
        })
        self.insert(db_name, table_name, [], {
            "status_code": 500,
            "error_code": 3067
        })
        self.drop_table(db_name, table_name)
        return

    def test_http_insert_no_match_column(self):
        column_name = ["c2", "$%#$sadf", ]
        db_name = "default"
        table_name = "test_insert_no_match_column"
        for name in column_name:
            self.show_database(db_name)
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, {
                "c1": {"type": "integer", },
            })
            self.insert(db_name, table_name, [{str(name): "100"}], {
                "status_code": 500,
                "error_code": 3024,
            })
            self.drop_table(db_name, table_name)
        return
