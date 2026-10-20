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
| 5 | `Storage` 无状态机 | ⏳ | 缺 `Get/SetStorageMode`、`InitToAdmin`/`AdminToReader`/`ReaderToWriter`、`RecoverMemIndex`、`AttachCatalog(ts)`；接线方案研究中 |
| 6 | `BufferManager` 缺对象管理/分片 | ✅ | `fff4b4ff8`：分片 `lru_caches_`、`RequestSpace`、`buffered_object_count`/`unused_buffer_object` |
| 7 | `VarBufferManager` 是类型别名 | ✅ | `24429ee91`：真正的 outline-backed manager + `free_success` 预算钩子 |
| 8 | `BlockVersion` 只有 `SpillToFile` | ✅ | `b20b5632f`：checkpoint 剪枝 `save_to_file(ckp_ts)`、`commit_row_count`、`restore_from_snapshot` |

### 全文

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 9 | 封盘段不存位置（`add_position` 无参数） | ✅ | `a16e1b361`（writer/reader 位置流）+ `ac909e68b`（executor 两条 seal 路径携带位置、spill 合并读 `positions()`）；一次性格式破坏已声明 |
| 10 | `RowFulltextProvider::postings` 返回所有行 | 🔄 | `RowFulltextProvider` 改走列倒排索引的 `Lookup` 并按扫描行 id 求交；同列两个不同分词器的索引回退为保留该行。实现完成，待提交哈希（workspace 被并发编辑阻塞）|
| 11 | `index_delims` 抹平分析器 | 🔄 | `TextIndex` 增加 `TextTokenizer { delims, analyzer }`，insert/query 都走 `AnalyzerPool::get_analyzer`。实现完成，待提交哈希 |
| 12 | `QueryBuilder`/短语/布尔迭代器不在产品路径 | ⏳ | MATCH 仍只走「单 term WAND，否则整行重分词」 |
| 13 | `MemoryIndexer` 无 spill-to-sorted-runs / `InsertGap` | ⏳ | `FULLTEXT_SPILL_SUFFIX` 声明未用，`SortMerger` 无调用方 |
| 14 | BMW 无块级最大值、非增量 | 🔄 | `wand.rs` 移植中（per-block maxima + `Next/NextShallow`） |
| 15 | `StandardAnalyzer` 用 Porter2 顶替 `STEM_LANG_PORT` | ⏳ | 只影响 `standard-porter`；rust-stemmers 无原版 Porter 算法 |

### 索引

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 16 | EMVB OPQ 写成单位阵 | ✅ | `a19d08364`：OPQ 训练（6 轮 + 通用 `JacobiSVD` 移植）、查询侧 `R^T q`、R 存 chunk；`R=U·V^T` 对符号/排序不变 |
| 17 | 低基数缺 `count_` | ✅ | `d13d21834`：`WriteAdv` 逐字节，`count_` 为 chunk 级 `max_offset+1`，run/array/bitset 选择 + cookie 复刻；fixture 由 CRoaring 实测生成 |
| 18 | PGM 单层、`levels_offsets` 恒 0 | ✅ | 同上：真正的 `PGMIndex<u64,64,4,float>` 递归层级 + C++ 字节 fixture（1 级/2 级各一） |
| 19 | Plaid `MergeChunks`/`DiskMerger` 零调用 | ⏳ | 引擎仍从 tail 重建整块；接线方案研究中 |
| 20 | `InitAbstractIndexT` 硬编码 M/ef_construction | ✅ | `04ee86483`：从索引选项读取并写入 chunk 头 |
| 21 | BMP 搜索选项恒 default | 🔄 | `parse_bmp_search_options` 解析 `alpha/beta/use_tail`，WITH 优先于 option 字符串，超范围忽略；磁盘路径原先硬编码 0.9 一并修正。实现完成，待提交哈希 |

### 规划 / 执行 / 网络

| # | 条目 | 状态 | 提交 / 说明 |
|---|---|---|---|
| 22 | 不存在 `PhysicalOperator`/`OperatorState` 模型 | ⏳ | 见下「算子模型迁移切片」 |
| 23 | 非 `Eq` 谓词不下推、掩码全 false 退回全部行 | 🔄 | `ac909e68b`：掩码全 false 返回空集（已收敛一半）；非等值谓词仍取全表内存过滤 |
| 24 | `FastRoughFilterEvaluator` 无生产实现/调用方 | ⏳ | 默认规则空 `IndexCatalog`，只能折叠字面 false |
| 25 | `ResultCacheGetter` 仅测试构造 | ⏳ | 生产是「整条 `QueryResult` 按语句文本做键」，无 begin-ts/表键/列投影 |
| 26 | `SHOW SEGMENTS/BLOCKS` 只有合成元数据 | ✅ | `d385fb7c8`：真实段/块元数据 + `FormatByteSize` 移植 |
| 27 | `BindContext` 无对应类 | ✅ | `2d09f0b8c`：`infinity-planner/src/bind_context.rs` 逐字段/逐方法移植 `binding.cppm`+`bind_context.cppm`（计数委托根 context、`get_cte` 认 `masked_name_set`、`add_bind_context` 重名报错、`resolve_column_id` 的 Ambiguous/`doesn't exist.` 文案、`bound_search` 的 distance/similarity 冲突规则）；`plan_select` 与 `QueryBinder::bind_select` 都先建根 context，`t.c` 限定列从此可绑定，两表同名未限定列报 ambiguous。11 个类级测试 + 1 个绑定器测试。未移植字段（`column_types_`/`table_info_`/`block_index_`/`expression_binder_`）已登记进 `CLASS_MAP.md`「有意的设计变更」 |
| 28 | `BGTaskProcessor` 从未 attach | ⏳ | `cleanup_interval`/`compact_interval` 解析但不调度；接线方案研究中 |
| 29 | HTTP 路由缺 `/admin`、`/instance`、`/snapshots`、`/variables` | 🔄 | `d385fb7c8` 已实现 30 条真实路由；余 14 条返回 501 + `error_code 3032`，等 `Engine` accessor |
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
| P2 | `SHOW SEGMENTS` 的 `status` 恒为字面量 `"No value"` | `physical_show_segments_impl.cpp:37` | 参考实现本身也读已删除的 `SegmentEntry::status_`，属参考侧遗留，非遗漏 |

## 处置口径

1. **P0 全部要改**（磁盘格式/结果正确性/互操作），改完补针对性回归（C++ 写、Rust 读；Rust 写、C++ 读）。
2. **P1 逐项判定**：要么接线（`FilterExpressionPushDown`、`pg`/HTTP 路由、bg_task 调度、`BindContext`），要么写进「有意的设计变更」并说明理由与影响面。
3. **P2 可登记后保留**，但必须在表里写清「差在哪、影响什么、何时需要补」。
4. 审计与处置进度在本文件更新；`CLASS_MAP.md` 的 ✅ 以本文件为准收紧。
