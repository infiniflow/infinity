# C++ → Rust 类/方法对照（移植原则 §3.6）

> 目的：防止为了过测试而实现**过度简化**的设计。原则上（大体上），
> 每个被其他 C++ 类依赖的类、以及这些调用点用到的方法，都应在 Rust 中有
> 同职责的类型与 public 方法，并可逐项回溯到 `文件:行号`；已明确决定的设计变更
> （调度器、FST、SIMD/压缩原语等）不要求同名一一对应，但必须登记。

## 已一一对应

| C++ | Rust | 方法对应 | 状态 |
|---|---|---|---|
| `BufferManager`（`storage/buffer/buffer_manager*.cppm`） | `infinity_storage::buffer::{BufferManager, BufferPool}` | `Get`→`get`、`Put`→`put`、LRU 淘汰→`evict`、`SetBufferSize`→`set_budget`；`AllocateBufferObject`→`BufferPool::allocate`/`allocate_persistent`、加载/保存/释放→`BufferObj::{load,save,free}` | ✅ |
| `BufferObj` / `BufferHandle`（`storage/buffer/buffer_obj.cppm`、`buffer_handle.cppm`） | `infinity_storage::buffer::{BufferObj, BufferHandle}` | `Load`→`load`（按池预算计费，超限报错）、`GetData/GetDataMut`→`data`/`data_mut`、`Save`→`save`、`Free`→`free`、`rc`/`status`/`type`；句柄 Drop 递减 rc，ephemeral 溢出到临时文件、persistent 常驻、transient 随句柄释放 | ✅ |
| `BlockVersionFile`（`storage/buffer/block_version.cppm`） | `cpp_format::BlockVersionFile` | `Decode/Encode` | ✅ |
| `TableDef` / `ColumnDef` / `IndexBase` 系列（`table_def.cppm`、`column_def.cppm`、`index_def*.cppm`） | `infinity_catalog::{TableMeta, IndexMeta, ColumnDefValue, IndexDefValue}` + `cpp_layout` | catalog 键空间按 C++ 布局编解码；`IndexFullText/Secondary/HNSW/IVF/BMP/EMVB` 的参数存于 `IndexMeta.options` | ✅ |
| `SecondaryIndexDataBase::SaveIndexInner` | `secondary_segment::write_secondary_segment` / `SecondarySegmentReader` | 保存/查找 | ✅ |
| `SecondaryIndexDataLowCardinality` | `secondary_lowcard::LowCardWriter/Reader` | `WriteAdv` 兼容的 Roaring | ✅ |
| `PGMIndex<T,64,4>` | `secondary_segment` 内嵌 PGM（`pgm_*`） | `SaveIndex/LoadIndex`、有界二分 | ✅ |
| `BMPAlg::SaveToPtr` | `bmp_cpp::write_bmp_file` / `BmpCppReader` | 保存/读取/`SearchKnn` | ✅ |
| `EMVBIndex::SaveIndexInner` / `PQ::Save` | `emvb_segment::encode_emvb_refs` / `EmvbPqIndex` | 保存/预过滤 | ✅ |
| `HnswIndex`（图区/头部） | `knn_format::encode/decode_hnsw_chunk` | 字节布局 | ✅ |
| `IVF_Index_Storage::SearchIndex` | `infinity_index::ivf::IvfIndex::candidates` | 质心选择 + part 扫描 | ✅ |
| `Fst` 尾块/校验和 | `inverted_format` 的 FST 写入/`fst_len_from_trailer` | 保存/校验 | ✅ |
| `std::hash<string_view>`（VARCHAR 次级键） | `secondary_segment::hash_varchar` | MurmurHash64A | ✅（g++ 值已 pin） |
| `TxnManager`（`new_txn_manager`） | `infinity_txn::TxnManager` | `BeginTxn`→`begin`、`CommitTxn`→`commit`（含删除冲突校验） | ✅ |
| `NewTxn::Delete/Commit` 冲突检查 | `Txn::delete` + `TxnManager::commit` | 删除键在提交时按 `begin_ts` 校验 | ✅ |
| `BM25Ranker`（`bm25_ranker_impl.cpp`） | `wand::Bm25Ranker` | `idf`/`score`/`upper_bound` | ✅ |
| `BlockMaxWandIterator`（`blockmax_wand_iterator_impl.cpp`） | `wand::BlockMaxWandIterator` | `Next`→`run`（pivot/NextShallow/block-max 剪枝） | ✅ |
| `HnswIndexInMem`（`hnsw_alg.cppm`） | `hnsw_mem::HnswIndexInMem` | `Add`→`insert`、`Search`→`search`、`MemUsage` | ✅ |
| `HnswHandler`（`hnsw_handler.cppm`） | `infinity_index::hnsw_handler::HnswHandler` | `Make`→`new`/`from_index`、`InsertVecs`→`insert_vecs`（按序，调度器设计变更）、`SearchIndex`→`search_index`（`KnnSearchOption.ef`→`ef`）、`MemUsage`→`mem_usage`、`GetDocNum`→`len` | ✅ |
| `LVQVecStoreMeta` / `LVQVecStoreInner` + `LVQ{L2,Cos,IP}Dist` + 缓存（`lvq_vec_store.cppm`、`dist_func_{l2,cos,ip}.cppm`） | `infinity_index::lvq::{LvqStore, LvqData, LvqQuery, Metric}` | `CompressTo`→`compress`（mean 中心化 + i8 标量量化 + scale/bias）、`MakeQuery`→`make_query`、L2/Cos/IP 距离与 local/global cache、`Optimize`→`build/train`（重算 mean）、meta/records 按 C++ 布局读写；`HnswIndex::with_encoding(Lvq)` 使用 | ✅ |
| `RabitqVecStoreMeta` / `Inner` + `Rabitq{L2,Cos,IP}Dist`（`rabitq_vec_store.cppm`、`dist_func_{l2,cos,ip}.cppm`） | `infinity_index::rabitq::{RabitqStore, RabitqData, RabitqQuery, Metric}` | Givens 随机正交矩阵、旋转质心、1-bit 符号码、MSQ u8 查询、`RecoverIpDistance`/`RecoverL2DistanceSqr`、三指标恢复公式、meta/records 布局；`HnswIndex::with_encoding(Rabitq)` 使用 | ✅ |
| `HnswLSGBuilder` / `LSGConfig` / `LSGDistWrapper`（`hnsw_lsg_builder.cppm`、`dist_func_lsg_wrapper.cppm`、`index_hnsw.cppm`） | `infinity_index::lsg::{LsgConfig, LsgIndex}` | 按 `sample_ratio` 采样、每行对样本的前 `ls_k` 距离均值、`d / sqrt(avg_i*avg_j)^alpha`、`lsg_config` 解析；构图期作为 HNSW 距离包装 | ✅ |
| `InitAbstractIndexT`（`hnsw_handler_impl.cpp`） | `HnswIndex::with_encoding` + `HnswIndexInMem::new_with` | `encode` ∈ plain/lvq/rabitq、`build_type` ∈ plain/lsg、`metric` ∈ l2/ip/cosine 组合选择存储与距离；chunk 的 vec-store meta 按选择写读 | ✅ |
| `MemoryIndexer`（`memory_indexer.cppm`） | 执行器 `MemoryIndexer` | `Insert`→`insert`（含预算检查）、`MemUsed`→`mem_used`、`GetDocCount`→`doc_count`、`Reset`；删除只去掉 live 标记（postings 保留到压实） | ✅ |
| `ColumnIndexReader`（`column_index_reader.cppm`） | 执行器 `ColumnIndexReader` | 每段 `IndexSegmentReader`→`fulltext_segment::SegmentIndexReader`；`GetIndexReader(term)`→`postings`、`GetColumnLength`→`doc_len`、跨段 `doc_count`/`total_len`/`contains` | ✅ |
| `EMVBIndex::GetQueryResult` / `EMVBSearch`（`emvb_search_impl.cpp`） | `infinity_index::emvb::{EmvbView, EmvbSearchOptions}` | 三阶段：`find_candidate_docs`→centroid 候选、`compute_hit_frequency`、`second_stage_filtering`、`compute_topk_documents_selected`（残差 PQ 精排）；`PLAID_*` 默认值 | ✅ |
| `EMVBIndex::Train` / `EMVBProductQuantizer` | `emvb_segment::{coarse_centroids, train_residual_pq, encode_emvb_refs}` | 质心 k-means、残差 PQ 训练与编码（写入路径） | ✅（编码函数而非类） |
| `EMVBIndexInMem`（`emvb_index_in_mem.cppm`） | 执行器 `MultiVectorIndex`（`MvTailEmvb` + `EMVB_BUILD_THRESHOLD`） | `Insert`→`insert`（tail）、`IsBuilt`→按 `32 * (1 << 8)` 阈值惰性 `build_emvb`、`SearchWithBitmask`→`EmvbView::search` + `tombstone_set` | ✅ |
| `MergeKnn` + `HeapResultHandler`（`merge_knn.cppm`、`result_handler.cppm`） | `infinity_index::merge_knn::MergeKnn` | `Search/AddResult`→`search/add_result`、`Begin/End/EndWithoutSort`、`GetSize/GetIDsByIdx/GetDistancesByIdx/GetUniqueIDs`；`CompareMax/CompareMin`→`Compare::{Max,Min}` | ✅ |
| `BlockFwd` / `TailFwd`（`bmp_fwd.cppm`，own-mem） | `infinity_index::sparse::{BlockFwd, TailFwd, BlockTerm}` | `AddDoc`→`add_doc`、`ToBlockFwd`、`GetBlockTerms`、`GetScores`/`GetScoresTail`、`Prefetch` | ✅ |
| `BMPIvt` / `BlockData` / `BlockPostings`（`bmp_ivt.cppm`，own-mem） | `infinity_index::sparse::{BMPIvt, Postings}` | `AddBlock`→`add_block`、`GetPostings`、`Optimize`→`build`、`kth`、`block_ids`/`max_scores` | ✅ |
| `BMPAlg` / `BMPAlgBase::SearchKnn`（`bmp_alg.cppm`，own-mem） | `infinity_index::sparse::{BmpAlg, BmpSearchOptions}` | `AddDoc`→`add_doc`、`SearchKnn`→`search_knn`、`GetScores`/`Calculate2`、`alpha/beta/use_tail` | ✅ |
| `BMPIvt`/`BlockFwd` 的 `BMPOwnMem::kFalse`（mmap） | `infinity_storage::bmp_cpp::BmpCppReader` | 读取同一份 `SaveToPtr` 字节布局并查询 | ✅（设计变更：读路径共用磁盘格式） |
| `KnnScanSharedData` / `KnnSearchOption`（`knn_scan_data.cppm`） | `knn_scan::KnnScanOptions` | `nprobe`/`ef`/`threshold` 从 `WITH (…)` 与 legacy 串解析（`WITH` 优先），供 IVF/HNSW/阈值共用 | ✅ |
| `PhysicalKnnScan`（`physical_knn_scan.cppm` / `physical_knn_scan_impl.cpp`） | `knn_scan::{PhysicalKnnScan, KnnScanOperatorState}` + `Engine::knn_candidates` | `GetColumnID`→`column_index`、`IsKnnMinHeap`→`is_knn_min_heap`、`Execute`→`execute`（逐行 + 行缓存的精确评分）、`PlanWithIndex` 的索引探测→`KnnCandidateScan`/`vector_candidates`、排序/阈值/截断→`finish`（`MergeKnn` 方向语义）；bitmask 过滤由过滤谓词承担，多查询由上层逐次调用 | ✅（多 block 并行由行管线代替，见 D4） |
| `TableMeta` / `SegmentMeta` / `BlockMeta` / `ColumnMeta`（`catalog/meta/*.cppm`） | `infinity_catalog::{TableMeta}` + `infinity_storage::block_index::BlockMeta` + `cpp_layout` | 表/段/块/列的元数据按 C++ catalog 布局持久化；`BlockMeta` 提供 `block_dir`/`row_offset`/`row_count`/`GetRowCnt1` 语义，`infinity_catalog` 提供表与列定义 | ✅ |
| `BlockIndex`（`storage/common/block_index.cppm`） | `infinity_storage::block_index::BlockIndex` | `NewInit/LoadSet`→`open`（扫描 `version` 文件恢复块与行区间）、`BlockCount`/`SegmentCount`、`GetBlockMeta`、`GetSegmentOffset`/`GetBlockOffset`、`IsEmpty`；块列解码经共享 `BufferManager` LRU；checkpoint 调 `create_block` 落块，块读与全行读（`try_cpp_rows`）都走它 | ✅ |
| `Storage`（`storage.cppm`） | 执行器 `Engine::storage: Storage` + `Engine` 的 `txn`/`dir`/`context` | `new_catalog`/`kv_store`/`new_txn_manager`/`buffer_manager`/`wal_manager`/`object_storage_processor` 分别由 `Storage` 与 `Engine` 组合；后台处理器由 CLI 定时线程 + Tokio 取代（D4） | ✅ |
| `InfinityContext`（`infinity_context.cppm`） | 执行器 `Context` | `config`/`BufferManager`/`memindex_memory_quota`/`scan_gate`/`object_store`/exchange 容量；`Engine` 组合而非内联；C++ 线程池由 Tokio 取代（D4） | ✅ |
| `ColumnVector<T>` / `DataBlock`（`column_vector.cppm`、`data_block.cppm`） | `infinity_storage::data_block::{ColumnVector, Column, DataBlock}` | `AppendValue/AppendWith`→`push`/`extend_value`、`GetValue`/`SetValue`→`get`/`set`、null bitmap→`validity`、`DataBlock::Make/AppendRow/Finalize/GetValue/Reset`→`new`/`push_slot`/`finalize`/`value_text`/`reset`、`MemUsed`→`mem_used` | ✅（执行器求值仍以 `Value` 行渲染块内容；列容器本身已移植） |
| `RegexLiteralRun` / `ExtractRegexLiteralRuns`（`planner/optimizer/regex_literal_extractor.h`） | `regex_literal::{extract_regex_literal_runs, has_any_sparse_gram, parse_sparsegram_analyzer}` | 只抽取每次匹配都必含的字面 run；`\Q…\E`/`\C`/内联 `(?i:)` 标志、量词回退、备选分支丢弃、ASCII fold 切分、`sparsegram-<min>-<max>[-fold]` 解析与 gram 下限 | ✅ |
| `DumpMemIndexTask` / `OptimizeIndexTask` / `CleanupIndexTask`（`bg_task/`） | `Engine::{dump_hnsw_segments, optimize_indexes, cleanup}` | dump 按 `mem_index_capacity`/字节配额写增量 chunk；optimize 在可承受的内存内重建全表 merged chunk；cleanup 只删除被 merged chunk 覆盖的旧 chunk（调度由 CLI 定时线程代替，见 D4） | ✅（任务对象由定时线程取代） |
| `PlaidQuantizer`（`plaid_quantizer.cppm`） | `infinity_index::plaid::PlaidQuantizer` | `Train`（全局均值 + 分位切断点/权重，超限按 `MAX_RESIDUAL_VALUES` 蓄水池采样）、`Quantize/Dequantize`（2/4-bit MSB-first 打包、反位映射、L2 归一）、`GetIPDistanceTable`/`BuildPositionLUT`/`GetSingleIPDistance*`、`Save/Load` 字节布局 | ✅ |
| `PlaidGlobalCentroids`（`plaid_global_centroids.cppm`） | `infinity_index::plaid::PlaidGlobalCentroids` + `kmeans_l2` | `Train`（k*256 采样 + `GetKMeansCentroids`：随机置换初始化、空簇分裂、L2 早停）、`FindNearestCentroid(s)`、`ComputeCentroidScores`/`ComputeResiduals`、ColBERTSaR 无量化器、`Save/Load` | ✅ |
| `PlaidIndex`（`plaid_index.cppm`） | `infinity_index::plaid::PlaidIndex` | `Train`（含 ColBERTSaR query-aware SGD）、`AddMultipleDocsEmbeddings(WithCentroids)`、`SearchWithBitmask`（IVF 探测→近似分→`n_doc_to_score`→残差精排）、batched probe（`n_centroids > 4096`）、`ExactScore(Batch)`、`MergeChunks`/流式 merge/`ExpandCentroids`/`FindOutliers`、`Save/Load` 与 `CalcSize` | ✅（mmap 读取改为 owned，见设计变更） |
| `PlaidIndexInMem`（`plaid_index_in_mem.cppm`） | 执行器 `MultiVectorIndex` 的 `plaid`/`plaid_seqs`/`PlaidConfig` | `NewPlaidIndexInMem`→按 `WITH` 选项构造；`Insert`→tail 插入时增量 `add_multiple_docs_embeddings`；`BuildIndex(FromScratch/WithGlobalCentroids)`→`build_plaid`（`max(256, k*32)` 阈值、auto-k 降级、8 的倍数）；`Dump`→`write_cpp_plaid_index` | ✅ |
| `PlaidIndexDiskMerger` / `PlaidMergeManifest`（`plaid_index_disk_merger.cppm`） | `infinity_index::plaid::{PlaidIndexDiskMerger, PlaidMergeManifest}` | `MergeChunk(sParallel/Incremental)`、`Finalize`→`accept_merged_data`、`CalculateMergeSize`、manifest JSON（`NeedsMerge`/`UpdateChunk`/`Load/SaveManifest`） | ✅ |
| `SMVETransform` / `GenerateProjectionMatrix`（`smve_transform.cppm`） | `infinity_index::smve::{smve_transform, generate_projection_matrix, SmveResult}` | 随机单位列投影（固定种子）、逐 token 取 top-k、query 求和池化 / doc 非零均值池化 | ✅ |
| `SMVEIndexInMem` / `SMVESerializedData`（`smve_index.cppm`） | `infinity_index::smve::SmveIndex` + 执行器 `MultiVectorIndex` 的 `smve`/`smve_seqs`/`SmveConfig` | `BuildFromColumn`→`build_from_column`（内部 BMP 的 `AddDocs`）、`Search`→BMP `search_knn_filtered` + 执行器按列值精确 MaxSim 重排（与 C++ `PhysicalMatchTensorScan` 一致）、`GetSizeInBytes`/行数；`SMVEBMPCollector` 由 `search_knn` 的返回值承担 | ✅ |

> **基线说明**：`TableEntry` / `SegmentEntry` / `BlockManager` 已在基线 PR #2722 中从 C++ 源码删除；当前持久化元数据类是 `TableMeta` / `SegmentMeta` / `BlockMeta` / `ColumnMeta`，块映射类是 `BlockIndex`。本表按基线实际存在的类名对应，不再列出已删除的类。

## 有意的设计变更（§3.6 豁免）

| C++ | Rust | 原因 |
|---|---|---|
| `TaskScheduler` / `FragmentContext` / `BlockingQueue` | Tokio `mpsc` + `JoinSet` + 按边并发管线 | §4.4 D4（用户已批准） |
| 倒排 `fst` 打包/读取 | `fst` crate 数据路径 + 自实现 C++ 尾块 CRC-32/IEEE | §4.2（已批准；校验和不兼容，数据路径复用） |
| SIMD 距离核 / 压缩 / mmap | `simsimd`/`faer`、`lz4_flex`、`memmap2` | §3.4（仅底层原语，控制流仍自研） |
| 倒排 `PostingByteSlice` / `SkiplistReader` / `SkiplistWriter` | `inverted_format` 的 postings/positions 编解码；`fulltext_segment` 合并段 | 磁盘字节布局已对齐；skiplist 容器仅 C++ 构建期优化，读路径按同样的 postings 列表解码，类边界不单独移植 |
| `DataFileWorker` / `VirtualStore` / `PersistenceManager`（`storage/io`、`storage/buffer`） | `std::fs` + `infinity_storage::s3::S3Client` + WAL | 本地/对象存储的选择在 CLI 启动时完成（`storage_type=minio` 时先 restore 再按 S3 同步）；C++ 的文件 worker/虚拟存储层合并为这两条路径 |
| `TableIndexMeta` / `SegmentIndexMeta` / `ChunkIndexMeta`（`table_index_meta.cppm` 等） | `infinity_catalog` 的索引元数据 + `cpp_blocks` 的 `chunk_{n}.idx` 目录 | 持久化键空间与 chunk 目录布局已对齐；C++ 的模块类树未逐类拆分，索引生命周期由 `dump/optimize/cleanup` 方法承担 |
| `TableMeta` / `SegmentMeta`（`catalog/meta/*.cppm`）+ `BlockIndex` | `infinity-executor` 的 `TableEntry`（运行时聚合） | 上游 PR #2722 删除了 `TableEntry` / `SegmentEntry` / `BlockManager`；Rust 把它们过去承担的运行时状态（写入代次、行 ID 分配、索引注册表、块快照）聚合为一个进程内结构，持久化元数据仍由 `infinity_catalog` + `BlockIndex` 按 C++ 布局承担 |
| `PlaidIndex` 的 mmap 读取（`LoadFromPtr` / `centroid_ids` 指针模式） | `infinity_index::plaid::PlaidIndex` 的 owned `Vec<u32>` | mmap 属于已批准的原语豁免；执行器的 chunk 加载器总是解码为 owned 缓冲，语义（同一份 `SaveIndexInner` 字节布局）不变 |
| DiskANN / Vamana（`knn_diskann/*`：`VamanaAlg`、`PQFlashIndex`、`FixedChunkPQTable`、`DiskAnnMemGraphStore`、`NeighborPriorityQueue` 等） | 无 | 计划 §2.6/R14 明确不纳入：C++ 的 populate 分支只 `LOG_WARN("Not implemented yet")` 后返回成功，索引不可用；Rust 不移植不可达的骨架 |

## 尚未一一对应（待补，按优先级）

| C++ | 当前 Rust | 差距 |
|---|---|---|
| （无行为类缺口） | — | 承载行为、被其他类调用的 C++ 类均已在「已对应」表逐类映射；纯数据载体/模板特化/迭代器/生成类型见下方「类级覆盖审计」。新增上游行为类须先登记再移植 |

## 类级覆盖审计（基线 `5d27942c0`）

`src/**/*.cppm` 中 `export class/struct` 共 **1136** 个；本表显式命名 **83** 个。其余 1053 个按 §3.6 允许的「结构合并」处理（模板特化 / POD 状态 / 迭代器机件 / 生成的 Thrift 类型），不是静默省略。**逐名清单（1136 行，mapped 83 / grouped 1031 / excluded 15 / test 7）见 [`CLASS_AUDIT.md`](CLASS_AUDIT.md)**；下表是按区域汇总（计数以加入 PLAID/SMVE 映射前的快照统计）：

| 区域 | 未逐类命名 | Rust 承担方式 |
|---|---:|---|
| `storage/*` | 553 | `invertedindex` 117：posting/position 解码与 `And/Or/Phrase` 迭代器 → `inverted_format`/`fulltext_segment`/扫描求值函数；`knn_index` 110：模板特化（`Plain*/LVQ*/Rabitq*Dist`、`*VecStoreType`、`*Cache`）→ `vector::Store` 枚举 + 距离函数，`SparseVec*` → `Vec<(u32,f32)>` 别名，`BMPHandler`/`BMPIndexInMem` → 执行器 `SparseIndex`，DiskANN 15 个按 §2.6/R14 排除，`knn_flat` 11 个为距离原语；`catalog` 76 + `new_txn` 46 + `wal` 40 → `infinity_catalog`/`value_format` + `Txn` 操作 + 字节级 `wal`；`common` 40/`buffer` 16/`io` 16/`column_vector` 15/`definition` 12 → `Vec<u8>`/`BufferObj`/`std::fs`/S3 等原语（`ColumnVector`/`DataBlock` 已单独命名）；`bg_task` 26 → 定时线程 + `Engine::{dump,optimize,cleanup}` |
| `executor/*` | 173 | `operator` 78 + `operator_state.cppm` 74 → `PhysicalOperator` 变体与 `OperatorState`；`expression` 13 → `infinity-expression` 求值函数；其余为执行器内部管线结构 |
| `planner/*` | 117 | `node` 52 → `Statement`/计划结构；`optimizer` 17 → 规则函数；其余为绑定/解析中间结构 |
| `function/*` | 64 | `cast` 29 + `table` 17 + 其余 → Rust 的 cast/builtin 函数与 `AggFunc` 枚举 |
| `common/*` | 58 | `analyzer` 39 → `tokenize_*` analyzer 函数；其余为工具结构 |
| `main/*` + `network/*` + `scheduler/*` + `admin` + `metrics` + `unit_test` | 39 + 23 + 10 + 1 + 1 + 7 | 查询选项 → 解析结构；Thrift 类型 → 生成代码；调度器按 D4 由 Tokio 取代；admin → 语句分发；metrics → 服务端计数；`unit_test` 为 C++ 测试夹具 |
| `expression/*` | 12 | 标量表达式求值函数（`infinity-expression`） |

> 判定口径：C++ 中承载**行为**且被其他类调用的类（HNSW/IVF/EMVB/PLAID/SMVE、全文段、事务、Buffer、BlockIndex、PDataBlock 等）在本表中逐类对应；纯数据载体、模板特化、迭代器和生成类型按上表合并，理由与归属均已登记。新增 C++ 行为类时仍须先登记再移植。

## 维护规则

- 新增一个 C++ 子系统的移植时，先在本文档登记类/方法映射，再写代码。
- 局部合并/拆分是允许的，但必须写清动机与影响，不能以「测试能过」为唯一理由。
- 每个 Rust `pub fn` 的文档注释写 `C++: path:line`。
- 任何“简化”必须出现在「有意的设计变更」或「尚未一一对应」表里，不能默默省略。
- Gate A 通过不等于移植完成：状态以本表 + 计划书 §7.3 为准。

## C++ 单元测试对应

规则：`src/unit_test` 中针对某 C++ 类的用例，若 Rust 已有对应类/方法，**必须**移植同等场景与断言并纳入 `cargo test`；不能移植的登记到「未移植单测」。

### 已移植

| C++ 用例 | Rust 类/方法 | Rust 测试 |
|---|---|---|
| `txn/conflict_check_ut.cpp::conflict_check_delete` | `TxnManager::commit` + `Txn::delete` | `infinity_txn::tests::concurrent_deletes_conflict`、`tests::delete_conflicts_follow_the_reference` |
| `buffer/buffer_manager_ut.cpp`（容量/淘汰） | `buffer::BufferManager` | `buffer::tests::evicts_least_recently_used`、`shrinking_the_budget_evicts` |
| `buffer/buffer_handle_ut.cpp`（预算/rc/溢出） | `buffer::{BufferPool, BufferObj, BufferHandle}` | `buffer_handles_follow_the_reference_lifecycle`（3×512B/1KB：第三次 Load 失败、rc、New、ephemeral 溢出后读回、Save）、`persistent_buffers_outlive_their_handles` |
| `invertedindex/format/*`（posting/position 编解码） | `inverted_format` | 该模块的往返/边界单测 |
| `secondary_index` 的 PGM 查找 | `SecondarySegmentReader::lookup` | `secondary_segment::tests::pgm_lookup_matches_binary_search` |
| `std::hash<string_view>`（VARCHAR 次级键） | `secondary_segment::hash_varchar` | `varchar_hash_matches_libstdcxx` |
| （无独立 C++ 单测） | `merge_knn::MergeKnn` | 6 例：多段合并/并列取小 RowID/阈值方向/多查询/EndWithoutSort/余弦排序 |
| （无独立 C++ 单测） | `knn_scan::PhysicalKnnScan::finish` | `finish_ranks_by_metric_and_applies_threshold_and_topn`（相似度降序/距离升序/并列取小 RowID/阈值/topn） |
| （无独立 C++ 单测） | `full_rows_come_from_the_block_mirror_when_current` / `block_mirror_covers_rows_after_deletes_and_updates` / `projection_reads_columns_from_the_block_mirror` | 毒化 KV 行值后全行/投影扫描仍返回块中的值；delete/update 后块快照 + tombstone + 行日志覆盖，不再等 checkpoint |
| （无独立 C++ 单测） | `block_index::{create_block, get_block, open, remove_block}` | `creates_and_reads_blocks`、`reads_go_through_the_bounded_cache`、`open_recovers_block_ranges_from_version_files`、`removing_a_block_drops_it_from_the_manager` |
| `knn/sparse/test_bmp_fwd_ut.cpp`（块分组/迭代不变量） | `sparse::BlockFwd`/`TailFwd` | `block_fwd_groups_docs_into_blocks` |
| `knn/sparse/test_bmp_ivt_ut.cpp`（block max / kth 不变量） | `sparse::BMPIvt`/`Postings` | `bmp_ivt_keeps_block_maxima_and_kth` |
| `knn/sparse` 的 block-max 检索 | `sparse::BmpAlg::search_knn` | `matches_brute_force`、`beta_keeps_the_strongest_terms`、`tail_documents_are_scored_when_the_block_does_not_fill` |
| `knn/hnsw` 的插入/检索入口 | `hnsw_handler::HnswHandler` | `insert_vecs_counts_only_matching_dimensions`、`search_index_honours_the_ef_option` |
| `invertedindex/column_inverter_ut.cpp`（tf/positions/length） | `MemoryIndexer` | `memory_indexer_records_tf_positions_and_length`、`memory_indexer_delete_keeps_the_postings` |
| `knn/emvb`（候选/精排） | `emvb::EmvbView` / `EmvbPqIndex::prefilter` | `ranks_the_document_whose_centroid_matches_the_query`、`threshold_first_falls_back_to_the_closest_centroid`、`prefilter_keeps_the_exact_maxsim_winner` |
| `planner/optimizer/regex_literal_extractor_ut.cpp` | `regex_literal::extract_regex_literal_runs` | `extracts_only_mandatory_runs`（备选/量词/内联标志/`\Q..\E`/`\C`/fold）、`sparsegram_analyzer_names`、`gram_floor_follows_the_script` |
| `knn/hnsw/test_lvq_ut.cpp` | `lvq::LvqStore` | `reconstruction_error_is_below_two_percent`（2% 重建误差）、`distances_track_the_plain_metric`、`save_and_load_roundtrip` |
| `knn/hnsw/test_rabitq_ut.cpp` | `rabitq::RabitqStore` | `orthogonal_matrix_is_orthonormal`、`centroid_is_the_rotated_mean`、`save_and_load_roundtrip`、`l2_estimator_keeps_the_true_neighbour` |
| `knn/hnsw/test_lsg_build_ut.cpp` | `lsg::LsgIndex` + `HnswIndex` | `avg_is_at_least_the_exhaustive_topk_mean`、`lsg_hnsw_keeps_self_query_recall`（recall@1 ≥ 0.95） |
| `bg_task/*_ut.cpp` 的索引生命周期 | `dump_hnsw_segments` / `optimize_indexes` / `cleanup` | `hnsw_checkpoint_dumps_only_the_memory_segment`、`mem_index_capacity_dumps_the_hnsw_segment_after_the_batch`、`cleanup_keeps_incremental_hnsw_chunks_until_a_merge_covers_the_table` |
| `knn/plaid/test_plaid_ut.cpp::PlaidQuantizerTest` | `plaid::PlaidQuantizer` | `train_quantize_dequantize`、`two_bit_quantization`、`ip_table_and_position_lut_agree`、`save_and_load_roundtrip` |
| `PlaidGlobalCentroids`（由 `PlaidIndexTest` 覆盖） | `plaid::PlaidGlobalCentroids` | `trains_centroids_and_assigns_nearest`、`residuals_reconstruct_the_embedding`、`save_and_load_roundtrip`、`colbertsar_mode_has_no_quantizer` |
| `PlaidIndexTest` / `PlaidIndexColBERTSaRTest` | `plaid::PlaidIndex` | `basic_construction`、`train_and_add`、`search_ranks_the_queried_document_first`、`bitmask_filters_candidates`、`save_and_load_roundtrip`、`mem_usage_matches_calc_size`、`move_moves_the_index`、`colbertsar_mode_trains_and_searches_without_residuals`、`merge_keeps_documents_and_scores` |
| `PlaidIndexDiskMerger` / manifest（无独立 C++ 单测） | `plaid::{PlaidIndexDiskMerger, PlaidMergeManifest}` | `streams_chunks_into_the_output_index`、`manifest_roundtrips_through_json`、`incremental_merge_skips_unchanged_chunks` |
| （执行器集成，无独立 C++ 单测） | `MultiVectorIndex::build_plaid` / `write_cpp_plaid_index` | `plaid_index_answers_tensor_search_and_mirrors_a_chunk`（PLAID 建索引、扫描仍与穷举 top-1 一致、flush 产出可解码的 C++ PLAID chunk） |
| `SMVEIndexInMem` / `SMVETransform`（无独立 C++ 单测） | `smve::{SmveIndex, smve_transform}` | `projection_columns_are_unit_vectors`、`query_codes_are_sum_pooled_and_docs_averaged`、`search_returns_the_closest_document_and_honours_the_filter` |
| （执行器集成，无独立 C++ 单测） | `MultiVectorIndex::build_smve` + `exact_tensor_score` | `smve_index_answers_tensor_search`（SMVE 建索引、候选按列值精确重排、top-1 与穷举一致） |

### 未移植单测（Rust 类已存在）

| C++ 用例 | 对应 Rust | 缺口 |
|---|---|---|
| `buffer/buffer_obj_ut.cpp` | `BufferObj`/`BufferPool` + 索引镜像 | 核心生命周期（Load/Save/Free/rc/溢出）已由 `buffer_handle_ut.cpp` 用例覆盖；其余用例绑定 HNSW buffer obj 与事务导入，随索引集成测试覆盖 |
| `invertedindex/format/*` 其余部分 | `inverted_format` / `fulltext_segment` | posting/positions/term-meta 编解码已覆盖；`PostingByteSlice`/`Skiplist*` 为构建期容器，已登记为设计变更 |
| （无独立 WAND/BM25 单测） | `wand::BlockMaxWandIterator` | 已用 `ranker_matches_the_reference_formula`、`wand_matches_brute_force` 覆盖 |
| `invertedindex/search/*`（短语/布尔组合） | `wand` 类 + 扫描路径 | WAND/BM25 已拆类；短语与布尔组合仍走扫描（由 SQL 测试覆盖） |
| `secondary_index/*_ut.cpp` | `secondary_segment`/`secondary_lowcard` | PGM 查找/重复键/边界、VARCHAR 哈希、低基数 Roaring 往返已覆盖；跨事务集成由 SQL 测试覆盖 |
| `knn/hnsw/*_ut.cpp` | `infinity_index::vector::HnswIndex` | plain 图的插入/检索/ef 已覆盖；`LSG`/`LVQ`/`RaBitQ` 变体不进入 SQL 面（已登记设计变更） |
| `knn/sparse/*_ut.cpp` | `infinity_index::sparse` / `bmp_cpp` | 块/ivt 不变量与序列化往返已覆盖；`alpha` 剪枝边界用例未逐个移植 |
| `knn/emvb/*_ut.cpp` | `emvb::EmvbView` / `MultiVectorIndex` | 候选/精排、阈值回退、PQ 预过滤已覆盖；`test_fakepq` 的 OPQ 训练由写入路径测试覆盖 |
| `bg_task/*_ut.cpp` | 执行器/CLI 的 dump/optimize/cleanup | 索引生命周期已覆盖；任务调度/重试场景未移植（调度器为 D4 豁免） |

### 未移植单测（Rust 类也未移植）

`column_vector/*`、`definition/data_block_ut.cpp`、`common/loser_tree_ut.cpp`、`common/test_rcu_multimap_ut.cpp` 等属于尚未移植的 C++ 类（`ColumnVector`/`DataBlock`/`LoserTree`/RCU 容器），随对应类一起移植。
