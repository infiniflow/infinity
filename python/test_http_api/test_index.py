import sys
import pytest
from httpapibase import HttpTest
from common.common_values import *
import infinity.index as index
import pandas
import os
import time
import httputils


class TestIndex(HttpTest):
    def test_http_create_index_IVFFlat(self):
        db_name = "default_db"
        table_name = "test_create_index_IVFFlat"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "c1": {
                    "type": "vector",
                    "dimension": 1024,
                    "element_type": "float",
                }
            }
        )

        self.create_index(db_name, table_name, idxname,
                          [
                              "c1",
                          ],
                          {
                              "type": "IVFFlat",
                              "centroids_count": "128",
                              "metric": "l2",
                          })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_create_index_HNSW(self):
        db_name = "default_db"
        table_name = "test_create_index_HMSW"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "c1": {
                    "type": "vector",
                    "dimension": 1024,
                    "element_type": "float",
                }
            }
        )
        self.create_index(db_name, table_name, idxname,
                          [
                              "c1",
                          ],
                          {
                              "type": "HNSW",
                              "M": "16",
                              "ef_construction": "50",
                              "ef": "50",
                              "metric": "l2"
                          })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_create_index_fulltext(self):
        db_name = "default_db"
        table_name = "test_create_index_fulltext"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "doctitle": {
                    "type": "varchar",
                },
                "docdate": {
                    "type": "varchar",
                },
                "body": {
                    "type": "varchar",
                }
            }
        )
        self.create_index(db_name, table_name, idxname,
        ["body",],{
            "type": "FULLTEXT",
        })

        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_drop_non_existent_index(self):
        db_name = "default_db"
        table_name = "test_drop_non_existent_index"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "doctitle": {
                    "type": "varchar",
                }
            }
        )
        self.drop_index(db_name, table_name, idxname, {
            "status_code": 500,
            "error_code": 3023,
        },"kError")
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_create_created_index(self):
        db_name = "default_db"
        table_name = "test_create_index_fulltext"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {"doctitle": {"type": "varchar", }, "docdate": {"type": "varchar", }, "body": {"type": "varchar", }}
        )
        self.create_index(db_name, table_name, idxname, ["body", ], {"type": "FULLTEXT", })
        self.create_index(db_name, table_name, idxname, ["body", ], {"type": "FULLTEXT", })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # create / drop index with invalid options
    @pytest.mark.parametrize("column_name",
                             [(1, False), (2.2, False), ((1, 2), False), ([1, 2, 3], False), ("c1", True)])
    @pytest.mark.parametrize("index_type", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        (index.IndexType.Hnsw, False),
        (index.IndexType.IVFFlat, True)
    ])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False), ((1, 2), False), ({"1": 2}, False)
    ])
    def test_http_create_drop_vector_index_invalid_options(self, column_name, index_type, params):
        db_name = "default_db"
        table_name = "test_create_drop_vector_index_invalid_options"
        idxname = "my_index"

        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "c1": {
                    "type": "vector",
                    "dimension": 3,
                    "element_type": "float",
                },
            }
        )
        if not column_name[1] or not index_type[1] or not params[1]:
            self.create_index(
                db_name,
                table_name,
                idxname,
                [str(column_name[0])],
                {
                    "type": str(index_type[0]),
                    "centroids_count": 128,
                    "metric": "l2",
                },
                {
                    "status_code": 500,
                    "error_code": 3060,
                }
            )
        else:
            self.create_index(
                db_name,
                table_name,
                idxname,
                [str(column_name[0])],
                {
                    "type": str(index_type[0]),
                    "centroids_count": 128,
                    "metric": "l2",
                },
            )
        return

    @pytest.mark.parametrize("column_name", [
        (1, False),
        (2.2, False),
        ([1, 2], False),
        ("$#%dfva", False),
        ((1, 2), False),
        ({"1": 2}, False),
        ("c1", True)])
    @pytest.mark.parametrize("index_type", ["FullText"])
    @pytest.mark.parametrize("params", [
        (1, False), (2.2, False), ([1, 2], False), ("$#%dfva", False),
        ((1, 2), False), ({"1": 2}, False), ([], True)
    ])
    @pytest.mark.parametrize("types",
                             [{"type": "integer"}, {"type": "tinyint"}, {"type": "smallint"}, {"type": "bigint"},
                              {"type": "float"},
                              {"type": "double"}, {"type": "varchar"}, {"type": "boolean"},
                              {"type": "vector", "dimension": 3, "element_type": "float", }])
    def test_http_create_drop_different_fulltext_index_invalid_options(self, column_name, index_type, params, types):
        db_name = "default_db"
        table_name = "test_create_drop_different_fulltext_index_invalid_options"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "c1": types
            }
        )
        if types != "varchar" or not column_name[1] or not index_type[1] or not params[1]:
            self.create_index(db_name, table_name, idxname,
                              [str(column_name[0])],
                              {
                                  "type": str(index_type[0]),
                              },
                              {
                                  "status_code": 500,
                                  "error_code": 3060,
                              }
                              )
        else:
            self.create_index(db_name, table_name, idxname,
                              [str(column_name[0])],
                              {
                                  "type": str(index_type[0]),
                              },
                              )

        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_create_index_on_dropped_table(self):
        db_name = "default_db"
        table_name = "test_create_index_on_dropped_table"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name, table_name, {
                "c1": {
                    "type": "vector",
                    "dimension": 3,
                    "element_type": "float",
                }

            }
        )
        self.drop_table(db_name, table_name)
        self.create_index(db_name, table_name, idxname,
                          ["c1", ], {
                              "type": "IVFFlat",
                              "centroids_count": 128,
                              "metric": "l2",
                          }, {
                              "status_code": 500,
                              "error_code": 3022,
                          })
        return

    # PASS
    def test_http_create_index_show_index(self):
        db_name = "default_db"
        table_name = "test_create_index_show_index"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name, table_name, {
                "c1": {"type": "vector", "dimension": 3, "element_type": "float", }}
        )
        self.create_index(db_name, table_name, idxname,
                          ["c1", ], {"type": "IVFFlat", "centroids_count": 128, "metric": "l2",
                                     })
        self.show_index(db_name, table_name, idxname, expect={
            "error_code": 0,
            "database_name": db_name,
            "table_name": table_name,
            "index_name": idxname,
            "index_type": "IVFFlat",
            "other_parameters": "metric = l2, centroids_count = 128",
        })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    def test_http_drop_index_show_index(self):
        db_name = "default_db"
        table_name = "test_drop_index_show_index"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "vector", "dimension": 3, "element_type": "float", }}
                          )
        self.create_index(db_name, table_name, idxname,
                          ["c1", ], {"type": "IVFFlat", "centroids_count": 128, "metric": "l2",
                                     })
        self.show_index(db_name, table_name, idxname, expect={
            "error_code": 0,
            "database_name": db_name,
            "table_name": table_name,
            "index_name": idxname,
            "index_type": "IVFFlat",
            "other_parameters": "metric = l2, centroids_count = 128",
        })
        tlist = self.list_index(db_name, table_name, expect={
            "error_code": 0
        })
        assert len(tlist) == 0

        self.drop_index(db_name, table_name, idxname)
        self.drop_index(db_name, table_name, idxname, {
            "status_code": 500,
            "error_code": 3023,
        },"kError")

        self.drop_table(db_name, table_name)
        return

    # create index on different type of column and show index
    @pytest.mark.parametrize("types", [{"type": "vector", "dimension": 3, "element_type": "float"}])
    @pytest.mark.parametrize("index_type", [
        ("Hnsw", False, "ERROR:3061*"),
        ("IVFFlat", True),
        ("FullText", False, "ERROR:3009*")
    ])
    def test_http_create_index_on_different_type_of_column(self, types, index_type):
        db_name = "default_db"
        table_name = "test_create_index_on_different_type_of_column"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(
            db_name,
            table_name,
            {
                "c1": types
            }
        )
        if not index_type[1]:
            self.create_index(db_name, table_name, idxname,
                              ["c1"],
                              {
                                  "type": str(index_type[0]),
                                  "centroids_count": "128",
                                  "metric": "l2",
                              },
                              {
                                  "status_code": 500,
                                  "error_code": 3060,
                              }
                              )
        else:
            self.create_index(db_name, table_name, idxname,
                              ["c1"],
                              {
                                  "type": str(index_type[0]),
                                  "centroids_count": "128",
                                  "metric": "l2",
                              },
                              )
            return
        return

    @pytest.mark.parametrize("index_type", [
        "IVFFlat"
    ])
    def test_http_insert_data_create_index(self, index_type):
        db_name = "default_db"
        table_name = "test_insert_data_create_index"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector", "dimension": 1024, "element_type": "float",
            }}
                          )
        values = [{"c1": [1.1 for _ in range(1024)]}]
        self.insert(db_name, table_name, values)
        self.create_index(
            db_name, table_name, idxname,
            ["c1"], {"type": index_type, "centroids_count": "128", "metric": "l2", }
        )
        return

    def test_http_import_data_create_index(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_import_data_create_index"
        idxname = "my_index"
        index_type = ["IVFFlat", "FullText"]
        for t in index_type:
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, {
                "c1": {"type": "integer"},
                "c2": {"type": "vector", "dimension": 3, "element_type": "float", }
            })
            httputils.copy_data("embedding_int_dim3.csv")
            test_csv_dir = TEST_TMP_DIR + "embedding_int_dim3.csv"
            assert os.path.exists(test_csv_dir)

            self.import_data(db_name, table_name, {
                "file_path": test_csv_dir,
                "file_type": "csv",
                "header": False,
                "delimiter": ","
            })
            if t == "IVFFlat":
                self.create_index(db_name, table_name, idxname, ["c2"], {
                    "type": "IVFFlat",
                    "centroids_count": "128",
                    "metric": "l2"
                })
                continue
        self.drop_table(db_name, table_name)
        return

    # ERROR: IVFFlat realtime index is not supported yet
    @pytest.mark.skip(reason="IVFFlat realtime index is not supported yet")
    def test_http_create_vector_index_import_data(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_create_vector_index_import_data"
        idxname = "my_index"

        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {"type": "integer"},
            "c2": {"type": "vector", "dimension": 4, "element_type": "float", }
        })
        httputils.copy_data("embedding_float_dim4.csv")
        test_csv_dir = TEST_TMP_DIR + "embedding_float_dim4.csv"
        assert os.path.exists(test_csv_dir)
        # ERROR
        self.create_index(db_name, table_name, idxname, ["c2"], {
            "type": "IVFFlat",
            "centroids_count": "128",
            "metric": "l2"
        })
        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": ","
        })
        return

    # ERROR: IVFFlat realtime index is not supported yet
    @pytest.mark.skip(reason="IVFFlat realtime index is not supported yet")
    def test_http_create_index_import_data(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_create_index_import_data"
        idxname = "my_index"
        index_type = ["IVFFlat", "FullText"]
        for t in index_type:
            self.drop_table(db_name, table_name)
            self.create_table(db_name, table_name, {
                "c1": {"type": "integer"},
                "c2": {"type": "vector", "dimension": 3, "element_type": "float", }
            })
            httputils.copy_data("embedding_int_dim3.csv")
            test_csv_dir = TEST_TMP_DIR + "embedding_int_dim3.csv"
            assert os.path.exists(test_csv_dir)
            # ERROR
            if t == "IVFFlat":
                self.create_index(db_name, table_name, idxname, ["c2"], {
                    "type": "IVFFlat",
                    "centroids_count": "128",
                    "metric": "l2"
                })
            else:
                # self.createIdx(db_name,table_name,idxname,["c2"],{
                #     "type":"FullText",
                # })
                continue
            self.import_data(db_name, table_name, {
                "file_path": test_csv_dir,
                "file_type": "csv",
                "header": False,
                "delimiter": ","
            })
        return

    # ERROR: IVFFlat realtime index is not supported yet
    def test_http_insert_data_fulltext_index_search(self):
        httputils.check_data(TEST_TMP_DIR)
        httputils.copy_data("enwiki_99.csv")
        db_name = "default_db"
        table_name = "test_insert_data_fulltext_index_search"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "doctitle": {"type": "varchar"},
            "docdate": {"type": "varchar"},
            "body": {"type": "varchar"},
        })
        test_csv_dir = TEST_TMP_DIR + "enwiki_99.csv"
        print("test_csv_dir: " + test_csv_dir)
        assert os.path.exists(test_csv_dir)

        self.create_index(db_name, table_name, idxname, ["body"], {
            "type": "FULLTEXT",
        })

        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": "\t"
        })
        self.drop_table(db_name, table_name)

        return

    def test_http_fulltext_match_with_invalid_analyzer(self):
        httputils.check_data(TEST_TMP_DIR)
        db_name = "default_db"
        table_name = "test_fulltext_match_with_invalid_analyzer"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "doctitle": {"type": "varchar"},
            "docdate": {"type": "varchar"},
            "body": {"type": "varchar"},
        })
        httputils.copy_data("enwiki_9.csv")
        test_csv_dir = TEST_TMP_DIR + "enwiki_9.csv"
        print("test_csv_dir: " + test_csv_dir)
        assert os.path.exists(test_csv_dir)

        self.import_data(db_name, table_name, {
            "file_path": test_csv_dir,
            "file_type": "csv",
            "header": False,
            "delimiter": "\t"
        })

        self.create_index(db_name, table_name, idxname, ["body"], {
            "type": "FULLTEXT",
            "ANALYZER": "segmentation"
        }, {
            "status_code": 500,
        })

        self.select(db_name, table_name, ["*"], "", {
        }, {}, {
            "error_code": 0,
        })
        self.drop_table(db_name, table_name)
        return
        # ERROR

    def test_http_create_index_on_deleted_table(self):
        db_name = "default_db"
        table_name = "test_create_index_on_deleted_table"
        idxname = "my_index"
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 128,
                "element_type": "float"
            }
        })
        embedding_data = [float(i) for i in range(128)]
        value = [{"c1": embedding_data} for _ in range(1024)]
        self.insert(db_name, table_name, value)

        self.drop_table(db_name, table_name)
        self.select(db_name, table_name, ["*"], "", {
        }, {}, {
                        "status_code": 500,
                        "error_code": 3022,
                    })
        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "IVFFlat",
            "centroids_count": "128",
            "metric": "l2"
        }, {
            "status_code": 500,
            "error_code": 3022
        })
        return

    # ERROR update error
    @pytest.mark.skip
    #@pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    #@pytest.mark.xfail(reason="Not support to convert Embedding to Embedding")
    def test_http_create_index_on_update_table(self):
        db_name = "default_db"
        table_name = "test_create_index_on_update_table"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 4,
                "element_type": "float"
            },
            "c2": {
                "type": "integer",
            }
        })
        embedding_data = [float(i) for i in range(4)]
        for i in range(10):
            self.insert(db_name, table_name, [{"c1": embedding_data, "c2": i}])

        embedding_data = [(float(i) + 0.1) for i in range(4)]
    
    
        for i in range(10):
            self.update(db_name, table_name, {"c1": embedding_data}, "c2 = " + str(i))
        self.drop_table(db_name, table_name)

        self.select(db_name, table_name, ["c1", "c2"], "", {}, {}, {
            "status_code": 500,
        })
        return

    # PASS
    def test_http_create_index_with_valid_options(self):
        db_name = "default_db"
        table_name = "test_create_index_with_valid_options"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 4,
                "element_type": "float"
            },
            "c2": {
                "type": "integer",
            }
        })
        idx_option = ["error", "ignore_if_exists"]
        for opt in idx_option:
            self.create_index(db_name, table_name, idxname, ["c1"], {
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": "l2"
            }, {"error_code": 0}, opt)
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_create_index_with_invalid_options(self):
        db_name = "default_db"
        table_name = "test_http_create_index_with_invalid_options"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 4,
                "element_type": "float"
            },
        })
        idx_option = [1.1, "#@$@!%string", [], {}, ()]
        i = 0
        for opt in idx_option:
            if i == 0:
                self.create_index(db_name, table_name, idxname, ["c1"], {
                    "type": "HNSW",
                    "M": "16",
                    "ef_construction": "50",
                    "ef": "50",
                    "metric": "l2"
                }, {"error_code": 0}, str(opt))
            else:
                self.create_index(db_name, table_name, idxname, ["c1"], {
                    "type": "HNSW",
                    "M": "16",
                    "ef_construction": "50",
                    "ef": "50",
                    "metric": "l2"
                }, {"status_code": 500}, str(opt))
            i += 1
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_create_duplicated_index_with_valid_options(self):
        db_name = "default_db"
        table_name = "test_create_duplicated_index_with_valid_options"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float"
            },
        })
        for _ in range(10):
            self.create_index(db_name, table_name, idxname, ["c1"], {
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": "l2"
            })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    # PASS
    def test_http_create_duplicated_index_with_valid_error_options(self):
        db_name = "default_db"
        table_name = "test_create_duplicated_index_with_valid_error_options"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float"
            },
        })

        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        }, {
            "error_code": 0
        }, "kError")

        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        }, {
            "status_code": 500,
            "error_code": 3018
        }, "kReplace")
          
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return
        # PASS

    def test_http_show_index(self):
        db_name = "default_db"
        table_name = "test_show_index"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        for i in range(5):
            self.create_index(db_name, table_name, idxname + str(i), ["c1"], {
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": "l2"
            })
            self.show_index(db_name, table_name, idxname + str(i), {
                "error_code": 0,
            })
            self.drop_index(db_name, table_name, idxname + str(i))
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_show_valid_name_index(self):
        db_name = "default_db"
        table_name = "test_show_various_name_index"
        idxname = "my_index"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })
        self.show_index(db_name, table_name, idxname, {
            "error_code": 0,
        })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_show_invalid_name_index(self):
        db_name = "default_db"
        table_name = "test_show_various_name_index"
        idxname = "my_idx"
        idx_name_list = ["*Invalid name", "not_exist_name", 1, 1.1, True, [], (), {}]

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })

        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })
        for name in idx_name_list:
            self.show_index(db_name, table_name, name, {
                "status_code": 500,
                "error_code": 3023,
            })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return
    # PASS
    # PASS
    def test_http_list_index(self):
        db_name = "default_db"
        table_name = "test_list_index"
        idxname = "my_idx"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        for i in range(10):
            self.create_index(db_name, table_name, idxname + str(i), ["c1"], {
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": "l2"
            })
        self.list_index(db_name, table_name, {
            "error_code": 0,
        })
        for i in range(10):
            self.drop_index(db_name, table_name, idxname + str(i))

        self.drop_table(db_name, table_name)
        return

    # PASS (drop options doesn't need option: ignore ?)
    def test_http_drop_index_with_valid_options(self):
        db_name = "default_db"
        table_name = "test_drop_index_with_valid_options"
        idxname = "my_idx"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })
        self.drop_index(db_name, table_name, idxname, {
            "error_code": 0,
        }, "kError")
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return
    
    def test_http_drop_index_with_invalid_options(self):
        db_name = "default_db"
        table_name = "test_http_drop_index_with_invalid_options"
        idx_name = "my_idx"
        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        self.create_index(db_name, table_name, idx_name, ["c1"], {
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": "l2"
        })

        self.drop_index(db_name,table_name,idx_name)

        conflict_type = [1.1,"$#$$$$",[],{},()]
        for ctype in conflict_type:
            self.drop_index(db_name,table_name,idx_name,{
                "status_code":500,
                "error_code":3023,
            },str(ctype))
        self.drop_table(db_name,table_name)
        return

    # PASS
    # PASS
    def test_http_supported_vector_index(self):
        db_name = "default_db"
        table_name = "test_drop_index_with_invalid_options"
        idxname = "my_idx"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        
        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "l2"
        })
        self.create_index(db_name, table_name, idxname, ["c1"], {
            "type": "HNSW",
            "M": "16",
            "ef_construction": "50",
            "ef": "50",
            "metric": "ip"
        })
        self.drop_index(db_name, table_name, idxname)
        self.drop_table(db_name, table_name)
        return

    # PASS
    def test_http_unsupported_vector_index(self):
        db_name = "default_db"
        table_name = "test_http_unsupported_vector_index"
        idxname = "my_idx"

        self.show_database(db_name)
        self.drop_table(db_name, table_name)
        self.create_table(db_name, table_name, {
            "c1": {
                "type": "vector",
                "dimension": 1024,
                "element_type": "float",
            },
        })
        self.drop_index(db_name,table_name,idxname)
        
        index_distance_type = ["cosine", "hamming"]
        for t in index_distance_type:
            self.create_index(db_name, table_name, idxname, ["c1"], {
                "type": "HNSW",
                "M": "16",
                "ef_construction": "50",
                "ef": "50",
                "metric": t
            }, {
                "status_code": 500,
                "error_code": 3061,
            })

        self.drop_table(db_name, table_name)
        return
