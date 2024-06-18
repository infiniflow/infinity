# python-infinity

# update python client

- Update "version" field of [project] chapter and client_version field of ConnectRequest message.
- build new python SDK
- upload to pypi.org
- install new python SDK

# build python SDK
Execute the following command under the root path of infinity project.
```shell
rm -f dist/* && pip wheel . -w dist
```
Note: This command will compile C++ code, which may take a long time. You can use `-v` to display the compilation details, i.e. `rm -f dist/* && pip wheel . -v -w dist`

# install python SDK
```shell
pip uninstall -y infinity-sdk && pip install dist/*.whl
```

This will install infinity-sdk and its dependencies.

# upload to pypi.org
```shell
twine upload dist/*.whl
```

Enter your pypi API token according to the prompt.

Note that pypi allow a version of a package [be uploaded only once](https://pypi.org/help/#file-name-reuse). You need to change the `version` inside the `pyproject.toml` before build and upload.

# using

```python
import infinity
from infinity.common import REMOTE_HOST
from infinity.common import ConflictType

infinity_obj = infinity.connect(REMOTE_HOST)
db = infinity_obj.get_database("default_db")
db.drop_table("my_table", ConflictType.Ignore)
table = db.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}}, ConflictType.Error)
table.insert([{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
table.insert([{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
table.insert([{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])

res = table.output(["*"]).knn("vec", [3.0] * 5, "float", "ip", 2).to_pl()
print(res)

```

# For developer
```shell
pip install -e .
```
Build the release version of infinity-sdk in the target location `cmake-build-release`
```shell
pip install . -v --config-settings=cmake.build-type="Release"  --config-settings=build-dir="cmake-build-release"
```
Build the debug version of infinity-sdk in the target location `cmake-build-debug`
```shell
pip install . -v --config-settings=cmake.build-type="Debug"  --config-settings=build-dir="cmake-build-debug"
```

