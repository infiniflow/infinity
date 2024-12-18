---
sidebar_position: 1
slug: /release_notes
---

# Release notes

Key features, improvements and bug fixes in the latest releases.

## v0.5.0

Released on December 18, 2024.

### New features

- Implements a highly available architecture based on shared storage, featuring one write node and multiple read nodes.
- Implements 8-bit scalar quantization and product quantization in the IVF index.
- Supports using Hamming distance as metric when matching/searching binary vectors.
- Implements query result caching and pagination.
- Supports specifying comments when creating database or index.
- Supports regular expressions (regex), including md5，substring, lower, upper, ltrim, rtrim, trim, char_posiition, sqrt, round, ceil, floor, IN, isnan, isinf, and isfinite
- Enhances the search performance for full-text searches with multiple terms/clauses.
- Supports using `MinimumShouldMatch` as a full-text filter.
- When using BM25 for full-text search or using L2/IP/Cosine distance metric for dense vector, sparse vector, or tensor search, you can use the `threshold` option to filter results.
- Supports ARM64 CPUs.
- Tokenizers: Adds an IK analyzer and enhances the performance of the RAG analyzer.
- Integrated into RAGFlow.

### Documentation

#### Added documents

- [Conduct a search](https://infiniflow.org/docs/dev/search_guide)
- [Set up an Infinity cluster](https://infiniflow.org/docs/dev/set_up_cluster)