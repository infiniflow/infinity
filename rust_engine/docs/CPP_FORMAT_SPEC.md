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
| 4 | HNSW/IVF 索引**文件头与图数据布局** | **已冻结 → §9.1 / §9.2**（两文件均**无 magic/version/checksum**，靠 IndexBase JSON + 列定义解释） |
| 5 | 倒排索引各文件（term dict / postings / block-max / column-length）**文件头、PForDelta 段布局、块边界** | **已冻结 → §9.3**（`.dic`=TermMeta+FST、`.pos`=逐 term 块、`.len`=裸 `u32[]`；无 magic/checksum） |
| 6 | Catalog KV 的**键前缀 / 值编码 / MergeOperator 语义**、RocksDB 选项与 CF 集合 | **键空间已冻结 → §8.2**（值编码 / MergeOperator 仍待） |
| 7 | WAL/redo 记录格式与 checkpoint（跨底座恢复的契约） | `src/storage/wal/*`（若存在）、`src/storage/txn/*` |
| 8 | 二级索引（PGM-index）文件布局 | `src/storage/index_secondary.cpp(m)`、`third_party/pgm/` |

## 7. 当前 Rust 实现与规格的差距（诚实记录）

| 子系统 | 现状 | 差距 |
|---|---|---|
| 表数据 | 行文本 `row:<table>:<seq>` 存于 fjall | **字节级布局已实现**（`infinity-storage::cpp_format`：数据文件框架 / `version` 文件 / `[data_region][null_bitmap]` 列体 / outline / 块目录读写，含逐字节单测）；**尚未接入执行器的读写路径** |
| Catalog/Meta | fjall | 非 RocksDB，键空间不同（§2） |
| 向量索引 | 仅内存，`open` 时重建 | 未持久化，无 §6-4 的文件 |
| 全文索引 | 内存匹配 + 自研 BM25 | 未落盘，无 §6-5 的文件 |
| 二级索引 | 内存 BTree | 无 PGM 文件 |

> 结论：**§2.6.1 的四项均未达标**；本文件是施工前置件（任务 #67 的第一半）。§6 的 #1/#2/#3 已冻结（见 §8），并已在 `cpp_format` 中落成可读写、可逐字节验证的实现；#68 的剩余部分是把该格式**接入执行器的表数据路径**，#69–#71 仍需按 §6 清单补齐字节级细节后再实现。

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

> **Varchar slot 的字节序（实现按此落盘）**：slot 共 16 字节 = `word0`（低 24 位 = `length`，其后 40 位 = payload 的前 5 字节，即 `prefix_[5]`）+ `word1`（= `file_offset`，指向 `col_{id}_out` 的追加偏移）。因此 `word0 = length \| (prefix[0]<<24) \| (prefix[1]<<32) \| …`，`word1 = offset`。NULL 行整槽置零、不追加 outline 内容。（C++ 对 `length<=13` 走 `inline_[13]` 分支，Rust 侧一律走 outline 分支——`file_offset` 在 `length>13` 时才是权威来源，短串的 inline 分支待接执行器时再补。）
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
| `.pos/.dic/.len` 内部格式 | **已冻结 → §9.3** | — |
| `chunk_{id}.idx` 内部头/图布局 | **已冻结 → §9.1（HNSW）/ §9.2（IVF）** | — |
| `ColumnMeta` 分别传 `column_idx`（`block_meta_impl.cpp:340`）与 `column_id`（`new_catalog_static_impl.cpp:1345,1354`） | **潜在不一致**：文件按 `col_def->id()` 命名；当 id==序号时无碍。格式本身不受影响 | 确认列 id 是否恒等于定义顺序 |
| 快照文件布局 | 与运行期同布局（`data_file_worker_impl.cpp:120-150`） | 若需支持快照再读 `snapshot_info.*` |
| WAL / checkpoint 记录格式 | 属 §6-7，**未核**（`wal_dir`、`wal.log` 前缀见 `default_values.cppm:132-133`） | `src/storage/wal/*` |

---

# 9. 已验证：HNSW / IVF 索引与全文倒排索引的字节布局（关闭 §6 的 4/5）

> 由 `src/storage/knn_index/**`、`src/storage/invertedindex/**`、`src/storage/buffer/file_worker/**` 逐行核对产出。**多字节定宽整数**除注明 `WriteInt`（大端）者外均为**本机小端（x86_64 LE）**、按 `struct` 字段原样 `memcpy`/`Append`；**变长整数为 LEB128**（无符号，7 位组，低位在前）。

## 9.0 通用读写约定

| 约定 | 实现 | 依据 |
|---|---|---|
| 定宽整数 | `Append(&x, sizeof(x))` / `memcpy`，**本机 LE**，无对齐填充 | `serialize.h:37-49` |
| 变长整数 `WriteVInt/WriteVLong` | **LEB128** | `file_writer_impl.cpp:55-76`；`vbyte_compressor.cppm:60-127` |
| `WriteInt/WriteLong` | **大端**（仅少数 spilling 字段） | `file_writer_impl.cpp:48-53,64-67` |
| `size_t` | 8 字节（x86_64），格式中当 `u64` LE | §1 |

> **陷阱**：索引 chunk 文件走 `*FileWorker::WriteToFileImpl`（**无外层头/尾包装**，`index_file_worker.cppm:30-53`、`file_worker_impl.cpp:142-204`），文件内容 = 序列化函数的直接输出。

## 9.1 清单 4a：HNSW `chunk_{chunk_id}.idx`

入口：`chunk_index_meta_impl.cpp:55`（文件名 `chunk_{}.idx`）、`:176-188`（kHnsw → `HnswFileWorker`）。写出 = `HnswHandler::SaveToPtr`（`hnsw_file_worker_impl.cpp:95-103`），读入断言读到文件尾（`hnsw_alg.cppm:541-543`）→ **无 magic/checksum/版本号，内容须精确等于下述布局**。

顶层布局（顺序即偏移）：

| # | 类型 | 字段 | 依据 |
|---|---|---|---|
| 1 | `u64` | `M_` | `hnsw_alg.cppm:99-103` |
| 2 | `u64` | `ef_construction_` (= max(M, ef_construction)) | `:99-103,507` |
| 3 | `u64` | `cur_vec_num` | `data_store.cppm:180-189` |
| 4 | `u64` | `VecStoreMeta`：**plain = `dim`** | `plain_vec_store.cppm:63-74` |
| 5 | `u64,u64,i32,i32` | `GraphStoreMeta`：`Mmax0`,`Mmax`,`max_layer_`,`enterpoint_` | `graph_store.cppm:74-80` |
| 6 | VecStoreInner | 向量数据（9.1.2） | `plain_vec_store.cppm:103-113` |
| 7 | GraphStoreInner | 图数据（9.1.3） | `graph_store.cppm:172-219` |
| 8 | `u32[cur]` | `labels_`（= SegmentOffset = rowid.segment_offset） | `data_store.cppm:555-558`；`infinity_type.cppm:35` |

- `Mmax0/Mmax = GetMmax(M) = {2*M, M}`（`hnsw_alg.cppm:69`）。
- `SaveToPtr`/`LoadFromPtr` **不含** `chunk_size_`/`max_chunk_n_`（仅内存 spill 路径 `Save/Load` 有，`data_store.cppm:164-178` vs `:180-189`）；Rust 按 `cur_vec_num` 推导。
- **单 inner 不变式**：`LoadFromPtr` 只读 1 个 inner，其 `chunk_size = 2^ceil(log2(cur_vec_num))`（`data_store.cppm:219-235`），数据量按 `cur_vec_num` 计算 → 仅当 `cur_vec_num ≤ block_size_` 时可正确回读。默认 `HNSW_BLOCK_SIZE = 8192`（`default_values.cppm:180`；`index_hnsw_impl.cpp:164,180-181`）。**VERIFY**：确认 chunk 行数恒 ≤ `block_size_`。

### 9.1.2 向量区 `VecStoreInner`

`DataStoreInner::SaveToPtr`（`data_store.cppm:540-559`）逐 chunk：`VecStoreInner::SaveToPtr` → `GraphStoreInner::SaveToPtr` → append `labels`。

| encode | Meta 额外字段 | 向量体 | 依据 |
|---|---|---|---|
| plain (`kPlain`) | 仅 `dim(u64)` | 每 chunk：`chunk_size × dim × sizeof(elem)` 原始 elem | `plain_vec_store.cppm:63-74,103-113` |
| LVQ (`kLVQ`) | `dim(u64)` + `double mean[dim]`(+可选 cache) | 每 chunk：`chunk_size × compress_data_size` | `lvq_vec_store.cppm:104-110,340-350` |
| Rabitq | `origin_dim(u64)` + `elem[dim*dim]`(旋转矩阵) + `elem[dim]`(rot centroid) | 每 chunk 同上 | `rabitq_vec_store.cppm:182-185,522-534` |
| sparse | `max_dim(u64)` 等 | 见文件 | `sparse_vec_store.cppm:30-88` |

### 9.1.3 图区 `GraphStoreInner`（关键字节结构）

```
struct VertexL0 { LayerSize layer_n_; char* layers_p_; VertexListSize neighbor_n_; VertexType neighbors_[]; }
struct VertexLX { VertexListSize neighbor_n_; VertexType neighbors_[]; }
```
（`LayerSize=VertexListSize=i32`，`VertexType=i32`；`graph_store.cppm:31-40`）

- 步长：`level0_size_ = sizeof(VertexL0) + 4*Mmax0`、`levelx_size_ = sizeof(VertexLX) + 4*Mmax`（`graph_store.cppm:45-46`）。**已实测确认（不再是 VERIFY）**：用 g++ 按 `graph_store.cppm:32-40` 的字段顺序编译量测得到 `sizeof(VertexL0)=24`（`offsetof`：`layer_n_`=0、`layers_p_`=8、`neighbor_n_`=16、`neighbors_`=20；`layer_n_` 后 4 字节为对齐填充）、`sizeof(VertexLX)=4`。因此 `M=16`（`Mmax0=32, Mmax=16`）时 `level0_size=152`、`levelx_size=68`。Rust 侧常量见 `infinity-storage::knn_format::{VERTEX_L0_SIZE, VERTEX_LX_SIZE}`。
- 顶点按 `vertex_i` 密集排列；顶点 `v` 第 `l≥1` 层邻居紧跟 `v->layers_p_`，偏移 `(l-1)*levelx_size`（`graph_store.cppm:244-256,440-446`）。
- 写盘（`graph_store.cppm:172-219`）：先 `u64 layer_sum = Σ_v layer_n_`；再逐 chunk 写 `chunk_size*level0_size` 的 level0 区——写前把 `layer_n_>0` 的顶点的 `layers_p_` **原地替换为 u64 字节偏移**（相对 layerx 拼接区）；最后逐 chunk 追加 layerx 段。
- 读盘（mmap 路径 `graph_store.cppm:459-471`）：`layer_start_ = ptr + cur_vertex_n*level0_size`。
- 邻接表：level0 邻居 = `i32[neighbor_n_]`（≤`Mmax0`），高层 = `i32[neighbor_n_]`（≤`Mmax`）；邻居是**顶点 id**，rowid 由末尾 `labels_` 给出。

## 9.2 清单 4b：IVF `chunk_{chunk_id}.idx`

同样无 magic/checksum；文件名同为 `chunk_{}.idx`（`chunk_index_meta_impl.cpp:55,163-173`）。读写 = `IVFIndexInChunk::SaveIndexInner/ReadIndexInner` → `IVF_Index_Storage::Save/Load`（`ivf_index_file_worker_impl.cpp:55-76`；`ivf_index_storage.cpp:99-114`）。

| # | 类型 | 字段 |
|---|---|---|
| 1 | `u32` | `row_count_` |
| 2 | `u32` | `embedding_count_` |
| 3 | `u32` | `embedding_dimension` |
| 4 | `u32` | `centroids_num` |
| 5 | `f32[num*dim]` | 质心（始终 f32，行主序） |
| 6 | parts | 见下 |

（`ivf_index_storage.cpp:52-57,100-104`）

- **文件不自描述**：`column_logical_type_`、`embedding_data_type_`、`ivf_option_`（metric/storage/bits/subspace）**不落盘**，读时由 IndexBase JSON + 列定义提供（`:107-114`）。

parts 区（先写 base 的 `IVF_Parts_Storage_Info::Save`，再逐 part 写）：

| storage | base | 每 part（0..num-1） |
|---|---|---|
| kPlain | 无 | `PartStorage::Save` + `chunk_num × dim × sizeof(elem)` 原始向量 |
| kScalarQuantization | `f32[dim] common_vec_a` + `f32[dim] common_vec_b` | `PartStorage::Save` + `sq_data(embedding_num*sq_bytes)` + `dot_v_e(embedding_num*f32)`；`sq_bytes = dim`(8bit) 或 `(dim+1)/2`(4bit) |
| kProductQuantization | `u32 real_subspace_centroid_num` + `f32[dim*that]` + `f32[that*subspace_num]` | `PartStorage::Save` + `PQ_Code_Storage::Save` |

（`ivf_index_storage_parts_impl.cpp:96-107,162-163,256-263,355-367,485-497,521-540,560-571,672-688,904-916`）

`IVF_Part_Storage::Save` 通用前缀 = `u32 embedding_num_` → `u32[embedding_num_]`（各向量的 `SegmentOffset`）。`PQ_Code_StorageB<T>::Save` = `u64 last_code_id_` → `u32 storage_size` → `T[storage_size]`。

## 9.3 清单 5：全文倒排 `.pos` / `.dic` / `.len`

文件集与命名：`{base_name}.{dic,pos,len}`，`base_name = ft_{rowid:016x}`（合并后加 `_{row_count:x}`）；后缀常量见 `index_defines.cppm:55-58`。写出 `MemoryIndexer::Save`（`memory_indexer_impl.cpp:491-544`）/ `TupleListToIndexFile`（`:661-778`）；读入 `DiskIndexSegmentReader`（`disk_segment_reader_impl.cpp:45-93`）、`.len` 由 `FullTextColumnLengthReader`（`column_length_io_impl.cpp:40-67`）。

### 9.3.1 全局

| 项 | 结论 |
|---|---|
| magic/checksum/版本 | **三个文件都没有**（`memory_indexer_impl.cpp:491-544`） |
| `optionflag_t`（term/doc payload、position、tf、block_max、realtime） | **不落盘**，在 catalog 的 **IndexBase JSON** 里，读时经 `IndexFullText::flag_` 传入（`index_defines.cppm:13-38`；`index_full_text.cppm:42,73`） |
| 常量 | `MAX_DOC_PER_RECORD=128`、`MAX_POS_PER_RECORD=128`、`MAX_DOC_PER_BITMAP_BLOCK=256`、`SKIP_LIST_BUFFER_SIZE=32`（`index_defines.cppm:43-53`） |

### 9.3.2 `.dic`（term 词典）

```
[ TermMeta 记录区（按 term 字典序、可变长）... ][ FST 区（term -> u64 TermMeta 偏移）... ]
```
- 写：先写 TermMeta 记录，再用 `FstBuilder` 构造临时 `.fst`，最后 `VirtualStore::Merge` **把 FST 追加到 .dic 尾部**（`memory_indexer_impl.cpp:516-543`）。
- 读：取**文件最后 12 字节** —— `len-12` 处 `u64 root_addr`；`fst_len = (root_addr==0) ? 36 : root_addr+21`；FST 在 `[len-fst_len, len)`；之前全是 TermMeta 记录（`dict_reader_impl.cpp:46-67`）。
- Lookup：FST `Get(term)` → `u64 offset` → 从 `dict+offset` 解析 TermMeta（`:80-90`）。

**TermMeta 编码**（`term_meta_impl.cpp:92-104`，字段见 `term_meta.cppm:44-49`）：

| # | 编码 | 字段 | 条件 |
|---|---|---|---|
| 1 | `VInt(u32)` LEB128 | `doc_freq_` | 总是 |
| 2 | `VInt(u32)` | `total_tf_` | 仅 `of_term_frequency` |
| 3 | `u32` LE **raw** | `payload_` | 仅 `of_term_payload` |
| 4 | `VLong(u64)` | `doc_start_`（.pos 偏移） | 总是 |
| 5 | `VLong(u64)` | `pos_start_` | 总是 |
| 6 | `VLong(u64)` | `pos_end_` | 总是 |

### 9.3.3 FST 区

| 偏移 | 类型 | 值 |
|---|---|---|
| 0 | `u64` LE | `VERSION = 3`（`node.cppm:40`） |
| 8 | `u64` LE | `FstType`（默认 `0`） |
| … | 节点数据（逆序编译） | |
| len-20 | `u64` LE | `len_`（key 数） |
| len-12 | `u64` LE | `root_addr` |
| len-4 | `u32` LE | checksum = 掩码化 **CRC-32/IEEE**（`mask = ((crc>>15)|(crc<<17)) + 0xa282ead8`） |

（`fst.cppm:64-112,134-144`；`writer.cppm:80-83`；`crc.cppm:48-50`）

- 空 FST：`root_addr=0`，总长 36。
- **Rust 侧 `.dic` 组装/读取已实现**（`infinity-storage::inverted_format`）：`encode_dic`（`[TermMeta…][FST]` 拼接）、`encode_term_meta_region_with_offsets`（同时返回每条记录的绝对偏移，即 FST 的 value）、`dic_fst_region`/`dic_fst`（由末尾 12 字节的 `root_addr` 反推 FST 区间：`root_addr==0` → 36，否则 `root_addr+21`）、`fst_header`（读 `VERSION`/`FstType`）、`decode_dic_meta_at`（按绝对偏移解析 TermMeta）。FST 的**节点状态机本身仍是不可读的**（见下），所以这些函数把 FST 当作**不透明字节**；一旦节点编解码落地即可直接接上。
- 节点编码见 `fst/node.cppm:255-814`（每 state 首字节高 2 位区分 `StateOneTransNext/StateOneTrans/StateAnyTrans`，`COMMON_INPUTS` 为固定 256 字节表 `fst/common_inputs.cppm`，转移地址用相对 delta 打包 `Pack/UnpackDelta` `:303-337`）。FST 的 `u64` value = 该 term 的 TermMeta 在 `.dic` 的**绝对字节偏移**。
- **Rust 侧整数打包原语已实现**（`infinity-storage::inverted_format`，语义无歧义、可单测）：`fst_pack_size`/`fst_pack_uint`/`fst_pack_uint_in`/`fst_unpack_uint`（`fst/bytes.cppm:107-170` 的 `Pack*`/`UnpackUint`，均为**小端**、低位在前）与 `fst_pack_delta`/`fst_pack_delta_size`/`fst_unpack_delta`（`fst/node.cppm:303-337`，存 `node_addr - trans_addr`，`trans_addr==0` 时存哨兵 `0`；`EMPTY_ADDRESS=0`）。
- **Rust 侧 `COMMON_INPUTS` 表已移植**（`FST_COMMON_INPUTS`/`FST_COMMON_INPUTS_INV`，逐元素取自 `fst/common_inputs.cppm:20-291`）与 `fst_common_idx`/`fst_common_input`（`fst/node.cppm:282-301`）。已验证两表都是 `0..=255` 的**排列**且 `INV[A[b]] == b` 恒成立；`CommonIdx` 的 `(rank+1)%256` 对 rank=255 的字节会回绕成 `0`（即该字节**永远按原值存**）——这点已在测试中固定。
- **节点状态机：编解码均已实现并用 C++ 真值逐位验证**（`infinity-storage::inverted_format`）：
  - **解码**（`fst_get`/`FstNode`）：三种 state（`0b11` OneTransNext / `0b10` OneTrans / 其它 AnyTrans）的 `end_`/`PackSizes`/`FindInput`/`OutputAt`/`TransAddr`/`FinalOutput` 全部按 `fst/node.cppm` 实现（含 `TRANS_INDEX_THRESHOLD=32` 的 256 字节索引表、`Ntrans` 的「1 表示 256」约定、输出按路径 `Cat`=求和）。
  - **编码**（`FstBuilder`/`build_fst`，复刻 `fst/build.cppm`+`build_impl.cpp`+`fst/registry.cppm`）：FNV-1a `BuilderNode::Hash`、直接映射 `Registry`（槽碰撞会**破坏性覆盖**，与 C++ 一致）、`UnfinishedNodes` 栈、`AddSuffix`/`FindCommonPrefixAndSetOutput`/`CompileFrom`/`Compile`、三种 state 的 `Compile` 与 `PackSizes` 字节。`CheckLastKey` 忠实复刻（注意 C++ 在 `last_` 为空时**直接 return 且不记录**，故这套版本里顺序校验**从不生效**）。
  - **真值来源**：`rust_engine/tools/fst_golden/gen.py` 把仓库里**真实的** C++ FST 源码摊平（用极小的 `infinity_type`/`crc`/`fmt` stub 替代 `third_party`）后用 clang++-23 编译运行，打印小键集（空;`a`;`a,b`;`app/apple/apply/banana/band/cat`;共享前缀 `aaaa/aaab/aabb/abbb/baaa`）的 FST 字节。这些字节作为固定装置写进 Rust 测试：header/trailer/掩码 CRC/`root_addr+21` 长度规则、`fst_get` 的全部 16 个键值查找、以及 `FstBuilder` 的**逐字节**输出，三者均通过。
- 校验：C++ = CRC32/IEEE + Snappy 掩码；Rust `fst` crate = CRC32C → **不兼容**（呼应 §3 / PoC F）。

### 9.3.4 `.pos`（postings）

`.pos` 是 term 的有序串联（顺序 = `.dic` 的字典序 = FST 迭代序）。每 term 片段由 TermMeta 的 `doc_start_/pos_start_/pos_end_` 界定（`disk_segment_reader_impl.cpp:120-131`）。

```
VInt  doc_skiplist_len
VInt  doc_list_len
u8[doc_skiplist_len]
u8[doc_list_len]
VInt  pos_skiplist_len
VInt  pos_list_len
u8[pos_skiplist_len]
u8[pos_list_len]
```
（`column_index_iterator_impl.cpp:86-116`；写出 `doc_list_encoder_impl.cpp:77-113`、`position_list_encoder_impl.cpp:66-91`）

### 9.3.5 postings 块编码（PForDelta / SIMD-BP128）

- **Doc list** 字段顺序（`doc_list_format_option.cppm:144-175`）：`doc_id`(u32, `Int32Encoder`) → `tf`(u32, 若 `of_term_frequency`) → `doc_payload`(u16, 若 `of_doc_payload`)。每 **128** 条一块（`posting_byte_slice.cppm:48`）。
- **Position list**（`position_list_format_option.cppm:71-84`）：`pos`(u32)，每 128 个一块。
- **Doc skip list**（`doc_list_format_option.cppm:67-127`）：`doc_id`(u32)、`tf`(u32)、`block_max_tf`(u32)、`block_max_percentage`(u16)、`offset`(u32)，每 **32** 项一块（`skiplist_writer_impl.cpp:37`）。
- **Pos skip list**（`position_list_format_option.cppm:27-54`）：`total_pos`(u32)、`compressed_pos_size`(u32)。
- 块字节格式：①**u32 字段 = SIMD-BP128**（`CompositeCodec<SIMDBinaryPacking, VariableByte>`，`fastpfor_impl.cpp:54-57`）：每块 = `u8 word_cnt` + `word_cnt*4` 字节；②**u16 字段 = NewPForDelta**（IndexLib 移植，`third_party/newpfor/`）：每块自描述，首 `u32` header = `isLastBlock:1 | exceptionIntRange:11 | frameBits:5 | firstExceptionPos:8 | dataNum:7`（`new_pfordelta_compressor.h:30-42`），长度由 `GetCompressedLength(header)` 推出。读取侧靠各编码器自描述长度逐块前进（`posting_decoder_impl.cpp:57-104`）。
- **framing 已核对（`src/storage/common/int_encoder.cppm`）**：u32 路径 `IntEncoder<u32, FastPForWrapper<Codec>>::Encode` 写 **`u8 encode_len`（单位是 u32 word 数）** + `encode_len*4` 字节（`:91-99`）；u16 路径 `IntEncoder<u16, NewPForDeltaCompressor>::Encode` **不写长度前缀**，只写 `Compress()` 返回的 `len*4` 字节，解码时先 `PeekInt32()` 取 header 再 `GetCompressedLength(header)`（`:46-68`）。注意 u32 是用 `CompositeCodec<SIMDBinaryPacking, VariableByte>`（`fastpfor_impl.cpp:55-57`），而 `fastpfor.cppm` 里的 `SIMDFastPFor<8>` 走的是另一条 `IntEncoder<u32, FastPForWrapper<...>>` 特化——**两条路都存在**，实际用哪条由字段的 `Encoder` typedef 决定（`posting_field.cppm:44,61` 取 `SIMDBitPacking`）。
- **C++ 真值（已生成，供 Rust 逐位实现比对）**——把 FastPForLib（C++ 构建拉取的 `third_party/fastpfor`，headers+src）与仓库自带的 `third_party/newpfor` 一起用 clang++ 编成小 harness 后实测：
  - u32 `CompositeCodec<SIMDBinaryPacking, VariableByte>`：
    - `{1,2,3,4,5}`（5 个）→ 3 words = `00000000 81828384 85000000`
    - `{0}×128` → 5 words = `80000000 00000000 00000000 00000000 00000000`
    - `{1,1,1,2,3,5,8,13,21,34,55,89}` → 4 words = `00000000 81818182 8385888d95a2b7d9`
    - `{100000,200000,300000,400000}` → 4 words = `00000000 200d8640 1a8c6027 92003598`
  - u16 `indexlib::NewPForDeltaCompressor`：
    - `{1,2,3,4,5}` → 1 word = `0300fa09`
    - `{1,1,1,2,3,5,8,13,21,34,55,89}` → 4 words = `01700016 81404030 28201a15 d12d0b00`
  - **复现配方**：`clang++-23 -std=c++23 -O2 -march=native -I<fastpfor>/headers -I<repo>/third_party/newpfor main.cpp <fastpfor>/src/{bitpacking,bitpackingaligned,bitpackingunaligned,simdbitpacking,simdunalignedbitpacking,horizontalbitpacking}.cpp <repo>/third_party/newpfor/{new_pfordelta_compressor,decompress_sse4}.cpp`，其中 `main.cpp` 用 `encodeArray`（u32）/ `Compress(...,true)`（u16）并打印字节。**注意**：FastPForLib 不在仓库内（由 C++ 构建拉取），故未随仓库提交生成器；上述字节已固定在本节。同理注意 `-O2` 与 `-march` 必须与目标一致（SIMD 路径按位宽分派，但字节输出与 -O 无关——已核）。
- **Rust 侧状态**：`TermPostings` 目前把 skip/list 体当作**不透明字节**（`inverted_format.rs`），u32/u16 块编解码**尚未实现**。
- **实现难度已定量（2026-10-17 实测结论，重要）**：SIMD-BP128 的字节布局**不是**「按位顺序的朴素打包」，因此**无法从一个简单公式推导**，必须**逐宽度移植 FastPForLib 的 SSE 位打包表**。已用探针（`a[i]=i`，逐宽度）反解验证：u32 布局按 **32 位 SSE lane 交错**（例如宽度 4 时输出顺序为 `0,4,8,…,28,1,5,…,29,…`，即 `i mod 4` 分 lane），且**不同宽度的交错模式不同**（宽度 2/4 恰好等于朴素顺序，宽度 7 明显不是）。原路线的规模：`__SIMD_fastpackN_32`/`__SIMD_fastunpackN_32` 共 `N=1..32`（各约 60–130 行 SSE），即 **≈2000+ 行**机械移植；u16 侧对应 `third_party/newpfor` 的 `pack.h`/`unpack.h`/`unaligned_unpack.h`。**建议**：u32 直接用「朴素布局 + 自己的 128 值块」不行（会写出与 C++ 不兼容的字节），所以要么逐宽度移植 SSE 表，要么接受「只读 C++ 索引、不写 C++ 索引」的降级；真值字节与复现配方见上，可逐宽度断言。

### 9.3.6 `.len`（列长 / BM25 统计）

- **裸 `u32[row_cnt]`，无头无尾**：写 `Append(column_length_array.data(), sizeof(u32)*size)`（`memory_indexer_impl.cpp:550-552,777-778`）；读直接 cast（`column_length_io_impl.cpp:62-66`）。承载者 `RawFileWorker`（无头，`raw_file_worker_impl.cpp:65-73`）。

### 9.3.7 BM25 统计量落点

| 统计量 | 位置 |
|---|---|
| 每词 `df` | `.dic` TermMeta 字段 1（也隐含于 doc list 条数） |
| 每词 `total_tf` | `.dic` TermMeta 字段 2（`of_term_frequency` 时） |
| 每文档 `column_len` | `.len`（`u32[row_cnt]`） |
| chunk `doc_cnt`/`term_cnt` | **RocksDB KV** 的 `chunk_info` JSON（§8.2），非索引文件内 |
| `total_df` | = 段 doc_cnt（`column_length_io_impl.cpp:69-78`） |
| block-max（WAND 上界） | doc skip list 的 `block_max_tf`(u32) + `block_max_percentage`(u16) |

## 9.4 Byte-level 实现清单（Rust 照此读写）

**HNSW `chunk_{id}.idx`**：①`u64 M`；②`u64 ef_construction`；③`u64 cur_vec_num`；④VecStoreMeta（plain=`u64 dim`）；⑤GraphStoreMeta（`u64 Mmax0`,`u64 Mmax`,`i32 max_layer`,`i32 enterpoint`）；⑥向量区 `cur_vec_num × dim × elem_size`；⑦图区：`u64 layer_sum` + `cur_vec_num × level0_size` + `layer_sum × levelx_size`；⑧`u32[cur_vec_num]` labels。节点内 `layers_p_` 是 **u64 字节偏移**（相对 layerx 区起点），邻居为 i32 数组。全部本机 LE。

**IVF `chunk_{id}.idx`**：①`u32 row_count`；②`u32 embedding_count`；③`u32 dim`；④`u32 num`；⑤`f32[num*dim]`；⑥parts：先 base（SQ=`f32[2*dim]`、PQ=`u32 real_num`+`f32[dim*real_num]`+`f32[real_num*subsp]`、Plain 空），再逐 part 写 `u32 embedding_num` + `u32[embedding_num]` + 向量/SQ/码本。metric/dtype/option 不在文件内。

**全文 `.dic`/`.pos`/`.len`**：①`.dic` = `[TermMeta…][FST]`，FST 位置由 `len-12` 的 `root_addr` 推出；②TermMeta = `VInt df` [`VInt ttf`][`u32 payload`]`VLong doc_start``VLong pos_start``VLong pos_end`；③`.pos` 每 term = `VInt doc_skip_len, VInt doc_len, doc_skip, doc_list, VInt pos_skip_len, VInt pos_len, pos_skip, pos_list`；④u32 字段块 = `u8 word_cnt` + `word_cnt*4B`（SIMD-BP128），u16 字段块 = NewPForDelta 自描述块；⑤`.len` = 裸 `u32[row_cnt]`；⑥`doc_cnt/term_cnt` 从 KV 的 `chunk_info` JSON 取；⑦FST 校验 = 掩码 CRC-32/IEEE（**非 CRC32C**）。

**实现要点（易错项）**：①两个向量索引文件**均无 magic/version/checksum**，靠外部（IndexBase JSON + 列定义）解释，写出的字节必须精确等于 `SaveToPtr` 输出，否则 `LoadFromPtr` 的尾部断言失败；②全文 `optionflag` 不在文件内，读 `.dic/.pos` 前必须先取得它；③全文 postings 的 u32 块是 **FastPFor SIMD-BP128 + VariableByte**（带 1 字节长度前缀），u16 块是 **NewPForDelta**（自描述 header）——这是逐位兼容的最大工作量点，已定位到 `third_party/newpfor/`。

## 9.5 未核实 / 存疑

| 项 | 状态 | 需读/验证 |
|---|---|---|
| `sizeof(VertexL0)/sizeof(VertexLX)` 的精确 padding | **已实测确认**：g++ 量测 `sizeof(VertexL0)=24`、`sizeof(VertexLX)=4`（见 §9.1.3），Rust 常量 `knn_format::{VERTEX_L0_SIZE, VERTEX_LX_SIZE}` + `encode/decode_graph_region` 已按其 stride 实现并有往返测试 | — |
| HNSW chunk 行数恒 ≤ `block_size_`（单 inner 不变式） | **VERIFY** | `new_txn_index_impl.cpp` |
| HNSW/IVF 是否存在“追加写”优化路径 | **UNKNOWN** | `new_txn_index_impl.cpp`、`segment_index_meta*` |
| FST 节点状态机/`COMMON_INPUTS` 逐位表 | **未逐位冻结**（结构已给） | `fst/node.cppm:255-814`、`fst/common_inputs.cppm` |
| LVQ/Rabitq/Sparse 的 Meta 完整字段与 `compress_data_size` | **部分**（字段顺序已给） | `lvq_vec_store.cppm`、`rabitq_vec_store.cppm`、`sparse_vec_store.cppm` |
| 全文跨段合并后的 base_name/row_count 后缀语义 | **UNKNOWN** | `new_txn_index_impl.cpp:2239`、`column_index_merger_impl.cpp` |
| FastPForLib 版本（影响 SIMD-BP128 逐位一致） | **VERIFY** | `cmake/`、`vcpkg.json` 中的 pin |

## 8.6 已验证：Catalog 的**值编码**（§2.6.1 第 ④ 项的剩余一半）

§8.2 只说「值为 ColumnDef / IndexBase JSON」；本节把**已核对**的字段与嵌套格式冻结如下（`file:line` 为证），供实现 catalog 时逐字节复刻。

**写点**：`TableMeta::InitSet` 对每个列写 `KeyEncode::TableColumnKey(db_id, table_id, column->name(), commit_ts)` → **`column->ToJson().dump()`**（`src/storage/catalog/meta/table_meta_impl.cpp:297-303`）。

**`ColumnDef::ToJson()`**（`src/parser/definition/column_def.cpp:228-248`）：

| 键 | 值 | 条件 |
|---|---|---|
| `column_type` | `DataType::Serialize()`（见下，嵌套对象） | 总是 |
| `column_id` | `column->id()`（整数） | 总是 |
| `column_name` | 列名（字符串） | 总是 |
| `constraints` | **数组**，逐项 append 约束枚举的整数值 | 仅当有约束 |
| `column_comment` | 列注释（字符串） | 仅当注释非空 |
| `default` | `ConstantExpr::Serialize()` | 仅当有默认值 |

> nlohmann 的 `json` 是 `std::map`，**键按字典序**输出；`serde_json::Value` 默认同为 `BTreeMap`，故两者的键序天然一致，可直接逐字节比较。

**`DataType::Serialize()`**（`src/parser/type/data_type.cpp:427-435`）：

| 键 | 值 | 条件 |
|---|---|---|
| `data_type` | `LogicalType` 的整数值（`type_`） | 总是 |
| `type_info` | `type_info_->Serialize()`（见下） | 仅当 `type_info_ != nullptr` |

**`type_info` 的分支**（以 `DataType::Deserialize` 的读法为准，`src/parser/type/data_type.cpp:437-476`，读取方与写入方字段名一致）：

| LogicalType | `type_info` 形状 |
|---|---|
| `kArray` | **就是元素类型的 DataType JSON 对象本身**（`DataType::Deserialize(type_info_json.raw_json())` 直接递归解析该 json） |
| `kDecimal` | `{"precision": <int>, "scale": <int>}` |
| `kEmbedding` / `kTensor` / `kTensorArray` / `kMultiVector` | `{"embedding_type": <EmbeddingDataType 整数>, "dimension": <int>}` |
| `kSparse` | 由 `SparseInfo::Deserialize(type_info_json.raw_json())` 解析（字段名未逐条核对 → **VERIFY**：`src/parser/type/type_info.cpp` 中 `SparseInfo::Serialize/Deserialize`） |
| 其余 | 无 `type_info` |

**`IndexBase::Serialize()`**（`src/storage/definition/index_base_impl.cpp:243-251`，写点 `SetIndexBase` `src/storage/catalog/meta/table_index_meta_impl.cpp:90-92`）：**五个公共字段** `index_type`（**字符串名**，取自 `IndexInfo::IndexTypeToString`：`IVF`/`HNSW`/`FULLTEXT`/`SECONDARY`/`SECONDARYFUNCTIONAL`/`BMP`/`DISKANN`/`EMVB`/`PLAID`/`SMVE`，`src/parser/statement/extra/create_index_info.cpp:25-62`）、`index_name`、`index_comment`（**无条件写出**；读侧缺失或非字符串时当空串）、`file_name`、`column_names`（字符串数组）；各索引类型再追加自己的字段（HNSW 的 `M`/`ef_construction`/`block_size`/`metric_type`/`encode_type`/可选 `build_type`,`lsg_config`；IVF 的 `ivf_option`；函数索引的 `func_return_type`/`function_expression` 等，见 `index_base_impl.cpp:267-360`）。

**已冻结的子类字段与 `file_name` 规则**（Rust 侧 `infinity-catalog::value_format::{IndexDefValue, index_file_name}`）：

- **`file_name` 生成规则**：`fmt::format("{}_{}", create_index_info->table_name_, *index_name)`（`src/planner/logical_planner_impl.cpp:859`），即 `{table}_{index}`（表名用**不带库名**的 `table_name_`）。此前的 VERIFY 已解除。
- **HNSW**（`index_hnsw_impl.cpp:278-290`；默认值 `src/common/default_values.cppm:178-180`）：`build_type`（`HnswBuildTypeToString`：`plain`/`lsg`）、`ef_construction`（默认 200）、`encode_type`（`HnswEncodeTypeToString`：`plain`/`lvq`/`rabitq`）、`M`（默认 16）、`block_size`（默认 8192）、`metric_type`（`MetricTypeToString`：`cosine`/`ip`/`l2`/`Invalid`）。注意大写 `M` 在 nlohmann 的 `std::map` 里排在小写键**之前**。
- **FULLTEXT**（`index_full_text_impl.cpp:114-119`；`Make` `:90-116`）：`analyzer`（缺省 `standard`）、`flag`（`OPTION_FLAG_ALL = 63`，`index_defines.cppm:13-32`）。
- **SECONDARY**（`index_secondary_impl.cpp:117-124`；`Make`）：`cardinality`（`"low"`/`"high"`）、`is_json_index`（bool）。
- **IVF**（`index_ivf_impl.cpp:339-393`；默认值 `index_ivf.cppm:30-61`）：`ivf_option` 为**嵌套对象**：`metric_`（`MetricType` 序号 i8→整数）、`centroid_option_`{`centroids_num_ratio_`=1.0,`min_points_per_centroid_`=32,`max_points_per_centroid_`=256}、`storage_option_`{`type_`(0 plain/1 sq/2 pq),`plain_storage_data_type_`,`scalar_quantization_bits_`,`product_quantization_subspace_num_`,`product_quantization_subspace_bits_`}。
- **BMP**（`index_bmp_impl.cpp:118-123`）：`block_size`（size_t）、`compress_type`（`static_cast<i8>(BMPCompressType)`，`bmp_util.cppm:28-32`：`kRaw`0/`kCompressed`1/`kInvalid`2；类默认 `kCompressed`）。
- **SMVE**（`index_smve_impl.cpp:99-104`；默认 `index_smve.cppm:53-54`）：`width`（默认 2048）、`topk`（默认 8）。
- **PLAID**（`index_plaid_impl.cpp:133-139`；默认 `index_plaid.cppm:59-61`）：`nbits`（默认 4）、`n_centroids`（默认 0）、`colbertsar_mode`（bool，默认 false）。
- **EMVB**（`index_emvb_impl.cpp:128-133`；默认 `index_emvb.cppm:58-59`）：`pq_subspace_num`、`pq_subspace_bits`（默认均 0）。
- **DISKANN**（`index_diskann_impl.cpp:142-151`）：`metric_type`（字符串名）、`encode_type`（`DiskAnnEncodeTypeToString`：`plain`/`invalid`）、`R`、`L`、`num_pq_chunks`、`num_parts`。
- **SECONDARYFUNCTIONAL**（`index_secondary_functional_impl.cpp:69-78`）：`cardinality`（`"low"`/`"high"`）、`func_col_params`（字符串）、`func_return_type`（**`DataType::Serialize()` 对象**）、`function_expression`（**内嵌 JSON 对象**，因 C++ 把存下的字符串再 `parse`）。
- **索引 id 与段记录**：表级 tag `next_index_id` 从 `0` 起（`table_meta_impl.cpp:281-282`），index id 为 `"0"`,`"1"`,…（`catalog_cache_impl.cpp:250-253`）；名→id 键 `catalog|idx|{db}|{table}|{index_name}|{commit_ts}` → `index_id` 字符串，值键 `idx|{db}|{table}|{index_id}|index_base`（`new_catalog_static_impl.cpp:628-635`、`table_index_meta_impl.cpp:90-92`）。FULLTEXT 另写 `idx|…|segment_update_ts` tag（`table_index_meta_impl.cpp:198-206`）。

**已冻结的 `ConstantExpr` 与 `SparseInfo`**（Rust 侧 `value_format::{constant_expr_*, sparse_type_info}`）：

- **`ConstantExpr::Serialize()`**（`src/parser/expr/constant_expr.cpp:452-530`）：`{"type": <int32 LiteralType>, "value": …}`（键序 `type` < `value`）。`LiteralType` 序号（`constant_expr.h`）：`kBoolean`0/`kDouble`1/`kString`2/`kInteger`3/`kNull`4/`kDate`5/`kTime`6/`kDateTime`7/`kTimestamp`8/`kIntegerArray`9/`kDoubleArray`10/`kSubArrayArray`11/`kInterval`12/`kLongSparseArray`13/`kDoubleSparseArray`14/`kEmptyArray`15/`kCurlyBracketsArray`16/`kJson`17。**`kNull` 与 `kEmptyArray` 不写 `value` 键**；`kDate`/`kTime`/`kDateTime`/`kTimestamp` 的 `value` 是**字符串**（`char* date_value_`）；`kString`/`kJson` 也是字符串（`char*`，不二次解析）；`kSubArrayArray`/`kCurlyBracketsArray` 的 `value` 是**逐元素递归 `Serialize()`** 的数组；`kLongSparseArray`/`kDoubleSparseArray` 的 `value` 是对象 `{indices:[i64…], data:[…]}`（dump 后键序 `data`<`indices`）。
- **`SparseInfo::Serialize()`**（`src/parser/type/info/sparse_info.cpp:103-110`）：SPARSE 列的 `type_info` = `{"data_type": <EmbeddingDataType>, "index_type": <EmbeddingDataType>, "dimension": <int>, "sort": <int8 SparseStoreType>}`。`index_type` 由 `dimension` 推得（`GetIndexType` `:44-58`）：≤128→`int8`(2)、≤32768→`int16`(3)、≤2^31→`int32`(4)、否则 `int64`(5)；`sort` 默认 `kSort`(0)（`store_type_{SparseStoreType::kSort}` `sparse_info.h:107`）。普通 SPARSE 列的 `type_info` **不嵌在 `{"data_type":…}` 里**，而是直接是上面的对象（`data_type.cpp:471` 用 `SparseInfo::Deserialize(type_info_json.raw_json())`）。

**已查证：自定义 MergeOperator 无语义影响**（此前列为待冻结项）。C++ 在 `kv_store_impl.cpp:308,333` 为 RocksDB 实例注册 `String2UInt64AddOperator`（`src/storage/common/rocksdb_merge_operator_impl.cpp:28-49`）：`Merge` 把已存在的值与操作数**各自按十进制字符串** `stoull` 后相加，再 `std::to_string` 写回（已存在值缺失时按 0）。**但全仓库没有任何 `->Merge(` 调用点**（仅 `Put/Get/Delete/GetIterator`），因此该算子从不产生 merge 记录，磁盘上永远是普通键值。结论：**读写互通不需要它**——Rust 侧不注册也不影响；只有当 C++ 将来开始 `Merge` 时才会出现。

**§8.6 全部冻结完毕**：catalog 的键（§8.2）、列值（`ColumnDef`/`DataType`）、索引值（`IndexBase` 全部十种类型）、默认值（`ConstantExpr`）、SPARSE `type_info`（`SparseInfo`）都已逐字段复刻并有 pin 测试；MergeOperator 经查证无语义影响。
