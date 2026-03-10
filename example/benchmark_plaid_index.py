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

"""
PLAID Index Benchmark

This benchmark tests PLAID index performance for multi-vector (tensor) retrieval.
It measures:
1. Data insertion throughput
2. Index creation time
3. Query latency and throughput

Usage:
    python benchmark_plaid_index.py [--docs N] [--dim D] [--tokens T] [--query Q]
"""

import infinity
import infinity.common
import infinity.index
import numpy as np
import time
import argparse
from typing import Tuple


class PlaidBenchmark:
    def __init__(self, host: str = "127.0.0.1", port: int = 23817):
        self.host = host
        self.port = port
        self.infinity_instance = None
        self.db_instance = None
        self.table_instance = None
        self.embedding_dim = 128
        self.table_name = "plaid_benchmark_table"
        
    def connect(self):
        """Connect to Infinity server."""
        print(f"Connecting to Infinity server at {self.host}:{self.port}...")
        try:
            self.infinity_instance = infinity.connect(
                infinity.common.NetworkAddress(self.host, self.port)
            )
            self.db_instance = self.infinity_instance.get_database("default_db")
            print("Connected successfully.")
        except Exception as e:
            print(f"Failed to connect: {e}")
            raise
        
    def setup_table(self, drop_existing: bool = True):
        """Create table for benchmark."""
        if drop_existing:
            print(f"Dropping existing table '{self.table_name}'...")
            self.db_instance.drop_table(self.table_name, infinity.common.ConflictType.Ignore)
        
        print(f"Creating table '{self.table_name}' with TENSOR(FLOAT, {self.embedding_dim})...")
        self.table_instance = self.db_instance.create_table(self.table_name, {
            "id": {"type": "integer"},
            "title": {"type": "varchar"},
            "embeddings": {"type": f"tensor,{self.embedding_dim},float"},
        })
        print("Table created.")
        
    def generate_document(self, doc_id: int, n_tokens: int) -> dict:
        """Generate a single document with random tensor embeddings."""
        # Generate random tensor data
        tensor_data = np.random.randn(n_tokens * self.embedding_dim).astype(np.float32).tolist()
        return {
            "id": doc_id,
            "title": f"Document_{doc_id}",
            "embeddings": tensor_data
        }
        
    def insert_data(self, n_docs: int, n_tokens_per_doc: int, batch_size: int = 1000) -> float:
        """
        Insert documents in batches.
        
        Returns:
            Total insertion time in seconds
        """
        print(f"\n{'='*60}")
        print(f"Inserting {n_docs} documents")
        print(f"  - Tokens per document: {n_tokens_per_doc}")
        print(f"  - Embedding dimension: {self.embedding_dim}")
        print(f"  - Batch size: {batch_size}")
        print(f"{'='*60}")
        
        total_records = 0
        start_time = time.time()
        
        for batch_start in range(0, n_docs, batch_size):
            batch_end = min(batch_start + batch_size, n_docs)
            batch_data = []
            
            for i in range(batch_start, batch_end):
                doc = self.generate_document(i, n_tokens_per_doc)
                batch_data.append(doc)
            
            # Insert batch
            batch_start_time = time.time()
            self.table_instance.insert(batch_data)
            batch_elapsed = time.time() - batch_start_time
            
            total_records += len(batch_data)
            elapsed = time.time() - start_time
            throughput = total_records / elapsed if elapsed > 0 else 0
            
            print(f"  Batch {batch_start//batch_size + 1}: "
                  f"Inserted {len(batch_data)} docs in {batch_elapsed:.3f}s | "
                  f"Total: {total_records}/{n_docs} | "
                  f"Throughput: {throughput:.1f} docs/sec", end='\r')
        
        total_elapsed = time.time() - start_time
        print()  # New line after progress
        
        final_throughput = n_docs / total_elapsed if total_elapsed > 0 else 0
        print("\nInsertion complete:")
        print(f"  Total time: {total_elapsed:.3f}s")
        print(f"  Average throughput: {final_throughput:.1f} docs/sec")
        print(f"  Total embeddings: {n_docs * n_tokens_per_doc}")
        
        return total_elapsed
        
    def create_plaid_index(self, nbits: int = 4, n_centroids: int = 0) -> float:
        """
        Create PLAID index on the embeddings column.
        
        Args:
            nbits: Quantization bits (2 or 4)
            n_centroids: Number of centroids (0 for auto)
            
        Returns:
            Index creation time in seconds
        """
        print(f"\n{'='*60}")
        print("Creating PLAID index")
        print(f"  - nbits: {nbits}")
        print(f"  - n_centroids: {n_centroids if n_centroids > 0 else 'auto (sqrt(N))'}")
        print(f"{'='*60}")
        
        params = {"nbits": str(nbits)}
        if n_centroids > 0:
            params["n_centroids"] = str(n_centroids)
            
        start_time = time.time()
        
        self.table_instance.create_index(
            "idx_plaid",
            infinity.index.IndexInfo(
                "embeddings",
                infinity.index.IndexType.PLAID,
                params
            ),
            infinity.common.ConflictType.Error
        )
        
        elapsed = time.time() - start_time
        print(f"Index created in {elapsed:.3f}s")
        
        return elapsed
        
    def benchmark_queries(self, n_queries: int, n_tokens_per_query: int, topn: int = 10) -> Tuple[float, float]:
        """
        Benchmark query performance.
        
        Args:
            n_queries: Number of queries to execute
            n_tokens_per_query: Number of tokens in each query
            topn: Number of top results to return
            
        Returns:
            (total_time, avg_latency) in seconds
        """
        print(f"\n{'='*60}")
        print("Benchmarking queries")
        print(f"  - Number of queries: {n_queries}")
        print(f"  - Tokens per query: {n_tokens_per_query}")
        print(f"  - TopN: {topn}")
        print(f"{'='*60}")
        
        # Generate query tensors
        queries = []
        for _ in range(n_queries):
            query_tensor = np.random.randn(n_tokens_per_query * self.embedding_dim).astype(np.float32).tolist()
            queries.append(query_tensor)
        
        # Warm-up: execute a few queries first
        print("Warming up with 5 queries...")
        for i in range(min(5, n_queries)):
            self.table_instance.output(["id"]).match_tensor(
                "embeddings", [queries[i]], "float", topn
            ).to_pl()
        
        # Benchmark
        print(f"Executing {n_queries} queries...")
        latencies = []
        
        start_time = time.time()
        for i, query in enumerate(queries):
            query_start = time.time()
            result, _ = self.table_instance.output(["id", "title"]).match_tensor(
                "embeddings", [query], "float", topn
            ).to_pl()
            query_elapsed = time.time() - query_start
            latencies.append(query_elapsed)
            
            if (i + 1) % 10 == 0 or i == n_queries - 1:
                progress = (i + 1) / n_queries * 100
                print(f"  Progress: {i+1}/{n_queries} ({progress:.1f}%)", end='\r')
        
        total_elapsed = time.time() - start_time
        print()  # New line
        
        # Calculate statistics
        latencies_ms = [latency * 1000 for latency in latencies]  # Convert to ms
        avg_latency = np.mean(latencies_ms)
        p50_latency = np.percentile(latencies_ms, 50)
        p95_latency = np.percentile(latencies_ms, 95)
        p99_latency = np.percentile(latencies_ms, 99)
        min_latency = np.min(latencies_ms)
        max_latency = np.max(latencies_ms)
        throughput = n_queries / total_elapsed if total_elapsed > 0 else 0
        
        print("\nQuery performance:")
        print(f"  Total time: {total_elapsed:.3f}s")
        print(f"  Throughput: {throughput:.1f} queries/sec")
        print("  Latency (ms):")
        print(f"    Min:   {min_latency:.3f}")
        print(f"    Avg:   {avg_latency:.3f}")
        print(f"    P50:   {p50_latency:.3f}")
        print(f"    P95:   {p95_latency:.3f}")
        print(f"    P99:   {p99_latency:.3f}")
        print(f"    Max:   {max_latency:.3f}")
        
        return total_elapsed, avg_latency / 1000  # Return in seconds
        
    def cleanup(self):
        """Clean up resources."""
        if self.db_instance:
            print(f"\nCleaning up table '{self.table_name}'...")
            try:
                self.db_instance.drop_table(self.table_name, infinity.common.ConflictType.Ignore)
            except Exception as e:
                print(f"Cleanup warning: {e}")
        if self.infinity_instance:
            try:
                self.infinity_instance.disconnect()
                print("Cleanup complete.")
            except Exception as e:
                print(f"Disconnect warning: {e}")
        
    def run_full_benchmark(self, n_docs: int, n_tokens_per_doc: int, 
                           n_queries: int, n_tokens_per_query: int,
                           batch_size: int = 1000, nbits: int = 4, 
                           n_centroids: int = 0, topn: int = 10):
        """Run complete benchmark workflow."""
        try:
            self.connect()
            self.setup_table(drop_existing=True)
            
            # Insert data
            insert_time = self.insert_data(n_docs, n_tokens_per_doc, batch_size)
            
            # Create index
            index_time = self.create_plaid_index(nbits, n_centroids)
            
            # Benchmark queries
            query_time, avg_latency = self.benchmark_queries(n_queries, n_tokens_per_query, topn)
            
            # Summary
            print(f"\n{'='*60}")
            print("BENCHMARK SUMMARY")
            print(f"{'='*60}")
            print("Configuration:")
            print(f"  Documents: {n_docs}")
            print(f"  Tokens per document: {n_tokens_per_doc}")
            print(f"  Embedding dimension: {self.embedding_dim}")
            print(f"  Total embeddings: {n_docs * n_tokens_per_doc}")
            print("\nResults:")
            print(f"  Insertion time:     {insert_time:.3f}s")
            print(f"  Index creation:     {index_time:.3f}s")
            print(f"  Query time ({n_queries} queries): {query_time:.3f}s")
            print(f"  Average latency:    {avg_latency*1000:.3f}ms")
            
        finally:
            self.cleanup()


def main():
    parser = argparse.ArgumentParser(
        description='PLAID Index Benchmark for Infinity',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('--host', default='127.0.0.1', help='Infinity server host')
    parser.add_argument('--port', type=int, default=23817, help='Infinity server port')
    parser.add_argument('--docs', type=int, default=10000, help='Number of documents to insert')
    parser.add_argument('--tokens', type=int, default=32, help='Number of tokens per document')
    parser.add_argument('--dim', type=int, default=128, help='Embedding dimension')
    parser.add_argument('--batch-size', type=int, default=1000, help='Insertion batch size')
    parser.add_argument('--queries', type=int, default=100, help='Number of queries to benchmark')
    parser.add_argument('--query-tokens', type=int, default=8, help='Number of tokens per query')
    parser.add_argument('--topn', type=int, default=10, help='TopN results per query')
    parser.add_argument('--nbits', type=int, default=4, choices=[2, 4], help='Quantization bits')
    parser.add_argument('--centroids', type=int, default=0, help='Number of centroids (0=auto)')
    parser.add_argument('--cleanup-only', action='store_true', help='Only cleanup and exit')
    
    args = parser.parse_args()
    
    benchmark = PlaidBenchmark(host=args.host, port=args.port)
    benchmark.embedding_dim = args.dim
    
    if args.cleanup_only:
        benchmark.connect()
        benchmark.cleanup()
        return
    
    print(f"\n{'#'*70}")
    print("# PLAID Index Benchmark")
    print(f"{'#'*70}")
    
    benchmark.run_full_benchmark(
        n_docs=args.docs,
        n_tokens_per_doc=args.tokens,
        n_queries=args.queries,
        n_tokens_per_query=args.query_tokens,
        batch_size=args.batch_size,
        nbits=args.nbits,
        n_centroids=args.centroids,
        topn=args.topn
    )
    
    print(f"\n{'#'*70}")
    print("# Benchmark Complete!")
    print(f"{'#'*70}\n")


if __name__ == '__main__':
    main()
