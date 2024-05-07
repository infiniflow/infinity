---
sidebar_position: 1
slug: /
---

# Quickstart

## Prerequisites

CPU >= 4 cores, with FMA and SSE4_2
RAM >= 16 GB
Disk >= 50 GB
OS: Linux x86_64 or aarch64
Glibc >=2.17

## Deploy Infinity database

### Deploy Infinity using Docker on Linux x86_64 and MacOS x86_64

```bash
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
docker pull infiniflow/infinity:nightly
docker run -d --name infinity -v /var/infinity/:/var/infinity --ulimit nofile=500000:500000 --network=host infiniflow/infinity:nightly
```

### Deploy Infinity using binary package on Linux x86_64

You can download the binary package (deb, rpm, or tgz) for your respective host operating system from https://github.com/infiniflow/infinity/releases. The prebuilt packages are compatible with Linux distributions based on glibc 2.17 or later, for example, RHEL 7, Debian 8, Ubuntu 14.04.

Fedora/RHEL/CentOS/OpenSUSE
```bash
sudo rpm -i infinity-0.2.0-dev-x86_64.rpm
sudo systemctl start infinity
```

Ubuntu/Debian
```bash
sudo dpkg -i infinity-0.2.0-dev-x86_64.deb
sudo systemctl start infinity
```
### 🛠️ Build from Source

See [Build from Source](./build_from_source.md).

## Install a Python client

`infinity-sdk` requires Python 3.10+.

```bash
pip3 install infinity-sdk==0.2.0.dev1
```

## Import necessary modules

```python
import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST
from infinity.common import ConflictType
```

## Connect to the remote server

```python
infinity_obj = infinity.connect(REMOTE_HOST)
```

## Get a database

```python
db = infinity_obj.get_database("default_db")
```

## Create a table

```python
# Drop my_table if it already exists
db.drop_table("my_table", ConflictType.Ignore)
# Create a table named "my_table"
table = db.create_table(
          "my_table", {
            "num": {"type": "integer"}, 
            "body": {"type": "varchar"},
            "vec": {"type": "vector, 4, float"}
          })
```

## Insert two records 

```python
table.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
table.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
```

## Execute a vector search

```python
res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()
print(res)
```

> 💡 For more information about the Python API, see the [Python API Reference](../references/pysdk_api_reference.md).