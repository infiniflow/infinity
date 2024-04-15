import sys 
import pytest 
from httpapibase import HttpTest
from common.common_values import * 
import infinity.index as index

@pytest.mark.skip(reason="can not insert varchar")
class TestQuery(HttpTest):
    def test_query(self):
        dbname = "default"
        tbname = "my_table"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,tbname,{
                "num":{
                    "type":"integer",
                },
                "body":{
                    "type":"varchar",
                },
                "vec":{
                    "type": "vector",
                    "dimension": 5,
                    "element_type": "float"
                }
            }
        )
        self.insert(dbname,tbname,
            [{"num":1},{"body": "undesirable, unnecessary, and harmful"},{"vec":[1.0]*5}],
        )
        self.insert(dbname,tbname,
            [{"num":2},{"body": "publisher=US National Office for Harmful Algal Blooms"},{"vec":[4.0]*5}],
        )
        self.insert(dbname,tbname,
            [{"num":3},{"body": "in the case of plants, growth and chemical"},{"vec":[7.0]*5}],
        )
        self.createIdx(dbname,tbname,idxname,{
            ["body"],
            {
                "type":"FULLTEXT",
            }
        })
        
        return 
    
    def test_query_builder(self):
        dbname = "default"
        tbname = "my_table"
        idxname = "my_index"
        self.dropTable(dbname,tbname)
        self.createTable(
            dbname,tbname,{
                "c1":{
                    "type":"integer",
                },
            }
        )
        self.select(dbname,tbname,
                ["*"])