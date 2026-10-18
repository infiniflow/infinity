# C++ → Rust 类/方法对照（移植原则 §3.6）

> 目的：防止为了过测试而实现**过度简化**的设计。原则上（大体上），
> 每个被其他 C++ 类依赖的类、以及这些调用点用到的方法，都应在 Rust 中有
> 同职责的类型与 public 方法，并可逐项回溯到 `文件:行号`；已明确决定的设计变更
> （调度器、FST、SIMD/压缩原语等）不要求同名一一对应，但必须登记。

## 已一一对应

| C++ | Rust | 方法对应 | 状态 |
|---|---|---|---|
| `BufferManager`（`storage/buffer/buffer_manager*.cppm`） | `infinity_storage::buffer::BufferManager` | `Get`→`get`、`Put`→`put`、LRU 淘汰→`evict`、`SetBufferSize`→`set_budget` | ✅ |
| `BlockVersionFile`（`storage/buffer/block_version.cppm`） | `cpp_format::BlockVersionFile` | `Decode/Encode` | ✅ |
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
| `HnswIndexInMem`（`hnsw_alg.cppm`） | `hnsw_mem::HnswIndexInMem` | `Add`→`insert`、`Search`→`search` | ✅ |

## 有意的设计变更（§3.6 豁免）

| C++ | Rust | 原因 |
|---|---|---|
| `TaskScheduler` / `FragmentContext` / `BlockingQueue` | Tokio `mpsc` + `JoinSet` + 按边并发管线 | §4.4 D4（用户已批准） |
| 倒排 `fst` 打包/读取 | `fst` crate 数据路径 + 自实现 C++ 尾块 CRC-32/IEEE | §4.2（已批准；校验和不兼容，数据路径复用） |
| SIMD 距离核 / 压缩 / mmap | `simsimd`/`faer`、`lz4_flex`、`memmap2` | §3.4（仅底层原语，控制流仍自研） |
| `ColumnVector` / `DataBlock`（typed 列式运行时） | `Value` / `Vec<Value>` 行式求值 | **已知偏差**：执行器仍是行式；列式运行时是 Gate E 的后续项 |

## 尚未一一对应（待补，按优先级）

| C++ | 当前 Rust | 差距 |
|---|---|---|
| `BlockMaxWandIterator`（pivot/NextShallow/block-max） | `Engine::wand_scan` 自由函数 | 控制流已移植，但**没有类**（无 `Next()`/游标对象）；需拆成 `BlockMaxWandIterator` 结构 |
| `BM25Ranker` | `wand_scan` 内联公式 | 需独立 `Bm25Ranker`，供扫描路径与 WAND 共用 |
| `MemoryIndexer` / `Indexer` / `IndexReader` | `TextIndex` + `SegmentIndexReader` + 查询期 `MergedIndex` | 构建/落盘/读取职责混在 `TextIndex` 与执行器；需拆出 `MemoryIndexer`、`Indexer`、`IndexReader` |
| `HnswIndexHandler`（多 segment 合并） | `Engine::vector_candidates` | 内存段已拆为 `HnswIndexInMem`；handler/segment 合并仍在执行器 |
| `EMVBIndex` / `PLAIDIndex` / `MultiVectorIndexHandler` | `MultiVectorIndex`（执行器）+ `EmvbPqIndex` | 构建、候选、merge 在一个结构里；需按 C++ 拆成 index + handler |
| `BMPIvt` / `BMPFwd` / `BMPSearch` | `infinity_index::sparse::BmpIndex` + `bmp_cpp` | 内存形态与磁盘形态分离，但缺少 C++ 的 `BMPIvt`/`BMPFwd` 类边界 |
| `InfinityContext` / `Storage` / `TableEntry` / `SegmentEntry` | `Engine`（单一大结构） | **最大的偏差**：`Engine` 同时承担 context/storage/catalog/txn/索引；需按 C++ 边界拆分 |

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
| `invertedindex/format/*`（posting/position 编解码） | `inverted_format` | 该模块的往返/边界单测 |
| `secondary_index` 的 PGM 查找 | `SecondarySegmentReader::lookup` | `secondary_segment::tests::pgm_lookup_matches_binary_search` |
| `knn/sparse` 的 block-max 检索 | `infinity_index::sparse::BmpIndex` / `bmp_cpp` | `sparse` / `bmp_cpp` 单测 |
| `std::hash<string_view>`（VARCHAR 次级键） | `secondary_segment::hash_varchar` | `varchar_hash_matches_libstdcxx` |

### 未移植单测（Rust 类已存在）

| C++ 用例 | 对应 Rust | 缺口 |
|---|---|---|
| `buffer/buffer_handle_ut.cpp`、`buffer_obj_ut.cpp` | `BufferManager`（无 handle/obj 抽象） | 类边界未拆，测试无法一一对应 |
| `invertedindex/column_inverter_ut.cpp`、`format/*` 其余部分 | `TextIndex` / `inverted_format` | 构建器/格式读写测试未逐个移植 |
| （无独立 WAND/BM25 单测） | `wand::BlockMaxWandIterator` | 已用 `ranker_matches_the_reference_formula`、`wand_matches_brute_force` 覆盖 |
| `invertedindex/search/*`（短语/布尔组合） | `wand` 类 + 扫描路径 | WAND/BM25 已拆类；短语与布尔组合仍走扫描 |
| `secondary_index/*_ut.cpp` | `secondary_segment`/`secondary_lowcard` | 低基数/多类型/边界用例未逐个移植 |
| `knn/hnsw/*_ut.cpp` | `infinity_index::vector::HnswIndex` | 层级/启发式/召回用例未逐个移植 |
| `knn/sparse/*_ut.cpp` | `infinity_index::sparse` | alpha/beta/tail 边界用例未逐个移植 |
| `knn/emvb/*_ut.cpp` | `MultiVectorIndex`/`EmvbPqIndex` | 量化/候选/OPQ 用例未移植 |
| `bg_task/*_ut.cpp` | 执行器/CLI 的 dump/optimize/cleanup | 任务级场景未移植 |

### 未移植单测（Rust 类也未移植）

`column_vector/*`、`definition/data_block_ut.cpp`、`common/loser_tree_ut.cpp`、`common/test_rcu_multimap_ut.cpp` 等属于尚未移植的 C++ 类（`ColumnVector`/`DataBlock`/`LoserTree`/RCU 容器），随对应类一起移植。
