# PLAID Index Implementation

This is a port of next-plaid (multi-vector retrieval) to Infinity, following the EMVB interface patterns.

## Overview

PLAID (Product Lookups for AI-Dense retrieval) is an efficient index structure for multi-vector (tensor) retrieval,
commonly used with ColBERT-style late interaction models. Each document is represented by multiple embeddings
(one per token), and search uses MaxSim scoring.

## Key Features

1. **Residual Quantization**: 2-bit or 4-bit quantization for compact storage
2. **Mmap Support**: Memory-mapped index files for efficient memory management
3. **Batch Processing**: Efficient batch search capabilities
4. **Incremental Updates**: Support for incremental index building


## Usage

### Create Index

```sql
CREATE INDEX idx_plaid ON table_name(tensor_column) USING PLAID WITH (nbits=4, n_centroids=1000);
```

Parameters:
- `nbits`: Quantization bits (2 or 4, default 4)
- `n_centroids`: Number of IVF centroids (0 = auto, default 0)

### Search

The index is automatically used for tensor column search with MATCH TENSOR.

## Architecture

### Index Structure

1. **K-means Centroids**: Coarse quantization for IVF structure
2. **Residual Quantization**: Fine-grained quantization of residuals
3. **IVF Lists**: Inverted index from centroids to documents
4. **Packed Residuals**: Compressed residual storage

### Search Pipeline

1. **IVF Probing**: Find top centroids for each query token
2. **Candidate Collection**: Gather documents from selected IVF lists
3. **Approximate Scoring**: Fast scoring using centroid distances
4. **Exact Reranking**: Decompress and compute precise MaxSim scores

### Mmap Support

Following Infinity's HNSW pattern, the index supports:
- `SaveToPtr()`: Save index to mmap'd memory
- `LoadFromPtr()`: Load index from mmap'd memory
- File worker manages mmap lifecycle
- Cache manager for hot index caching

## Integration with Infinity

### Index Type Registration

The index type `kPLAID` is registered in:
- `create_index_info.h`: Enum definition
- `create_index_info.cpp`: String conversion
- `file_worker_type.cppm`: File worker type
- `index_base_impl.cpp`: Serialization/deserialization

### NewTxn Integration

To fully integrate, add to `new_txn_index_impl.cpp`:
- Case handling for `IndexType::kPLAID`
- `PopulatePlaidIndexInner()` function
- Memindex dump/load for PLAID

### ChunkIndexMeta Integration

Add to `chunk_index_meta.cppm` and `chunk_index_meta_impl.cpp`:
- Case handling for `kPLAID` in file worker creation
- Index path management

## Comparison with EMVB

| Feature | EMVB | PLAID |
|---------|------|-------|
| Quantization | PQ/OPQ (8/16-bit) | Residual (2/4-bit) |
| Compression | Moderate | High |
| Accuracy | Higher | Good |
| Memory Usage | Higher | Lower |
| Mmap Support | No | Yes |
| Batch Search | Limited | Full |

## Performance Characteristics

- **Indexing**: O(N × K × D) for K-means, where N=embeddings, K=centroids, D=dimension
- **Search**: O(Q × K + C × D) where Q=query tokens, C=candidates
- **Memory**: ~1/8 to 1/16 of original with 4-bit or 2-bit quantization

