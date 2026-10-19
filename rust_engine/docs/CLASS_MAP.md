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
| `ColumnDef::ToString` / `ConstrainTypeToString` / `LogicalType2Str`（`parser/definition/column_def.cpp:213`、`parser/type/logical_type.cpp:28`） | `ColumnDefValue` 的 `Display` | 约束按 `std::set<ConstraintType>` 顺序、缺省默认打印 `Null`、`DataType` 的 C++ 拼写（`TinyInt`/`Varchar`/`Decimal(p,s)`…） | ✅ |
| `KeyEncode`（`storage/common/kv_code.cppm`） | `infinity_storage::catalog_format` 的 `catalog|…` 与名称键两族函数 | `CatalogDbKey`/`CatalogDbPrefix`/`CatalogDbTagPrefix`/`CatalogTableKey`/`TableColumnKey`/`CatalogTableSegmentKey` 与 `DatabaseKey`/`TableKey`/`TableSegmentKey`/`TableSegmentBlockKey`/`TableSegmentBlockColumnKey`/`TableIndexKey`/`TableIndexSegmentKey`/`TableIndexChunkKey` 及各自 `*Prefix` | ✅ |
| `CachedNodeBase` / `CachedScanBase` / `CacheContent` / `CacheOutput` / `ResultCacheManager`（`cached_node/*.cppm`、`logical_read_cache.cppm`、`result_cache_manager.cppm`） | `infinity_storage::result_cache::{CachedNode, CacheContent, CacheOutput, CacheResultMap, ResultCacheManager}` | `AddCache`→`add_cache`（已有条目合并新列）、`GetCache`→`get_cache`（按输出列投影，缺列即 miss）、`DropTable`→`drop_table`、`ResetCacheNumCapacity`/`ClearCache`、LRU 淘汰 | ✅ |
| `KnnFlatL2` / `KnnFlatIP` / `*Blas` / `*Reservoir` / `*Top1`（`knn_flat/*.cppm`） | `infinity_index::knn_flat::{top_k, top_1}` | `Search`→`top_k`（L2 升序 / IP 相似度降序、bitmask 过滤、并列取小 id）、`*Top1`→`top_1`；BLAS/reservoir 变体合并为同一实现（结构合并，行为一致） | ✅ |
| `NGramAnalyzer`（`common/analyzer/ngram_analyzer.cppm`） | `infinity_executor::analyzer::NGramAnalyzer` | `Analyze`→`analyze`（按 code point 滑动窗口，窗口内空格结束当前词，尾部不足 n 停止） | ✅ |
| `StandardAnalyzer` / `CommonLanguageAnalyzer` / `Tokenizer` / `TokenizeConfig` / `Term` / `TermList`（`common/analyzer/*.cppm`） | `infinity_executor::analyzer::{StandardAnalyzer, TokenizeConfig, Term, tokenize}` | `InitStemmer(ENGLISH)`、`SetCaseSensitive`、`SetExtractEngStem`、`SetStemOnly`、`SetCharOffset`、`SetTokenizerConfig`、`Analyze`→`analyze`（word/stem/case 的三种输出组合、stem 与原词共享 position） | ✅ |
| `MixedT`（`parser/type/mixed_type.h`） | `infinity_common::types::{MixedT, MixedValue}` | `MakeInteger/MakeFloat/MakeString/MakeTuple(count)/MakeArray(count)/MakeNull/MakeMiss`、`Insert*IntoTuple/IntoArray`（重复键/越界/满容量报错）、`GetFromTuple/GetByIndex`、`type`→`value_type`、`Reset`、值访问与渲染；14 字节短串内联阈值；C++ 的 union 内存布局与移动后 `kInvalid` 由 Rust 所有权模型取代 |
| `BaseMixedType` / `MixedValueType`（`parser/type/base_mixed_type.h`） | `infinity_common::types::MixedValueType` | `GetTypeName`→`type_name`（Invalid/Integer/Float/Short|Long String/Tuple/Array/Null/Missing/Dummy） | ✅ |
| `TableDef`（`storage/definition/table_def.cppm`） | `infinity_catalog::value_format::TableDef` | `Make`→`make`、`column_count`、`GetColIdByName`→`get_col_id_by_name`；字段（schema/table/comment/columns）与 `ColumnDefValue` 一致 | ✅ |
| `Selection`（`column_vector/selection.cppm`）/ `VarBuffer`（`buffer/buffer_obj.cppm`） | `infinity_storage::selection::Selection` / `infinity_storage::var_buffer::VarBuffer` | `Initialize/Set/Append/Get/Capacity/Size/Reset`；`Append/Get/Write/TotalSize/GetSize`（分块存储 + 前缀和，零长度行返回空） | ✅ |
| `BlockVersion` / `CreateField`（`catalog/meta/block_version.cppm`） | `infinity_storage::mvcc::{BlockVersion, CreateField}` | `Append`→`append`（累计行数）、`CommitAppend`→`commit_append`、`Delete`→`delete`（重复删除报 `TxnWWConflict`）、`RollbackDelete`、`CheckDelete`、`GetCreateTS`/`GetDeleteTS`、`SpillToFile`/`LoadFromFile`（复用 `BlockVersionFile` 字节布局） | ✅ |
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
| `Utility`（`common/utility/utility.cppm`）/ `UnrecoverableException` / `RecoverableException` | `infinity_common::{trim_path, extract_u64_from_string_suffix, Error}` | `TrimPath`→`trim_path`、`ExtractU64FromStringSuffix`→`extract_u64_from_string_suffix`、`Error::Unrecoverable`（裸消息）/`Error::Unexpected`（`Unexpected error: …`） | ✅ |
| `Config` / `DefaultConfig`（`main/config.cppm`） | `infinity_cli::config::Config` | `Init`→`init`（默认值 + TOML 覆盖）、`Set`→`set`、`Validate`→`validate`（版本/时区/IP/字节大小/日志级别/时间信息错误分类）、全部访问器；错误码 `InvalidConfig`/`MismatchVersion`/`InvalidTimezone`/`InvalidIpAddr`/`InvalidByteSize`/`InvalidLogLevel`/`InvalidTimeInfo` | ✅ |
| `QueryProfiler` / `BaseProfiler` / `OptimizerProfiler` / `TaskProfiler` / `ProfileHistory`（`main/profiler.cppm`） | `infinity_executor::profiler::{QueryProfiler, BaseProfiler, OptimizerProfiler, TaskProfiler, TaskBinding, OperatorInformation, ProfileHistory}` | `StartPhase`→`start_phase`、`StopPhase`→`stop_phase`、`Stop`→`stop`、`Flush`→`flush`、`GetTaskProfile`→`task_profile`、`QueryPhaseToString`→`query_phase_to_string`（`kInvalid` 报错）、`StartRule`/`StopRule`、`Resize`/`Enqueue`/`GetElement`/`GetElements`、`ElapsedToString` 单位阈值 | ✅ |
| `GlobalVariable` / `SessionVariable` / `VarUtil`（`main/variables.cppm`） | `infinity_executor::variables::*` + `Engine::show_variable` | `global_name_map_`/`session_name_map_` 的全部名字；未知名字报 `NoSysVar`；`SHOW VARIABLE` 按 `ExecuteShowGlobalVariable`/`ExecuteShowSessionVariable` 的取值分支返回 | ✅ |
| `TxnState` / `TxnState2Str` / `TransactionType` / `TransactionType2Str`（`storage/new_txn/txn_state.cppm`） | `infinity_txn::state::{TxnState, TransactionType, txn_state_to_str, transaction_type_to_str}` | 状态与类型的全部枚举值与参考字符串（`Started`/`Not Started`/`CreateDB`/`optimize_index`…） | ✅ |
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
| `RcuMap<Key, Value>`（`storage/common/rcu_multimap.cppm`，单值变体） | `infinity_storage::rcu_map::RcuMap` | `Get/GetValue/GetWithRcuTime/GetReadOnly`、`Insert`（替换语义）、`Delete`、`CheckGc`/`CheckExpired`、`GetAllValuesWithRef`/`GetAllKeyValuePairs`、`size`/`emplace`/`range`/`GetOrAdd`/`Clear`；`CheckSwapInLock` 的 100 条/3×miss 保守阈值一致 | ✅（全部 19 例已移植） |
| `RcuMultiMap<Key, Value>` / `MapValue` / `DeletedMap`（`storage/common/rcu_multimap.cppm`） | `infinity_storage::rcu_map::RcuMultiMap` | `Get/GetWithRcuTime`、`Insert`、`Delete(key)`/`Delete(key,value)`、`CheckGc`、`CheckExpired`、`GetAllValuesWithRef`/`GetAllKeyValuePairs`/`GetMergedMultiMap`、`size`/`emplace`/`range`；`CheckSwapInLock` 的快照发布语义一致 | ✅（全部 18 例已移植） |
| `BPReordering<IdxType, DocID>` / `BPReorderContext`（`bp_reordering.cppm`） | `infinity_index::bp::BpReordering` | `AddDoc`→`add_doc`、`operator()`→`reorder`（递归二分、`ComputeGains`/`ComputeTermGain`/`ComputeTermCost`；参考实现的线程并行只影响耗时，结果一致） | ✅ |
| `LinScan<DataType, IdxType>`（`linscan_alg.cppm`） | `infinity_index::linscan::LinScan` | `Insert`→`insert`、`SearchBF`→`search_bf`、`SearchKnn`→`search_knn`（按 |value| 排序读 postings 至 budget，保留 top-k 候选）、`Save/Load`→`to_bytes`/`from_bytes`（含重复 doc_id/term 校验） | ✅ |
| `BMPAlg` / `BMPAlgBase::SearchKnn`（`bmp_alg.cppm`，own-mem） | `infinity_index::sparse::{BmpAlg, BmpSearchOptions}` | `AddDoc`→`add_doc`、`SearchKnn`→`search_knn`、`GetScores`/`Calculate2`、`alpha/beta/use_tail` | ✅ |
| `BMPIvt`/`BlockFwd` 的 `BMPOwnMem::kFalse`（mmap） | `infinity_storage::bmp_cpp::BmpCppReader` | 读取同一份 `SaveToPtr` 字节布局并查询 | ✅（设计变更：读路径共用磁盘格式） |
| `KnnScanSharedData` / `KnnSearchOption`（`knn_scan_data.cppm`） | `knn_scan::KnnScanOptions` | `nprobe`/`ef`/`threshold` 从 `WITH (…)` 与 legacy 串解析（`WITH` 优先），供 IVF/HNSW/阈值共用 | ✅ |
| `PhysicalKnnScan`（`physical_knn_scan.cppm` / `physical_knn_scan_impl.cpp`） | `knn_scan::{PhysicalKnnScan, KnnScanOperatorState}` + `Engine::knn_candidates` | `GetColumnID`→`column_index`、`IsKnnMinHeap`→`is_knn_min_heap`、`Execute`→`execute`（逐行 + 行缓存的精确评分）、`PlanWithIndex` 的索引探测→`KnnCandidateScan`/`vector_candidates`、排序/阈值/截断→`finish`（`MergeKnn` 方向语义）；bitmask 过滤由过滤谓词承担，多查询由上层逐次调用 | ✅（多 block 并行由行管线代替，见 D4） |
| `TableMeta` / `SegmentMeta` / `BlockMeta` / `ColumnMeta`（`catalog/meta/*.cppm`） | `infinity_catalog::{TableMeta}` + `infinity_storage::block_index::BlockMeta` + `cpp_layout` | 表/段/块/列的元数据按 C++ catalog 布局持久化；`BlockMeta` 提供 `block_dir`/`row_offset`/`row_count`/`GetRowCnt1` 语义，`infinity_catalog` 提供表与列定义 | ✅ |
| `BlockIndex`（`storage/common/block_index.cppm`） | `infinity_storage::block_index::BlockIndex` | `NewInit/LoadSet`→`open`（扫描 `version` 文件恢复块与行区间）、`BlockCount`/`SegmentCount`、`GetBlockMeta`、`GetSegmentOffset`/`GetBlockOffset`、`IsEmpty`；块列解码经共享 `BufferManager` LRU；checkpoint 调 `create_block` 落块，块读与全行读（`try_cpp_rows`）都走它 | ✅ |
| `Storage`（`storage.cppm`） | 执行器 `Engine::storage: Storage` + `Engine` 的 `txn`/`dir`/`context` | `new_catalog`/`kv_store`/`new_txn_manager`/`buffer_manager`/`wal_manager`/`object_storage_processor` 分别由 `Storage` 与 `Engine` 组合；后台处理器由 CLI 定时线程 + Tokio 取代（D4） | ✅ |
| `InfinityContext`（`infinity_context.cppm`） | 执行器 `Context` | `config`/`BufferManager`/`memindex_memory_quota`/`scan_gate`/`object_store`/exchange 容量；`Engine` 组合而非内联；C++ 线程池由 Tokio 取代（D4） | ✅ |
| `ColumnVector<T>` / `DataBlock`（`column_vector.cppm`、`data_block.cppm`） | `infinity_storage::data_block::{ColumnVector, Column, DataBlock, BlockValue}` | `Initialize/SetVectorType`→`initialize`/`set_vector_type`（含 constant/selection/slice）、`AppendValue/AppendWith`→`append_value`/`push`/`extend_value`、`GetValueByIndex`/`SetValueByIndex`→`get`/`set_value`、`Size/capacity`、`Finalize`、`Reset`、`ShallowCopy`、null bitmap→`validity`、`DataBlock::Make/AppendRow/AppendValue/Finalize/GetValue/Reset`→`new`/`push_slot`/`append_value`/`finalize`/`get_value`/`reset`（`BlockValue` 覆盖 bool/各整数宽度/float/double/decimal/varchar/date-time/interval/array/uuid/embedding/mixed）、`MemUsed`→`mem_used`；`VectorBuffer`/`Bitmask` 未复刻（Rust `Vec`） | ✅（integer 20 + date/time 20 + `data_block_ut` 2 例已移植；geo 列按用户决定排除） |
| `FixedDimensionalEncodingConfig` / `Tensor` / `RandomGenerator`（`fixed_dimensional_encoding.cppm`） | `infinity_expression::fde::*` | 配置默认值/枚举、`Tensor` 点云访问、确定性随机（Gaussian/Bernoulli/Uniform） | ✅ |
| `HashPartitioner` / `SparseProjector` / `EncodingAccumulator` / `EncodingPipeline`（同上） | `fde::{HashPartitioner, SparseProjector, EncodingAccumulator, EncodingPipeline}` | SimHash 分区（Gray 码）、AMS/Count-Sketch 稀疏投影、sum/average 聚合、空分区填充、最终投影；`Generate*FixedDimensionalEncoding` 与 `internal::*` | ✅（`FDEFunction`/`RegisterFDEFunction` 的 SQL 绑定待执行器接入） |
| `LoserTreeBase` / `LoserTree`（`invertedindex/common/loser_tree.cppm`） | `infinity_common::loser_tree::LoserTree` | `InsertStart`→`insert_start`、`InitWinner`/`Init`→`init_winner`/`init`、`TopSource`/`TopKey`、`DeleteTopInsert`→`delete_top_insert`、`round_up_to_power_of_two`；sentinel（`sup`/`invalid_`）语义一致 | ✅ |
| `RegexLiteralRun` / `ExtractRegexLiteralRuns`（`planner/optimizer/regex_literal_extractor.h`） | `regex_literal::{extract_regex_literal_runs, has_any_sparse_gram, parse_sparsegram_analyzer}` | 只抽取每次匹配都必含的字面 run；`\Q…\E`/`\C`/内联 `(?i:)` 标志、量词回退、备选分支丢弃、ASCII fold 切分、`sparsegram-<min>-<max>[-fold]` 解析与 gram 下限 | ✅ |
| `DumpMemIndexTask` / `OptimizeIndexTask` / `CleanupIndexTask`（`bg_task/`） | `Engine::{dump_hnsw_segments, optimize_indexes, cleanup}` | dump 按 `mem_index_capacity`/字节配额写增量 chunk；optimize 在可承受的内存内重建全表 merged chunk；cleanup 只删除被 merged chunk 覆盖的旧 chunk（调度由 CLI 定时线程代替，见 D4） | ✅（任务对象由定时线程取代） |
| `PlaidQuantizer`（`plaid_quantizer.cppm`） | `infinity_index::plaid::PlaidQuantizer` | `Train`（全局均值 + 分位切断点/权重，超限按 `MAX_RESIDUAL_VALUES` 蓄水池采样）、`Quantize/Dequantize`（2/4-bit MSB-first 打包、反位映射、L2 归一）、`GetIPDistanceTable`/`BuildPositionLUT`/`GetSingleIPDistance*`、`Save/Load` 字节布局 | ✅ |
| `PlaidGlobalCentroids`（`plaid_global_centroids.cppm`） | `infinity_index::plaid::PlaidGlobalCentroids` + `kmeans_l2` | `Train`（k*256 采样 + `GetKMeansCentroids`：随机置换初始化、空簇分裂、L2 早停）、`FindNearestCentroid(s)`、`ComputeCentroidScores`/`ComputeResiduals`、ColBERTSaR 无量化器、`Save/Load` | ✅ |
| `PlaidIndex`（`plaid_index.cppm`） | `infinity_index::plaid::PlaidIndex` | `Train`（含 ColBERTSaR query-aware SGD）、`AddMultipleDocsEmbeddings(WithCentroids)`、`SearchWithBitmask`（IVF 探测→近似分→`n_doc_to_score`→残差精排）、batched probe（`n_centroids > 4096`）、`ExactScore(Batch)`、`MergeChunks`/流式 merge/`ExpandCentroids`/`FindOutliers`、`Save/Load` 与 `CalcSize` | ✅（mmap 读取改为 owned，见设计变更） |
| `PlaidIndexInMem`（`plaid_index_in_mem.cppm`） | 执行器 `MultiVectorIndex` 的 `plaid`/`plaid_seqs`/`PlaidConfig` | `NewPlaidIndexInMem`→按 `WITH` 选项构造；`Insert`→tail 插入时增量 `add_multiple_docs_embeddings`；`BuildIndex(FromScratch/WithGlobalCentroids)`→`build_plaid`（`max(256, k*32)` 阈值、auto-k 降级、8 的倍数）；`Dump`→`write_cpp_plaid_index` | ✅ |
| `PlaidIndexDiskMerger` / `PlaidMergeManifest`（`plaid_index_disk_merger.cppm`） | `infinity_index::plaid::{PlaidIndexDiskMerger, PlaidMergeManifest}` | `MergeChunk(sParallel/Incremental)`、`Finalize`→`accept_merged_data`、`CalculateMergeSize`、manifest JSON（`NeedsMerge`/`UpdateChunk`/`Load/SaveManifest`） | ✅ |
| `SMVETransform` / `GenerateProjectionMatrix`（`smve_transform.cppm`） | `infinity_index::smve::{smve_transform, generate_projection_matrix, SmveResult}` | 随机单位列投影（固定种子）、逐 token 取 top-k、query 求和池化 / doc 非零均值池化 | ✅ |
| `SMVEIndexInMem` / `SMVESerializedData`（`smve_index.cppm`） | `infinity_index::smve::SmveIndex` + 执行器 `MultiVectorIndex` 的 `smve`/`smve_seqs`/`SmveConfig` | `BuildFromColumn`→`build_from_column`（内部 BMP 的 `AddDocs`）、`Search`→BMP `search_knn_filtered` + 执行器按列值精确 MaxSim 重排（与 C++ `PhysicalMatchTensorScan` 一致）、`GetSizeInBytes`/行数；`SMVEBMPCollector` 由 `search_knn` 的返回值承担 | ✅ |
| `AnalyzerPool` / `RAGAnalyzer` / `ChineseAnalyzer` / `IKAnalyzer` / `DartsTrie`（`common/analyzer/*`） | `infinity_executor::analyzer::{AnalyzerPool, RAGAnalyzer, ChineseAnalyzer, IKAnalyzer, DartsTrie}` | 名称解析（`rag-<lang>[-fine]`）、trie 驱动分词、按语言折叠/停用词/词干、jieba MP-DAG+HMM 粗/细粒度、IK 词典最长匹配；`Tokenize`/`Analyze`→`tokenize`/`analyze` | ✅（OpenCC 为等价折叠，IK 歧义仲裁未移植） |
| `QueryBuilder` / `TermLookup` / `AndIterator` / `OrIterator` / `AndNotIterator`（`invertedindex/search/query_builder*`） | `infinity_executor::query_builder::{QueryBuilder, TermLookup, AndIterator, OrIterator, AndNotIterator, PhraseQueryNode, BmwIterator}` | 布尔树扁平化、`NOT`→`AND_NOT`、`Next(doc_id)`/`DocID()`、短语（精确/滑窗）与 BMW/WAND | ✅ |
| `SearchDriver` / 查询解析器（`invertedindex/search/search_driver*`、`search_lexer.l`、`search_parser.y`） | `infinity_executor::fulltext_query::{SearchDriver, MemoryColumnIndex, MemorySegment}` | 词法（引号、`~slop`、`^boost`、转义）、隐式 OR、`AND`/`NOT`、`field:term`、分析器解析 | ✅ `^boost` 进入查询树并参与 BM25 打分；无列名字段组走与具名列相同的 BM25 路径 |
| `Highlighter` / `PostingWriter` / `PostingMerger` / `PostingIterator`（`invertedindex/*`） | `infinity_executor::highlight_with_analyzer`、`infinity_storage::fulltext_segment::{PostingWriter, PostingMerger, PostingIterator}` | postings 写入/合并/`seek_doc`/`skip_to`、tf、高亮片段 | ✅ |
| `SortMerger` / `TermTuple`（`invertedindex/common/external_sort*`） | `infinity_storage::external_sort::{SortMerger, SortKey, TermTuple}` | 单趟 k 路归并、run 帧格式与键序 | ✅ |
| `PersistenceManager` / `PersistResultHandler`（`storage/persistence/*`） | `infinity_storage::persistence::{PersistenceManager, PersistResultHandler}` | 128 字节对象对齐、独立/组合对象、compose footer、引用计数、区间合并清理 | ✅ |
| `MemIndexTracer`（`storage/invertedindex/memory_indexer*`） | `infinity_storage::memindex_tracer::MemIndexTracer` | 配额/提案、dump 选择 | ✅ |
| `BinaryFuseFilter`（`storage/binary_fuse_filter/*`） | `infinity_storage::binary_fuse_filter::BinaryFuse` | arity-3 fuse-8 allocate/populate/contain | ✅ |
| `DataTable` / `DataBlock` 行式视图（`storage/definition/table*`） | `infinity_executor::data_table::{DataTable, TableType}` + `data_block::fill_row_id_vector` | 行式读取、布尔 compact 位、`RowID(segment, offset)` | ✅ |
| `ByteSliceWriter/Reader` / `FileWriter/Reader` / `StreamReader` / `VirtualStore`（`storage/io/*`） | `infinity_storage::io::{ByteSliceWriter, ByteSliceReader, FileWriter, FileReader, StreamReader}` + `infinity_storage::s3::S3Client` | 尺寸跟踪、vint/vlong、mmap 读写、行读取；对象存储仍走 S3 | ✅（对象存储路径见设计变更） |
| `AddFunction` 及全部标量注册（`function/scalar/*`） | `infinity_expression::scalar::{ScalarFunction, ScalarFunctionSet, TypedColumn}` | 24 个 `+(T,T)->T` 重载、按列宽 checked 求值（溢出→NULL） | ✅ |
| JSON 值（`parser/type/*`、`storage/column_vector/json*`） | `infinity_common::json::Json` + `LogicalType::Json`/`BlockValue::Json` | 解析树、紧凑序列化、列/WAL 编解码 | ✅（以结构化 JSON 替代 BSON 字节） |
| 多语句事务（`storage/txn/*`） | `infinity_executor::EngineTxn` + `infinity_txn::TxnManager` | `begin_txn`/`append`/`delete_row_ids`/`create_table_snapshot`/`commit`/`rollback`、begin/commit 时间戳 | ✅（rollback 不回退行号分配器） |
| 稀疏 HNSW（`KnnHnsw<SparseIPVecStoreType>`，`knn_hnsw/*`） | `infinity_index::vector::{Store::SparseIp, HnswIndex::sparse_ip/add_sparse/search_sparse/save_sparse/load_sparse}` | 稀疏点积距离、图构建/检索/序列化 | ✅（不含 BMP 块压缩） |
| `OptimizerRule` / `Optimizer` / `ColumnPruner` / `ColumnRemapper` / `LazyLoad` / `ApplyFastRoughFilter` / `ResultCacheGetter`（`planner/optimizer/*`、`optimizer_rule.cppm`） | `infinity_planner::{optimize, column, result_cache, index_scan}` | `OptimizerRule` trait + `Optimizer` 按序执行；`ColumnPruner::prune` / `ColumnRemapper::remap` / `LazyLoad::{collect,clean_scan}` 做位置化裁剪与重映射（默认管线不调用裁剪：search/unnest 隐藏列在规划后才追加）；`ApplyFastRoughFilter::build` 生成 min/max + bloom 跳过树；`ResultCacheGetter::{get,rewrite}` 命中替换为 `Values` | ✅ |
| `FilterExpressionPushDown` / `IndexFilterEvaluator` 及其 AllTrue/AllFalse/Secondary/Fulltext/LogicalChildren / `FilterExpressionPushDownHelper` / `ExpressionInfoTree` / `IndexScanBuilder`（`planner/optimizer/index_scan/*`） | `infinity_planner::index_scan` | `push_down_to_index_scan` 拆分 index/leftover 并构建求值器；`unwind_cast`、JSON term 编码、区间 merge/AND-OR 组合；`IndexScanBuilder::build` 返回 `IndexScanCandidate`（Rust 无 `LogicalIndexScan` 节点，候选与 leftover 一并返回） | ✅ |
| `RegexIndexRewrite`（`planner/optimizer/regex_index_rewrite.cppm`） | `infinity_planner::regex_rewrite::RegexIndexRewrite` | 对 sparsegram 全文索引列，把 `regex` 的必含字面 run 转成 `filter_fulltext` 合取并保留原正则作为残余过滤；run 抽取复用 `infinity_expression::regex_literal` | ✅ |
| `QueryBinder`（`planner/query_binder.cppm`） | `infinity_planner::binder::QueryBinder` | `bind_select`/`bind_insert`/`bind_delete`/`bind_update`/`bind_from`/`bind_table`/`bind_expression`/`bind_predicate`；COMPACT 无 AST/绑定结构，由 `infinity-executor` 直接执行 | ✅ |
| `CorrelatedExpressionsDetector` / `DependentJoinFlattener` / `RewriteCorrelatedExpressions` / `SubqueryUnnest`（`planner/subquery/*`） | `infinity_planner::subquery` | 深度化绑定子查询树 `SubqueryPlan`/`SubqueryExpr`；depth 检测、相关列重写、dependent join 下推、标量/IN/EXISTS unnest 与 mark join | ✅ |

> **基线说明**：`TableEntry` / `SegmentEntry` / `BlockManager` 已在基线 PR #2722 中从 C++ 源码删除；当前持久化元数据类是 `TableMeta` / `SegmentMeta` / `BlockMeta` / `ColumnMeta`，块映射类是 `BlockIndex`。本表按基线实际存在的类名对应，不再列出已删除的类。

## 有意的设计变更（§3.6 豁免）

| C++ | Rust | 原因 |
|---|---|---|
| `TaskScheduler` / `FragmentContext` / `BlockingQueue` | Tokio `mpsc` + `JoinSet` + 按边并发管线 | §4.4 D4（用户已批准） |
| 倒排 `fst` 打包/读取 | `fst` crate 数据路径 + 自实现 C++ 尾块 CRC-32/IEEE | §4.2（已批准；校验和不兼容，数据路径复用） |
| SIMD 距离核 / 压缩 / mmap | `simsimd`/`faer`、`lz4_flex`、`memmap2` | §3.4（仅底层原语，控制流仍自研） |
| 倒排 `PostingByteSlice` / `SkiplistReader` / `SkiplistWriter` | `inverted_format` 的 postings/positions 编解码；`fulltext_segment` 合并段 | 磁盘字节布局已对齐；skiplist 容器仅 C++ 构建期优化，读路径按同样的 postings 列表解码，类边界不单独移植 |
| `DataFileWorker`（`storage/buffer/file_worker*`） | `std::fs` + `memmap2`（`infinity_storage::io`） | C++ 的 file worker 线程池合并为按需读写；`VirtualStore`/`PersistenceManager` 已按类移植（见上表），对象存储仍由 `storage_type=minio` 选择 `S3Client` |
| `TableIndexMeta` / `SegmentIndexMeta` / `ChunkIndexMeta`（`table_index_meta.cppm` 等） | `infinity_catalog` 的索引元数据 + `cpp_blocks` 的 `chunk_{n}.idx` 目录 | 持久化键空间与 chunk 目录布局已对齐；C++ 的模块类树未逐类拆分，索引生命周期由 `dump/optimize/cleanup` 方法承担 |
| `TableMeta` / `SegmentMeta`（`catalog/meta/*.cppm`）+ `BlockIndex` | `infinity-executor` 的 `TableEntry`（运行时聚合） | 上游 PR #2722 删除了 `TableEntry` / `SegmentEntry` / `BlockManager`；Rust 把它们过去承担的运行时状态（写入代次、行 ID 分配、索引注册表、块快照）聚合为一个进程内结构，持久化元数据仍由 `infinity_catalog` + `BlockIndex` 按 C++ 布局承担 |
| 整数算术的宽度语义 | `infinity-expression` 的 `scalar::TypedColumn` 按列宽 checked；SQL `eval_binary` 仍为 i64/f64 域 | C++ 的 `AddFunction`/`SubFunction`/`MulFunction` 按列宽 checked 求值（溢出→NULL）。标量函数层已按列宽实现；SQL 表达式的值域统一为 i64，未按列宽检查溢出 |
| `TableDef` / `ColumnDef` 的 `WriteAdv`/`ReadAdv` 二进制布局 | `TableDef::to_bytes`/`from_bytes`（JSON，复用 `ColumnDefValue` 编码） | Rust 侧不需要 C++ 的内存二进制布局；持久化仍走 `cpp_layout` 的键空间。字段集合与往返语义不变 |
| `PlaidIndex` 的 mmap 读取（`LoadFromPtr` / `centroid_ids` 指针模式） | `infinity_index::plaid::PlaidIndex` 的 owned `Vec<u32>` | mmap 属于已批准的原语豁免；执行器的 chunk 加载器总是解码为 owned 缓冲，语义（同一份 `SaveIndexInner` 字节布局）不变 |
| geo 类型（`PointType`/`LineType`/`LineSegmentType`/`BoxType`/`CircleType`/`PathType`/`PolygonType`）及其 `column_vector_geo_ut`、`function/cast/geography/*` 用例 | 无 | 用户明确决定不需要 Box/Point/Circle 等 geo 类型，测试与产品代码均不移植 | 
| DiskANN / Vamana（`knn_diskann/*`：`VamanaAlg`、`PQFlashIndex`、`FixedChunkPQTable`、`DiskAnnMemGraphStore`、`NeighborPriorityQueue` 等） | 无 | 计划 §2.6/R14 明确不纳入：C++ 的 populate 分支只 `LOG_WARN("Not implemented yet")` 后返回成功，索引不可用；Rust 不移植不可达的骨架 |

## peer / cluster（已一一对应）

| C++ | Rust | 说明 |
|---|---|---|
| `PeerServerThriftService` / `PeerClient` / `PoolPeerThriftServer` | `infinity_server::peer::{PeerServerThriftService, PeerClient, PoolPeerThriftServer}` | 注册/注销/心跳/同步日志/改角色的错误码与 leader-only 规则；Thrift 传输替换为 `PeerTransport`（已登记设计变更），状态机未简化 |
| `PeerTask` / `RegisterPeerTask` / `UnregisterPeerTask` / `TerminatePeerTask` / `HeartBeatPeerTask` / `SyncLogTask` / `ChangeRoleTask` | `infinity_server::peer::{PeerTask, RegisterPeerTask, UnregisterPeerTask, TerminatePeerTask, HeartBeatPeerTask, SyncLogTask, ChangeRoleTask}` | `execute`/`wait`/`complete` 完成门、C++ 字段集合与 `ToString`、重试/重连策略（2 次/1000ms） |
| `RingBufferIterator` | `infinity_server::ring_buffer::RingBufferIterator` | 4096 环形缓冲、模自增、带符号 `Distance`、`Find`/`CopyN`，共享缓冲用 `Arc<RwLock<Vec<u8>>>`（crate 禁用 unsafe 别名） |

## 类级覆盖审计（基线 `5d27942c0`）

`src/**/*.cppm` 中 `export class/struct` 共 **1136** 个；**逐名清单（1136 行，mapped 1094 / exempt 20 / excluded 15 / test 7，gap 0）见 [`CLASS_AUDIT.md`](CLASS_AUDIT.md)**：每行都指向具体 Rust 模块或已登记的设计变更/排除；下表是按区域汇总的聚合说明：

| 区域 | 未逐类命名 | Rust 承担方式 |
|---|---:|---|
| `storage/*` | 553 | `invertedindex` 117：posting/position 解码与 `And/Or/Phrase` 迭代器 → `inverted_format`/`fulltext_segment`/扫描求值函数；`knn_index` 110：模板特化（`Plain*/LVQ*/Rabitq*Dist`、`*VecStoreType`、`*Cache`）→ `vector::Store` 枚举 + 距离函数，`SparseVec*` → `Vec<(u32,f32)>` 别名，`BMPHandler`/`BMPIndexInMem` → 执行器 `SparseIndex`，DiskANN 15 个按 §2.6/R14 排除，`knn_flat` 11 个为距离原语；`catalog` 76 + `new_txn` 46 + `wal` 40 → `infinity_catalog`/`value_format` + `Txn` 操作 + 字节级 `wal`；`common` 40/`buffer` 16/`io` 16/`column_vector` 15/`definition` 12 → `Vec<u8>`/`BufferObj`/`std::fs`/S3 等原语（`ColumnVector`/`DataBlock` 已单独命名）；`bg_task` 26 → 定时线程 + `Engine::{dump,optimize,cleanup}` |
| `executor/*` | 173 | `operator` 78 + `operator_state.cppm` 74 → `PhysicalOperator` 变体与 `OperatorState`；`expression` 13 → `infinity-expression` 求值函数；其余为执行器内部管线结构 |
| `planner/*` | 117 | `node` 52 → `Statement`/计划结构；`optimizer` 17 → 规则函数；其余为绑定/解析中间结构 |
| `function/*` | 64 | `cast` 29 + `table` 17 + 其余 → Rust 的 cast/builtin 函数与 `AggFunc` 枚举 |
| `common/*` | 58 | `analyzer` 39 → `tokenize_*` analyzer 函数；其余为工具结构 |
| `main/*` + `network/*` + `scheduler/*` + `admin` + `metrics` + `unit_test` | 39 + 23 + 10 + 1 + 1 + 7 | 查询选项 → 解析结构；peer/cluster → `infinity_server::peer` + `ring_buffer`；调度器按 D4 由 Tokio 取代；admin → 语句分发；metrics → 服务端计数；`unit_test` 为 C++ 测试夹具 |
| `expression/*` | 12 | 标量表达式求值函数（`infinity-expression`） |

> 判定口径：C++ 中承载**行为**且被其他类调用的类（HNSW/IVF/EMVB/PLAID/SMVE、全文段、事务、Buffer、BlockIndex、PDataBlock 等）在本表中逐类对应；纯数据载体、模板特化、迭代器和生成类型按上表合并，理由与归属均已登记。新增 C++ 行为类时仍须先登记再移植。

**聚合对应（模块级映射的构成；逐名状态见 `CLASS_AUDIT.md`）**

| 聚合到（Rust） | 类数 |
|---|---:|
| `Statement`/计划结构 + 规则函数（planner） | 115 |
| `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 | 117 |
| `PhysicalOperator` 变体（执行器算子） | 78 |
| `infinity_catalog` + `value_format` | 76 |
| `PhysicalOperator` + `OperatorState`（状态结构） | 73 |
| `infinity_storage`/执行器对应模块 | 36 |
| `vector::{HnswIndex, Store}` + `hnsw_handler`/`lvq`/`rabitq`/`lsg` | 50 |
| `infinity_txn::{Txn, TxnManager}` | 46 |
| `Vec<u8>`/`BufferObj` 等存储原语 | 40 |
| `infinity_storage::wal` | 40 |
| `tokenize_*` analyzer 函数 | 32 |
| parser/CLI 结构 | 29 |
| `infinity-expression` cast 函数 | 29 |
| 定时线程 + `Engine::{dump,optimize,cleanup}` | 26 |
| 生成的 Thrift 类型 / `infinity-server` | 23 |
| `infinity-common` 工具/类型 | 17 |
| 标量/聚合函数 + `AggFunc` | 17 |
| 表函数（执行器直接求值） | 17 |
| `infinity_storage::buffer` | 16 |
| `std::fs` + `infinity_storage::s3` | 16 |
| `data_block::{ColumnVector, DataBlock}` | 14 |
| `infinity-expression` 求值函数 | 13 |
| `infinity_catalog::value_format` | 12 |
| `sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + `SparseIndex` | 12 |
| `infinity-expression`（表达式结构） | 12 |
| Tokio（D4） | 10 |
| 执行器管线（`lib.rs`） | 7 |
| `ivf::IvfIndex` + `ivf`/`ivf_mem` | 7 |
| `merge_knn::MergeKnn` + 结果堆 | 3 |
| 其余 8 个聚合组（每组 ≤2 个类） | 11 |
| **合计** | **994** |

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
| `column_vector/column_vector_integer_ut.cpp`（20 例） | `data_block::ColumnVector<T>` | `tinyint/smallint/int/bigint/hugeint_flat_constant_select_slice`（flat/constant/selection/slice 四类场景） |
| `definition/data_block_ut.cpp`（2 例） | `data_block::{DataBlock, BlockValue, Column}` | `all_types_append_and_read_back`（19 种非 geo 列 × 8192 行追加/读回/Reset）、`nulls_and_type_mismatches` |
| `column_vector/{column_vector_bool,column_vector_float,column_vector_uuid,column_vector_embedding,column_vector_decimal}_ut.cpp`（24 例） | `data_block::ColumnVector<T>` | `bool/float/double/uuid/embedding/decimal_flat_constant_select_slice`（flat/constant/selection/slice 四类场景） |
| `column_vector/column_vector_date_time_ut.cpp`（20 例） | `data_block::ColumnVector<T>` + `DateT`/`TimeT`/`DateTimeT`/`TimestampT`/`IntervalT` | `date/time/datetime/timestamp/interval_flat_constant_select_slice`、`interval_constant_set_value_before_finalize` |
| `function/scalar/fixed_dimensional_encoding_ut.cpp`（14 例） | `infinity_expression::fde` | `config/tensor/random_generator/hash_partitioner/sparse_projector/internal_functions/simple_encoding/invalid_input/document_encoding/simhash_partitioning/ams_projection/final_projection/multiple_repetitions/fill_empty_partitions` |
| `storage/common/test_rcu_multimap_ut.cpp`（RcuMap 功能 15 例 + 并发 2 例） | `infinity_storage::rcu_map::RcuMap` | `basic_insert_and_get`…`map_vs_multimap_behavior`、并发 4 例（InsertAndRead/InsertDeleteRead/SwapUnderLoad/RcuBehavior） |
| `storage/common/test_rcu_multimap_ut.cpp`（RcuMultiMap 功能 13 例 + 并发 2 例） | `infinity_storage::rcu_map::RcuMultiMap` | `basic_insert_and_get`…`different_value_types`、并发 5 例（InsertAndRead/InsertDeleteRead/HighVolume/SwapUnderLoad/RcuBehavior） |
| `knn/sparse/test_bp_reordering_ut.cpp`（2 例） | `infinity_index::bp::BpReordering` | `reordering_does_not_increase_cost`（1000×1000、10% 边，cost 不增）、`reordering_on_the_small_csr_when_present`（有 `base_small.csr` 时按 `SparseMatrix::Load` 布局读取并验证 cost） |
| `knn/sparse/test_bmp_index_ut.cpp::test1` | `sparse::BmpAlg` + `bmp_cpp` | `bmp_index_accuracy_and_disk_roundtrip`（1000×1000、5% 稀疏、100 查询，block=8/64 的 top-10 hit ≥90%，以及 C++ BMP 磁盘布局写入/读回） |
| `knn/sparse/test_bmp_index_ut.cpp::test2` | `sparse::BmpAlg` | `bmp_index_ranks_documents_across_blocks`（6 文档/block=2/topk=5 的跨块排名与分数） |
| `knn/sparse/test_linscan_ut.cpp`（2 例） | `infinity_index::linscan::LinScan` | `accurate_scan`（1000×10000、1% 稀疏、50 查询，与暴力 top-10 逐位/1e-6 一致）、`approximate_scan`（budget = 1000，候选精确重排后 hit ≥ 90%）、`save_and_load_roundtrip` |
| `storage/common/loser_tree_ut.cpp::LoserTreeTest` | `infinity_common::loser_tree::LoserTree` | `basic_merge_1`…`basic_merge_4`（10/3、10/12、1000/200、1000/2000 路归并）、`round_up_matches_the_reference` |
| `bg_task/*_ut.cpp` 的索引生命周期 | `dump_hnsw_segments` / `optimize_indexes` / `cleanup` | `hnsw_checkpoint_dumps_only_the_memory_segment`、`mem_index_capacity_dumps_the_hnsw_segment_after_the_batch`、`cleanup_keeps_incremental_hnsw_chunks_until_a_merge_covers_the_table` |
| `knn/plaid/test_plaid_ut.cpp::PlaidQuantizerTest` | `plaid::PlaidQuantizer` | `train_quantize_dequantize`、`two_bit_quantization`、`ip_table_and_position_lut_agree`、`save_and_load_roundtrip` |
| `PlaidGlobalCentroids`（由 `PlaidIndexTest` 覆盖） | `plaid::PlaidGlobalCentroids` | `trains_centroids_and_assigns_nearest`、`residuals_reconstruct_the_embedding`、`save_and_load_roundtrip`、`colbertsar_mode_has_no_quantizer` |
| `PlaidIndexTest` / `PlaidIndexColBERTSaRTest` | `plaid::PlaidIndex` | `basic_construction`、`train_and_add`、`search_ranks_the_queried_document_first`、`bitmask_filters_candidates`、`save_and_load_roundtrip`、`mem_usage_matches_calc_size`、`move_moves_the_index`、`colbertsar_mode_trains_and_searches_without_residuals`、`merge_keeps_documents_and_scores` |
| `PlaidIndexDiskMerger` / manifest（无独立 C++ 单测） | `plaid::{PlaidIndexDiskMerger, PlaidMergeManifest}` | `streams_chunks_into_the_output_index`、`manifest_roundtrips_through_json`、`incremental_merge_skips_unchanged_chunks` |
| （执行器集成，无独立 C++ 单测） | `MultiVectorIndex::build_plaid` / `write_cpp_plaid_index` | `plaid_index_answers_tensor_search_and_mirrors_a_chunk`（PLAID 建索引、扫描仍与穷举 top-1 一致、flush 产出可解码的 C++ PLAID chunk） |
| `SMVEIndexInMem` / `SMVETransform`（无独立 C++ 单测） | `smve::{SmveIndex, smve_transform}` | `projection_columns_are_unit_vectors`、`query_codes_are_sum_pooled_and_docs_averaged`、`search_returns_the_closest_document_and_honours_the_filter` |
| （执行器集成，无独立 C++ 单测） | `MultiVectorIndex::build_smve` + `exact_tensor_score` | `smve_index_answers_tensor_search`（SMVE 建索引、候选按列值精确重排、top-1 与穷举一致） |
| `storage/txn/constants_ut.cpp::TxnContants/test1` | `infinity_txn::state` | `started_state_renders_as_started`、`every_state_and_kind_has_the_reference_name` |
| `main/config_ut.cpp`（6 例） | `infinity_cli::config::Config` | `defaults`（默认值逐项断言）、`full_config_file`（`infinity_conf.toml`）、`wrong_parameter_names`、`invalid_values`（版本/时区/IP/字节大小/日志级别/时间信息错误码）、`out_of_range_values`（5 个越界 fixture）、`valid_values` |
| `main/infinity_ut.cpp::InfinityTest`（2 例） | `Engine` + `Variables` | `catalog_lifecycle_matches_the_reference_test`（库/表 CRUD、`SHOW DATABASES`/`SHOW TABLES`、插入与选择）、`show_variable_covers_the_reference_names`（全局/会话变量名与 `error` 拒绝） |
| `main/query_profiler_ut.cpp::QueryProfilerTest`（3 例） | `profiler::{QueryProfiler, QueryPhase, OptimizerProfiler, ProfileHistory, TaskProfiler}` | `query_phase_names_match_the_reference`、`disabled_profiler_ignores_phase_transitions`、`enabled_profiler_enforces_phase_order`、`all_phases_and_optimizer_rules_record_time`、`history_keeps_the_newest_profiles`、`task_profiler_records_operator_timings` |
| `storage/knnindex/knn_flat/*_ut.cpp`（12 例） | `knn_flat::{top_k, top_1}` | `l2_top_k_matches_the_reference`、`ip_top_k_matches_the_reference`、`l2_top_1_matches_the_reference`、`ip_top_1_matches_the_reference`（含 bitmask 场景） |
| `common/analyzer/ngram_analyzer_ut.cpp`（2 例） | `analyzer::NGramAnalyzer` | `ngram_two_slides_over_words`、`ngram_one_emits_each_character` |
| `common/analyzer/standard_analyzer_ut.cpp`（7 例） | `analyzer::StandardAnalyzer` | `standard_analyzer_lowercases_without_stemming`、`keeps_the_source_case`、`emits_word_and_stem`、`emits_case_and_lower_forms`、`deduplicates_the_stem`、`honours_the_tokenizer_config`、`handles_a_large_extract` |
| `storage/definition/table_def_ut.cpp`（2 例） | `TableDef` | `table_def_reports_schema_table_comment_and_columns`、`table_def_roundtrips_through_its_bytes` |
| `storage/definition/column_def_ut.cpp`（2 例） | `ColumnDefValue` | `column_def_to_string_renders_constraints_in_set_order`、`column_def_to_string_renders_a_primary_key` |
| `storage/definition/index_base_ut.cpp`（3 例） | `value_format::IndexDefValue` | `ivf_index_base_roundtrips_through_its_bytes`、`hnsw_index_base_roundtrips_through_its_bytes`、`full_text_index_base_roundtrips_through_its_bytes` |
| `storage/common/kv_code_ut.cpp`（2 例） | `catalog_format` | `catalog_keys_match_the_reference`、`name_keyed_catalog_keys_match_the_reference` |
| `common/utility/{trim,utility,exception}_ut.cpp`（3 例） | `infinity_common::{utility, Error}` | `trim_path_keeps_the_repository_relative_part`、`extract_u64_reads_the_key_suffix`、`non_alphanumeric_suffix_reports_failure`、`exception_messages_match_the_reference` |
| `storage/result_cache_manager_ut.cpp`（2 例） | `result_cache::ResultCacheManager` | `cache_projects_columns_and_merges_new_ones`、`cache_evicts_the_least_recently_used_entry` |
| `sql_select_statement_ut` 的 bad 用例（OFFSET/LIMIT、SEARCH 组合、VECTOR 度量与元素类型） | `infinity-parser` 的 `Select` 解析 | 新增校验：`OFFSET` 必须带 `LIMIT`、`SEARCH` 不得与 `ORDER BY`/`LIMIT`/`OFFSET` 组合、`MATCH VECTOR` 的 `hamming` 仅用于 bit 向量 |
| `function/scalar/{plus,minus,subtract,mul,modulo,and,or,not,reverse,substr,like,pow,truncate,year,month,quarter,hour,minute,second,weekday,week_of_year}_*_ut.cpp` | `infinity-expression` 运算符/字符串/日期求值 | `arithmetic_and_boolean_operators_match_the_reference`、`string_and_power_functions_match_the_reference`、`date_component_functions_match_the_reference`；补充 `POW`/`POWER` 求值与 `SELECT ±expr` 语法 |
| `function/scalar/{century,day,day_of_month,day_of_week,day_of_year,era,extract,date_part}_*_ut.cpp` | `dt_component` + `EXTRACT`/`DATE_PART` 解析 | `date_component_functions_match_the_reference`；补充 `EXTRACT(field FROM src)` 语法（`parser.rs`，此前只有函数式 `date_part(field, src)`） |
| `function/aggregate/{sum,avg,count,min,max,first}_functions_ut.cpp` | `AggFunc` 求值（`infinity-expression::aggregate`） | `aggregate_functions_match_the_reference`（六种聚合、NULL 忽略、布尔 min/max/first） |
| `function/scalar/{abs,add,div,equals,inequals,greater,greater_equals,less,less_equals}_functions_ut.cpp` | `infinity-expression` 标量求值 | `abs_function_covers_every_numeric_type`、`arithmetic_and_equality_follow_the_reference`（含比较族与除零 NULL；溢出语义见设计变更） |
| `storage/invertedindex/memory_indexer_ut.cpp::Chunk` | `MemoryIndexer` | `memory_indexer_builds_the_reference_postings`（5 段 FST 文本的 term→doc/tf 与参考一致） |
| `column_vector/value_ut.cpp`（2 例） | `Column::value_text` | `value_to_string_matches_the_reference`（全部标量渲染）、`string_to_value_roundtrips_the_scalar_types`（标量部分） |
| `planner/node/logical_{create_table,drop_table,create_index,drop_index,insert,update,aggregate,project,limit,optimize,compact,flush,check,command,show}_ut.cpp`（15 例） | `Engine::execute` 的语句执行 | `logical_planner_statements_succeed`（C++ 用例只断言语句执行成功并打印逻辑节点） |
| `executor/expression/expression_executor_select_ut.cpp`（3 例） | `ColumnVector<bool>` | `expression_select_alternating_input`、`expression_select_all_true_input`、`expression_select_all_false_input`（C++ 的 selector 调用本身在 `#if 0` 内） |
| `storage/column_vector/{selection,var_buffer}_ut.cpp`（2 例） | `selection::Selection` / `var_buffer::VarBuffer` | `selection_matches_the_reference`、`var_buffer_appends_and_roundtrips` |
| `storage/meta/entry/block_version_ut.cpp`（5 例） | `mvcc::BlockVersion` | `save_and_load_roundtrip`、`reopen_preserves_the_slot_array`、`delete_twice_is_rejected`、`check_delete_compares_timestamps`、`get_delete_ts_returns_the_slot_range` |
| `main/table_ut.cpp::InfinityTableTest::test1` | `Engine` + `ExplainKind` | `explain_covers_every_plan_kind`（AST/LOGICAL/OPTIMIZED/PHYSICAL/FRAGMENT/PIPELINE） |

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

### 未移植单测（全量清单）

`src/unit_test` 共 **1012** 个 `TEST/TEST_F/TEST_P`，逐条状态见 [`TEST_AUDIT.md`](TEST_AUDIT.md)：`ported` 951、`exempt` 13、`excluded` 48（geo 47 + DiskANN 1），无 `partial`/`gap`/`gate-a`/`integration`。

- `column_vector/*` 的 `ColumnVector`/`DataBlock` 类已移植；integer 套件（flat/constant/select/slice × tinyint/smallint/int/bigint/hugeint，20 例）已逐条移植。`VectorBuffer`/`Bitmask` 为设计变更（Rust `Vec` + `validity`）；integer 与 date/time 套件（40 例）已移植；geo 类型与用例按用户决定不移植（见设计变更）。
- `LoserTree` 与 RCU 容器（`rcu_map`）均已移植并有对应测试。
- 未移植的 61 行仅为 geo/DiskANN（用户确认排除）与整段 `#if 0`（C++ 构建中不存在）；其余全部有可追溯的具体测试。
