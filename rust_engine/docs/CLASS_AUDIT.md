# C++ 类级覆盖审计（逐名）

基线 `5d27942c0` 的 `src/**/*.cppm` 中每个 `export class/struct` 一行。

- 总数：**1136**
- 状态分布：`excluded` 15, `grouped` 993, `mapped` 121, `test` 7

| # | C++ 类/结构 | 定义位置 | 状态 | Rust 承担方式 |
|---:|---|---|---|---|
| 1 | `AdminExecutor` | `src/admin/admin_executor.cppm` | grouped | `execute_stmt_inner` 的语句分发 |
| 2 | `Analyzer` | `src/common/analyzer/analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 3 | `AnalyzerPool` | `src/common/analyzer/analyzer_pool.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 4 | `ChineseAnalyzer` | `src/common/analyzer/chinese_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 5 | `CommonLanguageAnalyzer` | `src/common/analyzer/common_analyzer.cppm` | mapped | `infinity_executor::analyzer::StandardAnalyzer` 的选项（case/stem/stem_only） |
| 6 | `DartsTrie` | `src/common/analyzer/darts_trie.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 7 | `DartsTuple` | `src/common/analyzer/darts_trie.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 8 | `POSTable` | `src/common/analyzer/darts_trie.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 9 | `AnalyzeContext` | `src/common/analyzer/ik/analyze_context.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 10 | `IKArbitrator` | `src/common/analyzer/ik/arbitrator.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 11 | `CharacterUtil` | `src/common/analyzer/ik/character_util.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 12 | `CJKSegmenter` | `src/common/analyzer/ik/cjk_segmenter.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 13 | `CNQuantifierSegmenter` | `src/common/analyzer/ik/cn_quantifier_segmenter.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 14 | `Dictionary` | `src/common/analyzer/ik/dict.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 15 | `DictSegment` | `src/common/analyzer/ik/dict_segment.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 16 | `Hit` | `src/common/analyzer/ik/hit.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 17 | `IKAnalyzer` | `src/common/analyzer/ik/ik_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 18 | `LetterSegmenter` | `src/common/analyzer/ik/letter_segmenter.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 19 | `Lexeme` | `src/common/analyzer/ik/lexeme.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 20 | `LexemePath` | `src/common/analyzer/ik/lexeme_path.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 21 | `QuickSortSet` | `src/common/analyzer/ik/quick_sort_set.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 22 | `Segmenter` | `src/common/analyzer/ik/segmenter.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 23 | `JapaneseAnalyzer` | `src/common/analyzer/japanese_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 24 | `KoreanAnalyzer` | `src/common/analyzer/korean_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 25 | `NGramAnalyzer` | `src/common/analyzer/ngram_analyzer.cppm` | mapped | `analyzer::NGramAnalyzer`（n-gram 窗口按 UTF-8 code point 滑动） |
| 26 | `RAGAnalyzer` | `src/common/analyzer/rag_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 27 | `RankFeaturesAnalyzer` | `src/common/analyzer/rank_features_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 28 | `SparsegramAnalyzer` | `src/common/analyzer/sparsegram_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 29 | `StandardAnalyzer` | `src/common/analyzer/standard_analyzer.cppm` | mapped | `analyzer::StandardAnalyzer`（Analyze 的 word/stem/case 组合） |
| 30 | `Stemmer` | `src/common/analyzer/stemmer/stemmer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 31 | `Term` | `src/common/analyzer/term.cppm` | mapped | `analyzer::Term` |
| 32 | `TermList` | `src/common/analyzer/term.cppm` | mapped | `Vec<analyzer::Term>` |
| 33 | `CharTypeTable` | `src/common/analyzer/tokenizer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 34 | `TokenizeConfig` | `src/common/analyzer/tokenizer.cppm` | mapped | `analyzer::TokenizeConfig`（AddAllows/AddDivides/AddUnites） |
| 35 | `Tokenizer` | `src/common/analyzer/tokenizer.cppm` | mapped | `analyzer::tokenize` |
| 36 | `TraditionalChineseAnalyzer` | `src/common/analyzer/traditional_chinese_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 37 | `UserDefinedAnalyzer` | `src/common/analyzer/user_defined_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 38 | `UserDefinedTermWeight` | `src/common/analyzer/user_defined_term_weight.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 39 | `WhitespaceAnalyzer` | `src/common/analyzer/whitespace_analyzer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 40 | `WordNetLemmatizer` | `src/common/analyzer/wordnet_lemmatizer.cppm` | grouped | `tokenize_*` analyzer 函数 |
| 41 | `BlockingQueue` | `src/common/blocking_queue.cppm` | mapped | CLASS_MAP 已显式命名 |
| 42 | `ByteSlice` | `src/common/memory/byte_slice.cppm` | grouped | `infinity-common` 工具/类型 |
| 43 | `ByteSliceList` | `src/common/memory/byte_slice.cppm` | grouped | `infinity-common` 工具/类型 |
| 44 | `ByteSliceListIterator` | `src/common/memory/byte_slice.cppm` | grouped | `infinity-common` 工具/类型 |
| 45 | `ChainedMemoryChunk` | `src/common/memory/memory_chunk.cppm` | grouped | `infinity-common` 工具/类型 |
| 46 | `ChunkAllocator` | `src/common/memory/memory_chunk.cppm` | grouped | `infinity-common` 工具/类型 |
| 47 | `MemoryChunk` | `src/common/memory/memory_chunk.cppm` | grouped | `infinity-common` 工具/类型 |
| 48 | `PythonInstance` | `src/common/python.cppm` | grouped | `infinity-common` 工具/类型 |
| 49 | `SIMD_FUNCTIONS` | `src/common/simd/simd_functions.cppm` | grouped | `infinity-common` 工具/类型 |
| 50 | `Singleton` | `src/common/singleton.cppm` | grouped | `infinity-common` 工具/类型 |
| 51 | `Status` | `src/common/status.cppm` | grouped | `infinity-common` 工具/类型 |
| 52 | `DeferFn` | `src/common/utility/defer_op.cppm` | grouped | `infinity-common` 工具/类型 |
| 53 | `RecoverableException` | `src/common/utility/exception.cppm` | mapped | `infinity_common::Error::Unexpected`（`Unexpected error: {msg}`） |
| 54 | `UnrecoverableException` | `src/common/utility/exception.cppm` | mapped | `infinity_common::Error::Unrecoverable`（裸消息） |
| 55 | `SmallFloat` | `src/common/utility/smallfloat.cppm` | grouped | `infinity-common` 工具/类型 |
| 56 | `ScopedSpinLock` | `src/common/utility/spinlock.cppm` | grouped | `infinity-common` 工具/类型 |
| 57 | `ScopedTryLock` | `src/common/utility/spinlock.cppm` | grouped | `infinity-common` 工具/类型 |
| 58 | `SpinLock` | `src/common/utility/spinlock.cppm` | grouped | `infinity-common` 工具/类型 |
| 59 | `ThreadUtil` | `src/common/utility/threadutil.cppm` | grouped | `infinity-common` 工具/类型 |
| 60 | `ZsvParser` | `src/common/zsv.cppm` | grouped | `infinity-common` 工具/类型 |
| 61 | `ExplainFragment` | `src/executor/explain_fragment.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 62 | `ExplainPhysicalPlan` | `src/executor/explain_physical_plan.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 63 | `AggregateExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 64 | `BaseExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 65 | `CaseExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 66 | `CastExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 67 | `ColumnExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 68 | `ExpressionEvaluator` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 69 | `ExpressionState` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 70 | `FilterFulltextExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 71 | `FunctionExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 72 | `InExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 73 | `ReferenceExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 74 | `ValueExpression` | `src/executor/expression/expression_evaluator.cppm` | grouped | `infinity-expression` 求值函数 |
| 75 | `ExpressionSelector` | `src/executor/expression/expression_selector.cppm` | grouped | `infinity-expression` 求值函数 |
| 76 | `ColumnVector` | `src/executor/expression/expression_state.cppm` | mapped | CLASS_MAP 已显式命名 |
| 77 | `PlanFragment` | `src/executor/fragment/plan_fragment.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 78 | `FragmentBuilder` | `src/executor/fragment_builder.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 79 | `HashRange` | `src/executor/operator/physical_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 80 | `PhysicalAggregate` | `src/executor/operator/physical_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 81 | `PhysicalAddColumns` | `src/executor/operator/physical_alter.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 82 | `PhysicalAlter` | `src/executor/operator/physical_alter.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 83 | `PhysicalAlterIndex` | `src/executor/operator/physical_alter.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 84 | `PhysicalDropColumns` | `src/executor/operator/physical_alter.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 85 | `PhysicalRenameTable` | `src/executor/operator/physical_alter.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 86 | `PhysicalCheck` | `src/executor/operator/physical_check/physical_check.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 87 | `PhysicalCommand` | `src/executor/operator/physical_command.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 88 | `PhysicalCompact` | `src/executor/operator/physical_compact.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 89 | `PhysicalCreateCollection` | `src/executor/operator/physical_create_collection.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 90 | `PhysicalCreateIndexPrepare` | `src/executor/operator/physical_create_index_prepare.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 91 | `PhysicalCreateSchema` | `src/executor/operator/physical_create_schema.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 92 | `PhysicalCreateTable` | `src/executor/operator/physical_create_table.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 93 | `PhysicalCreateView` | `src/executor/operator/physical_create_view.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 94 | `PhysicalCrossProduct` | `src/executor/operator/physical_cross_product.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 95 | `PhysicalDelete` | `src/executor/operator/physical_delete.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 96 | `PhysicalDropCollection` | `src/executor/operator/physical_drop_collection.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 97 | `PhysicalDropIndex` | `src/executor/operator/physical_drop_index.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 98 | `PhysicalDropSchema` | `src/executor/operator/physical_drop_schema.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 99 | `PhysicalDropTable` | `src/executor/operator/physical_drop_table.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 100 | `PhysicalDropView` | `src/executor/operator/physical_drop_view.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 101 | `PhysicalDummyOperator` | `src/executor/operator/physical_dummy_operator.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 102 | `PhysicalDummyScan` | `src/executor/operator/physical_dummy_scan.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 103 | `PhysicalExcept` | `src/executor/operator/physical_except.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 104 | `PhysicalExplain` | `src/executor/operator/physical_explain.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 105 | `PhysicalExport` | `src/executor/operator/physical_export.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 106 | `PhysicalFilter` | `src/executor/operator/physical_filter.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 107 | `PhysicalFlush` | `src/executor/operator/physical_flush.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 108 | `MatchTensorRerankDoc` | `src/executor/operator/physical_fusion.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 109 | `PhysicalFusion` | `src/executor/operator/physical_fusion.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 110 | `PhysicalHash` | `src/executor/operator/physical_hash.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 111 | `PhysicalHashAggregate` | `src/executor/operator/physical_hash_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 112 | `PhysicalHashJoin` | `src/executor/operator/physical_hash_join.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 113 | `PhysicalImport` | `src/executor/operator/physical_import.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 114 | `PhysicalIndexJoin` | `src/executor/operator/physical_index_join.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 115 | `PhysicalInsert` | `src/executor/operator/physical_insert.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 116 | `PhysicalIntersect` | `src/executor/operator/physical_intersect.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 117 | `AtomicCounter` | `src/executor/operator/physical_limit.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 118 | `LimitCounter` | `src/executor/operator/physical_limit.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 119 | `PhysicalLimit` | `src/executor/operator/physical_limit.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 120 | `UnSyncCounter` | `src/executor/operator/physical_limit.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 121 | `PhysicalMatch` | `src/executor/operator/physical_match.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 122 | `PhysicalMergeAggregate` | `src/executor/operator/physical_merge_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 123 | `PhysicalMergeHash` | `src/executor/operator/physical_merge_hash.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 124 | `PhysicalMergeHashAggregate` | `src/executor/operator/physical_merge_hash_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 125 | `PhysicalMergeLimit` | `src/executor/operator/physical_merge_limit.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 126 | `PhysicalMergeParallelAggregate` | `src/executor/operator/physical_merge_parallel_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 127 | `PhysicalMergeSort` | `src/executor/operator/physical_merge_sort.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 128 | `PhysicalMergeTop` | `src/executor/operator/physical_merge_top.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 129 | `PhysicalNestedLoopJoin` | `src/executor/operator/physical_nested_loop_join.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 130 | `PhysicalOptimize` | `src/executor/operator/physical_optimize.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 131 | `PhysicalParallelAggregate` | `src/executor/operator/physical_parallel_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 132 | `PhysicalPreparedPlan` | `src/executor/operator/physical_prepared_plan.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 133 | `PhysicalProject` | `src/executor/operator/physical_project.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 134 | `PhysicalReadCache` | `src/executor/operator/physical_read_cache.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 135 | `PhysicalFilterScanBase` | `src/executor/operator/physical_scan/physical_filter_scan_base.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 136 | `PhysicalIndexScan` | `src/executor/operator/physical_scan/physical_index_scan.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 137 | `PhysicalKnnScan` | `src/executor/operator/physical_scan/physical_knn_scan.cppm` | mapped | CLASS_MAP 已显式命名 |
| 138 | `PhysicalMatchSparseScan` | `src/executor/operator/physical_scan/physical_match_sparse_scan.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 139 | `PhysicalMatchTensorScan` | `src/executor/operator/physical_scan/physical_match_tensor_scan.cppm` | mapped | CLASS_MAP 已显式命名 |
| 140 | `PhysicalMergeKnn` | `src/executor/operator/physical_scan/physical_merge_knn.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 141 | `PhysicalMergeMatchSparse` | `src/executor/operator/physical_scan/physical_merge_match_sparse.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 142 | `PhysicalMergeMatchTensor` | `src/executor/operator/physical_scan/physical_merge_match_tensor.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 143 | `PhysicalScanBase` | `src/executor/operator/physical_scan/physical_scan_base.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 144 | `PhysicalTableScan` | `src/executor/operator/physical_scan/physical_table_scan.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 145 | `PhysicalShow` | `src/executor/operator/physical_show/physical_show.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 146 | `PhysicalSink` | `src/executor/operator/physical_sink.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 147 | `PhysicalSort` | `src/executor/operator/physical_sort.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 148 | `PhysicalSortMergeJoin` | `src/executor/operator/physical_sort_merge_join.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 149 | `PhysicalSource` | `src/executor/operator/physical_source.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 150 | `CompareTwoRowAndPreferLeft` | `src/executor/operator/physical_top.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 151 | `PhysicalTop` | `src/executor/operator/physical_top.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 152 | `TopSolver` | `src/executor/operator/physical_top.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 153 | `PhysicalUnionAll` | `src/executor/operator/physical_union_all.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 154 | `PhysicalUnnest` | `src/executor/operator/physical_unnest.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 155 | `PhysicalUnnestAggregate` | `src/executor/operator/physical_unnest_aggregate.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 156 | `PhysicalUpdate` | `src/executor/operator/physical_update.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 157 | `Snapshot` | `src/executor/operator/snapshot/snapshot.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 158 | `SnapshotBrief` | `src/executor/operator/snapshot/snapshot_brief.cppm` | grouped | `PhysicalOperator` 变体（`lib.rs`/`knn_scan`） |
| 159 | `AggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 160 | `AggregateSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 161 | `AlterOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 162 | `CheckOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 163 | `CommandOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 164 | `CompactOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 165 | `CompactSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 166 | `CreateCollectionOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 167 | `CreateDatabaseOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 168 | `CreateIndexPrepareOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 169 | `CreateTableOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 170 | `CreateViewOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 171 | `CrossProductOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 172 | `DeleteOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 173 | `DropCollectionOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 174 | `DropDatabaseOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 175 | `DropIndexOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 176 | `DropTableOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 177 | `DropViewOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 178 | `EmptySourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 179 | `ExplainOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 180 | `ExportOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 181 | `FilterOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 182 | `FlushOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 183 | `FusionOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 184 | `HashAggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 185 | `HashJoinOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 186 | `HashOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 187 | `ImportOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 188 | `IndexJoinOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 189 | `IndexScanOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 190 | `IndexScanSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 191 | `InsertOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 192 | `KnnScanOperatorState` | `src/executor/operator_state.cppm` | mapped | CLASS_MAP 已显式命名 |
| 193 | `KnnScanSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 194 | `LimitOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 195 | `MatchOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 196 | `MatchSparseScanOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 197 | `MatchSparseScanSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 198 | `MatchTensorScanOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 199 | `MatchTensorScanSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 200 | `MaterializeSinkState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 201 | `MergeAggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 202 | `MergeHashAggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 203 | `MergeHashOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 204 | `MergeJoinOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 205 | `MergeKnnOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 206 | `MergeLimitOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 207 | `MergeMatchSparseOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 208 | `MergeMatchTensorOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 209 | `MergeParallelAggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 210 | `MergeSortOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 211 | `MergeTopOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 212 | `MessageSinkState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 213 | `NestedLoopOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 214 | `OperatorState` | `src/executor/operator_state.cppm` | mapped | CLASS_MAP 已显式命名 |
| 215 | `OptimizeOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 216 | `ParallelAggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 217 | `ProjectionOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 218 | `QueueSinkState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 219 | `QueueSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 220 | `ReadCacheState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 221 | `ResultSinkState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 222 | `ShowOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 223 | `SinkState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 224 | `SortOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 225 | `SourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 226 | `SummarySinkState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 227 | `TableScanOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 228 | `TableScanSourceState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 229 | `TopOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 230 | `UnionAllOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 231 | `UnnestAggregateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 232 | `UnnestOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 233 | `UpdateOperatorState` | `src/executor/operator_state.cppm` | grouped | `PhysicalOperator` 变体 + `OperatorState` |
| 234 | `OutputToDataBlockHelper` | `src/executor/physical_operator.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 235 | `PhysicalCommonFunctionUsingLoadMeta` | `src/executor/physical_operator.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 236 | `PhysicalOperator` | `src/executor/physical_operator.cppm` | mapped | CLASS_MAP 已显式命名 |
| 237 | `PhysicalPlanner` | `src/executor/physical_planner.cppm` | grouped | 执行器管线（`lib.rs` 的 `Engine`/计划节点） |
| 238 | `SourcePosition` | `src/expression/base_expression.cppm` | grouped | `infinity-expression` |
| 239 | `BetweenExpression` | `src/expression/between_expression.cppm` | grouped | `infinity-expression` |
| 240 | `CaseCheck` | `src/expression/case_expression.cppm` | grouped | `infinity-expression` |
| 241 | `ConjunctionExpression` | `src/expression/conjunction_expression.cppm` | grouped | `infinity-expression` |
| 242 | `FusionExpression` | `src/expression/fusion_expression.cppm` | grouped | `infinity-expression` |
| 243 | `KnnExpression` | `src/expression/knn_expression.cppm` | grouped | `infinity-expression` |
| 244 | `MatchExpression` | `src/expression/match_expression.cppm` | grouped | `infinity-expression` |
| 245 | `MatchSparseExpression` | `src/expression/match_sparse_expression.cppm` | grouped | `infinity-expression` |
| 246 | `MatchTensorExpression` | `src/expression/match_tensor_expression.cppm` | grouped | `infinity-expression` |
| 247 | `SearchExpression` | `src/expression/search_expression.cppm` | grouped | `infinity-expression` |
| 248 | `SubqueryExpression` | `src/expression/subquery_expression.cppm` | grouped | `infinity-expression` |
| 249 | `UnnestExpression` | `src/expression/unnest_expression.cppm` | grouped | `infinity-expression` |
| 250 | `AggregateFunction` | `src/function/aggregate_function.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 251 | `AggregateFunctionSet` | `src/function/aggregate_function_set.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 252 | `BuiltinFunctions` | `src/function/builtin_functions.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 253 | `BitmapTryCastToVarlen` | `src/function/cast/bitmap_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 254 | `TryCastBoolean` | `src/function/cast/bool_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 255 | `BoundCastFunc` | `src/function/cast/bound_cast_func.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 256 | `CastParameters` | `src/function/cast/bound_cast_func.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 257 | `CastFunction` | `src/function/cast/cast_function.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 258 | `CastTable` | `src/function/cast/cast_table.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 259 | `ColumnVectorCast` | `src/function/cast/column_vector_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 260 | `DateTryCastToFixlen` | `src/function/cast/date_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 261 | `DateTryCastToVarlen` | `src/function/cast/date_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 262 | `DateTimeTryCastToFixlen` | `src/function/cast/datetime_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 263 | `DateTimeTryCastToVarlen` | `src/function/cast/datetime_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 264 | `DecimalTryCastToFixlen` | `src/function/cast/decimal_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 265 | `DecimalTryCastToVarlen` | `src/function/cast/decimal_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 266 | `EmbeddingTryCastToFixlen` | `src/function/cast/embedding_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 267 | `FloatTryCastToFixlen` | `src/function/cast/float_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 268 | `FloatTryCastToVarlen` | `src/function/cast/float_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 269 | `GeographyTryCastToVarlen` | `src/function/cast/geography_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 270 | `IntegerTryCastToFixlen` | `src/function/cast/integer_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 271 | `IntegerTryCastToVarlen` | `src/function/cast/integer_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 272 | `IntervalTryCastToVarlen` | `src/function/cast/interval_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 273 | `TimeTryCastToVarlen` | `src/function/cast/time_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 274 | `TimestampTryCastToFixlen` | `src/function/cast/timestamp_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 275 | `TimestampTryCastToVarlen` | `src/function/cast/timestamp_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 276 | `UuidTryCastToVarlen` | `src/function/cast/uuid_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 277 | `TryCastVarchar` | `src/function/cast/varchar_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 278 | `TryCastVarcharToChar` | `src/function/cast/varchar_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 279 | `TryCastVarcharToVarchar` | `src/function/cast/varchar_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 280 | `TryCastVarcharVector` | `src/function/cast/varchar_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 281 | `TryCastVarcharVectorToJson` | `src/function/cast/varchar_cast.cppm` | grouped | `infinity-expression` 的 cast 函数 |
| 282 | `Function` | `src/function/function.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 283 | `FunctionData` | `src/function/function_data.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 284 | `FunctionSet` | `src/function/function_set.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 285 | `EncodingAccumulator` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 286 | `EncodingPipeline` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 287 | `FixedDimensionalEncodingConfig` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 288 | `HashPartitioner` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 289 | `RandomGenerator` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 290 | `SparseProjector` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 291 | `Tensor` | `src/function/scalar/fixed_dimensional_encoding.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 292 | `NewCatalog` | `src/function/scalar/modulo.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 293 | `ScalarFunction` | `src/function/scalar_function.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 294 | `ScalarFunctionSet` | `src/function/scalar_function_set.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 295 | `SpecialFunction` | `src/function/special_function.cppm` | grouped | 标量/聚合函数 + `AggFunc` 枚举 |
| 296 | `CompactSegmentData` | `src/function/table/compact_state_data.cppm` | grouped | 表函数由执行器直接求值 |
| 297 | `CompactStateData` | `src/function/table/compact_state_data.cppm` | grouped | 表函数由执行器直接求值 |
| 298 | `RowIDRemap` | `src/function/table/compact_state_data.cppm` | grouped | 表函数由执行器直接求值 |
| 299 | `CreateIndexSharedData` | `src/function/table/create_index_data.cppm` | grouped | 表函数由执行器直接求值 |
| 300 | `AppendFilter` | `src/function/table/knn_filter.cppm` | grouped | 表函数由执行器直接求值 |
| 301 | `BitmaskFilter` | `src/function/table/knn_filter.cppm` | grouped | 表函数由执行器直接求值 |
| 302 | `KnnDistance1` | `src/function/table/knn_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 303 | `KnnDistanceBase1` | `src/function/table/knn_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 304 | `KnnScanFunctionData` | `src/function/table/knn_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 305 | `KnnScanSharedData` | `src/function/table/knn_scan_data.cppm` | mapped | CLASS_MAP 已显式命名 |
| 306 | `MatchSparseScanFunctionData` | `src/function/table/match_sparse_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 307 | `MergeSparseFunctionData` | `src/function/table/match_sparse_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 308 | `SparseBitDistance` | `src/function/table/match_sparse_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 309 | `SparseDistance` | `src/function/table/match_sparse_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 310 | `SparseDistanceBase` | `src/function/table/match_sparse_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 311 | `MatchTensorScanFunctionData` | `src/function/table/match_tensor_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 312 | `MergeKnnFunctionData` | `src/function/table/merge_knn_data.cppm` | grouped | `infinity_index::merge_knn::MergeKnn` |
| 313 | `TableScanFunctionData` | `src/function/table/table_scan_data.cppm` | grouped | 表函数由执行器直接求值 |
| 314 | `TableFunctionData` | `src/function/table_function.cppm` | grouped | 表函数由执行器直接求值 |
| 315 | `ClusterManager` | `src/main/cluster_manager.cppm` | grouped | parser/CLI 结构 |
| 316 | `Config` | `src/main/config.cppm` | mapped | `infinity_cli::config::Config`（`Init`/`Set`/访问器/校验） |
| 317 | `DefaultConfig` | `src/main/config.cppm` | mapped | `infinity_cli::config::Config::default` + `init(None)` |
| 318 | `Infinity` | `src/main/infinity.cppm` | grouped | parser/CLI 结构 |
| 319 | `InfinityContext` | `src/main/infinity_context.cppm` | mapped | CLASS_MAP 已显式命名 |
| 320 | `Logger` | `src/main/logger.cppm` | grouped | parser/CLI 结构 |
| 321 | `LoggerConfig` | `src/main/logger.cppm` | grouped | parser/CLI 结构 |
| 322 | `BaseOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 323 | `BooleanOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 324 | `FloatOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 325 | `FlushOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 326 | `GlobalOptions` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 327 | `IntegerOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 328 | `LogLevelOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 329 | `StringOption` | `src/main/options.cppm` | grouped | parser/CLI 结构 |
| 330 | `BaseProfiler` | `src/main/profiler.cppm` | mapped | `infinity_executor::profiler::BaseProfiler`（Begin/End/Elapsed/ElapsedToString/BeginTime/EndTime） |
| 331 | `OptimizerProfiler` | `src/main/profiler.cppm` | mapped | `profiler::OptimizerProfiler`（StartRule/StopRule/ToString） |
| 332 | `ProfileHistory` | `src/main/profiler.cppm` | mapped | `profiler::ProfileHistory`（Resize/Enqueue/GetElement/GetElements） |
| 333 | `QueryProfiler` | `src/main/profiler.cppm` | mapped | `profiler::QueryProfiler`（StartPhase/StopPhase/Stop/Flush/ToString/QueryPhaseToString/GetTaskProfile） |
| 334 | `TaskBinding` | `src/main/profiler.cppm` | mapped | `profiler::TaskBinding` |
| 335 | `TaskProfiler` | `src/main/profiler.cppm` | mapped | `profiler::TaskProfiler`（Begin/End/StartOperator/StopOperator） |
| 336 | `QueryContext` | `src/main/query_context.cppm` | grouped | parser/CLI 结构 |
| 337 | `AlterIndexOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 338 | `CreateDatabaseOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 339 | `CreateIndexOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 340 | `CreateTableOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 341 | `DropDatabaseOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 342 | `DropIndexOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 343 | `DropTableOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 344 | `ExportOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 345 | `ImportOptions` | `src/main/query_options.cppm` | grouped | parser/CLI 结构 |
| 346 | `BaseResult` | `src/main/query_result.cppm` | grouped | parser/CLI 结构 |
| 347 | `QueryResult` | `src/main/query_result.cppm` | grouped | parser/CLI 结构 |
| 348 | `ResourceManager` | `src/main/resource_manager.cppm` | grouped | parser/CLI 结构 |
| 349 | `BaseSession` | `src/main/session.cppm` | grouped | parser/CLI 结构 |
| 350 | `LocalSession` | `src/main/session.cppm` | grouped | parser/CLI 结构 |
| 351 | `RemoteSession` | `src/main/session.cppm` | grouped | parser/CLI 结构 |
| 352 | `QueryInfo` | `src/main/session_manager.cppm` | grouped | parser/CLI 结构 |
| 353 | `SessionManager` | `src/main/session_manager.cppm` | grouped | parser/CLI 结构 |
| 354 | `VarUtil` | `src/main/variables.cppm` | mapped | `infinity_executor::variables::{GlobalVariable, SessionVariable}::from_name` |
| 355 | `SystemInfo` | `src/metrics/system_info.cppm` | grouped | 服务端计数 |
| 356 | `BufferReader` | `src/network/buffer_reader.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 357 | `BufferWriter` | `src/network/buffer_writer.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 358 | `Connection` | `src/network/connection.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 359 | `HTTPSearch` | `src/network/http/http_search.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 360 | `HTTPServer` | `src/network/http_server.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 361 | `InfinityThriftService` | `src/network/infinity_thrift_service.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 362 | `NodeInfo` | `src/network/node_info.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 363 | `PeerServerThriftService` | `src/network/peer_server_thrift_service.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 364 | `ChangeRoleTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 365 | `HeartBeatPeerTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 366 | `PeerTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 367 | `RegisterPeerTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 368 | `SyncLogTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 369 | `TerminatePeerTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 370 | `UnregisterPeerTask` | `src/network/peer_task.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 371 | `PeerClient` | `src/network/peer_thrift_client.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 372 | `PoolPeerThriftServer` | `src/network/peer_thrift_server.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 373 | `PGProtocolHandler` | `src/network/pg_protocol_handler.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 374 | `PGServer` | `src/network/pg_server.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 375 | `RingBufferIterator` | `src/network/ring_buffer_iterator.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 376 | `NonBlockPoolThriftServer` | `src/network/thrift_server.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 377 | `PoolThriftServer` | `src/network/thrift_server.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 378 | `ThreadedThriftServer` | `src/network/thrift_server.cppm` | grouped | 生成的 Thrift 类型 / `infinity-server` |
| 379 | `BindContext` | `src/planner/bind_context.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 380 | `CommonTableExpressionInfo` | `src/planner/bind_context.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 381 | `ExpressionBinder` | `src/planner/bind_context.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 382 | `BindAliasProxy` | `src/planner/binder/bind_alias_proxy.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 383 | `GroupBinder` | `src/planner/binder/group_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 384 | `HavingBinder` | `src/planner/binder/having_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 385 | `InsertBinder` | `src/planner/binder/insert_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 386 | `JoinBinder` | `src/planner/binder/join_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 387 | `LimitBinder` | `src/planner/binder/limit_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 388 | `OrderBinder` | `src/planner/binder/order_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 389 | `ProjectBinder` | `src/planner/binder/project_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 390 | `WhereBinder` | `src/planner/binder/where_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 391 | `Binding` | `src/planner/binding.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 392 | `BaseTableRef` | `src/planner/bound/base_table_ref.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 393 | `CrossProductTableRef` | `src/planner/bound/cross_product_table_ref.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 394 | `JoinTableRef` | `src/planner/bound/join_table_ref.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 395 | `SubqueryTableRef` | `src/planner/bound/subquery_table_ref.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 396 | `TableRef` | `src/planner/bound/table_ref.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 397 | `BoundCompactStatement` | `src/planner/bound_compact_statement.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 398 | `BoundDeleteStatement` | `src/planner/bound_delete_statement.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 399 | `BoundInsertStatement` | `src/planner/bound_insert_statement.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 400 | `BoundSelectStatement` | `src/planner/bound_select_statement.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 401 | `BoundStatement` | `src/planner/bound_statement.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 402 | `BoundUpdateStatement` | `src/planner/bound_update_statement.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 403 | `CachedIndexScan` | `src/planner/cached_node/cached_index_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 404 | `CachedMatch` | `src/planner/cached_node/cached_match.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 405 | `CachedKnnScan` | `src/planner/cached_node/cached_match_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 406 | `CachedMatchScanBase` | `src/planner/cached_node/cached_match_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 407 | `CachedMatchSparseScan` | `src/planner/cached_node/cached_match_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 408 | `CachedMatchTensorScan` | `src/planner/cached_node/cached_match_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 409 | `CachedNodeBase` | `src/planner/cached_node/cached_node_base.cppm` | mapped | `infinity_storage::result_cache::CachedNode`（Hash/Eq/output_names/type） |
| 410 | `CachedScanBase` | `src/planner/cached_node/cached_scan_base.cppm` | mapped | `result_cache::CachedNode` 的 `schema_name`/`table_name` |
| 411 | `ColumnBinding` | `src/planner/column_binding.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 412 | `ColumnIdentifier` | `src/planner/column_identifier.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 413 | `ExplainAST` | `src/planner/explain_ast.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 414 | `ExplainLogicalPlan` | `src/planner/explain_logical_plan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 415 | `LoadMeta` | `src/planner/load_meta.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 416 | `LogicalCommonFunctionUsingLoadMeta` | `src/planner/logical_node.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 417 | `LogicalNode` | `src/planner/logical_node.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 418 | `LogicalNodeVisitor` | `src/planner/logical_node_visitor.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 419 | `LogicalPlanner` | `src/planner/logical_planner.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 420 | `LogicalAggregate` | `src/planner/node/logical_aggregate.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 421 | `LogicalAddColumns` | `src/planner/node/logical_alter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 422 | `LogicalAlter` | `src/planner/node/logical_alter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 423 | `LogicalAlterIndex` | `src/planner/node/logical_alter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 424 | `LogicalDropColumns` | `src/planner/node/logical_alter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 425 | `LogicalRenameTable` | `src/planner/node/logical_alter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 426 | `LogicalCheck` | `src/planner/node/logical_check.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 427 | `LogicalCommand` | `src/planner/node/logical_command.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 428 | `LogicalCompact` | `src/planner/node/logical_compact.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 429 | `LogicalCreateCollection` | `src/planner/node/logical_create_collection.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 430 | `LogicalCreateIndex` | `src/planner/node/logical_create_index.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 431 | `LogicalCreateSchema` | `src/planner/node/logical_create_schema.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 432 | `LogicalCreateTable` | `src/planner/node/logical_create_table.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 433 | `LogicalCreateView` | `src/planner/node/logical_create_view.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 434 | `LogicalCrossProduct` | `src/planner/node/logical_cross_product.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 435 | `LogicalDelete` | `src/planner/node/logical_delete.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 436 | `LogicalDropCollection` | `src/planner/node/logical_drop_collection.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 437 | `LogicalDropIndex` | `src/planner/node/logical_drop_index.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 438 | `LogicalDropSchema` | `src/planner/node/logical_drop_schema.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 439 | `LogicalDropTable` | `src/planner/node/logical_drop_table.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 440 | `LogicalDropView` | `src/planner/node/logical_drop_view.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 441 | `LogicalDummyScan` | `src/planner/node/logical_dummy_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 442 | `LogicalExplain` | `src/planner/node/logical_explain.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 443 | `LogicalExport` | `src/planner/node/logical_export.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 444 | `LogicalFilter` | `src/planner/node/logical_filter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 445 | `LogicalFlush` | `src/planner/node/logical_flush.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 446 | `LogicalFusion` | `src/planner/node/logical_fusion.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 447 | `LogicalHashAggregate` | `src/planner/node/logical_hash_aggregate.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 448 | `LogicalImport` | `src/planner/node/logical_import.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 449 | `LogicalIndexScan` | `src/planner/node/logical_index_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 450 | `LogicalInsert` | `src/planner/node/logical_insert.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 451 | `LogicalJoin` | `src/planner/node/logical_join.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 452 | `LogicalLimit` | `src/planner/node/logical_limit.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 453 | `LogicalMatch` | `src/planner/node/logical_match.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 454 | `LogicalKnnScan` | `src/planner/node/logical_match_scan/logical_knn_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 455 | `LogicalMatchScanBase` | `src/planner/node/logical_match_scan/logical_match_scan_base.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 456 | `LogicalMatchSparseScan` | `src/planner/node/logical_match_scan/logical_match_sparse_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 457 | `LogicalMatchTensorScan` | `src/planner/node/logical_match_scan/logical_match_tensor_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 458 | `MatchTensorScanIndexOptions` | `src/planner/node/logical_match_scan/logical_match_tensor_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 459 | `LogicalMergeHashAggregate` | `src/planner/node/logical_merge_hash_aggregate.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 460 | `LogicalOptimize` | `src/planner/node/logical_optimize.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 461 | `LogicalProject` | `src/planner/node/logical_project.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 462 | `CacheContent` | `src/planner/node/logical_read_cache.cppm` | mapped | `result_cache::CacheContent`（AppendColumns/Clone） |
| 463 | `LogicalReadCache` | `src/planner/node/logical_read_cache.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 464 | `LogicalShow` | `src/planner/node/logical_show.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 465 | `LogicalSort` | `src/planner/node/logical_sort.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 466 | `LogicalTableScan` | `src/planner/node/logical_table_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 467 | `LogicalTop` | `src/planner/node/logical_top.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 468 | `LogicalUnnest` | `src/planner/node/logical_unnest.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 469 | `LogicalUnnestAggregate` | `src/planner/node/logical_unnest_aggregate.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 470 | `LogicalUpdate` | `src/planner/node/logical_update.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 471 | `LogicalViewScan` | `src/planner/node/logical_view_scan.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 472 | `Optimizer` | `src/planner/optimizer.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 473 | `ApplyFastRoughFilter` | `src/planner/optimizer/apply_fast_rough_filter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 474 | `ColumnPruner` | `src/planner/optimizer/column_pruner.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 475 | `ColumnRemapper` | `src/planner/optimizer/column_remapper.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 476 | `FilterExpressionPushDown` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 477 | `IndexFilterEvaluator` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 478 | `IndexScanFilterExpressionPushDownResult` | `src/planner/optimizer/index_scan/filter_expression_push_down.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 479 | `FilterExpressionPushDownHelper` | `src/planner/optimizer/index_scan/filter_expression_push_down_helper.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 480 | `IndexFilterEvaluatorAllFalse` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 481 | `IndexFilterEvaluatorAllTrue` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 482 | `IndexFilterEvaluatorFulltext` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 483 | `IndexFilterEvaluatorLogicalChildren` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 484 | `IndexFilterEvaluatorSecondary` | `src/planner/optimizer/index_scan/index_filter_evaluators.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 485 | `ExpressionInfoTree` | `src/planner/optimizer/index_scan/index_filter_expression_info_tree.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 486 | `IndexScanBuilder` | `src/planner/optimizer/index_scan_builder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 487 | `LazyLoad` | `src/planner/optimizer/lazy_load.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 488 | `RegexIndexRewrite` | `src/planner/optimizer/regex_index_rewrite.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 489 | `ResultCacheGetter` | `src/planner/optimizer/result_cache_getter.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 490 | `OptimizerRule` | `src/planner/optimizer_rule.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 491 | `QueryBinder` | `src/planner/query_binder.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 492 | `CorrelatedExpressionsDetector` | `src/planner/subquery/correlated_expressions_detector.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 493 | `DependentJoinFlattener` | `src/planner/subquery/dependent_join_flattener.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 494 | `RewriteCorrelatedExpressions` | `src/planner/subquery/rewrite_correlated_expressions.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 495 | `SubqueryUnnest` | `src/planner/subquery/subquery_unnest.cppm` | grouped | `Statement`/计划结构 + 规则函数 |
| 496 | `FragmentContext` | `src/scheduler/fragment_context.cppm` | mapped | CLASS_MAP 已显式命名 |
| 497 | `Notifier` | `src/scheduler/fragment_context.cppm` | grouped | Tokio（D4 已批准） |
| 498 | `ParallelMaterializedFragmentCtx` | `src/scheduler/fragment_context.cppm` | grouped | Tokio（D4 已批准） |
| 499 | `ParallelStreamFragmentCtx` | `src/scheduler/fragment_context.cppm` | grouped | Tokio（D4 已批准） |
| 500 | `SerialMaterializedFragmentCtx` | `src/scheduler/fragment_context.cppm` | grouped | Tokio（D4 已批准） |
| 501 | `FragmentData` | `src/scheduler/fragment_data.cppm` | grouped | Tokio（D4 已批准） |
| 502 | `FragmentDataBase` | `src/scheduler/fragment_data.cppm` | grouped | Tokio（D4 已批准） |
| 503 | `FragmentError` | `src/scheduler/fragment_data.cppm` | grouped | Tokio（D4 已批准） |
| 504 | `FragmentNone` | `src/scheduler/fragment_data.cppm` | grouped | Tokio（D4 已批准） |
| 505 | `FragmentTask` | `src/scheduler/fragment_task.cppm` | grouped | Tokio（D4 已批准） |
| 506 | `TaskResult` | `src/scheduler/task_result.cppm` | grouped | Tokio（D4 已批准） |
| 507 | `TaskScheduler` | `src/scheduler/task_scheduler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 508 | `BaseTable` | `src/storage/base_table.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 509 | `BGTaskProcessor` | `src/storage/bg_task/background_process.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 510 | `AppendMemIndexBatch` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 511 | `AppendMemIndexTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 512 | `BGTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 513 | `BGTaskInfo` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 514 | `CheckpointTaskBase` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 515 | `CleanupTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 516 | `DumpMemIndexTask` | `src/storage/bg_task/bg_task.cppm` | mapped | CLASS_MAP 已显式命名 |
| 517 | `ManualCompactTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 518 | `ManualOptimizeTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 519 | `NewCheckpointTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 520 | `NotifyCompactTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 521 | `NotifyOptimizeTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 522 | `StopProcessorTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 523 | `TestCommandTask` | `src/storage/bg_task/bg_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 524 | `CompactionProcessor` | `src/storage/bg_task/compaction_process.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 525 | `DumpIndexProcessor` | `src/storage/bg_task/dump_index_process.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 526 | `MemIndexAppender` | `src/storage/bg_task/mem_index_appender.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 527 | `OptimizationProcessor` | `src/storage/bg_task/optimization_process.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 528 | `CheckpointPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 529 | `CleanupPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 530 | `CompactPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 531 | `OptimizeIndexPeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 532 | `PeriodicTrigger` | `src/storage/bg_task/periodic_trigger.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 533 | `PeriodicTriggerThread` | `src/storage/bg_task/periodic_trigger_thread.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 534 | `BuildFastRoughFilterTask` | `src/storage/bg_task/segment_sealing_tasks/build_fast_rough_filter_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 535 | `BuildingSegmentFastFilters` | `src/storage/bg_task/segment_sealing_tasks/build_fast_rough_filter_task.cppm` | grouped | 定时线程 + `Engine::{dump,optimize,cleanup}` |
| 536 | `BufferHandle` | `src/storage/buffer/buffer_handle.cppm` | mapped | CLASS_MAP 已显式命名 |
| 537 | `BufferObj` | `src/storage/buffer/buffer_handle.cppm` | mapped | CLASS_MAP 已显式命名 |
| 538 | `BufferManager` | `src/storage/buffer/buffer_manager.cppm` | mapped | CLASS_MAP 已显式命名 |
| 539 | `BufferObjectInfo` | `src/storage/buffer/buffer_obj.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 540 | `VarBuffer` | `src/storage/buffer/buffer_obj.cppm` | mapped | `infinity_storage::var_buffer::VarBuffer`（Append/Get/Write/TotalSize/GetSize） |
| 541 | `BMPIndexFileWorker` | `src/storage/buffer/file_worker/bmp_index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 542 | `DataFileWorker` | `src/storage/buffer/file_worker/data_file_worker.cppm` | mapped | CLASS_MAP 已显式命名 |
| 543 | `EMVBIndexFileWorker` | `src/storage/buffer/file_worker/emvb_index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 544 | `FileWorker` | `src/storage/buffer/file_worker/file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 545 | `FileWorkerSaveCtx` | `src/storage/buffer/file_worker/file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 546 | `HnswFileWorker` | `src/storage/buffer/file_worker/hnsw_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 547 | `IndexFileWorker` | `src/storage/buffer/file_worker/index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 548 | `IVFIndexFileWorker` | `src/storage/buffer/file_worker/ivf_index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 549 | `PlaidIndexFileWorker` | `src/storage/buffer/file_worker/plaid_index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 550 | `RawFileWorker` | `src/storage/buffer/file_worker/raw_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 551 | `SecondaryIndexFileWorker` | `src/storage/buffer/file_worker/secondary_index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 552 | `SMVEIndexFileWorker` | `src/storage/buffer/file_worker/smve_index_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 553 | `VarFileWorker` | `src/storage/buffer/file_worker/var_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 554 | `VersionFileWorker` | `src/storage/buffer/file_worker/version_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 555 | `VersionFileWorkerSaveCtx` | `src/storage/buffer/file_worker/version_file_worker.cppm` | grouped | `infinity_storage::buffer`（`BufferObj`/`BufferManager`） |
| 556 | `AppendPrepareInfo` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 557 | `DbCache` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 558 | `SegmentCache` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 559 | `SegmentIndexCache` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 560 | `SystemCache` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 561 | `TableCache` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 562 | `TableIndexCache` | `src/storage/catalog/catalog_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 563 | `KVInstance` | `src/storage/catalog/kv_store.cppm` | grouped | `infinity_catalog` + `value_format` |
| 564 | `KVIterator` | `src/storage/catalog/kv_store.cppm` | grouped | `infinity_catalog` + `value_format` |
| 565 | `KVStore` | `src/storage/catalog/kv_store.cppm` | grouped | `infinity_catalog` + `value_format` |
| 566 | `MemIndex` | `src/storage/catalog/mem_index.cppm` | grouped | `infinity_catalog` + `value_format` |
| 567 | `MemIndexID` | `src/storage/catalog/mem_index.cppm` | grouped | `infinity_catalog` + `value_format` |
| 568 | `BlockVersion` | `src/storage/catalog/meta/block_version.cppm` | mapped | `infinity_storage::mvcc::{BlockVersion, CreateField}`（Append/CommitAppend/Delete/RollbackDelete/CheckDelete/GetCreateTS/GetDeleteTS/SpillToFile/LoadFromFile） |
| 569 | `CatalogMeta` | `src/storage/catalog/meta/catalog_meta.cppm` | grouped | `infinity_catalog` + `value_format` |
| 570 | `ChunkIndexMetaInfo` | `src/storage/catalog/meta/chunk_index_meta.cppm` | grouped | `infinity_catalog` + `value_format` |
| 571 | `ChunkIndexSnapshotInfo` | `src/storage/catalog/meta/chunk_index_meta.cppm` | grouped | `infinity_catalog` + `value_format` |
| 572 | `BlockMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 573 | `BlockTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 574 | `ChunkIndexMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 575 | `ChunkIndexTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 576 | `ColumnMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 577 | `DBMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 578 | `DBTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 579 | `DropMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 580 | `MetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 581 | `PmObjectMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 582 | `PmStatMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 583 | `SegmentIndexMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 584 | `SegmentIndexTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 585 | `SegmentMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 586 | `SegmentTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 587 | `SystemTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 588 | `TableColumnMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 589 | `TableIndexMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 590 | `TableIndexTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 591 | `TableMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 592 | `TableNameMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 593 | `TableTagMetaKey` | `src/storage/catalog/meta/meta_key.cppm` | grouped | `infinity_catalog` + `value_format` |
| 594 | `MetaBlockColumnObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 595 | `MetaBlockObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 596 | `MetaChunkIndexObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 597 | `MetaDBObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 598 | `MetaObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 599 | `MetaPmObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 600 | `MetaSegmentIndexObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 601 | `MetaSegmentObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 602 | `MetaTableIndexObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 603 | `MetaTableObject` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 604 | `MetaTree` | `src/storage/catalog/meta/meta_tree.cppm` | grouped | `infinity_catalog` + `value_format` |
| 605 | `SegmentInfo` | `src/storage/catalog/meta/segment_meta.cppm` | grouped | `infinity_catalog` + `value_format` |
| 606 | `CacheInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 607 | `CacheStatus` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 608 | `DBCacheCommentInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 609 | `DBCacheInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 610 | `EraseBaseCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 611 | `IndexCacheIndexDefInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 612 | `IndexCacheInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 613 | `MetaCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 614 | `MetaDbCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 615 | `MetaEraseDbCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 616 | `MetaEraseIndexCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 617 | `MetaEraseTableCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 618 | `MetaIndexCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 619 | `MetaTableCache` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 620 | `TableCacheColumnInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 621 | `TableCacheIndexInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 622 | `TableCacheInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 623 | `TableCacheSegmentInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 624 | `TableCacheSegmentTagInfo` | `src/storage/catalog/meta_cache.cppm` | grouped | `infinity_catalog` + `value_format` |
| 625 | `BlockLock` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 626 | `BlockMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 627 | `ChunkIndexMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 628 | `ColumnMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 629 | `DBMeta` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 630 | `NewTxnBlockVisitor` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 631 | `NewTxnGetVisibleRangeState` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 632 | `SegmentIndexFtInfo` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 633 | `SegmentIndexMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 634 | `SegmentMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 635 | `SegmentUpdateTS` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 636 | `TableIndexMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 637 | `TableLockForMemIndex` | `src/storage/catalog/new_catalog.cppm` | grouped | `infinity_catalog` + `value_format` |
| 638 | `TableMeta` | `src/storage/catalog/new_catalog.cppm` | mapped | CLASS_MAP 已显式命名 |
| 639 | `Allocator` | `src/storage/column_vector/allocator.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 640 | `BinaryOperator` | `src/storage/column_vector/operator/binary_operator.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 641 | `EmbeddingUnaryOperator` | `src/storage/column_vector/operator/embedding_unary_operator.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 642 | `NullaryOperator` | `src/storage/column_vector/operator/nullary_operation.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 643 | `TernaryOperator` | `src/storage/column_vector/operator/ternary_operator.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 644 | `UnaryOperator` | `src/storage/column_vector/operator/unary_operator.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 645 | `Selection` | `src/storage/column_vector/selection.cppm` | mapped | `infinity_storage::selection::{Selection, SelectionData}`（Initialize/Set/Append/Get/operator[]/Capacity/Size/Reset） |
| 646 | `ArrayValueInfo` | `src/storage/column_vector/value.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 647 | `EmbeddingValueInfo` | `src/storage/column_vector/value.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 648 | `JsonValueInfo` | `src/storage/column_vector/value.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 649 | `SparseValueInfo` | `src/storage/column_vector/value.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 650 | `TensorArrayValueInfo` | `src/storage/column_vector/value.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 651 | `Value` | `src/storage/column_vector/value.cppm` | mapped | CLASS_MAP 已显式命名 |
| 652 | `VarBufferManager` | `src/storage/column_vector/var_buffer.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 653 | `VectorBuffer` | `src/storage/column_vector/vector_buffer.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 654 | `VectorHeapChunk` | `src/storage/column_vector/vector_heap_chunk.cppm` | grouped | `infinity_storage::data_block::{ColumnVector, DataBlock}` |
| 655 | `BlockIndex` | `src/storage/common/block_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 656 | `IndexIndex` | `src/storage/common/block_index.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 657 | `NewIndexSnapshot` | `src/storage/common/block_index.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 658 | `NewSegmentSnapshot` | `src/storage/common/block_index.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 659 | `DatabaseDetail` | `src/storage/common/database_detail.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 660 | `FastPForWrapper` | `src/storage/common/fastpfor.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 661 | `GlobalBlockID` | `src/storage/common/global_block_id.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 662 | `GlobalBlockIDHash` | `src/storage/common/global_block_id.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 663 | `IntEncoder` | `src/storage/common/int_encoder.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 664 | `JsonManager` | `src/storage/common/json_manager.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 665 | `KeyBase` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 666 | `KeyDb` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 667 | `KeyDbTag` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 668 | `KeyEncode` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 669 | `KeyIndex` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 670 | `KeyIndexTag` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 671 | `KeyTable` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 672 | `KeyTableTag` | `src/storage/common/kv_code.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 673 | `BlockColumnInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 674 | `BlockInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 675 | `DatabaseInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 676 | `SegmentIndexInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 677 | `TableDetail` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 678 | `TableIndexInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 679 | `TableInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 680 | `ViewDetail` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 681 | `WalChunkIndexInfo` | `src/storage/common/meta_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 682 | `RcuMap` | `src/storage/common/rcu_multimap.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 683 | `RcuMultiMap` | `src/storage/common/rcu_multimap.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 684 | `String2UInt64AddOperator` | `src/storage/common/rocksdb_merge_operator.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 685 | `BlockColumnSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 686 | `BlockSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 687 | `DatabaseSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 688 | `OutlineSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 689 | `SegmentIndexSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 690 | `SegmentSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 691 | `SnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 692 | `SystemSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 693 | `TableIndexSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 694 | `TableSnapshotInfo` | `src/storage/common/snapshot_info.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 695 | `VByteCompressor` | `src/storage/common/vbyte_compressor.cppm` | grouped | `Vec<u8>`/`BufferObj` 等原语 |
| 696 | `NewCompactionAlg` | `src/storage/compaction/new_compaction_alg.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 697 | `DataBlock` | `src/storage/data_block.cppm` | mapped | CLASS_MAP 已显式命名 |
| 698 | `DataTable` | `src/storage/data_table.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 699 | `IndexBase` | `src/storage/definition/index_base.cppm` | mapped | CLASS_MAP 已显式命名 |
| 700 | `IndexBMP` | `src/storage/definition/index_bmp.cppm` | grouped | `infinity_catalog::value_format` |
| 701 | `IndexDiskAnn` | `src/storage/definition/index_diskann.cppm` | grouped | `infinity_catalog::value_format` |
| 702 | `IndexEMVB` | `src/storage/definition/index_emvb.cppm` | grouped | `infinity_catalog::value_format` |
| 703 | `IndexFullText` | `src/storage/definition/index_full_text.cppm` | mapped | CLASS_MAP 已显式命名 |
| 704 | `IndexHnsw` | `src/storage/definition/index_hnsw.cppm` | grouped | `infinity_catalog::value_format` |
| 705 | `LSGConfig` | `src/storage/definition/index_hnsw.cppm` | mapped | CLASS_MAP 已显式命名 |
| 706 | `IndexIVF` | `src/storage/definition/index_ivf.cppm` | grouped | `infinity_catalog::value_format` |
| 707 | `IndexIVFCentroidOption` | `src/storage/definition/index_ivf.cppm` | grouped | `infinity_catalog::value_format` |
| 708 | `IndexIVFOption` | `src/storage/definition/index_ivf.cppm` | grouped | `infinity_catalog::value_format` |
| 709 | `IndexIVFStorageOption` | `src/storage/definition/index_ivf.cppm` | grouped | `infinity_catalog::value_format` |
| 710 | `IndexPLAID` | `src/storage/definition/index_plaid.cppm` | grouped | `infinity_catalog::value_format` |
| 711 | `IndexSecondary` | `src/storage/definition/index_secondary.cppm` | grouped | `infinity_catalog::value_format` |
| 712 | `IndexSecondaryFunctional` | `src/storage/definition/index_secondary_functional.cppm` | grouped | `infinity_catalog::value_format` |
| 713 | `IndexSMVE` | `src/storage/definition/index_smve.cppm` | grouped | `infinity_catalog::value_format` |
| 714 | `TableDef` | `src/storage/definition/table_def.cppm` | mapped | CLASS_MAP 已显式命名 |
| 715 | `BinaryFuse` | `src/storage/fast_rough_filter/binary_fuse_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 716 | `FastRoughFilter` | `src/storage/fast_rough_filter/fast_rough_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 717 | `FastRoughFilterEvaluator` | `src/storage/fast_rough_filter/fast_rough_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 718 | `Overload` | `src/storage/fast_rough_filter/filter_value_type_classification.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 719 | `TypeList` | `src/storage/fast_rough_filter/filter_value_type_classification.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 720 | `InnerMinMaxDataFilterInfo` | `src/storage/fast_rough_filter/min_max_data_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 721 | `InnerMinMaxDataFilterVarcharType` | `src/storage/fast_rough_filter/min_max_data_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 722 | `MinMaxDataFilter` | `src/storage/fast_rough_filter/min_max_data_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 723 | `ProbabilisticDataFilter` | `src/storage/fast_rough_filter/probabilistic_data_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 724 | `ColumnIndexIterator` | `src/storage/invertedindex/column_index_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 725 | `ColumnIndexMerger` | `src/storage/invertedindex/column_index_merger.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 726 | `ColumnIndexReader` | `src/storage/invertedindex/column_index_reader.cppm` | mapped | CLASS_MAP 已显式命名 |
| 727 | `IndexReader` | `src/storage/invertedindex/column_index_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 728 | `TableIndexReaderCache` | `src/storage/invertedindex/column_index_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 729 | `TermDocIterator` | `src/storage/invertedindex/column_index_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 730 | `ColumnInverter` | `src/storage/invertedindex/column_inverter.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 731 | `ColumnInverterJsonArray` | `src/storage/invertedindex/column_inverter_json_array.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 732 | `AhoCorasick` | `src/storage/invertedindex/common/aho_corasick.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 733 | `BufWriter` | `src/storage/invertedindex/common/buf_writer.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 734 | `DirectIO` | `src/storage/invertedindex/common/external_sort_merger.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 735 | `SortMerger` | `src/storage/invertedindex/common/external_sort_merger.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 736 | `TermTuple` | `src/storage/invertedindex/common/external_sort_merger.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 737 | `TermTupleList` | `src/storage/invertedindex/common/external_sort_merger.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 738 | `MapWithLock` | `src/storage/invertedindex/common/map_with_lock.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 739 | `MmapReader` | `src/storage/invertedindex/common/mmap.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 740 | `PriorityQueue` | `src/storage/invertedindex/common/priority_queue.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 741 | `Arena` | `src/storage/invertedindex/common/skiplist.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 742 | `KeyComparator` | `src/storage/invertedindex/common/skiplist.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 743 | `SkipList` | `src/storage/invertedindex/common/skiplist.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 744 | `SkipListNode` | `src/storage/invertedindex/common/skiplist.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 745 | `VectorWithLock` | `src/storage/invertedindex/common/vector_with_lock.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 746 | `DictionaryReader` | `src/storage/invertedindex/dict_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 747 | `DiskIndexSegmentReader` | `src/storage/invertedindex/disk_segment_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 748 | `DocListEncoder` | `src/storage/invertedindex/format/doc_list_encoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 749 | `DocListFormat` | `src/storage/invertedindex/format/doc_list_format_option.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 750 | `DocListFormatOption` | `src/storage/invertedindex/format/doc_list_format_option.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 751 | `DocSkipListFormat` | `src/storage/invertedindex/format/doc_list_format_option.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 752 | `FlushInfo` | `src/storage/invertedindex/format/flush_info.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 753 | `InDocPositionState` | `src/storage/invertedindex/format/in_doc_pos_state.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 754 | `IndexDecoder` | `src/storage/invertedindex/format/index_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 755 | `SkipIndexDecoder` | `src/storage/invertedindex/format/index_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 756 | `InMemDocListDecoder` | `src/storage/invertedindex/format/inmem_doc_list_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 757 | `InMemPositionListDecoder` | `src/storage/invertedindex/format/inmem_position_list_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 758 | `InMemPostingDecoder` | `src/storage/invertedindex/format/inmem_posting_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 759 | `NoCompressIntEncoder` | `src/storage/invertedindex/format/no_compress_encoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 760 | `PositionListDecoder` | `src/storage/invertedindex/format/position_list_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 761 | `PositionListEncoder` | `src/storage/invertedindex/format/position_list_encoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 762 | `PositionListFormat` | `src/storage/invertedindex/format/position_list_format_option.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 763 | `PositionListFormatOption` | `src/storage/invertedindex/format/position_list_format_option.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 764 | `PositionSkipListFormat` | `src/storage/invertedindex/format/position_list_format_option.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 765 | `PostingBuffer` | `src/storage/invertedindex/format/posting_buffer.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 766 | `PostingByteSlice` | `src/storage/invertedindex/format/posting_byte_slice.cppm` | mapped | CLASS_MAP 已显式命名 |
| 767 | `PostingByteSliceReader` | `src/storage/invertedindex/format/posting_byte_slice_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 768 | `PostingDecoder` | `src/storage/invertedindex/format/posting_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 769 | `NoCompressPostingField` | `src/storage/invertedindex/format/posting_field.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 770 | `PostingField` | `src/storage/invertedindex/format/posting_field.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 771 | `PostingFields` | `src/storage/invertedindex/format/posting_field.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 772 | `TypedPostingField` | `src/storage/invertedindex/format/posting_field.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 773 | `VByteCompressPostingField` | `src/storage/invertedindex/format/posting_field.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 774 | `PostingFormat` | `src/storage/invertedindex/format/posting_list_format.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 775 | `PostingFormatOption` | `src/storage/invertedindex/format/posting_list_format.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 776 | `ShortListOptimizeUtil` | `src/storage/invertedindex/format/short_list_optimize_util.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 777 | `SkipListReader` | `src/storage/invertedindex/format/skiplist_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 778 | `SkipListReaderByteSlice` | `src/storage/invertedindex/format/skiplist_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 779 | `SkipListReaderPostingByteSlice` | `src/storage/invertedindex/format/skiplist_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 780 | `SkipListWriter` | `src/storage/invertedindex/format/skiplist_writer.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 781 | `TermMeta` | `src/storage/invertedindex/format/term_meta.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 782 | `TermMetaDumper` | `src/storage/invertedindex/format/term_meta.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 783 | `TermMetaLoader` | `src/storage/invertedindex/format/term_meta.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 784 | `VByteIntEncoder` | `src/storage/invertedindex/format/vbyte_compress_encoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 785 | `FstBuilder` | `src/storage/invertedindex/fst/build.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 786 | `Bound` | `src/storage/invertedindex/fst/fst.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 787 | `Fst` | `src/storage/invertedindex/fst/fst.cppm` | mapped | CLASS_MAP 已显式命名 |
| 788 | `FstStream` | `src/storage/invertedindex/fst/fst.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 789 | `Output` | `src/storage/invertedindex/fst/node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 790 | `Registry` | `src/storage/invertedindex/fst/registry.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 791 | `RegistryEntry` | `src/storage/invertedindex/fst/registry.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 792 | `InDocPositionIterator` | `src/storage/invertedindex/in_doc_pos_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 793 | `InDocStateKeeper` | `src/storage/invertedindex/in_doc_state_keeper.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 794 | `IndexSegmentReader` | `src/storage/invertedindex/index_segment_reader.cppm` | mapped | CLASS_MAP 已显式命名 |
| 795 | `InMemIndexSegmentReader` | `src/storage/invertedindex/inmem_segment_reader.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 796 | `BatchInvertTask` | `src/storage/invertedindex/invert_task.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 797 | `MemoryIndexer` | `src/storage/invertedindex/memory_indexer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 798 | `MultiPostingDecoder` | `src/storage/invertedindex/multi_posting_decoder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 799 | `PostingIterator` | `src/storage/invertedindex/posting_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 800 | `PostingMerger` | `src/storage/invertedindex/posting_merger.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 801 | `PostingWriter` | `src/storage/invertedindex/posting_writer.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 802 | `Ring` | `src/storage/invertedindex/ring.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 803 | `AndIterator` | `src/storage/invertedindex/search/and_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 804 | `AndNotIterator` | `src/storage/invertedindex/search/and_not_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 805 | `BatchOrIterator` | `src/storage/invertedindex/search/batch_or_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 806 | `BlockMaxLeafIterator` | `src/storage/invertedindex/search/blockmax_leaf_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 807 | `BlockMaxWandIterator` | `src/storage/invertedindex/search/blockmax_wand_iterator.cppm` | mapped | CLASS_MAP 已显式命名 |
| 808 | `BM25Ranker` | `src/storage/invertedindex/search/bm25_ranker.cppm` | mapped | CLASS_MAP 已显式命名 |
| 809 | `FullTextColumnLengthReader` | `src/storage/invertedindex/search/column_length_io.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 810 | `DocIterator` | `src/storage/invertedindex/search/doc_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 811 | `DocIteratorEstimateIterateCost` | `src/storage/invertedindex/search/doc_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 812 | `FilterIterator` | `src/storage/invertedindex/search/filter_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 813 | `FilterQueryNode` | `src/storage/invertedindex/search/filter_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 814 | `HighlightInfo` | `src/storage/invertedindex/search/highlighter.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 815 | `Highlighter` | `src/storage/invertedindex/search/highlighter.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 816 | `KeywordIterator` | `src/storage/invertedindex/search/keyword_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 817 | `MinimumShouldMatchIterator` | `src/storage/invertedindex/search/minimum_should_match_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 818 | `MultiDocIterator` | `src/storage/invertedindex/search/multi_doc_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 819 | `MustFirstIterator` | `src/storage/invertedindex/search/must_first_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 820 | `DocIteratorEntry` | `src/storage/invertedindex/search/or_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 821 | `DocIteratorHeap` | `src/storage/invertedindex/search/or_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 822 | `OrIterator` | `src/storage/invertedindex/search/or_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 823 | `BM25Params` | `src/storage/invertedindex/search/parse_fulltext_options.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 824 | `RankFeatureOption` | `src/storage/invertedindex/search/parse_fulltext_options.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 825 | `PhraseDocIterator` | `src/storage/invertedindex/search/phrase_doc_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 826 | `FullTextQueryContext` | `src/storage/invertedindex/search/query_builder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 827 | `QueryBuilder` | `src/storage/invertedindex/search/query_builder.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 828 | `AndNotQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 829 | `AndQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 830 | `CreateSearchParams` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 831 | `KeywordQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 832 | `MultiQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 833 | `NotQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 834 | `OrQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 835 | `PhraseQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 836 | `QueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 837 | `RankFeatureQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 838 | `RankFeaturesQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 839 | `TermQueryNode` | `src/storage/invertedindex/search/query_node.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 840 | `RankFeatureDocIterator` | `src/storage/invertedindex/search/rank_feature_doc_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 841 | `RankFeaturesDocIterator` | `src/storage/invertedindex/search/rank_features_doc_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 842 | `FullTextScoreResultHeap` | `src/storage/invertedindex/search/score_result_heap.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 843 | `ScoreThresholdIterator` | `src/storage/invertedindex/search/score_threshold_iterator.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 844 | `SearchDriver` | `src/storage/invertedindex/search/search_driver.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 845 | `SegmentPosting` | `src/storage/invertedindex/segment_posting.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 846 | `SegmentTermPosting` | `src/storage/invertedindex/segment_term_posting.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 847 | `SegmentTermPostingQueue` | `src/storage/invertedindex/segment_term_posting.cppm` | grouped | `inverted_format`/`fulltext_segment`/`wand`/`analyzer` 函数 |
| 848 | `ByteSliceReader` | `src/storage/io/byte_slice_reader.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 849 | `ByteSliceWriter` | `src/storage/io/byte_slice_writer.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 850 | `FileReader` | `src/storage/io/file_reader.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 851 | `FileWriter` | `src/storage/io/file_writer.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 852 | `LocalFileHandle` | `src/storage/io/local_file_handle.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 853 | `ObjectStorageProcess` | `src/storage/io/object_storage_process.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 854 | `BaseObjectStorageTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 855 | `CopyTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 856 | `DownloadTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 857 | `LocalDropTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 858 | `RemoveTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 859 | `StopObjectStorageProcessTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 860 | `UploadTask` | `src/storage/io/object_storage_task.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 861 | `S3Client` | `src/storage/io/s3_client.cppm` | mapped | CLASS_MAP 已显式命名 |
| 862 | `S3ClientMinio` | `src/storage/io/s3_client_minio.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 863 | `StreamReader` | `src/storage/io/stream_reader.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 864 | `MmapInfo` | `src/storage/io/virtual_store.cppm` | grouped | `std::fs` + `infinity_storage::s3` |
| 865 | `VirtualStore` | `src/storage/io/virtual_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 866 | `FlattenedTerm` | `src/storage/json_index/json_flattener.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 867 | `JsonFlattener` | `src/storage/json_index/json_flattener.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 868 | `EMVBIndex` | `src/storage/knn_index/emvb/emvb_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 869 | `EMVBIndexInMem` | `src/storage/knn_index/emvb/emvb_index_in_mem.cppm` | mapped | CLASS_MAP 已显式命名 |
| 870 | `EMVBSearch` | `src/storage/knn_index/emvb/emvb_search.cppm` | mapped | CLASS_MAP 已显式命名 |
| 871 | `EMVBSharedVec` | `src/storage/knn_index/emvb/emvb_shared_vec.cppm` | grouped | `infinity_index::emvb::*` / `emvb_segment` |
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
| 893 | `KnnDistance` | `src/storage/knn_index/knn_distance.cppm` | grouped | `simsimd`/`vector::distance_*` 距离原语 |
| 894 | `KnnDistanceBase` | `src/storage/knn_index/knn_distance.cppm` | grouped | `simsimd`/`vector::distance_*` 距离原语 |
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
| 906 | `DataStore` | `src/storage/knn_index/knn_hnsw/data_store/data_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 907 | `DataStoreBase` | `src/storage/knn_index/knn_hnsw/data_store/data_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 908 | `DataStoreChunkIter` | `src/storage/knn_index/knn_hnsw/data_store/data_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 909 | `ArrayPtr` | `src/storage/knn_index/knn_hnsw/data_store/data_store_util.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 910 | `PPtr` | `src/storage/knn_index/knn_hnsw/data_store/data_store_util.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 911 | `GraphStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/graph_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 912 | `GraphStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/graph_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 913 | `LVQData` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 914 | `LVQVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 915 | `LVQVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 916 | `LVQVecStoreMetaType` | `src/storage/knn_index/knn_hnsw/data_store/lvq_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 917 | `PlainVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/plain_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 918 | `PlainVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/plain_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 919 | `RabitqQueryData` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 920 | `RabitqStoreData` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 921 | `RabitqVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 922 | `RabitqVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | mapped | CLASS_MAP 已显式命名 |
| 923 | `RabitqVecStoreMetaType` | `src/storage/knn_index/knn_hnsw/data_store/rabitq_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 924 | `SparseVecStoreInner` | `src/storage/knn_index/knn_hnsw/data_store/sparse_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 925 | `SparseVecStoreMeta` | `src/storage/knn_index/knn_hnsw/data_store/sparse_vec_store.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 926 | `LVQCosVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 927 | `LVQIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 928 | `LVQL2VecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 929 | `PlainCosVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 930 | `PlainIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 931 | `PlainL2VecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 932 | `RabitqCosVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 933 | `RabitqIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 934 | `RabitqL2VecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 935 | `SparseIPVecStoreType` | `src/storage/knn_index/knn_hnsw/data_store/vec_store_type.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 936 | `LVQCosCache` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 937 | `LVQCosDist` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 938 | `PlainCosDist` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 939 | `RabitqCosDist` | `src/storage/knn_index/knn_hnsw/dist_func_cos.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 940 | `LVQIPCache` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 941 | `LVQIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 942 | `PlainIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 943 | `RabitqIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_ip.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 944 | `LVQL2Cache` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 945 | `LVQL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 946 | `PlainL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 947 | `PlainLSL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 948 | `RabitqL2Dist` | `src/storage/knn_index/knn_hnsw/dist_func_l2.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 949 | `LSGDistWrapper` | `src/storage/knn_index/knn_hnsw/dist_func_lsg_wrapper.cppm` | mapped | CLASS_MAP 已显式命名 |
| 950 | `SparseIPDist` | `src/storage/knn_index/knn_hnsw/dist_func_sparse_ip.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 951 | `KnnHnsw` | `src/storage/knn_index/knn_hnsw/hnsw_alg.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 952 | `KnnHnswBase` | `src/storage/knn_index/knn_hnsw/hnsw_alg.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 953 | `KnnSearchOption` | `src/storage/knn_index/knn_hnsw/hnsw_alg.cppm` | mapped | CLASS_MAP 已显式命名 |
| 954 | `DenseVectorIter` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 955 | `FilterBase` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 956 | `HnswConfig` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 957 | `HnswInsertConfig` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 958 | `SparseVectorIter` | `src/storage/knn_index/knn_hnsw/hnsw_common.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 959 | `HnswHandler` | `src/storage/knn_index/knn_hnsw/hnsw_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 960 | `HnswIndexInMem` | `src/storage/knn_index/knn_hnsw/hnsw_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 961 | `HnswLSGBuilder` | `src/storage/knn_index/knn_hnsw/hnsw_lsg_builder.cppm` | mapped | CLASS_MAP 已显式命名 |
| 962 | `HnswOptimizeOptions` | `src/storage/knn_index/knn_hnsw/hnsw_util.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 963 | `HnswUtil` | `src/storage/knn_index/knn_hnsw/hnsw_util.cppm` | grouped | `infinity_index::vector::{HnswIndex, Store}` / `hnsw_handler` / `lvq` / `rabitq` / `lsg` |
| 964 | `heap_twin` | `src/storage/knn_index/knn_ivf/heap_twin_operation.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 965 | `heap_twin_multiple` | `src/storage/knn_index/knn_ivf/heap_twin_operation.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 966 | `IVFDataAccessorBase` | `src/storage/knn_index/knn_ivf/ivf_index_data.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 967 | `IVFIndexInChunk` | `src/storage/knn_index/knn_ivf/ivf_index_data.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 968 | `IVFIndexInMem` | `src/storage/knn_index/knn_ivf/ivf_index_data_in_mem.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 969 | `IVF_Search_Handler` | `src/storage/knn_index/knn_ivf/ivf_index_search.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 970 | `IVF_Search_Params` | `src/storage/knn_index/knn_ivf/ivf_index_search.cppm` | grouped | `infinity_index::ivf::IvfIndex` + 执行器 `ivf`/`ivf_mem` |
| 971 | `IVF_Index_Storage` | `src/storage/knn_index/knn_ivf/ivf_index_storage.cppm` | mapped | CLASS_MAP 已显式命名 |
| 972 | `MergeKnnBase` | `src/storage/knn_index/merge_knn.cppm` | grouped | `infinity_index::merge_knn::MergeKnn` |
| 973 | `MergeKnnResultHandler` | `src/storage/knn_index/merge_knn.cppm` | grouped | `infinity_index::merge_knn::MergeKnn` |
| 974 | `MultiVectorResultHandler` | `src/storage/knn_index/multivector/multivector_result_handler.cppm` | grouped | `MultiVectorIndex` / `MvTailEmvb` |
| 975 | `MultiVectorRef` | `src/storage/knn_index/multivector/multivector_util.cppm` | grouped | `MultiVectorIndex` / `MvTailEmvb` |
| 976 | `PlaidGlobalCentroids` | `src/storage/knn_index/plaid/plaid_global_centroids.cppm` | mapped | CLASS_MAP 已显式命名 |
| 977 | `PlaidIndex` | `src/storage/knn_index/plaid/plaid_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 978 | `PlaidIndexDiskMerger` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | mapped | CLASS_MAP 已显式命名 |
| 979 | `PlaidMergeManifest` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | mapped | CLASS_MAP 已显式命名 |
| 980 | `PlaidMergeManifestChunk` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | grouped | `infinity_index::plaid::*` 类族 |
| 981 | `PlaidMergeSizeInfo` | `src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` | grouped | `infinity_index::plaid::*` 类族 |
| 982 | `PlaidIndexInMem` | `src/storage/knn_index/plaid/plaid_index_in_mem.cppm` | mapped | CLASS_MAP 已显式命名 |
| 983 | `PlaidQuantizer` | `src/storage/knn_index/plaid/plaid_quantizer.cppm` | mapped | CLASS_MAP 已显式命名 |
| 984 | `CompareMax` | `src/storage/knn_index/result_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 985 | `CompareMin` | `src/storage/knn_index/result_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 986 | `HeapResultHandler` | `src/storage/knn_index/result_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 987 | `ReservoirResultHandler` | `src/storage/knn_index/result_handler.cppm` | grouped | `merge_knn` 的结果堆 |
| 988 | `SingleBestResultHandler` | `src/storage/knn_index/result_handler.cppm` | grouped | `merge_knn` 的结果堆 |
| 989 | `SMVEBMPCollector` | `src/storage/knn_index/smve/smve_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 990 | `SMVEIndexInMem` | `src/storage/knn_index/smve/smve_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 991 | `SMVESerializedData` | `src/storage/knn_index/smve/smve_index.cppm` | mapped | CLASS_MAP 已显式命名 |
| 992 | `SMVEResult` | `src/storage/knn_index/smve/smve_transform.cppm` | grouped | `infinity_index::smve::{SmveIndex, smve_transform}` |
| 993 | `BMPAlg` | `src/storage/knn_index/sparse/bmp_alg.cppm` | mapped | CLASS_MAP 已显式命名 |
| 994 | `BMPAlgBase` | `src/storage/knn_index/sparse/bmp_alg.cppm` | mapped | CLASS_MAP 已显式命名 |
| 995 | `BlockFwd` | `src/storage/knn_index/sparse/bmp_fwd.cppm` | mapped | CLASS_MAP 已显式命名 |
| 996 | `BlockTerms` | `src/storage/knn_index/sparse/bmp_fwd.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 997 | `TailFwd` | `src/storage/knn_index/sparse/bmp_fwd.cppm` | mapped | CLASS_MAP 已显式命名 |
| 998 | `BMPHandler` | `src/storage/knn_index/sparse/bmp_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 999 | `BMPIndexInMem` | `src/storage/knn_index/sparse/bmp_handler.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1000 | `BMPIvt` | `src/storage/knn_index/sparse/bmp_ivt.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1001 | `BlockData` | `src/storage/knn_index/sparse/bmp_ivt.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1002 | `BMPOptimizeOptions` | `src/storage/knn_index/sparse/bmp_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1003 | `BMPUtil` | `src/storage/knn_index/sparse/bmp_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1004 | `BmpSearchOptions` | `src/storage/knn_index/sparse/bmp_util.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1005 | `VecPtr` | `src/storage/knn_index/sparse/bmp_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1006 | `BPReordering` | `src/storage/knn_index/sparse/bp_reordering.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1007 | `LinScan` | `src/storage/knn_index/sparse/linscan_alg.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1008 | `SparseTestUtil` | `src/storage/knn_index/sparse/sparse_test_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1009 | `SparseMatrix` | `src/storage/knn_index/sparse/sparse_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1010 | `SparseMatrixIter` | `src/storage/knn_index/sparse/sparse_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1011 | `SparseVec` | `src/storage/knn_index/sparse/sparse_util.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1012 | `SparseVecEle` | `src/storage/knn_index/sparse/sparse_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1013 | `SparseVecRef` | `src/storage/knn_index/sparse/sparse_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1014 | `SparseVecUtil` | `src/storage/knn_index/sparse/sparse_util.cppm` | grouped | `infinity_index::sparse::{BmpAlg, BMPIvt, BlockFwd, TailFwd}` + 执行器 `SparseIndex` |
| 1015 | `AccessState` | `src/storage/meta/entry/data_access_state.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1016 | `DeleteState` | `src/storage/meta/entry/data_access_state.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1017 | `MemIndexInserterIter1` | `src/storage/meta/iter/block_column_iter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1018 | `AddColumnsTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1019 | `AlterIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1020 | `AppendTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1021 | `BaseTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1022 | `CheckpointTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1023 | `CleanupTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1024 | `CompactTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1025 | `CreateDBSnapshotTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1026 | `CreateDBTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1027 | `CreateIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1028 | `CreateSystemSnapshotTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1029 | `CreateTableSnapshotTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1030 | `CreateTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1031 | `DeleteTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1032 | `DropColumnsTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1033 | `DropDBTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1034 | `DropIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1035 | `DropTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1036 | `DummyTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1037 | `DumpMemIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1038 | `FlushDataEntry` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1039 | `ImportTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1040 | `MemIndexRange` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1041 | `OptimizeIndexStoreEntry` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1042 | `OptimizeIndexTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1043 | `RenameTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1044 | `RestoreDatabaseTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1045 | `RestoreSystemTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1046 | `RestoreTableTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1047 | `UpdateAppendBlock` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1048 | `UpdateTxnStore` | `src/storage/new_txn/base_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1049 | `CheckpointOption` | `src/storage/new_txn/new_txn.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1050 | `ChunkInfoForCreateIndex` | `src/storage/new_txn/new_txn.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1051 | `NewTxn` | `src/storage/new_txn/new_txn.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1052 | `SnapshotOption` | `src/storage/new_txn/new_txn.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1053 | `NewTxnManager` | `src/storage/new_txn/new_txn_manager.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1054 | `NewTxnStore` | `src/storage/new_txn/new_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1055 | `NewTxnTableStore1` | `src/storage/new_txn/new_txn_store.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1056 | `TxnAllocator` | `src/storage/new_txn/txn_allocator.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1057 | `TxnAllocatorTask` | `src/storage/new_txn/txn_allocator_task.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1058 | `TxnContext` | `src/storage/new_txn/txn_context.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1059 | `TxnCheckpointInfo` | `src/storage/new_txn/txn_info.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1060 | `TxnCleanInfo` | `src/storage/new_txn/txn_info.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1061 | `TxnCompactInfo` | `src/storage/new_txn/txn_info.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1062 | `TxnImportInfo` | `src/storage/new_txn/txn_info.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1063 | `TxnInfo` | `src/storage/new_txn/txn_info.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1064 | `TxnOptimizeInfo` | `src/storage/new_txn/txn_info.cppm` | grouped | `infinity_txn::{Txn, TxnManager}` 操作 |
| 1065 | `ObjectStats` | `src/storage/persistence/obj_stat_accessor.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1066 | `ObjStat` | `src/storage/persistence/obj_status.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1067 | `Range` | `src/storage/persistence/obj_status.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1068 | `PersistResultHandler` | `src/storage/persistence/persist_result_handler.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1069 | `ObjAddr` | `src/storage/persistence/persistence_manager.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1070 | `PersistReadResult` | `src/storage/persistence/persistence_manager.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1071 | `PersistWriteResult` | `src/storage/persistence/persistence_manager.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1072 | `PersistenceManager` | `src/storage/persistence/persistence_manager.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1073 | `CacheOutput` | `src/storage/result_cache_manager.cppm` | mapped | `result_cache::CacheOutput` |
| 1074 | `ResultCacheManager` | `src/storage/result_cache_manager.cppm` | mapped | `result_cache::ResultCacheManager`（AddCache/GetCache/DropTable/ResetCacheNumCapacity/ClearCache） |
| 1075 | `CommonQueryFilter` | `src/storage/secondary_index/common_query_filter.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1076 | `HighCardinalityTag` | `src/storage/secondary_index/secondary_index_data.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1077 | `LowCardinalityTag` | `src/storage/secondary_index/secondary_index_data.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1078 | `SecondaryIndexDataBase` | `src/storage/secondary_index/secondary_index_data.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1079 | `SecondaryIndexInMem` | `src/storage/secondary_index/secondary_index_in_mem.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1080 | `SecondaryPGMIndex` | `src/storage/secondary_index/secondary_index_pgm.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1081 | `Storage` | `src/storage/storage.cppm` | mapped | CLASS_MAP 已显式命名 |
| 1082 | `BaseMemIndex` | `src/storage/tracer/base_memindex.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1083 | `DummyIndexInMem` | `src/storage/tracer/base_memindex.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1084 | `MemUsageChange` | `src/storage/tracer/mem_usage_change.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1085 | `BGMemIndexTracer` | `src/storage/tracer/memindex_tracer.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1086 | `MemIndexDetail` | `src/storage/tracer/memindex_tracer.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1087 | `MemIndexTracer` | `src/storage/tracer/memindex_tracer.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1088 | `MemIndexTracerInfo` | `src/storage/tracer/memindex_tracer.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1089 | `View` | `src/storage/view.cppm` | grouped | `infinity_storage`/执行器的对应模块 |
| 1090 | `BottomExecutor` | `src/storage/wal/bottom_executor.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1091 | `TempWalFileInfo` | `src/storage/wal/log_file.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1092 | `WalFile` | `src/storage/wal/log_file.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1093 | `WalFileInfo` | `src/storage/wal/log_file.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1094 | `WalBlockInfo` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1095 | `WalCmd` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1096 | `WalCmdAddColumnsV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1097 | `WalCmdAlterIndexV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1098 | `WalCmdAppendV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1099 | `WalCmdCheckpointV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1100 | `WalCmdCleanup` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1101 | `WalCmdCompactV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1102 | `WalCmdCreateDBSnapshot` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1103 | `WalCmdCreateDatabaseV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1104 | `WalCmdCreateIndexV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1105 | `WalCmdCreateSystemSnapshot` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1106 | `WalCmdCreateTableSnapshot` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1107 | `WalCmdCreateTableV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1108 | `WalCmdDeleteV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1109 | `WalCmdDropColumnsV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1110 | `WalCmdDropDatabaseV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1111 | `WalCmdDropIndexV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1112 | `WalCmdDropTableV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1113 | `WalCmdDummy` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1114 | `WalCmdDumpIndexV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1115 | `WalCmdImportV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1116 | `WalCmdRenameTableV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1117 | `WalCmdRestoreDatabaseSnapshot` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1118 | `WalCmdRestoreSystemSnapshot` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1119 | `WalCmdRestoreTableSnapshot` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1120 | `WalEntry` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1121 | `WalEntryHeader` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1122 | `WalEntryIterator` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1123 | `WalListIterator` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1124 | `WalRestoreIndexV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1125 | `WalSegmentIndexInfo` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1126 | `WalSegmentInfo` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1127 | `WalSegmentInfoV2` | `src/storage/wal/wal_entry.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1128 | `ReplayWalOptions` | `src/storage/wal/wal_manager.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1129 | `WalManager` | `src/storage/wal/wal_manager.cppm` | grouped | `infinity_storage::wal` 字节级日志 |
| 1130 | `BaseTestNoParam` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1131 | `BaseTestParamStr` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1132 | `BaseTestWithParam` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1133 | `NewBaseTestNoParam` | `src/unit_test/base_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1134 | `NewReplayTest` | `src/unit_test/storage/new_catalog/replay_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1135 | `NewRequestTest` | `src/unit_test/storage/new_request/request_test.cppm` | test | C++ 测试夹具（`src/unit_test`） |
| 1136 | `SQLRunner` | `src/unit_test/test_helper/sql_runner.cppm` | test | C++ 测试夹具（`src/unit_test`） |
