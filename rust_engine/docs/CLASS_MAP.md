# C++ → Rust 类/方法对照（移植原则 §3.6）

> 目的：防止为了过测试而实现**过度简化**的设计。除 §3.6 明确列出的设计变更外，
> 每个被其他 C++ 类依赖的类、以及这些调用点用到的方法，都应在 Rust 中有
> 同职责的类型与 public 方法，并可逐项回溯到 `文件:行号`。

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
| `TxnManager`（`new_txn_manager`） | `infinity_txn::TxnManager` | `BeginTxn`→`begin`、`CommitTxn`→`commit` | ✅（冲突校验在引擎层，见下） |

## 有意的设计变更（§3.6 豁免）

| C++ | Rust | 原因 |
|---|---|---|
| `TaskScheduler` / `FragmentContext` / `BlockingQueue` | Tokio `mpsc` + `JoinSet` + 按边并发管线 | §4.4 D4（用户已批准） |
| `ColumnVector` / `DataBlock`（typed 列式运行时） | `Value` / `Vec<Value>` 行式求值 | **已知偏差**：执行器仍是行式；列式运行时是 Gate E 的后续项 |

## 尚未一一对应（待补，按优先级）

| C++ | 当前 Rust | 差距 |
|---|---|---|
| `BlockMaxWandIterator`（pivot/NextShallow/block-max） | `Engine::wand_scan` 自由函数 | 控制流已移植，但**没有类**（无 `Next()`/游标对象）；需拆成 `BlockMaxWandIterator` 结构 |
| `BM25Ranker` | `wand_scan` 内联公式 | 需独立 `Bm25Ranker`，供扫描路径与 WAND 共用 |
| `MemoryIndexer` / `Indexer` / `IndexReader` | `TextIndex` + `SegmentIndexReader` + 查询期 `MergedIndex` | 构建/落盘/读取职责混在 `TextIndex` 与执行器；需拆出 `MemoryIndexer`、`Indexer`、`IndexReader` |
| `HnswIndexInMem` / `HnswIndexHandler` | `MemHnsw`（执行器字段）+ `infinity_index::vector::HnswIndex` | 缺独立的 handler/segment 抽象；内存段与落盘段由 `Engine` 直接拼装 |
| `EMVBIndex` / `PLAIDIndex` / `MultiVectorIndexHandler` | `MultiVectorIndex`（执行器）+ `EmvbPqIndex` | 构建、候选、merge 在一个结构里；需按 C++ 拆成 index + handler |
| `BMPIvt` / `BMPFwd` / `BMPSearch` | `infinity_index::sparse::BmpIndex` + `bmp_cpp` | 内存形态与磁盘形态分离，但缺少 C++ 的 `BMPIvt`/`BMPFwd` 类边界 |
| `InfinityContext` / `Storage` / `TableEntry` / `SegmentEntry` | `Engine`（单一大结构） | **最大的偏差**：`Engine` 同时承担 context/storage/catalog/txn/索引；需按 C++ 边界拆分 |
| `NewTxn::Delete/Commit` 冲突检查 | `Engine::commit_with_deletes` | 语义已移植，但职责在 `Engine` 而非 `Txn`/`TxnManager`；后续移入 `TxnManager` |

## 维护规则

- 新增一个 C++ 子系统的移植时，先在本文档登记类/方法映射，再写代码。
- 每个 Rust `pub fn` 的文档注释写 `C++: path:line`。
- 任何“简化”必须出现在「有意的设计变更」或「尚未一一对应」表里，不能默默省略。
- Gate A 通过不等于移植完成：状态以本表 + 计划书 §7.3 为准。
