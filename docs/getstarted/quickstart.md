---
sidebar_position: 1
slug: /
---

# Quickstart

## Prerequisites

- CPU: x86_64 with AVX2
- OS: Linux with glibc &ge; 2.17
- Python: Python 3.10+

## Install Infinity locally using pip

Infinity, also available as a Python module, eliminates the need for a separate back-end server and all the complex communication settings. Using `pip install` and `import infinity`, you can quickly build a local AI application in Python, leveraging the world's fastest and the most powerful RAG database:

1. Install Infinity as a module:
   ```bash
   pip install infinity-sdk==0.2.1
   ```
2. Use Infinity to conduct a KNN search:
   ```python
   import infinity

   # Connect to infinity
   infinity_obj = infinity.connect("/path/to/save/to")
   db = infinity_obj.get_database("default_db")
   table = db.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector, 4, float"}})
   table.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
   table.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
   res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()
   print(res)
   ```

## Deploy Infinity as a separate server

If you wish to deploy Infinity with the server and client as separate processes: 

See [Deploy infinity server](https://infiniflow.org/docs/dev/deploy_infinity_server).

## 🛠️ Build from Source

See [Build from Source](https://infiniflow.org/docs/dev/build_from_source).

### Try our Python examples

- [Create table, insert data, and search](https://github.com/infiniflow/infinity/blob/main/example/simple_example.py)
- [Import file and export data](https://github.com/infiniflow/infinity/blob/main/example/import_data.py)
- [Delete or update data](https://github.com/infiniflow/infinity/blob/main/example/delete_update_data.py)
- [Conduct a vector search](https://github.com/infiniflow/infinity/blob/main/example/vector_search.py)
- [Conduct a full-text search](https://github.com/infiniflow/infinity/blob/main/example/fulltext_search.py)
- [Conduct a hybrid search](https://github.com/infiniflow/infinity/blob/main/example/hybrid_search.py)

## Python API reference

For detailed information about Infinity's Python API, see the [Python API Reference](../references/pysdk_api_reference.md).
