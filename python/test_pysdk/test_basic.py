import sys
import os
import pandas as pd
from numpy import dtype
import pytest
from common import common_values
import infinity
import infinity_embedded
import infinity.index as index
from infinity.errors import ErrorCode
from infinity.common import ConflictType

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http
from common.utils import copy_data
import importlib

test_csv_file = "embedding_int_dim3.csv"
test_export_csv_file = "export_embedding_int_dim3.csv"
test_export_csv_file_part = "export_embedding_int_dim3_part.csv"
test_export_jsonl_file = "export_embedding_int_dim3.jsonl"
test_export_jsonl_file_part = "export_embedding_int_dim3_part.jsonl"


@pytest.mark.usefixtures("local_infinity")
@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity, http):
        if local_infinity:
            module = importlib.import_module("infinity_embedded.index")
            globals()["index"] = module
            self.uri = common_values.TEST_LOCAL_PATH
            self.infinity_obj = infinity_embedded.connect(self.uri)
        elif http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)

    def teardown(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # def test_version(self):
    #     self.test_infinity_obj._test_version()
    def test_connection(self, local_infinity):
        """
        target: test connect and disconnect server ok
        method: connect server
        expect: connect and disconnect successfully
        """
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        assert infinity_obj
        assert infinity_obj.disconnect()

    def test_create_db_with_invalid_name(self, local_infinity):
        """
        target: test db name limitation
        method: create db with empty name
        expect: create db fail with error message
        """
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        assert infinity_obj

        db_name = ""
        with pytest.raises(Exception,
                           match=f"DB name '{db_name}' is not valid. It should start with a letter and can contain only letters, numbers and underscores"):
            db = infinity_obj.create_database("")
        assert infinity_obj.disconnect()

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_basic(self, check_data ,suffix):
        """
        target: test basic operation
        method:
        1. connect
        2. create db
        3. list db
        4. drop db
        5. get db
        6. create table
        7. drop table
        8. create index
        9. drop index
        10. insert and search
        11. import and search
        12.
        expect: all operations successfully
        """
        res = self.infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK
        for db_name in res.db_names:
            if db_name == "my_db"+suffix:
                self.infinity_obj.drop_database(db_name, ConflictType.Error)

        # infinity
        db_obj = self.infinity_obj.create_database("my_db"+suffix, ConflictType.Error)
        assert db_obj is not None

        res = self.infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        res_db_names = []
        for db in res.db_names:
            if db == "my_db"+suffix or db == "default_db":
                res_db_names.append(db)
        assert len(res_db_names) == 2

        res = self.infinity_obj.drop_database("my_db"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table1"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table1"+suffix, {"c1": {"type": "int", "constraints": ["primary key"]}}, ConflictType.Error)
        assert table_obj is not None

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("my_table1"+suffix)
        assert res.error_code == ErrorCode.OK

        # index
        db_obj.drop_table("my_table2"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table2"+suffix, {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        table_obj = db_obj.get_table("my_table2"+suffix)
        assert table_obj

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "200",
                                                         "metric": "l2",
                                                         "encode": "lvq"
                                                     }),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("my_table2"+suffix)
        assert res.error_code == ErrorCode.OK

        # insert
        db_obj.drop_table("my_table3"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table3"+suffix, {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert table_obj is not None

        table_obj = db_obj.get_table("my_table3"+suffix)
        assert table_obj

        res = table_obj.insert(
            [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
        assert res.error_code == ErrorCode.OK
        # search
        res = table_obj.output(["c1 + 0.1"]).to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'(c1 + 0.1)': (1.1, 2.1)}).astype(
            {'(c1 + 0.1)': dtype('float64')}))

        res = table_obj.output(
            ["*"]).filter("c1 > 1").to_df()
        print(res)
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (2,), 'c2': (2.2,)}).astype(
            {'c1': dtype('int32'), 'c2': dtype('float32')}))

        res = db_obj.drop_table("my_table3"+suffix)
        assert res.error_code == ErrorCode.OK

        # import
        db_obj.drop_table("my_table4"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table4"+suffix, {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
        assert table_obj is not None
        table_obj = db_obj.get_table("my_table4"+suffix)
        assert table_obj

        if not check_data:
            copy_data(test_csv_file)

        assert os.path.exists(common_values.TEST_TMP_DIR + test_csv_file)

        res = table_obj.import_data(common_values.TEST_TMP_DIR + test_csv_file)
        assert res.error_code == ErrorCode.OK

        # export
        test_export_csv_file_path = common_values.TEST_TMP_DIR + suffix + test_export_csv_file
        if os.path.exists(test_export_csv_file_path):
            os.remove(test_export_csv_file_path)
        res = table_obj.export_data(test_export_csv_file_path, {"file_type": "csv"})
        assert res.error_code == ErrorCode.OK

        if os.path.exists(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file):
            os.remove(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file)
        res = table_obj.export_data(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file, {"file_type": "jsonl"})
        assert res.error_code == ErrorCode.OK

        if os.path.exists(common_values.TEST_TMP_DIR + suffix + test_export_csv_file_part):
            os.remove(common_values.TEST_TMP_DIR + suffix + test_export_csv_file_part)
        res = table_obj.export_data(common_values.TEST_TMP_DIR + suffix + test_export_csv_file_part, {"file_type": "csv"},
                                    ["c2", "c1", "_row_id", "_create_timestamp", "_delete_timestamp"])
        assert res.error_code == ErrorCode.OK

        if os.path.exists(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file_part):
            os.remove(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file_part)
        res = table_obj.export_data(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file_part, {"file_type": "jsonl"},
                                    ["c2", "c1", "_row_id", "_create_timestamp", "_delete_timestamp"])
        assert res.error_code == ErrorCode.OK

        db_obj.drop_table("my_table_export"+suffix, ConflictType.Ignore)
        export_table_obj = db_obj.create_table("my_table_export"+suffix,
                                               {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                                               ConflictType.Error)
        assert export_table_obj is not None

        res = export_table_obj.import_data(common_values.TEST_TMP_DIR + suffix + test_export_csv_file)
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)

        res = db_obj.drop_table("my_table_export"+suffix)
        assert res.error_code == ErrorCode.OK
        export_table_obj = db_obj.create_table("my_table_export"+suffix,
                                               {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                                               ConflictType.Error)
        assert export_table_obj is not None
        res = export_table_obj.import_data(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file,
                                           import_options={"file_type": "jsonl"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("my_table_export"+suffix)
        assert res.error_code == ErrorCode.OK

        export_table_obj = db_obj.create_table("my_table_export"+suffix,
                                               {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                                               ConflictType.Error)
        assert export_table_obj is not None
        res = export_table_obj.import_data(common_values.TEST_TMP_DIR + suffix + test_export_csv_file,
                                           import_options={"file_type": "csv"})
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("my_table_export"+suffix)
        assert res.error_code == ErrorCode.OK

        os.remove(common_values.TEST_TMP_DIR + suffix + test_export_csv_file)
        os.remove(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file)
        os.remove(common_values.TEST_TMP_DIR + suffix + test_export_csv_file_part)
        os.remove(common_values.TEST_TMP_DIR + suffix + test_export_jsonl_file_part)

        # search
        res = table_obj.output(
            ["c1"]).filter("c1 > 1").to_df()
        print(res)
        res = db_obj.drop_table("my_table4"+suffix)
        assert res.error_code == ErrorCode.OK
