import re

import requests
import logging
import os
from test_pysdk.common.common_data import *
from infinity.common import ConflictType, InfinityException, SparseVector
from test_pysdk.common import common_values
import infinity
from typing import Optional, Any
from infinity.errors import ErrorCode
from infinity.utils import deprecated_api
import numpy as np
import pandas as pd
import polars as pl
import pyarrow as pa
from infinity.table import ExplainType


class infinity_http:
    url = default_url
    header_dict = baseHeader
    response_dict = baseResponse
    data_dict = baseData

    def disconnect(self):
        print("disconnect")
        return database_result(error_code=ErrorCode.OK)

    def set_up_header(self, param=[], tp={}):
        header = {}
        if len(param) != 0:
            for i in range(len(param)):
                item = self.header_dict.get(param[i])
                if item is None:
                    logging.warning("can't find header param: " + param[i])
                    continue
                header[param[i]] = item
        if len(tp) != 0:
            for item in dict.items():
                header[item[0]] = item[1]

        return header

    def set_up_data(self, param=[], tp={}):
        data = {}
        if len(param) != 0:
            for i in range(len(param)):
                item = self.data_dict.get(param[i], "")
                if len(item) == 0:
                    logging.warning("can't find data param: " + param[i])
                    continue
                data[param[i]] = item
        if len(tp) != 0:
            for item in tp.items():
                data[item[0]] = item[1]
        # logging.debug(data)
        return data
        # Post operation

    def request(self, url, method, header={}, data={}):
        if header is None:
            header = {}
        url = default_url + url
        logging.debug("url: " + url)
        match method:
            case "get":
                response = requests.get(url, headers=header, json=data)
            case "post":
                response = requests.post(url, headers=header, json=data)
            case "put":
                response = requests.put(url, headers=header, json=data)
            case "delete":
                response = requests.delete(url, headers=header, json=data)
        return response

    def raise_exception(self, resp, expect={}):
        logging.debug("status_code:" + str(resp.status_code))
        if expect.get("status_code", None) is not None:
            logging.debug(
                f"expect: {expect['status_code']}, actual: {resp.status_code}"
            )
            assert resp.status_code == expect["status_code"]
        else:
            logging.debug(f"actual: {resp.status_code}, expect: 200")
            if resp.status_code != 200:
                if resp.status_code == 500:
                    resp_json = resp.json()
                    print(500, resp_json["error_code"], resp_json["error_message"])
                    raise InfinityException(resp_json["error_code"], resp_json["error_message"])
                elif resp.status_code == 404:
                    #create_database("") return status_code 404 with no json
                    print(404)
                    raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME)
                else:
                    print(resp.status_code)
                    raise InfinityException()
            else:
                #print(200)
                pass

        logging.debug("----------------------------------------------")
        return

    def get_database_result(self, resp, expect={}):
        try:
            self.raise_exception(resp, expect)
            return database_result(error_code=ErrorCode.OK)
        except InfinityException as e:
            print(e)
            return database_result(error_code=e.error_code)

    # database
    def create_database(self, db_name, opt=ConflictType.Error):
        url = f"databases/{db_name}"
        h = self.set_up_header(["accept", "content-type"])
        if opt in [ConflictType.Error, ConflictType.Ignore, ConflictType.Replace]:
            d = self.set_up_data(
                ["create_option"], {"create_option": baseCreateOptions[opt]}
            )
            r = self.request(url, "post", h, d)
            self.raise_exception(r)
            return True
        else:
            try:
                d = self.set_up_data(
                   ["create_option"], {"create_option": opt}
                )
                r = self.request(url, "post", h, d)
                self.raise_exception(r)
            except:
                raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE)
            #d = self.set_up_data(
            #    ["create_option"], {"create_option": str(opt)}
            #)
            #different exception type
            #<ExceptionInfo InfinityException(3074, 'Invalid create option: 1.1') tblen=3>

    def drop_database(self, db_name, opt=ConflictType.Error):
        url = f"databases/{db_name}"
        h = self.set_up_header(["accept", "content-type"])
        if opt in [ConflictType.Error, ConflictType.Ignore]:
            d = self.set_up_data(["drop_option"], {"drop_option": baseDropOptions[opt]})
            r = self.request(url, "delete", h, d)
            self.raise_exception(r)
            return database_result()
        else:
            try:
                d = self.set_up_data(["drop_option"], {"drop_option": opt})
                r = self.request(url, "delete", h, d)
                self.raise_exception(r)
            except:
                raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE)


    def get_database(self, db_name, opt=ConflictType.Error):
        url = f"databases/{db_name}"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h, {})
        try:
            self.raise_exception(r)
            return database_result(database_name=r.json()["database_name"])
        except:
            raise InfinityException(ErrorCode.DB_NOT_EXIST)

    def list_databases(self):
        url = "databases"
        self.set_up_header(["accept"])
        r = self.request(url, "get")
        self.raise_exception(r)
        return database_result(list = r.json()["databases"])

    def show_database(self,db_name):
        url = f"databases/{db_name}"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h, {})
        self.raise_exception(r)
        return database_result(database_name=r.json()["database_name"])

    # table
    def create_table(
        self,
        table_name,
        columns_definition = {},
        conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if type(conflict_type) != type([]) and type(conflict_type) != type({}) and type(conflict_type) != type(()):
            exists = baseCreateOptions.get(conflict_type, None)
            if exists is not None:
                copt = baseCreateOptions[conflict_type]

        # parser
        try:
            fields = []
            for col in columns_definition:
                tmp = {"name": col}
                for param_name in columns_definition[col]:
                    tmp[param_name.lower()] = columns_definition[col][param_name]
                fields.append(tmp)
        except:
            raise InfinityException(ErrorCode.SYNTAX_ERROR, "http adapter create table parse error")
        print(fields)

        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data(
            ["create_option"],
            {
                "fields": fields,
                "create_option": copt,
            },
        )
        r = self.request(url, "post", h, d)
        self.raise_exception(r)
        self.table_name = table_name
        return database_result(database_name=self.database_name, table_name=self.table_name)


    def drop_table(
        self,
        table_name,
        conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if type(conflict_type) != type([]) and type(conflict_type) != type({}) and type(conflict_type) != type(()):
            exists = baseDropOptions.get(conflict_type, None)
            if exists is not None:
                copt = baseDropOptions[conflict_type]

        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data(["drop_option"], {"drop_option": copt})
        r = self.request(url, "delete", h, d)
        self.raise_exception(r)
        return self


    def list_tables(self):
        url = f"databases/{self.database_name}/tables"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        self.raise_exception(r)
        return self

    def show_table(self, table_name):
        check_valid_name(table_name)
        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        self.raise_exception(r)
        self.table_name = table_name
        return database_result(database_name=self.database_name, table_name=self.table_name)

    def get_all_tables(self):
        url = f"databases/{self.database_name}/tables"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        # return all db names
        ret = []
        r_json = r.json()
        if r_json.get("tables", None) is None:
            return ret
        table_list = (r_json)["tables"]
        for item in table_list:
            ret.append(item)
        return ret

    def get_table(self, table_name):
        return self.show_table(table_name)

    def show_columns(self,table_name):
        url = f"databases/{self.database_name}/tables/{table_name}/columns"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        self.raise_exception(r)
        res = {"column_name":[], "column_type":[], "constraint":[], "default":[]}
        print(r.json())
        for col in r.json()["columns"]:
            res["column_name"].append(col["column_name"])
            res["column_type"].append(col["column_type"])
            res["constraint"].append(col["constraint"])
            res["default"].append(col["default"])
        res = pl.from_pandas(pd.DataFrame(res))
        return res

    def show_columns_type(self,table_name):
        url = f"databases/{self.database_name}/tables/{table_name}/columns"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        self.raise_exception(r)
        res = {}
        for col in r.json()["columns"]:
            res[col["column_name"]] = col["column_type"]
        return res

    # not implemented, just to pass test
    def show_tables(self):
        self.get_all_tables()
        return database_result(columns=["database", "table", "type", "column_count", "block_count", "block_capacity",
                                   "segment_count", "segment_capacity"])

    # index
    def create_index(
        self,
        index_name,
        index_info,
        conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if type(conflict_type) != type([]) and type(conflict_type) != type({}) and type(conflict_type) != type(()):
            exists = baseCreateOptions.get(conflict_type, None)
            if exists is not None:
                copt = baseCreateOptions[conflict_type]

        fields = []
        create_index_info = {}
        fields.append(index_info.column_name)
        create_index_info["type"] = index_type_transfrom[index_info.index_type.value]
        if index_info.params is not None:
            for key, value in index_info.params.items():
                if not isinstance(value, str):
                    raise InfinityException(ErrorCode.INVALID_INDEX_PARAM, f"parameter value: {value} isn't string")
                create_index_info[key] = value
        #print(fields)
        #print(create_index_info)

        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"
        h = self.set_up_header(
            ["accept", "content-type"],
        )
        d = self.set_up_data(
            ["create_option"], {"fields": fields, "index": create_index_info, "create_option": copt}
        )
        r = self.request(url, "post", h, d)
        self.raise_exception(r)
        return self

    def drop_index(
        self,
        index_name,
        conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if type(conflict_type) != type([]) and type(conflict_type) != type({}) and type(conflict_type) != type(()):
            exists = baseDropOptions.get(conflict_type, None)
            if exists is not None:
                copt = baseDropOptions[conflict_type]

        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"

        h = self.set_up_header(["accept"])
        d = self.set_up_data([], {"drop_option": copt})
        r = self.request(url, "delete", h, d)
        self.raise_exception(r)
        return self

    def show_index(self,index_name):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        self.raise_exception(r)
        return self

    def list_indexes(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes"
        h = self.set_up_header(["accept"])
        r = self.request(url, "get", h)
        self.raise_exception(r)
        r_json = r.json()
        index_list = []
        exists = r_json.get("tables", None)
        if exists is not None:
            for t in r_json["tables"]:
                index_list.append(t)
        self.index_list = index_list
        return self

    def optimize(self, index_name = "", optimize_options = {}):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"
        h = self.set_up_header(
            ["accept", "content-type"],
        )
        opt_opt = {"optimize_options": optimize_options}
        r = self.request(url, "put", h, opt_opt)
        self.raise_exception(r)
        return self

    def insert(self,values=[]):
        if isinstance(values, list):
            pass
        else:
            values = [values]

        for value in values:
            if isinstance(value, dict):
                for key in value:
                    if isinstance(value[key], np.ndarray):#trans np array to list since http api can not parse np array
                        value[key] = value[key].tolist()
                    if isinstance(value[key], list):
                        for idx in range(len(value[key])):
                            if isinstance(value[key][idx], np.ndarray):
                                value[key][idx] = value[key][idx].tolist()
                    if isinstance(value[key], SparseVector):
                        value[key] = value[key].to_dict()

        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.set_up_header(["accept", "content-type"])
        r = self.request(url, "post", h, values)
        self.raise_exception(r)
        return self

    def import_data(self,data_path = "/home/infiniflow/Documents/development/infinity/test/data/csv/pysdk_test.csv",
                    import_options = {}):
        data = {}
        data["file_path"] = data_path
        data["file_type"] = "csv"
        data["header"] = False
        data["delimiter"] = ","
        if import_options is not None:
            if "file_type" in import_options:
                data["file_type"] = import_options["file_type"]
            if "header" in import_options:
                data["header"] = import_options["header"]
            if "delimiter" in import_options:
                data["delimiter"] = import_options["delimiter"]

        url = f"databases/{self.database_name}/tables/{self.table_name}"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data([], data)
        r = self.request(url, "put", h, d)
        self.raise_exception(r)
        return self

    def export_data(self,data_path = "", export_options = {}, columns = []):
        data = {}
        data["file_path"] = data_path
        data["file_type"] = "csv"
        data["header"] = False
        data["delimiter"] = ","
        if "file_type" in export_options:
            data["file_type"] = export_options["file_type"]
        if "header" in export_options:
            data["header"] = export_options["header"]
        if "delimiter" in export_options:
            data["delimiter"] = export_options["delimiter"]
        if "offset" in export_options:
            data["offset"] = export_options["offset"]
        if "limit" in export_options:
            data["limit"] = export_options["limit"]
        if "row_limit" in export_options:
            data["row_limit"] = export_options["row_limit"]

        data["columns"] = columns

        url = f"databases/{self.database_name}/table/{self.table_name}"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data([], data)
        r = self.request(url, "get", h, d)
        self.raise_exception(r)
        return self

    def select(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.set_up_header(["accept", "content-type"])
        tmp = {}
        if len(self._filter):
            tmp.update({"filter": self._filter})
        if len(self._search_exprs):
            tmp.update({"search": self._search_exprs})
        if len(self._output):
            tmp.update({"output":self._output})
        #print(tmp)
        d = self.set_up_data([], tmp)
        r = self.request(url, "get", h, d)
        self.raise_exception(r)
        #print(r.json())
        if "output" in r.json():
            self.output_res = r.json()["output"]
        else:
            self.output_res = []
        return self

    def explain(self, ExplainType = ExplainType.Physical):
        url = f"databases/{self.database_name}/tables/{self.table_name}/meta"
        h = self.set_up_header(["accept", "content-type"])
        tmp = {}
        if len(self._filter):
            tmp.update({"filter": self._filter})
        if len(self._fusion):
            tmp.update({"fusion": self._fusion})
        if len(self._knn):
            tmp.update({"knn": self._knn})
        if len(self._match):
            tmp.update({"match": self._match})
        if len(self._match_tensor):
            tmp.update({"match_tensor": self._match_tensor})
        if len(self._match_sparse):
            tmp.update({"match_sparse": self._match_sparse})
        if len(self._output):
            tmp.update({"output": self._output})
        tmp.update({"explain_type":ExplainType_transfrom(ExplainType)})
        # print(tmp)
        d = self.set_up_data([], tmp)
        r = self.request(url, "get", h, d)
        self.raise_exception(r)
        message = ""
        sign = 0
        for res in r.json()["output"]:
            for k in res:
                if sign == 0:
                    message = message + k + "\n"
                    message = message + res[k] + "\n"
                    sign = 1
                else:
                    message = message + res[k] + "\n"
        return message

    def add_columns(self, columns_definition = {}):
        url = f"databases/{self.database_name}/tables/{self.table_name}/columns"
        h = self.set_up_header(["accept", "content-type"])
        fields = []
        for col in columns_definition:
            tmp = {"name": col}
            for param_name in columns_definition[col]:
                tmp[param_name.lower()] = columns_definition[col][param_name]
            fields.append(tmp)
        d = self.set_up_data([], {"fields": fields})
        r = self.request(url, "put", h, d)
        return self.get_database_result(r)

    def drop_columns(self, column_name: list[str] | str):
        if isinstance(column_name, str):
            column_name = [column_name]
        url = f"databases/{self.database_name}/tables/{self.table_name}/columns"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data([], {"column_names": column_name})
        r = self.request(url, "delete", h, d)
        return self.get_database_result(r)

    def output(
        self,
        output=[],
    ):
        self.output_res = []
        self._output = output
        self._filter = ""
        self._search_exprs = []
        return self

    def match_text(self, fields: str, query: str, topn: int, opt_params: Optional[dict] = None):
        tmp_match_expr = {"match_method": "text", "fields": fields, "matching_text": query, "topn": topn}
        if opt_params is not None:
            tmp_match_expr["params"] = opt_params
        self._search_exprs.append(tmp_match_expr)
        return self

    def match(self, *args, **kwargs):
        deprecated_api("match is deprecated, please use match_text instead")
        return self.match_text(*args, **kwargs)

    def match_tensor(self, column_name: str, query_data, query_data_type: str, topn: int,
                     extra_option: Optional[dict] = None):
        tmp_match_tensor = {"match_method": "tensor", "fields": column_name, "query_tensor": query_data,
                            "element_type": query_data_type, "topn": topn}
        if extra_option is not None:
            tmp_match_tensor["params"] = extra_option
        self._search_exprs.append(tmp_match_tensor)
        return self

    def match_sparse(self, vector_column_name: str, sparse_data: SparseVector | dict, distance_type: str, topn: int,
                     opt_params: Optional[dict] = None):
        tmp_match_sparse = {"match_method": "sparse", "fields": vector_column_name,
                            "query_vector": sparse_data.to_dict(), "metric_type": distance_type, "topn": topn}
        if opt_params is not None:
            tmp_match_sparse["params"] = opt_params
        self._search_exprs.append(tmp_match_sparse)
        return self

    def filter(self, filter_expr):
        self._filter = filter_expr
        return self

    def match_dense(self, fields: str, query_vector: list, element_type: str, metric_type: str, top_k: int,
                    opt_params: Optional[dict] = None):
        tmp_match_dense = {"match_method": "dense", "fields": fields, "query_vector": query_vector,
                           "element_type": element_type, "metric_type": metric_type, "topn": top_k}
        if opt_params is not None:
            tmp_match_dense["params"] = opt_params
        self._search_exprs.append(tmp_match_dense)
        return self

    def knn(self, *args, **kwargs):
        deprecated_api("knn is deprecated, please use match_dense instead")
        return self.match_dense(*args, **kwargs)

    def fusion(self, method: str, topn: int, fusion_params: Optional[dict] = None):
        tmp_fusion_expr = {"fusion_method": method, "topn": topn}
        if method == "match_tensor":
            tmp_new_params = {"fields": fusion_params["field"], "query_tensor": fusion_params["data"],
                              "element_type": fusion_params["data_type"]}
            # handle left params
            fusion_params.pop("field")
            fusion_params.pop("data")
            fusion_params.pop("data_type")
            tmp_new_params.update(fusion_params)
            tmp_fusion_expr["params"] = tmp_new_params
        else:
            if fusion_params is not None:
                tmp_fusion_expr["params"] = fusion_params
        self._search_exprs.append(tmp_fusion_expr)
        return self

    def to_result(self):
        self.select()

    def to_pl(self):
        return pl.from_pandas(self.to_df())

    def to_df(self):
        if self.output_res == []:
            self.select()

        df_dict = {}
        col_types = self.show_columns_type(self.table_name)
        for output_col in self._output:
            if output_col in col_types:
                df_dict[output_col] = ()
        #when output["*"] and output_res is empty
        for output_col in self._output:
            if output_col == "*":
                for col in col_types:
                    df_dict[col] = ()

        for res in self.output_res:
            for k in res:
                if k not in df_dict:
                    df_dict[k] = ()
                tup = df_dict[k]
                if res[k].isdigit() or is_float(res[k]):
                    new_tup = tup + (eval(res[k]), )
                elif is_list(res[k]):
                    new_tup = tup + (ast.literal_eval(res[k]), )
                elif is_sparse(res[k]):# sparse vector
                    sparse_vec = str2sparse(res[k])
                    new_tup = tup + (sparse_vec, )
                else:
                    if res[k].lower() == 'true':
                        res[k] = True
                    elif res[k].lower() == 'false':
                        res[k] = False
                    new_tup = tup + (res[k],)
                df_dict[k] = new_tup
        #print(self.output_res)
        #print(df_dict)

        df_type = {}
        for k in df_dict:
            if k in col_types:# might be object
                df_type[k] = type_to_dtype(col_types[k])
            if k in ["DISTANCE", "SCORE", "SIMILARITY"]:
                df_type[k] = dtype('float32')
            #"(c1 + c2)"
            k1 = k.replace("(", "")
            k1 = k1.replace(")", "")
            cols = k1.split("+") + k1.split("-") #["c1 ", " c2", "c1 + c2"]
            #print(cols)
            #haven't considered data type priority
            for col in cols:
                if col.strip() in col_types:
                    df_type[k] = type_to_dtype(col_types[col.strip()])
                if col.strip().isdigit():
                    df_type[k] = dtype('int32')
                if is_float(col.strip()):
                    df_type[k] = dtype('float64')
        return pd.DataFrame(df_dict).astype(df_type)

    def to_arrow(self):
        return pa.Table.from_pandas(self.to_df())

    def delete(self,filter=""):
        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data([], {"filter": filter})
        r = self.request(url, "delete", h, d)
        self.raise_exception(r)
        return self

    def update(self, filter_str: str, update: dict[str, Any]):
        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.set_up_header(["accept", "content-type"])
        d = self.set_up_data([], {"update": update, "filter": filter_str})
        r = self.request(url, "put", h, d)
        self.raise_exception(r)
        return self


class database_result(infinity_http):
    def __init__(self, list = [], error_code = ErrorCode.OK, database_name = "" ,columns=[], table_name = "",
                 index_list = [], output = ["*"], filter="", fusion=[], knn={}, match = {}, match_tensor = {}, match_sparse = {}, output_res = []):
        self.db_names = list
        self.error_code = error_code
        self.database_name = database_name # get database
        self._db_name = database_name
        self.columns = columns
        self.table_name = table_name
        self.index_list = index_list
        self._output = output
        self._filter = filter
        self._fusion = fusion
        self._knn = knn
        self._match = match
        self._match_tensor = match_tensor
        self._match_sparse = match_sparse
        self.output_res = output_res



identifier_limit = 65536
def check_valid_name(name, name_type: str = "Table"):
    if not isinstance(name, str):
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name must be a string, got {type(name)}")
    if not re.match(r"^[a-zA-Z][a-zA-Z0-9_]*$", name):
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name '{name}' is not valid. It should start with a letter and can contain only letters, numbers and underscores")
    if len(name) > identifier_limit:
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name '{name}' is not of appropriate length")
    if name is None:
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name.isspace():
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"{name_type} name cannot be composed of whitespace characters only")
    if name == '':
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name == ' ':
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name.isdigit():
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")


