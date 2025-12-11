import re
import time

import requests
import logging
import ast
from numpy import dtype
from .http_utils import (
    baseHeader, baseResponse, baseData, default_url, baseCreateOptions, baseDropOptions,
    index_type_transfrom, ExplainType_transfrom, is_list, is_date, is_time, is_datetime,
    is_sparse, str2sparse, type_to_dtype, function_return_type, is_float, functions, bool_functions
)
from infinity.common import ConflictType, InfinityException, SparseVector, SortType, FDE
from typing import Optional, Any
from infinity.errors import ErrorCode
from infinity.utils import deprecated_api
import numpy as np
import pandas as pd
import polars as pl
import pyarrow as pa
from infinity.table import ExplainType
from typing import List
from dataclasses import dataclass
from collections import namedtuple


class http_network_util:
    header_dict = baseHeader
    response_dict = baseResponse
    data_dict = baseData

    def __init__(self, url: str = default_url):
        self.base_url = url
        self.retry = False

    def set_retry(self, retry: bool = True):
        self.retry = retry
        if (self.retry):
            self.retry_time = 100
            self.retry_sleep = 0.5

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
        url = self.base_url + url
        logging.debug("url: " + url)
        if self.retry:
            for i in range(self.retry_time):
                try:
                    response = self.request_inner(url, method, header, data)
                    self.raise_exception(response)
                except InfinityException as e:
                    if e.error_code == ErrorCode.INFINITY_IS_INITING:
                        pass
                    else:
                        raise e
                else:
                    break
                logging.debug(f"retry {i} times")
                time.sleep(self.retry_sleep)
            else:
                raise Exception(f"Cannot connect to {url} after {self.retry_time} retries")
            return response
        return self.request_inner(url, method, header, data)

    def request_inner(self, url, method, header={}, data={}):
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
                    if "error_msg" in resp_json:
                        print(500, resp_json["error_code"], resp_json["error_msg"])
                        raise InfinityException(resp_json["error_code"], resp_json["error_msg"])
                    else:
                        print(500, resp_json["error_code"])
                        raise InfinityException(resp_json["error_code"], "")
                elif resp.status_code == 404:
                    # create_database("") return status_code 404 with no json
                    print(404)
                    raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME)
                else:
                    print(resp.status_code)
                    raise InfinityException()
            else:
                # print(200)
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


class infinity_http:
    def __init__(self, *, net: http_network_util = None):
        if net is not None:
            self.net = net
        else:
            self.net = http_network_util(default_url)

    def disconnect(self):
        print("disconnect")
        return database_result()

    def set_role_standalone(self, node_name):
        url = "admin/node/current"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"role": "standalone", "name": node_name})
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)

    def set_role_admin(self):
        url = "admin/node/current"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"role": "admin"})
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)

    def set_role_leader(self, node_name):
        url = "admin/node/current"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"role": "leader", "name": node_name})
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def set_role_follower(self, node_name, leader_addr):
        url = "admin/node/current"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"role": "follower", "name": node_name, "address": leader_addr})
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def set_role_learner(self, node_name, leader_addr):
        url = "admin/node/current"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"role": "learner", "name": node_name, "address": leader_addr})
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    # database
    def create_database(self, db_name, opt=ConflictType.Error):
        url = f"databases/{db_name}"
        h = self.net.set_up_header(["accept", "content-type"])
        if opt in [ConflictType.Error, ConflictType.Ignore, ConflictType.Replace]:
            d = self.net.set_up_data(
                ["create_option"], {"create_option": baseCreateOptions[opt]}
            )
            r = self.net.request(url, "post", h, d)
            self.net.raise_exception(r)
            return True
        else:
            try:
                d = self.net.set_up_data(["create_option"], {"create_option": opt})
                r = self.net.request(url, "post", h, d)
                self.net.raise_exception(r)
            except Exception:
                raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE)
            # d = self.net.set_up_data(
            #    ["create_option"], {"create_option": str(opt)}
            # )
            # different exception type
            # <ExceptionInfo InfinityException(3074, 'Invalid create option: 1.1') tblen=3>

    def drop_database(self, db_name, opt=ConflictType.Error):
        url = f"databases/{db_name}"
        h = self.net.set_up_header(["accept", "content-type"])
        if opt in [ConflictType.Error, ConflictType.Ignore]:
            d = self.net.set_up_data(
                ["drop_option"], {"drop_option": baseDropOptions[opt]}
            )
            r = self.net.request(url, "delete", h, d)
            self.net.raise_exception(r)
            return database_result()
        else:
            try:
                d = self.net.set_up_data(["drop_option"], {"drop_option": opt})
                r = self.net.request(url, "delete", h, d)
                self.net.raise_exception(r)
            except Exception:
                raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE)

    def get_database(self, db_name, opt=ConflictType.Error):
        url = f"databases/{db_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        try:
            self.net.raise_exception(r)
            return database_http(self.net, database_name=r.json()["database_name"])
        except Exception:
            raise InfinityException(ErrorCode.DB_NOT_EXIST)

    def list_databases(self):
        url = "databases"
        self.net.set_up_header(["accept"])
        r = self.net.request(url, "get")
        self.net.raise_exception(r)
        return database_result(list=r.json()["databases"])

    def show_database(self, db_name):
        url = f"databases/{db_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(database_name=r.json()["database_name"])

    def show_nodes(self):
        url = "admin/nodes"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(nodes=r.json()["nodes"])

    def remove_node(self, node_name):
        url = f"admin/node/{node_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "delete", h, {})
        self.net.raise_exception(r)
        return database_result()

    def show_node(self, node_name):
        url = f"admin/node/{node_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        # print(r.json())
        return database_result(node_name=r.json()["node"]["name"], node_role=r.json()["node"]["role"],
                               node_status=r.json()["node"]["status"])

    def show_current_node(self):
        url = "admin/node/current"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        # print(r.json())
        return database_result(node_role=r.json()["node"]["role"], node_status=r.json()["node"]["status"])

    def show_global_variables(self):
        url = "variables/global"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return r.json()

    def show_global_variable(self, var_name: str):
        url = f"variables/global/{var_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return r.json()

    def set_config(self, config_name: str, config_value: any):
        url = "configs"
        config = dict({config_name: config_value})
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], config)
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def show_config(self, config_name: str):
        url = f"configs/{config_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(config_value=r.json()[config_name])

    def show_admin_variables(self):
        url = "admin/variables"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(data=r.json()["role"])

    def show_admin_configs(self):
        url = "admin/configs"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(data=r.json())

    def show_admin_catalogs(self):
        url = "admin/catalogs"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(data=r.json())

    def show_admin_logs(self):
        url = "admin/logs"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(data=r.json())

    def list_nodes(self):
        url = "admin/nodes"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h, {})
        self.net.raise_exception(r)
        return database_result(data=r.json()["nodes"])

    def list_snapshots(self):
        """List all snapshots via HTTP API"""
        url = "snapshots"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        response_data = r.json()

        # Extract snapshots from response
        snapshots = []
        if "snapshots" in response_data:
            for snapshot_info in response_data["snapshots"]:
                # Create a simple object with name attribute
                class SnapshotInfo:
                    def __init__(self, name):
                        self.name = name

                snapshots.append(SnapshotInfo(snapshot_info.get("name", "")))

        return database_result(data=response_data, snapshots=snapshots)

    def show_snapshot(self, snapshot_name):
        """Show details of a specific snapshot via HTTP API"""
        url = f"snapshots/{snapshot_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        return database_result(data=r.json())

    def create_database_snapshot(self, db_name, snapshot_name):
        """Create a snapshot of an entire database"""
        url = "snapshots/database"
        h = self.net.set_up_header(["accept", "content-type"])
        d = {
            "db_name": db_name,
            "snapshot_name": snapshot_name,
        }
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def restore_database_snapshot(self, snapshot_name):
        """Restore a database from a snapshot"""
        url = "snapshots/database/restore"
        h = self.net.set_up_header(["accept", "content-type"])
        d = {"snapshot_name": snapshot_name}
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def create_system_snapshot(self, snapshot_name):
        """Create a system-wide snapshot"""
        url = "snapshots/system"
        h = self.net.set_up_header(["accept", "content-type"])
        d = {"snapshot_name": snapshot_name}
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def restore_system_snapshot(self, snapshot_name):
        """Restore system from a snapshot"""
        url = "snapshots/system/restore"
        h = self.net.set_up_header(["accept", "content-type"])
        d = {"snapshot_name": snapshot_name}
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def drop_snapshot(self, snapshot_name):
        """Drop a snapshot"""
        url = f"snapshots/{snapshot_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "delete", h)
        self.net.raise_exception(r)
        return database_result()


####################3####################3####################3####################3####################3####################3####################3####################3

class database_http:
    def __init__(self, net: http_network_util, database_name: str):
        self.net = net
        self.database_name = database_name
        self._db_name = database_name

    # table
    def create_table(
            self,
            table_name,
            columns_definition={},
            conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if not isinstance(conflict_type, (list, dict, tuple)):
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
        except Exception:
            raise InfinityException(ErrorCode.SYNTAX_ERROR, "http adapter create table parse error")
        # print(fields)

        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data(
            ["create_option"],
            {
                "fields": fields,
                "create_option": copt,
            },
        )
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        self.table_name = table_name
        return table_http(self.net, self.database_name, table_name)

    def drop_table(
            self,
            table_name,
            conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if not isinstance(conflict_type, (list, dict, tuple)):
            exists = baseDropOptions.get(conflict_type, None)
            if exists is not None:
                copt = baseDropOptions[conflict_type]

        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data(["drop_option"], {"drop_option": copt})
        r = self.net.request(url, "delete", h, d)
        self.net.raise_exception(r)
        return database_result()

    def list_tables(self):
        url = f"databases/{self.database_name}/tables"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        return convert_to_list_tables_result(r.json())

    def show_table(self, table_name):
        check_valid_name(table_name)
        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        return convert_to_show_table_result(r.json())

    def get_all_tables(self):
        url = f"databases/{self.database_name}/tables"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
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
        check_valid_name(table_name)
        url = f"databases/{self.database_name}/tables/{table_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        return table_http(self.net, self.database_name, table_name)

    # table snapshot operations
    def create_table_snapshot(self, snapshot_name, table_name):
        """Create a snapshot of a specific table"""
        url = "snapshots"
        h = self.net.set_up_header(["accept", "content-type"])
        d = {
            "db_name": self.database_name,
            "table_name": table_name,
            "snapshot_name": snapshot_name,
        }
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def restore_table_snapshot(self, snapshot_name):
        """Restore a table from a snapshot"""
        url = "snapshots/restore"
        h = self.net.set_up_header(["accept", "content-type"])
        d = {"snapshot_name": snapshot_name}
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()


class table_http:
    def __init__(self, net: http_network_util, database_name: str, table_name: str):
        self.net = net
        self.database_name = database_name
        self.table_name = table_name

    def rename(self, new_table_name):
        check_valid_name(new_table_name)
        url = f"databases/{self.database_name}/tables/{self.table_name}/rename"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"new_table_name": new_table_name})
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def show_columns(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/columns"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        res = {"name": [], "type": [], "default": [], "comment": []}
        print(r.json())
        for col in r.json()["columns"]:
            res["name"].append(col["name"])
            res["type"].append(col["type"])
            res["default"].append(col["default"])
            res["comment"].append(col["comment"])
        res = pl.from_pandas(pd.DataFrame(res))
        return res

    def show_columns_type(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/columns"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        res = {}
        for col in r.json()["columns"]:
            res[col["name"]] = col["type"]
        return res

    def compact(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/compact"
        h = self.net.set_up_header(["accept", "content-type"])
        r = self.net.request(url, "put", h)
        self.net.raise_exception(r)
        return database_result()

    # index
    def create_index(
            self,
            index_name,
            index_info,
            conflict_type=ConflictType.Error,
            index_comment: str = ""
    ):
        copt = conflict_type
        if not isinstance(conflict_type, (list, dict, tuple)):
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
        # print(fields)
        # print(create_index_info)

        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"
        h = self.net.set_up_header(
            ["accept", "content-type"],
        )
        d = self.net.set_up_data(
            ["create_option"],
            {"comment": index_comment, "fields": fields, "index": create_index_info, "create_option": copt}
        )
        r = self.net.request(url, "post", h, d)
        self.net.raise_exception(r)
        return database_result()

    def drop_index(
            self,
            index_name,
            conflict_type=ConflictType.Error,
    ):
        copt = conflict_type
        if not isinstance(conflict_type, (list, dict, tuple)):
            exists = baseDropOptions.get(conflict_type, None)
            if exists is not None:
                copt = baseDropOptions[conflict_type]

        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"

        h = self.net.set_up_header(["accept"])
        d = self.net.set_up_data([], {"drop_option": copt})
        r = self.net.request(url, "delete", h, d)
        self.net.raise_exception(r)
        return database_result()

    def show_index(self, index_name):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        r_json = r.json()
        index_type = r_json.get("index_type", "")
        index_comment = r_json.get("index_comment", "")
        return database_result(index_type=index_type, index_comment=index_comment)

    def list_indexes(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        r_json = r.json()
        index_list = []
        exists = r_json.get("indexes", None)
        if exists is not None:
            for t in r_json["indexes"]:
                index_list.append(t["index_name"])
        return database_result(index_names=index_list)

    def dump_index(self, index_name):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}/dump"
        h = self.net.set_up_header(["accept"])
        r = self.net.request(url, "put", h)
        self.net.raise_exception(r)
        return database_result()

    def optimize(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/optimize"
        h = self.net.set_up_header(["accept", "content-type"])
        r = self.net.request(url, "put", h)
        self.net.raise_exception(r)
        return database_result()

    def alter_index(self, index_name="", alter_index_options={}):
        url = f"databases/{self.database_name}/tables/{self.table_name}/indexes/{index_name}"
        h = self.net.set_up_header(["accept", "content-type"])
        opt_opt = {"alter_index_options": alter_index_options}
        r = self.net.request(url, "put", h, opt_opt)
        self.net.raise_exception(r)
        return database_result()

    def insert(self, values=[]):
        if isinstance(values, list):
            pass
        else:
            values = [values]

        for value in values:
            if isinstance(value, dict):
                for key in value:
                    if isinstance(value[key], FDE):
                        # Convert FDE object to HTTP API format
                        fde_obj = value[key]
                        value[key] = {
                            "function": "fde",
                            "tensor_data": fde_obj.tensor_data,
                            "target_dimension": fde_obj.target_dimension
                        }
                    elif isinstance(value[key],
                                    np.ndarray):  # trans np array to list since http api can not parse np array
                        value[key] = value[key].tolist()
                    elif isinstance(value[key], list):
                        for idx in range(len(value[key])):
                            if isinstance(value[key][idx], np.ndarray):
                                value[key][idx] = value[key][idx].tolist()
                    elif isinstance(value[key], SparseVector):
                        value[key] = value[key].to_dict()

        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.net.set_up_header(["accept", "content-type"])
        r = self.net.request(url, "post", h, values)
        self.net.raise_exception(r)
        return database_result()

    def import_data(self, data_path="/home/infiniflow/Documents/development/infinity/test/data/csv/pysdk_test.csv",
                    import_options={}):
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
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], data)
        r = self.net.request(url, "put", h, d)
        self.net.raise_exception(r)
        return database_result()

    def export_data(self, data_path="", export_options={}, columns=[]):
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
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], data)
        r = self.net.request(url, "get", h, d)
        self.net.raise_exception(r)
        return database_result()

    def add_columns(self, columns_definition={}):
        url = f"databases/{self.database_name}/tables/{self.table_name}/columns"
        h = self.net.set_up_header(["accept", "content-type"])
        fields = []
        for col in columns_definition:
            tmp = {"name": col}
            for param_name in columns_definition[col]:
                tmp[param_name.lower()] = columns_definition[col][param_name]
            fields.append(tmp)
        d = self.net.set_up_data([], {"fields": fields})
        r = self.net.request(url, "post", h, d)
        return self.net.get_database_result(r)

    def drop_columns(self, column_name: list[str] | str):
        if isinstance(column_name, str):
            column_name = [column_name]
        url = f"databases/{self.database_name}/tables/{self.table_name}/columns"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"column_names": column_name})
        r = self.net.request(url, "delete", h, d)
        return self.net.get_database_result(r)

    def output(
            self,
            output=[],
    ):
        return table_http_result(output, self)

    def delete(self, filter=""):
        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"filter": filter})
        r = self.net.request(url, "delete", h, d)
        self.net.raise_exception(r)
        json_res = r.json()
        error_code = json_res['error_code']
        delete_rows = json_res['deleted_rows']
        return database_result(error_code=error_code, deleted_rows=delete_rows)

    def update(self, filter_str: str, update: dict[str, Any]):
        url = f"databases/{self.database_name}/tables/{self.table_name}/docs"
        h = self.net.set_up_header(["accept", "content-type"])
        d = self.net.set_up_data([], {"update": update, "filter": filter_str})
        r = self.net.request(url, "put", h, d)
        self.net.raise_exception(r)
        return database_result()

    def show_segments(self):
        url = f"databases/{self.database_name}/tables/{self.table_name}/segments"
        h = self.net.set_up_header(["accept", "content-type"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        return pd.DataFrame(r.json()['segments'])

    def show_blocks(self, segment_id: int):
        url = f"databases/{self.database_name}/tables/{self.table_name}/segments/{segment_id}/blocks"
        h = self.net.set_up_header(["accept", "content-type"])
        r = self.net.request(url, "get", h)
        self.net.raise_exception(r)
        return pd.DataFrame(r.json()['blocks'])


class table_http_result:
    def __init__(self, output: list, table_http: table_http):
        self.table_http = table_http

        self.output_res = []
        self._output = output
        self._highlight = []
        self._filter = ""
        self._fusion = []
        self._knn = []
        self._match = []
        self._match_tensor = []
        self._match_sparse = []
        self._search_exprs = []
        self._sort = []
        self._group_by = []
        self._having = []
        self._limit = None
        self._offset = None
        self._option = None

    def select(self):
        url = f"databases/{self.table_http.database_name}/tables/{self.table_http.table_name}/docs"
        h = self.table_http.net.set_up_header(["accept", "content-type"])
        tmp = {}
        if len(self._filter):
            tmp["filter"] = self._filter
        if len(self._search_exprs):
            tmp["search"] = self._search_exprs
        if len(self._output):
            tmp["output"] = self._output
        if len(self._highlight):
            tmp["highlight"] = self._highlight
        if len(self._sort):
            tmp["sort"] = self._sort
        if len(self._group_by):
            tmp["group_by"] = self._group_by
        if len(self._having):
            tmp["having"] = self._having
        if self._limit is not None:
            tmp["limit"] = str(self._limit)
        if self._offset is not None:
            tmp["offset"] = str(self._offset)
        if self._option is not None:
            tmp["option"] = self._option
        # print(tmp)
        d = self.table_http.net.set_up_data([], tmp)

        # Add timing measurement for network transfer and JSON parsing
        import time

        # Time the network transfer
        start_network = time.perf_counter()
        r = self.table_http.net.request(url, "get", h, d)
        end_network = time.perf_counter()

        network_time = (end_network - start_network) * 1000
        response_size = len(r.content)

        # Time JSON parsing
        start_parse = time.perf_counter()
        result_json = r.json()
        end_parse = time.perf_counter()

        parse_time = (end_parse - start_parse) * 1000

        # Print timing information for large responses
        if response_size > 100000:  # Only print for responses > 100KB
            print(
                f"HTTP Timing - Network: {network_time:.2f}ms, Parse: {parse_time:.2f}ms, Size: {response_size:,} bytes")
            if "output" in result_json:
                row_count = len(result_json["output"])
                print(f"  Rows: {row_count:,}, Avg row size: {response_size / row_count:.1f} bytes")

        self.table_http.net.raise_exception(r)
        # print(r.json())
        if "output" in result_json:
            self.output_res = result_json["output"]
        else:
            self.output_res = []

        if "total_hits_count" in result_json:
            self.total_hits_count = result_json["total_hits_count"]
        else:
            self.total_hits_count = None

        return self

    def explain(self, ExplainType=ExplainType.Physical):
        url = f"databases/{self.table_http.database_name}/tables/{self.table_http.table_name}/meta"
        h = self.table_http.net.set_up_header(["accept", "content-type"])
        tmp = {}
        if len(self._filter):
            tmp["filter"] = self._filter
        if len(self._fusion):
            tmp["fusion"] = self._fusion
        if len(self._knn):
            tmp["knn"] = self._knn
        if len(self._match):
            tmp["match"] = self._match
        if len(self._match_tensor):
            tmp["match_tensor"] = self._match_tensor
        if len(self._match_sparse):
            tmp["match_sparse"] = self._match_sparse
        if len(self._output):
            tmp["output"] = self._output
        if len(self._highlight):
            tmp["highlight"] = self._highlight
        if len(self._sort):
            tmp["sort"] = self._sort
        if len(self._group_by):
            tmp["group_by"] = self._group_by
        if len(self._having):
            tmp["having"] = self._having
        if self._limit is not None:
            tmp["limit"] = self._limit
        if self._offset is not None:
            tmp["offset"] = self._offset
        if self._option is not None:
            tmp["option"] = self._option

        tmp["explain_type"] = ExplainType_transfrom(ExplainType)
        # print(tmp)
        d = self.table_http.net.set_up_data([], tmp)
        r = self.table_http.net.request(url, "get", h, d)
        self.table_http.net.raise_exception(r)
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

    def highlight(
            self,
            highlight=[],
    ):
        self._highlight = highlight
        return self

    def sort(self, order_by_expr_list: Optional[List[list[str, SortType]]]):
        for order_by_expr in order_by_expr_list:
            tmp = {}
            if len(order_by_expr) != 2:
                raise InfinityException(ErrorCode.INVALID_PARAMETER,
                                        "order_by_expr_list must be a list of [column_name, sort_type]")
            if order_by_expr[1] not in [SortType.Asc, SortType.Desc]:
                raise InfinityException(ErrorCode.INVALID_PARAMETER, "sort_type must be SortType.Asc or SortType.Desc")
            if order_by_expr[1] == SortType.Asc:
                tmp[order_by_expr[0]] = "asc"
            else:
                tmp[order_by_expr[0]] = "desc"
            self._sort.append(tmp)
        return self

    def limit(self, limit_num):
        self._limit = limit_num
        return self

    def offset(self, offset):
        self._offset = offset
        return self

    def group_by(self, group_by_list):
        self._group_by = group_by_list
        return self

    def having(self, having_expr):
        self._having = having_expr
        return self

    def option(self, option: {}):
        # option_str = json.dumps(option)
        # option_str = str(option)
        # option_str.replace("'\"'", "")
        # eval(option_str)
        # option_str.replace("'", "")
        self._option = option
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
        tmp_match_tensor = {"match_method": "tensor", "field": column_name, "query_tensor": query_data,
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
            tmp_new_params = {"field": fusion_params["field"], "query_tensor": fusion_params["query_tensor"],
                              "element_type": fusion_params["element_type"]}
            # handle left params
            fusion_params.pop("field")
            fusion_params.pop("query_tensor")
            fusion_params.pop("element_type")
            tmp_new_params.update(fusion_params)
            tmp_fusion_expr["params"] = tmp_new_params
        else:
            if fusion_params is not None:
                tmp_fusion_expr["params"] = fusion_params
        self._search_exprs.append(tmp_fusion_expr)
        return self

    def to_result(self):
        import time

        if self.output_res == []:
            self.select()

        # Time the data processing
        start_process = time.perf_counter()

        df_dict = {}
        col_types = self.table_http.show_columns_type()
        for output_col in self._output:
            if output_col in col_types:
                df_dict[output_col] = ()
        # when output["*"] and output_res is empty
        for output_col in self._output:
            if output_col == "*":
                for col in col_types:
                    df_dict[col] = ()

        line_i = 0
        for res in self.output_res:
            for col in res:
                col_name = next(iter(col))
                v = col[col_name]
                if col_name not in df_dict:
                    df_dict[col_name] = ()
                tup = df_dict[col_name]
                if len(tup) == line_i + 1:
                    continue
                if isinstance(v, (int, float)):
                    new_tup = tup + (v,)
                elif is_list(v):
                    new_tup = tup + (ast.literal_eval(v),)
                elif is_date(v):
                    new_tup = tup + (v,)
                elif is_time(v):
                    new_tup = tup + (v,)
                elif is_datetime(v):
                    new_tup = tup + (v,)
                elif is_sparse(v):  # sparse vector
                    sparse_vec = str2sparse(v)
                    new_tup = tup + (sparse_vec,)
                else:
                    if v.lower() == 'true':
                        v = True
                    elif v.lower() == 'false':
                        v = False
                    new_tup = tup + (v,)
                df_dict[col_name] = new_tup
            line_i += 1
        # print(self.output_res)
        # print(df_dict)
        extra_result = None
        if self.total_hits_count is not None:
            extra_result = {}
            extra_result["total_hits_count"] = self.total_hits_count

        df_type = {}
        for k in df_dict:
            if k in col_types:  # might be object
                df_type[k] = type_to_dtype(col_types[k])
            if k in ["DISTANCE", "SCORE", "SIMILARITY"]:
                df_type[k] = dtype('float32')
            # "(c1 + c2)", "sqrt(c1), round(c1)"
            k1 = k.replace("(", " ")
            k1 = k1.replace(")", " ")
            k1 = k1.replace("+", " ")
            k1 = k1.replace("-", " ")
            k1 = k1.replace(",", " ")
            cols = k1.split(" ")
            cols = [col for col in cols if col != ""]

            function_name = ""
            for col in cols:
                if col.strip() in col_types:
                    df_type[k] = type_to_dtype(col_types[col.strip()])
                    df_type[k] = function_return_type(function_name, df_type[k])
                elif col.strip().isdigit() and df_type.get(k) != dtype('float64'):
                    df_type[k] = dtype('int32')
                    df_type[k] = function_return_type(function_name, df_type[k])
                elif is_float(col.strip()):
                    df_type[k] = dtype('float64')
                    df_type[k] = function_return_type(function_name, df_type[k])
                elif col == "/":
                    df_type[k] = dtype('float64')
                    break
                else:
                    function_name = col.strip().lower()
                    if (function_name in functions):
                        df_type[k] = function_return_type(function_name, None)
                    if (function_name in bool_functions):
                        df_type[k] = dtype('bool')
                        break

        end_process = time.perf_counter()
        process_time = (end_process - start_process) * 1000

        # Print timing for large datasets
        if len(self.output_res) > 1000:  # Only print for datasets with > 1000 rows
            print(f"HTTP Data Processing - Time: {process_time:.2f}ms, Rows: {len(self.output_res):,}")

        return df_dict, df_type, extra_result

    def to_pl(self):
        dataframe, extra_result = self.to_df()
        return pl.from_pandas(dataframe), extra_result

    def to_df(self):
        import time

        # Time the to_result() call (data processing)
        start_result = time.perf_counter()
        df_dict, df_type, extra_result = self.to_result()
        end_result = time.perf_counter()

        result_time = (end_result - start_result) * 1000

        # Time DataFrame construction
        start_df = time.perf_counter()
        df = pd.DataFrame(df_dict).astype(df_type)
        end_df = time.perf_counter()

        df_time = (end_df - start_df) * 1000

        # Print timing for large DataFrames
        if len(df) > 1000:  # Only print for DataFrames with > 1000 rows
            print(
                f"HTTP DataFrame Timing - Processing: {result_time:.2f}ms, Construction: {df_time:.2f}ms, Rows: {len(df):,}")

        return df, extra_result

    def to_arrow(self):
        dataframe, extra_result = self.to_df()
        return pa.Table.from_pandas(dataframe), extra_result


@dataclass
class database_result():
    def __init__(self, list=[], database_name: str = "", error_code=ErrorCode.OK, columns=[], index_names=[],
                 node_name="", node_role="", node_status="", index_type=None, index_comment=None, deleted_rows=0,
                 data={}, nodes=[], error_msg="", snapshots=[], config_value: Any = None):
        self.db_names = list
        self.database_name = database_name  # get database
        self.error_code = error_code
        self.columns = columns
        self.index_names = index_names
        self.node_name = node_name
        self.node_role = node_role
        self.node_status = node_status
        self.index_type = index_type
        self.index_comment = index_comment
        self.deleted_rows = deleted_rows
        self.data = data
        self.nodes = nodes
        self.error_msg = error_msg
        self.snapshots = snapshots
        self.config_value = config_value


list_tables_result = namedtuple('list_tables_result', [
    'error_code',
    'error_msg',
    'table_names',
])


def convert_to_list_tables_result(data_dict):
    return list_tables_result(
        error_code=data_dict.get('error_code'),
        error_msg=data_dict.get('error_msg'),
        table_names=data_dict.get('table_names'),
    )


show_table_result = namedtuple('show_table_result', [
    'error_code',
    'error_msg',
    'database_name',
    'table_name',
    'store_dir',
    'column_count',
    'segment_count'
])


def convert_to_show_table_result(data_dict):
    return show_table_result(
        error_code=data_dict.get('error_code'),
        error_msg=data_dict.get('error_msg'),
        database_name=data_dict.get('database_name'),
        table_name=data_dict.get('table_name'),
        store_dir=data_dict.get('storage_directory'),
        column_count=int(data_dict.get('column_count')),
        segment_count=int(data_dict.get('segment_count')),
    )


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
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name cannot be composed of whitespace characters only")
    if name == '':
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name == ' ':
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name.isdigit():
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
