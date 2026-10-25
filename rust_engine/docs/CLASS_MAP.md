# C++ → Rust：类级覆盖与方法对应

> **子目标 S3 · 类级覆盖与方法对应**｜纲领见 [`PORTING.md`](PORTING.md) §5

一份台账、两层粒度：

- **§2 逐名清单**——参考 `src/**/*.cppm` 里每个 `export class/struct` 一行，给出状态与 Rust 落点，回答"这个类有没有对应"；
- **§3 方法级对应**——把必须逐方法核对的类展开到方法级，回答"这个类的哪些方法落到哪"。

判定口径（承自计划书 §3.6）：承载**行为**且被其他类调用的类（HNSW/IVF/EMVB/PLAID/SMVE、全文段、事务、Buffer、BlockIndex、PDataBlock 等）逐类列出；纯数据载体、模板特化、迭代器与生成类型按区域聚合，聚合理由与归属见 §1.2。已明确决定的设计变更不要求同名一一对应，但必须出现在 §4。

## 1 口径

### 1.1 状态与总数

`src/**/*.cppm` 中 `export class/struct` 共 **1136** 个：

| 状态 | 数量 | 含义 |
|---|---:|---|
| `mapped` | 1094 | Rust 有同职责的类型/函数（「Rust 承担方式」给出落点） |
| `partial` | 0 | 类型存在，但生产路径只走到一部分——出现即须在行内写清缺哪一段 |
| `test` | 7 | C++ 测试夹具 |
| `exempt` | 20 | 设计变更豁免的原语（`fst`/SIMD/压缩等） |
| `excluded` | 15 | 不移植（DiskANN 15 类） |
| **合计** | **1136** | |

geo 类型类未逐类列入本表（`GeographyTryCastToVarlen` 例外，承担于 cast）；geo 用例的排除见 [`TEST_AUDIT.md`](TEST_AUDIT.md)，DiskANN 15 类在本表 `excluded`。

### 1.2 按区域聚合

逐名列出的是承载行为的类；其余按区域合并，下表说明每一片合到哪里、为什么可以合并。

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

**聚合对应（模块级映射的构成；逐名状态见 §2）**

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

## 2 逐名清单（基线 `5d27942c0`）

| # | C++ 类/结构 | 定义位置 | 状态 | Rust 承担方式 |
|---:|---|---|---|---|
| 1 | `AdminExecutor` | `src/admin/admin_executor.cppm` | mapped | infinity_executor 的语句分发 
| 2 | `Analyzer` | `src/common/analyzer/analyzer.cppm` | mapped | `infinity_executor::analyzer::StandardAnalyzer` 
| 3 | `AnalyzerPool` | `src/common/analyzer/analyzer_pool.cppm` | mapped | `infinity_executor::analyzer::AnalyzerPool` 
| 4 | `ChineseAnalyzer` | `src/common/analyzer/chinese_analyzer.cppm` | mapped | `infinity_executor::analyzer::ChineseAnalyzer` 
| 5 | `CommonLanguageAnalyzer` | `src/common/analyzer/common_analyzer.cppm` | mapped | `infinity_executor::analyzer::StandardAnalyzer` 的选项（case/stem/stem_only） |
| 6 | `DartsTrie` | `src/common/analyzer/darts_trie.cppm` | mapped | `infinity_executor::analyzer::DartsTrie` 
| 7 | `DartsTuple` | `src/common/analyzer/darts_trie.cppm` | mapped | infinity_executor::analyzer 
| 8 | `POSTable` | `src/common/analyzer/darts_trie.cppm` | mapped | infinity_executor::analyzer 
| 9 | `AnalyzeContext` | `src/common/analyzer/ik/analyze_context.cppm` | mapped | infinity_executor::analyzer 
| 10 | `IKArbitrator` | `src/common/analyzer/ik/arbitrator.cppm` | mapped | infinity_executor::analyzer 
| 11 | `CharacterUtil` | `src/common/analyzer/ik/character_util.cppm` | mapped | infinity_executor::analyzer 
| 12 | `CJKSegmenter` | `src/common/analyzer/ik/cjk_segmenter.cppm` | mapped | infinity_executor::analyzer 
| 13 | `CNQuantifierSegmenter` | `src/common/analyzer/ik/cn_quantifier_segmenter.cppm` | mapped | infinity_executor::analyzer 
| 14 | `Dictionary` | `src/common/analyzer/ik/dict.cppm` | mapped | infinity_executor::analyzer 
| 15 | `DictSegment` | `src/common/analyzer/ik/dict_segment.cppm` | mapped | infinity_executor::analyzer 
| 16 | `Hit` | `src/common/analyzer/ik/hit.cppm` | mapped | infinity_executor::analyzer 
| 17 | `IKAnalyzer` | `src/common/analyzer/ik/ik_analyzer.cppm` | mapped | `infinity_executor::analyzer::IKAnalyzer` 
| 18 | `LetterSegmenter` | `src/common/analyzer/ik/letter_segmenter.cppm` | mapped | infinity_executor::analyzer 
| 19 | `Lexeme` | `src/common/analyzer/ik/lexeme.cppm` | mapped | infinity_executor::analyzer 
| 20 | `LexemePath` | `src/common/analyzer/ik/lexeme_path.cppm` | mapped | infinity_executor::analyzer 
| 21 | `QuickSortSet` | `src/common/analyzer/ik/quick_sort_set.cppm` | mapped | infinity_executor::analyzer 
| 22 | `Segmenter` | `src/common/analyzer/ik/segmenter.cppm` | mapped | infinity_executor::analyzer 
| 23 | `JapaneseAnalyzer` | `src/common/analyzer/japanese_analyzer.cppm` | mapped | `infinity_executor::analyzer::{JapaneseAnalyzer, KoreanAnalyzer, TraditionalChineseAnalyzer}`（MeCab/OpenCC 端口，cc1843436） 
| 24 | `KoreanAnalyzer` | `src/common/analyzer/korean_analyzer.cppm` | mapped | `infinity_executor::analyzer::{JapaneseAnalyzer, KoreanAnalyzer, TraditionalChineseAnalyzer}`（MeCab/OpenCC 端口，cc1843436） 
| 25 | `NGramAnalyzer` | `src/common/analyzer/ngram_analyzer.cppm` | mapped | `analyzer::NGramAnalyzer`（n-gram 窗口按 UTF-8 code point 滑动） |
| 26 | `RAGAnalyzer` | `src/common/analyzer/rag_analyzer.cppm` | mapped | `infinity_executor::analyzer::RAGAnalyzer` 
| 27 | `RankFeaturesAnalyzer` | `src/common/analyzer/rank_features_analyzer.cppm` | mapped | infinity_executor::analyzer 
| 28 | `SparsegramAnalyzer` | `src/common/analyzer/sparsegram_analyzer.cppm` | mapped | infinity_executor::analyzer 
| 29 | `StandardAnalyzer` | `src/common/analyzer/standard_analyzer.cppm` | mapped | `analyzer::StandardAnalyzer`（Analyze 的 word/stem/case 组合） |
| 30 | `Stemmer` | `src/common/analyzer/stemmer/stemmer.cppm` | mapped | infinity_executor::analyzer 
| 31 | `Term` | `src/common/analyzer/term.cppm` | mapped | `analyzer::Term` |
| 32 | `TermList` | `src/common/analyzer/term.cppm` | mapped | `Vec<analyzer::Term>` |
| 33 | `CharTypeTable` | `src/common/analyzer/tokenizer.cppm` | mapped | infinity_executor::analyzer 
| 34 | `TokenizeConfig` | `src/common/analyzer/tokenizer.cppm` | mapped | `analyzer::TokenizeConfig`（AddAllows/AddDivides/AddUnites） |
| 35 | `Tokenizer` | `src/common/analyzer/tokenizer.cppm` | mapped | `analyzer::tokenize` |
| 36 | `TraditionalChineseAnalyzer` | `src/common/analyzer/traditional_chinese_analyzer.cppm` | mapped | `infinity_executor::analyzer::{JapaneseAnalyzer, KoreanAnalyzer, TraditionalChineseAnalyzer}`（MeCab/OpenCC 端口，cc1843436） 
| 37 | `UserDefinedAnalyzer` | `src/common/analyzer/user_defined_analyzer.cppm` | mapped | infinity_executor::analyzer 
| 38 | `UserDefinedTermWeight` | `src/common/analyzer/user_defined_term_weight.cppm` | mapped | infinity_executor::analyzer 
| 39 | `WhitespaceAnalyzer` | `src/common/analyzer/whitespace_analyzer.cppm` | mapped | infinity_executor::analyzer 
| 40 | `WordNetLemmatizer` | `src/common/analyzer/wordnet_lemmatizer.cppm` | mapped | infinity_executor::analyzer 
| 41 | `BlockingQueue` | `src/common/blocking_queue.cppm` | mapped | CLASS_MAP 已显式命名 |
| 42 | `ByteSlice` | `src/common/memory/byte_slice.cppm` | mapped | infinity_common 的内存/资源计费 
| 43 | `ByteSliceList` | `src/common/memory/byte_slice.cppm` | mapped | infinity_common 的内存/资源计费 
| 44 | `ByteSliceListIterator` | `src/common/memory/byte_slice.cppm` | mapped | infinity_common 的内存/资源计费 
| 45 | `ChainedMemoryChunk` | `src/common/memory/memory_chunk.cppm` | mapped | infinity_common 的内存/资源计费 
| 46 | `ChunkAllocator` | `src/common/memory/memory_chunk.cppm` | mapped | infinity_common 的内存/资源计费 
| 47 | `MemoryChunk` | `src/common/memory/memory_chunk.cppm` | mapped | infinity_common 的内存/资源计费 
| 48 | `PythonInstance` | `src/common/python.cppm` | exempt | 无内嵌 Python 解释器；用户自定义分析器改为注册表 
| 49 | `SIMD_FUNCTIONS` | `src/common/simd/simd_functions.cppm` | mapped | infinity_common::simd 
| 50 | `Singleton` | `src/common/singleton.cppm` | mapped | 进程内 OnceLock/静态状态 
| 51 | `Status` | `src/common/status.cppm` | mapped | infinity_common::Error 
| 52 | `DeferFn` | `src/common/utility/defer_op.cppm` | mapped | infinity_common 
| 53 | `RecoverableException` | `src/common/utility/exception.cppm` | mapped | `infinity_common::Error::Unexpected`（`Unexpected error: {msg}`） |
| 54 | `UnrecoverableException` | `src/common/utility/exception.cppm` | mapped | `infinity_common::Error::Unrecoverable`（裸消息） |
| 55 | `SmallFloat` | `src/common/utility/smallfloat.cppm` | mapped | infinity_common 
| 56 | `ScopedSpinLock` | `src/common/utility/spinlock.cppm` | mapped | infinity_common 
| 57 | `ScopedTryLock` | `src/common/utility/spinlock.cppm` | mapped | infinity_common 
| 58 | `SpinLock` | `src/common/utility/spinlock.cppm` | mapped | infinity_common 
| 59 | `ThreadUtil` | `src/common/utility/threadutil.cppm` | mapped | infinity_common 
| 60 | `ZsvParser` | `src/common/zsv.cppm` | exempt | CSV 读取改为 Rust csv 路径 
| 61 | `ExplainFragment` | `src/executor/explain_fragment.cppm` | mapped | infinity_executor 的物理算子/管线 
| 62 | `ExplainPhysicalPlan` | `src/executor/explain_physical_plan.cppm` | mapped | infinity_executor 的物理算子/管线 
| 63 | `AggregateExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 64 | `BaseExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 65 | `CaseExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 66 | `CastExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 67 | `ColumnExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 68 | `ExpressionEvaluator` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 69 | `ExpressionState` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 70 | `FilterFulltextExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 71 | `FunctionExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 72 | `InExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 73 | `ReferenceExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 74 | `ValueExpression` | `src/executor/expression/expression_evaluator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 75 | `ExpressionSelector` | `src/executor/expression/expression_selector.cppm` | mapped | infinity_executor 的物理算子/管线 
| 76 | `ColumnVector` | `src/executor/expression/expression_state.cppm` | mapped | CLASS_MAP 已显式命名 |
| 77 | `PlanFragment` | `src/executor/fragment/plan_fragment.cppm` | mapped | infinity_executor 的物理算子/管线 
| 78 | `FragmentBuilder` | `src/executor/fragment_builder.cppm` | mapped | infinity_executor 的物理算子/管线 
| 79 | `HashRange` | `src/executor/operator/physical_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 80 | `PhysicalAggregate` | `src/executor/operator/physical_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 81 | `PhysicalAddColumns` | `src/executor/operator/physical_alter.cppm` | mapped | infinity_executor 的物理算子/管线 
| 82 | `PhysicalAlter` | `src/executor/operator/physical_alter.cppm` | mapped | infinity_executor 的物理算子/管线 
| 83 | `PhysicalAlterIndex` | `src/executor/operator/physical_alter.cppm` | mapped | infinity_executor 的物理算子/管线 
| 84 | `PhysicalDropColumns` | `src/executor/operator/physical_alter.cppm` | mapped | infinity_executor 的物理算子/管线 
| 85 | `PhysicalRenameTable` | `src/executor/operator/physical_alter.cppm` | mapped | infinity_executor 的物理算子/管线 
| 86 | `PhysicalCheck` | `src/executor/operator/physical_check/physical_check.cppm` | mapped | infinity_executor 的物理算子/管线 
| 87 | `PhysicalCommand` | `src/executor/operator/physical_command.cppm` | mapped | infinity_executor 的物理算子/管线 
| 88 | `PhysicalCompact` | `src/executor/operator/physical_compact.cppm` | mapped | infinity_executor 的物理算子/管线 
| 89 | `PhysicalCreateCollection` | `src/executor/operator/physical_create_collection.cppm` | mapped | infinity_executor 的物理算子/管线 
| 90 | `PhysicalCreateIndexPrepare` | `src/executor/operator/physical_create_index_prepare.cppm` | mapped | infinity_executor 的物理算子/管线 
| 91 | `PhysicalCreateSchema` | `src/executor/operator/physical_create_schema.cppm` | mapped | infinity_executor 的物理算子/管线 
| 92 | `PhysicalCreateTable` | `src/executor/operator/physical_create_table.cppm` | mapped | infinity_executor 的物理算子/管线 
| 93 | `PhysicalCreateView` | `src/executor/operator/physical_create_view.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 94 | `PhysicalCrossProduct` | `src/executor/operator/physical_cross_product.cppm` | mapped | infinity_executor 的物理算子/管线 
| 95 | `PhysicalDelete` | `src/executor/operator/physical_delete.cppm` | mapped | infinity_executor 的物理算子/管线 
| 96 | `PhysicalDropCollection` | `src/executor/operator/physical_drop_collection.cppm` | mapped | infinity_executor 的物理算子/管线 
| 97 | `PhysicalDropIndex` | `src/executor/operator/physical_drop_index.cppm` | mapped | infinity_executor 的物理算子/管线 
| 98 | `PhysicalDropSchema` | `src/executor/operator/physical_drop_schema.cppm` | mapped | infinity_executor 的物理算子/管线 
| 99 | `PhysicalDropTable` | `src/executor/operator/physical_drop_table.cppm` | mapped | infinity_executor 的物理算子/管线 
| 100 | `PhysicalDropView` | `src/executor/operator/physical_drop_view.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 101 | `PhysicalDummyOperator` | `src/executor/operator/physical_dummy_operator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 102 | `PhysicalDummyScan` | `src/executor/operator/physical_dummy_scan.cppm` | mapped | infinity_executor 的物理算子/管线 
| 103 | `PhysicalExcept` | `src/executor/operator/physical_except.cppm` | mapped | infinity_executor 的物理算子/管线 
| 104 | `PhysicalExplain` | `src/executor/operator/physical_explain.cppm` | mapped | infinity_executor 的物理算子/管线 
| 105 | `PhysicalExport` | `src/executor/operator/physical_export.cppm` | mapped | infinity_executor 的物理算子/管线 
| 106 | `PhysicalFilter` | `src/executor/operator/physical_filter.cppm` | mapped | infinity_executor 的物理算子/管线 
| 107 | `PhysicalFlush` | `src/executor/operator/physical_flush.cppm` | mapped | infinity_executor 的物理算子/管线 
| 108 | `MatchTensorRerankDoc` | `src/executor/operator/physical_fusion.cppm` | mapped | infinity_executor 的物理算子/管线 
| 109 | `PhysicalFusion` | `src/executor/operator/physical_fusion.cppm` | mapped | infinity_executor 的物理算子/管线 
| 110 | `PhysicalHash` | `src/executor/operator/physical_hash.cppm` | mapped | infinity_executor 的物理算子/管线 
| 111 | `PhysicalHashAggregate` | `src/executor/operator/physical_hash_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 112 | `PhysicalHashJoin` | `src/executor/operator/physical_hash_join.cppm` | mapped | infinity_executor 的物理算子/管线 
| 113 | `PhysicalImport` | `src/executor/operator/physical_import.cppm` | mapped | infinity_executor 的物理算子/管线 
| 114 | `PhysicalIndexJoin` | `src/executor/operator/physical_index_join.cppm` | mapped | infinity_executor 的物理算子/管线 
| 115 | `PhysicalInsert` | `src/executor/operator/physical_insert.cppm` | mapped | infinity_executor 的物理算子/管线 
| 116 | `PhysicalIntersect` | `src/executor/operator/physical_intersect.cppm` | mapped | infinity_executor 的物理算子/管线 
| 117 | `AtomicCounter` | `src/executor/operator/physical_limit.cppm` | mapped | infinity_executor 的物理算子/管线 
| 118 | `LimitCounter` | `src/executor/operator/physical_limit.cppm` | mapped | infinity_executor 的物理算子/管线 
| 119 | `PhysicalLimit` | `src/executor/operator/physical_limit.cppm` | mapped | infinity_executor 的物理算子/管线 
| 120 | `UnSyncCounter` | `src/executor/operator/physical_limit.cppm` | mapped | infinity_executor 的物理算子/管线 
| 121 | `PhysicalMatch` | `src/executor/operator/physical_match.cppm` | mapped | infinity_executor 的物理算子/管线 
| 122 | `PhysicalMergeAggregate` | `src/executor/operator/physical_merge_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 123 | `PhysicalMergeHash` | `src/executor/operator/physical_merge_hash.cppm` | mapped | infinity_executor 的物理算子/管线 
| 124 | `PhysicalMergeHashAggregate` | `src/executor/operator/physical_merge_hash_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 125 | `PhysicalMergeLimit` | `src/executor/operator/physical_merge_limit.cppm` | mapped | infinity_executor 的物理算子/管线 
| 126 | `PhysicalMergeParallelAggregate` | `src/executor/operator/physical_merge_parallel_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 127 | `PhysicalMergeSort` | `src/executor/operator/physical_merge_sort.cppm` | mapped | infinity_executor 的物理算子/管线 
| 128 | `PhysicalMergeTop` | `src/executor/operator/physical_merge_top.cppm` | mapped | infinity_executor 的物理算子/管线 
| 129 | `PhysicalNestedLoopJoin` | `src/executor/operator/physical_nested_loop_join.cppm` | mapped | infinity_executor 的物理算子/管线 
| 130 | `PhysicalOptimize` | `src/executor/operator/physical_optimize.cppm` | mapped | infinity_executor 的物理算子/管线 
| 131 | `PhysicalParallelAggregate` | `src/executor/operator/physical_parallel_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 132 | `PhysicalPreparedPlan` | `src/executor/operator/physical_prepared_plan.cppm` | mapped | infinity_executor 的物理算子/管线 
| 133 | `PhysicalProject` | `src/executor/operator/physical_project.cppm` | mapped | infinity_executor 的物理算子/管线 
| 134 | `PhysicalReadCache` | `src/executor/operator/physical_read_cache.cppm` | mapped | infinity_executor 的物理算子/管线 
| 135 | `PhysicalFilterScanBase` | `src/executor/operator/physical_scan/physical_filter_scan_base.cppm` | mapped | infinity_executor 的物理算子/管线 
| 136 | `PhysicalIndexScan` | `src/executor/operator/physical_scan/physical_index_scan.cppm` | mapped | infinity_executor 的物理算子/管线 
| 137 | `PhysicalKnnScan` | `src/executor/operator/physical_scan/physical_knn_scan.cppm` | mapped | CLASS_MAP 已显式命名 |
| 138 | `PhysicalMatchSparseScan` | `src/executor/operator/physical_scan/physical_match_sparse_scan.cppm` | mapped | infinity_executor 的物理算子/管线 
| 139 | `PhysicalMatchTensorScan` | `src/executor/operator/physical_scan/physical_match_tensor_scan.cppm` | mapped | CLASS_MAP 已显式命名 |
| 140 | `PhysicalMergeKnn` | `src/executor/operator/physical_scan/physical_merge_knn.cppm` | mapped | infinity_executor 的物理算子/管线 
| 141 | `PhysicalMergeMatchSparse` | `src/executor/operator/physical_scan/physical_merge_match_sparse.cppm` | mapped | infinity_executor 的物理算子/管线 
| 142 | `PhysicalMergeMatchTensor` | `src/executor/operator/physical_scan/physical_merge_match_tensor.cppm` | mapped | infinity_executor 的物理算子/管线 
| 143 | `PhysicalScanBase` | `src/executor/operator/physical_scan/physical_scan_base.cppm` | mapped | infinity_executor 的物理算子/管线 
| 144 | `PhysicalTableScan` | `src/executor/operator/physical_scan/physical_table_scan.cppm` | mapped | infinity_executor 的物理算子/管线 
| 145 | `PhysicalShow` | `src/executor/operator/physical_show/physical_show.cppm` | mapped | infinity_executor 的物理算子/管线 
| 146 | `PhysicalSink` | `src/executor/operator/physical_sink.cppm` | mapped | infinity_executor 的物理算子/管线 
| 147 | `PhysicalSort` | `src/executor/operator/physical_sort.cppm` | mapped | infinity_executor 的物理算子/管线 
| 148 | `PhysicalSortMergeJoin` | `src/executor/operator/physical_sort_merge_join.cppm` | mapped | infinity_executor 的物理算子/管线 
| 149 | `PhysicalSource` | `src/executor/operator/physical_source.cppm` | mapped | infinity_executor 的物理算子/管线 
| 150 | `CompareTwoRowAndPreferLeft` | `src/executor/operator/physical_top.cppm` | mapped | infinity_executor 的物理算子/管线 
| 151 | `PhysicalTop` | `src/executor/operator/physical_top.cppm` | mapped | infinity_executor 的物理算子/管线 
| 152 | `TopSolver` | `src/executor/operator/physical_top.cppm` | mapped | infinity_executor 的物理算子/管线 
| 153 | `PhysicalUnionAll` | `src/executor/operator/physical_union_all.cppm` | mapped | infinity_executor 的物理算子/管线 
| 154 | `PhysicalUnnest` | `src/executor/operator/physical_unnest.cppm` | mapped | infinity_executor 的物理算子/管线 
| 155 | `PhysicalUnnestAggregate` | `src/executor/operator/physical_unnest_aggregate.cppm` | mapped | infinity_executor 的物理算子/管线 
| 156 | `PhysicalUpdate` | `src/executor/operator/physical_update.cppm` | mapped | infinity_executor 的物理算子/管线 
| 157 | `Snapshot` | `src/executor/operator/snapshot/snapshot.cppm` | mapped | infinity_executor 的物理算子/管线 
| 158 | `SnapshotBrief` | `src/executor/operator/snapshot/snapshot_brief.cppm` | mapped | infinity_executor 的物理算子/管线 
| 159 | `AggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 160 | `AggregateSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 161 | `AlterOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 162 | `CheckOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 163 | `CommandOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 164 | `CompactOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 165 | `CompactSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 166 | `CreateCollectionOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 167 | `CreateDatabaseOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 168 | `CreateIndexPrepareOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 169 | `CreateTableOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 170 | `CreateViewOperatorState` | `src/executor/operator_state.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 171 | `CrossProductOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 172 | `DeleteOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 173 | `DropCollectionOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 174 | `DropDatabaseOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 175 | `DropIndexOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 176 | `DropTableOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 177 | `DropViewOperatorState` | `src/executor/operator_state.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 178 | `EmptySourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 179 | `ExplainOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 180 | `ExportOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 181 | `FilterOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 182 | `FlushOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 183 | `FusionOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 184 | `HashAggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 185 | `HashJoinOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 186 | `HashOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 187 | `ImportOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 188 | `IndexJoinOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 189 | `IndexScanOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 190 | `IndexScanSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 191 | `InsertOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 192 | `KnnScanOperatorState` | `src/executor/operator_state.cppm` | mapped | CLASS_MAP 已显式命名 |
| 193 | `KnnScanSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 194 | `LimitOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 195 | `MatchOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 196 | `MatchSparseScanOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 197 | `MatchSparseScanSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 198 | `MatchTensorScanOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 199 | `MatchTensorScanSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 200 | `MaterializeSinkState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 201 | `MergeAggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 202 | `MergeHashAggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 203 | `MergeHashOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 204 | `MergeJoinOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 205 | `MergeKnnOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 206 | `MergeLimitOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 207 | `MergeMatchSparseOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 208 | `MergeMatchTensorOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 209 | `MergeParallelAggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 210 | `MergeSortOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 211 | `MergeTopOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 212 | `MessageSinkState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 213 | `NestedLoopOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 214 | `OperatorState` | `src/executor/operator_state.cppm` | mapped | CLASS_MAP 已显式命名 |
| 215 | `OptimizeOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 216 | `ParallelAggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 217 | `ProjectionOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 218 | `QueueSinkState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 219 | `QueueSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 220 | `ReadCacheState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 221 | `ResultSinkState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 222 | `ShowOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 223 | `SinkState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 224 | `SortOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 225 | `SourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 226 | `SummarySinkState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 227 | `TableScanOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 228 | `TableScanSourceState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 229 | `TopOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 230 | `UnionAllOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 231 | `UnnestAggregateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 232 | `UnnestOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 233 | `UpdateOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 234 | `OutputToDataBlockHelper` | `src/executor/physical_operator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 235 | `PhysicalCommonFunctionUsingLoadMeta` | `src/executor/physical_operator.cppm` | mapped | infinity_executor 的物理算子/管线 
| 236 | `PhysicalOperator` | `src/executor/physical_operator.cppm` | mapped | CLASS_MAP 已显式命名 |
| 237 | `PhysicalPlanner` | `src/executor/physical_planner.cppm` | mapped | infinity_executor 的物理算子/管线 
| 238 | `SourcePosition` | `src/expression/base_expression.cppm` | mapped | infinity_expression 
| 239 | `BetweenExpression` | `src/expression/between_expression.cppm` | mapped | infinity_expression 
| 240 | `CaseCheck` | `src/expression/case_expression.cppm` | mapped | infinity_expression 
| 241 | `ConjunctionExpression` | `src/expression/conjunction_expression.cppm` | mapped | infinity_expression 
| 242 | `FusionExpression` | `src/expression/fusion_expression.cppm` | mapped | infinity_expression 
| 243 | `KnnExpression` | `src/expression/knn_expression.cppm` | mapped | infinity_expression 
| 244 | `MatchExpression` | `src/expression/match_expression.cppm` | mapped | infinity_expression 
| 245 | `MatchSparseExpression` | `src/expression/match_sparse_expression.cppm` | mapped | infinity_expression 
| 246 | `MatchTensorExpression` | `src/expression/match_tensor_expression.cppm` | mapped | infinity_expression 
| 247 | `SearchExpression` | `src/expression/search_expression.cppm` | mapped | infinity_expression 
| 248 | `SubqueryExpression` | `src/expression/subquery_expression.cppm` | mapped | infinity_expression 
| 249 | `UnnestExpression` | `src/expression/unnest_expression.cppm` | mapped | infinity_expression 
| 250 | `AggregateFunction` | `src/function/aggregate_function.cppm` | mapped | infinity_expression 
| 251 | `AggregateFunctionSet` | `src/function/aggregate_function_set.cppm` | mapped | infinity_expression 
| 252 | `BuiltinFunctions` | `src/function/builtin_functions.cppm` | mapped | infinity_expression 
| 253 | `BitmapTryCastToVarlen` | `src/function/cast/bitmap_cast.cppm` | mapped | infinity_expression 
| 254 | `TryCastBoolean` | `src/function/cast/bool_cast.cppm` | mapped | infinity_expression 
| 255 | `BoundCastFunc` | `src/function/cast/bound_cast_func.cppm` | mapped | infinity_expression 
| 256 | `CastParameters` | `src/function/cast/bound_cast_func.cppm` | mapped | infinity_expression 
| 257 | `CastFunction` | `src/function/cast/cast_function.cppm` | mapped | infinity_expression 
| 258 | `CastTable` | `src/function/cast/cast_table.cppm` | mapped | infinity_expression 
| 259 | `ColumnVectorCast` | `src/function/cast/column_vector_cast.cppm` | mapped | infinity_expression 
| 260 | `DateTryCastToFixlen` | `src/function/cast/date_cast.cppm` | mapped | infinity_expression 
| 261 | `DateTryCastToVarlen` | `src/function/cast/date_cast.cppm` | mapped | infinity_expression 
| 262 | `DateTimeTryCastToFixlen` | `src/function/cast/datetime_cast.cppm` | mapped | infinity_expression 
| 263 | `DateTimeTryCastToVarlen` | `src/function/cast/datetime_cast.cppm` | mapped | infinity_expression 
| 264 | `DecimalTryCastToFixlen` | `src/function/cast/decimal_cast.cppm` | mapped | infinity_expression 
| 265 | `DecimalTryCastToVarlen` | `src/function/cast/decimal_cast.cppm` | mapped | infinity_expression 
| 266 | `EmbeddingTryCastToFixlen` | `src/function/cast/embedding_cast.cppm` | mapped | infinity_expression 
| 267 | `FloatTryCastToFixlen` | `src/function/cast/float_cast.cppm` | mapped | infinity_expression 
| 268 | `FloatTryCastToVarlen` | `src/function/cast/float_cast.cppm` | mapped | infinity_expression 
| 269 | `GeographyTryCastToVarlen` | `src/function/cast/geography_cast.cppm` | mapped | infinity_expression 
| 270 | `IntegerTryCastToFixlen` | `src/function/cast/integer_cast.cppm` | mapped | infinity_expression 
| 271 | `IntegerTryCastToVarlen` | `src/function/cast/integer_cast.cppm` | mapped | infinity_expression 
| 272 | `IntervalTryCastToVarlen` | `src/function/cast/interval_cast.cppm` | mapped | infinity_expression 
| 273 | `TimeTryCastToVarlen` | `src/function/cast/time_cast.cppm` | mapped | infinity_expression 
| 274 | `TimestampTryCastToFixlen` | `src/function/cast/timestamp_cast.cppm` | mapped | infinity_expression 
| 275 | `TimestampTryCastToVarlen` | `src/function/cast/timestamp_cast.cppm` | mapped | infinity_expression 
| 276 | `UuidTryCastToVarlen` | `src/function/cast/uuid_cast.cppm` | mapped | infinity_expression 
| 277 | `TryCastVarchar` | `src/function/cast/varchar_cast.cppm` | mapped | infinity_expression 
| 278 | `TryCastVarcharToChar` | `src/function/cast/varchar_cast.cppm` | mapped | infinity_expression 
| 279 | `TryCastVarcharToVarchar` | `src/function/cast/varchar_cast.cppm` | mapped | infinity_expression 
| 280 | `TryCastVarcharVector` | `src/function/cast/varchar_cast.cppm` | mapped | infinity_expression 
| 281 | `TryCastVarcharVectorToJson` | `src/function/cast/varchar_cast.cppm` | mapped | infinity_expression 
| 282 | `Function` | `src/function/function.cppm` | mapped | infinity_expression 
| 283 | `FunctionData` | `src/function/function_data.cppm` | mapped | infinity_expression 
| 284 | `FunctionSet` | `src/function/function_set.cppm` | mapped | infinity_expression 
| 285 | `EncodingAccumulator` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 286 | `EncodingPipeline` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 287 | `FixedDimensionalEncodingConfig` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 288 | `HashPartitioner` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 289 | `RandomGenerator` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 290 | `SparseProjector` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 291 | `Tensor` | `src/function/scalar/fixed_dimensional_encoding.cppm` | mapped | infinity_expression 
| 292 | `NewCatalog` | `src/function/scalar/modulo.cppm` | mapped | infinity_expression 
| 293 | `ScalarFunction` | `src/function/scalar_function.cppm` | mapped | `infinity_expression::scalar::ScalarFunction` 
| 294 | `ScalarFunctionSet` | `src/function/scalar_function_set.cppm` | mapped | `infinity_expression::scalar::ScalarFunctionSet` 
| 295 | `SpecialFunction` | `src/function/special_function.cppm` | mapped | infinity_expression 
| 296 | `CompactSegmentData` | `src/function/table/compact_state_data.cppm` | mapped | infinity_expression 
| 297 | `CompactStateData` | `src/function/table/compact_state_data.cppm` | mapped | infinity_expression 
| 298 | `RowIDRemap` | `src/function/table/compact_state_data.cppm` | mapped | infinity_expression 
| 299 | `CreateIndexSharedData` | `src/function/table/create_index_data.cppm` | mapped | infinity_expression 
| 300 | `AppendFilter` | `src/function/table/knn_filter.cppm` | mapped | infinity_expression 
| 301 | `BitmaskFilter` | `src/function/table/knn_filter.cppm` | mapped | infinity_expression 
| 302 | `KnnDistance1` | `src/function/table/knn_scan_data.cppm` | mapped | infinity_expression 
| 303 | `KnnDistanceBase1` | `src/function/table/knn_scan_data.cppm` | mapped | infinity_expression 
| 304 | `KnnScanFunctionData` | `src/function/table/knn_scan_data.cppm` | mapped | infinity_expression 
| 305 | `KnnScanSharedData` | `src/function/table/knn_scan_data.cppm` | mapped | CLASS_MAP 已显式命名 |
| 306 | `MatchSparseScanFunctionData` | `src/function/table/match_sparse_scan_data.cppm` | mapped | infinity_expression 
| 307 | `MergeSparseFunctionData` | `src/function/table/match_sparse_scan_data.cppm` | mapped | infinity_expression 
| 308 | `SparseBitDistance` | `src/function/table/match_sparse_scan_data.cppm` | mapped | infinity_expression 
| 309 | `SparseDistance` | `src/function/table/match_sparse_scan_data.cppm` | mapped | infinity_expression 
| 310 | `SparseDistanceBase` | `src/function/table/match_sparse_scan_data.cppm` | mapped | infinity_expression 
| 311 | `MatchTensorScanFunctionData` | `src/function/table/match_tensor_scan_data.cppm` | mapped | infinity_expression 
| 312 | `MergeKnnFunctionData` | `src/function/table/merge_knn_data.cppm` | mapped | infinity_expression 
| 313 | `TableScanFunctionData` | `src/function/table/table_scan_data.cppm` | mapped | infinity_expression 
| 314 | `TableFunctionData` | `src/function/table_function.cppm` | mapped | infinity_expression 
| 315 | `ClusterManager` | `src/main/cluster_manager.cppm` | mapped | infinity_server 的集群/成员管理 
| 316 | `Config` | `src/main/config.cppm` | mapped | `infinity_cli::config::Config`（`Init`/`Set`/访问器/校验） |
| 317 | `DefaultConfig` | `src/main/config.cppm` | mapped | `infinity_cli::config::Config::default` + `init(None)` |
| 318 | `Infinity` | `src/main/infinity.cppm` | mapped | infinity_server / infinity_cli 启动汇编 
| 319 | `InfinityContext` | `src/main/infinity_context.cppm` | mapped | CLASS_MAP 已显式命名 |
| 320 | `Logger` | `src/main/logger.cppm` | mapped | tracing 
| 321 | `LoggerConfig` | `src/main/logger.cppm` | mapped | tracing 
| 322 | `BaseOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 323 | `BooleanOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 324 | `FloatOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 325 | `FlushOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 326 | `GlobalOptions` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 327 | `IntegerOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 328 | `LogLevelOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 329 | `StringOption` | `src/main/options.cppm` | mapped | infinity_cli::config 
| 330 | `BaseProfiler` | `src/main/profiler.cppm` | mapped | `infinity_executor::profiler::BaseProfiler`（Begin/End/Elapsed/ElapsedToString/BeginTime/EndTime） |
| 331 | `OptimizerProfiler` | `src/main/profiler.cppm` | mapped | `profiler::OptimizerProfiler`（StartRule/StopRule/ToString） |
| 332 | `ProfileHistory` | `src/main/profiler.cppm` | mapped | `profiler::ProfileHistory`（Resize/Enqueue/GetElement/GetElements） |
| 333 | `QueryProfiler` | `src/main/profiler.cppm` | mapped | `profiler::QueryProfiler`（StartPhase/StopPhase/Stop/Flush/ToString/QueryPhaseToString/GetTaskProfile） |
| 334 | `TaskBinding` | `src/main/profiler.cppm` | mapped | `profiler::TaskBinding` |
| 335 | `TaskProfiler` | `src/main/profiler.cppm` | mapped | `profiler::TaskProfiler`（Begin/End/StartOperator/StopOperator） |
| 336 | `QueryContext` | `src/main/query_context.cppm` | mapped | infinity_server 的会话上下文 
| 337 | `AlterIndexOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 338 | `CreateDatabaseOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 339 | `CreateIndexOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 340 | `CreateTableOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 341 | `DropDatabaseOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 342 | `DropIndexOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 343 | `DropTableOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 344 | `ExportOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 345 | `ImportOptions` | `src/main/query_options.cppm` | mapped | infinity_parser 的 SearchOptions 
| 346 | `BaseResult` | `src/main/query_result.cppm` | mapped | infinity_executor::QueryResult 
| 347 | `QueryResult` | `src/main/query_result.cppm` | mapped | infinity_executor::QueryResult 
| 348 | `ResourceManager` | `src/main/resource_manager.cppm` | mapped | infinity_common 资源计费 + infinity_executor 
| 349 | `BaseSession` | `src/main/session.cppm` | mapped | infinity_server 的会话 
| 350 | `LocalSession` | `src/main/session.cppm` | mapped | infinity_server 的会话 
| 351 | `RemoteSession` | `src/main/session.cppm` | mapped | infinity_server 的会话 
| 352 | `QueryInfo` | `src/main/session_manager.cppm` | mapped | infinity_server 的会话 
| 353 | `SessionManager` | `src/main/session_manager.cppm` | mapped | infinity_server 的会话 
| 354 | `VarUtil` | `src/main/variables.cppm` | mapped | `infinity_executor::variables::{GlobalVariable, SessionVariable}::from_name` |
| 355 | `SystemInfo` | `src/metrics/system_info.cppm` | mapped | infinity_server 的 metrics 
| 356 | `BufferReader` | `src/network/buffer_reader.cppm` | mapped | infinity_server 
| 357 | `BufferWriter` | `src/network/buffer_writer.cppm` | mapped | infinity_server 
| 358 | `Connection` | `src/network/connection.cppm` | mapped | infinity_server 
| 359 | `HTTPSearch` | `src/network/http/http_search.cppm` | mapped | infinity_server 
| 360 | `HTTPServer` | `src/network/http_server.cppm` | mapped | infinity_server 
| 361 | `InfinityThriftService` | `src/network/infinity_thrift_service.cppm` | mapped | infinity_server 
| 362 | `NodeInfo` | `src/network/node_info.cppm` | mapped | infinity_server 
| 363 | `PeerServerThriftService` | `src/network/peer_server_thrift_service.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 364 | `ChangeRoleTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 365 | `HeartBeatPeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 366 | `PeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 367 | `RegisterPeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 368 | `SyncLogTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 369 | `TerminatePeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 370 | `UnregisterPeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 371 | `PeerClient` | `src/network/peer_thrift_client.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 372 | `PoolPeerThriftServer` | `src/network/peer_thrift_server.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 373 | `PGProtocolHandler` | `src/network/pg_protocol_handler.cppm` | mapped | `infinity_server::pg::PGProtocolHandler` / `PgServer`（有启动/命令/响应帧单测） 
| 374 | `PGServer` | `src/network/pg_server.cppm` | mapped | `infinity_server::pg::PGProtocolHandler` / `PgServer`（有启动/命令/响应帧单测） 
| 375 | `RingBufferIterator` | `src/network/ring_buffer_iterator.cppm` | mapped | `infinity_server::{peer,ring_buffer}`，经 `PeerManager` 挂到 `Server`；传输为进程内模拟（无 Thrift/TCP 线路，已注明） 
| 376 | `NonBlockPoolThriftServer` | `src/network/thrift_server.cppm` | mapped | infinity_server 
| 377 | `PoolThriftServer` | `src/network/thrift_server.cppm` | mapped | infinity_server 
| 378 | `ThreadedThriftServer` | `src/network/thrift_server.cppm` | mapped | infinity_server 
| 379 | `BindContext` | `src/planner/bind_context.cppm` | mapped | infinity_planner 
| 380 | `CommonTableExpressionInfo` | `src/planner/bind_context.cppm` | mapped | infinity_planner 
| 381 | `ExpressionBinder` | `src/planner/bind_context.cppm` | mapped | infinity_planner 
| 382 | `BindAliasProxy` | `src/planner/binder/bind_alias_proxy.cppm` | mapped | infinity_planner 
| 383 | `GroupBinder` | `src/planner/binder/group_binder.cppm` | mapped | infinity_planner 
| 384 | `HavingBinder` | `src/planner/binder/having_binder.cppm` | mapped | infinity_planner 
| 385 | `InsertBinder` | `src/planner/binder/insert_binder.cppm` | mapped | infinity_planner 
| 386 | `JoinBinder` | `src/planner/binder/join_binder.cppm` | mapped | infinity_planner 
| 387 | `LimitBinder` | `src/planner/binder/limit_binder.cppm` | mapped | infinity_planner 
| 388 | `OrderBinder` | `src/planner/binder/order_binder.cppm` | mapped | infinity_planner 
| 389 | `ProjectBinder` | `src/planner/binder/project_binder.cppm` | mapped | infinity_planner 
| 390 | `WhereBinder` | `src/planner/binder/where_binder.cppm` | mapped | infinity_planner 
| 391 | `Binding` | `src/planner/binding.cppm` | mapped | infinity_planner 
| 392 | `BaseTableRef` | `src/planner/bound/base_table_ref.cppm` | mapped | infinity_planner 
| 393 | `CrossProductTableRef` | `src/planner/bound/cross_product_table_ref.cppm` | mapped | infinity_planner 
| 394 | `JoinTableRef` | `src/planner/bound/join_table_ref.cppm` | mapped | infinity_planner 
| 395 | `SubqueryTableRef` | `src/planner/bound/subquery_table_ref.cppm` | mapped | infinity_planner 
| 396 | `TableRef` | `src/planner/bound/table_ref.cppm` | mapped | infinity_planner 
| 397 | `BoundCompactStatement` | `src/planner/bound_compact_statement.cppm` | mapped | infinity_planner 
| 398 | `BoundDeleteStatement` | `src/planner/bound_delete_statement.cppm` | mapped | infinity_planner 
| 399 | `BoundInsertStatement` | `src/planner/bound_insert_statement.cppm` | mapped | infinity_planner 
| 400 | `BoundSelectStatement` | `src/planner/bound_select_statement.cppm` | mapped | infinity_planner 
| 401 | `BoundStatement` | `src/planner/bound_statement.cppm` | mapped | infinity_planner 
| 402 | `BoundUpdateStatement` | `src/planner/bound_update_statement.cppm` | mapped | infinity_planner 
| 403 | `CachedIndexScan` | `src/planner/cached_node/cached_index_scan.cppm` | mapped | infinity_planner 
| 404 | `CachedMatch` | `src/planner/cached_node/cached_match.cppm` | mapped | infinity_planner 
| 405 | `CachedKnnScan` | `src/planner/cached_node/cached_match_scan.cppm` | mapped | infinity_planner 
| 406 | `CachedMatchScanBase` | `src/planner/cached_node/cached_match_scan.cppm` | mapped | infinity_planner 
| 407 | `CachedMatchSparseScan` | `src/planner/cached_node/cached_match_scan.cppm` | mapped | infinity_planner 
| 408 | `CachedMatchTensorScan` | `src/planner/cached_node/cached_match_scan.cppm` | mapped | infinity_planner 
| 409 | `CachedNodeBase` | `src/planner/cached_node/cached_node_base.cppm` | mapped | `infinity_storage::result_cache::CachedNode`（Hash/Eq/output_names/type） |
| 410 | `CachedScanBase` | `src/planner/cached_node/cached_scan_base.cppm` | mapped | `result_cache::CachedNode` 的 `schema_name`/`table_name` |
| 411 | `ColumnBinding` | `src/planner/column_binding.cppm` | mapped | infinity_planner 
| 412 | `ColumnIdentifier` | `src/planner/column_identifier.cppm` | mapped | infinity_planner 
| 413 | `ExplainAST` | `src/planner/explain_ast.cppm` | mapped | infinity_planner 
| 414 | `ExplainLogicalPlan` | `src/planner/explain_logical_plan.cppm` | mapped | infinity_planner 
| 415 | `LoadMeta` | `src/planner/load_meta.cppm` | mapped | infinity_planner 
| 416 | `LogicalCommonFunctionUsingLoadMeta` | `src/planner/logical_node.cppm` | mapped | infinity_planner 
| 417 | `LogicalNode` | `src/planner/logical_node.cppm` | mapped | infinity_planner 
| 418 | `LogicalNodeVisitor` | `src/planner/logical_node_visitor.cppm` | mapped | infinity_planner 
| 419 | `LogicalPlanner` | `src/planner/logical_planner.cppm` | mapped | infinity_planner 
| 420 | `LogicalAggregate` | `src/planner/node/logical_aggregate.cppm` | mapped | infinity_planner 
| 421 | `LogicalAddColumns` | `src/planner/node/logical_alter.cppm` | mapped | infinity_planner 
| 422 | `LogicalAlter` | `src/planner/node/logical_alter.cppm` | mapped | infinity_planner 
| 423 | `LogicalAlterIndex` | `src/planner/node/logical_alter.cppm` | mapped | infinity_planner 
| 424 | `LogicalDropColumns` | `src/planner/node/logical_alter.cppm` | mapped | infinity_planner 
| 425 | `LogicalRenameTable` | `src/planner/node/logical_alter.cppm` | mapped | infinity_planner 
| 426 | `LogicalCheck` | `src/planner/node/logical_check.cppm` | mapped | infinity_planner 
| 427 | `LogicalCommand` | `src/planner/node/logical_command.cppm` | mapped | infinity_planner 
| 428 | `LogicalCompact` | `src/planner/node/logical_compact.cppm` | mapped | infinity_planner 
| 429 | `LogicalCreateCollection` | `src/planner/node/logical_create_collection.cppm` | mapped | infinity_planner 
| 430 | `LogicalCreateIndex` | `src/planner/node/logical_create_index.cppm` | mapped | infinity_planner 
| 431 | `LogicalCreateSchema` | `src/planner/node/logical_create_schema.cppm` | mapped | infinity_planner 
| 432 | `LogicalCreateTable` | `src/planner/node/logical_create_table.cppm` | mapped | infinity_planner 
| 433 | `LogicalCreateView` | `src/planner/node/logical_create_view.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 434 | `LogicalCrossProduct` | `src/planner/node/logical_cross_product.cppm` | mapped | infinity_planner 
| 435 | `LogicalDelete` | `src/planner/node/logical_delete.cppm` | mapped | infinity_planner 
| 436 | `LogicalDropCollection` | `src/planner/node/logical_drop_collection.cppm` | mapped | infinity_planner 
| 437 | `LogicalDropIndex` | `src/planner/node/logical_drop_index.cppm` | mapped | infinity_planner 
| 438 | `LogicalDropSchema` | `src/planner/node/logical_drop_schema.cppm` | mapped | infinity_planner 
| 439 | `LogicalDropTable` | `src/planner/node/logical_drop_table.cppm` | mapped | infinity_planner 
| 440 | `LogicalDropView` | `src/planner/node/logical_drop_view.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 441 | `LogicalDummyScan` | `src/planner/node/logical_dummy_scan.cppm` | mapped | infinity_planner 
| 442 | `LogicalExplain` | `src/planner/node/logical_explain.cppm` | mapped | infinity_planner 
| 443 | `LogicalExport` | `src/planner/node/logical_export.cppm` | mapped | infinity_planner 
| 444 | `LogicalFilter` | `src/planner/node/logical_filter.cppm` | mapped | infinity_planner 
| 445 | `LogicalFlush` | `src/planner/node/logical_flush.cppm` | mapped | infinity_planner 
| 446 | `LogicalFusion` | `src/planner/node/logical_fusion.cppm` | mapped | infinity_planner 
| 447 | `LogicalHashAggregate` | `src/planner/node/logical_hash_aggregate.cppm` | mapped | infinity_planner 
| 448 | `LogicalImport` | `src/planner/node/logical_import.cppm` | mapped | infinity_planner 
| 449 | `LogicalIndexScan` | `src/planner/node/logical_index_scan.cppm` | mapped | infinity_planner 
| 450 | `LogicalInsert` | `src/planner/node/logical_insert.cppm` | mapped | infinity_planner 
| 451 | `LogicalJoin` | `src/planner/node/logical_join.cppm` | mapped | infinity_planner 
| 452 | `LogicalLimit` | `src/planner/node/logical_limit.cppm` | mapped | infinity_planner 
| 453 | `LogicalMatch` | `src/planner/node/logical_match.cppm` | mapped | infinity_planner 
| 454 | `LogicalKnnScan` | `src/planner/node/logical_match_scan/logical_knn_scan.cppm` | mapped | infinity_planner 
| 455 | `LogicalMatchScanBase` | `src/planner/node/logical_match_scan/logical_match_scan_base.cppm` | mapped | infinity_planner 
| 456 | `LogicalMatchSparseScan` | `src/planner/node/logical_match_scan/logical_match_sparse_scan.cppm` | mapped | infinity_planner 
| 457 | `LogicalMatchTensorScan` | `src/planner/node/logical_match_scan/logical_match_tensor_scan.cppm` | mapped | infinity_planner 
| 458 | `MatchTensorScanIndexOptions` | `src/planner/node/logical_match_scan/logical_match_tensor_scan.cppm` | mapped | infinity_planner 
| 459 | `LogicalMergeHashAggregate` | `src/planner/node/logical_merge_hash_aggregate.cppm` | mapped | infinity_planner 
| 460 | `LogicalOptimize` | `src/planner/node/logical_optimize.cppm` | mapped | infinity_planner 
| 461 | `LogicalProject` | `src/planner/node/logical_project.cppm` | mapped | infinity_planner 
| 462 | `CacheContent` | `src/planner/node/logical_read_cache.cppm` | mapped | `result_cache::CacheContent`（AppendColumns/Clone） |
| 463 | `LogicalReadCache` | `src/planner/node/logical_read_cache.cppm` | mapped | infinity_planner 
| 464 | `LogicalShow` | `src/planner/node/logical_show.cppm` | mapped | infinity_planner 
| 465 | `LogicalSort` | `src/planner/node/logical_sort.cppm` | mapped | infinity_planner 
| 466 | `LogicalTableScan` | `src/planner/node/logical_table_scan.cppm` | mapped | infinity_planner 
| 467 | `LogicalTop` | `src/planner/node/logical_top.cppm` | mapped | infinity_planner 
| 468 | `LogicalUnnest` | `src/planner/node/logical_unnest.cppm` | mapped | infinity_planner 
| 469 | `LogicalUnnestAggregate` | `src/planner/node/logical_unnest_aggregate.cppm` | mapped | infinity_planner 
| 470 | `LogicalUpdate` | `src/planner/node/logical_update.cppm` | mapped | infinity_planner 
| 471 | `LogicalViewScan` | `src/planner/node/logical_view_scan.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 472 | `Optimizer` | `src/planner/optimizer.cppm` | mapped | infinity_planner 
| 473 | `ApplyFastRoughFilter` | `src/planner/optimizer/apply_fast_rough_filter.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 474 | `ColumnPruner` | `src/planner/optimizer/column_pruner.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 475 | `ColumnRemapper` | `src/planner/optimizer/column_remapper.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 476 | `FilterExpressionPushDown` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 477 | `IndexFilterEvaluator` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 478 | `IndexScanFilterExpressionPushDownResult` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 479 | `FilterExpressionPushDownHelper` | `src/planner/optimizer/index_scan/filter_expression_push_down_helper.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 480 | `IndexFilterEvaluatorAllFalse` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 481 | `IndexFilterEvaluatorAllTrue` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 482 | `IndexFilterEvaluatorFulltext` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 483 | `IndexFilterEvaluatorLogicalChildren` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 484 | `IndexFilterEvaluatorSecondary` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 485 | `ExpressionInfoTree` | `src/planner/optimizer/index_scan/index_filter_expression_info_tree.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 486 | `IndexScanBuilder` | `src/planner/optimizer/index_scan_builder.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 487 | `LazyLoad` | `src/planner/optimizer/lazy_load.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 488 | `RegexIndexRewrite` | `src/planner/optimizer/regex_index_rewrite.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 489 | `ResultCacheGetter` | `src/planner/optimizer/result_cache_getter.cppm` | mapped | `infinity_planner::result_cache::ResultCacheGetter::rewrite` 在 `run_plan` 里最后挂上（`result_cache` 打开时生效），键是节点形状 + `begin_ts`（同参考的 `CacheKey`），命中替换成 `Values` 节点；另有语句级结果缓存（见 §4） 
| 490 | `OptimizerRule` | `src/planner/optimizer_rule.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 491 | `QueryBinder` | `src/planner/query_binder.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 492 | `CorrelatedExpressionsDetector` | `src/planner/subquery/correlated_expressions_detector.cppm` | mapped | 已接入 `Engine::run_select`/`plan_select`，有 plan-shape 测试；residual 谓词挂在 base scan 上，Gate A 全量通过 
| 493 | `DependentJoinFlattener` | `src/planner/subquery/dependent_join_flattener.cppm` | mapped | 计划侧保留并单测；执行侧由 `infinity-executor/src/dependent.rs` 按同一模型执行（相关列 = join key，内层按 distinct key 求值一次后拼回外层行）。参考自身没有执行算子（`JoinType::kMark` 只出现在 planner，`src/executor` 无对应实现，实测 C++ 直接 abort），因此这里的“mapped”指模型一致、执行由端口实现 
| 494 | `RewriteCorrelatedExpressions` | `src/planner/subquery/rewrite_correlated_expressions.cppm` | mapped | 同上：把 depth-1 列改写成可下推的列引用，端口对应“把嵌套查询改写成隐藏扫描列 `#_corr_n`” 
| 495 | `SubqueryUnnest` | `src/planner/subquery/subquery_unnest.cppm` | mapped | 同上：标量/IN/EXISTS 的 unnest 语义（mark 列 + 相关列 join）由 `dependent.rs` 执行；测试 `a_correlated_subquery_works_outside_the_where_clause`、`a_correlated_subquery_runs_once_per_distinct_key`、`a_correlated_subquery_reads_a_cte_or_joined_relation` 
| 496 | `FragmentContext` | `src/scheduler/fragment_context.cppm` | mapped | CLASS_MAP 已显式命名 |
| 497 | `Notifier` | `src/scheduler/fragment_context.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 498 | `ParallelMaterializedFragmentCtx` | `src/scheduler/fragment_context.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 499 | `ParallelStreamFragmentCtx` | `src/scheduler/fragment_context.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 500 | `SerialMaterializedFragmentCtx` | `src/scheduler/fragment_context.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 501 | `FragmentData` | `src/scheduler/fragment_data.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 502 | `FragmentDataBase` | `src/scheduler/fragment_data.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 503 | `FragmentError` | `src/scheduler/fragment_data.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 504 | `FragmentNone` | `src/scheduler/fragment_data.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 505 | `FragmentTask` | `src/scheduler/fragment_task.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 506 | `TaskResult` | `src/scheduler/task_result.cppm` | exempt | Tokio mpsc/JoinSet 调度器（§4.4 D4 设计变更） 
| 507 | `TaskScheduler` | `src/scheduler/task_scheduler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 508 | `BaseTable` | `src/storage/base_table.cppm` | mapped | infinity_executor 的 Engine 表 
| 509 | `BGTaskProcessor` | `src/storage/bg_task/background_process.cppm` | mapped | infinity_executor::bg_task 
| 510 | `AppendMemIndexBatch` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 511 | `AppendMemIndexTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 512 | `BGTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 513 | `BGTaskInfo` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 514 | `CheckpointTaskBase` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 515 | `CleanupTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 516 | `DumpMemIndexTask` | `src/storage/bg_task/bg_task.cppm` | mapped | CLASS_MAP 已显式命名 |
| 517 | `ManualCompactTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 518 | `ManualOptimizeTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 519 | `NewCheckpointTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 520 | `NotifyCompactTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 521 | `NotifyOptimizeTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 522 | `StopProcessorTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 523 | `TestCommandTask` | `src/storage/bg_task/bg_task.cppm` | mapped | infinity_executor::bg_task 
| 524 | `CompactionProcessor` | `src/storage/bg_task/compaction_process.cppm` | mapped | infinity_executor::bg_task 
| 525 | `DumpIndexProcessor` | `src/storage/bg_task/dump_index_process.cppm` | mapped | infinity_executor::bg_task 
| 526 | `MemIndexAppender` | `src/storage/bg_task/mem_index_appender.cppm` | mapped | infinity_executor::bg_task 
| 527 | `OptimizationProcessor` | `src/storage/bg_task/optimization_process.cppm` | mapped | infinity_executor::bg_task 
| 528 | `CheckpointPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | mapped | infinity_executor::bg_task 
| 529 | `CleanupPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | mapped | infinity_executor::bg_task 
| 530 | `CompactPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | mapped | infinity_executor::bg_task 
| 531 | `OptimizeIndexPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | mapped | infinity_executor::bg_task 
| 532 | `PeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | mapped | infinity_executor::bg_task 
| 533 | `PeriodicTriggerThread` | `src/storage/bg_task/periodic_trigger_thread.cppm` | mapped | infinity_executor::bg_task 
| 534 | `BuildFastRoughFilterTask` | `src/storage/bg_task/segment_sealing_tasks/build_fast_rough_filter_task.cppm` | mapped | infinity_executor::bg_task 
| 535 | `BuildingSegmentFastFilters` | `src/storage/bg_task/segment_sealing_tasks/build_fast_rough_filter_task.cppm` | mapped | infinity_executor::bg_task 
| 536 | `BufferHandle` | `src/storage/buffer/buffer_handle.cppm` | mapped | CLASS_MAP 已显式命名 |
| 537 | `BufferObj` | `src/storage/buffer/buffer_handle.cppm` | mapped | `infinity_storage::buffer::BufferObj` 
| 538 | `BufferManager` | `src/storage/buffer/buffer_manager.cppm` | mapped | CLASS_MAP 已显式命名 |
| 539 | `BufferObjectInfo` | `src/storage/buffer/buffer_obj.cppm` | mapped | infinity_storage::{buffer, io} 
| 540 | `VarBuffer` | `src/storage/buffer/buffer_obj.cppm` | mapped | `infinity_storage::var_buffer::VarBuffer`（Append/Get/Write/TotalSize/GetSize） |
| 541 | `BMPIndexFileWorker` | `src/storage/buffer/file_worker/bmp_index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 542 | `DataFileWorker` | `src/storage/buffer/file_worker/data_file_worker.cppm` | mapped | CLASS_MAP 已显式命名 |
| 543 | `EMVBIndexFileWorker` | `src/storage/buffer/file_worker/emvb_index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 544 | `FileWorker` | `src/storage/buffer/file_worker/file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 545 | `FileWorkerSaveCtx` | `src/storage/buffer/file_worker/file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 546 | `HnswFileWorker` | `src/storage/buffer/file_worker/hnsw_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 547 | `IndexFileWorker` | `src/storage/buffer/file_worker/index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 548 | `IVFIndexFileWorker` | `src/storage/buffer/file_worker/ivf_index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 549 | `PlaidIndexFileWorker` | `src/storage/buffer/file_worker/plaid_index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 550 | `RawFileWorker` | `src/storage/buffer/file_worker/raw_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 551 | `SecondaryIndexFileWorker` | `src/storage/buffer/file_worker/secondary_index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 552 | `SMVEIndexFileWorker` | `src/storage/buffer/file_worker/smve_index_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 553 | `VarFileWorker` | `src/storage/buffer/file_worker/var_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 554 | `VersionFileWorker` | `src/storage/buffer/file_worker/version_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 555 | `VersionFileWorkerSaveCtx` | `src/storage/buffer/file_worker/version_file_worker.cppm` | mapped | infinity_storage::{buffer, io} 
| 556 | `AppendPrepareInfo` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 557 | `DbCache` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 558 | `SegmentCache` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 559 | `SegmentIndexCache` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 560 | `SystemCache` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 561 | `TableCache` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 562 | `TableIndexCache` | `src/storage/catalog/catalog_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 563 | `KVInstance` | `src/storage/catalog/kv_store.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 564 | `KVIterator` | `src/storage/catalog/kv_store.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 565 | `KVStore` | `src/storage/catalog/kv_store.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 566 | `MemIndex` | `src/storage/catalog/mem_index.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 567 | `MemIndexID` | `src/storage/catalog/mem_index.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 568 | `BlockVersion` | `src/storage/catalog/meta/block_version.cppm` | mapped | `infinity_storage::mvcc::{BlockVersion, CreateField}`（Append/CommitAppend/Delete/RollbackDelete/CheckDelete/GetCreateTS/GetDeleteTS/SpillToFile/LoadFromFile） |
| 569 | `CatalogMeta` | `src/storage/catalog/meta/catalog_meta.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 570 | `ChunkIndexMetaInfo` | `src/storage/catalog/meta/chunk_index_meta.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 571 | `ChunkIndexSnapshotInfo` | `src/storage/catalog/meta/chunk_index_meta.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 572 | `BlockMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 573 | `BlockTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 574 | `ChunkIndexMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 575 | `ChunkIndexTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 576 | `ColumnMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 577 | `DBMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 578 | `DBTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 579 | `DropMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 580 | `MetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 581 | `PmObjectMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 582 | `PmStatMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 583 | `SegmentIndexMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 584 | `SegmentIndexTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 585 | `SegmentMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 586 | `SegmentTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 587 | `SystemTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 588 | `TableColumnMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 589 | `TableIndexMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 590 | `TableIndexTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 591 | `TableMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 592 | `TableNameMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 593 | `TableTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 594 | `MetaBlockColumnObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 595 | `MetaBlockObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 596 | `MetaChunkIndexObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 597 | `MetaDBObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 598 | `MetaObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 599 | `MetaPmObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 600 | `MetaSegmentIndexObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 601 | `MetaSegmentObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 602 | `MetaTableIndexObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 603 | `MetaTableObject` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 604 | `MetaTree` | `src/storage/catalog/meta/meta_tree.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 605 | `SegmentInfo` | `src/storage/catalog/meta/segment_meta.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 606 | `CacheInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 607 | `CacheStatus` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 608 | `DBCacheCommentInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 609 | `DBCacheInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 610 | `EraseBaseCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 611 | `IndexCacheIndexDefInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 612 | `IndexCacheInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 613 | `MetaCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 614 | `MetaDbCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 615 | `MetaEraseDbCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 616 | `MetaEraseIndexCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 617 | `MetaEraseTableCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 618 | `MetaIndexCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 619 | `MetaTableCache` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 620 | `TableCacheColumnInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 621 | `TableCacheIndexInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 622 | `TableCacheInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 623 | `TableCacheSegmentInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 624 | `TableCacheSegmentTagInfo` | `src/storage/catalog/meta_cache.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 625 | `BlockLock` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 626 | `BlockMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 627 | `ChunkIndexMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 628 | `ColumnMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 629 | `DBMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 630 | `NewTxnBlockVisitor` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 631 | `NewTxnGetVisibleRangeState` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 632 | `SegmentIndexFtInfo` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 633 | `SegmentIndexMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 634 | `SegmentMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 635 | `SegmentUpdateTS` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 636 | `TableIndexMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 637 | `TableLockForMemIndex` | `src/storage/catalog/new_catalog.cppm` | mapped | infinity_catalog + infinity_executor::Engine 
| 638 | `TableMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 639 | `Allocator` | `src/storage/column_vector/allocator.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 640 | `BinaryOperator` | `src/storage/column_vector/operator/binary_operator.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 641 | `EmbeddingUnaryOperator` | `src/storage/column_vector/operator/embedding_unary_operator.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 642 | `NullaryOperator` | `src/storage/column_vector/operator/nullary_operation.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 643 | `TernaryOperator` | `src/storage/column_vector/operator/ternary_operator.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 644 | `UnaryOperator` | `src/storage/column_vector/operator/unary_operator.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 645 | `Selection` | `src/storage/column_vector/selection.cppm` | mapped | `infinity_storage::selection::{Selection, SelectionData}`（Initialize/Set/Append/Get/operator[]/Capacity/Size/Reset） |
| 646 | `ArrayValueInfo` | `src/storage/column_vector/value.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 647 | `EmbeddingValueInfo` | `src/storage/column_vector/value.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 648 | `JsonValueInfo` | `src/storage/column_vector/value.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 649 | `SparseValueInfo` | `src/storage/column_vector/value.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 650 | `TensorArrayValueInfo` | `src/storage/column_vector/value.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 651 | `Value` | `src/storage/column_vector/value.cppm` | mapped | CLASS_MAP 已显式命名 |
| 652 | `VarBufferManager` | `src/storage/column_vector/var_buffer.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 653 | `VectorBuffer` | `src/storage/column_vector/vector_buffer.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 654 | `VectorHeapChunk` | `src/storage/column_vector/vector_heap_chunk.cppm` | mapped | infinity_storage::{data_block, mixed_column} 
| 655 | `BlockIndex` | `src/storage/common/block_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 656 | `IndexIndex` | `src/storage/common/block_index.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 657 | `NewIndexSnapshot` | `src/storage/common/block_index.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 658 | `NewSegmentSnapshot` | `src/storage/common/block_index.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 659 | `DatabaseDetail` | `src/storage/common/database_detail.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 660 | `FastPForWrapper` | `src/storage/common/fastpfor.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 661 | `GlobalBlockID` | `src/storage/common/global_block_id.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 662 | `GlobalBlockIDHash` | `src/storage/common/global_block_id.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 663 | `IntEncoder` | `src/storage/common/int_encoder.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 664 | `JsonManager` | `src/storage/common/json_manager.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 665 | `KeyBase` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 666 | `KeyDb` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 667 | `KeyDbTag` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 668 | `KeyEncode` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 669 | `KeyIndex` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 670 | `KeyIndexTag` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 671 | `KeyTable` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 672 | `KeyTableTag` | `src/storage/common/kv_code.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 673 | `BlockColumnInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 674 | `BlockInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 675 | `DatabaseInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 676 | `SegmentIndexInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 677 | `TableDetail` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 678 | `TableIndexInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 679 | `TableInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 680 | `ViewDetail` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 681 | `WalChunkIndexInfo` | `src/storage/common/meta_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 682 | `RcuMap` | `src/storage/common/rcu_multimap.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 683 | `RcuMultiMap` | `src/storage/common/rcu_multimap.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 684 | `String2UInt64AddOperator` | `src/storage/common/rocksdb_merge_operator.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 685 | `BlockColumnSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 686 | `BlockSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 687 | `DatabaseSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 688 | `OutlineSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 689 | `SegmentIndexSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 690 | `SegmentSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 691 | `SnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 692 | `SystemSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 693 | `TableIndexSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 694 | `TableSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 695 | `VByteCompressor` | `src/storage/common/vbyte_compressor.cppm` | mapped | infinity_storage::{catalog_format, wal_entry} + infinity_catalog 
| 696 | `NewCompactionAlg` | `src/storage/compaction/new_compaction_alg.cppm` | mapped | infinity_executor::compact_table 
| 697 | `DataBlock` | `src/storage/data_block.cppm` | mapped | CLASS_MAP 已显式命名 |
| 698 | `DataTable` | `src/storage/data_table.cppm` | mapped | `infinity_executor::data_table::DataTable` 
| 699 | `IndexBase` | `src/storage/definition/index_base.cppm` | mapped | CLASS_MAP 已显式命名 |
| 700 | `IndexBMP` | `src/storage/definition/index_bmp.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 701 | `IndexDiskAnn` | `src/storage/definition/index_diskann.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 702 | `IndexEMVB` | `src/storage/definition/index_emvb.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 703 | `IndexFullText` | `src/storage/definition/index_full_text.cppm` | mapped | CLASS_MAP 已显式命名 |
| 704 | `IndexHnsw` | `src/storage/definition/index_hnsw.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 705 | `LSGConfig` | `src/storage/definition/index_hnsw.cppm` | mapped | CLASS_MAP 已显式命名 |
| 706 | `IndexIVF` | `src/storage/definition/index_ivf.cppm` | mapped | `infinity_index::ivf_def::IndexIvf::{make,validate_column}`，由 `Engine::validate_index` 调用 
| 707 | `IndexIVFCentroidOption` | `src/storage/definition/index_ivf.cppm` | mapped | `infinity_index::ivf_def::CentroidOption`；质心数按 `min(sqrt(n)·ratio, n/min_points)` 在镜像时取 
| 708 | `IndexIVFOption` | `src/storage/definition/index_ivf.cppm` | mapped | `infinity_index::ivf_def::IvfOption`；`CREATE INDEX ... USING IVF` 的参数即经它解析。WAL 里它是 32 字节整块，参考把两段 padding 写成显式字段（`padding_after_metric_[3]`、`padding_after_storage_enums_[2]`），端口按同一布局写 0，见 `infinity-storage::wal::index_def::IndexDef::Ivf` 
| 709 | `IndexIVFStorageOption` | `src/storage/definition/index_ivf.cppm` | mapped | `infinity_index::ivf_def::StorageOption`；镜像按 `storage_type` 写 plain/SQ/PQ，检索见 `ivf::{IvfIndex,IvfSqIndex,IvfPqIndex}` 
| 710 | `IndexPLAID` | `src/storage/definition/index_plaid.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 711 | `IndexSecondary` | `src/storage/definition/index_secondary.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 712 | `IndexSecondaryFunctional` | `src/storage/definition/index_secondary_functional.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 713 | `IndexSMVE` | `src/storage/definition/index_smve.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 714 | `TableDef` | `src/storage/definition/table_def.cppm` | mapped | CLASS_MAP 已显式命名 |
| 715 | `BinaryFuse` | `src/storage/fast_rough_filter/binary_fuse_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 716 | `FastRoughFilter` | `src/storage/fast_rough_filter/fast_rough_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 717 | `FastRoughFilterEvaluator` | `src/storage/fast_rough_filter/fast_rough_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 718 | `Overload` | `src/storage/fast_rough_filter/filter_value_type_classification.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 719 | `TypeList` | `src/storage/fast_rough_filter/filter_value_type_classification.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 720 | `InnerMinMaxDataFilterInfo` | `src/storage/fast_rough_filter/min_max_data_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 721 | `InnerMinMaxDataFilterVarcharType` | `src/storage/fast_rough_filter/min_max_data_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 722 | `MinMaxDataFilter` | `src/storage/fast_rough_filter/min_max_data_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 723 | `ProbabilisticDataFilter` | `src/storage/fast_rough_filter/probabilistic_data_filter.cppm` | mapped | infinity_storage 的 bloom/rough-filter 路径 
| 724 | `ColumnIndexIterator` | `src/storage/invertedindex/column_index_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 725 | `ColumnIndexMerger` | `src/storage/invertedindex/column_index_merger.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 726 | `ColumnIndexReader` | `src/storage/invertedindex/column_index_reader.cppm` | mapped | CLASS_MAP 已显式命名 |
| 727 | `IndexReader` | `src/storage/invertedindex/column_index_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 728 | `TableIndexReaderCache` | `src/storage/invertedindex/column_index_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 729 | `TermDocIterator` | `src/storage/invertedindex/column_index_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 730 | `ColumnInverter` | `src/storage/invertedindex/column_inverter.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 731 | `ColumnInverterJsonArray` | `src/storage/invertedindex/column_inverter_json_array.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 732 | `AhoCorasick` | `src/storage/invertedindex/common/aho_corasick.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 733 | `BufWriter` | `src/storage/invertedindex/common/buf_writer.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 734 | `DirectIO` | `src/storage/invertedindex/common/external_sort_merger.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 735 | `SortMerger` | `src/storage/invertedindex/common/external_sort_merger.cppm` | mapped | `infinity_storage::external_sort::SortMerger` 
| 736 | `TermTuple` | `src/storage/invertedindex/common/external_sort_merger.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 737 | `TermTupleList` | `src/storage/invertedindex/common/external_sort_merger.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 738 | `MapWithLock` | `src/storage/invertedindex/common/map_with_lock.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 739 | `MmapReader` | `src/storage/invertedindex/common/mmap.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 740 | `PriorityQueue` | `src/storage/invertedindex/common/priority_queue.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 741 | `Arena` | `src/storage/invertedindex/common/skiplist.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 742 | `KeyComparator` | `src/storage/invertedindex/common/skiplist.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 743 | `SkipList` | `src/storage/invertedindex/common/skiplist.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 744 | `SkipListNode` | `src/storage/invertedindex/common/skiplist.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 745 | `VectorWithLock` | `src/storage/invertedindex/common/vector_with_lock.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 746 | `DictionaryReader` | `src/storage/invertedindex/dict_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 747 | `DiskIndexSegmentReader` | `src/storage/invertedindex/disk_segment_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 748 | `DocListEncoder` | `src/storage/invertedindex/format/doc_list_encoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 749 | `DocListFormat` | `src/storage/invertedindex/format/doc_list_format_option.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 750 | `DocListFormatOption` | `src/storage/invertedindex/format/doc_list_format_option.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 751 | `DocSkipListFormat` | `src/storage/invertedindex/format/doc_list_format_option.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 752 | `FlushInfo` | `src/storage/invertedindex/format/flush_info.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 753 | `InDocPositionState` | `src/storage/invertedindex/format/in_doc_pos_state.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 754 | `IndexDecoder` | `src/storage/invertedindex/format/index_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 755 | `SkipIndexDecoder` | `src/storage/invertedindex/format/index_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 756 | `InMemDocListDecoder` | `src/storage/invertedindex/format/inmem_doc_list_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 757 | `InMemPositionListDecoder` | `src/storage/invertedindex/format/inmem_position_list_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 758 | `InMemPostingDecoder` | `src/storage/invertedindex/format/inmem_posting_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 759 | `NoCompressIntEncoder` | `src/storage/invertedindex/format/no_compress_encoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 760 | `PositionListDecoder` | `src/storage/invertedindex/format/position_list_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 761 | `PositionListEncoder` | `src/storage/invertedindex/format/position_list_encoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 762 | `PositionListFormat` | `src/storage/invertedindex/format/position_list_format_option.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 763 | `PositionListFormatOption` | `src/storage/invertedindex/format/position_list_format_option.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 764 | `PositionSkipListFormat` | `src/storage/invertedindex/format/position_list_format_option.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 765 | `PostingBuffer` | `src/storage/invertedindex/format/posting_buffer.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 766 | `PostingByteSlice` | `src/storage/invertedindex/format/posting_byte_slice.cppm` | mapped | CLASS_MAP 已显式命名 |
| 767 | `PostingByteSliceReader` | `src/storage/invertedindex/format/posting_byte_slice_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 768 | `PostingDecoder` | `src/storage/invertedindex/format/posting_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 769 | `NoCompressPostingField` | `src/storage/invertedindex/format/posting_field.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 770 | `PostingField` | `src/storage/invertedindex/format/posting_field.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 771 | `PostingFields` | `src/storage/invertedindex/format/posting_field.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 772 | `TypedPostingField` | `src/storage/invertedindex/format/posting_field.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 773 | `VByteCompressPostingField` | `src/storage/invertedindex/format/posting_field.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 774 | `PostingFormat` | `src/storage/invertedindex/format/posting_list_format.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 775 | `PostingFormatOption` | `src/storage/invertedindex/format/posting_list_format.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 776 | `ShortListOptimizeUtil` | `src/storage/invertedindex/format/short_list_optimize_util.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 777 | `SkipListReader` | `src/storage/invertedindex/format/skiplist_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 778 | `SkipListReaderByteSlice` | `src/storage/invertedindex/format/skiplist_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 779 | `SkipListReaderPostingByteSlice` | `src/storage/invertedindex/format/skiplist_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 780 | `SkipListWriter` | `src/storage/invertedindex/format/skiplist_writer.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 781 | `TermMeta` | `src/storage/invertedindex/format/term_meta.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 782 | `TermMetaDumper` | `src/storage/invertedindex/format/term_meta.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 783 | `TermMetaLoader` | `src/storage/invertedindex/format/term_meta.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 784 | `VByteIntEncoder` | `src/storage/invertedindex/format/vbyte_compress_encoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 785 | `FstBuilder` | `src/storage/invertedindex/fst/build.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 786 | `Bound` | `src/storage/invertedindex/fst/fst.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 787 | `Fst` | `src/storage/invertedindex/fst/fst.cppm` | mapped | CLASS_MAP 已显式命名 |
| 788 | `FstStream` | `src/storage/invertedindex/fst/fst.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 789 | `Output` | `src/storage/invertedindex/fst/node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 790 | `Registry` | `src/storage/invertedindex/fst/registry.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 791 | `RegistryEntry` | `src/storage/invertedindex/fst/registry.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 792 | `InDocPositionIterator` | `src/storage/invertedindex/in_doc_pos_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 793 | `InDocStateKeeper` | `src/storage/invertedindex/in_doc_state_keeper.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 794 | `IndexSegmentReader` | `src/storage/invertedindex/index_segment_reader.cppm` | mapped | CLASS_MAP 已显式命名 |
| 795 | `InMemIndexSegmentReader` | `src/storage/invertedindex/inmem_segment_reader.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 796 | `BatchInvertTask` | `src/storage/invertedindex/invert_task.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 797 | `MemoryIndexer` | `src/storage/invertedindex/memory_indexer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 798 | `MultiPostingDecoder` | `src/storage/invertedindex/multi_posting_decoder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 799 | `PostingIterator` | `src/storage/invertedindex/posting_iterator.cppm` | mapped | `infinity_storage::fulltext_segment::PostingIterator` 
| 800 | `PostingMerger` | `src/storage/invertedindex/posting_merger.cppm` | mapped | `infinity_storage::fulltext_segment::PostingMerger` 
| 801 | `PostingWriter` | `src/storage/invertedindex/posting_writer.cppm` | mapped | `infinity_storage::fulltext_segment::PostingWriter` 
| 802 | `Ring` | `src/storage/invertedindex/ring.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 803 | `AndIterator` | `src/storage/invertedindex/search/and_iterator.cppm` | mapped | `infinity_executor::query_builder::AndIterator` 
| 804 | `AndNotIterator` | `src/storage/invertedindex/search/and_not_iterator.cppm` | mapped | `infinity_executor::query_builder::AndNotIterator` 
| 805 | `BatchOrIterator` | `src/storage/invertedindex/search/batch_or_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 806 | `BlockMaxLeafIterator` | `src/storage/invertedindex/search/blockmax_leaf_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 807 | `BlockMaxWandIterator` | `src/storage/invertedindex/search/blockmax_wand_iterator.cppm` | mapped | CLASS_MAP 已显式命名 |
| 808 | `BM25Ranker` | `src/storage/invertedindex/search/bm25_ranker.cppm` | mapped | CLASS_MAP 已显式命名 |
| 809 | `FullTextColumnLengthReader` | `src/storage/invertedindex/search/column_length_io.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 810 | `DocIterator` | `src/storage/invertedindex/search/doc_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 811 | `DocIteratorEstimateIterateCost` | `src/storage/invertedindex/search/doc_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 812 | `FilterIterator` | `src/storage/invertedindex/search/filter_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 813 | `FilterQueryNode` | `src/storage/invertedindex/search/filter_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 814 | `HighlightInfo` | `src/storage/invertedindex/search/highlighter.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 815 | `Highlighter` | `src/storage/invertedindex/search/highlighter.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 816 | `KeywordIterator` | `src/storage/invertedindex/search/keyword_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 817 | `MinimumShouldMatchIterator` | `src/storage/invertedindex/search/minimum_should_match_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 818 | `MultiDocIterator` | `src/storage/invertedindex/search/multi_doc_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 819 | `MustFirstIterator` | `src/storage/invertedindex/search/must_first_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 820 | `DocIteratorEntry` | `src/storage/invertedindex/search/or_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 821 | `DocIteratorHeap` | `src/storage/invertedindex/search/or_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 822 | `OrIterator` | `src/storage/invertedindex/search/or_iterator.cppm` | mapped | `infinity_executor::query_builder::OrIterator` 
| 823 | `BM25Params` | `src/storage/invertedindex/search/parse_fulltext_options.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 824 | `RankFeatureOption` | `src/storage/invertedindex/search/parse_fulltext_options.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 825 | `PhraseDocIterator` | `src/storage/invertedindex/search/phrase_doc_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 826 | `FullTextQueryContext` | `src/storage/invertedindex/search/query_builder.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 827 | `QueryBuilder` | `src/storage/invertedindex/search/query_builder.cppm` | mapped | `infinity_executor::query_builder::QueryBuilder` 
| 828 | `AndNotQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 829 | `AndQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 830 | `CreateSearchParams` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 831 | `KeywordQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 832 | `MultiQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 833 | `NotQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 834 | `OrQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 835 | `PhraseQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | `infinity_executor::query_builder::PhraseQueryNode` 
| 836 | `QueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 837 | `RankFeatureQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 838 | `RankFeaturesQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 839 | `TermQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 840 | `RankFeatureDocIterator` | `src/storage/invertedindex/search/rank_feature_doc_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 841 | `RankFeaturesDocIterator` | `src/storage/invertedindex/search/rank_features_doc_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 842 | `FullTextScoreResultHeap` | `src/storage/invertedindex/search/score_result_heap.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 843 | `ScoreThresholdIterator` | `src/storage/invertedindex/search/score_threshold_iterator.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 844 | `SearchDriver` | `src/storage/invertedindex/search/search_driver.cppm` | mapped | `infinity_executor::fulltext_query::SearchDriver` 
| 845 | `SegmentPosting` | `src/storage/invertedindex/segment_posting.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 846 | `SegmentTermPosting` | `src/storage/invertedindex/segment_term_posting.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 847 | `SegmentTermPostingQueue` | `src/storage/invertedindex/segment_term_posting.cppm` | mapped | infinity_storage::inverted_format / fulltext_segment + infinity_executor::fulltext_query 
| 848 | `ByteSliceReader` | `src/storage/io/byte_slice_reader.cppm` | mapped | `infinity_storage::io::ByteSliceReader` 
| 849 | `ByteSliceWriter` | `src/storage/io/byte_slice_writer.cppm` | mapped | `infinity_storage::io::ByteSliceWriter` 
| 850 | `FileReader` | `src/storage/io/file_reader.cppm` | mapped | `infinity_storage::io::FileReader` 
| 851 | `FileWriter` | `src/storage/io/file_writer.cppm` | mapped | `infinity_storage::io::FileWriter` 
| 852 | `LocalFileHandle` | `src/storage/io/local_file_handle.cppm` | mapped | infinity_storage::{io, s3} 
| 853 | `ObjectStorageProcess` | `src/storage/io/object_storage_process.cppm` | mapped | infinity_storage::{io, s3} 
| 854 | `BaseObjectStorageTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 855 | `CopyTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 856 | `DownloadTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 857 | `LocalDropTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 858 | `RemoveTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 859 | `StopObjectStorageProcessTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 860 | `UploadTask` | `src/storage/io/object_storage_task.cppm` | mapped | infinity_storage::{io, s3} 
| 861 | `S3Client` | `src/storage/io/s3_client.cppm` | mapped | CLASS_MAP 已显式命名 |
| 862 | `S3ClientMinio` | `src/storage/io/s3_client_minio.cppm` | mapped | infinity_storage::{io, s3} 
| 863 | `StreamReader` | `src/storage/io/stream_reader.cppm` | mapped | `infinity_storage::io::StreamReader` 
| 864 | `MmapInfo` | `src/storage/io/virtual_store.cppm` | mapped | infinity_storage::{io, s3} 
| 865 | `VirtualStore` | `src/storage/io/virtual_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 866 | `FlattenedTerm` | `src/storage/json_index/json_flattener.cppm` | mapped | infinity_storage::json_index 路径 
| 867 | `JsonFlattener` | `src/storage/json_index/json_flattener.cppm` | mapped | infinity_storage::json_index 路径 
| 868 | `EMVBIndex` | `src/storage/knn_index/emvb/emvb_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 869 | `EMVBIndexInMem` | `src/storage/knn_index/emvb/emvb_index_in_mem.cppm` | mapped | CLASS_MAP 已显式命名 |
| 870 | `EMVBSearch` | `src/storage/knn_index/emvb/emvb_search.cppm` | mapped | CLASS_MAP 已显式命名 |
| 871 | `EMVBSharedVec` | `src/storage/knn_index/emvb/emvb_shared_vec.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 872 | `EMVBProductQuantizer` | `src/storage/knn_index/emvb/product_quantizer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 873 | `OPQ` | `src/storage/knn_index/emvb/product_quantizer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 874 | `PQ` | `src/storage/knn_index/emvb/product_quantizer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 875 | `DiskAnnDistance` | `src/storage/knn_index/knn_diskann/diskann_dist_func.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 876 | `DiskAnnIndexData` | `src/storage/knn_index/knn_diskann/diskann_index_data.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 877 | `FixedChunkPQTable` | `src/storage/knn_index/knn_diskann/diskann_partition_and_pq.cppm` | mapped | CLASS_MAP 已显式命名 |
| 878 | `DiskAnnMemGraphStore` | `src/storage/knn_index/knn_diskann/inner/diskann_mem_graph_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 879 | `AbstractScratch` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 880 | `AlignedFileReader` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 881 | `AlignedRead` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 882 | `ConcurrentQueue` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 883 | `InMemQueryScratch` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 884 | `Neighbor` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 885 | `NeighborPriorityQueue` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | mapped | CLASS_MAP 已显式命名 |
| 886 | `PQScratch` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 887 | `QueryStats` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 888 | `ScratchStoreManager` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 889 | `SsdQueryScratch` | `src/storage/knn_index/knn_diskann/inner/diskann_utils.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 890 | `KnnDiskAnn` | `src/storage/knn_index/knn_diskann/knn_diskann.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 891 | `PqFlashIndex` | `src/storage/knn_index/knn_diskann/pq_flash_index.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 892 | `MemVamana` | `src/storage/knn_index/knn_diskann/vamana_alg.cppm` | excluded | 无：计划 §2.6/R14 排除（C++ populate 为 stub） |
| 893 | `KnnDistance` | `src/storage/knn_index/knn_distance.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 894 | `KnnDistanceBase` | `src/storage/knn_index/knn_distance.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 895 | `KnnFlatCOS` | `src/storage/knn_index/knn_flat/knn_flat_cos.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 896 | `KnnFlatIP` | `src/storage/knn_index/knn_flat/knn_flat_ip.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 897 | `KnnFlatIPBlas` | `src/storage/knn_index/knn_flat/knn_flat_ip_blas.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 898 | `KnnFlatIPBlasReservoir` | `src/storage/knn_index/knn_flat/knn_flat_ip_blas_reservoir.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 899 | `KnnFlatIPReservoir` | `src/storage/knn_index/knn_flat/knn_flat_ip_reservoir.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 900 | `KnnFlatL2` | `src/storage/knn_index/knn_flat/knn_flat_l2.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 901 | `KnnFlatL2Blas` | `src/storage/knn_index/knn_flat/knn_flat_l2_blas.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 902 | `KnnFlatL2BlasReservoir` | `src/storage/knn_index/knn_flat/knn_flat_l2_blas_reservoir.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 903 | `KnnFlatL2Reservoir` | `src/storage/knn_index/knn_flat/knn_flat_l2_reservoir.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 904 | `KnnFlatL2Top1` | `src/storage/knn_index/knn_flat/knn_flat_l2_top1.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 905 | `KnnFlatL2Top1Blas` | `src/storage/knn_index/knn_flat/knn_flat_l2_top1_blas.cppm` | mapped | `infinity_index::knn_flat::{top_k, top_1}`（按 metric 排序，mask 过滤） |
| 906 | `DataStore` | `src/storage/knn_index/knn_hnsw/data_store/data_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 907 | `DataStoreBase` | `src/storage/knn_index/knn_hnsw/data_store/data_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 908 | `DataStoreChunkIter` | `src/storage/knn_index/knn_hnsw/data_store/data_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 909 | `ArrayPtr` | `src/storage/knn_index/knn_hnsw/data_store/data_store_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 910 | `PPtr` | `src/storage/knn_index/knn_hnsw/data_store/data_store_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 911 | `GraphStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/graph_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 912 | `GraphStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/graph_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 913 | `LVQData` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 914 | `LVQVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 915 | `LVQVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 916 | `LVQVecStoreMetaType` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 917 | `PlainVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/plain_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 918 | `PlainVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/plain_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 919 | `RabitqQueryData` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 920 | `RabitqStoreData` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 921 | `RabitqVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 922 | `RabitqVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 923 | `RabitqVecStoreMetaType` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 924 | `SparseVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/sparse_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 925 | `SparseVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/sparse_vec_store.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 926 | `LVQCosVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 927 | `LVQIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 928 | `LVQL2VecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 929 | `PlainCosVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 930 | `PlainIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 931 | `PlainL2VecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 932 | `RabitqCosVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 933 | `RabitqIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 934 | `RabitqL2VecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 935 | `SparseIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 936 | `LVQCosCache` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 937 | `LVQCosDist` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 938 | `PlainCosDist` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 939 | `RabitqCosDist` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 940 | `LVQIPCache` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 941 | `LVQIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 942 | `PlainIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 943 | `RabitqIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 944 | `LVQL2Cache` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 945 | `LVQL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 946 | `PlainL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 947 | `PlainLSL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 948 | `RabitqL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 949 | `LSGDistWrapper` | `src/storage/knn_index/knn_hnsw/dist_func_lsg_wrapper.cppm` | mapped | CLASS_MAP 已显式命名 |
| 950 | `SparseIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_sparse_ip.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 951 | `KnnHnsw` | `src/storage/knn_index/knn_hnsw/hnsw_alg.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 952 | `KnnHnswBase` | `src/storage/knn_index/knn_hnsw/hnsw_alg.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 953 | `KnnSearchOption` | `src/storage/knn_index/knn_hnsw/hnsw_alg.cppm` | mapped | CLASS_MAP 已显式命名 |
| 954 | `DenseVectorIter` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 955 | `FilterBase` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 956 | `HnswConfig` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 957 | `HnswInsertConfig` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 958 | `SparseVectorIter` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 959 | `HnswHandler` | `src/storage/knn_index/knn_hnsw/hnsw_handler.cppm` | mapped | `infinity_index::hnsw_handler::HnswHandler` 
| 960 | `HnswIndexInMem` | `src/storage/knn_index/knn_hnsw/hnsw_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 961 | `HnswLSGBuilder` | `src/storage/knn_index/knn_hnsw/hnsw_lsg_builder.cppm` | mapped | CLASS_MAP 已显式命名 |
| 962 | `HnswOptimizeOptions` | `src/storage/knn_index/knn_hnsw/hnsw_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 963 | `HnswUtil` | `src/storage/knn_index/knn_hnsw/hnsw_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 964 | `heap_twin` | `src/storage/knn_index/knn_ivf/heap_twin_operation.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 965 | `heap_twin_multiple` | `src/storage/knn_index/knn_ivf/heap_twin_operation.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 966 | `IVFDataAccessorBase` | `src/storage/knn_index/knn_ivf/ivf_index_data.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 967 | `IVFIndexInChunk` | `src/storage/knn_index/knn_ivf/ivf_index_data.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 968 | `IVFIndexInMem` | `src/storage/knn_index/knn_ivf/ivf_index_data_in_mem.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 969 | `IVF_Search_Handler` | `src/storage/knn_index/knn_ivf/ivf_index_search.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 970 | `IVF_Search_Params` | `src/storage/knn_index/knn_ivf/ivf_index_search.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 971 | `IVF_Index_Storage` | `src/storage/knn_index/knn_ivf/ivf_index_storage.cppm` | mapped | CLASS_MAP 已显式命名 |
| 972 | `MergeKnnBase` | `src/storage/knn_index/merge_knn.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 973 | `MergeKnnResultHandler` | `src/storage/knn_index/merge_knn.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 974 | `MultiVectorResultHandler` | `src/storage/knn_index/multivector/multivector_result_handler.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 975 | `MultiVectorRef` | `src/storage/knn_index/multivector/multivector_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 976 | `PlaidGlobalCentroids` | `src/storage/knn_index/plaid/plaid_global_centroids.cppm` | mapped | CLASS_MAP 已显式命名 |
| 977 | `PlaidIndex` | `src/storage/knn_index/plaid/plaid_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 978 | `PlaidIndexDiskMerger` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | mapped | CLASS_MAP 已显式命名 |
| 979 | `PlaidMergeManifest` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | mapped | CLASS_MAP 已显式命名 |
| 980 | `PlaidMergeManifestChunk` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 981 | `PlaidMergeSizeInfo` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 982 | `PlaidIndexInMem` | `src/storage/knn_index/plaid/plaid_index_in_mem.cppm` | mapped | CLASS_MAP 已显式命名 |
| 983 | `PlaidQuantizer` | `src/storage/knn_index/plaid/plaid_quantizer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 984 | `CompareMax` | `src/storage/knn_index/result_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 985 | `CompareMin` | `src/storage/knn_index/result_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 986 | `HeapResultHandler` | `src/storage/knn_index/result_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 987 | `ReservoirResultHandler` | `src/storage/knn_index/result_handler.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 988 | `SingleBestResultHandler` | `src/storage/knn_index/result_handler.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 989 | `SMVEBMPCollector` | `src/storage/knn_index/smve/smve_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 990 | `SMVEIndexInMem` | `src/storage/knn_index/smve/smve_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 991 | `SMVESerializedData` | `src/storage/knn_index/smve/smve_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 992 | `SMVEResult` | `src/storage/knn_index/smve/smve_transform.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 993 | `BMPAlg` | `src/storage/knn_index/sparse/bmp_alg.cppm` | mapped | CLASS_MAP 已显式命名 |
| 994 | `BMPAlgBase` | `src/storage/knn_index/sparse/bmp_alg.cppm` | mapped | CLASS_MAP 已显式命名 |
| 995 | `BlockFwd` | `src/storage/knn_index/sparse/bmp_fwd.cppm` | mapped | CLASS_MAP 已显式命名 |
| 996 | `BlockTerms` | `src/storage/knn_index/sparse/bmp_fwd.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 997 | `TailFwd` | `src/storage/knn_index/sparse/bmp_fwd.cppm` | mapped | CLASS_MAP 已显式命名 |
| 998 | `BMPHandler` | `src/storage/knn_index/sparse/bmp_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 999 | `BMPIndexInMem` | `src/storage/knn_index/sparse/bmp_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1000 | `BMPIvt` | `src/storage/knn_index/sparse/bmp_ivt.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1001 | `BlockData` | `src/storage/knn_index/sparse/bmp_ivt.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1002 | `BMPOptimizeOptions` | `src/storage/knn_index/sparse/bmp_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1003 | `BMPUtil` | `src/storage/knn_index/sparse/bmp_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1004 | `BmpSearchOptions` | `src/storage/knn_index/sparse/bmp_util.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1005 | `VecPtr` | `src/storage/knn_index/sparse/bmp_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1006 | `BPReordering` | `src/storage/knn_index/sparse/bp_reordering.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1007 | `LinScan` | `src/storage/knn_index/sparse/linscan_alg.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1008 | `SparseTestUtil` | `src/storage/knn_index/sparse/sparse_test_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1009 | `SparseMatrix` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1010 | `SparseMatrixIter` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1011 | `SparseVec` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1012 | `SparseVecEle` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1013 | `SparseVecRef` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1014 | `SparseVecUtil` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | infinity_index::{vector, hnsw_handler, sparse, plaid, smve, ivf_def, knn_flat} 
| 1015 | `AccessState` | `src/storage/meta/entry/data_access_state.cppm` | mapped | infinity_catalog 
| 1016 | `DeleteState` | `src/storage/meta/entry/data_access_state.cppm` | mapped | infinity_catalog 
| 1017 | `MemIndexInserterIter1` | `src/storage/meta/iter/block_column_iter.cppm` | mapped | infinity_catalog 
| 1018 | `AddColumnsTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1019 | `AlterIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1020 | `AppendTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1021 | `BaseTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1022 | `CheckpointTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1023 | `CleanupTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1024 | `CompactTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1025 | `CreateDBSnapshotTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1026 | `CreateDBTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1027 | `CreateIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1028 | `CreateSystemSnapshotTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1029 | `CreateTableSnapshotTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1030 | `CreateTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1031 | `DeleteTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1032 | `DropColumnsTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1033 | `DropDBTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1034 | `DropIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1035 | `DropTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1036 | `DummyTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1037 | `DumpMemIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1038 | `FlushDataEntry` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1039 | `ImportTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1040 | `MemIndexRange` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1041 | `OptimizeIndexStoreEntry` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1042 | `OptimizeIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1043 | `RenameTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1044 | `RestoreDatabaseTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1045 | `RestoreSystemTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1046 | `RestoreTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1047 | `UpdateAppendBlock` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1048 | `UpdateTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1049 | `CheckpointOption` | `src/storage/new_txn/new_txn.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1050 | `ChunkInfoForCreateIndex` | `src/storage/new_txn/new_txn.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1051 | `NewTxn` | `src/storage/new_txn/new_txn.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1052 | `SnapshotOption` | `src/storage/new_txn/new_txn.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1053 | `NewTxnManager` | `src/storage/new_txn/new_txn_manager.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1054 | `NewTxnStore` | `src/storage/new_txn/new_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1055 | `NewTxnTableStore1` | `src/storage/new_txn/new_txn_store.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1056 | `TxnAllocator` | `src/storage/new_txn/txn_allocator.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1057 | `TxnAllocatorTask` | `src/storage/new_txn/txn_allocator_task.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1058 | `TxnContext` | `src/storage/new_txn/txn_context.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1059 | `TxnCheckpointInfo` | `src/storage/new_txn/txn_info.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1060 | `TxnCleanInfo` | `src/storage/new_txn/txn_info.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1061 | `TxnCompactInfo` | `src/storage/new_txn/txn_info.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1062 | `TxnImportInfo` | `src/storage/new_txn/txn_info.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1063 | `TxnInfo` | `src/storage/new_txn/txn_info.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1064 | `TxnOptimizeInfo` | `src/storage/new_txn/txn_info.cppm` | mapped | infinity_txn + infinity_executor::EngineTxn 
| 1065 | `ObjectStats` | `src/storage/persistence/obj_stat_accessor.cppm` | mapped | infinity_storage::persistence 
| 1066 | `ObjStat` | `src/storage/persistence/obj_status.cppm` | mapped | infinity_storage::persistence 
| 1067 | `Range` | `src/storage/persistence/obj_status.cppm` | mapped | infinity_storage::persistence 
| 1068 | `PersistResultHandler` | `src/storage/persistence/persist_result_handler.cppm` | mapped | `infinity_storage::persistence::PersistResultHandler` 
| 1069 | `ObjAddr` | `src/storage/persistence/persistence_manager.cppm` | mapped | infinity_storage::persistence 
| 1070 | `PersistReadResult` | `src/storage/persistence/persistence_manager.cppm` | mapped | infinity_storage::persistence 
| 1071 | `PersistWriteResult` | `src/storage/persistence/persistence_manager.cppm` | mapped | infinity_storage::persistence 
| 1072 | `PersistenceManager` | `src/storage/persistence/persistence_manager.cppm` | mapped | `infinity_storage::persistence::PersistenceManager` 
| 1073 | `CacheOutput` | `src/storage/result_cache_manager.cppm` | mapped | `result_cache::CacheOutput` |
| 1074 | `ResultCacheManager` | `src/storage/result_cache_manager.cppm` | mapped | `result_cache::ResultCacheManager`（AddCache/GetCache/DropTable/ResetCacheNumCapacity/ClearCache） |
| 1075 | `CommonQueryFilter` | `src/storage/secondary_index/common_query_filter.cppm` | mapped | infinity_storage::secondary_* 
| 1076 | `HighCardinalityTag` | `src/storage/secondary_index/secondary_index_data.cppm` | mapped | infinity_storage::secondary_* 
| 1077 | `LowCardinalityTag` | `src/storage/secondary_index/secondary_index_data.cppm` | mapped | infinity_storage::secondary_* 
| 1078 | `SecondaryIndexDataBase` | `src/storage/secondary_index/secondary_index_data.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1079 | `SecondaryIndexInMem` | `src/storage/secondary_index/secondary_index_in_mem.cppm` | mapped | infinity_storage::secondary_* 
| 1080 | `SecondaryPGMIndex` | `src/storage/secondary_index/secondary_index_pgm.cppm` | mapped | infinity_storage::secondary_* 
| 1081 | `Storage` | `src/storage/storage.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1082 | `BaseMemIndex` | `src/storage/tracer/base_memindex.cppm` | mapped | infinity_storage::memindex_tracer 
| 1083 | `DummyIndexInMem` | `src/storage/tracer/base_memindex.cppm` | mapped | infinity_storage::memindex_tracer 
| 1084 | `MemUsageChange` | `src/storage/tracer/mem_usage_change.cppm` | mapped | infinity_storage::memindex_tracer 
| 1085 | `BGMemIndexTracer` | `src/storage/tracer/memindex_tracer.cppm` | mapped | infinity_storage::memindex_tracer 
| 1086 | `MemIndexDetail` | `src/storage/tracer/memindex_tracer.cppm` | mapped | infinity_storage::memindex_tracer 
| 1087 | `MemIndexTracer` | `src/storage/tracer/memindex_tracer.cppm` | mapped | `infinity_storage::memindex_tracer::MemIndexTracer` 
| 1088 | `MemIndexTracerInfo` | `src/storage/tracer/memindex_tracer.cppm` | mapped | infinity_storage::memindex_tracer 
| 1089 | `View` | `src/storage/view.cppm` | exempt | C++ 侧 `PhysicalCreateView` 是空壳、DDL 分发没有 view 分支；端口同样以 `Unknown DDL type` 拒绝，不计入逐类移植 
| 1090 | `BottomExecutor` | `src/storage/wal/bottom_executor.cppm` | mapped | infinity_storage::wal_entry 
| 1091 | `TempWalFileInfo` | `src/storage/wal/log_file.cppm` | mapped | infinity_storage::wal_entry 
| 1092 | `WalFile` | `src/storage/wal/log_file.cppm` | mapped | infinity_storage::wal_entry 
| 1093 | `WalFileInfo` | `src/storage/wal/log_file.cppm` | mapped | infinity_storage::wal_entry 
| 1094 | `WalBlockInfo` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1095 | `WalCmd` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1096 | `WalCmdAddColumnsV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1097 | `WalCmdAlterIndexV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1098 | `WalCmdAppendV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1099 | `WalCmdCheckpointV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1100 | `WalCmdCleanup` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1101 | `WalCmdCompactV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1102 | `WalCmdCreateDBSnapshot` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1103 | `WalCmdCreateDatabaseV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1104 | `WalCmdCreateIndexV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1105 | `WalCmdCreateSystemSnapshot` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1106 | `WalCmdCreateTableSnapshot` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1107 | `WalCmdCreateTableV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1108 | `WalCmdDeleteV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1109 | `WalCmdDropColumnsV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1110 | `WalCmdDropDatabaseV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1111 | `WalCmdDropIndexV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1112 | `WalCmdDropTableV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1113 | `WalCmdDummy` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1114 | `WalCmdDumpIndexV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1115 | `WalCmdImportV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1116 | `WalCmdRenameTableV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1117 | `WalCmdRestoreDatabaseSnapshot` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1118 | `WalCmdRestoreSystemSnapshot` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1119 | `WalCmdRestoreTableSnapshot` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1120 | `WalEntry` | `src/storage/wal/wal_entry.cppm` | mapped | `infinity_storage::wal_entry::WalEntry` 
| 1121 | `WalEntryHeader` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1122 | `WalEntryIterator` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1123 | `WalListIterator` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1124 | `WalRestoreIndexV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1125 | `WalSegmentIndexInfo` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1126 | `WalSegmentInfo` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1127 | `WalSegmentInfoV2` | `src/storage/wal/wal_entry.cppm` | mapped | infinity_storage::wal_entry 
| 1128 | `ReplayWalOptions` | `src/storage/wal/wal_manager.cppm` | mapped | infinity_storage::wal_entry 
| 1129 | `WalManager` | `src/storage/wal/wal_manager.cppm` | mapped | infinity_storage::wal_entry 
| 1130 | `BaseTestNoParam` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1131 | `BaseTestParamStr` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1132 | `BaseTestWithParam` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1133 | `NewBaseTestNoParam` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1134 | `NewReplayTest` | `src/unit_test/storage/new_catalog/replay_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1135 | `NewRequestTest` | `src/unit_test/storage/new_request/request_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1136 | `SQLRunner` | `src/unit_test/test_helper/sql_runner.cppm` | test | C++ 测试夹具（`src/unit_test`） |

## 3 方法级对应

只列需要逐方法核对的类；其余类的落点见 §2 的「Rust 承担方式」。

### 3.1 已一一对应

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
| `IVF_Index_Storage::SearchIndex` / `IVF_Parts_Storage_Info<kPlain|kScalarQuantization|kProductQuantization>` | `infinity_index::ivf::{IvfIndex, IvfSqIndex, IvfPqIndex}`（`IvfSegment` 统一三种存储） | 质心选择 + part 扫描；SQ = 残差码 + `dot_v_e`（a/b 按 trimmed min/max 训练），PQ = 每子空间码本 + `-0.5‖c‖²` 权重，三者都按 C++ 的 ip/cosine/l2 公式打分；`ivf_def::IndexIvf::make` 由 `CREATE INDEX` 调用做参数校验，镜像按 `storage_type` 写出对应 chunk | ✅ |
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
| `BlockMaxWandIterator`（`blockmax_wand_iterator_impl.cpp`） | `wand::BlockMaxWandIterator` | `Next`→`run`（pivot/NextShallow/block-max 剪枝）。整个 `MATCH TEXT` 路径都从 posting 取数：候选文档、每个文档的 term 频次、语料统计（`n_docs`/平均字段长度/`df`）来自索引；短语（`"a b"`）的频次从 `.pos` 位置表算（`TextIndex::merge_positions`，内存尾/spill run/封存段各自的 `positions()`），多词单元按参考的 `PhraseWeight` 打分（短语频次作 tf、idf 取各项之和）。行只在需要取值时才读：`filter=`、`rank_features`、取 top-n 结果行；字段组形式（`MATCH TEXT ('', '(doc:…)')`）同样先取候选（`query_leaf_terms`）。只有带否定的查询树（`NOT doc:x`）退回全表扫描，因为 posting 的并集会把它要的文档剔掉。测试 `a_phrase_match_answers_from_the_index`、`a_field_group_match_tokenises_only_the_candidates` 用 `MATCH_ROWS_TOKENIZED` 计数证明列形式一次都不逐行分词 | ✅ |
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
| `BMPAlg` / `BMPAlgBase::SearchKnn`（`bmp_alg.cppm`，own-mem） | `infinity_index::sparse::{BmpAlg, BmpSearchOptions}` | `AddDoc`→`add_doc`、`SearchKnn`→`search_knn`、`GetScores`/`Calculate2`、`alpha/beta/use_tail`（`alpha`/`beta`/`threshold` 一路传到磁盘 reader，含 `beta` 截断与 k-th 阈值；`use_tail` 像参考一样被 binder 拒绝、`tail` 接受但不生效；门禁 `test_the_sparse_search_options_change_the_answer_the_same_way`） | ✅ |
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
| `AnalyzerPool` / `RAGAnalyzer` / `ChineseAnalyzer` / `IKAnalyzer` / `DartsTrie`（`common/analyzer/*`） | `infinity_executor::analyzer::{AnalyzerPool, AnalyzerName, parse_analyzer_name, RAGAnalyzer, ChineseAnalyzer, IKAnalyzer, DartsTrie}` | 名称解析（族名大小写敏感；`rag-<lang>[-fine]`、`chinese/traditional/ik` 的精确 `-fine`、`standard-<language>` 大小写不敏感、`keyword/whitespace-<delims>` 逐字节、`ngram-<strtol>`、`sparsegram-<min>-<max>-fold`）、trie 驱动分词、按语言折叠/停用词/词干、jieba MP-DAG+HMM 粗/细粒度、IK 词典最长匹配；`Tokenize`/`Analyze`→`tokenize`/`analyze`；`CREATE INDEX … WITH (ANALYZER = …)` 与查询共用同一个 `AnalyzerPool`（测试 `a_fulltext_index_uses_its_declared_analyzer`），DDL 校验与 `get_analyzer` 共用 `parse_analyzer_name`（单测 `analyzer_name_grammar_matches_the_reference`、`analyzer_suffixes_reach_the_constructed_analyzers`、`a_keyword_fulltext_index_takes_its_delimiters_from_the_name`、`create_index_validates_the_analyzer_name_like_the_reference`，跨引擎门禁 `test_analyzer_names_and_tokenisation_match`） | ✅（OpenCC 为等价折叠，IK 歧义仲裁未移植） |
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
| `OptimizerRule` / `Optimizer` / `ColumnPruner` / `ColumnRemapper` / `LazyLoad` / `ApplyFastRoughFilter` / `ResultCacheGetter`（`planner/optimizer/*`、`optimizer_rule.cppm`） | `infinity_planner::{optimize, column, result_cache, index_scan}` | `OptimizerRule` trait + `Optimizer` 按序执行。生产顺序即参考的 `optimizer_impl.cpp:40-46`：执行器先用真实 catalog 跑 `ApplyFastRoughFilter` + `IndexScanBuilder`（候选要在 lowering 之前拿到），`run_plan` 接 `ColumnPruner`/`LazyLoad`/`ColumnRemapper`，`result_cache` 打开时最后挂 `ResultCacheGetter`。`ColumnRemapper::remap`、`LazyLoad::{collect,clean_scan}` 做位置化裁剪/重映射；`Limit`/`ORDER BY` 由 binder 直接摆到参考位置（`optimize::normalize`），优化器里没有额外的改写趟。 | ✅ |
| `FilterExpressionPushDown` / `IndexFilterEvaluator` 及其 AllTrue/AllFalse/Secondary/Fulltext/LogicalChildren / `FilterExpressionPushDownHelper` / `ExpressionInfoTree` / `IndexScanBuilder`（`planner/optimizer/index_scan/*`） | `infinity_planner::index_scan` | `push_down_to_index_scan` 拆分 index/leftover 并构建求值器；`unwind_cast`、JSON term 编码、区间 merge/AND-OR 组合；`IndexScanBuilder::build` 返回 `IndexScanCandidate`；规则把它写进 `LogicalPlan::IndexScan`（对应 C++ 的 `LogicalIndexScan`），残余谓词挂在节点上；`ColumnPruner` 对 index scan 不访问其表达式（同参考的 `// do not visit node expression`） | ✅ |
| `RegexIndexRewrite`（`planner/optimizer/regex_index_rewrite.cppm`） | `infinity_planner::regex_rewrite::RegexIndexRewrite` | 对 sparsegram 全文索引列，把 `regex` 的必含字面 run 转成 `filter_fulltext` 合取并保留原正则作为残余过滤；run 抽取复用 `infinity_expression::regex_literal` | ✅ |
| `QueryBinder`（`planner/query_binder.cppm`） | `infinity_planner::binder::QueryBinder` | `bind_select`/`bind_insert`/`bind_delete`/`bind_update`/`bind_from`/`bind_table`/`bind_expression`/`bind_predicate`；COMPACT 无 AST/绑定结构，由 `infinity-executor` 直接执行 | ✅ |
| `Binding` / `BindContext`（`planner/binding.cppm`、`planner/bind_context.cppm`） | `infinity_planner::bind_context::{BindingType, Binding, CommonTableExpressionInfo, BindContext, ResolvedColumn}` | `make`/`add_{left,right}_child`/`get_new_logical_node_id`/`generate_{table,binding_context}_index`（委托根 context 的自增计数，初值 1）/`get_cte`+`is_cte_bound`/`is_view_bound`/`is_table_bound`/`bound_*`/`bound_search`/`add_{subquery,cte,view,table}_binding`/`add_bind_context`（重名表/重表号/重绑定名报错、列生产者追加）/`resolve_column_id`（未限定→`binding_names_by_column_`，多生产者报 Ambiguous，限定→按表绑定；`Ok(None)` 表示本层未绑定）/`add_correlated_column_expr`/`has_correlated_column`/`get_binding_from_current_or_parent_by_name`/私有 `add_binding`。`plan_select` 与 `QueryBinder::bind_select` 都先建根 context，`t.c` 限定列因此可绑定，两表同名的未限定列报 ambiguous | ✅ |
| `PhysicalOperator` / `OperatorState` 及算子实现（`physical_operator.cppm`、`operator_state.cppm`、`operator/physical_{scan,project,filter,limit,top,aggregate,sort,hash_join}*`） | `infinity_executor::{chunk::DataChunk, operators::{PhysicalOperator, OperatorState, OperatorStates, execute_task, run_fragment, lower, scan::PhysicalSource, project::Projection, filter::Filter, aggregate::Aggregate, sort::Sort, top::Top, limit::{Limit, UnSyncCounter}}}` | **基类/状态**：`GetName`/`GetOutputNames`/`Init`/`TaskletCount`/`Execute`；每算子一个 `OperatorState`（`data_blocks`/`status`/`empty_source`/`complete`/`total_hits_count_flag`/`total_hits_count`），`prev_op_state_->data_block_array_.clear()` → `OperatorStates::take_prev`（移出并清空前驱块）。**交换单位**：`chunk::DataChunk`，列式 `Value` 单元、容量 8192（复用 `infinity-storage` 常量）；刻意不复用 `DataBlock`（其单元是 `BlockValue`，而表达式在 `Value` 上求值）。**算子集**：`PhysicalSource`（= `PhysicalScan`，`source_state_` → 源状态；Table/Values/Join 三种源，取数/`apply_index_filter`/`align_rows`/`#_row_id`/`#_score` 都在这里）、`Projection`、`Filter`、`Aggregate`（HashMap 分组 + 首见序 + 流式累加器，不再按行 clone）、`Sort`（分块 run + k 路归并，全平局取更早 run）、`Top`（Sort 与 Limit/Offset 融合）、`Limit`（`UnSyncCounter` 块级预算，耗尽即 `complete` 并停止拉取）。**驱动**：`FragmentTask::OnExecute` → `execute_task`（链按扫描在前存储，正向遍历等价于参考的 `operator_count-1 ..= 0`）+ `run_fragment`。**分工**：`lower()` 承担 `PhysicalPlanner`+`FragmentBuilder`，`run_plan` = 优化 → lower → 建源 → 跑 | ✅（残余：`Top` 非 bounded heap；扫描先物化再分块；`distinct` 仍是 LIMIT 之后的后处理；无 `MergeTop`/`MergeSort`（单 fragment）；`HashJoin` 参考侧本身为空实现（`physical_hash_join_impl.cpp:30` 直接 `return true`），Rust 的 join 走 `PhysicalSource` 的 `JoinScan` + `nested_loop_join`；`TaskScheduler`/`FragmentContext`/`BlockingQueue` 沿 D4 豁免） |
| `CorrelatedExpressionsDetector` / `DependentJoinFlattener` / `RewriteCorrelatedExpressions` / `SubqueryUnnest`（`planner/subquery/*`） | `infinity_planner::subquery` | 深度化绑定子查询树 `SubqueryPlan`/`SubqueryExpr`；depth 检测、相关列重写、dependent join 下推、标量/IN/EXISTS unnest 与 mark join。深度检测与重写有单测覆盖；`SubqueryUnnest` 产出的 mark join 在参考侧没有执行算子，端口由 `infinity-executor/src/dependent.rs` 执行同一模型（相关列 = join key，内层按 distinct key 求值一次、结果拼回外层行，见下方说明） | ✅ |

> **基线说明**：`TableEntry` / `SegmentEntry` / `BlockManager` 已在基线 PR #2722 中从 C++ 源码删除；当前持久化元数据类是 `TableMeta` / `SegmentMeta` / `BlockMeta` / `ColumnMeta`，块映射类是 `BlockIndex`。本表按基线实际存在的类名对应，不再列出已删除的类。

### 3.2 peer / cluster

| C++ | Rust | 说明 |
|---|---|---|
| `PeerServerThriftService` / `PeerClient` / `PoolPeerThriftServer` | `infinity_server::peer::{PeerServerThriftService, PeerClient, PoolPeerThriftServer}`，由 `PeerManager` 挂到 `Server` | 注册/注销/心跳/同步日志/改角色的错误码与 leader-only 规则；状态机、任务队列、事件环未简化。**传输为进程内 `InProcessPeerTransport` 模拟，没有 Thrift/TCP 线路**——`thrift/peer_server.thrift` 的 TBinaryProtocol 监听未实现（剩余缺口，见 `PeerManager::handle_peer_request` 分派边界） |
| `PeerTask` / `RegisterPeerTask` / `UnregisterPeerTask` / `TerminatePeerTask` / `HeartBeatPeerTask` / `SyncLogTask` / `ChangeRoleTask` | `infinity_server::peer::{PeerTask, RegisterPeerTask, UnregisterPeerTask, TerminatePeerTask, HeartBeatPeerTask, SyncLogTask, ChangeRoleTask}` | `execute`/`wait`/`complete` 完成门、C++ 字段集合与 `ToString`、重试/重连策略（2 次/1000ms） |
| `RingBufferIterator` | `infinity_server::ring_buffer::RingBufferIterator` | 4096 环形缓冲、模自增、带符号 `Distance`、`Find`/`CopyN`，共享缓冲用 `Arc<RwLock<Vec<u8>>>`（crate 禁用 unsafe 别名） |

> **相关子查询**：`dependent.rs` 把每个读外层的嵌套查询改写成隐藏扫描列 `#_corr_n`，外层关系物化后按 distinct key 求值内层并拼回每行（dependent join 的缓存形式），所以 `WHERE`、投影（含投影表达式）与 `HAVING` 都走普通列路径，外层可以是基表、CTE 或 JOIN 的 FROM 侧。参考侧 `JoinType::kMark` 只有 planner 生成、没有执行算子（实测 C++ 对投影里的相关子查询直接 abort），端口不复刻该崩溃；作用域边界见 `GAPS.md` §5。peer 层为进程内实现（`InProcessPeerTransport`）：状态机、任务队列、事件环按 C++ 移植，但没有 Thrift/TCP 线路，`thrift/peer_server.thrift` 的 PeerService 未在 Rust 侧监听——相关类在 §2 记为 `mapped`（附进程内传输说明），本节不宣称线路兼容。

## 4 有意的设计变更（§3.6 豁免）

| C++ | Rust | 原因 |
|---|---|---|
| `TaskScheduler` / `FragmentContext` / `BlockingQueue` | 单线程 fragment 驱动（`operators::{execute_task, run_fragment}`） | §4.4 D4（用户已批准）。算子模型本身已按 `PhysicalOperator`/`OperatorState` 移植（见上表），缺的是并行 tasklet 与交换队列：每个 fragment 一个任务、一个 `OperatorStates`，`TaskletCount` 恒 1。**影响**：并行度与 C++ 不同，结果一致 |
| `BGTaskProcessor` + `CompactionProcessor` + `OptimizationProcessor` + `DumpIndexProcessor`（四个 processor，各自一条线程） | 单个 `BGTaskProcessor`（工作线程池）+ `Engine::start_background_tasks` 里的 1 s tick 线程；`CleanupTask` / `OptimizeTask` / `DumpMemIndexTask` 共用同一队列 | C++ 四个 processor 各只服务一类任务（checkpoint+cleanup / compact / optimize / dump），分开是为了按类别隔离长阻塞；Rust 侧三类任务都靠 `flush_lock` / `batch_lock` 串行化落盘，独立队列没有收益。**影响**：`cleanup_interval`、`optimize_interval`、`compact_interval` 都由同一个 1 s tick 线程按各自周期触发（`Engine::start_background_tasks` → `cleanup` / `optimize_indexes` / `notify_compact`）；`checkpoint_interval` 的 checkpoint 仍由 CLI 自己的线程驱动，因为它是 `trim_allocator()`（`malloc_trim`，`infinity-executor` 禁 unsafe）的唯一调用点 |
| VARCHAR/JSON 次级索引的范围扫描 | `hash_varchar` 哈希键，只能等值查找 | 参考侧对 `Varchar` 同样不保留有序键（`KeepOrderedSelf` 不含 `Varchar`），因此**两边一致**：字符串列的范围谓词在 C++ 也退回全表过滤。INT/UINT 已用有序键并支持范围扫描（`SecondarySegmentReader::range`），布尔走低基数位图；FLOAT/DOUBLE/DATE 的键有序，`SecondarySegmentReader::range` 支持范围扫描|
| `Storage::SetStorageMode` 的 `InitToAdmin` / `AdminToReader(Bottom)` / `AdminToWriter` / `WriterTo*` 子系统生命周期，`AttachCatalog(ts)`，`RecoverMemIndex()` | `StorageMode` 只做**准入守卫**（`Engine::ensure_writable` / `ensure_cleanup_allowed`）；所有子系统在 `Engine::open` 一次性建好、`drop` 一次性拆除；catalog 是实时 `MetaStore` 视图，无 checkpoint-ts 快照 | C++ 的每个迁移都会重建 buffer / txn / wal / bg_task / catalog 并重放 WAL；Rust 的 `open()` 已完成等价初始化（`txn.recover` + `recover_cpp_blocks` + `rebuild_indexes`），没有可切换的独立 processor 生命周期。**影响**：切换角色只改变写入准入（非 Writable ⇒ 8007 `InvalidNodeRole`），不真正拆装子系统；单进程内切到 Reader 后已加载的内存索引不会释放 |
| `standard-porter` 的 `STEM_LANG_PORT`（libstemmer 原版 Porter） | `rust_stemmers::Algorithm::English`（Porter2） | 仅 `standard-porter` 受影响：少数词的词干与参考不同（Porter2 与 Porter 的 step 1c/5b 规则有别）。`standard`（`STEM_LANG_ENGLISH`）与其余 17 种语言都用 Snowball 同族算法，字节一致；为单一分词器引入一份手写 Porter 实现（或新依赖）风险大于收益，若不接受此差异需单独决定 |
| `ApplyFastRoughFilter` 的统计来源 | 规则在 `run_plan` 的 `Optimizer::with_catalog` 里跑，`IndexCatalog` 只回答"哪些列有索引"，所以规则只把可证明不可命中的过滤折叠成空关系（`AlwaysFalse`）；按块 min/max 的跳过发生在扫描侧，读 `blk|…|fast_rough_filter` 记录（`Engine::prune_conditions`） | C++ 在规则里就用列级 min/max + bloom 统计决定块裁剪。**影响**：跳过发生在扫描而不是规划期，计划形状可能与参考不同，结果一致；见 `GAPS.md` §2 |
| `ResultCacheGetter` 的缓存层 | 计划级 `ResultCacheGetter::rewrite`（`run_plan` 优化之后挂上，`result_cache` 打开时生效；键 = 节点形状 + `begin_ts`，命中替换成 `Values`）；另有语句级 `Engine::result_cache`（`format!("{qualified:?}")` 为键，只缓存小结果，任何写原语清空） | C++ 只有计划级缓存（begin-ts + 表键 + 列投影），命中替换成 `LogicalReadCache`。**影响**：多一层更粗的语句级缓存（不同参数不共享、写入即整体失效），结果语义一致；`Values` 替身没有参考的 read-cache 节点 |
| `BindContext` 的 `column_types_` / `table_info_` / `block_index_` / `expression_binder_` 字段，`select_expression_`（`ParsedExpr *`），`bound_cte_set_`（`shared_ptr` 身份集），`correlated_column_exprs_`（`ColumnExpression` 指针），`parent_` 裸指针 + 根 context 计数 | 去掉前四个字段；`select_expression: Vec<SelectItem>`；`bound_cte_set: HashSet<String>`（按 alias，等价于以其为键的 CTE 对象身份）；`correlated_column_exprs: Vec<ResolvedColumn>`；`parent/left_child/right_child: Rc<BindContext>`，三个计数器与 `knn_table_index` 为 `Cell`，相关列对为 `RefCell` | Rust 侧无 `DataType`/`TableInfo`/`BlockIndex`/`ExpressionBinder`，也没有可承托绑定的 `ColumnExpression`（`Expr::Column` 只有位置）；`ExprAst` 表达不了 `*` 与聚合，故 select list 用 `SelectItem`。`&self` 下的“委托根 context 自增”只能靠 `Cell`/`RefCell`，语义（同一序列、不重号）不变 |
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

## 5 C++ 单元测试对应

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

## 6 维护规则

- 新增一个 C++ 子系统的移植时，先在本文档登记类/方法映射，再写代码。
- 局部合并/拆分是允许的，但必须写清动机与影响，不能以「测试能过」为唯一理由。
- 每个 Rust `pub fn` 的文档注释写 `C++: path:line`。
- 任何“简化”必须出现在「有意的设计变更」表里（或 §2 行内的 `partial` 说明），不能默默省略。
- Gate A 通过不等于移植完成：状态以本表 + 计划书 §7.3 为准。
