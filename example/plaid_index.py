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
This example demonstrates PLAID index creation and search for multi-vector (tensor) retrieval.
PLAID is an IVF-based index with residual quantization optimized for tensor search.
'''

import infinity
import sys
import numpy as np

def generate_random_tensors(n_docs, n_tokens_per_doc, dim):
    """Generate random tensor data for testing."""
    data = []
    for _ in range(n_docs):
        # Each document has n_tokens_per_doc embeddings
        doc_tensor = np.random.randn(n_tokens_per_doc * dim).astype(np.float32).tolist()
        data.append(doc_tensor)
    return data

try:
    # Use infinity module to connect a remote server
    infinity_instance = infinity.connect(infinity.common.NetworkAddress("127.0.0.1", 23817))

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("plaid_test_table", infinity.common.ConflictType.Ignore)

    # Create a table with tensor column
    # PLAID index works on TENSOR type columns (multi-vector)
    embedding_dim = 32
    table_instance = db_instance.create_table("plaid_test_table", {
        "id": {"type": "integer"},
        "embeddings": {"type": f"tensor,{embedding_dim},float"},  # Tensor column for multi-vector
    })

    # Create a PLAID index on the tensor column
    # Parameters:
    # - nbits: quantization bits (2 or 4, default 4)
    # - n_centroids: number of centroids for IVF (0 for auto: sqrt(N))
    # SQL equivalent: CREATE INDEX idx_plaid ON plaid_test_table(embeddings) USING PLAID WITH (nbits=4, n_centroids=100)
    table_instance.create_index("idx_plaid",
                                infinity.index.IndexInfo("embeddings",
                                                         infinity.index.IndexType.PLAID,
                                                         {
                                                             "nbits": "4",
                                                             "n_centroids": "16"  # Small for demo; use ~sqrt(N) in practice
                                                         }), infinity.common.ConflictType.Error)

    # Insert data
    # Each row is a document with multiple token embeddings
    n_docs = 100
    n_tokens_per_doc = 8
    
    print(f"Inserting {n_docs} documents, each with {n_tokens_per_doc} embeddings of dimension {embedding_dim}...")
    
    np.random.seed(42)
    for i in range(n_docs):
        # Generate random tensor for this document
        doc_tensor = np.random.randn(n_tokens_per_doc * embedding_dim).astype(np.float32).tolist()
        table_instance.insert([{"id": i, "embeddings": doc_tensor}])

    print("Insertion complete.")

    # Search with PLAID index
    # Generate a query tensor (multiple token embeddings)
    query_tensor = np.random.randn(n_tokens_per_doc * embedding_dim).astype(np.float32).tolist()
    
    # Perform tensor search using maxsim method
    # SQL equivalent: SELECT id FROM plaid_test_table SEARCH MATCH TENSOR (embeddings, query_tensor, 'float', 'maxsim', 'topn=5')
    print("\nSearching for top 5 documents...")
    res, extra_result = table_instance.output(["id"]).match_tensor("embeddings",
                                                                   [query_tensor],
                                                                   "float", 5).to_pl()
    print("Search results:")
    print(res)
    
    if extra_result is not None:
        print("\nExtra results:")
        print(extra_result)

    # Clean up
    db_instance.drop_table("plaid_test_table", infinity.common.ConflictType.Error)
    infinity_instance.disconnect()

    print('\nPLAID index test completed successfully!')
    sys.exit(0)

except Exception as e:
    print(f"Error: {str(e)}")
    import traceback
    traceback.print_exc()
    sys.exit(-1)
