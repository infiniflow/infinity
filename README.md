<div align="center">
  <img width="187" src="https://github.com/infiniflow/infinity/assets/7248/015e1f02-1f7f-4b09-a0c2-9d261cd4858b"/>
</div>


<p align="center">
    <b>The AI-native database built for LLM applications, providing incredibly fast full-text and vector search</b>
</p>

<h4 align="center">
  <a href="https://infiniflow.org/docs/dev/category/get-started">Document</a> |
  <a href="https://infiniflow.org/docs/dev/benchmark">Benchmark</a> |
  <a href="https://twitter.com/infiniflowai">Twitter</a> |
  <a href="https://discord.gg/jEfRUwEYEV">Discord</a>
</h4>


Infinity is a cutting-edge AI-native database that provides a wide range of search capabilities for rich data types such as vectors, full-text, and structured data. It provides robust support for various LLM applications, including search, recommenders, question-answering, conversational AI, copilot, content generation, and many more **RAG** (Retrieval-augmented Generation) applications.

- [Key Features](#-key-features)
- [Get Started](#-get-started)
- [Document](#-document)
- [Roadmap](#-roadmap)
- [Community](#-community)


## 🌟 Key Features

Infinity comes with high performance, flexibility, ease-of-use, and many features designed to address the challenges facing the next-generation AI applications:

### ⚡️ Incredibly fast

- Achieves 0.1 milliseconds query latency and 15K+ QPS on million-scale vector datasets.
- Achieves 1 millisecond latency and 12K+ QPS in full-text search on 33M documents.

> See the [Benchmark report](https://infiniflow.org/docs/dev/benchmark) for more information.


### 🔮 Fused search

Supports a fused search of multiple embeddings and full text, in addition to filtering.

### 🍔 Rich data types

Supports a wide range of data types including strings, numerics, vectors, and more.

### 🎁 Ease-of-use

- Intuitive Python API. See the [Python API](https://infiniflow.org/docs/dev/python_api_reference)
- A single-binary architecture with no dependencies, making deployment a breeze.

## 🎮 Get Started

### Deploy Infinity database

#### Deploy Infinity using Docker on Linux x86_64 and MacOS x86_64

```bash
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
docker pull infiniflow/infinity:nightly
docker run -d --name infinity -v /var/infinity/:/var/infinity --ulimit nofile=500000:500000 --network=host infiniflow/infinity:nightly
```

#### Deploy Infinity using binary package on Linux x86_64

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
#### 🛠️ Build from Source

See [Build from Source](https://infiniflow.org/docs/dev/build_from_source).

### Install Infinity's Python client

`infinity-sdk` requires Python 3.10+.

```bash
pip3 install infinity-sdk==0.2.0.dev3
```

### Import necessary modules

```python
import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST
from infinity.common import ConflictType
```



### Connect to the remote server

```python
infinity_obj = infinity.connect(REMOTE_HOST)
```


### Get a database

```python
db = infinity_obj.get_database("default_db")
```


### Create a table

```python
# Drop my_table if it already exists
db.drop_table("my_table", ConflictType.Ignore)
# Create a table named "my_table"
table = db.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector, 4, float"}})
```


### Insert two records 

```python
table.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
table.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
```


### Execute a vector search

```python
res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()
print(res)
```

> 💡 For more information about the Python API, see the [Python API Reference](https://infiniflow.org/docs/dev/python_api_reference).

## Document

- [Quickstart](https://infiniflow.org/docs/dev/)
- [Python API](https://infiniflow.org/docs/dev/python_api_reference)
- [HTTP API](https://infiniflow.org/docs/dev/http_api_reference)
- [References](https://infiniflow.org/docs/dev/category/references)
- [FAQ](https://infiniflow.org/docs/dev/FAQ)

## 📜 Roadmap

See the [Infinity Roadmap 2024](https://github.com/infiniflow/infinity/issues/338)

## 🙌 Community

- [Discord](https://discord.gg/jEfRUwEYEV)
- [Twitter](https://twitter.com/infiniflowai)
- [GitHub Discussions](https://github.com/infiniflow/infinity/discussions)

