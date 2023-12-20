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
python setup.py sdist bdist_wheel
```

# install
```shell
pip install dist/*.whl
```

# upload
twine upload dist/*  
# using

```python
import infinity
from infinity.common import REMOTE_HOST

infinity_obj = infinity.connect(REMOTE_HOST)
db = infinity_obj.get_database("default")
db.drop_table("my_table", if_exists=True)
table = db.create_table(
    "my_table", {"num": "integer", "body": "varchar", "vec": "vector,5,float"}, None)
table.insert(
    [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
table.insert(
    [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
table.insert(
    [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])

res = table.output(["*"]).knn("vec", [3.0] * 5, "float", "ip", 2).to_pl()
print(res)

```