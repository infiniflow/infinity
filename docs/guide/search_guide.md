---
sidebar_position: 1
slug: /search_guide
---
# Search usage guide

## Overview

Infinity offers powerful search capabilities. This page covers the search usage.  These search operators are available:

- [Full text search](#full-text-search)
- [Dense Vector search](#dense-vector-search)
- [Sparse vector search](#sparse-vector-search)
- [Tensor search](#tensor-search)
- [Hybrid search](#hybrid-search)
- [Conditional filters](#conditional-filters)

## Full text search

Full text search will work if full text index is created. There are two kinds of working modes for full text indexing:

- Real time mode -- If the full text index is created immediately after the table is created, then the full-text index will work in real time mode if data is ingested at this time. Real time index will accumulate posting data within memory and flush to disk if it reaches up the quota.
- Offline mode -- If the full text index is created after the data is ingested, then it will work under offline mode, where the full text index is constructed through external sorting.

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
- RAG analyzer: It's a c++ migration of tokenizer imported from [RAGFlow](https://github.com/infiniflow/ragflow/blob/main/rag/nlp/rag_tokenizer.py). It's a multilingual tokenizer and currently, both Chinese and English are well supported. RAG analyzer has better recall compared to [Jieba](https://github.com/yanyiwu/cppjieba) analyzer, but lower tokenization throughput due to much more expensive computation.  The English processing within RAG analyzer is also different from Standard analyzer, because it has an extra step of lemmatization before stemming, additionally, the tokenization of latin characters is a c++ migration of [NLTK](https://www.nltk.org/api/nltk.tokenize.punkt.html). RAG analyzer also supports fined grained mode through `rag-fine`, which will output tokenization results with the second highest score. In RAGFlow, both RAG tokenization and fine-grained RAG tokenization are used to guarantee the recall.
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

Dense vector search is actually `Vector Search` in other vector databases. Infinity has two kinds of built-in vector indices : `HNSW` and `IVF`, `HNSW` has faster search performance while `IVF`has lower memory footprint. Additionally, Infinity offers `Vector Streaming Search` because vector indices will not be built on streaming fresh data due to high cost of index building. For these fresh data, a brute force strategy is applied directly to provide real time search.  As a result, each vector search response is a combination of results from streaming part and vector index parts. The vector index is maintained automatically.  You don't need to care about whether the search is performed through vector index or brute forcing.

`HNSW` index is an optimized implementation introducing locally-adaptive quantization, to deliver a much higher performance together with lower memory footprint compared with `HNSW` index from other vector databases.

`IVF` index offers embedding storage with different types including `float32`, `float16`, `bfloat16`, `int8` and `uint8`, combined with scalar quantization as well as product quantization, `IVF` delivers much lower memory footprint.

Infinity offers built-in support for `multi-vector` search,  which makes modeling such scenarios conveniently:  each row in the table indicates a single document, where each document is composed of multiple chunks, and for each chunk, we get corresponding embedding. Vector index is built over these embeddings while we hope the vector search could provide the top K document in the results.

## Sparse vector search

Sparse vector search is based on sparse vector index, implemented according to [Block-Max Pruning(BMP)](https://arxiv.org/abs/2405.01117) . Infinity also offers real time search for sparse vector by default.

## Tensor search

`Tensor` is actually multi-vector. It is used to serve the scenario where embedding of all the tokens of each document needs to be preserved. However, it's different from `multi-vector` in dense vector search. Because `multi-vector` search is still a general KNN search,  and the search results will be aggregated according to vector identifiers, while `tensor` is dedicated to `ColXXX` models where `Col` means `contextualized late interaction`and `xxx` refers to a series models that can be added with `Col adaptor`, such as `ColBERT`, `ColPali`,`ColQwen2`,...,etc.  For these late interaction models, `MaxSim` is a special similarty operation which accumulate the similarity among all pair embeddings between query tensor and document tensor.

As a result, these types are similar but have different meaning:

- `multivector`: Used for dense vector search.

```json
{
	"name": "multivector_column",
	"type": "multivector,4,float",
	"default": [[0.0, 0.0, 0.0, 0.0], [0.0, 0.0, 0.0, 0.0]]
},  
```

- `tensor`: Used for tensor search and tensor reranking.

```json
{
	"name": "tensor_column",
	"type": "tensor,4,float",
	"default": [[1.0, 0.0, 0.0, 0.0], [1.1, 0.0, 0.0, 0.0]]
}, 
```

- `tensorarray`: It's used to serve scenarios where there are some late interaction models having token limits. For example, the ColBERT model has a token limit of 128, which means it cannot generate tensor for documents with tokens larger than 128.  In that case, the document is splitted into multiple chunks, each encoded into a tensor. And tensors for each document can be stored into a `tensorarray` . During search and reranking, the `MaxSim` of each tensor pair is computed at first, the eventual `MaxSim` score for each document is the accumulation of all `MaxSim` scores among all tensor pairs.

```json
{
	"name": "tensorarray_column",
	"type": "tensorarray,2,float",
	"default": [[[1.0, 1.0], [1.0, 1.0]], [[1.0, 1.0]]]
}   
```

Due to the storage explosion caused by tensor compared with raw document data, tensor search is not recommended to use directly. It's more suitable to be used as a reranker, which will be described in the hybrid search part.

Infinity offers two approaches to improve tensor performance:

- Binary quantization : It uses single bit to represent each float in the tensor and therefore consumes 1/32 of the space of the original tensor. Infinity supports the insertion of data directly into the database as a result of such quantization.
- Hamming distance : If tensor data is inserted into Infinity after binary quantization, the `MaxSim` operation is performed on hamming distance, which is a kind of acceleration compared with the original inner product between embeddings.

## Hybrid search

Infinity offers you the flexibility to assemble any search methods into the hybrid search pipeline, including full text search, vector search and sparse vector search. For any combinations, a fusion must be attached to decide the final ranking results. The outputs of fusion can also be further combined with other search ways, for example, you can assemble dense vector search and sparse vector search to build the first stage retrieval, attached with a fusion reranker, then continue assemble the fused results with full text search to build the second stage retrieval.

Infinity offers three kinds of rerankers for the fusion:

- `rrf`: Reciprocal rank fusion
  RRF is a method for combining multiple result sets with varying relevance indicators into a single result set. It requires no tuning, and the relevance indicators need not be related to achieve high-quality results. RRF is particularly useful when you are uncertain of the relative importance of each retrieval method.
- `weighted_sum`
  The weighted sum approach assigns different weights to different retrieval ways, allowing you to emphasize specific ways. This is particularly useful when you are certain of each path's relative importance.
- `match_tensor`
  The `tensor` based rank fusion, the final results are decided by the `MaxSim` operator between the tensor of query and tensor of each document.

## Conditional filters

Infinity offers comprehensive filters for search operators. Filters must work through indices. There are two kinds of indices to provide conditional filters in Infinity : One is secondary index, which is used to build on columns with type of numeric and string. For string type columns, secondary index does not apply any tokenization. The other is full text index, which is used to build over columns with type of text.  The filters based on full text index will apply tokenization on the column content, but it will not trigger any relevance scoring procedure.

Infinity offers flexible filter syntax, all kinds of filters can be applied to each search way separately,  which means each search way can have different filter conditions; The filter can also be applied to all search ways, which means all search ways share the same filter conditions.

### Filter based on secondary index

Secondary index is built on numeric and string columns. It's optimized for columns with high cardinality. As a result, even if the column has numeric types, the index could still be constructed and searched very fast. Secondary index has built-in real time support.

Filters based on secondary index can have any number of logical combinations, supported expressions include `<`, `<=`, `>`, `>=`, `=`, `==`, `!=`, `in`, `not in` .

### Filter based on full text index

Full text index provides filters through `filter_fulltext` , since it's a keyword based filter, it does not provide similar expressions as filters based on secondary index.  It has a special parameter of `minimum_should_match` , which specifies how many keywords should be satisfied at least during fitlering.
