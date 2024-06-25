---
sidebar_position: 1
slug: /
---

# Quickstart

## Prerequisites

- CPU: &ge; 4 cores, x86_64 with AVX2, or aarch64
- RAM: &ge; 16 GB
- Disk: &ge; 50 GB
- OS: Linux with glibc &ge; 2.17
- Python: Python 3.10+

## Deploy Infinity locally using pip

Infinity, also available as a Python library, eliminates the need for a separate back-end server and all the complex communication settings. Using `pip install` and `import infinity`, you can quickly build a local AI application in Python, leveraging the world's fastest and the most powerful RAG database:

1. Install the Infinity SDK:

   ```bash
   pip install infinity-sdk==0.2.0
   ```
2. Write a Python script to conduct a KNN search:

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

## Deploy Infinity as a separate server

If you wish to deploy a standalone Infinity server and access it remotely: 

See [Deploy infinity server](https://infiniflow.org/docs/dev/deploy_infinity_server).

## 🛠️ Build from Source

See [Build from Source](https://infiniflow.org/docs/dev/build_from_source).

## Install a Python client


```bash
pip3 install infinity-sdk==0.2.0
```

### Try our Python examples

- [Create table, insert data, and search](../../example/simple_example.py)
- [Import file and export data](../../example/import_data.py)
- [Delete or update data](../../example/delete_update_data.py)
- [Conduct a vector search](../../example/vector_search.py)
- [Conduct a full-text search](../../example/fulltext_search.py)
- [Conduct a fused search](../../example/fusion_search.py)
- [ColBERT reranker examples](../../example)


## Python API reference

For detailed information about Infinity's Python API, see the [Python API Reference](../references/pysdk_api_reference.md).
