---
sidebar_position: 1
slug: /search_guide
---
# Conduct a search

Full-text, vector, sparse vector, tensor, hybrid search.

---

## Overview

This document offers guidance on conducting a search within Infinity.

## Full-text search

### Work modes for full-text index

A full-text index must be built to perform a full-text search, and this index operates in two modes:

- **Real-time mode** - If created immediately after a table is created, a full-text index will be built on ingested data in real time, accumulating posting data within memory and flushing it to disk when a specified quota is reached.
- **Offline mode** - For data inserted before the creation of a full-text index, index will be built in offline mode using external sorting.

### Tokenizer

When creating a full-text index, you are required to specify a tokenizer/analyzer, which will be used for future full-text searches on the same column(s). Infinity has many built-in tokenizers. Except for the Ngram analyzer and the default standard analyzer, all other analyzers require dedicated resource files. Please download the appropriate files for your chosen analyzer from [this link](https://github.com/infiniflow/resource) and save it to the directory specified by `resource_dir` in the configuration file.

```yaml
[resource]
# Directory for Infinity's resource files, including dictionaries to be used by analyzers
resource_dir                  = "/var/infinity/resource"
```

The following are Infinity's built-in analyzers/tokenizers.

#### Standard analyzer

The standard analyzer is the default tokenizer and works best with Latin characters. It uses stemmer before outputting tokens segmented by white space, and `English` is the default stemmer. To specify a stemmer for a different language, use `"standard-xxx"`, where `xxx` is the language to use.

Supported language stemmers include: `Danish`, `Dutch`, `English`, `Finnish`, `French`, `German`, `Hungarian`, `Italian`, `Norwegian`, `Porter`, `Portuguese`, `Romanian`, `Russian`, `Spanish`, `Swedish`, and `Turkish`.

#### Ngram analyzer

A definition of N-gram can be found on [wikipedia](https://en.wikipedia.org/wiki/N-gram). Use `"ngram-x"` to select the Ngram analyzer, where `x` represents the value of `N`. For example, a common choice for full-text searches in code is `"ngram-3"`.

#### Simplified Chinese analyzer

Use `"chinese"` to select the simplified Chinese analyzer, which is a wrapper of [Jieba](https://github.com/yanyiwu/cppjieba) analyzer. Use `"chinese-fine"` to output fine-grained analyzer results.

#### Traditional Chinese analyzer

Use `"traditional"` to select the traditional Chinese analyzer, which essentially converts simplified Chinese into traditional Chinese based on the outputs of the [Jieba](https://github.com/yanyiwu/cppjieba) analyzer.

#### Japanese analyzer

Use `"japanese"` to select the Japanese analyzer, which is a wrapper of [mecab](http://taku910.github.io/mecab/).

#### Korean analyzer

Use `"korean"` to select the Korean tokenizer, which is a wrapper of [mecab](http://taku910.github.io/mecab/) but uses a different Korean dictionary.

#### RAG analyzer

The RAG analyzer is a bilingual tokenizer that supports Chinese (simplified and traditional) and English. It is a C++ adaptation of [RAGFlow's tokenizer](https://github.com/infiniflow/ragflow/blob/main/rag/nlp/rag_tokenizer.py), and its tokenization of Latin characters derives from [NLTK](https://www.nltk.org/api/nltk.tokenize.punkt.html).

This analyzer offers better recall for Chinese than the [Jieba](https://github.com/yanyiwu/cppjieba) analyzer, but lower tokenization throughput due to higher computational costs. Its English language processing involves an additional lemmatization step before stemming, different from that of the standard analyzer.

Use `"rag"` to select the RAG analyzer or `"rag-fine"` for fine-grained mode, which outputs tokenization results with the second highest score.

:::note
Both RAG tokenization and fine-grained RAG tokenization are used in RAGFlow to ensure high recall.
:::

#### IK analyzer

The IK analyzer is a bilingual tokenizer that supports Chinese (simplified and traditional) and English. It is a C++ adaptation of the [IK Analyzer](https://github.com/infinilabs/analysis-ik), which is widely used as a tokenizer by Chinese Elasticsearch users.

Use `"ik"` to select this analyzer, which is equivalent to the `ik_smart` option in the [IK Analyzer](https://github.com/infinilabs/analysis-ik), or `"ik-fine"` for fine-grained mode, which is equivalent to the `ik_max_word` option in the [IK Analyzer](https://github.com/infinilabs/analysis-ik).

#### Keyword analyzer

The keyword analyzer is a "noop" analyzer used for columns containing keywords only, where traditional scoring methods like `BM25` do not apply. It scores `0` or `1`, depending on whether any keywords are matched.

Use `"keyword"` to select this analyzer.

### Search and ranking syntax

Infinity supports the following syntax or full-text search expressions:

- Single term
- AND multiple terms
- OR multiple terms
- Phrase search
- CARAT opertor
- Sloppy phrase search
- Field-specific search
- Escape character

#### Single term

Example: `"blooms"`

#### AND multiple terms

- `"space AND efficient"`
- `"space && efficient"`
- `"space + efficient"`

#### OR multiple terms

- `"Bloom OR filter"`
- `"Bloom || filter"`
- `"Bloom filter"`

:::tip NOTE
`OR` is the default semantic in a multi-term full-text search unless explicitly specified otherwise.
:::

#### Phrase search

- `"Bloom filter"`
- `'Bloom filter'`

#### CARAT operator

Use `^` to boost the importance of a specific term. For example: `quick^2 brown` boosts the importance of `quick` by a factor of 2, making it twice as important as `brown`.

#### Sloppy phrase search

Example: `'"harmful chemical"~10'`

#### Field-specific search

Example: `"title:(quick OR brown) AND body:foobar"`

#### Escape character

Use `\` to escape reserved characters like `:` `~` `(` `)` `"` `+` `-` `=` `&` `|` `[` `]` `{` `}` `*` `?` `\` `/`. For example: `"space\-efficient"`.

### Scoring

Infinity offers `BM25` scoring and block-max `WAND` for dynamic pruning to accelerate multi-term searches. To *not* use `BM25` scoring, do either of the following:

- Set `"analyzer"` to `"keyword"` when creating index (to select the keyword analyzer).  
  *The returned score will then be based on whether keywords are matched.*
- Add `{"similarity": "boolean"}` as a search option.  
  *The scoring will then depend on the number of matched keywords.*

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

Conditional filters in Infinity must work through an index to facilitate search. The following two types of indexes in Infinity support conditional filters:

- **Secondary index**: Built on numeric or string columns. This index does not apply any tokenization to a string column when using conditional filters.
- **Full-text index**: Built on full-text columns. This index applies tokenization to the full-text column but does not trigger any relevance scoring procedure.

Infinity offers flexible filter syntax. Filters can be applied to either individual search paths, allowing each path to have different filter conditions, or to all search paths, allowing them to share the same filtering conditions.

### Secondary index filters

Infinity's secondary index supports real-time construction and search. Optimized for columns with high cardinality, this index can be built and searched rapidly even on numeric columns.

Filters based on secondary index can have arbitrary logical combinations. Supported expressions include `<`, `<=`, `>`, `>=`, `=`, `==`, `!=`, `in`, and `not in`.

### Full-text index filters

Infinity's full-text index supports conditional filtering through the `filter_fulltext` parameter. Full-text index filters are keyword-based and do not support the expressions available for secondary index filters. They use the `minimum_should_match` parameter to specify the minimum number of keywords that must be satisfied during fitlering.
