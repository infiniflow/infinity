# dependency

## python

```shell
pip install -r requirements.txt
```

## cpp

### thrift

```shell
sudo apt-get install thrift-compiler
```

# build

```shell
python setup.py bdist_wheel
```

# install
```shell
pip install dist/infinity-0.0.1-py3-none-any.whl
```
# using

```python
import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder

# infinity
infinity_obj = infinity.connect(REMOTE_HOST)
db = infinity_obj.get_database("default")
db.drop_table("my_table")
db.create_table(
    "my_table", {"num": "integer", "body": "varchar", "vec": "vector,5,float"}, None)

table = db.get_table("my_table")
res = table.insert(
    [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
assert res.success
res = table.insert(
    [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
assert res.success
res = table.insert(
    [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])
assert res.success

res = table.create_index("my_index",
                         [index.IndexInfo("body",
                                          index.IndexType.IRSFullText,
                                          [index.InitParameter("ANALYZER", "segmentation")]),
                          ], None)
assert res.success

query_builder = InfinityThriftQueryBuilder(table)
query_builder.output(["num", "body"])
query_builder.knn('vec', [3.0] * 5, 'float', 'ip', 2)
query_builder.match('body', 'harmful', 'topn=2')
query_builder.fusion('rrf')
res = query_builder.to_pl()
print(res)

```