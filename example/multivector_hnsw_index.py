# Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

'''
This example is about connecting to the local infinity instance, creating table, inserting data, 
creating hnsw index on multivector column, and searching data with both brute force and HNSW methods
'''

import infinity
import argparse
import sys
import numpy as np
import polars as pl
from polars.testing import assert_frame_equal as pl_assert_frame_equal

try:

    parser = argparse.ArgumentParser(description="Benchmark Infinity")
    parser.add_argument('--crabitq', action='store_true', default=False)
    args = parser.parse_args()

    # Use infinity_embedded module to open a local directory
    # infinity_instance = infinity.connect("/var/infinity")

    #  Use infinity module to connect a remote server
    infinity_instance = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # generate normalized vectors
    def generate_normalized_vectors(shape, dtype=np.float32):
        x = np.random.randn(*shape).astype(dtype)
        x /= np.linalg.norm(x, axis=1, keepdims=True)  # L2 normalize
        return x

    # calculate maxsim between two multivectors
    def maxsim(query_multivec: np.ndarray, data_multivec: np.ndarray) -> float:
        maxsim_sum = 0.0
        for i in range(query_multivec.shape[0]):
            maxsim = -float('inf')
            for j in range(data_multivec.shape[0]):
                sim = float(np.dot(query_multivec[i], data_multivec[j]))
                if sim > maxsim:
                    maxsim = sim
            maxsim_sum += maxsim
        return maxsim_sum

    # Generate test data
    row_count = 32
    embedding_dimension = 4
    topK = 3
    
    # Generate query multivector
    query_multivector = generate_normalized_vectors((2, embedding_dimension))
    
    # Generate data multivectors
    data_multivec = [generate_normalized_vectors((3, embedding_dimension)) for i in range(row_count)]
    
    # Calculate maxsims and create expected result
    maxsims = [(i, maxsim(query_multivector, data_multivec[i])) for i in range(row_count)]
    maxsims.sort(key=lambda x: x[1], reverse=True)
    print("Max similarities:", maxsims)
    topK_maxsims = maxsims[:topK]
    
    # Convert list of tuples to dictionary format for Polars DataFrame
    topK_res = pl.DataFrame({"c1": [item[0] for item in topK_maxsims], "SIMILARITY": [item[1] for item in topK_maxsims]})
    # Cast SIMILARITY column to float32
    topK_res = topK_res.with_columns(pl.col("SIMILARITY").cast(pl.Float32))

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    # "CREATE TABLE my_table (c1 INT, c2 MULTIVECTOR(FLOAT, 4))"
    table_instance = db_instance.create_table("my_table", {
        "c1": {"type": "integer"},
        "c2": {"type": "multivector,4,float"},
    })

    # Insert data
    for i in range(row_count):
        table_instance.insert([{
            "c1": i,
            "c2": data_multivec[i],
        }])

    # Brute force search
    brute_force_res, extra_result = table_instance.output(["c1", "SIMILARITY()"]).match_dense(
        "c2", query_multivector.flatten(), "float", "ip", topK).to_pl()
    print("Brute force result:")
    print(brute_force_res)
    
    # Compare brute force result with expected result
    pl_assert_frame_equal(brute_force_res, topK_res)

    # Create a hnsw index on the table
    # "CREATE INDEX index1 ON my_table (c2) USING HNSW WITH (M = 16, ef_construction = 200, metric = ip)"
    table_instance.create_index("index1",
                                infinity.index.IndexInfo("c2",
                                                         infinity.index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "200",
                                                             "metric": "ip"
                                                         }), infinity.common.ConflictType.Error)
    if args.crabitq:
        table_instance.optimize("index1", {"compress_to_rabitq": "true"})

    # Search with hnsw index
    hnsw_res, extra_result = table_instance.output(["c1", "SIMILARITY()"]).match_dense(
        "c2", query_multivector.flatten(), "float", "ip", topK).to_pl()
    print("HNSW result:")
    print(hnsw_res)
    
    # Compare HNSW result with brute force result
    pl_assert_frame_equal(brute_force_res, hnsw_res)

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
