---
sidebar_position: 1
slug: /search_guide
---
# Search usage guide

## Overview

Infinity offers powerful search capabilities. This page covers the search usage.

## Full-text search

### Work modes for full-text index

A full-text search will work if full text index is created. There are two kinds of work modes for full-text indexing:

- **Real-time mode** - If the full text index is created immediately after the table is created, then the full-text index will work in real-time mode if data is ingested at this time. Real-time index will accumulate posting data within memory and flush to disk if it reaches up the quota.
- **Offline mode** - If the full-text index is created after the data is inserted, then it will work in offline mode, where the full-text index is constructed through external sorting.

### Tokenizer

There are several built-in tokenizers within Infinity. With the exception of the default standard analyzer and ngram analyzer, everything else requires the resource file to be in the right place. Make sure to download [resource package](https://github.com/infiniflow/resource) and put it to correct directory according to `[resource]` configuration:

```yaml
[resource]
# Directory for Infinity's resource files, including the dictionary files used by the analyzer
resource_dir                  = "/var/infinity/resource"
```

You must specify a tokenizer when creating a full text index, but you don't need to specify one when querying, because the query will select the same tokenizer in the same columns.

- Standard analyzer: It's the default tokenizer, and is suitable for latin characters. Standard analyzer will just output tokens segmented by white spaces. It will also use stemmer before outputing, and `English` is the default stemmer. If you want to specify stemmers for other languages, use `standard-xxx` and `xxx` is the language you want to use. Currently, supported language stemmer includes: `Danish`, `Dutch`, `English`, `Finnish`, `French`, `German`, `Hungarian`, `Italian`, `Norwegian`, `Porter`, `Portuguese`, `Romanian`, `Russian`, `Spanish`, `Swedish`, `Turkish`.
- Ngram analyzer: The definition of Ngram could be referred to through [wikipedia](https://en.wikipedia.org/wiki/N-gram).  You must specify the number of  `N` when creating full text index if you want to use Ngram analyzer through `ngram-x` where `x` equals to the definition of `N`. For example, for code search, a typical choice is  `ngram-3` .
- Chinese analyzer: Use `chinese` to specify tokenizer for simplified Chinese. It's a wrapper of [Jieba](https://github.com/yanyiwu/cppjieba) analyzer. Use `chinese-fine` to output the fine-grained analyzer results.
- Traditional Chinese analyzer: Use `traditional` to specify tokenizer for traditional Chinese, which is actually a conversion between simplified Chinese and traditional Chinese based on the outputs of  [Jieba](https://github.com/yanyiwu/cppjieba) analyzer.
- Japanese analyzer: Use `japanese` to specify tokenizer for Japanese. It's a wrapper of [mecab](http://taku910.github.io/mecab/).
- Korean analyzer: Use `korean` to specify tokenizer for Korean. It's also a wrapper of [mecab](http://taku910.github.io/mecab/) but has different Korean dictionary.
- RAG analyzer: It's a C++ migration of tokenizer imported from [RAGFlow](https://github.com/infiniflow/ragflow/blob/main/rag/nlp/rag_tokenizer.py). It's a multilingual tokenizer and currently, both Chinese and English are well supported. RAG analyzer has better recall compared to [Jieba](https://github.com/yanyiwu/cppjieba) analyzer, but lower tokenization throughput due to much more expensive computation.  The English processing within RAG analyzer is also different from Standard analyzer, because it has an extra step of lemmatization before stemming, additionally, the tokenization of latin characters is a c++ migration of [NLTK](https://www.nltk.org/api/nltk.tokenize.punkt.html). RAG analyzer also supports fined grained mode through `rag-fine`, which will output tokenization results with the second highest score. In RAGFlow, both RAG tokenization and fine-grained RAG tokenization are used to guarantee the recall.
- Keyword analyzer: It's a noop analyzer. This is used if you have columns containing keywords only, and you don't want such traditional scoring approaches as `BM25`to take into effects, the score will return 0 or 1 according to whether any keywords are hit.

### Search and ranking

Infinity offers following syntax for full text search:

- Single term: `"blooms"`
- AND multiple terms: `"space AND efficient"`, `"space && efficient"` or `"space + efficient"`
- OR multiple terms: `"Bloom OR filter"`, `"Bloom || filter"` or just `"Bloom filter"` .
- Phrase search: `"Bloom filter" or 'Bloom filter'`
- CARAT operator: `^`: Used to boost the importance of a term, e.g., `quick^2 brown` boosts the importance of `quick` by a factor of 2, making it twice as important as `brown`.
- Sloppy phrase search: `'"harmful chemical"~10'`
- Field-specific search: `"title:(quick OR brown) AND body:foobar"`
- Escaping reserved characters: `"space\-efficient"` . `:` `~` `()` `""` `+` `-` `=` `&` `|` `[]` `{}` `*` `?` `\` `/` are reserved characters for search syntax.

`OR`  is the default semantic among multiple terms if user does not specify in search syntax. Infinity offers `BM25` scoring and block-max `WAND` for dynamic pruning to accelerate the multiple terms search processing. There are two approaches to bypass `BM25` scoring:

* Using `keyword` analyzer when creating index, then `BM25` will not be used and it will return the score based on whether keywords are hit.
* Specifying `similarity=boolean` during searching. Then the scoring is decided by the number of keywords hits.

## Dense vector search

Infinity's dense vector search is similar to the 'vector search' feature in many other vector databases. It supports two types of vector indexes: HNSW, which offers faster search performance, and IVF, which consumes less memory.

- Infinity optimizes its implementation of the HNSW index by incorporating locally-adaptive quantization, delivering much higher performance and a much smaller memory footprint than other HNSW implementations.
- Infinity's IVF index supports multiple data types, including `float32`, `float16`, `bfloat16`, `int8` and `uint8`. By implementing scalar quantization and product quantization, Infinity's IVF index achieves a significantly lower memory footprint than its peers.

For incoming streaming vectors, Infinity executes a brute-force search rather than building index first, ensuring a real-time search experience. The search results are from both the incoming streaming vectors and those indexed. This search strategy is applied autonomously, without the need for human intervention.

Infinity also has built-in support for multi-vector search. Each row in a table represents a document, comprising a collection of vectors that represent chunks of the document. Infinity builds a vector index on these vectors to retrieve the top K documents (rows). This model is applicable to many scenarios.

## Sparse vector search

Infinity supports real-time sparse vector search, which is based on a sparse vector index implemented using [Block-Max Pruning(BMP)](https://arxiv.org/abs/2405.01117).

## Tensor search

### Tensor vs. multi-vector

Tensors work in scenarios where embeddings of all tokens of each document need to be preserved. Tensor is another form of multi-vector, but *not* the multi-vector mentioned in dense vector search. They share similarities but are different in several aspects:

Multi-vectors are mainly used for dense vector search, while tensors are mainly used for tensor reranking. Tensor search is *not* recommended due to the potential for significant storage overhead when converting raw documents to tensors; tensors are more appropriate for reranking in a [hyprid search](#hybrid-search).

A multi-vector search remains a KNN search, where search results are aggregated using vector identifiers. In contrast, a tensor search uses 'Colxxx' models, such as ColBERT, ColPali, and ColQwen2. In this context, 'Col' denotes 'contextualized late interaction' and 'xxx' refers to specific models that can be added using a 'Col adapter'. Additionally, a tensor search uses the MaxSim method for similarity operations, which accumulates similarities of all embedding pairs between a query tensor and a document tensor.

### Tensor array

Tensor arrays are specifically used with late interaction models with a token limit. For example, the ColBERT model has a token limit of 128, meaning that it cannot generate tensors for documents exceeding this token limit. In such cases, a document is split into multiple chunks, with each chunk represented by a tensor and all tensors for that document stored as a tensor array. During search and reranking, MaxSim scores of all embedding pairs (between a query tensor and a chunk tensor) are calculated and aggregated to produce the final MaxSim score for each document.

### Performance tuning

Infinity offers two approaches for improving the performance of a tensor search:

- Binary quantization: Each float in a tensor is represented by a bit, occupying only 1/32 of the original tensor's space. Infinity supports direct insertion of tensor data resulting from binary quantization.
- Hamming distance: The MaxSim scores of tensor data inserted after binary quantization are calculated using the Hamming distance instead of the inner product, thereby accelerating retrieval.

## Hybrid search

Infinity allows you to integrate various search methods, including full-text search, dense vector search, and sparse vector search, into a hybrid search pipeline. For any search combination in a hybrid search, a fusion method is required to determine the final reranking results. The results can be further combined with other search paths. For instance, you can integrate dense vector search with sparse vector search to build the first-stage retrieval, apply a fusion reranker to this path, and integrate the reranking results with a full-text search path to build a second-stage retrieval.

Infinity offers three types of rerankers for fusion:

- `rrf` (Reciprocal rank fusion)  
  RRF is a method that combines multiple result sets with varying relevance indicators into a single result set. This method requires no tuning, and the relevance indicators need not be correlated to achieve high-quality results. It is particularly useful when you are uncertain of each retrieval path's relative importance.
- `weighted_sum`  
  The weighted sum approach assigns different weights to different retrieval paths, allowing you to emphasize specific paths. It is particularly useful when you are certain of each path's relative importance.
- `match_tensor`  
  The tensor-based rank fusion, where the final results are determined by the MaxSim operator between the query tensor and each document tensor.

## Conditional filters

Conditional filters in Infinity must work through an index to facilitate search. There are two types of indexes in Infinity that support conditional filters:

- **Secondary index**: Built on numeric or string columns. This index does not apply any tokenization to a string column when using conditional filters.
- **Full-text index**: Built on full-text columns. This index applies tokenization to the full-text column but does not trigger any relevance scoring procedure.

Infinity offers flexible filter syntax. Filters can be applied to either individual search paths, allowing each path to have different filter conditions, or to all search paths, allowing them to share the same filtering conditions.

### Secondary index filters

Infinity's secondary index supports real-time construction and search. Optimized for columns with high cardinality, this index can be built and searched rapidly even on numeric columns.

Filters based on secondary index can have arbitrary logical combinations. Supported expressions include `<`, `<=`, `>`, `>=`, `=`, `==`, `!=`, `in`, and `not in`.

### Full-text index filters

Infinity's full-text index supports conditional filtering through the `filter_fulltext` parameter. Full-text index filters are keyword-based and do not support the expressions available for secondary index filters. They use the `minimum_should_match` parameter to specify the minimum number of keywords that must be satisfied during fitlering.
