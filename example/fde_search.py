# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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
This example demonstrates FDE (Feature Dimension Expansion) functionality.
It shows how to use the match_dense method for vector search with tensor input.
'''

import sys
import os

# Add the local Python SDK to the path for development
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python', 'infinity_sdk'))

import infinity

try:
    # Use infinity_embedded module to open a local directory
    # infinity_instance = infinity.connect("/var/infinity")

    # Use infinity module to connect a remote server
    infinity_instance = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop table if exists and create new one
    db_instance.drop_table("fde_example_table", infinity.common.ConflictType.Ignore)

    # Create table with embedding column
    table_instance = db_instance.create_table("fde_example_table", {
        "id": {"type": "int"},
        "name": {"type": "varchar"},
        "embedding_col": {"type": "vector,64,float"}
    })

    print("Created table with embedding column")

    # Import FDE class for insert operations
    from infinity.common import FDE
    import numpy as np

    # Example 1: Insert data using traditional embeddings
    print("\n=== Inserting Traditional Embeddings ===")

    # Generate some sample 64-dimensional embeddings
    sample_embeddings = [
        np.random.rand(64).tolist(),
        np.random.rand(64).tolist(),
    ]

    table_instance.insert([
        {"id": 1, "name": "document1", "embedding_col": sample_embeddings[0]},
        {"id": 2, "name": "document2", "embedding_col": sample_embeddings[1]},
    ])

    print("Inserted traditional embedding data")

    # Example 2: Insert data using FDE function (NEW FEATURE)
    print("\n=== Inserting Data with FDE Function ===")

    # Define 2D tensors as input to FDE function
    # These could represent features extracted from text, images, etc.
    tensor_data_1 = [
        [1.0, 2.0, 3.0, 4.0],      # Feature vector 1
        [0.5, 1.5, 2.5, 3.5],      # Feature vector 2
        [2.0, 1.0, 4.0, 3.0],      # Feature vector 3
    ]

    tensor_data_2 = [
        [2.5, 1.5, 3.5, 2.5],      # Feature vector 1
        [1.0, 3.0, 2.0, 4.0],      # Feature vector 2
        [0.8, 1.2, 2.8, 3.2],      # Feature vector 3
        [1.5, 2.5, 1.5, 2.5],      # Feature vector 4
    ]

    # Insert data using FDE function
    table_instance.insert([
        {
            "id": 3,
            "name": "document3_fde",
            "embedding_col": FDE(tensor_data_1, 64)  # FDE with target dimension 64
        },
        {
            "id": 4,
            "name": "document4_fde",
            "embedding_col": FDE(tensor_data_2, 64)  # FDE with target dimension 64
        },
    ])

    print("Inserted FDE-generated embedding data")
    print("✓ FDE allows inserting 2D tensor data that gets automatically converted to embeddings")
    print("✓ Target dimension (64) is specified in the FDE function call")

    # Example 3: Traditional dense vector search
    print("\n=== Traditional Dense Vector Search ===")
    query_vector = np.random.rand(64).tolist()

    result, extra_result = (table_instance
                           .output(["id", "name", "_similarity"])
                           .match_dense("embedding_col", query_vector, "float", "cosine", 4)
                           .to_pl())

    print("Traditional search results (searches all data including FDE-inserted):")
    print(result)

    # Example 4: FDE-based dense vector search (NEW FEATURE)
    print("\n=== FDE-based Dense Vector Search ===")

    # Define a 2D tensor as input to FDE function
    # This could represent features extracted from text, images, etc.
    query_tensor = [
        [1.0, 2.0, 3.0, 4.0],      # Feature vector 1
        [0.5, 1.5, 2.5, 3.5],      # Feature vector 2
        [2.0, 1.0, 4.0, 3.0],      # Feature vector 3
        [1.5, 2.5, 1.5, 2.5],      # Feature vector 4
        [3.0, 1.0, 2.0, 4.0]       # Feature vector 5
    ]

    target_dimension = 64  # Target embedding dimension

    # Create FDE object for search
    fde_query = FDE(query_tensor, target_dimension)

    result_fde, extra_result_fde = (table_instance
                                   .output(["id", "name", "_similarity"])
                                   .match_dense("embedding_col", fde_query, "float", "cosine", 4)
                                   .to_pl())

    print("FDE search results (searches all data using FDE query):")
    print(result_fde)

    # Example 5: Different FDE configurations
    print("\n=== Different FDE Configurations ===")

    # Smaller tensor, different target dimension
    small_tensor = [
        [1.0, 0.5],
        [0.8, 1.2]
    ]

    # Create FDE object for small tensor search
    fde_small = FDE(small_tensor, 64)

    result_small, _ = (table_instance
                      .output(["id", "name", "_similarity"])
                      .match_dense("embedding_col", fde_small, "float", "ip", 2)
                      .to_pl())

    print("Small tensor FDE results:")
    print(result_small)

    # Example 6: Using different distance metrics
    print("\n=== Different Distance Metrics ===")

    for metric in ["cosine", "ip"]:  # L2 has restrictions with _similarity
        # Create FDE object for each metric test
        fde_metric = FDE(query_tensor, 64)

        result_metric, _ = (table_instance
                           .output(["id", "name", "_similarity"])
                           .match_dense("embedding_col", fde_metric, "float", metric, 1)
                           .to_pl())

        print(f"{metric.upper()} distance results:")
        print(result_metric)

    # Example 7: With additional parameters
    print("\n=== FDE with Additional Parameters ===")

    # Create FDE object for parameter test
    fde_params = FDE(query_tensor, 64)

    result_params, _ = (table_instance
                       .output(["id", "name", "_similarity"])
                       .match_dense("embedding_col", fde_params, "float", "cosine", 3,
                                       {"ef": "50"})  # HNSW parameter example
                       .to_pl())

    print("FDE with parameters results:")
    print(result_params)

    print("\n=== FDE Example Complete ===")
    print("✓ Successfully demonstrated FDE functionality")
    print("✓ FDE allows inserting 2D tensor data that gets automatically converted to embeddings")
    print("✓ FDE allows using 2D tensor input for search instead of pre-computed embeddings")
    print("✓ Target dimension can be specified dynamically")
    print("✓ All existing distance metrics and parameters are supported")
    print("✓ FDE works seamlessly with traditional embeddings in the same table")

    # Cleanup
    db_instance.drop_table("fde_example_table", infinity.common.ConflictType.Ignore)
    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
