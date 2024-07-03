import os
import pytest
from httpapibase import HttpTest
from common.common_values import *
import httputils


class TestImport(HttpTest):
    def test_http_version(self):
        return

    # PASS
    def test_http_import(self):
        httputils.check_data(TEST_TMP_DIR)

        file_name = "csv/embedding_int_dim3.csv"
        db_name = "default_db"
        table_name = "test_import"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])

        test_csv_dir = self.get_project_path() + TEST_DATA_DIR + file_name
        print("test_csv_dir: " + test_csv_dir)
        assert os.path.exists(test_csv_dir)

        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        })

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c1",
                "type": "vector",
                "dimension": 4,
                "element_type": "float",
            }
        ])

        file_path = self.get_project_path() + TEST_DATA_DIR + "csv/embedding_float_dim4.csv"
        assert os.path.exists(file_path)
        self.import_data(db_name, table_name, {
            "file_path": file_path,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        })

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_different_file_format_data(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_different_file_format_data"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])

        file_format = ["csv", "fvecs"]
        for format in file_format:
            if format == "fvecs":
                httputils.generate_fvecs(100, 128, "pysdk_test.fvecs")
                httputils.copy_data("pysdk_test.fvecs")
                self.drop_table(db_name, table_name)
                self.create_table(db_name, table_name, [
                    {
                        "name": "c1",
                        "type": "vector",
                        "dimension": 128,
                        "element_type": "float",
                    }
                ])
                file_path = TEST_TMP_DIR + "pysdk_test.fvecs"
                assert os.path.exists(file_path)
                self.import_data(db_name, table_name, {
                    "file_path": file_path,
                    "file_type": format,
                    "header": False,
                    "delimiter": ","
                })
                print(file_path)
            else:
                file_path = self.get_project_path() + TEST_DATA_DIR + format + "/embedding_int_dim3." + format
                assert os.path.exists(file_path)
                self.import_data(db_name, table_name, {
                    "file_path": file_path,
                    "file_type": format,
                    "header": False,
                    "delimiter": ","
                })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_empty_file_fvecs(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_different_file_format_data"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c2",
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            }
        ])
        file_path = self.get_project_path() + TEST_DATA_DIR + "fvecs/test_empty.fvecs"
        assert os.path.exists(file_path)
        self.import_data(db_name, table_name, {
            "file_path": file_path,
            "file_type": "fvecs",
            "header": False,
            "delimiter": ","
        })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_empty_file_csv(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_different_file_format_data"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])
        file_path = self.get_project_path() + TEST_DATA_DIR + "csv/test_empty.csv"
        assert os.path.exists(file_path)

        self.import_data(db_name, table_name, {
            "file_path": file_path, "file_type": "csv", "header": False, "delimiter": ","
        }, {
                             "status_code": 200, "error_code": 0,
                         })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_empty_file_jsonl(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_different_file_format_data"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])

        file_path = self.get_project_path() + TEST_DATA_DIR + "jsonl/test_empty.jsonl"
        self.import_data(db_name, table_name, {
            "file_path": file_path, "file_type": "jsonl", "header": False, "delimiter": ","
        }, {
                             "status_code": 200, "error_code": 0,
                         })
        assert os.path.exists(file_path)
        self.drop_table(db_name, table_name)
        return

    def test_http_import_empty_file_json(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_different_file_format_data"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])

        file_path = self.get_project_path() + TEST_DATA_DIR + "json/test_empty.json"
        assert os.path.exists(file_path)
        self.import_data(db_name, table_name, {
            "file_path": file_path, "file_type": "json", "header": False, "delimiter": ","
        }, {
                             "status_code": 200, "error_code": 0,
                         })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_format_unrecognized_data(self):
        httputils.check_data(TEST_TMP_DIR)
        file_format = ["txt"]
        db_name = "default_db"
        table_name = "test_import_format_unrecognized_data"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])
        for format in file_format:
            file_path = self.get_project_path() + TEST_DATA_DIR + format + "/http_test." + format
            assert os.path.exists(file_path)
            self.import_data(db_name, table_name, {
                "file_path": file_path,
                "file_type": format,
                "header": False,
                "delimiter": ","
            }, {
                                 "status_code": 500,
                                 "error_code": 3032,
                             })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_csv_with_different_delimiter(self):
        httputils.check_data(TEST_TMP_DIR)

        db_name = "default_db"
        table_name = "test_csv_with_different_delimiter"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        for i in range(len(delimiter)):
            self.create_table(db_name, table_name, [
                {"name": "c1", "type": "integer"},
                {"name": "c2", "type": "integer"},
            ])
            file_path = self.get_project_path() + TEST_DATA_DIR + "/csv/" + check_file_data[i]["file_name"]
            assert os.path.exists(file_path)
            self.import_data(db_name, table_name, {
                "file_path": file_path,
                "file_type": "csv",
                "header": False,
                "delimiter": delimiter[i][1],
            })
            self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_csv_with_different_delimiter_more_than_one_character(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_csv_with_different_delimiter_more_than_one_character"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, [
            {"name": "c1", "type": "integer"},
            {"name": "c2", "type": "integer"},
        ])
        file_path = self.get_project_path() + TEST_DATA_DIR + "/csv/pysdk_test_blankspace.csv"
        assert os.path.exists(file_path)
        self.import_data(db_name, table_name, {
            "file_path": file_path,
            "file_type": "csv",
            "header": False,
            "delimiter": ' ', }
                         )
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_csv_with_headers(self):
        httputils.check_data(TEST_TMP_DIR)
        file_name = "csv/embedding_int_dim3.csv"
        db_name = "default_db"
        table_name = "test_import"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 3,
                "element_type": "integer",
            }
        ])

        test_csv_dir = self.get_project_path() + TEST_DATA_DIR + file_name
        print("test_csv_dir: " + test_csv_dir)
        assert os.path.exists(test_csv_dir)

        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": True,
            "delimiter": ","
        })

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_fvecs_table_with_more_columns(self):
        httputils.check_data(TEST_TMP_DIR)
        file_name = "http_test.fvecs"
        httputils.generate_fvecs(100, 128, file_name)
        httputils.copy_data(file_name)
        db_name = "default_db"
        table_name = "test_import_fvecs_table_with_more_columns"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 128,
                "element_type": "float",
            }
        ])
        test_csv_dir = TEST_TMP_DIR + file_name
        print("test_file_dir: " + test_csv_dir)
        assert os.path.exists(test_csv_dir)

        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "fvecs",
            "header": False,
            "delimiter": ","
        }, {
                             "status_code": 500,
                             "error_code": 3037,
                         })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_embedding_with_not_match_definition(self):
        httputils.check_data(TEST_TMP_DIR)
        types = [{"name": "c2", "type": "vector", "dimension": 3, "element_type": "integer"},
                 {"name": "c2", "type": "vector", "dimension": 128, "element_type": "integer"},
                 {"name": "c2", "type": "vector", "dimension": 3, "element_type": "float"},
                 {"name": "c2", "type": "vector", "dimension": 128, "element_type": "float"},
                 {"name": "c2", "type": "vector", "dimension": 3, "element_type": "double"}]
        file_name = "embedding_int_dim3.csv"
        httputils.copy_data(file_name)
        db_name = "default_db"
        table_name = "test_import_embedding_with_not_match_definition"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        for i in range(len(types)):
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, [
                {"name": "c1", "type": "integer"},
                types[i]
            ])
            test_csv_dir = TEST_TMP_DIR + file_name
            self.import_data(db_name, table_name, {
                "file_path": test_csv_dir,
                "file_type": "fevcs",
                "header": False,
                "delimiter": ","
            }, {
                                 "status_code": 500,
                                 "error_code": 3032,
                             })

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_embedding_with_dimension_unmatch(self):
        httputils.check_data(TEST_TMP_DIR)
        file_name = "embedding_int_dim3.csv"
        httputils.copy_data(file_name)
        db_name = "default_db"
        table_name = "test_import_embedding_with_dimension_unmatch"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, [
            {
                "name": "c1",
                "type": "integer",
            },
            {
                "name": "c2",
                "type": "vector",
                "dimension": 2,
                "element_type": "integer",
            }
        ])

        test_csv_dir = TEST_TMP_DIR + file_name
        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "fevcs",
            "header": False,
            "delimiter": ","
        }, {
                             "status_code": 500,
                             "error_code": 3032,
                         })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_embedding_with_unmatched_elem_type(self):
        httputils.check_data(TEST_TMP_DIR)
        types = [{"name": "c2", "type": "vector", "dimension": 3, "element_type": "bool"},
                 {"name": "c2", "type": "vector", "dimension": 128, "element_type": "varchar", }]
        file_name = "embedding_int_dim3.csv"
        httputils.copy_data(file_name)
        db_name = "default_db"
        table_name = "test_import_embedding_with_unmatched_elem_type"

        self.show_database(db_name)
        for i in range(len(types)):
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, [
                {"name": "c1", "type": "integer"},
                types[i],
            ], None, {
                                  "status_code": 500,
                                  "error_code": 3057,
                              })

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_varchar_with_not_match_definition(self):
        httputils.check_data(TEST_TMP_DIR)
        file_name = "pysdk_test_varchar.csv"
        httputils.copy_data(file_name)
        db_name = "default_db"
        table_name = "test_import_varchar_with_not_match_definition"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)

        self.create_table(db_name, table_name, [
            {"name": "c1", "type": "integer"},
            {"name": "c2", "type": "varchar"}
        ])

        test_csv_dir = TEST_TMP_DIR + file_name
        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir, "file_type": "csv", "header": False, "delimiter": ","
        })

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_10000_columns(self):
        httputils.check_data(TEST_TMP_DIR)
        httputils.generate_big_int_csv(10000, "pysdk_test_big_int.csv")
        httputils.copy_data("pysdk_test_big_int.csv")
        db_name = "default_db"
        table_name = "test_import_10000_columns"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {"name": "c1", "type": "integer"},
            {"name": "c2", "type": "integer"}
        ])
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_big_int.csv"
        assert os.path.exists(test_csv_dir)
        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_table_with_not_matched_columns(self):
        httputils.check_data(TEST_TMP_DIR)
        httputils.copy_data("pysdk_test_commas.csv")
        db_name = "default_db"
        table_name = "test_table_with_not_matched_columns"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {"name": "c1", "type": "integer"},
            {"name": "c2", "type": "integer"},
            {"name": "c3", "type": "integer"},
        ])
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_commas.csv"
        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        }, {
                             "status_code": 500,
                             "error_code": 3037,
                         })

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_with_different_size(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_with_different_size"
        filename = "pysdk_test_import_with_different_size.csv"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {"name": "c1", "type": "integer"},
            {"name": "c2", "type": "varchar"}
        ])
        data_size = [1, 8191, 8192, 8193, 16 * 8192]
        for sz in data_size:
            httputils.generate_big_rows_csv(sz, filename)
            httputils.copy_data(filename)
            test_csv_dir = TEST_TMP_DIR + filename
            self.import_data(db_name, table_name, {
                "file_path": test_csv_dir,
                "file_type": "csv",
                "header": False,
                "delimiter": ","
            })
        self.drop_table(db_name, table_name)
        return

    # @pytest.mark.skip(reason="cost too much time")
    def test_http_import_exceeding_rows(self):
        httputils.check_data(TEST_TMP_DIR)
        httputils.generate_big_rows_csv(1024 * 8192, "pysdk_test_big_varchar_rows.csv")
        httputils.copy_data("pysdk_test_big_varchar_rows.csv")
        db_name = "default_db"
        table_name = "test_import_exceeding_rows"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, [
            {"name": "c1", "type": "integer"},
            {"name": "c2", "type": "varchar"}
        ])
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_big_varchar_rows.csv"
        res = self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        })
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_import_exceeding_columns(self):
        httputils.check_data(TEST_TMP_DIR)
        httputils.generate_big_columns_csv(1024, "pysdk_test_big_columns.csv")
        httputils.copy_data("pysdk_test_big_columns.csv")
        db_name = "default_db"
        table_name = "test_import_exceeding_rows"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        columns = []
        for i in range(1024):
            columns.append({"name": "c" + str(i), "type": "integer"})
        self.create_table(db_name, table_name, columns)
        test_csv_dir = TEST_TMP_DIR + "pysdk_test_big_columns.csv"
        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        })

        self.drop_table(db_name, table_name)
        return
