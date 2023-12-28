
<div align="center">
  <img width="187" src="https://user-images.githubusercontent.com/93570324/234292265-889228a8-7a68-4e2d-b891-f75262410af1.png"/>
</div>

<p align="center">
    <b>The AI-native database built for LLM applications, providing incredibly fast vector and full-text search</b>
</p>

<h4 align="center">
  <a href="https://github.com/infiniflow/infinity/issues/338">Roadmap 2024</a> |
  <a href="https://twitter.com/infiniflowai">Twitter</a> |
  <a href="https://discord.gg/jEfRUwEYEV">Discord</a> |
  <a href="https://www.youtube.com/@InfiniFlow-AI">YouTube</a> |
</h4>


Infinity is a cutting-edge AI-native database that provides a wide range of search capabilities for rich data types such as vectors, full-text, and structured data. It provides robust support for various LLM applications, including search, recommenders, question-answering, conversational AI, copilot, content generation, and many more **RAG** (Retrieval-augmented Generation) applications.

## 🌟 Key Features

Infinity comes with high performance, flexibility, ease-of-use, and many features designed to address the challenges facing the next-generation AI applications:

### ⚡️ Incredibly fast

- Achieves 0.1 milliseconds query latency on million-scale vector datasets.
- Up to 10K QPS on million-scale vector datasets.

> See the [Benchmark report](./docs/benchmark.md) for more information.


### 🔮 Fused search

Supports a fused search of multiple embeddings and full text, in addition to filtering.

### 🍔 Rich data types

Supports a wide range of data types including strings, numerics, vectors, and more.

### 🎁 Ease-of-use

- Intuitive Python API. See the [Python API](docs/pysdk_api_reference.md)
- A single-binary architecture with no dependencies, making deployment a breeze.

## 🎮 Get Started

### Deploy Infinity database

#### Deploy using binary package on Linux x86_64

You can download the binary package (deb, rpm, or tgz) for your respective host operating system from https://github.com/infiniflow/infinity/releases. These packages are designed to be compatible with Linux x86_64 hosts that are no older than CentOS 7. They are statically linked, except for glibc. For example:

```bash
sudo rpm -i infinity-0.1.0-dev-x86_64.rpm
sudo systemctl start infinity
```

#### Deploy using Docker on Linux x86_64 and MacOS x86_64

```bash
docker pull infiniflow/infinity:nightly
docker run -d --name infinity -v /tmp/infinity/:/tmp/infinity --network=host infiniflow/infinity:nightly
```


### Install Infinity's Python client

```bash
pip install infinity_sdk 
```

### Import necessary modules

```python
import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST
```



### Connect to the remote server

```python
infinity_obj = infinity.connect(REMOTE_HOST)
```


### Get a database

```python
db = infinity_obj.get_database("default")
```


### Create a table

```python
# Drop my_table if it already exists
db.drop_table("my_table", if_exists=True)
# Create a table named "my_table"
table = db.create_table("my_table", {"num": "integer", "body": "varchar", "vec": "vector, 4, float"}, None)
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

> 💡 For more information about the Python API, see the [Python API Reference](docs/pysdk_api_reference.md).


## 🛠️ Build from Source

See [Build from Source](docs/build_from_source.md).

## 📜 Roadmap

See the [Infinity Roadmap 2024](https://github.com/infiniflow/infinity/issues/338)

## 🙌 Community

- [Discord](https://discord.gg/jEfRUwEYEV)
- [Twitter](https://twitter.com/infiniflowai)
- [GitHub Discussions](https://github.com/infiniflow/infinity/discussions)
- [YouTube](https://www.youtube.com/@InfiniFlow-AI)
