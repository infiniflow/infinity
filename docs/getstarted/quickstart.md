---
sidebar_position: 1
slug: /
---

# Quickstart

## Prerequisites

- CPU: x86_64 with AVX2 support.
- OS: Linux with glibc version 2.17 or higher.
- Python: Python 3.10+ or higher.

## Install embedded Infinity using pip

The Infinity-embedded SDK allows you to embed Infinity into Python applications, eliminates the need for a separate back-end server. By using `pip install infinity-embedded-sdk` and `import infinity_embedded`, you can quickly develop a local AI application in Python, leveraging the fastest and most powerful RAG database in the world:

1. Install the Infinity-embedded SDK:
   ```bash
   pip install infinity-embedded-sdk==0.4.0.dev2
   ```
2. Use Infinity to conduct a dense vector search:
   ```python
   import infinity_embedded

   # Connect to infinity
   infinity_object = infinity_embedded.connect("/absolute/path/to/save/to")
   # Retrieve a database object named default_db
   db_object = infinity_object.get_database("default_db")
   # Create a table with an integer column, a varchar column, and a dense vector column
   table_object = db_object.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector, 4, float"}})
   # Insert two rows into the table
   table_object.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
   table_object.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
   # Conduct a dense vector search
   res = table_object.output(["*"])
                     .match_dense("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2)
                     .to_pl()
   print(res)
   ```

## Deploy Infinity in client-server mode

For scenarios where you prefer running Infinity with the server and client as separate processes, see [Deploy infinity server](https://infiniflow.org/docs/dev/deploy_infinity_server).

## Build from Source

If you wish to build Infinity from source for customization or development purposes, see the [Build from Source](https://infiniflow.org/docs/dev/build_from_source) guide.

### Try our Python examples

Try the following links to explore practical examples of using Infinity in Python:

- [Create table, insert data, and search](https://github.com/infiniflow/infinity/blob/main/example/simple_example.py)
- [Import file and export data](https://github.com/infiniflow/infinity/blob/main/example/import_data.py)
- [Delete or update data](https://github.com/infiniflow/infinity/blob/main/example/delete_update_data.py)
- [Conduct a vector search](https://github.com/infiniflow/infinity/blob/main/example/vector_search.py)
- [Conduct a full-text search](https://github.com/infiniflow/infinity/blob/main/example/fulltext_search.py)
- [Conduct a hybrid search](https://github.com/infiniflow/infinity/blob/main/example/hybrid_search.py)

## Python API reference

For detailed information about the capabilities and usage of Infinity's Python API, see the [Python API Reference](../references/pysdk_api_reference.md).
