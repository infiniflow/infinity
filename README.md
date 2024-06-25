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
- Embedded in Python as a library and friendly to AI developers.  

## 🎮 Get Started

Infinity, also available as a Python library, eliminates the need for a separate back-end server and all the complex communication settings. Using `pip install` and `import infinity`, you can quickly build a local AI application in Python, leveraging the world's fastest and the most powerful RAG database:

   ```bash
   pip install infinity-sdk==0.2.0
   ```

   ```python
   import infinity

   # Connect to infinity
   infinity_obj = infinity.connect("/path/to/save/your/files/to")
   db = infinity_obj.get_database("default_db")
   table = db.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector, 4, float"}})
   table.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
   table.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
   res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()
   print(res)
   ```

### 🛠️ Deploy Infinity as a separate server

If you wish to deploy a standalone Infinity server and access it remotely: 

See [Deploy infinity server](https://infiniflow.org/docs/dev/deploy_infinity_server).

#### 🛠️ Build from Source

See [Build from Source](https://infiniflow.org/docs/dev/build_from_source).

> 💡 For more information about Infinity's Python API, see the [Python API Reference](https://infiniflow.org/docs/dev/python_api_reference).

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

