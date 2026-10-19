# C++ 类级覆盖审计（逐名）

基线 `5d27942c0` 的 `src/**/*.cppm` 中每个 `export class/struct` 一行。geo 类型类未逐类列入本表（`GeographyTryCastToVarlen` 例外，承担于 cast）；geo 用例的排除见 `TEST_AUDIT.md`，DiskANN 15 类在本表 `excluded`。

- 总数：**1136**
- 状态分布：`excluded` 15, `exempt` 20, `gap` 23, `mapped` 1067, `partial` 4, `test` 7

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
| 23 | `JapaneseAnalyzer` | `src/common/analyzer/japanese_analyzer.cppm` | partial | 需 MeCab ipadic（Japanese），Rust 未移植且 AnalyzerPool 明确拒绝，不得标 mapped 
| 24 | `KoreanAnalyzer` | `src/common/analyzer/korean_analyzer.cppm` | partial | 需 MeCab ko-dic（Korean），Rust 未移植且 AnalyzerPool 明确拒绝，不得标 mapped 
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
| 36 | `TraditionalChineseAnalyzer` | `src/common/analyzer/traditional_chinese_analyzer.cppm` | partial | 需 OpenCC 简繁转换（Traditional Chinese），Rust 未移植且 AnalyzerPool 明确拒绝，不得标 mapped 
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
| 93 | `PhysicalCreateView` | `src/executor/operator/physical_create_view.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
| 94 | `PhysicalCrossProduct` | `src/executor/operator/physical_cross_product.cppm` | mapped | infinity_executor 的物理算子/管线 
| 95 | `PhysicalDelete` | `src/executor/operator/physical_delete.cppm` | mapped | infinity_executor 的物理算子/管线 
| 96 | `PhysicalDropCollection` | `src/executor/operator/physical_drop_collection.cppm` | mapped | infinity_executor 的物理算子/管线 
| 97 | `PhysicalDropIndex` | `src/executor/operator/physical_drop_index.cppm` | mapped | infinity_executor 的物理算子/管线 
| 98 | `PhysicalDropSchema` | `src/executor/operator/physical_drop_schema.cppm` | mapped | infinity_executor 的物理算子/管线 
| 99 | `PhysicalDropTable` | `src/executor/operator/physical_drop_table.cppm` | mapped | infinity_executor 的物理算子/管线 
| 100 | `PhysicalDropView` | `src/executor/operator/physical_drop_view.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
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
| 170 | `CreateViewOperatorState` | `src/executor/operator_state.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
| 171 | `CrossProductOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 172 | `DeleteOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 173 | `DropCollectionOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 174 | `DropDatabaseOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 175 | `DropIndexOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 176 | `DropTableOperatorState` | `src/executor/operator_state.cppm` | mapped | infinity_executor 的物理算子/管线 
| 177 | `DropViewOperatorState` | `src/executor/operator_state.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
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
| 363 | `PeerServerThriftService` | `src/network/peer_server_thrift_service.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 364 | `ChangeRoleTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 365 | `HeartBeatPeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 366 | `PeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 367 | `RegisterPeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 368 | `SyncLogTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 369 | `TerminatePeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 370 | `UnregisterPeerTask` | `src/network/peer_task.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 371 | `PeerClient` | `src/network/peer_thrift_client.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 372 | `PoolPeerThriftServer` | `src/network/peer_thrift_server.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
| 373 | `PGProtocolHandler` | `src/network/pg_protocol_handler.cppm` | mapped | `infinity_server::pg::PGProtocolHandler`（泛型读写流：启动握手/SSL 拒绝、前端命令解析、响应帧、命令循环），`PgServer` 每条连接都由它处理，有单测 |
| 374 | `PGServer` | `src/network/pg_server.cppm` | mapped | `infinity_server::PgServer` 
| 375 | `RingBufferIterator` | `src/network/ring_buffer_iterator.cppm` | mapped | `infinity_server::peer` / `ring_buffer`，经 `PeerManager` 挂到 `Server`，有服务端行为测试（进程内传输，见 CLASS_MAP 限制说明） 
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
| 433 | `LogicalCreateView` | `src/planner/node/logical_create_view.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
| 434 | `LogicalCrossProduct` | `src/planner/node/logical_cross_product.cppm` | mapped | infinity_planner 
| 435 | `LogicalDelete` | `src/planner/node/logical_delete.cppm` | mapped | infinity_planner 
| 436 | `LogicalDropCollection` | `src/planner/node/logical_drop_collection.cppm` | mapped | infinity_planner 
| 437 | `LogicalDropIndex` | `src/planner/node/logical_drop_index.cppm` | mapped | infinity_planner 
| 438 | `LogicalDropSchema` | `src/planner/node/logical_drop_schema.cppm` | mapped | infinity_planner 
| 439 | `LogicalDropTable` | `src/planner/node/logical_drop_table.cppm` | mapped | infinity_planner 
| 440 | `LogicalDropView` | `src/planner/node/logical_drop_view.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
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
| 471 | `LogicalViewScan` | `src/planner/node/logical_view_scan.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
| 472 | `Optimizer` | `src/planner/optimizer.cppm` | mapped | infinity_planner 
| 473 | `ApplyFastRoughFilter` | `src/planner/optimizer/apply_fast_rough_filter.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 474 | `ColumnPruner` | `src/planner/optimizer/column_pruner.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 475 | `ColumnRemapper` | `src/planner/optimizer/column_remapper.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 476 | `FilterExpressionPushDown` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 477 | `IndexFilterEvaluator` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 478 | `IndexScanFilterExpressionPushDownResult` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 479 | `FilterExpressionPushDownHelper` | `src/planner/optimizer/index_scan/filter_expression_push_down_helper.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 480 | `IndexFilterEvaluatorAllFalse` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 481 | `IndexFilterEvaluatorAllTrue` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 482 | `IndexFilterEvaluatorFulltext` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 483 | `IndexFilterEvaluatorLogicalChildren` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 484 | `IndexFilterEvaluatorSecondary` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 485 | `ExpressionInfoTree` | `src/planner/optimizer/index_scan/index_filter_expression_info_tree.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 486 | `IndexScanBuilder` | `src/planner/optimizer/index_scan_builder.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 487 | `LazyLoad` | `src/planner/optimizer/lazy_load.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 488 | `RegexIndexRewrite` | `src/planner/optimizer/regex_index_rewrite.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 489 | `ResultCacheGetter` | `src/planner/optimizer/result_cache_getter.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 490 | `OptimizerRule` | `src/planner/optimizer_rule.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 491 | `QueryBinder` | `src/planner/query_binder.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 492 | `CorrelatedExpressionsDetector` | `src/planner/subquery/correlated_expressions_detector.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 493 | `DependentJoinFlattener` | `src/planner/subquery/dependent_join_flattener.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 494 | `RewriteCorrelatedExpressions` | `src/planner/subquery/rewrite_correlated_expressions.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
| 495 | `SubqueryUnnest` | `src/planner/subquery/subquery_unnest.cppm` | gap | 实现快照已回滚（接线导致 index-scan 候选过滤位置错误、Gate A 1209/1546）；快照在 /tmp/planner_wiring.patch，接线完成后恢复 mapped 
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
| 706 | `IndexIVF` | `src/storage/definition/index_ivf.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 707 | `IndexIVFCentroidOption` | `src/storage/definition/index_ivf.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 708 | `IndexIVFOption` | `src/storage/definition/index_ivf.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
| 709 | `IndexIVFStorageOption` | `src/storage/definition/index_ivf.cppm` | mapped | infinity_executor::data_table + infinity_storage::data_block 
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
| 1089 | `View` | `src/storage/view.cppm` | exempt | C++ `BuildCreateView` 返回 `NotSupport`（实现位于 `#if 0`）；Rust 视图 DDL 为行为扩展，不计入逐类移植 
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
