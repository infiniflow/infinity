import requests
import logging
import os
from common.common_data import *


class HttpTest:

    # Pre operation: set header
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

        # print(header)
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
        # print(data)
        return data
        # Post operation

    def tear_down(self, resp, expect={}):
        print("status_code:" + str(resp.status_code))
        if expect.get("status_code", None) is not None:
            assert resp.status_code == expect['status_code']
        else:
            assert resp.status_code == expected_status_code

        resp_json = resp.json()
        # print(resp_json)
        for item in expect.items():
            if resp_json.get(item[0], None) is None:
                continue
            print("[" + str(item[0]) + "]: " + "resp:" + str(resp_json[item[0]]) + ", expect:" + str(item[1]))
            assert str(resp_json[item[0]]) == str(item[1])
        print("----------------------------------------------")
        return

    def request(self, url, method, header={}, data={}):
        if header is None:
            header = {}
        url = default_url + url
        print("url: " + url)
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

    def create_database(self, db_name, expect={
        "error_code": 0
    }):
        url = f"databases/{db_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = self.set_up_data(['create_option'])
        r = self.request(url, "post", h, d)
        self.tear_down(r, expect)
        return

    def create_db_without_check(self, db_name):
        url = f"databases/{db_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = self.set_up_data(['create_option'])
        r = self.request(url, "post", h, d)
        return r

    def show_database(self, db_name, expect={
        "error_code": 0,
        "table_count": 0,
    }):

        expect["database_name"] = db_name
        url = f"databases/{db_name}"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h, {})
        self.tear_down(r, expect)
        return

    def drop_database(self, db_name, expect={
        "error_code": 0
    }):
        url = f"databases/{db_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = self.set_up_data(['drop_option'])
        r = self.request(url, "delete", h, d)
        self.tear_down(r, expect)
        return

    def drop_db_without_check(self, db_name):
        url = f"databases/{db_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = self.set_up_data(['drop_option'])
        r = self.request(url, "delete", h, d)
        assert r.status_code == 200
        return r

    def list_database(self, expect):
        url = f"databases"
        self.set_up_header(['accept'])
        r = self.request(url, "get")
        self.tear_down(r, expect)

    def get_database(self):
        url = f"databases"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get")
        assert r.status_code == 200
        # return all db names
        ret = []
        r_json = r.json()
        if r_json.get("databases", None) is None:
            return ret
        dblist = (r_json)["databases"]
        for item in dblist:
            ret.append(item)
        return ret

    def clear_database(self):
        dbs = self.get_database()
        for db_name in dbs:
            if db_name == "default_db":
                continue
            self.drop_database(db_name)

    def create_table(self, db_name, table_name, fields=[], properties=[], expect={
        "error_code": 0
    }, opt="kIgnore"):
        url = f"databases/{db_name}/tables/{table_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = self.set_up_data(['create_option'],
                             {'fields': fields, 'properties': properties, 'create_option': baseCreateOptions[opt]})
        r = self.request(url, "post", h, d)
        print(r)
        self.tear_down(r, expect)
        return

    def drop_table(self, db_name, table_name, expect={
        "error_code": 0,
    }, opt="kIgnore"):
        url = f"databases/{db_name}/tables/{table_name}"
        h = self.set_up_header(['accept', 'content-type'])
        d = self.set_up_data(['drop_option'], {'drop_option': baseDropOptions[opt]})
        r = self.request(url, "delete", h, d)
        self.tear_down(r, expect)
        return

    def show_table(self, db_name, table_name, expect):
        url = f"databases/{db_name}/tables/{table_name}"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def list_tables(self, db_name, expect={
        "error_code": 0,
    }):
        url = f"databases/{db_name}/tables"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def get_all_tables(self, db_name):
        url = f"databases/{db_name}/tables"
        h = self.set_up_header(['accept'])
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

    def show_table_columns(self, db_name, table_name, expect):
        url = f"databases/{db_name}/tables/{table_name}/columns"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    # part index
    def create_index(self, db_name, table_name, index_name, fields=[], index={}, expect={
        "error_code": 0
    }, opt="ignore_if_exists"):
        url = f"databases/{db_name}/tables/{table_name}/indexes/{index_name}"
        h = self.set_up_header(['accept', 'content-type'], )
        d = self.set_up_data([], {"fields": fields, "index": index, "create_option": opt})
        r = self.request(url, "post", h, d)
        self.tear_down(r, expect)
        return

    def drop_index(self, db_name, table_name, index_name, expect={
        "error_code": 0,
    }, opt="kIgnore"):
        url = f"databases/{db_name}/tables/{table_name}/indexes/{index_name}"
        ignore = False
        if opt == "kIgnore":
            ignore = True
        h = self.set_up_header(['accept'])
        d = self.set_up_data([], {"drop_option": {"ignore_if_not_exists": ignore}})
        r = self.request(url, "delete", h, d)
        self.tear_down(r, expect)
        return

    def show_index(self, db_name, table_name, index_name, expect):
        url = f"databases/{db_name}/tables/{table_name}/indexes/{index_name}"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def list_index(self, db_name, table_name, expect):
        url = f"databases/{db_name}/tables/{table_name}/indexes"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        r_json = r.json()
        index_list = []
        exists = r_json.get("tables", None)
        if exists is not None:
            for t in r_json["tables"]:
                index_list.append(t)
        return index_list

        # insert data to tables

    def insert(self, db_name, table_name, values=[], expect={
        "error_code": 0
    }):
        url = f"databases/{db_name}/tables/{table_name}/docs"
        h = self.set_up_header(['accept', "content-type"])
        r = self.request(url, "post", h, values)
        self.tear_down(r, expect)
        return

        # import data to table

    def import_data(self, db_name, table_name, data={}, expect={}):
        url = f"databases/{db_name}/tables/{table_name}"
        h = self.set_up_header(['accept', "content-type"])
        d = self.set_up_data([], {"data": data})
        r = self.request(url, "put", h, d)
        self.tear_down(r, expect)
        return

        # delete data from table

    def delete(self, db_name, table_name, filter="", expect={
        "error_code": 0,
    }):
        url = f"databases/{db_name}/tables/{table_name}/docs"
        h = self.set_up_header(['accept', "content-type"])
        d = self.set_up_data([], {"filter": filter})
        r = self.request(url, "delete", h, d)
        self.tear_down(r, expect)
        return

    def update(self, db_name, table_name, update={}, filter="", expect={
        "error_code": 0,
    }):
        url = f"databases/{db_name}/tables/{table_name}/docs"
        h = self.set_up_header(['accept', "content-type"])
        d = self.set_up_data([], {"update": update, "filter": filter})
        r = self.request(url, "put", h, d)
        self.tear_down(r, expect)
        return

    def select(self, db_name, table_name, output=[], filter="", fusion={}, knn={}, expect={
        "error_code": 0,
    }):
        url = f"databases/{db_name}/tables/{table_name}/docs"
        h = self.set_up_header(['accept', "content-type"])
        tmp = {"output": output}
        if len(filter):
            tmp.update({"filter": filter})
        if len(fusion):
            tmp.update({"fusion": fusion})
        if len(knn):
            tmp.update({"knn": knn})
        d = self.set_up_data([], tmp)
        r = self.request(url, "get", h, d)
        self.tear_down(r, expect)
        return

    def get_variables(self, variable_name, expect={
        "error_code": 0
    }):
        url = f"variables/{variable_name}"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def show_segment_list(self, db_name, table_name, expect):
        url = f"databases/{db_name}/tables/{table_name}/segments/"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def show_specific_segment_detail(self, db_name, table_name, segment_id, expect):
        url = f"databases/{db_name}/tables/{table_name}/segments/{segment_id}"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def show_blocks_list(self, db_name, table_name, segment_id, expect):
        url = f"databases/{db_name}/tables/{table_name}/segments/{segment_id}/blocks/"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def show_specific_block_detail(self, db_name, table_name, segment_id, bid, expect):
        url = f"databases/{db_name}/tables/{table_name}/segments/{segment_id}/blocks/{bid}"
        h = self.set_up_header(['accept'])
        r = self.request(url, "get", h)
        self.tear_down(r, expect)
        return

    def get_project_path(self):
        current_file = os.path.abspath(__file__)
        index = current_file.index("infinity")
        desired_path = current_file[:index + len("infinity")]
        return str(desired_path)

    # url: str
    # header_dict: dict
    # response_dict: dict
    # data_dict: dict 

    url = default_url
    header_dict = baseHeader
    response_dict = baseResponse
    data_dict = baseData
