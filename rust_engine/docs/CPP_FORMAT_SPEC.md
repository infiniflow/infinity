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
| 1 | 数据库/表/索引的**目录与文件命名**（表目录下有哪些文件、扩展名） | **已冻结 → §8.1** |
| 2 | 段/块元数据（segment entry、block entry、`block_count`、版本号、行数） | **已冻结 → §8.2**（元数据在 RocksDB KV + `version` 文件，**不存在** `SegmentEntry/BlockEntry` 结构文件） |
| 3 | 列式数据的**编码**（列块内的定长/变长编码、NULL 位图、向量/张量列布局） | **已冻结 → §8.3** |
| 4 | HNSW/IVF 索引**文件头与图数据布局** | `src/storage/index_hnsw.cpp(m)`、`index_ivf.cpp(m)`、`src/storage/index_base.cppm`、`src/storage/buffer/buffer_manager.cppm` |
| 5 | 倒排索引各文件（term dict / postings / block-max / column-length）**文件头、PForDelta 段布局、块边界** | `src/storage/invertedindex/format/*`（逐文件）、`src/storage/invertedindex/dict_reader_impl.cpp` |
| 6 | Catalog KV 的**键前缀 / 值编码 / MergeOperator 语义**、RocksDB 选项与 CF 集合 | **键空间已冻结 → §8.2**（值编码 / MergeOperator 仍待） |
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

---

# 8. 已验证：目录/文件命名、段块元数据、块内列式编码（关闭 §6 的 1/2/3）

> 本节由源码逐行核对产出（`file:line` 为证）。**重要更正**：C++ 树中已无 `src/storage/meta/entry/{block_entry,segment_entry}*`（仅剩 `data_access_state.cppm`），`SegmentEntry`/`BlockEntry` 类不存在；段/块元数据改为 **RocksDB KV + 每块的 `version` 文件**，实现位于 `src/storage/catalog/meta/*`。

## 8.0 全局目录

`src/main/infinity_impl.cpp:100-107`（默认根 `/var/infinity`）：

| 配置 | 默认值 | 说明 |
|---|---|---|
| `data_dir` | `<root>/data` | 列块/索引数据文件根 |
| `catalog_dir` | `<root>/catalog` | **RocksDB** 目录（§2），`storage_impl.cpp:295` 打开 |
| `wal_dir` | `<root>/wal` | WAL |
| `temp_dir` | `<root>/tmp` | 临时 |
| `persistence_dir` | `<root>/persistence` | 对象持久化 |

数据目录下只有 `db_{db_id}/…` 一棵相对路径树（相对 `data_dir`）。

## 8.1 清单 1：目录 / 文件命名

权威正则（C++ 自带校验，`src/storage/catalog/meta/meta_tree_impl.cpp:1004-1006`）：

```
列数据: ^(?:db_(\d+)/tbl_(\d+)/seg_(\d+)/blk_(\d+)/(?:version|(\d+)\.col|col_(\d+)_out))$
索引:   ^(?:db_(\d+)/tbl_(\d+)/idx_(\d+)/seg_(\d+)/(?:chunk_(\d+)\.idx|ft_(\d+)\.(\w+)))$
```

| 层级 | 相对路径 | 依据 |
|---|---|---|
| database | `db_{db_id}` | `db_meta_impl.cpp:231` |
| table | `db_{db_id}/tbl_{table_id}` | `table_meta_impl.cpp:435-436` |
| segment | `db_{db_id}/tbl_{table_id}/seg_{segment_id}` | `block_meta_impl.cpp:215,239` |
| block | `db_{db_id}/tbl_{table_id}/seg_{segment_id}/blk_{block_id}` | 同上 |
| table-index | `db_{db_id}/tbl_{table_id}/idx_{index_id}` | `table_index_meta_impl.cpp:99-102` |
| segment-index | `db_{db_id}/tbl_{table_id}/idx_{index_id}/seg_{segment_id}` | `segment_index_meta_impl.cpp:282-288` |

**block 目录内**：

| 文件名 | 含义 | 依据 |
|---|---|---|
| `version` | 块版本文件（行可见性），**无 magic/无 checksum** | `block_version.cppm:37,39`；`block_meta_impl.cpp:84-90` |
| `{column_id}.col` | 某列数据文件，`column_id` 是**十进制列 id**（`ColumnDef::id()`），非列序号 | `column_meta_impl.cpp:56,109,280`；`new_txn_data_impl.cpp:2312` |
| `col_{column_id}_out` | 变长列的 outline 文件（仅 `VectorBufferType==kVarBuffer` 才建） | `column_meta_impl.cpp:78,131,220` |

文件顺序（列文件枚举/导出）＝各列文件按列定义 id 升序（每列 `.col` 后跟可选 `_out`）在前，`version` 在最后：`new_catalog_static_impl.cpp:1336-1361`。

**index 目录内**：

| 索引类型 | 文件名 | 依据 |
|---|---|---|
| Secondary / SecondaryFunctional / HNSW / IVF / BMP / EMVB / SMVE / PLAID | `chunk_{chunk_id}.idx` | `chunk_index_meta_impl.cpp:55,290/304/…/717` |
| FullText | `{base_name}.pos` / `.dic` / `.len` | `index_defines.cppm:55-58`；`chunk_index_meta_impl.cpp:701-703` |
| FullText `base_name` | `ft_{rowid:016x}`；合并后 `ft_{rowid:016x}_{row_count:x}` | `new_txn_index_impl.cpp:860,2239` |
| PLAID 全局质心（额外） | `plaid_centroids.bin`（segment-index 目录） | `segment_index_meta_impl.cpp:383,408` |

**创建顺序**：database 只写 KV（`db_meta_impl.cpp:48-61`）；segment 只写 KV（`table_meta_impl.cpp:237-241`）；block 先写 KV 再用 `block_meta.InitSet()` 建 `version`（`block_meta_impl.cpp:72-97`），随后按列定义顺序建 `{col_id}.col`（+ 变长列的 `col_{col_id}_out`）（`column_meta_impl.cpp:49-92`）；index 先写 table-index KV，再按需 `MakeDirectory` 建 segment-index 目录，逐 chunk 建 `chunk_{id}.idx`（`chunk_index_meta_impl.cpp:104-262`）。

> Rust 落地要点：目录/文件名全部为**无前导零十进制 id**，唯 full-text 的 `ft_{rowid:016x}` 为 16 位小写 16 进制。

## 8.2 清单 2：段 / 块元数据

新 catalog **没有**序列化的 `SegmentEntry/BlockEntry` 文件。元数据分两处：① RocksDB KV（id/commit_ts/tag/列定义 JSON/chunk_info JSON）；② 每块的 `version` 文件（行可见性与行数）。

### KV 键（`src/storage/common/kv_code_impl.cpp`；`MetaType` 见 `meta_type.cppm:21-45`）

分隔符统一 `|`，值多为十进制字符串（注明 JSON/binary 者除外）。

| 语义 | 键格式 | 值 | 依据 |
|---|---|---|---|
| DB 名→id | `catalog\|db\|{db_name}\|{commit_ts}` | `db_id` | `kv_code_impl.cpp:37` |
| DB tag | `db\|{db_id}\|{tag}` | `comment` / `next_table_id` | `:41`；`db_meta_impl.cpp:50,306` |
| Table 名→id | `catalog\|tbl\|{db_id}\|{table_name}\|{commit_ts}` | `table_id` | `:45-47` |
| Table 列定义 | `tbl\|col\|{db_id}\|{table_id}\|{column_name}\|{commit_ts}` | **ColumnDef JSON** | `:112-114`；`table_meta_impl.cpp:298-299` |
| Table tag | `tbl\|{db_id}\|{table_id}\|{tag}` | `comment`,`next_segment_id`,`next_column_id`,`next_index_id`,`unsealed_segment_id`,`ft_index_cache` | `:54-56`；`table_meta_impl.cpp:761` |
| Segment 列表 | `catalog\|seg\|{db_id}\|{table_id}\|{segment_id}` | `commit_ts` | `:129-131` |
| Segment tag | `seg\|{db_id}\|{table_id}\|{segment_id}\|{tag}` | `first_delete_ts`,`fast_rough_filter` | `:135-138`；`segment_meta_impl.cpp:164-166` |
| Block 列表 | `catalog\|blk\|{db_id}\|{table_id}\|{segment_id}\|{block_id}` | `commit_ts` | `:139-141` |
| Block tag | `blk\|{db_id}\|{table_id}\|{segment_id}\|{block_id}\|{tag}` | `lock`,`fast_rough_filter`(binary/base64) | `:145-151`；`block_meta_impl.cpp:245-248` |
| Block 列 tag | `blk_col\|{db}\|{tbl}\|{seg}\|{blk}\|{column_id}\|{tag}` | 值 | `:152-159` |
| Table index 名→id | `catalog\|idx\|{db_id}\|{table_id}\|{index_name}\|{commit_ts}` | `index_id` | `:58-60` |
| Table index tag | `idx\|{db_id}\|{table_id}\|{index_id}\|{tag}` | `index_base`=**IndexBase JSON**；`segment_update_ts` | `:68-71`；`table_index_meta_impl.cpp:92` |
| Segment index 列表 | `idx_seg\|{db}\|{tbl}\|{idx}\|{segment_id}` | `commit_ts` | `:73-76` |
| Segment index tag | `idx_seg\|…\|{segment_id}\|{tag}` | `next_chunk_id`,`mem_index`,`ft_info` | `:82-88` |
| Chunk index 列表 | `idx_chunk\|{db}\|{tbl}\|{idx}\|{seg}\|{chunk_id}` | `commit_ts` | `:90-96` |
| Chunk index tag | `idx_chunk\|…\|{chunk_id}\|{tag}` | `chunk_info`=**JSON** `{base_name,base_row_id,row_count,term_count,index_size}` | `:103-109`；`chunk_index_meta_impl.cpp:59-65` |
| 全局系统 tag | 裸键 `next_database_id`,`next_table_id`,`meta\|version` | 十进制/版本串 | `new_txn_impl.cpp:231-241`；`kv_code_impl.cpp:241` |
| Drop 墓碑 | `drop\|…` | drop ts | `kv_code_impl.cpp:243-293` |

> `DBMetaKey::ToString()` 等（`meta_key_impl.cpp:45-126`）只是调试/JSON 表示，落库用的是上表键。**行数不存 KV**，由 `version` 文件推导。

### `version` 文件字节布局（`BlockVersion`）

写入 `block_version_impl.cpp:108-165`；读取 `:167-184`；字段 `block_version.cppm:24-34,81-85`。`BlockOffset = uint16_t`、`TxnTimeStamp = uint64_t`（`infinity_type.cppm:24,34`）。

| 偏移（累积） | 类型 | 内容 |
|---|---|---|
| 0 | `u16` LE | `create_size` = `created_.size()` |
| 2 | `u64` LE | 第 1 个 `CreateField::create_ts_` |
| 10 | `i64` LE | 第 1 个 `CreateField::row_count_`（**累积**行数） |
| … | 每 CreateField 16 字节 | 共 `create_size` 个（`block_version_impl.cpp:32-54`） |
| `2+16·create_size` | `u16` LE | `capacity` = `deleted_.size()`（= 8192） |
| +2 | `u64` LE × capacity | 每行 delete_ts；`0` = 未删除，否则为删除提交时间戳 |

`deleted_` 长度构造时固定为 block 容量（`block_version.cppm:41`）；`GetRowCount(begin_ts)` 二分（`:90-106`），`GetCommitRowCount` 返回 `(offset, delta)`（`:71-88`）。**无 magic / 无 checksum**（`version_file_worker_impl.cpp:66-86`）。直接打开路径 `{data_dir}/db_{}/tbl_{}/seg_{}/blk_{}/version`（`kv_utility_impl.cpp:162-168`；`meta_tree_impl.cpp:702-708`）。

### 行数 / 列数 / 容量

| 量 | 来源 | 依据 |
|---|---|---|
| block 行数 | `version` 的 `created_`/`deleted_` | `kv_utility_impl.cpp:144-186` |
| segment 行数 | 各 block 行数求和 | `kv_utility_impl.cpp:188-203`；`segment_meta_impl.cpp:241-272` |
| block 容量 | `DEFAULT_BLOCK_CAPACITY = 8192` | `block_meta.cppm:41`；`default_values.cppm:50` |
| segment 容量 | `1024*8192`（8M 行）、`DEFAULT_BLOCK_PER_SEGMENT = 1024` | `default_values.cppm:69-70` |
| 列数 | 运行时由 table 列定义 count 得出，**不落盘** | `segment_meta_impl.cpp:319-328` |
| block_id / segment_id 分配 | `max+1` / table tag `next_segment_id` | `segment_meta_impl.cpp:201`；`table_meta_impl.cpp:275,786-796` |

`RowID` 布局（`row_id.h:36-47`，小端）：u32 `segment_offset_` + u32 `segment_id_`；`BLOCK_OFFSET_SHIFT=13`、`BLOCK_OFFSET_MASK=0x1FFF`（`default_values.cppm:51-52`）。

## 8.3 清单 3：块内列式编码

| 文件 | 容器 | 字节布局 |
|---|---|---|
| `{col_id}.col` | `DataFileWorker` | §1 通用头尾：`u64 magic(0x00dd3344)` + `u64 buffer_size` + `data[buffer_size]` + `u64 checksum(0)`（`data_file_worker_impl.cpp:66-118`）。`buffer_size = data_cap_size + null_size` 且 `SetDataSize(buffer_size)`（`new_txn_data_impl.cpp:2375`），故无补零 |
| `col_{col_id}_out` | `VarFileWorker` | **无头无尾**，直接 append 原始字节（`var_file_worker_impl.cpp:77-94`；读取 `:113-133`） |

尺寸（`C = block_capacity = 8192`）：`data_cap_size = C * DataType::Size()`，**boolean** 例外 `data_cap_size = (C+7)/8`（`column_meta_impl.cpp:56-63`；写出侧 `new_txn_data_impl.cpp:2314-2321`）；`null_size = (C+7)/8`。

`.col` 数据区布局（源码注释即规格，`column_vector_impl.cpp:273-275`）：

```
[ data_region : data_cap_size 字节 ][ null_bitmap : null_size 字节 ]
```

- 读取：从 `data_ptr_ + data_cap_size` 解析位图（`column_vector_impl.cpp:276-287`）；写出：在 `data_cap_size` 处写位图（`new_txn_data_impl.cpp:2356-2371`）。
- **位图语义：bit=1 表示非 NULL，bit=0 表示 NULL**（`column_vector.cppm:75-78`）。
- 定宽类型：本机小端（x86_64 LE）原始值逐行紧密排列，`slot = DataType::Size()`。

slot 宽度（`logical_type.cpp:183-255` 的 `type_size[]`；`DataType::Size()` `data_type.cpp:227-238`）：

| 类型 | slot | 备注 |
|---|---|---|
| Boolean | 1 | 用 `kCompactBit`，实际占 `(C+7)/8` 位 |
| TinyInt/SmallInt/Integer/BigInt | 1/2/4/8 | |
| HugeInt / Decimal | 16 | |
| Float / Double | 4 / 8 | |
| Float16 / BFloat16 | 2 / 2 | |
| Varchar | 16 | 见下 |
| Date / Time | 4 / 4 | |
| DateTime / Timestamp / Interval | 8 | |
| Array | 8 | 变长，outline |
| UUID | 16 | |
| Embedding | `dim * elem_width`（bit→`(dim+7)/8`） | **内联定长，无 outline**（`embedding_info.h:52`；`embedding_type.h:129-137`） |
| RowID / Mixed | 8 / 16 | |
| Tensor / TensorArray / MultiVector | 8 / 8 / 8 | 变长，outline |
| Sparse / Json | 16 / 16 | 变长，outline |
| Null / Missing | 0 | `kCompactBit` |

`GetVectorBufferType` 分类（`column_vector_impl.cpp:159-194`）：`kVarBuffer = {Array, Sparse, Varchar, Json, MultiVector, Tensor, TensorArray}`；`kCompactBit = {Boolean, Null, Missing}`；其余 `kStandard`。

变长类型 slot 内部结构（编译期 `#pragma pack(1)`）：

| 类型 | slot 布局 | 依据 |
|---|---|---|
| Varchar | `u64 length_:24` + `union{ char inline[13] \| { char prefix[5]; u64 file_offset } }`，`sizeof=16`；`length<=13` 内联，否则体在 outline 的 `file_offset` | `varchar.h:25-27,31-68` |
| MultiVector / Tensor / TensorArray / Array | `u64 { count:16; file_offset:48 }`，`sizeof=8` | `multi_vector_type.h:23-33`；`tensor_type.h:23-30`；`tensor_array_type.h:28-36`；`array_type.h:23-29` |
| Sparse | `i64 nnz_` + `i64 file_offset_`，`sizeof=16` | `sparse_type.h:26,76-77` |
| Json | `u64 length_` + `u64 file_offset_`，`sizeof=16` | `json_type.h:23-26` |
| Embedding | 无槽结构，直接内联 `dim*width` 个本机元素 | `column_vector.cppm:712-727` |

outline 文件（`col_{id}_out`）：内容 = 各次 `Append` 的原始字节按序首尾相接（`var_buffer_impl.cpp:28-46,85-104`）；`Append` 返回的 `offset`（= 追加前累积大小）写入 slot 的 `file_offset_`；**文件无索引、无头**，寻址全靠 slot 字段。Sparse 的 outline 内容 = `indices`（`nnz * index_width`）紧跟 `data`（`nnz * data_width`）（`vector_buffer.cppm:161-184`；`sparse_info.h:55-65`）。

## 8.4 Byte-level 实现清单（Rust 照此读写）

1. 数据根 `{data_dir}`；路径节点全十进制：`db_{u64}/tbl_{u64}/seg_{u32}/blk_{u16}`；索引 `…/idx_{index_id}/seg_{seg_id}`。
2. 每个 block 目录必含 `version`（最后写）；列文件 `{col_id}.col`，变长列另有 `col_{col_id}_out`。
3. `version`（**裸数据，无头**）：`u16 create_size` → `create_size × (u64 create_ts, i64 cum_row_count)` → `u16 capacity(=8192)` → `capacity × u64 delete_ts`。
4. `{col_id}.col`：`u64 magic=0x00dd3344` → `u64 buffer_size` → `buffer_size` 字节（前 `data_cap_size` 数据、随后 `(C+7)/8` null 位图）→ `u64 checksum=0`；总长 `8+8+buffer_size+8`。
5. `data_cap_size = C*slot_size`（boolean 用 `(C+7)/8`），`C=8192`。
6. `.col` 数据区逐行紧密 raw LE；变长类型 slot 存 count/offset，体在 `col_{col_id}_out`。
7. null 位图在 `.col` 数据区之后，长度 `(C+7)/8`，**bit=1 非空**。
8. outline 文件**纯拼接、无头尾**，仅变长类型列存在。
9. 索引 chunk：`chunk_{chunk_id}.idx`；全文 `{ft_<rowid:016x>}.{pos,dic,len}`；PLAID 另有 `plaid_centroids.bin`。
10. block/table/index 元数据在 RocksDB（`catalog_dir`），键见 §8.2；行数不由 KV 存储，只从 `version` 推导。

## 8.5 未核实 / 存疑

| 项 | 状态 | 需读/验证 |
|---|---|---|
| 变长 slot 的 16/48 位域在 u64 中的位序（`{count:16; offset:48}`） | **VERIFY**：源码未断言，依赖编译器位域分配；Clang/GCC x86_64 小端通常低 16 位=count、高 48 位=offset。Rust 侧应以 C++ 实际写出的文件回读校验 | `multi_vector_type.h:23-24` 等 |
| `.pos/.dic/.len` 内部格式 | 属 §6-5，**未核** | `src/storage/invertedindex/format/*`、`dict_reader_impl.cpp` |
| `chunk_{id}.idx` 内部头/图布局 | 属 §6-4，**未核** | `src/storage/index_hnsw*`、`*_file_worker_impl.cpp` |
| `ColumnMeta` 分别传 `column_idx`（`block_meta_impl.cpp:340`）与 `column_id`（`new_catalog_static_impl.cpp:1345,1354`） | **潜在不一致**：文件按 `col_def->id()` 命名；当 id==序号时无碍。格式本身不受影响 | 确认列 id 是否恒等于定义顺序 |
| 快照文件布局 | 与运行期同布局（`data_file_worker_impl.cpp:120-150`） | 若需支持快照再读 `snapshot_info.*` |
| WAL / checkpoint 记录格式 | 属 §6-7，**未核**（`wal_dir`、`wal.log` 前缀见 `default_values.cppm:132-133`） | `src/storage/wal/*` |
