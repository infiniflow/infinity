# C++ 磁盘格式冻结规格（任务 #67）

> 目的：§2.6.1 要求「本次移植必须兼容 C++ 磁盘格式」。本文件冻结**可被 Rust 侧读写复现**的字节级规格，作为 #68（列/段存储）、#69（Catalog KV）、#70（HNSW/IVF）、#71（倒排索引）的施工依据。
>
> 规则：只记录**已在 C++ 源码中核对**的事实，并给出 `文件:行`；尚未核对的项列在 §6「待核对清单」，不得凭推测实现。
>
> 判定口径：C++ 侧按原路径能打开 Rust 写出的文件（`DESCRIBE / SELECT / SHOW / SEARCH` 结果一致），反向亦然。

---

## 1. 已验证：数据文件（列块 / 快照数据）通用头尾

**写入**：`src/storage/buffer/file_worker/data_file_worker_impl.cpp:69-116`（普通文件）与 `:119-155`（快照文件）使用**同一布局**，源码内注释即规格：

```
// File structure:
// - header: magic number
// - header: buffer size
// - data buffer
// - footer: checksum
```
(`data_file_worker_impl.cpp:68-72`)

| 偏移 | 类型 | 内容 |
|---|---|---|
| 0 | `u64` (LE) | `magic_number = 0x00dd3344`（`:73` / `:121`） |
| 8 | `size_t`（8 字节） | `buffer_size_`（分配的块大小；`:81`） |
| 16 | `u8[buffer_size]` | 数据区：前 `data_size` 字节为数据，其余**补零**到 `buffer_size`（`:83-107`） |
| 16+`buffer_size` | `u64` (LE) | `checksum`（当前实现写入 0；`:109-111`） |

**读取/校验**：`data_file_worker_impl.cpp:160-172` 先读 8 字节并断言 `== 0x00dd3344`，否则 `Status::DataIOError("Read magic error, … != 0x00dd3344.")`；`:217-220` 为内存镜像路径的同款校验。

**Rust 侧落地要点**：`magic` 与 `buffer_size` 均为小端定宽整数（架构无关）；`size_t` 必须固定按 8 字节读写（`x86_64` 目标），不可直接用 `usize` 的平台宽度语义之外的假设。

## 2. 已验证：Catalog / Meta KV = RocksDB + 自定义 MergeOperator

- 实现文件：`src/storage/catalog/kv_store.cppm`、`src/storage/catalog/kv_store_impl.cpp`；合并算子：`src/storage/common/rocksdb_merge_operator.cppm` / `_impl.cpp`。
- 结论：Rust 侧**必须改用 `rust-rocksdb`**（计划 §4.2 已列为「保留磁盘兼容」的回退方案），当前 `fjall`（纯 Rust LSM）与 C++ 目录**互不可读**。
- 迁移前需冻结（见 §6）：RocksDB `Options`/`ColumnFamily` 集合、比较器、`MergeOperator` 的 `Merge`/`FullMergeV2` 语义、以及键前缀命名空间。

## 3. 已验证：全文倒排索引的组成模块（待逐文件冻结细节）

`src/storage/invertedindex/` 下的构成（`ls` 实测）：

| 类别 | 文件 |
|---|---|
| 词典 | `dict_reader.cppm`（`:19 import :fst.fst`）、`fst/`（**自研 FST**） |
| postings 编码 | `format/`：`doc_list_encoder*`、`position_list_encoder*`、`posting_decoder*`、`inmem_posting_decoder*`、`posting_buffer*`、`short_list_optimize_util.*`、`posting_list_format.cppm`、`doc_list_format_option.cppm` |
| block-max / 打分 | `search/blockmax_leaf_iterator.cppm`、`search/blockmax_wand_iterator.cppm`、`search/bm25_ranker*` |
| 列长 | `search/column_length_io.cppm`（`FullTextColumnLengthReader::GetColumnLength|GetDocTermCount`）、`column_inverter*`（写入侧 `InvertColumn` 返回 `terms_once_->size()`，`:82-87`） |

**PoC F 结论（校验和）**：自研 FST 的校验和为 **CRC-32/IEEE 802.3**（`src/common/utility/crc.cppm:49`），而 Rust `fst` crate 为 **CRC32C + Snappy 掩码**（`fst-0.4.7/src/raw/crc32.rs:21-24`）→ **`Fst::Verify()` 互不通过**；边界谓词亦不同（crate `&&` vs C++ `||`，`fst.cppm:104`）。复用 crate 必须统一校验和或放弃 `Verify()`。

## 4. 已验证：BM25 打分公式（决定索引里必须存哪些统计量）

`src/storage/invertedindex/search/bm25_ranker_impl.cpp:25-34`：

```
k1 = 1.2, b = 0.75
smooth_idf = log(1 + (total_df - df + 0.5) / (df + 0.5))
smooth_tf  = (k1 + 1) * tf / (tf + k1 * (1 - b + b * column_len / avg_column_len))
score     += smooth_idf * smooth_tf * weight
```

统计量来源（`term_doc_iterator_impl.cpp:57-70`）：`(doc_cnt, doc_len) = column_length_reader->GetDocTermCount()`；`avg_column_len = doc_len / doc_cnt`；`total_df_ = doc_cnt`；每篇文档长度来自 `column_length_reader->GetColumnLength(doc_id)`。
→ **索引文件必须持久化**：每词的 `df`、每文档的 `column_len`、段的 `doc_cnt`、以及 postings 中的 `tf`。Rust 侧打分已按此实现（`fulltext_delete` 7/7 逐位一致）；剩余差异为**段级统计**。

## 5. 已验证：SEARCH 相关语义（影响索引读写正确性）

- 删除的行**仍计入**倒排统计（只从结果中过滤）——`fulltext_delete` 用例实测：`DELETE FROM ft_delete WHERE 1 = num;` 之后分数不变。
- `minimum_should_match` 为 Lucene 规格（计数 / 负计数 / 百分比 / `n<spec`），`operator=and` 等价于「全词命中」，`similarity=boolean` 时 score = 命中词数 × 字段/词权重。
- 查询内的 `bm25_param_delta_term|delta_phrase` 只影响 **block-max 上界**，不改变精确打分路径（`bm25_ranker_impl` 的 `delta` 仅用于 `BlockMaxBM25Score`）。

## 6. 待核对清单（实现 #68–#71 前必须逐项冻结）

| # | 待冻结项 | 从哪里读 |
|---|---|---|
| 1 | 数据库/表/索引的**目录与文件命名**（表目录下有哪些文件、扩展名） | `src/storage/meta/entry/*`（table/db entry）、`src/storage/new_txn/*`、`src/storage/buffer/file_worker/file_worker.cppm` |
| 2 | 段/块元数据（segment entry、block entry、`block_count`、版本号、行数） | `src/storage/meta/entry/block_entry*`、`segment_entry*` |
| 3 | 列式数据的**编码**（列块内的定长/变长编码、NULL 位图、向量/张量列布局） | `src/storage/column_vector/column_vector_impl.cpp`、`src/storage/segment/segment_iter*` |
| 4 | HNSW/IVF 索引**文件头与图数据布局** | `src/storage/index_hnsw.cpp(m)`、`index_ivf.cpp(m)`、`src/storage/index_base.cppm`、`src/storage/buffer/buffer_manager.cppm` |
| 5 | 倒排索引各文件（term dict / postings / block-max / column-length）**文件头、PForDelta 段布局、块边界** | `src/storage/invertedindex/format/*`（逐文件）、`src/storage/invertedindex/dict_reader_impl.cpp` |
| 6 | Catalog KV 的**键前缀 / 值编码 / MergeOperator 语义**、RocksDB 选项与 CF 集合 | `src/storage/catalog/kv_store_impl.cpp`、`src/storage/common/rocksdb_merge_operator_impl.cpp`、`src/storage/new_txn/new_txn_impl.cpp` |
| 7 | WAL/redo 记录格式与 checkpoint（跨底座恢复的契约） | `src/storage/wal/*`（若存在）、`src/storage/txn/*` |
| 8 | 二级索引（PGM-index）文件布局 | `src/storage/index_secondary.cpp(m)`、`third_party/pgm/` |

## 7. 当前 Rust 实现与规格的差距（诚实记录）

| 子系统 | 现状 | 差距 |
|---|---|---|
| 表数据 | 行文本 `row:<table>:<seq>` 存于 fjall | 未使用 §1 的块文件布局，未实现列式编码 |
| Catalog/Meta | fjall | 非 RocksDB，键空间不同（§2） |
| 向量索引 | 仅内存，`open` 时重建 | 未持久化，无 §6-4 的文件 |
| 全文索引 | 内存匹配 + 自研 BM25 | 未落盘，无 §6-5 的文件 |
| 二级索引 | 内存 BTree | 无 PGM 文件 |

> 结论：**§2.6.1 的四项均未达标**；本文件是施工前置件（任务 #67 的第一半），#68–#71 仍需按 §6 清单补齐字节级细节后再实现。
