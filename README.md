<div align="center">
  <img width="187" src="https://github.com/infiniflow/infinity/assets/7248/015e1f02-1f7f-4b09-a0c2-9d261cd4858b" alt="Infinity logo"/>
</div>


<p align="center">
    <b>The AI-native database built for LLM applications, providing incredibly fast hybrid search of dense embedding, sparse embedding, tensor and full-text</b>
</p>

<h4 align="center">
  <a href="https://infiniflow.org/docs/dev/category/get-started">Document</a> |
  <a href="https://infiniflow.org/docs/dev/benchmark">Benchmark</a> |
  <a href="https://twitter.com/infiniflowai">Twitter</a> |
  <a href="https://discord.gg/jEfRUwEYEV">Discord</a>
</h4>


Infinity is a cutting-edge AI-native database that provides a wide range of search capabilities for rich data types such as dense vector, sparse vector, tensor, full-text, and structured data. It provides robust support for various LLM applications, including search, recommenders, question-answering, conversational AI, copilot, content generation, and many more **RAG** (Retrieval-augmented Generation) applications.

- [Key Features](#-key-features)
- [Get Started](#-get-started)
- [Document](#-document)
- [Roadmap](#-roadmap)
- [Community](#-community)

## ⚡️ Performance

<div class="column" align="middle">
  <img src="https://github.com/user-attachments/assets/c4c98e23-62ac-4d1a-82e5-614bca96fe0a" alt="Infinity performance comparison"/>
</div>

## 🌟 Key Features

Infinity comes with high performance, flexibility, ease-of-use, and many features designed to address the challenges facing the next-generation AI applications:

### 🚀 Incredibly fast

- Achieves 0.1 milliseconds query latency and 15K+ QPS on million-scale vector datasets.
- Achieves 1 millisecond latency and 12K+ QPS in full-text search on 33M documents.

> See the [Benchmark report](https://infiniflow.org/docs/dev/benchmark) for more information.

### 🔮 Powerful search

- Supports a hybrid search of dense embedding, sparse embedding, tensor, and full text, in addition to filtering.
- Supports several types of rerankers including RRF, weighted sum and **ColBERT**.

### 🍔 Rich data types

Supports a wide range of data types including strings, numerics, vectors, and more.

### 🎁 Ease-of-use

- Intuitive Python API. See the [Python API](https://infiniflow.org/docs/dev/pysdk_api_reference)
- A single-binary architecture with no dependencies, making deployment a breeze.
- Embedded in Python as a module and friendly to AI developers.  

## 🎮 Get Started

This section provides guidance on deploying the Infinity database using Docker, with the client and server as separate processes. 

### Prerequisites

- CPU: x86_64 with AVX2 support.
- OS:
  - Linux with glibc 2.17+.
  - Windows 10+ with WSL/WSL2.
  - MacOS
- Python: Python 3.11+.


### Install Infinity server

#### Linux x86_64 & MacOS x86_64

```bash
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
docker pull infiniflow/infinity:nightly
docker run -d --name infinity -v /var/infinity/:/var/infinity --ulimit nofile=500000:500000 --network=host infiniflow/infinity:nightly
```
#### Windows

If you are on Windows 10+, you must enable WSL or WSL2 to deploy Infinity using Docker. Suppose you've installed Ubuntu in WSL2:

1. Follow [this](https://learn.microsoft.com/en-us/windows/wsl/systemd) to enable systemd inside WSL2.
2. Install docker-ce according to the [instructions here](https://docs.docker.com/engine/install/ubuntu).
3. If you have installed Docker Desktop version 4.29+ for Windows: **Settings** **>** **Features in development**, then select **Enable host networking**.
4. Pull the Docker image and start Infinity: 

   ```bash
   sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
   docker pull infiniflow/infinity:nightly
   docker run -d --name infinity -v /var/infinity/:/var/infinity --ulimit nofile=500000:500000 --network=host infiniflow/infinity:nightly
   ```

### Install Infinity client

```
pip install infinity-sdk==0.6.11
```

### Run a vector search

```python
import infinity

infinity_obj = infinity.connect(infinity.NetworkAddress("<SERVER_IP_ADDRESS>", 23817)) 
db_object = infinity_object.get_database("default_db")
table_object = db_object.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector, 4, float"}})
table_object.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
table_object.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
res = table_object.output(["*"])
                  .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2)
                  .to_pl()
print(res)
```

## 🔧 Deploy Infinity using binary

If you wish to deploy Infinity using binary with the server and client as separate processes, see the [Deploy infinity using binary](https://infiniflow.org/docs/dev/deploy_infinity_server) guide.

## 🔧 Build from Source

See the [Build from Source](https://infiniflow.org/docs/dev/build_from_source) guide.

## 📚 Document

- [Quickstart](https://infiniflow.org/docs/dev/)
- [Python API](https://infiniflow.org/docs/dev/pysdk_api_reference)
- [HTTP API](https://infiniflow.org/docs/dev/http_api_reference)
- [References](https://infiniflow.org/docs/dev/category/references)
- [FAQ](https://infiniflow.org/docs/dev/FAQ)

## 📜 Roadmap

See the [Infinity Roadmap 2025](https://github.com/infiniflow/infinity/issues/2393)

## 🙌 Community

- [Discord](https://discord.gg/jEfRUwEYEV)
- [Twitter](https://twitter.com/infiniflowai)
- [GitHub Discussions](https://github.com/infiniflow/infinity/discussions)

