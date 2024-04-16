import requests
import logging
import os 
import sys 
import numpy as np
from common.common_data import *
from common.common_values import *

class HttpTest:
 
    
    #Pre operation: set header
    def SetUpHeader(self,param=[],tp={}): 
        header = {}
        if len(param) != 0:
            for i in range(len(param)):
                item = self.header_dict.get(param[i])
                if item == None:
                    logging.warn("can't find header param: "+param[i])
                    continue
                header[param[i]] = item
        if len(tp) != 0:
            for item in dict.items():
                header[item[0]] = item[1] 
        
        print(header)
        return header 
    
    ##Pre operation: set data
    def SetUpData(self,param=[],tp={}):
        data = {}
        if len(param) != 0:
            for i in range(len(param)):
                item = self.data_dict.get(param[i],"")
                if len(item) == 0:
                    logging.warn("can't find data param: "+param[i])
                    continue
                data[param[i]] = item
        if len(tp) != 0:
            for item in tp.items():
                data[item[0]] = item[1] 
        print(data)
        return data  
    #Post operation
    def TearDown(self,resp,expect={}): 
        print("status_code:"+str(resp.status_code))
        status_code = expect.get("status_code", None)
        if status_code is not None:
            assert resp.status_code == expect['status_code']
        else:
           assert resp.status_code ==  expected_status_code
          
        resp_json = resp.json()
        print(resp_json)
        for item in expect.items():
            if resp_json.get(item[0],None) is None:
                continue 
            print("["+str(item[0])+"]: "+"resp:"+str(resp_json[item[0]])+", expect:"+str(item[1]))
            assert str(resp_json[item[0]]) == str(item[1])
        print("----------------------------------------------")
        return
    def Request(self,url,method,header={},data={}):
        url = default_url + url 
        print("url: "+ url); 
        match method:
            case "get":
                response = requests.get(url, headers=header,json=data)
            case "post":
                response = requests.post(url, headers=header,json=data)
            case "put":
                response = requests.put(url, headers=header,json=data)
            case "delete":
                response = requests.delete(url, headers=header,json=data)
        return response
    
    def GetHeaderDict(self):
        return self.header_dict 
    
    def GetReponseDict(self):
        return self.response_dict
    
    def GetDataDict(self):
        return self.data_dict
    
    def createdb(self,dbname,expect={
        "error_code": 0
    }):
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = self.SetUpData(['create_option'])
        r = self.Request(url,"post",h,d)
        self.TearDown(r,expect)
        return 
    def createdbWithoutCheck(self,dbname):
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = self.SetUpData(['create_option'])
        r = self.Request(url,"post",h,d)
        assert r.status_code == 200
        return r 
    
    def showdb(self,dbname,expect={
        "error_code": 0,
        "table_count": 0,
    }):
        
        expect["database_name"] = dbname 
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h,{})
        self.TearDown(r,expect)
        return 
    
    def dropdb(self,dbname,expect={
        "error_code": 0
    }):
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = self.SetUpData(['drop_option'])
        r = self.Request(url,"delete",h,d)
        self.TearDown(r,expect)
        return
    
    def dropdbWithoutCheck(self,dbname):
        url = f"databases/{dbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = self.SetUpData(['drop_option'])
        r = self.Request(url,"delete",h,d)
        assert r.status_code == 200
        return r 
    
    def listdb(self,expect):
        url = f"databases"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get")
        self.TearDown(r,expect)

    def getdbs(self):
        url = f"databases"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get")
        assert r.status_code == 200
        #return all db names
        ret = []
        dblist = (r.json())["databases"]
        for item in dblist:
            ret.append(item)
        return ret 
    
    def cleardbs(self):
        dbs = self.getdbs()
        for dbname in dbs:
            if dbname == 'default':
                continue
            self.dropdb(dbname)
    
    def createTable(self,dbname,tbname,fields=[],properties=[],expect={
        "error_code": 0
    },opt="kIgnore"):
        url = f"databases/{dbname}/tables/{tbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = self.SetUpData(['create_option'],{'fields': fields, 'properties': properties,'create_option':baseCreateOptions[opt]})
        r = self.Request(url,"post",h,d)
        print(r)
        self.TearDown(r,expect)
        return 
    
    def dropTable(self,dbname,tbname,expect={
        "error_code": 0,
    },opt = "kIgnore"):
        url = f"databases/{dbname}/tables/{tbname}"
        h = self.SetUpHeader(['accept','content-type'])
        d = self.SetUpData(['drop_option'],{'drop_option':baseDropOptions[opt]})
        r = self.Request(url,"delete",h,d)
        self.TearDown(r,expect)
        return 
    
    def showTable(self,dbname,tbname,expect):
        url = f"databases/{dbname}/tables/{tbname}"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def ListTables (self,dbname,expect={
        "error_code": 0,
    }):
        url = f"databases/{dbname}/tables"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def GetAllTables (self,dbname):
        url = f"databases/{dbname}/tables"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        #return all db names
        ret = []
        tblist = (r.json())["tables"]
        for item in tblist:
            ret.append(item)
        return ret 

    def showTableColums(self,dbname,tbname,expect):
        url = f"databases/{dbname}/tables/{tbname}/columns"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return
    
    # part index 
    def createIdx(self,dbname,tbname,idxname,fields=[],index={},expect={
        "error_code":0,
    },opt = "kIgnore"):
        url = f"databases/{dbname}/tables/{tbname}/indexes/{idxname}"
        ignore = False 
        if opt == "kIgnore":
            ignore = True 
        elif opt == "kError":
            ignore = False
        else:
            ignore = opt 
        h = self.SetUpHeader(['accept','content-type'],)
        d = self.SetUpData([],{"fields":fields,"index":index,"create_option":{"ignore_if_exists":ignore}})
        r = self.Request(url,"post",h,d)
        self.TearDown(r,expect)
        return 
    
    def dropIdx(self,dbname,tbname,idxname,expect={
        "error_code":0,
    },opt = "kIgnore"):
        url = f"databases/{dbname}/tables/{tbname}/indexes/{idxname}"
        ignore = False 
        if opt == "kIgnore":
            ignore = True 
        h = self.SetUpHeader(['accept'])
        d = self.SetUpData([],{"drop_option":{"ignore_if_not_exists":ignore}})
        r = self.Request(url,"delete",h,d)
        self.TearDown(r,expect)
        return 
    
    def showIdx(self,dbname,tbname,idxname,expect):
        url = f"databases/{dbname}/tables/{tbname}/indexes/{idxname}"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def listIdx(self,dbname,tbname,expect):
        url = f"databases/{dbname}/tables/{tbname}/indexes"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        r_json = r.json()
        list = []
        isexist = expect.get("tables", None)
        if isexist is not None:
            for t in r_json["tables"]:
                list.append(t)
        return list 
    
    #insert data to tables
    def insert(self,dbname,tbname,filter=[],expect={
        "error_code":0
    }):
        url = f"databases/{dbname}/tables/{tbname}/docs"
        h = self.SetUpHeader(['accept',"content-type"])
        r = self.Request(url,"post",h,filter)
        self.TearDown(r,expect)
        return 
    
    #import data to table
    def importData(self,dbname,tbname,data={},expect={}):
        url = f"databases/{dbname}/tables/{tbname}"
        h = self.SetUpHeader(['accept',"content-type"])
        d = self.SetUpData([],{"data":data})
        r = self.Request(url,"put",h,d)
        self.TearDown(r,expect)
        return 

    # delete data from table
    def delete(self,dbname,tbname,filter="",expect={
        "error_code":0,
        #"delete_row_count": 10,
    }):
        url = f"databases/{dbname}/tables/{tbname}/docs"
        h = self.SetUpHeader(['accept',"content-type"])
        d = self.SetUpData([],{"filter":filter})
        r = self.Request(url,"delete",h,d)
        self.TearDown(r,expect)
        return 
    
    def update(self,dbname,tbname,update={},filter="",expect={
        "error_code":0,
        #"update_row_count":10
    }):
        url = f"databases/{dbname}/tables/{tbname}/docs"
        h = self.SetUpHeader(['accept',"content-type"])
        d = self.SetUpData([],{"update":update,"filter":filter})
        r = self.Request(url,"put",h,d)
        self.TearDown(r,expect)
        return 
    
    def select(self,dbname,tbname,output=[],filter="",fusion={},knn={},expect={
        "error_code":0,
        #output
    }):
        url = f"databases/{dbname}/tables/{tbname}/docs"
        h = self.SetUpHeader(['accept',"content-type"])
        tmp = {"output":output}
        if len(filter): tmp.update({"filter":filter})
        if len(fusion): tmp.update({"fusion":fusion})
        if len(knn): tmp.update({"knn":knn})
        d = self.SetUpData([],tmp)
        r = self.Request(url,"get",h,d)
        self.TearDown(r,expect)
        return 
    
    def getVariables(self,vanme,expect={
        "error_code":0
    }):
        url = f"variables/{vanme}"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def showSegmentList(self,dbname,tbname,expect):
        url = f"databases/{dbname}/tables/{tbname}/segments/"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def showSpecificSegmentDetail(self,dbname,tbname,segid,expect):
        url = f"databases/{dbname}/tables/{tbname}/segments/{segid}"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def showBlocksList(self,dbname,tbname,segid,expect):
        url = f"databases/{dbname}/tables/{tbname}/segments/{segid}/blocks/"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
        return 
    
    def showSpecificBlockDetail(self,dbname,tbname,segid,bid,expect):
        url = f"databases/{dbname}/tables/{tbname}/segments/{segid}/blocks/{bid}"
        h = self.SetUpHeader(['accept'])
        r = self.Request(url,"get",h)
        self.TearDown(r,expect)
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

