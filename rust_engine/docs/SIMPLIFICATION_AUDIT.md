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
| P1 | `Storage` | `executor/lib.rs:3254`（`struct Storage { catalog, store, flush_lock }`） | `storage.cppm:90-111`、`storage_impl.cpp:805-877` | **连 mode 字段都不存在**（不只是「缺状态机」）：无 `StorageMode{kUnInitialized,kAdmin,kReadable,kWritable}`、无迁移表、无 `AttachCatalog(ts)`（catalog 是实时视图，无 checkpoint-ts 语义）、无 `RecoverMemIndex`（近似物 `rebuild_indexes()`）。`NodeRole` 只活在 `infinity-server/src/peer.rs`，`change_role` 只改注册表标签、不碰引擎 |
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
| P1 | Plaid 多 chunk 与合并(行 79/81) | `plaid/index.rs:1192-1244`（`merge_chunks`）、`merger.rs:160-433`（`PlaidIndexDiskMerger`）、`cpp_blocks.rs:2863 write_cpp_plaid_index` | `plaid_index_impl.cpp:1992-2090`（`MergeChunks`）、`new_txn_index_impl.cpp:2348-2424`（`OptimizePlaidIndex`） | 参考侧 `MergeChunks` **只有单测调用**、`PlaidIndexDiskMerger` **零调用**，产品路径是 `OptimizePlaidIndex`→`InitializeMerge`/`MergeOneChunk`/`FinalizeMerge`/`CopyCentroidsFrom`。Rust 缺的不是接线而是**多 chunk 的产生**：`write_cpp_plaid_index` 把 chunk id 硬编码成 0、每次 flush 整表重写；`optimize/cleanup_cpp_index_chunks` 只认 HNSW。`merge_chunks` 还有三处与真实调用方不兼容：忽略 `doc_offset`、不复制 centroids（目标未训练时 `exact_score_batch` 会 `expect` panic）、不合并 `plaid_seqs` |
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
| P1 | `BGTaskProcessor`(行 76) | `bg_task.rs:35`（`start/submit/drain/stop`）、`lib.rs:13480 attach_bg_task_processor` | `bg_task/bg_task_processor.cppm`、`background_process_impl.cpp:67-148`、`periodic_trigger_thread_impl.cpp:49-74` | `BGTaskProcessor` **生产从未 attach**（唯一调用方是 `bg_task_ut.rs:178`）。Rust **没有** `OptimizeIndexTask`/`CleanupIndexTask`（只有真实的 `DumpMemIndexTask`）——缺的是**调度器**不是任务；`cleanup_interval`/`compact_interval` 解析但不调度（`optimize_interval` 由 CLI 裸线程消费）。C++ 是 4 个 processor（`BGTaskProcessor`/`DumpIndexProcessor`/`CompactionProcessor`/`OptimizationProcessor`）+ 1s tick 触发器，Rust 合并成 1 个（需登记） |
| P2 | HTTP 路由聚合(行 144) | `infinity-server/src/http.rs:961-1000` | `network/http_server_impl.cpp` | 缺 `/admin/*`、`/instance/*`、`/snapshots/*`、`/variables/*` 等大量路由（C++ 27 条） |
| P2 | `infinity-expression`(行 145/94) | `infinity-expression/src/lib.rs:107,856` | `expression_binder_impl.cpp:453` | 未知标量函数**静默返回 NULL**（C++ 绑定期报错）；`scalar::TypedColumn` 模块外零调用 |

## 未列在 CLASS_MAP 的死模块

- `infinity_index::multivector`（`multivector.rs`，暴力 MaxSim）零调用方。
- `infinity-storage::txn_kv`（`KVStore/KVInstance/KVIterator` 完整移植）零调用方。
- `infinity-storage::wal_entry` 零调用方。
- `infinity-planner::result_cache::ResultCacheGetter`、`index_scan::FastRoughFilterEvaluator` 零生产调用方。

## 进度（2026-10-20）

状态口径：**✅ 已收敛**（有提交、有回归）；**🔄 进行中**（本会话内有 agent 正在改）；**⏳ 未开始**。

### 存储 / WAL / 事务

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 1 | WAL 无类型化 redo | ✅ | `74a4728b2`：`WalCmd` Append/Delete 类型化 + `KvBatch(Catalog)` 容器、`WalEntryIterator` 回放 |
| 2 | `TxnManager` 无 rollback/registry | ✅ | `74a4728b2`：`rollback`、txn registry、范围冲突检查（从 WAL 重建） |
| 3 | `MetaStore`/`KVStore` 无批量 delete / 非原子 DDL | 🔄 | `6316ae8f0` 已给 store 侧 `atomic_batch`+`merge`；checkpoint-only 包装 `DeferredCatalogStore` 已转发 `atomic_batch`（保持 undo 日志完整）。**DDL 消费方仍逐键写**，待 executor 改为一次批量提交 |
| 4 | `BufferObj`/`BufferHandle` 缺 mmap/快照状态 | ✅ | `fff4b4ff8`：`ToMmap/Mmap/Munmap`、`save_snapshot` |
| 5 | `Storage` 无状态机 | ✅（准入守卫级） | `21ddc380c`：`StorageMode{kUnInitialized,kAdmin,kReadable,kWritable}` + `Storage::get/set_storage_mode`（`storage_impl.cpp:805-877` 的合法迁移表，同模式为 no-op，非法迁移报错）+ `accepts_writes/accepts_cleanup`；`Engine::ensure_writable`（8007 `InvalidNodeRole`）挂在 `execute()` 的写语句与 `flush`/`cleanup`/`optimize_indexes`/`compact_table`/`insert_literals`/`insert_rows`/`EngineTxn::commit` 上，`UnInitialized` 时为 fatal。5 个测试（默认 Writable、Readable 拒写但可读、回 Writable 恢复、`UnInit→Writable` 非法、Admin 只读）。**未复刻**：`InitToAdmin`/`AdminToReaderBottom`/`AdminToWriter` 的子系统重建与 `AttachCatalog(ts)`（Rust 在 `open()` 一次性建好），已登记进 `CLASS_MAP.md` |
| 6 | `BufferManager` 缺对象管理/分片 | ✅ | `fff4b4ff8`：分片 `lru_caches_`、`RequestSpace`、`buffered_object_count`/`unused_buffer_object` |
| 7 | `VarBufferManager` 是类型别名 | ✅ | `24429ee91`：真正的 outline-backed manager + `free_success` 预算钩子 |
| 8 | `BlockVersion` 只有 `SpillToFile` | ✅ | `b20b5632f`：checkpoint 剪枝 `save_to_file(ckp_ts)`、`commit_row_count`、`restore_from_snapshot` |

### 全文

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 9 | 封盘段不存位置（`add_position` 无参数） | ✅ | `a16e1b361`（writer/reader 位置流）+ `ac909e68b`（executor 两条 seal 路径携带位置、spill 合并读 `positions()`）；一次性格式破坏已声明 |
| 10 | `RowFulltextProvider::postings` 返回所有行 | ✅ | `80a2e24c0`：改走列倒排索引的 `merge`（C++ `ColumnIndexReader::Lookup`）并按扫描得到的 row id 求交——陈旧 posting 不会放行未物化的行；同列两个分词器不同的索引经 `guard.tokenizer != tokenizer` 判定为「不能回答」，回退为保留该行由残余谓词裁决。测试 `fulltext_index_filter_reads_postings_by_row_id` |
| 11 | `index_delims` 抹平分析器 | ✅ | `d05c0f227`：`TextTokenizer` 增加 `analyzer: Option<String>`，`index_analyzer` 从表元数据取索引的 `ANALYZER` 选项（`standard`/`keyword`/`whitespace` 仍走分隔符路径），`tokenize_text`/`tokenize_index_text`/`tokenize_index_positions` 统一问 `AnalyzerPool::get_analyzer(name)`；写侧与查询侧同一 tokenizer，等值保护仍有效。测试 `an_ik_fulltext_index_is_tokenised_by_its_analyzer`（`热爱` 产生 posting、整段中文不被当作一个标准 token） |
| 12 | `QueryBuilder`/短语/布尔迭代器不在产品路径 | ✅（改完 + 剩余已登记） | `cf5bffed0`：`filter_fulltext` 的索引过滤现在真的构造 `Or`/`And` 查询树——裸 term 列表是 `OrQueryNode`，只有 `operator=and` 才是 `AndQueryNode`，布尔词是算子而不是 term（`SearchDriver::AnalyzeAndBuildQueryNode`）。这条 bug 在索引过滤放行所有行时是隐形的，接上真实 postings 后立刻让 Gate A 掉到 1539/1546。**剩余**（按 §3.6 登记）：`SEARCH … MATCH` 的多词/短语仍走「整行重分词」而非迭代器，结果一致、只是没有索引加速 |
| 13 | `MemoryIndexer` 无 spill-to-sorted-runs | ✅ | 比审计写的更严重：预算用尽后原实现 `reset()` + `disabled = true`，**该列此后完全不再建索引**。新版：`MemoryIndexer::spill` 把常驻 tail 写成有序 run（复用现有 `.dic/.pos/.len/.row` 文件族，`FULLTEXT_SPILL_SUFFIX` 命名，因此 positions 保留、没有第二套 postings 格式），`merged_segment()` 按 source rank（旧 spill → 新 run → tail）逐行取最新副本后归并，`TextIndex::seal` 走原有 `write_segment_index`，查询侧 `merge` 由新到旧叠在封盘段上。**关键证据**：40 篇文档在 budget=160（40 个 run）与 budget=∞ 两种策略下，封盘段四个文件**逐字节相同**。顺带修两个独立正确性问题：重索引行只丢封盘段的旧副本（此前整行从搜索消失）、`overridden` 在 seal 后未清。测量（200k 文档 / 8 MiB）：旧策略 13,435 篇（93%）后停止建索引，新策略 200k 全建，峰值记账仍 8 MiB 级，端到端 4.71s vs 不限预算基线 5.01s。**剩余差异（已登记）**：合并仍在内存中物化整段（O(postings) 内存），参考实现是字节级外部归并（`SortMergerTermTuple`，O(1)） |
| 14 | BMW 无块级最大值、非增量 | ✅ | `ee9a8d3f2`：`MAX_DOC_PER_RECORD=128` 的块级最大值（`TermDocIterator::BlockMaxBM25Score` 的 `f1/(1+max_tf)+f3/(1+max_pct_u16)+f4`，`pct` 按编码器 `ceil(x*65535)` 存）、`FindPivotOptimized` + 前缀和的 score upper bound、增量 `next(Option<u64>)`/`NextShallow`、块跳（seek 到 `up_to` 为止，保证后续 pivot 需要的 doc 仍可达）。测试：`top_k_in_the_first_block_never_scores_later_blocks`（2000 doc 只解码 128 次）、`wand_matches_full_scan_on_a_random_corpus`（5 term × topn × delta × minimum_should_match × BM25/boolean 与全扫逐位比对）、`block_maximum_bounds_every_score_in_the_block`（剪枝的安全性质）。Gate A 1546/1546 |
| 15 | `StandardAnalyzer` 用 Porter2 顶替 `STEM_LANG_PORT` | ✅（已登记） | 仅 `standard-porter` 受影响（`standard` 与其余 17 种语言都是同族 Snowball 算法）。按 §3.6 登记进 `CLASS_MAP.md`「有意的设计变更」：手写原版 Porter 或引新依赖的风险大于收益；若不接受需单独决策 |

### 索引

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 16 | EMVB OPQ 写成单位阵 | ✅ | `a19d08364`：OPQ 训练（6 轮 + 通用 `JacobiSVD` 移植）、查询侧 `R^T q`、R 存 chunk；`R=U·V^T` 对符号/排序不变 |
| 17 | 低基数缺 `count_` | ✅ | `d13d21834`：`WriteAdv` 逐字节，`count_` 为 chunk 级 `max_offset+1`，run/array/bitset 选择 + cookie 复刻；fixture 由 CRoaring 实测生成 |
| 18 | PGM 单层、`levels_offsets` 恒 0 | ✅ | 同上：真正的 `PGMIndex<u64,64,4,float>` 递归层级 + C++ 字节 fixture（1 级/2 级各一） |
| 19 | Plaid `MergeChunks`/`DiskMerger` 零调用 | ⏳ | 引擎仍从 tail 重建整块；接线方案研究中 |
| 20 | `InitAbstractIndexT` 硬编码 M/ef_construction | ✅ | `04ee86483`：从索引选项读取并写入 chunk 头 |
| 21 | BMP 搜索选项恒 default | ✅ | `948731581`：`parse_bmp_search_options` 实现 `BMPUtil::ParseBmpSearchOptions`（`alpha`/`beta`/`use_tail`，WITH 优先于 option 字符串，越界忽略，`use_tail` 只认 `T`/`F`），内存与磁盘两条 sparse 路径都用它（磁盘路径原先硬编码 0.9）；顺带修了稀疏 option 串被整条丢弃的解析 bug。测试 `match_sparse_alpha_option_reaches_the_bmp_search`（`alpha=0` 改变 `SCORE()`） |

### 规划 / 执行 / 网络

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 22 | 不存在 `PhysicalOperator`/`OperatorState` 模型 | ✅ | 8 片全部完成（`0487acb7d`…`50a0411df`，main 上 `6b09d644e`）：`chunk::DataChunk`（8192 行、`Value` 单元）、`operators::{PhysicalOperator, OperatorState, OperatorStates}` + `execute_task`/`run_fragment` push 驱动、`PhysicalFilter/Projection/Limit/Top/Aggregate/Sort`、`PhysicalSource`（原 `Source::Table` 的取数/`apply_index_filter`/`align_rows` 移入）、`Stage`/`Source`/`decompose`/`pipeline`/`apply_aggregate` 全部删除、`INFINITY_OPERATOR_MODEL` 开关与 tokio 依赖一并移除，`lower()` 成为全函数。实测：GROUP BY 10 万行/1000 组只持有 1000 条目（旧路径持有 10 万克隆行 + 10 万 key 向量）；ORDER BY 10 万行 = 13 个 run 归并、单块时不再归并；`SELECT a+1 … LIMIT 10` 投影求值 10 万 → 10、进 chunk 10 万 → 8192。630 个 executor 单测、clippy/fmt 干净、Gate A 1752/1752 + 129/129 + 1546/1546。**未移植（沿 D4）**：`TaskScheduler`/`FragmentContext`/`BlockingQueue`（单 fragment、`TaskletCount` 恒 1）、`MergeTop`/`PhysicalMergeSort`；`Top` 是整表累加+排序+切片而非 bounded heap；扫描仍先整体物化再按块下发；`distinct` 仍是 LIMIT 之后的独立后处理 |
| 23 | 非 `Eq` 谓词不从索引回答、掩码全 false 退回全部行 | ✅ | 整数范围先落地（`54454e892` 范围读 + `948731581` 执行器接线 + `a6f638ef7` 测试）。随后发现更严重的问题：参考侧只对 Varchar 哈希，FLOAT/DOUBLE 走 `KeepOrderedSelf`、DATE/TIME 走 `ConvertToOrderedI32`、DATETIME/TIMESTAMP 走 `ConvertToOrderedI64`，都是**原样有序键**；Rust 一律哈希成 8 字节 u64，导致 C++ 写的这些类型次级块在 Rust 侧**宽度都不对、根本读不了**。`23854bf09` 合并 `KeyKind::{U64,F32,F64,I32,I64}`：原始 IEEE/整型位、浮点按 `total_cmp` 比较（否则负数排到正数之后）、`Segment` 按 `#pragma pack(1)` 为 12/16/20 字节、U64 路径字节不变。手工构造的 C++ 形状 fixture（f32/f64/i32/i64）都能打开，且用 U64 打开会**结构化失败**而不是读到垃圾。**剩余**：VARCHAR 范围与参考一致地退回全表过滤（不是缺陷）；日期/时间的**严格** `<`/`>` 仍回退，因为 planner 的严格界重写是字符串字典序（`\"1970-01-00\"` 不是合法日期），需要在 planner 侧按列类型重写——已登记 |
| 24 | `FastRoughFilterEvaluator` 无生产实现/调用方 | ✅（已登记） | `ApplyFastRoughFilter::build`/求值器已实现且单测覆盖，但 Rust 没有列级 min/max + bloom 统计可供 `IndexCatalog` 填充（C++ 由 block 级统计提供），默认管线挂空 catalog ⇒ 只能折叠字面 false；`Optimizer::with_catalog` 无生产调用方。已登记：只影响跳块性能，不影响结果 |
| 25 | `ResultCacheGetter` 仅测试构造 | ✅（已登记） | 生产用语句级结果缓存（语句文本为键、任何写原语失效），比 C++ 的计划级缓存（begin-ts + 表键 + 列投影）更粗但更保守；`Optimizer::with_result_cache` 作为 public API 保留。已登记 |
| 26 | `SHOW SEGMENTS/BLOCKS` 只有合成元数据 | ✅ | `d385fb7c8`：真实段/块元数据 + `FormatByteSize` 移植 |
| 27 | `BindContext` 无对应类 | ✅ | `2d09f0b8c`：`infinity-planner/src/bind_context.rs` 逐字段/逐方法移植 `binding.cppm`+`bind_context.cppm`（计数委托根 context、`get_cte` 认 `masked_name_set`、`add_bind_context` 重名报错、`resolve_column_id` 的 Ambiguous/`doesn't exist.` 文案、`bound_search` 的 distance/similarity 冲突规则）；`plan_select` 与 `QueryBinder::bind_select` 都先建根 context，`t.c` 限定列从此可绑定，两表同名未限定列报 ambiguous。11 个类级测试 + 1 个绑定器测试。未移植字段（`column_types_`/`table_info_`/`block_index_`/`expression_binder_`）已登记进 `CLASS_MAP.md`「有意的设计变更」 |
| 28 | `BGTaskProcessor` 从未 attach | ✅ | `81c8cca58`：`Engine::start_background_tasks(cleanup, compact, optimize)` 起 `BGTaskProcessor::new(1)` 并 attach（内存索引配额从此走后台提议路径），再起 1 s tick 线程复刻 `PeriodicTriggerThread::Run`（`Weak::upgrade` 逐 tick 判断，engine drop 后一 tick 退出）；`CleanupTask`/`OptimizeTask` 与 `DumpMemIndexTask` 同形。带 `last_cleanup_ts` 门槛（`needs_cleanup()`），否则空闲服务器会每 60 s 全量 checkpoint。**同时修掉 seal 竞争**：`dump_all_memory_indexes` 现在拿 `storage.flush_lock`（锁序恒为 `batch_lock → flush_lock`，无反向路径）。`compact_interval` 接成 `TODO(item 1b)` 空转；CLI 只把 optimize 的裸线程换掉，checkpoint 线程保留（`trim_allocator` 是 `malloc_trim`，executor 禁 unsafe）。测试：`background_cleanup_task_removes_dropped_files`（走任务而非直调 `cleanup()`）、`start_background_tasks_sets_processor`。已登记「四 processor 合一」 |
| 29 | HTTP 路由缺 `/admin`、`/instance`、`/snapshots`、`/variables` | ✅ | `d385fb7c8` 已实现 30 条真实路由（含 `SHOW SEGMENTS/BLOCKS` 的真实元数据）。分支 `accessors` 把余下 23 条翻成真实 JSON，并修掉 8 个硬编码 `"0"` 的全局变量：`buffer_object_count`/`unused_buffer_object`/`total_rollback_count`/`bg_task_count` 读真实计数器（引擎实际用的是 `BufferManager<RawBlockColumn>`，不是未接线的 `BufferPool`）、`system_memory_usage` 读 `/proc/self/status` VmRSS、`open_file_count` 数 `/proc/self/fd`、`cpu_usage` 按 `Δticks/CLOCK_TCK/Δwall/online_cpus`；新增 `diagnostics.rs`（buffer/file/memindex/txn/query/profile/variables），查询注册表带 drop guard 覆盖错误路径，`SET` 规则按 `PhysicalCommand::ExecuteSet`（会话变量只读、仅 `profile`/`profile_record_capacity` 可设、未知名 3076）。顺带修 `EngineTxn::rollback` 不累加 `total_rollbacked_txn_count`。**仍 501 的 3 条**（各有明确理由）：`/instance/logs`（需要 `WalCmd` 的文本渲染）、`/instance/memory/{objects,allocations}`（需要 per-object allocator tracker）、`/admin/logs`（没有日志文件） |
| 30 | 未知标量函数静默返回 NULL | ✅ | `3168c6e59`（注册表 67+16+14 特殊名）、`8866b848a`（planner 两个 `Func` 分支强制）、`22eb9ce9f`（补 `CREATE_TIMESTAMP` 等）、`94987cb24`（`UNNEST`） |

### 算子模型迁移切片（条目 22）

目标：按 `physical_operator.cppm:31` / `operator_state.cppm:40` 引入算子对象 + 每算子状态 + push 驱动，把 `Vec<Vec<Value>>` 整表物化改为 8192 行 `DataChunk` 流；`run_plan` 签名不变，`decompose`/`pipeline` 分片淘汰。每片独立可编译、测试保持全绿：

1. 新增 `chunk.rs`（`DataChunk`：`row_count/capacity/available_capacity/append_with/finalize/into_rows/from_rows`），无调用方；
2. `operators/{mod,project,lower}.rs`：trait + `OperatorState` + `run_fragment` 驱动，仅接 Projection（`run_plan` 一个分支），旧 `pipeline` 保留回退；
3. Filter；
4. Limit/Offset/Top 早停（扫描游标，`total_hits_count_flag` 时禁用早停）；
5. Aggregate（去 `r.clone()` 分组克隆，O(1) 哈希分组）；
6. Sort（分块 run + k 路归并，tie 取先出现的 run）；
7. Source 算子（`apply_index_filter`/`align_rows` 进 scan）；
8. 删除 `Stage`/`Source`/`decompose`/`pipeline`，更新 `CLASS_MAP.md` 行 78。

需保持的语义（每片都要过）：ORDER BY tie 为插入序；GROUP BY 输出为首见序；`#_row_id`/`#_score` 隐藏列位置不变；`distinct` 仍在 LIMIT 之后；空源仍产出一个默认块。并发/`exchange_capacity` 保持单线程并在 `CLASS_MAP.md` 登记（与 §3.6 ① 对 `TaskScheduler` 的豁免一致）。

### 2026-10-20 后续发现（审计时未列，同样属于「名义存在但不接线」）

| 优先级 | 条目 | 证据 | 影响 |
|---|---|---|---|
| P1 | HTTP `/variables/*`、`/instance/{buffer,objects,queries,transactions,profiles,memindex,files,logs,memory}`、`/admin/logs` 等 14 条路由返回 501 | `infinity-server/src/http.rs`（`d385fb7c8`） | 缺 `Engine` accessor：`SetScope` 私有、`buffer_objects`/`active_transactions`/`profiles`/`memory_indexes`/`all_files`/`logs`/分配器统计。参考实现有 27 条路由 |
| P2 | `global_variable_value()` 对 `buffer_object_count`/`unused_buffer_object`/`active_txn_count`/`total_rollback_count`/`bg_task_count`/`system_memory_usage`/`open_file_count`/`cpu_usage` 硬编码 `"0"` | `infinity-executor/src/lib.rs` | `SHOW VARIABLES`/HTTP 会报假值；需上面同一批 accessor |
| P0 | WAL 帧格式是自创的 | ✅ 已修 | `fbf444257` 去掉 `WAL1` magic，按参考的 `WalEntry` 布局写读（24 字节 header、`i32 cmd_count`、命令列表、末尾再写一次 size、CRC32IEEE 覆盖整条且校验和字段置 0）；`439f50dfb` 把文件职责拆开：`wal/wal.log` 只放一条 C++ framing 的 `kNewCheckpoint` 条目，本引擎的 redo 记录进 `wal.rust`，于是参考不必解码我们写的任何一条记录。**证据**：手写 C++ 布局的 golden 字节逐字节相等（`writer_emits_the_reference_bytes_and_reader_accepts_them`），坏字节/末尾 size 不符/截断三种坏帧分别被对应检查拒绝；跨引擎测试中参考接受我们的 checkpoint（`Checkpoint found, replay the catalog`） |
| P2 | `SHOW SEGMENTS` 的 `status` 恒为字面量 `"No value"` | `physical_show_segments_impl.cpp:37` | 参考实现本身也读已删除的 `SegmentEntry::status_`，属参考侧遗留，非遗漏 |

### 提交归属（2026-10-20）

并行 agent 在同一工作树里改同一个热点文件（`infinity-executor/src/lib.rs`）时，`git add <file>` 会把**别人尚未提交的 hunk 一起带走**：`948731581`（BMP 选项）实际包含了 bindctx-15 的整套整数范围索引扫描。两次复现（另一次是 `d05c0f227` 把已提交的 `memindex_pending` 修复回退，后由 `760e878b7` 恢复）。为此：本条目的功能归属以哈希链 `54454e892` → `948731581` → `a6f638ef7` 为准；后续热点文件同时只允许一个 agent 写，其余待命，且每个 agent 提交后要 `git show <hash>` 复核删除行是否属于别人。

| P1 | `result_cache` 默认值与参考相反 | `Engine::result_cache_active()`；C++ `DEFAULT_RESULT_CACHE = "off"`（`src/common/default_values.cppm:211`） | **已修**（`fd4d8677f`）：配置缺失时不再缓存、`SHOW CONFIG` 默认改为 off、缓存关闭时按 `ExecuteShowGlobalVariables` 整行省略 `cache_result_capacity`；同时 `cache_result_capacity` 不再泄漏 `usize::MAX` 哨兵（`80dddcc71`） |

| P1 | Rust 写出的目录 C++ 报 `segment_count = 0` | ✅ 已修 | 一串独立的缺陷叠在一起：`dc6361ab3` 写出 `catalog|seg|…`/`catalog|blk|…`/段标签/表上的 `next_segment_id`；数据库编号从 0 基改成参考的 1 基；`a3e415a40` 把每个索引的封盘 chunk 镜像进 `idx_{id}/seg_0/` 并发布 `catalog|idx_chunk|…|chunk_info`；`79a9d8808` 为变长列补上参考必读的 `col_{id}_out`（`ColumnMeta::FilePaths` 列出它，缺失即 abort 整个读路径）；`e92f34f8b` 发布真实的 BMP `block_size`（`BMPHandler::Load` 按它定结果向量大小）；`99b8a7c26` 让 BMP 倒排表按列声明的 bound 定尺寸（参考按维度建 `BMPAlg`，而参考读文件时段间没有长度字段，term 数少 2 就整体偏移 32 字节）。**证据**：跨引擎测试两个方向都通过，参考能 `RecoverMemIndex ... append_ranges 1: [(0, 200)]` |
| P2 | SPARSE 列的 `show_columns` 类型文本两引擎不一致 | ✅ 已修 | 参考打印 `Sparse(float,int32,8)`，端口打印 `Sparse(float,int8,0)`：`describe_type` 按**两字段** `SPARSE(elem,bound)` 解析，于是把索引类型当成了 bound（解析失败 → 0），再按 bound 反推索引宽度（→ int8）。规范写法是**三字段** `SPARSE(elem,index,bound)`，两字段形式才从 bound 反推宽度（参考的 `SparseInfo::GetIndexType`）。修正后端口与参考打印一致，`describe_type_names` 把每种类型的拼写都钉住。**影响**：此前参考读到端口写的 catalog 后，稀疏列的 bound 变成 0，于是它的稀疏索引没有段（`Cannot find segment with id: 0`）——这条 P1 的现场之一也是它造成的 |

| P1 | 两张表各有全文索引时，第二张表的索引记录丢失 | 端口在 cpp_only 模式下写 `catalog|idx|{db}|{tbl}|{name}|{ts}` 与 `idx|…|index_base`；两张表都建全文索引时，读回的 `IndexMeta` 只有第一张表的（`search_text_terms` 报 "fulltext index does not exist for column 'body'"） | 最小复现：建 `a1`、`b1` 两张表（都有 `body` 列），各建一个全文索引，`a1` 能匹配、`b1` 报 "fulltext index does not exist"。**影响**：跨引擎测试若用两张表（一种类型一张、一种索引一张）就会撞上它；当前测试因此保持单表。**待查**：`Catalog::cpp_indexes` 的 `index_meta_of` 按表 id 解析表名时，第二张表的 id 没有落到正确的表上 || P1 | 16 位浮点（`FLOAT16`/`BFLOAT16`）的列：读侧看不到任何行 | 端口的 C++ 布局块读取器 `column_type_of`（`cpp_blocks.rs:731`）不认识 16 位浮点的 `ColumnType` 码（7/8），`flush_cpp_blocks` 因「有列不认识」而**整表跳过**块镜像（`cpp_blocks.rs:1394`），于是目录里既没有 `db_*/tbl_*/seg_0/blk_0/*.col` 也没有 `version`；参考读这样的目录时 `segment_count 0`、count 0。 | **与列数无关**：二分定位（9/12/13/19 列逐个试）显示阈值是**列的类型**——12 列（含 `FLOAT16`）正常，13 列（多一个 `BFLOAT16`）失败；把两列换成 `FLOAT`/`DOUBLE` 后 19 列同样正常。参考自己写的 19 列表一直带着 `catalog|seg|…`，此前「参考不写段记录」的结论来自只建表未插入的探针 + 用 `strings` 扫 RocksDB 文件（压缩块里的键看不到），两者都不成立。 | **修法**：补上 `FLOAT16`→7、`BFLOAT16`→8 的类型码，并让固定宽度解码/编码按 2 字节读写（`f16_bits_to_f32` / `f32_to_f16_bits`；bfloat 的原始位是 `f32` 的高 16 位，与 C++ 的 `bfloat16_t` 一致）。修后 19 列（含 `FLOAT16`/`BFLOAT16`/`TINYINT`/`SMALLINT`/`TIME`/`DATETIME`/`TIMESTAMP`）在参考与端口之间双向交接通过（`python/restart_test/test_cross_engine_compat.py`，2 passed）。 | **顺带修掉的两处参考侧兼容缺陷**：(1) 索引 id 映射——端口把 chunk 写到 `idx_{按名字排序的位置}`，而目录里发布的是**创建顺序**的 id，参考按 id 找 chunk 时读到另一个索引的文件（`idx_3` 是稀疏索引的 chunk，参考却当 HNSW 解析 → `LoadFromPtr failed`）；改为直接用 catalog 的 `IndexMeta::id`，并让 `IndexDefValue` 带上参考记录的 id（`catalog|idx|…` 的值），导入时也保留它。(2) chunk **id** 键（`idx_chunk|{db}|{tbl}|{idx}|{seg}|{chunk}`）——参考靠它枚举一个段的 chunk（`SegmentIndexMeta::LoadChunkIDs1`），缺这个键时参考看不到任何 chunk，把整段当成索引缺口去重建。补上后参考会直接读端口写的 chunk 文件。 |

## 处置口径

1. **P0 全部要改**（磁盘格式/结果正确性/互操作），改完补针对性回归（C++ 写、Rust 读；Rust 写、C++ 读）。
2. **P1 逐项判定**：要么接线（`FilterExpressionPushDown`、`pg`/HTTP 路由、bg_task 调度、`BindContext`），要么写进「有意的设计变更」并说明理由与影响面。
3. **P2 可登记后保留**，但必须在表里写清「差在哪、影响什么、何时需要补」。
4. 审计与处置进度在本文件更新；`CLASS_MAP.md` 的 ✅ 以本文件为准收紧。
