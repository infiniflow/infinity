# 未登记简化审计（2026-10-20）

依据 `RUST_REWRITE_PLAN.md` §3.6 / D1：「以 C++ 类/public 方法为骨架逐类移植；**简化必须登记**在 `CLASS_MAP.md` 的『有意的设计变更』或『尚未一一对应』」。本文件记录一次按子系统并行的只读审计结果：把 `CLASS_MAP.md` 里标 ✅ 的行逐条对照 Rust 实现与 C++ 参考，找出**简化了却没有登记**的条目。

判定：`aligned` / `simplified-registered`（已登记）/ **`simplified-unregistered`（本文件要收敛的）**。共 **30** 条。

优先级：**P0 = 结果/磁盘格式不正确或与 C++ 不可互操作**；**P1 = 行为缺失（功能名义上存在但没接线）**；**P2 = 性能/规模退化**。

## 存储 / WAL / 事务（8）

| 优先级 | 条目 | Rust | C++ | 损失 |
|---|---|---|---|---|
| P0 | WAL **无对应行** | `infinity-storage/src/wal.rs:41-58`，载荷在 `infinity-txn/src/lib.rs:22-31,188-198` | `wal/wal_entry.cppm:42`（WalCmd）、`wal_manager.cppm:88` | 实际日志是 `[magic,size,crc][txn_id,commit_ts][KV 写批]`，**没有类型化 redo**（Append/Import/Compact/Checkpoint…）；移植好的 `wal_entry.rs:398`（含 `WalEntryIterator:516`）零调用方。回放退化为「重放 KV put」 |
| P0 | `TxnManager` | `infinity-txn/src/lib.rs:87-300` | `new_txn_manager.cppm:41-247` | 无 `RollBackTxn`、无 txn_map/TxnInfo（`SHOW TRANSACTIONS`/`…HISTORY` 缺失）、无 `CommitBottom`/`CommitKVInstance`、无 `GetReadCommitTS/OldestAliveTS`；冲突检查退化为内存 `delete_ts`（重启即丢、只覆盖 delete） |
| P0 | `MetaStore`/`KVStore` **无对应行** | `infinity-storage/src/metastore.rs:48-63` | `catalog/kv_store.cppm:42-98` | `atomic_write` 只支持 put、无批量 delete、无 per-txn instance/rollback、无 Merge；DDL 逐键删除非原子。完整移植的 `txn_kv.rs:113-660` 是死代码 |
| P1 | `BufferObj`/`BufferHandle` | `infinity-storage/src/buffer.rs:188-423` | `buffer/buffer_obj_impl.cpp:113-440` | 缺 `ToMmap/Mmap/Munmap`、`SaveSnapshot`（快照改为把行序列化成 KV blob）；**这两个类型零生产调用方** |
| P1 | `Storage` | `executor/lib.rs:3797` | `storage.cppm:90-111` | 缺状态机（`Get/SetStorageMode`、`InitToAdmin`/`AdminToReader`/`ReaderToWriter`…）、`RecoverMemIndex`、`AttachCatalog(ts)` |
| P1 | `BufferManager` | `buffer.rs:950-1025` | `buffer_manager_impl.cpp:177-365` | 只是「解码块 LRU」，没有按路径去重的 `buffer_map_`、没有分片 `lru_caches_`/`RequestSpace`、没有 `buffered_object_count`/`unused_buffer_object`（`lib.rs:13455-13456` 硬编码 "0"） |
| P2 | `VarBuffer` | `var_buffer.rs:10-95` | `column_vector/var_buffer.cppm:31-133` | `VarBufferManager` 是类型别名，缺 outline BufferObj 与 `Append(..., free_success)` 的预算/落盘钩子 |
| P2 | `BlockVersion` | `mvcc.rs:163-196` | `catalog/meta/block_version.cppm:44-77` | 只有 `SpillToFile`；缺 checkpoint 剪枝版 `SaveToFile`、`GetCommitRowCount`、`RestoreFromSnapshot` |

## 全文（7）

| 优先级 | 条目 | Rust | C++ | 损失 |
|---|---|---|---|---|
| P0 | `PostingWriter/PostingMerger/PostingIterator`(行 87) | `fulltext_segment.rs:108-113,286-317,354-358` | `posting_writer_impl.cpp:87-90`、`column_index_reader.cppm:56` | `add_position()` **不接收位置**，`write_segment_index` 写空 pos 列表；封盘段无法做短语查找 |
| P0 | `ColumnIndexReader`/`IndexReader`(行 52) | `lib.rs:14035-14057` | `column_index_reader.cppm:50-56` | `RowFulltextProvider::postings` 对非 sparsegram 的 term **返回所有行**，`filter_fulltext` 实际不做候选裁剪 |
| P0 | `AnalyzerPool` 家族(行 84) | `analyzer.rs:754-816`、`lib.rs:10678-10695` | `column_inverter_impl.cpp:51-57` | 分析器本身是真的，但 `index_delims` 把除 keyword/whitespace 外全部映射成标准分词器；`ik`/`rag-chinese`/`japanese`/`ngram` 索引用标准 token 建 |
| P1 | `QueryBuilder`/`And/Or/phrase`(行 85) | `query_builder.rs:371-1156` | `query_builder_impl.cpp:40-71` | 产品路径从不构造这些节点：MATCH 只用单 term 非短语的 WAND（`lib.rs:10933-10939`），否则整行重分词扫描；短语/布尔迭代器是死代码 |
| P1 | `MemoryIndexer`(行 51) | `lib.rs:2020-2110` | `memory_indexer_impl.cpp:103,371,661` | 无 spill-to-sorted-runs、无 `InsertGap`、无异步插入；`FULLTEXT_SPILL_SUFFIX` 声明但未用，`SortMerger` 无调用方 |
| P2 | `BlockMaxWandIterator`(行 44) | `wand.rs:68-286` | `blockmax_wand_iterator_impl.cpp:88-486` | 没有块级最大值，只有 per-term 上界；结果相同、剪枝更弱；`run()` 一次性 drain 而非增量 `Next(doc_id)` |
| P2 | `StandardAnalyzer`(行 20) | `analyzer.rs:690-709` | `analyzer_pool_impl.cpp:266-289` | Porter2 代替 STEM_LANG_PORT（自述 "closest match"） |

> 另：设计变更行 113 声称「磁盘字节布局已对齐」，但 `.pos` 块体用的是不同的 IntEncoder 帧、读侧多一个 `.row` 文件——**登记内容不准确**，需修正。

## 索引（6）

| 优先级 | 条目 | Rust | C++ | 损失 |
|---|---|---|---|---|
| P0 | `EMVBProductQuantizer`(行 54)、`PQ::Save`(行 31) | `emvb_segment.rs`（`train_residual_pq`） | `emvb_index_impl.cpp:62`、`product_quantizer.cppm:66-68` | OPQ 旋转矩阵写成**单位阵**，数据与查询都不旋转；C++ 写出的 chunk 无法按其 R 打分 |
| P0 | `SecondaryIndexDataLowCardinality`(行 28) | `secondary_lowcard.rs:145-151,226-228` | `secondary_index_data_impl.cpp:321-326`、`roaring_bitmap.cppm:299` | 少写 `u32 count_` 前缀，chunk 与 `WriteAdv` **不兼容**（行里却写「WriteAdv 兼容 ✅」） |
| P0 | `PGMIndex<T,64,4>`(行 29) | `secondary_segment.rs:77-160,212-225,292-298` | `secondary_index_pgm.cppm:100-153` | 单层贪心模型、`levels_offsets` 恒 0、miss 时退回二分；C++ 写出的 chunk 打不开（单向不兼容） |
| P1 | `PlaidIndex::MergeChunks`(行 79)、`DiskMerger`(行 81) | `plaid/index.rs:1192-1384`、`plaid/merger.rs:160-316` | `plaid_index_disk_merger.cppm` | 模块外**零调用**；引擎改为从 tail 重建整块，增量/并行合并与 manifest 生命周期从不发生 |
| P1 | `InitAbstractIndexT`(行 50) | `cpp_blocks.rs:1610,2985` | `index_hnsw.cppm:114-115` | 落盘图硬编码 `M=16/ef_construction=200`，`WITH (M=…, ef_construction=…)` 被接受但对持久化图无效 |
| P2 | `BmpAlg::SearchKnn` 选项(行 63) | `lib.rs:3156` | `physical_match_sparse_scan_impl.cpp:523` | 永远传 `BmpSearchOptions::default()`，SQL 无法设置 `alpha/beta/use_tail` |

## 规划 / 执行 / 网络（9）

| 优先级 | 条目 | Rust | C++ | 损失 |
|---|---|---|---|---|
| P0 | `PhysicalOperator`/`OperatorState` 聚合行(行 78) | `executor/lib.rs:230,247,15293` | `physical_operator.cppm:31`、`operator_state.cppm:40` | **不存在算子模型**：计划被拆成 stage，跑在整表物化的 `Vec<Vec<Value>>` 上；聚合按组克隆整行，无块/流式、无 early LIMIT。D4 只豁免 `TaskScheduler` |
| P0 | `FilterExpressionPushDown`/`IndexScanBuilder`(行 99) | `lib.rs:14077,14101-14110` | `physical_index_scan_impl.cpp` | 非 `Eq` 谓词不从索引回答（取全表再内存过滤）；掩码为 0 行时**退回全部行** |
| P1 | `ApplyFastRoughFilter`(行 98) | `index_scan.rs:2405-2442`、`optimize.rs:38-56` | `apply_fast_rough_filter_impl.cpp:46-60` | `FastRoughFilterEvaluator` **无生产实现/调用方**，默认规则空 `IndexCatalog`，只能折叠字面 false |
| P1 | `ResultCacheGetter`(行 98) | `optimize.rs:132-162` | `result_cache_getter_impl.cpp` | 规则仅测试构造；生产是「整条 `QueryResult` 按语句文本做键」的缓存，无 begin-ts/表键/列投影 |
| P1 | `physical_show` / `ShowSegments`,`ShowBlocks` | `infinity-server/src/lib.rs:1587-1626` | `physical_show_impl.cpp:112,376` | 只给 `segment_id` 和「行数 ÷ 8192」合成的 `block_id`，无真实段/块元数据 |
| P1 | `BindContext`（**无对应行**） | `planner/subquery.rs:787-802` | `bind_context.cppm:159-219` | 只剩相关列切片，缺 `AddTableBinding`/`ResolveColumnId`/`AddLeft/RightChild`/CTE/view/USING 绑定 |
| P1 | `DumpMemIndexTask`/`OptimizeIndexTask`/`CleanupIndexTask`(行 76) | `bg_task.rs:35`、`lib.rs:13364` | `storage/bg_task/*` | `BGTaskProcessor` **生产从未 attach**（仅测试）；`cleanup_interval`/`compact_interval` 解析但不调度，陈旧 chunk 不会周期回收 |
| P2 | HTTP 路由聚合(行 144) | `infinity-server/src/http.rs:961-1000` | `network/http_server_impl.cpp` | 缺 `/admin/*`、`/instance/*`、`/snapshots/*`、`/variables/*` 等大量路由（C++ 27 条） |
| P2 | `infinity-expression`(行 145/94) | `infinity-expression/src/lib.rs:107,856` | `expression_binder_impl.cpp:453` | 未知标量函数**静默返回 NULL**（C++ 绑定期报错）；`scalar::TypedColumn` 模块外零调用 |

## 未列在 CLASS_MAP 的死模块

- `infinity_index::multivector`（`multivector.rs`，暴力 MaxSim）零调用方。
- `infinity-storage::txn_kv`（`KVStore/KVInstance/KVIterator` 完整移植）零调用方。
- `infinity-storage::wal_entry` 零调用方。
- `infinity-planner::result_cache::ResultCacheGetter`、`index_scan::FastRoughFilterEvaluator` 零生产调用方。

## 进度

| 条目 | 状态 | 提交/说明 |
|---|---|---|
| 索引 · `InitAbstractIndexT` 硬编码 M/ef_construction | ✅ 已修 | `04ee86483`：从索引选项读取并写进 chunk 头；`HnswIndex::ef_construction()` 提供真值 |
| 索引过滤 · 掩码全 false 时返回全部行 | ✅ 已修 | 改为返回空集（`PhysicalIndexScan` 语义）；等 storage 侧 agent 落盘后统一跑 Gate A |
| 低基数 chunk / PGM 层级 / 全文 positions / EMVB OPQ | 🔄 进行中 | 三个并行 agent 分别改 `secondary_lowcard.rs`+`secondary_segment.rs`、`fulltext_segment.rs`+`inverted_format/**`、`emvb*.rs` |
| WAL 类型化 redo / TxnManager | 🔄 进行中 | agent 负责 `wal.rs`/`wal_entry.rs`/`infinity-txn/**` |
| 其余（MetaStore 原子删除、算子模型、`RowFulltextProvider`、`index_delims` 分派、HTTP/admin、bg_task 调度…） | ⏳ 待办 | 见上方清单 |

## 处置口径

1. **P0 全部要改**（磁盘格式/结果正确性/互操作），改完补针对性回归（C++ 写、Rust 读；Rust 写、C++ 读）。
2. **P1 逐项判定**：要么接线（`FilterExpressionPushDown`、`pg`/HTTP 路由、bg_task 调度、`BindContext`），要么写进「有意的设计变更」并说明理由与影响面。
3. **P2 可登记后保留**，但必须在表里写清「差在哪、影响什么、何时需要补」。
4. 审计与处置进度在本文件更新；`CLASS_MAP.md` 的 ✅ 以本文件为准收紧。
