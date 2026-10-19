# C++ 单元测试覆盖审计

`src/unit_test/**/*.cpp` 中 `TEST/TEST_F/TEST_P` 共 **1012** 个，状态分布（逐行重算）：`excluded` 48, `exempt` 13, `ported` 951。

- `ported`：Rust 已逐条移植同等场景。
- `partial`：Rust 类已存在、部分场景已覆盖，剩余用例待补（如 `column_vector/*` 的完整 `ColumnVector` API、HNSW 内部类）。
- `gate-a`：parser/planner/executor/function，当前由 Gate A 的 SQL 用例覆盖，单元断言待移植。
- `integration`：catalog/txn/wal/io/bg_task，当前由 Gate C 的 restart/快照集成用例覆盖，单元断言待移植。
- `excluded`：DiskANN（计划 §2.6/R14 排除）以及 geo 类型（`Point/Line/Box/Path/Polygon/Circle`）相关用例——用户确认不移植这些类型。
- `exempt`：SIMD/第三方夹具、整段或整块位于 `#if 0` 的用例（C++ 构建中不存在），以及已登记为设计变更的组件——`PostingByteSlice`/`Skiplist`/`PForDelta` 压缩原语、RocksDB 事务型 KV、无 WAL 的 checkpoint 模式、后台任务线程池等（后三项已补齐实现，见 ported）。
- `gap`：尚未移植。
- 带 `#[ignore]` 的 `ported` 行：C++ 的 `DISABLED_*` 用例，Rust 用 `#[ignore]` 表达，需 `cargo test -- --ignored` 显式运行。

| # | 文件 | Fixture | 用例 | 状态 |
|---:|---|---|---|---|
| 1 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_default_is_english_and_coarse` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_default_is_english_and_coarse` |
| 2 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_fine_keeps_english_stemming` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_fine_keeps_english_stemming` |
| 3 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_slovak_folds_and_disables_stemming` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_slovak_folds_and_disables_stemming` |
| 4 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_czech_folds_and_disables_stemming` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_czech_folds_and_disables_stemming` |
| 5 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_language_and_fine_in_both_orders` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_language_and_fine_in_both_orders` |
| 6 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_unknown_segment_falls_back_to_legacy` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_unknown_segment_falls_back_to_legacy` |
| 7 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_analyzers_are_independent` | ported | `infinity-executor` `analyzer::analyzer_pool_tests::test_rag_analyzers_are_independent` |
| 8 | `src/unit_test/common/analyzer/chinese_analyzer_ut.cpp` | `ChineseAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::chinese_analyzer_tests::test1` |
| 9 | `src/unit_test/common/analyzer/ik_analyzer_ut.cpp` | `IKAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::ik_analyzer_tests::test1` |
| 10 | `src/unit_test/common/analyzer/ngram_analyzer_ut.cpp` | `NGramAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::tests::ngram_two_slides_over_words` |
| 11 | `src/unit_test/common/analyzer/ngram_analyzer_ut.cpp` | `NGramAnalyzerTest` | `test2` | ported | `infinity-executor` `analyzer::tests::ngram_one_emits_each_character` |
| 12 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_analyze_enable_position` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_analyze_enable_position` |
| 13 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_analyze_enable_position_fine_grained` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_analyze_enable_position_fine_grained` |
| 14 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_tokenize_consistency_with_posiiton` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_tokenize_consistency_with_posiiton` |
| 15 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_tokenize_consistency_with_python` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_tokenize_consistency_with_python` |
| 16 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_fine_grained_tokenize_consistency_with_python` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_fine_grained_tokenize_consistency_with_python` |
| 17 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_set_language_dutch` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_set_language_dutch` |
| 18 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_set_language_slovak` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_set_language_slovak` |
| 19 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_set_language_czech` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_set_language_czech` |
| 20 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_slovak_no_stemming` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_slovak_no_stemming` |
| 21 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_diacritics_folding_table` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_diacritics_folding_table` |
| 22 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_out_of_range_diacritics_parity_with_python` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_out_of_range_diacritics_parity_with_python` |
| 23 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_slovak_tokenize_with_position` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_slovak_tokenize_with_position` |
| 24 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_chinese_stopword_filter` | ported | `infinity-executor` `analyzer::rag_analyzer_tests::test_chinese_stopword_filter` |
| 25 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `unicode_replacement_preserves_byte_positions` | ported | `infinity-executor` `analyzer::rag_regex_tests::unicode_replacement_preserves_byte_positions` |
| 26 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `empty_and_unmatched_input` | ported | `infinity-executor` `analyzer::rag_regex_tests::empty_and_unmatched_input` |
| 27 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `invalid_utf8_is_checked_before_any_replacement` | ported | `infinity-executor` `analyzer::rag_regex_tests::invalid_utf8_is_checked_before_any_replacement` |
| 28 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `non_character_offsets_remain_checked` | ported | `infinity-executor` `analyzer::rag_regex_tests::non_character_offsets_remain_checked` |
| 29 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `large_html_replacement` | ported | `infinity-executor` `analyzer::rag_regex_tests::large_html_replacement` |
| 30 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_lowercases_without_stemming` |
| 31 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test2` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_keeps_the_source_case` |
| 32 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test3` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_emits_word_and_stem` |
| 33 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test4` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_emits_case_and_lower_forms` |
| 34 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test5` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_deduplicates_the_stem` |
| 35 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test6` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_honours_the_tokenizer_config` |
| 36 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test7` | ported | `infinity-executor` `analyzer::tests::standard_analyzer_handles_a_large_extract` |
| 37 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_analyzer_test1` |
| 38 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test2` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_analyzer_test2` |
| 39 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test3` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_analyzer_test3` |
| 40 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test4` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_analyzer_test4` |
| 41 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test5` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_analyzer_test5` |
| 42 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test1` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_term_weight_test1` |
| 43 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test2` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_term_weight_test2` |
| 44 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test3` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_term_weight_test3` |
| 45 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test4` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_term_weight_test4` |
| 46 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test5` | ported | `infinity-executor` `analyzer::user_defined_reference_suite::user_defined_term_weight_test5` |
| 47 | `src/unit_test/common/test_simd_ut.cpp` | `SimdInitTest` | `GetSupportedSimdTypesList` | ported | `infinity-common` `simd::tests::simd_supported_types_list` |
| 48 | `src/unit_test/common/utility/exception_ut.cpp` | `ExceptionTest` | `all_exception_cases` | ported | `infinity-common` `error::tests::exception_messages_match_the_reference` |
| 49 | `src/unit_test/common/utility/trim_ut.cpp` | `TrimTest` | `test1` | ported | `infinity-common` `utility::tests::trim_path_keeps_the_repository_relative_part` |
| 50 | `src/unit_test/common/utility/utility_ut.cpp` | `UtilityTest` | `test1` | ported | `utility::tests::extract_u64_reads_the_key_suffix` |
| 51 | `src/unit_test/executor/expression/expression_executor_select_ut.cpp` | `ExpressionExecutorSelectTest` | `test1` | ported | `infinity-storage` `data_block::reference_suite::expression_select_alternating_input` |
| 52 | `src/unit_test/executor/expression/expression_executor_select_ut.cpp` | `ExpressionExecutorSelectTest` | `test2` | ported | `infinity-storage` `data_block::reference_suite::expression_select_all_true_input` |
| 53 | `src/unit_test/executor/expression/expression_executor_select_ut.cpp` | `ExpressionExecutorSelectTest` | `test3` | ported | `infinity-storage` `data_block::reference_suite::expression_select_all_false_input` |
| 54 | `src/unit_test/executor/expression/new_expression_evaluator_ut.cpp` | `ExpressionEvaluatorTest` | `add_bigint_constant_1` | ported | `infinity-expression` `tests::expression_evaluator_add_bigint_constant_1` |
| 55 | `src/unit_test/executor/expression/new_expression_evaluator_ut.cpp` | `ExpressionEvaluatorTest` | `subtract_constant_8192_bigint` | ported | `infinity-expression` `tests::expression_evaluator_subtract_constant_8192_bigint` |
| 56 | `src/unit_test/executor/fragment/plan_fragment_ut.cpp` | `PlanFragmentTest` | `test1` | ported | `infinity-executor` `gate_b_executor_ut::plan_fragment_test1` |
| 57 | `src/unit_test/executor/operator/physical_sort_ut_nimpl.cpp` | `PhysicalSortTest` | `test1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 58 | `src/unit_test/executor/pipeline/explain_ut.cpp` | `ExplainTest` | `test1` | ported | `infinity-executor` `explain::tests::explain_align_paragraphs` |
| 59 | `src/unit_test/executor/pipeline/fragment_ut.cpp` | `FragmentTest` | `test_build_fragment` | ported | `infinity-executor` `gate_b_executor_ut::fragment_test_build_fragment` |
| 60 | `src/unit_test/function/aggregate/avg_functions_ut.cpp` | `AvgFunctionTest` | `avg_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 61 | `src/unit_test/function/aggregate/count_functions_ut.cpp` | `CountFunctionTest` | `quarter_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 62 | `src/unit_test/function/aggregate/first_functions_ut.cpp` | `FirstFunctionTest` | `first_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 63 | `src/unit_test/function/aggregate/max_functions_ut.cpp` | `MaxFunctionTest` | `max_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 64 | `src/unit_test/function/aggregate/min_functions_ut.cpp` | `MinFunctionTest` | `min_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 65 | `src/unit_test/function/aggregate/sum_functions_ut.cpp` | `SumFunctionTest` | `sum_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 66 | `src/unit_test/function/cast/bitmap_cast_ut_nimpl.cpp` | `BitmapCastTest` | `bitmap_cast0` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 67 | `src/unit_test/function/cast/bitmap_cast_ut_nimpl.cpp` | `BitmapCastTest` | `bitmap_cast1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 68 | `src/unit_test/function/cast/bool_cast_ut.cpp` | `BoolCastTest` | `bool_cast0` | ported | `infinity-common` `cast::tests::bool_cast0` |
| 69 | `src/unit_test/function/cast/bool_cast_ut.cpp` | `BoolCastTest` | `bool_cast1` | ported | `infinity-common` `cast::tests::bool_cast1` |
| 70 | `src/unit_test/function/cast/cast_table_ut.cpp` | `CastTableTest` | `casttable_boolean` | ported | `infinity-common` `cast::tests::casttable_boolean` |
| 71 | `src/unit_test/function/cast/datetime/date_cast_ut.cpp` | `DateCastTest` | `date_cast0` | ported | `infinity-common` `cast::tests::date_cast0` |
| 72 | `src/unit_test/function/cast/datetime/date_cast_ut.cpp` | `DateCastTest` | `date_cast1` | ported | `infinity-common` `cast::tests::date_cast1` |
| 73 | `src/unit_test/function/cast/datetime/datetime_cast_ut.cpp` | `DateTimeCastTest` | `datetime_cast0` | ported | `infinity-common` `cast::tests::datetime_cast0` |
| 74 | `src/unit_test/function/cast/datetime/datetime_cast_ut.cpp` | `DateTimeCastTest` | `datetime_cast1` | ported | `infinity-common` `cast::tests::datetime_cast1` |
| 75 | `src/unit_test/function/cast/datetime/interval_cast_ut.cpp` | `IntervalCastTest` | `date_cast0` | ported | `infinity-common` `cast::tests::interval_cast0` |
| 76 | `src/unit_test/function/cast/datetime/interval_cast_ut.cpp` | `IntervalCastTest` | `date_cast1` | ported | `infinity-common` `cast::tests::interval_cast1` |
| 77 | `src/unit_test/function/cast/datetime/time_cast_ut.cpp` | `TimeCastTest` | `date_cast0` | ported | `infinity-common` `cast::tests::time_cast0` |
| 78 | `src/unit_test/function/cast/datetime/time_cast_ut.cpp` | `TimeCastTest` | `date_cast1` | ported | `infinity-common` `cast::tests::time_cast1` |
| 79 | `src/unit_test/function/cast/datetime/timestamp_cast_ut.cpp` | `TimestampCastTest` | `datetime_cast0` | ported | `infinity-common` `cast::tests::timestamp_cast0` |
| 80 | `src/unit_test/function/cast/datetime/timestamp_cast_ut.cpp` | `TimestampCastTest` | `datetime_cast1` | ported | `infinity-common` `cast::tests::timestamp_cast1` |
| 81 | `src/unit_test/function/cast/decimal/decimal_cast_ut.cpp` | `DecimalCastTest` | `decimal_cast0` | ported | `infinity-common` `cast::tests::decimal_cast0` |
| 82 | `src/unit_test/function/cast/decimal/decimal_cast_ut.cpp` | `DecimalCastTest` | `decimal_cast1` | ported | `infinity-common` `cast::tests::decimal_cast1` |
| 83 | `src/unit_test/function/cast/embedding_cast_ut.cpp` | `EmbeddingCastTest` | `embedding_cast1` | ported | `infinity-common` `cast::tests::embedding_cast1` |
| 84 | `src/unit_test/function/cast/float/double_cast_ut.cpp` | `DoubleCastTest` | `double_cast0` | ported | `infinity-common` `cast::tests::double_cast0` |
| 85 | `src/unit_test/function/cast/float/double_cast_ut.cpp` | `DoubleCastTest` | `double_cast1` | ported | `infinity-common` `cast::tests::double_cast1` |
| 86 | `src/unit_test/function/cast/float/float_cast_ut.cpp` | `FloatCastTest` | `float_cast0` | ported | `infinity-common` `cast::tests::float_cast0` |
| 87 | `src/unit_test/function/cast/float/float_cast_ut.cpp` | `FloatCastTest` | `float_cast1` | ported | `infinity-common` `cast::tests::float_cast1` |
| 88 | `src/unit_test/function/cast/geography/box_cast_ut.cpp` | `BoxCastTest` | `line_seg_cast0` | excluded |
| 89 | `src/unit_test/function/cast/geography/box_cast_ut.cpp` | `BoxCastTest` | `line_seg_cast1` | excluded |
| 90 | `src/unit_test/function/cast/geography/circle_cast_ut.cpp` | `CircleCastTest` | `circle_cast0` | excluded |
| 91 | `src/unit_test/function/cast/geography/circle_cast_ut.cpp` | `CircleCastTest` | `circle_cast1` | excluded |
| 92 | `src/unit_test/function/cast/geography/line_cast_ut.cpp` | `LineCastTest` | `line_cast0` | excluded |
| 93 | `src/unit_test/function/cast/geography/line_cast_ut.cpp` | `LineCastTest` | `line_cast1` | excluded |
| 94 | `src/unit_test/function/cast/geography/line_segment_cast_ut.cpp` | `LineSegCastTest` | `line_seg_cast0` | excluded |
| 95 | `src/unit_test/function/cast/geography/line_segment_cast_ut.cpp` | `LineSegCastTest` | `line_seg_cast1` | excluded |
| 96 | `src/unit_test/function/cast/geography/path_cast_ut_nimpl.cpp` | `PathCastTest` | `path_cast0` | excluded |
| 97 | `src/unit_test/function/cast/geography/path_cast_ut_nimpl.cpp` | `PathCastTest` | `path_cast1` | excluded |
| 98 | `src/unit_test/function/cast/geography/point_cast_ut.cpp` | `PointCastTest` | `point_cast0` | excluded |
| 99 | `src/unit_test/function/cast/geography/point_cast_ut.cpp` | `PointCastTest` | `point_cast1` | excluded |
| 100 | `src/unit_test/function/cast/geography/polygon_cast_ut_nimpl.cpp` | `PolygonCastTest` | `polygon_cast0` | excluded |
| 101 | `src/unit_test/function/cast/geography/polygon_cast_ut_nimpl.cpp` | `PolygonCastTest` | `polygon_cast1` | excluded |
| 102 | `src/unit_test/function/cast/integer_cast/bigint_cast_ut.cpp` | `BigIntCastTest` | `bigint_cast0` | ported | `infinity-common` `cast::tests::bigint_cast0` |
| 103 | `src/unit_test/function/cast/integer_cast/bigint_cast_ut.cpp` | `BigIntCastTest` | `bigint_cast1` | ported | `infinity-common` `cast::tests::bigint_cast1` |
| 104 | `src/unit_test/function/cast/integer_cast/hugeint_cast_ut.cpp` | `HugeIntCastTest` | `hugeint_cast0` | ported | `infinity-common` `cast::tests::hugeint_cast0` |
| 105 | `src/unit_test/function/cast/integer_cast/hugeint_cast_ut.cpp` | `HugeIntCastTest` | `hugeint_cast1` | ported | `infinity-common` `cast::tests::hugeint_cast1` |
| 106 | `src/unit_test/function/cast/integer_cast/integer_cast_ut.cpp` | `IntegerCastTest` | `integer_cast0` | ported | `infinity-common` `cast::tests::integer_cast0` |
| 107 | `src/unit_test/function/cast/integer_cast/integer_cast_ut.cpp` | `IntegerCastTest` | `integer_cast1` | ported | `infinity-common` `cast::tests::integer_cast1` |
| 108 | `src/unit_test/function/cast/integer_cast/small_integer_cast_ut.cpp` | `SmallIntegerCastTest` | `small_integer_cast0` | ported | `infinity-common` `cast::tests::small_integer_cast0` |
| 109 | `src/unit_test/function/cast/integer_cast/small_integer_cast_ut.cpp` | `SmallIntegerCastTest` | `small_integer_cast1` | ported | `infinity-common` `cast::tests::small_integer_cast1` |
| 110 | `src/unit_test/function/cast/integer_cast/tiny_integer_cast_ut.cpp` | `TinyIntegerCastTest` | `tiny_integer_cast0` | ported | `infinity-common` `cast::tests::tiny_integer_cast0` |
| 111 | `src/unit_test/function/cast/integer_cast/tiny_integer_cast_ut.cpp` | `TinyIntegerCastTest` | `tiny_integer_cast1` | ported | `infinity-common` `cast::tests::tiny_integer_cast1` |
| 112 | `src/unit_test/function/cast/uuid_cast_ut_nimpl.cpp` | `UuidCastTest` | `uuid_cast0` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 113 | `src/unit_test/function/cast/uuid_cast_ut_nimpl.cpp` | `UuidCastTest` | `uuid_cast1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 114 | `src/unit_test/function/cast/varchar_cast_ut.cpp` | `VarcharCastTest` | `varchar_cast0` | ported | `infinity-common` `cast::tests::varchar_cast0` |
| 115 | `src/unit_test/function/cast/varchar_cast_ut.cpp` | `VarcharCastTest` | `varchar_cast1` | ported | `infinity-common` `cast::tests::varchar_cast1` |
| 116 | `src/unit_test/function/scalar/abs_functions_ut.cpp` | `AbsFunctionsTest` | `abs_func` | ported | `infinity-executor` `tests::abs_function_covers_every_numeric_type` |
| 117 | `src/unit_test/function/scalar/add_functions_ut.cpp` | `AddFunctionsTest` | `add_func` | ported | `infinity-expression` `scalar::tests::add_func` |
| 118 | `src/unit_test/function/scalar/and_functions_ut.cpp` | `AndFunctionsTest` | `and_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 119 | `src/unit_test/function/scalar/century_functions_ut.cpp` | `CenturyFunctionsTest` | `century_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 120 | `src/unit_test/function/scalar/current_date_functions_ut.cpp` | `CurrentDateFunctionsTest` | `current_date_func` | ported | `infinity-expression` `scalar::tests::current_date_func` |
| 121 | `src/unit_test/function/scalar/current_time_functions_ut.cpp` | `CurrentTimeFunctionsTest` | `current_time_func` | ported | `infinity-expression` `scalar::tests::current_time_func` |
| 122 | `src/unit_test/function/scalar/current_timestamp_functions_ut.cpp` | `CurrentTimestampFunctionsTest` | `current_timestamp_func` | ported | `infinity-expression` `scalar::tests::current_timestamp_func` |
| 123 | `src/unit_test/function/scalar/date_part_functions_ut.cpp` | `DatePartFunctionsTest` | `datepart_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 124 | `src/unit_test/function/scalar/day_functions_ut.cpp` | `DayFunctionsTest` | `day_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 125 | `src/unit_test/function/scalar/day_of_month_functions_ut.cpp` | `DayOfMonthFunctionsTest` | `day_of_month_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 126 | `src/unit_test/function/scalar/day_of_week_ut.cpp` | `DayOfWeekFunctionsTest` | `day_of_week_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 127 | `src/unit_test/function/scalar/day_of_year_functions_ut.cpp` | `DayOfYearFunctionsTest` | `day_of_year_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 128 | `src/unit_test/function/scalar/div_functions_ut.cpp` | `DivFunctionsTest` | `div_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 129 | `src/unit_test/function/scalar/epoch_functions_ut.cpp` | `EpochFunctionsTest` | `epoch_func` | ported | `infinity-expression` `scalar::tests::epoch_func` |
| 130 | `src/unit_test/function/scalar/equals_functions_ut.cpp` | `EqualsFunctionsTest` | `equals_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 131 | `src/unit_test/function/scalar/era_functions_ut.cpp` | `EraFunctionsTest` | `era_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 132 | `src/unit_test/function/scalar/extract_function_ut.cpp` | `ExtractFunctionTest` | `extract_year_test` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 133 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `config_test` | ported | `infinity-expression` `fde::tests::config_test` |
| 134 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `tensor_test` | ported | `infinity-expression` `fde::tests::tensor_test` |
| 135 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `random_generator_test` | ported | `infinity-expression` `fde::tests::random_generator_test` |
| 136 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `hash_partitioner_test` | ported | `infinity-expression` `fde::tests::hash_partitioner_test` |
| 137 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `sparse_projector_test` | ported | `infinity-expression` `fde::tests::sparse_projector_test` |
| 138 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `internal_functions_test` | ported | `infinity-expression` `fde::tests::internal_functions_test` |
| 139 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `simple_encoding_test` | ported | `infinity-expression` `fde::tests::simple_encoding_test` |
| 140 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `invalid_input_test` | ported | `infinity-expression` `fde::tests::invalid_input_test` |
| 141 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `document_encoding_test` | ported | `infinity-expression` `fde::tests::document_encoding_test` |
| 142 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `simhash_partitioning_test` | ported | `infinity-expression` `fde::tests::simhash_partitioning_test` |
| 143 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `ams_projection_test` | ported | `infinity-expression` `fde::tests::ams_projection_test` |
| 144 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `final_projection_test` | ported | `infinity-expression` `fde::tests::final_projection_test` |
| 145 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `multiple_repetitions_test` | ported | `infinity-expression` `fde::tests::multiple_repetitions_test` |
| 146 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `fill_empty_partitions_test` | ported | `infinity-expression` `fde::tests::fill_empty_partitions_test` |
| 147 | `src/unit_test/function/scalar/greater_equals_functions_ut.cpp` | `GreaterEqualsFunctionsTest` | `greater_equals_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 148 | `src/unit_test/function/scalar/greater_functions_ut.cpp` | `GreaterFunctionsTest` | `greater_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 149 | `src/unit_test/function/scalar/hour_functions_ut.cpp` | `HourFunctionsTest` | `hour_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 150 | `src/unit_test/function/scalar/inequals_functions_ut.cpp` | `InEqualsFunctionsTest` | `inequals_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 151 | `src/unit_test/function/scalar/less_equals_functions_ut.cpp` | `LessEqualsFunctionsTest` | `less_equals_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 152 | `src/unit_test/function/scalar/less_functions_ut.cpp` | `LessFunctionsTest` | `less_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 153 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `re2_quote_meta_test` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 154 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_special_regex_chars_escape` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 155 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_basic_sanity` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 156 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_percent_wildcard` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 157 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_underscore_wildcard` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 158 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_escape_character` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 159 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_custom_escape_char` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 160 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_empty_pattern` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 161 | `src/unit_test/function/scalar/like_functions_ut.cpp` | `LikeFunctionsTest` | `like_to_regex_pattern_complex_real_world_patterns` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 162 | `src/unit_test/function/scalar/minus_functions_ut.cpp` | `MinusFunctionsTest` | `plus_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 163 | `src/unit_test/function/scalar/minute_function_ut.cpp` | `MinuteFunctionsTest` | `minute_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 164 | `src/unit_test/function/scalar/modulo_functions_ut.cpp` | `ModuloFunctionsTest` | `mod_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 165 | `src/unit_test/function/scalar/month_functions_ut.cpp` | `MonthFunctionsTest` | `month_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 166 | `src/unit_test/function/scalar/mul_functions_ut.cpp` | `MulFunctionsTest` | `mul_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 167 | `src/unit_test/function/scalar/not_functions_ut.cpp` | `NotFunctionsTest` | `not_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 168 | `src/unit_test/function/scalar/or_functions_ut.cpp` | `OrFunctionsTest` | `or_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 169 | `src/unit_test/function/scalar/or_functions_ut.cpp` | `OrFunctionsTest` | `or_func_nullable_flat` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 170 | `src/unit_test/function/scalar/or_functions_ut.cpp` | `OrFunctionsTest` | `or_func_nullable_constant` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 171 | `src/unit_test/function/scalar/plus_functions_ut.cpp` | `PlusFunctionsTest` | `plus_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 172 | `src/unit_test/function/scalar/pow_functions_ut.cpp` | `PowFunctionsTest` | `mul_func` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 173 | `src/unit_test/function/scalar/quarter_functions_ut.cpp` | `QuarterFunctionsTest` | `quarter_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 174 | `src/unit_test/function/scalar/reverse_functions_ut.cpp` | `ReverseFunctionsTest` | `reverse_func` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 175 | `src/unit_test/function/scalar/second_function_ut.cpp` | `SecondFunctionsTest` | `second_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 176 | `src/unit_test/function/scalar/substr_function_ut_nimpl.cpp` | `SubstrFunctionTest` | `varchar_substr` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 177 | `src/unit_test/function/scalar/subtract_functions_ut.cpp` | `SubtractFunctionsTest` | `sub_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 178 | `src/unit_test/function/scalar/truncate_functions_ut.cpp` | `TruncateFunctionsTest` | `truncate_func` | ported | `infinity-executor` `tests::string_and_power_functions_match_the_reference` |
| 179 | `src/unit_test/function/scalar/week_of_year_functions_ut.cpp` | `WeekOfYearFunctionsTest` | `week_of_year_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 180 | `src/unit_test/function/scalar/weekday_functions_ut.cpp` | `WeekdayFunctionsTest` | `weekday_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 181 | `src/unit_test/function/scalar/year_functions_ut.cpp` | `YearFunctionsTest` | `year_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 182 | `src/unit_test/infinity_cloud/upload_meta_to_s3_ut.cpp` | `UploadMetaToS3` | `DISABLED_MINIO_test1` | ported | `infinity-executor` `upload_meta_to_s3_ut::minio_upload_meta_to_s3` (`#[ignore]`: C++ DISABLED_MINIO_test1) |
| 183 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `test1` | ported | `infinity-cli` `config::tests::defaults` |
| 184 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `test2` | ported | `infinity-cli` `config::tests::full_config_file` |
| 185 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestWrongParamNames` | ported | `infinity-cli` `config::tests::wrong_parameter_names` |
| 186 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestConfInvalidValues` | ported | `infinity-cli` `config::tests::invalid_values` |
| 187 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestOutofRangeValues` | ported | `infinity-cli` `config::tests::out_of_range_values` |
| 188 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestValidValues` | ported | `infinity-cli` `config::tests::valid_values` |
| 189 | `src/unit_test/main/infinity_ut.cpp` | `InfinityTest` | `test1` | ported | `infinity-executor` `tests::catalog_lifecycle_matches_the_reference_test` |
| 190 | `src/unit_test/main/infinity_ut.cpp` | `InfinityTest` | `test2` | ported | `infinity-executor` `tests::show_variable_covers_the_reference_names` |
| 191 | `src/unit_test/main/query_profiler_ut.cpp` | `QueryProfilerTest` | `test1` | ported | `infinity-executor` `profiler::tests::query_phase_names_match_the_reference` |
| 192 | `src/unit_test/main/query_profiler_ut.cpp` | `QueryProfilerTest` | `test2` | ported | `profiler::tests::disabled_profiler_ignores_phase_transitions` + `enabled_profiler_enforces_phase_order` |
| 193 | `src/unit_test/main/query_profiler_ut.cpp` | `QueryProfilerTest` | `test3` | ported | `profiler::tests::all_phases_and_optimizer_rules_record_time` |
| 194 | `src/unit_test/main/stats/global_resource_usage_ut.cpp` | `GlobalResourceUsageTest` | `usage_test` | ported | `infinity-executor` `global_resource_usage::reference_suite::usage_test` |
| 195 | `src/unit_test/main/table_ut.cpp` | `InfinityTableTest` | `test1` | ported | `infinity-executor` `tests::explain_covers_every_plan_kind`（建库/取表/插入/查询 + 六种 EXPLAIN） |
| 196 | `src/unit_test/parallel/chaos_ut.cpp` | `ParallelTest` | `DISABLED_SLOW_ChaosTest` | ported | `infinity-executor` `tests::disabled_slow_chaos_test`（`#[ignore]`，`INFINITY_CHAOS_SECONDS` 可缩短） |
| 197 | `src/unit_test/parser/float_serialize_ut.cpp` | `FloatSerializeTest` | `test_precision` | ported | `infinity-parser` `parser::tests::float_serialize_test_precision` |
| 198 | `src/unit_test/parser/float_serialize_ut.cpp` | `FloatSerializeTest` | `test_json` | ported | `infinity-expression` `json::tests::float_serialize_test_json` |
| 199 | `src/unit_test/parser/float_serialize_ut.cpp` | `FloatSerializeTest` | `test_json_hex_float` | ported | `infinity-expression` `json::tests::float_serialize_test_json_hex_float` |
| 200 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `good_test1` | ported | `infinity-executor` `fulltext_query::tests::search_driver_good_test1` |
| 201 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `good_test2` | ported | `infinity-executor` `fulltext_query::tests::search_driver_good_test2` |
| 202 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `operator_option_test` | ported | `infinity-executor` `fulltext_query::tests::search_driver_operator_option_test` |
| 203 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `whitespace_analyzer_test` | ported | `infinity-executor` `fulltext_query::tests::search_driver_whitespace_analyzer_test` |
| 204 | `src/unit_test/parser/sql_file_parsing_test_ut.cpp` | `SQLFileParsingTest` | `tpch` | ported | `infinity-parser` `corpora_ut::tests::sql_file_parsing_tpch` |
| 205 | `src/unit_test/parser/sql_file_parsing_test_ut.cpp` | `SQLFileParsingTest` | `hyrise` | ported | `infinity-parser` `corpora_ut::tests::sql_file_parsing_hyrise` |
| 206 | `src/unit_test/parser/sql_file_parsing_test_ut.cpp` | `SQLFileParsingTest` | `infinity` | ported | `infinity-parser` `corpora_ut::tests::sql_file_parsing_infinity` |
| 207 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `good_test1` | ported | `infinity-parser` `parser::tests::sql_parser_good_test1` |
| 208 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `good_test2` | excluded | 含 geo 列（point/line/lseg/box/circle），用户确认不移植 geo |
| 209 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `bad_test1` | ported | `infinity-parser` `parser::tests::sql_parser_bad_test1` |
| 210 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `good_create_index_1` | ported | `infinity-parser` `parser::tests::sql_parser_good_create_index_1` |
| 211 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `bad_create_index_1` | ported | `infinity-parser` `parser::tests::sql_parser_bad_create_index_1` |
| 212 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test1` | ported | `infinity-parser` `parser::tests::select_statement_parsing_good_test1` |
| 213 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test2` | ported | `infinity-parser` `parser::tests::select_statement_parsing_good_test2` |
| 214 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test3` | ported | `infinity-parser` `parser::tests::select_statement_parsing_good_test3` |
| 215 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test4` | ported | `infinity-parser` `parser::tests::cast_date_minus_interval_parses` |
| 216 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `bad_test1` | ported | `infinity-parser` `parser::tests::offset_without_limit_is_rejected` |
| 217 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `bad_knn_test` | ported | `infinity-parser` `parser::tests::vector_metric_type_mismatches_are_rejected` |
| 218 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `bad_search_test` | ported | `infinity-parser` `parser::tests::search_with_order_by_is_rejected` |
| 219 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_search_test` | ported | `infinity-parser` `parser::tests::select_statement_parsing_good_search_test` |
| 220 | `src/unit_test/parser/sql_statement_ut.cpp` | `StatementParsingTest` | `good_test1` | ported | `infinity-parser` `parser::tests::statement_parsing_good_test1` |
| 221 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 222 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap2` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 223 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap33` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 224 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap34` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 225 | `src/unit_test/parser/type/complex/blob_type_ut_nimpl.cpp` | `BlobTypeTest` | `blob1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 226 | `src/unit_test/parser/type/complex/uuid_type_ut.cpp` | `UuidTypeTest` | `Uuid` | ported | `infinity-common` `types::tests::uuid` |
| 227 | `src/unit_test/parser/type/complex/varchar_ut.cpp` | `VarcharTest` | `structure_check` | ported | `infinity-parser` `varchar::tests::varchar_structure_check` |
| 228 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `GetTypeName` | ported | `infinity-common` `types::tests::data_type_get_type_name` |
| 229 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `TypeToString` | ported | `infinity-common` `types::tests::data_type_type_to_string` |
| 230 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `Serialize` | ported | `infinity-common` `types::tests::data_type_serialize` |
| 231 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `ReadWrite` | ported | `infinity-common` `types::tests::data_type_read_write` |
| 232 | `src/unit_test/parser/type/datetime/date_type_old_ut.cpp` | `DateTypeOldTest` | `test1` | ported | `infinity-common` `datetime::tests::date_type_old_test1` |
| 233 | `src/unit_test/parser/type/datetime/date_type_old_ut.cpp` | `DateTypeOldTest` | `TestEqStdChronoForward` | ported | `infinity-common` `datetime::tests::date_type_old_test_eq_std_chrono_forward` |
| 234 | `src/unit_test/parser/type/datetime/date_type_old_ut.cpp` | `DateTypeOldTest` | `TestEqStdChronoBackward` | ported | `infinity-common` `datetime::tests::date_type_old_test_eq_std_chrono_backward` |
| 235 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestSameSize` | ported | `infinity-parser` `datetime::tests::date_type_same_size` |
| 236 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestFromString` | ported | `infinity-common` `datetime::tests::test_from_string` |
| 237 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestAddSubstract` | ported | `infinity-common` `datetime::tests::test_add_subtract` |
| 238 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestNegativeYears` | ported | `infinity-common` `datetime::tests::test_negative_years` |
| 239 | `src/unit_test/parser/type/fp16_ut.cpp` | `FP16TypeTest` | `test1` | ported | `infinity-parser` `fp16::tests::fp16_test1` |
| 240 | `src/unit_test/parser/type/geo/box_type_ut.cpp` | `BoxTypeTest` | `point1` | excluded | geo 类型（用户决定不移植 Box/Point/Circle 等） |
| 241 | `src/unit_test/parser/type/geo/path_type_ut_nimpl.cpp` | `PathTypeTest` | `path1` | excluded | geo 类型（用户决定不移植 Box/Point/Circle 等） |
| 242 | `src/unit_test/parser/type/geo/point_type_ut.cpp` | `PointTypeTest` | `point1` | excluded | geo 类型（用户决定不移植 Box/Point/Circle 等） |
| 243 | `src/unit_test/parser/type/geo/polygon_type_ut_nimpl.cpp` | `PolygonTypeTest` | `polygon1` | excluded | geo 类型（用户决定不移植 Box/Point/Circle 等） |
| 244 | `src/unit_test/parser/type/heterogeneous/base_mixed_type_ut.cpp` | `BaseMixedTypeTest` | `GetTypeName` | ported | `infinity-common` `types::tests::mixed_value_type_names_match_the_reference` |
| 245 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_integer` | ported | `infinity-common` `types::tests::mixed_integer_value_and_clone` |
| 246 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_float` | ported | `infinity-common` `types::tests::mixed_float_value` |
| 247 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_short_str` | ported | `infinity-common` `types::tests::mixed_short_string_boundary` |
| 248 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_long_str` | ported | `infinity-common` `types::tests::mixed_long_string` |
| 249 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_null` | ported | `infinity-common` `types::tests::mixed_null_and_missing_tags` |
| 250 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_miss` | ported | `infinity-common` `types::tests::mixed_null_and_missing_tags` |
| 251 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_tuple1` | ported | `infinity-common` `types::tests::mixed_tuple1_matches_the_reference` |
| 252 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_tuple2` | ported | `infinity-common` `types::tests::mixed_named_tuple_matches_the_reference` |
| 253 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_tuple3` | ported | `infinity-common` `types::tests::mixed_tuple_errors_match_the_reference` |
| 254 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_array1` | ported | `infinity-common` `types::tests::mixed_array_matches_the_reference` |
| 255 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_array2` | ported | `infinity-common` `types::tests::mixed_array_nests_tuple_and_array` |
| 256 | `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp` | `MixedTypeTest` | `mixed_array3` | ported | `infinity-common` `types::tests::mixed_array_overwrites_and_bounds` |
| 257 | `src/unit_test/parser/type/info/decimal_info_ut.cpp` | `DecimalInfoTest` | `decimal_info_A` | ported | `infinity-common` `types::tests::decimal_info_a` |
| 258 | `src/unit_test/parser/type/info/embedding_info_ut.cpp` | `EmbeddingInfoTest` | `embedding_info_A` | ported | `infinity-common` `types::tests::embedding_info_a` |
| 259 | `src/unit_test/parser/type/null_value_ut.cpp` | `NullValueTest` | `null_value_test1` | ported | `infinity-common` `types::tests::null_value_test1` |
| 260 | `src/unit_test/planner/column_binding_ut.cpp` | `ColumnBindingTest` | `test1` | ported | `infinity-planner` `column_binding::tests::column_binding_test1` |
| 261 | `src/unit_test/planner/explain_basic_ut.cpp` | `ExplainBasicTest` | `test1` | ported | `infinity-executor` `gate_b_executor_ut::explain_basic_test1` |
| 262 | `src/unit_test/planner/logical_plan_tpch_ut.cpp` | `LogicalPlannerTpchTest` | `test1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 263 | `src/unit_test/planner/logical_planner_ut_nimpl.cpp` | `LogicalPlannerTest` | `test1` | exempt | C++ 整段位于 `#if 0`，无编译后可执行行为 |
| 264 | `src/unit_test/planner/node/logical_aggregate_ut.cpp` | `LogicalAggregateTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 265 | `src/unit_test/planner/node/logical_check_ut.cpp` | `LogicalCheckTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 266 | `src/unit_test/planner/node/logical_command_ut.cpp` | `LogicalCommandTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 267 | `src/unit_test/planner/node/logical_compact_ut.cpp` | `LogicalCompactTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 268 | `src/unit_test/planner/node/logical_create_collection_ut.cpp` | `LogicalCreateCollectionTest` | `test1` | ported | `infinity-executor` `gate_b_executor_ut::logical_create_collection_test1` |
| 269 | `src/unit_test/planner/node/logical_create_index_ut.cpp` | `LogicalCreateIndexTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 270 | `src/unit_test/planner/node/logical_create_table_ut.cpp` | `LogicalCreateTableTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 271 | `src/unit_test/planner/node/logical_drop_collection_ut.cpp` | `LogicalDropCollectionTest` | `test1` | ported | `infinity-executor` `gate_b_executor_ut::logical_drop_collection_test1` |
| 272 | `src/unit_test/planner/node/logical_drop_index_ut.cpp` | `LogicalDropIndexTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 273 | `src/unit_test/planner/node/logical_drop_table_ut.cpp` | `LogicalDropTableTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 274 | `src/unit_test/planner/node/logical_flush_ut.cpp` | `LogicalFlushTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 275 | `src/unit_test/planner/node/logical_insert_ut.cpp` | `LogicalInsertTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 276 | `src/unit_test/planner/node/logical_limit_ut.cpp` | `LogicalLimitTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 277 | `src/unit_test/planner/node/logical_match_scan_base_ut.cpp` | `LogicalMatchScanTest` | `test1` | ported | `infinity-executor` `table_ut::logical_match_scan_base_test1` |
| 278 | `src/unit_test/planner/node/logical_optimize_ut.cpp` | `LogicalOptimzieTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 279 | `src/unit_test/planner/node/logical_project_ut.cpp` | `LogicalProjectTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 280 | `src/unit_test/planner/node/logical_show_ut.cpp` | `LogicalShowTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 281 | `src/unit_test/planner/node/logical_update_ut.cpp` | `LogicalUpdateTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 282 | `src/unit_test/planner/optimizer/regex_literal_extractor_ut.cpp` | `RegexLiteralExtractorTest` | `top_level_alternation_requires_nothing` | ported | `infinity-executor` `regex_literal::tests::extracts_only_mandatory_runs` |
| 283 | `src/unit_test/planner/optimizer/regex_literal_extractor_ut.cpp` | `RegexLiteralExtractorTest` | `scoped_case_insensitive_group_keeps_its_mode` | ported | `infinity-executor` `regex_literal::tests::extracts_only_mandatory_runs` |
| 284 | `src/unit_test/planner/optimizer/regex_literal_extractor_ut.cpp` | `RegexLiteralExtractorTest` | `re2_escapes` | ported | `infinity-executor` `regex_literal::tests::extracts_only_mandatory_runs` |
| 285 | `src/unit_test/storage/bg_task/background_process_ut.cpp` | `BGProcessTest` | `test1` | ported | `infinity-executor` `bg_task_ut::background_process_start_stop` |
| 286 | `src/unit_test/storage/bg_task/cleanup_task_ut.cpp` | `CleanupTaskTest` | `test_delete_db_simple` | ported | `infinity-executor` `bg_task_ut::cleanup_removes_the_dropped_table_files` |
| 287 | `src/unit_test/storage/bg_task/compact_segments_task_ut.cpp` | `CompactTaskTest` | `bg_compact` | ported | `infinity-executor` `bg_task_ut::compact_reduces_the_segment_count` |
| 288 | `src/unit_test/storage/bg_task/dump_index_task_2_ut.cpp` | `DumpMemIndexTaskTest2` | `DISABLED_SLOW_row_cnt_exceed_memory_quota` | ported | `infinity-executor` `bg_task_ut::dump_index_task_row_cnt_exceed_memory_quota` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 289 | `src/unit_test/storage/bg_task/dump_index_task_ut.cpp` | `DumpMemIndexTaskTest` | `DISABLED_SLOW_row_cnt_exceed_memindex_capacity` | ported | `infinity-executor` `bg_task_ut::dump_index_task_row_cnt_exceed_memindex_capacity` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 290 | `src/unit_test/storage/binary_fuse_filter/binary_fuse_filter_test_ut.cpp` | `BinaryFuseFilterTest` | `test_2000` | ported | `infinity-storage` `binary_fuse_filter::tests::test_2000` |
| 291 | `src/unit_test/storage/buffer/buffer_handle_ut.cpp` | `BufferHandleTest` | `test1` | ported | `infinity-storage` `buffer::tests::buffer_handles_follow_the_reference_lifecycle` |
| 292 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerTest` | `cleanup_test` | ported | `infinity-storage` `buffer::tests::cleanup_test` |
| 293 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerTest` | `varfile_test` | ported | `infinity-storage` `buffer::tests::varfile_test` |
| 294 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerParallelTest` | `parallel_test1` | ported | `infinity-storage` `buffer::tests::parallel_test1` |
| 295 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerParallelTest` | `parallel_test2` | ported | `infinity-storage` `buffer::tests::parallel_test2` |
| 296 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `test1` | ported | `infinity-storage` `buffer::tests::buffer_obj_state_machine` |
| 297 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `test_hnsw_index_buffer_obj_shutdown` | ported | `infinity-executor` `buffer_obj_ut::test_hnsw_index_buffer_obj_shutdown` |
| 298 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `test_big_with_gc_and_cleanup` | ported | `infinity-executor` `buffer_obj_ut::test_big_with_gc_and_cleanup` |
| 299 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `DISABLED_SLOW_test_multiple_threads_read` | ported | `infinity-storage` `buffer::tests::disabled_slow_test_multiple_threads_read`（`#[ignore]`） |
| 300 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `flat_boolean` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 301 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `contant_bool` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 302 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `bool_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 303 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `bool_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 304 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_date` | ported | `infinity-storage` `data_block::reference_suite::date_flat_constant_select_slice` |
| 305 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_date` | ported | `infinity-storage` `data_block::reference_suite::date_flat_constant_select_slice` |
| 306 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `date_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::date_flat_constant_select_slice` |
| 307 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `date_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::date_flat_constant_select_slice` |
| 308 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_time` | ported | `infinity-storage` `data_block::reference_suite::time_flat_constant_select_slice` |
| 309 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_time` | ported | `infinity-storage` `data_block::reference_suite::time_flat_constant_select_slice` |
| 310 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `time_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::time_flat_constant_select_slice` |
| 311 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `time_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::time_flat_constant_select_slice` |
| 312 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_datetime` | ported | `infinity-storage` `data_block::reference_suite::datetime_flat_constant_select_slice` |
| 313 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_datetime` | ported | `infinity-storage` `data_block::reference_suite::datetime_flat_constant_select_slice` |
| 314 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `datetime_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::datetime_flat_constant_select_slice` |
| 315 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `datetime_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::datetime_flat_constant_select_slice` |
| 316 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_timestamp` | ported | `infinity-storage` `data_block::reference_suite::timestamp_flat_constant_select_slice` |
| 317 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_timestamp` | ported | `infinity-storage` `data_block::reference_suite::timestamp_flat_constant_select_slice` |
| 318 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `timestamp_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::timestamp_flat_constant_select_slice` |
| 319 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `timestamp_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::timestamp_flat_constant_select_slice` |
| 320 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_interval` | ported | `infinity-storage` `data_block::reference_suite::interval_flat_constant_select_slice` |
| 321 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_flat` | ported | `infinity-storage` `data_block::reference_suite::interval_flat_constant_select_slice` |
| 322 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `interval_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::interval_flat_constant_select_slice` |
| 323 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `interval_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::interval_flat_constant_select_slice` |
| 324 | `src/unit_test/storage/column_vector/column_vector_decimal_ut_nimpl.cpp` | `ColumnVectorDecimalTest` | `flat_decimal` | ported | `infinity-storage` `data_block::reference_suite::decimal_flat_constant_select_slice` |
| 325 | `src/unit_test/storage/column_vector/column_vector_decimal_ut_nimpl.cpp` | `ColumnVectorDecimalTest` | `contant_decimal` | ported | `infinity-storage` `data_block::reference_suite::decimal_flat_constant_select_slice` |
| 326 | `src/unit_test/storage/column_vector/column_vector_decimal_ut_nimpl.cpp` | `ColumnVectorDecimalTest` | `decimal_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::decimal_flat_constant_select_slice` |
| 327 | `src/unit_test/storage/column_vector/column_vector_decimal_ut_nimpl.cpp` | `ColumnVectorDecimalTest` | `decimal_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::decimal_flat_constant_select_slice` |
| 328 | `src/unit_test/storage/column_vector/column_vector_embedding_ut.cpp` | `ColumnVectorEmbeddingTest` | `flat_embedding` | ported | `infinity-storage` `data_block::reference_suite::embedding_flat_constant_select_slice` |
| 329 | `src/unit_test/storage/column_vector/column_vector_embedding_ut.cpp` | `ColumnVectorEmbeddingTest` | `contant_embedding` | ported | `infinity-storage` `data_block::reference_suite::embedding_flat_constant_select_slice` |
| 330 | `src/unit_test/storage/column_vector/column_vector_embedding_ut.cpp` | `ColumnVectorEmbeddingTest` | `embedding_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::embedding_flat_constant_select_slice` |
| 331 | `src/unit_test/storage/column_vector/column_vector_embedding_ut.cpp` | `ColumnVectorEmbeddingTest` | `embedding_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::embedding_flat_constant_select_slice` |
| 332 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `flat_float` | ported | `infinity-storage` `data_block::reference_suite::float_flat_constant_select_slice` |
| 333 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `contant_float` | ported | `infinity-storage` `data_block::reference_suite::float_flat_constant_select_slice` |
| 334 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `float_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::float_flat_constant_select_slice` |
| 335 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `float_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::float_flat_constant_select_slice` |
| 336 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `flat_double` | ported | `infinity-storage` `data_block::reference_suite::double_flat_constant_select_slice` |
| 337 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `contant_double` | ported | `infinity-storage` `data_block::reference_suite::double_flat_constant_select_slice` |
| 338 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `double_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::double_flat_constant_select_slice` |
| 339 | `src/unit_test/storage/column_vector/column_vector_float_ut.cpp` | `ColumnVectorFloatTest` | `double_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::double_flat_constant_select_slice` |
| 340 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_point` | excluded |
| 341 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_point` | excluded |
| 342 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `point_column_vector_select` | excluded |
| 343 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `point_column_slice_init` | excluded |
| 344 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_line` | excluded |
| 345 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_line` | excluded |
| 346 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `line_column_vector_select` | excluded |
| 347 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `line_column_slice_init` | excluded |
| 348 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_line_seg` | excluded |
| 349 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_line_seg` | excluded |
| 350 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `line_seg_column_vector_select` | excluded |
| 351 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `line_seg_column_slice_init` | excluded |
| 352 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_box` | excluded |
| 353 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_box` | excluded |
| 354 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `box_column_vector_select` | excluded |
| 355 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `box_column_slice_init` | excluded |
| 356 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_path` | excluded |
| 357 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_path` | excluded |
| 358 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `path_column_vector_select` | excluded |
| 359 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `path_column_slice_init` | excluded |
| 360 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_polygon` | excluded |
| 361 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_polygon` | excluded |
| 362 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `polygon_column_vector_select` | excluded |
| 363 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `polygon_column_slice_init` | excluded |
| 364 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `flat_circle` | excluded |
| 365 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `contant_circle` | excluded |
| 366 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `circle_column_vector_select` | excluded |
| 367 | `src/unit_test/storage/column_vector/column_vector_geo_ut.cpp` | `ColumnVectorGeoTest` | `circle_column_slice_init` | excluded |
| 368 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_tinyint` | ported | `infinity-storage` `data_block::reference_suite::tinyint_flat_constant_select_slice` |
| 369 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_tinyint` | ported | `infinity-storage` `data_block::reference_suite::tinyint_flat_constant_select_slice` |
| 370 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `tinyint_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::tinyint_flat_constant_select_slice` |
| 371 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `tinyint_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::tinyint_flat_constant_select_slice` |
| 372 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_smallint` | ported | `infinity-storage` `data_block::reference_suite::smallint_flat_constant_select_slice` |
| 373 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_smallint` | ported | `infinity-storage` `data_block::reference_suite::smallint_flat_constant_select_slice` |
| 374 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `smallint_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::smallint_flat_constant_select_slice` |
| 375 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `smallint_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::smallint_flat_constant_select_slice` |
| 376 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_int` | ported | `infinity-storage` `data_block::reference_suite::int_flat_constant_select_slice` |
| 377 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_int` | ported | `infinity-storage` `data_block::reference_suite::int_flat_constant_select_slice` |
| 378 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `integer_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::int_flat_constant_select_slice` |
| 379 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `integer_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::int_flat_constant_select_slice` |
| 380 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_bigint` | ported | `infinity-storage` `data_block::reference_suite::bigint_flat_constant_select_slice` |
| 381 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_bigint` | ported | `infinity-storage` `data_block::reference_suite::bigint_flat_constant_select_slice` |
| 382 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `bigint_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::bigint_flat_constant_select_slice` |
| 383 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `bigint_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::bigint_flat_constant_select_slice` |
| 384 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_hugeint` | ported | `infinity-storage` `data_block::reference_suite::flat_hugeint` |
| 385 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_hugeint` | ported | `infinity-storage` `data_block::reference_suite::contant_hugeint` |
| 386 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `hugeint_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::hugeint_column_vector_select` |
| 387 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `hugeint_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::hugeint_column_slice_init` |
| 388 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_integer_a` | ported | `infinity-storage` `mixed_column::mixed_integer_a` |
| 389 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_integer_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_integer_column_vector_select` |
| 390 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_integer_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_integer_column_slice_init` |
| 391 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_float_a` | ported | `infinity-storage` `mixed_column::mixed_float_a` |
| 392 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_float_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_float_column_vector_select` |
| 393 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_float_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_float_column_slice_init` |
| 394 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_short_str_a` | ported | `infinity-storage` `mixed_column::mixed_short_str_a` |
| 395 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_short_str_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_short_str_column_vector_select` |
| 396 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_short_str_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_short_str_column_slice_init` |
| 397 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_long_str_a` | ported | `infinity-storage` `mixed_column::mixed_long_str_a` |
| 398 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_long_str_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_long_str_column_vector_select` |
| 399 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_long_str_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_long_str_column_slice_init` |
| 400 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_null_a` | ported | `infinity-storage` `mixed_column::mixed_null_a` |
| 401 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_null_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_null_column_vector_select` |
| 402 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_null_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_null_column_slice_init` |
| 403 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_missing_a` | ported | `infinity-storage` `mixed_column::mixed_missing_a` |
| 404 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_missing_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_missing_column_vector_select` |
| 405 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_missing_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_missing_column_slice_init` |
| 406 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_tuple_a` | ported | `infinity-storage` `mixed_column::mixed_tuple_a` |
| 407 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_tuple_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_tuple_column_vector_select` |
| 408 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_tuple_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_tuple_column_slice_init` |
| 409 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_array_a` | ported | `infinity-storage` `mixed_column::mixed_array_a` |
| 410 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_array_column_vector_select` | ported | `infinity-storage` `mixed_column::mixed_array_column_vector_select` |
| 411 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_array_column_slice_init` | ported | `infinity-storage` `mixed_column::mixed_array_column_slice_init` |
| 412 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `flat_row` | ported | `infinity-storage` `data_block::reference_suite::row_flat` |
| 413 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `contant_row` | ported | `infinity-storage` `data_block::reference_suite::row_constant` |
| 414 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `row_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::row_column_vector_select` |
| 415 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `row_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::row_column_slice_init` |
| 416 | `src/unit_test/storage/column_vector/column_vector_ut.cpp` | `ColumnVectorTest` | `ReadWrite` | ported | `infinity-storage` `data_block::reference_suite::column_vector_read_write_roundtrips` |
| 417 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `flat_uuid` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 418 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `contant_uuid` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 419 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `uuid_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 420 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `uuid_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 421 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `flat_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 422 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `constant_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 423 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `varchar_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 424 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `varchar_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 425 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `flat_not_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_not_inline` |
| 426 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `constant_not_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_constant_not_inline` |
| 427 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `flat_mixed_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_mixed_inline` |
| 428 | `src/unit_test/storage/column_vector/selection_ut.cpp` | `SelectionTest` | `test1` | ported | `infinity-storage` `selection::tests::selection_matches_the_reference` |
| 429 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_embedding` | ported | `infinity-common` `json::tests::embedding_to_json_matches_reference` |
| 430 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_sparse` | ported | `infinity-common` `json::tests::sparse_to_json_matches_reference` |
| 431 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_tensor` | ported | `infinity-common` `json::tests::tensor_to_json_matches_reference` |
| 432 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_tensor_array` | ported | `infinity-common` `json::tests::tensor_array_to_json_matches_reference` |
| 433 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_basic_type` | ported | `infinity-common` `json::tests::basic_type_json_round_trip` |
| 434 | `src/unit_test/storage/column_vector/value_ut.cpp` | `ValueStringTest` | `test_tostring` | ported | `infinity-storage` `data_block::reference_suite::value_to_string_matches_the_reference`（JSON 以文本形式原样呈现） |
| 435 | `src/unit_test/storage/column_vector/value_ut.cpp` | `ValueStringTest` | `test_stringtovalue` | ported | `infinity-storage` `data_block::reference_suite::string_to_value_roundtrips_the_scalar_types` (JSON is a parsed tree, structural equality) |
| 436 | `src/unit_test/storage/column_vector/var_buffer_ut.cpp` | `VarBufferTest` | `test1` | ported | `infinity-storage` `var_buffer::tests::var_buffer_appends_and_roundtrips` |
| 437 | `src/unit_test/storage/common/kv_code_ut.cpp` | `KVCodeTest` | `kv_code1` | ported | `infinity-storage` `catalog_format::tests::catalog_keys_match_the_reference` |
| 438 | `src/unit_test/storage/common/kv_code_ut.cpp` | `KVCodeTest` | `kv_code2` | ported | `catalog_format::tests::name_keyed_catalog_keys_match_the_reference` |
| 439 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge1` | ported | `infinity-common` `loser_tree::tests::basic_merge_1` |
| 440 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge2` | ported | `infinity-common` `loser_tree::tests::basic_merge_2` |
| 441 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge3` | ported | `infinity-common` `loser_tree::tests::basic_merge_3` |
| 442 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge4` | ported | `infinity-common` `loser_tree::tests::basic_merge_4` |
| 443 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestBasicInsertAndGet` | ported | `infinity-storage` `rcu_map::reference_suite::basic_insert_and_get` |
| 444 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestLowercaseAliases` | ported | `infinity-storage` `rcu_map::reference_suite::lowercase_aliases` |
| 445 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestDelete` | ported | `infinity-storage` `rcu_map::reference_suite::delete` |
| 446 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestGetAllValuesWithRef` | ported | `infinity-storage` `rcu_map::reference_suite::get_all_values_with_ref` |
| 447 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestGarbageCollection` | ported | `infinity-storage` `rcu_map::reference_suite::garbage_collection` |
| 448 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestExpiredEntries` | ported | `infinity-storage` `rcu_map::reference_suite::expired_entries` |
| 449 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestGetWithRcuTime` | ported | `infinity-storage` `rcu_map::reference_suite::get_with_rcu_time` |
| 450 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestMultipleValuesPerKey` | ported | `infinity-storage` `rcu_map::reference_suite::multiple_values_per_key` |
| 451 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestIntegerKeys` | ported | `infinity-storage` `rcu_map::reference_suite::integer_keys` |
| 452 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestEmptyMap` | ported | `infinity-storage` `rcu_map::reference_suite::empty_map` |
| 453 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestStressInsertAndRetrieve` | ported | `infinity-storage` `rcu_map::reference_suite::stress_insert_and_retrieve` |
| 454 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestDeleteFunctionality` | ported | `infinity-storage` `rcu_map::reference_suite::delete_functionality` |
| 455 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestDifferentValueTypes` | ported | `infinity-storage` `rcu_map::reference_suite::different_value_types` |
| 456 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestConcurrentInsertAndRead` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::multimap_concurrent_insert_and_read` |
| 457 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestConcurrentInsertDeleteRead` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::multimap_concurrent_insert_delete_read` |
| 458 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestHighVolumeMultiThreading` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::multimap_high_volume_multi_threading` |
| 459 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestRcuSwapUnderLoad` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::multimap_rcu_swap_under_load` |
| 460 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestRcuBehavior` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::multimap_rcu_behavior` |
| 461 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestBasicInsertAndGet` | ported | `infinity-storage` `rcu_map::map_reference_suite::basic_insert_and_get` |
| 462 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestLowercaseAliases` | ported | `infinity-storage` `rcu_map::map_reference_suite::lowercase_aliases` |
| 463 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestDelete` | ported | `infinity-storage` `rcu_map::map_reference_suite::delete` |
| 464 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGetAllValuesWithRef` | ported | `infinity-storage` `rcu_map::map_reference_suite::get_all_values_with_ref` |
| 465 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGetAllKeyValuePairs` | ported | `infinity-storage` `rcu_map::map_reference_suite::get_all_key_value_pairs` |
| 466 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGarbageCollection` | ported | `infinity-storage` `rcu_map::map_reference_suite::garbage_collection` |
| 467 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestExpiredEntries` | ported | `infinity-storage` `rcu_map::map_reference_suite::expired_entries` |
| 468 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGetWithRcuTime` | ported | `infinity-storage` `rcu_map::map_reference_suite::get_with_rcu_time` |
| 469 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestUniqueKeys` | ported | `infinity-storage` `rcu_map::map_reference_suite::unique_keys` |
| 470 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestIntegerKeys` | ported | `infinity-storage` `rcu_map::map_reference_suite::integer_keys` |
| 471 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestEmptyMap` | ported | `infinity-storage` `rcu_map::map_reference_suite::empty_map` |
| 472 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestEmplace` | ported | `infinity-storage` `rcu_map::map_reference_suite::emplace` |
| 473 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestRange` | ported | `infinity-storage` `rcu_map::map_reference_suite::range` |
| 474 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestStressInsertAndRetrieve` | ported | `infinity-storage` `rcu_map::map_reference_suite::stress_insert_and_retrieve` |
| 475 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestConcurrentInsertAndRead` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::map_concurrent_insert_and_read` |
| 476 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestConcurrentInsertDeleteRead` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::map_concurrent_insert_delete_read` |
| 477 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestRcuSwapUnderLoad` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::map_rcu_swap_under_load` |
| 478 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestRcuBehavior` | ported | `infinity-storage` `rcu_map::concurrency_reference_suite::map_rcu_behavior` |
| 479 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestMapVsMultiMapBehavior` | ported | `infinity-storage` `rcu_map::map_reference_suite::map_vs_multimap_behavior` |
| 480 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `ReadHeavyWorkload` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::read_heavy_workload` |
| 481 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `WriteHeavyWorkload` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::write_heavy_workload` |
| 482 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `ReadOnlyWorkload` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::read_only_workload` |
| 483 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `HighContentionWorkload` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::high_contention_workload` |
| 484 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `ScalabilityTest` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::scalability_test` |
| 485 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `MemoryAndGcPerformance` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::memory_and_gc_performance` |
| 486 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `ComprehensiveBenchmarkSummary` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::comprehensive_benchmark_summary` |
| 487 | src/unit_test/storage/common/test_rcu_multimap_ut.cpp | `QuickPerformanceVerification` | ported | `infinity-storage` `rcu_map::benchmark_reference_suite::quick_performance_verification` |
| 488 | `src/unit_test/storage/definition/column_def_ut.cpp` | `ColumnDefTest` | `test1` | ported | `infinity-catalog` `value_format::tests::column_def_to_string_renders_constraints_in_set_order` |
| 489 | `src/unit_test/storage/definition/column_def_ut.cpp` | `ColumnDefTest` | `test2` | ported | `value_format::tests::column_def_to_string_renders_a_primary_key` |
| 490 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `test1` | ported | `infinity-storage` `data_block::data_block_reference_suite::all_types_append_and_read_back` |
| 491 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `test2` | ported | `infinity-storage` `data_block::data_block_reference_suite::append_rejects_bad_column_and_overflow_then_fills_row_ids` |
| 492 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `test3` | ported | `infinity-storage` `data_block::data_block_reference_suite::set_value_rejects_bad_column_and_row` |
| 493 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `ReadWrite` | ported | `infinity-storage` `data_block::data_block_reference_suite::data_block_write_adv_read_adv_roundtrip` |
| 494 | `src/unit_test/storage/definition/index_base_ut.cpp` | `IndexBaseTest` | `ivf_readwrite` | ported | `value_format::tests::ivf_index_base_roundtrips_through_its_bytes` |
| 495 | `src/unit_test/storage/definition/index_base_ut.cpp` | `IndexBaseTest` | `hnsw_readwrite` | ported | `value_format::tests::hnsw_index_base_roundtrips_through_its_bytes` |
| 496 | `src/unit_test/storage/definition/index_base_ut.cpp` | `IndexBaseTest` | `full_text_readwrite` | ported | `value_format::tests::full_text_index_base_roundtrips_through_its_bytes` |
| 497 | `src/unit_test/storage/definition/table_def_ut.cpp` | `TableDefTest` | `test1` | ported | `infinity-catalog` `value_format::tests::table_def_reports_schema_table_comment_and_columns` |
| 498 | `src/unit_test/storage/definition/table_def_ut.cpp` | `TableDefTest` | `ReadWrite` | ported | `value_format::tests::table_def_roundtrips_through_its_bytes` |
| 499 | src/unit_test/storage/definition/table_ut.cpp | `test1` | ported | `infinity-executor` `new_catalog_table_ut::table_test1` |
| 500 | `src/unit_test/storage/invertedindex/column_inverter_ut.cpp` | `ColumnInverterTest` | `Invert` | ported | `infinity-executor` `tests::memory_indexer_records_tf_positions_and_length` |
| 501 | `src/unit_test/storage/invertedindex/common/external_sort_ut.cpp` | `ExternalSortTest` | `test1` | ported | `infinity-storage` `external_sort::tests::test1` |
| 502 | `src/unit_test/storage/invertedindex/common/external_sort_ut.cpp` | `ExternalSortTest` | `test2` | ported | `infinity-storage` `external_sort::tests::test2` |
| 503 | `src/unit_test/storage/invertedindex/common/skiplist_ut_nimpl.cpp` | `SkiplistTest` | `test1` | ported | `infinity-storage` `inverted_format::skiplist::tests::skiplist_test1` |
| 504 | `src/unit_test/storage/invertedindex/common/skiplist_ut_nimpl.cpp` | `SkiplistTest` | `test2` | ported | `infinity-storage` `inverted_format::skiplist::tests::skiplist_test2` |
| 505 | `src/unit_test/storage/invertedindex/common/skiplist_ut_nimpl.cpp` | `SkiplistTest` | `test3` | ported | `infinity-storage` `inverted_format::skiplist::tests::skiplist_test3` |
| 506 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test1` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test1` |
| 507 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test2` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test2` |
| 508 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test3` | ported | `infinity-storage` `highlighter_ut::test3` |
| 509 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test4` | ported | `infinity-storage` `highlighter_ut::test4` |
| 510 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test5` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test5` |
| 511 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test6` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test6` |
| 512 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test7` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test7` |
| 513 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test8` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test8` |
| 514 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test9` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test9` |
| 515 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test10` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test10` |
| 516 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test11` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test11` |
| 517 | src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp | `test12` | ported | `infinity-storage` `inverted_format::doc_list::tests::inmem_doc_list_decoder_test12` |
| 518 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test1` | ported | `infinity-storage` `inverted_format::position_list::tests::inmem_position_list_decoder_test1` |
| 519 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test2` | ported | `infinity-storage` `inverted_format::position_list::tests::inmem_position_list_decoder_test2` |
| 520 | src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp | `test3` | ported | `infinity-storage` `highlighter_ut::test3` |
| 521 | src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp | `test4` | ported | `infinity-storage` `highlighter_ut::test4` |
| 522 | src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp | `test5` | ported | `infinity-storage` `inverted_format::position_list::tests::inmem_position_list_decoder_test5` |
| 523 | `src/unit_test/storage/invertedindex/format/position_list_decoder_ut.cpp` | `PositionListDecoderTest` | `test1` | ported | `infinity-storage` `inverted_format::position_list::tests::position_list_decoder_test1` |
| 524 | `src/unit_test/storage/invertedindex/format/position_list_encoder_ut.cpp` | `PositionListEncoderTest` | `test1` | ported | `infinity-storage` `inverted_format::position_list::tests::position_list_encoder_test1` |
| 525 | `src/unit_test/storage/invertedindex/format/position_list_encoder_ut.cpp` | `PositionListEncoderTest` | `test2` | ported | `infinity-storage` `inverted_format::position_list::tests::position_list_encoder_test2` |
| 526 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test1` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test1` |
| 527 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test2` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test2` |
| 528 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test3` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test3` |
| 529 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test4` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test4` |
| 530 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test5` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test5` |
| 531 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test6` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test6` |
| 532 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test7` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test7` |
| 533 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test8` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_buffer_test8` |
| 534 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_reader_ut.cpp` | `PostingByteSliceReaderTest` | `test1` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_byte_slice_reader_test1` |
| 535 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_reader_ut.cpp` | `PostingByteSliceReaderTest` | `test2` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_byte_slice_reader_test2` |
| 536 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_reader_ut.cpp` | `PostingByteSliceReaderTest` | `test3` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_byte_slice_reader_test3` |
| 537 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_ut.cpp` | `PostingByteSliceTest` | `test1` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_byte_slice_test1` |
| 538 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_ut.cpp` | `PostingByteSliceTest` | `test2` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_byte_slice_test2` |
| 539 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_ut.cpp` | `PostingByteSliceTest` | `test3` | ported | `infinity-storage` `inverted_format::posting_slice::tests::posting_byte_slice_test3` |
| 540 | `src/unit_test/storage/invertedindex/format/skiplist_reader_ut.cpp` | `SkiplistReaderTest` | `SkipListReaderByteSliceTest` | ported | `infinity-storage` `inverted_format::posting_slice::tests::skiplist_reader_byte_slice` |
| 541 | `src/unit_test/storage/invertedindex/format/skiplist_reader_ut.cpp` | `SkiplistReaderTest` | `SkipListReaderPostingByteSliceTest` | ported | `infinity-storage` `inverted_format::posting_slice::tests::skiplist_reader_posting_byte_slice` |
| 542 | `src/unit_test/storage/invertedindex/format/skiplist_writer_ut.cpp` | `SkipListWriterTest` | `test1` | ported | `infinity-storage` `inverted_format::posting_slice::tests::skiplist_writer_test1` |
| 543 | `src/unit_test/storage/invertedindex/format/skiplist_writer_ut.cpp` | `SkipListWriterTest` | `test2` | ported | `infinity-storage` `inverted_format::posting_slice::tests::skiplist_writer_test2` |
| 544 | `src/unit_test/storage/invertedindex/format/skiplist_writer_ut.cpp` | `SkipListWriterTest` | `test3` | ported | `infinity-storage` `inverted_format::posting_slice::tests::skiplist_writer_test3` |
| 545 | `src/unit_test/storage/invertedindex/format/term_meta_ut.cpp` | `TermMetaTest` | `test1` | ported | `infinity-storage` `inverted_format::tests::term_meta_all_options_roundtrip` |
| 546 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildMemEmpty` | ported | `infinity-storage` `inverted_format::tests::fst_build_mem_empty` |
| 547 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildFileEmpty` | ported | `infinity-storage` `inverted_format::tests::fst_build_file_empty` |
| 548 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildMem` | ported | `infinity-storage` `inverted_format::tests::fst_build_mem` |
| 549 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildFile` | ported | `infinity-storage` `inverted_format::tests::fst_build_file` |
| 550 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `Get` | ported | `infinity-storage` `inverted_format::tests::fst_get_values` |
| 551 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `Iterate` | ported | `infinity-storage` `inverted_format::tests::fst_iterate_with_bounds` |
| 552 | `src/unit_test/storage/invertedindex/fst/registry_ut.cpp` | `RegistryTest` | `InsertAndFind` | ported | `infinity-storage` `fst_registry::tests::insert_and_find` |
| 553 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `Chunk` | ported | `infinity-executor` `tests::memory_indexer_builds_the_reference_postings` |
| 554 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `DISABLED_SLOW_Memory` | ported | `infinity-executor` `tests::disabled_slow_memory`（`#[ignore]`） |
| 555 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `SpillLoadTest` | ported | `infinity-executor` `memory_indexer_ut::spill_load_test` |
| 556 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `DISABLED_SLOW_SeekPosition` | ported | `infinity-executor` `tests::disabled_slow_seek_position`（`#[ignore]`） |
| 557 | `src/unit_test/storage/invertedindex/posting_merger_ut.cpp` | `PostingMergerTest` | `Basic` | ported | `infinity-storage` `posting_merger_ut::basic` |
| 558 | `src/unit_test/storage/invertedindex/posting_writer_ut.cpp` | `PostingWriterTest` | `test1` | ported | `infinity-storage` `posting_writer_ut::test1` |
| 559 | `src/unit_test/storage/invertedindex/posting_writer_ut.cpp` | `PostingWriterTest` | `tf_after_skip_to_across_segments` | ported | `infinity-storage` `posting_writer_ut::tf_after_skip_to_across_segments` |
| 560 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test1` | ported | `infinity-executor` `highlighter_ut::test1` |
| 561 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test2` | ported | `infinity-executor` `highlighter_ut::test2` |
| 562 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test3` | ported | `infinity-executor` `highlighter_ut::test3` |
| 563 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test4` | ported | `infinity-executor` `highlighter_ut::test4` |
| 564 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_and` | ported | `infinity-executor` `query_builder::tests::test_and` |
| 565 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_or` | ported | `infinity-executor` `query_builder::tests::test_or` |
| 566 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_and_not` | ported | `infinity-executor` `query_builder::tests::test_and_not` |
| 567 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_and_not2` | ported | `infinity-executor` `query_builder::tests::test_and_not2` |
| 568 | `src/unit_test/storage/invertedindex/search/query_match_ut.cpp` | `QueryMatchTest` | `basic_term` | ported | `infinity-executor` `query_match_ut::basic_term` |
| 569 | `src/unit_test/storage/invertedindex/search/query_match_ut.cpp` | `QueryMatchTest` | `phrase` | ported | `infinity-executor` `query_match_ut::phrase` |
| 570 | `src/unit_test/storage/invertedindex/search/query_match_ut.cpp` | `QueryMatchTest` | `bmw_many_terms` | ported | `infinity-executor` `query_match_ut::bmw_many_terms` |
| 571 | `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp` | `QueryParserAndOptimizerTest` | `test1` | ported | `infinity-executor` `query_parser_and_optimizer_ut::test1` |
| 572 | `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp` | `QueryParserAndOptimizerTest` | `operator_option_test_standard` | ported | `infinity-executor` `query_parser_and_optimizer_ut::operator_option_test_standard` |
| 573 | `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp` | `QueryParserAndOptimizerTest` | `operator_option_test_chinese` | ported | `infinity-executor` `query_parser_and_optimizer_ut::operator_option_test_chinese` |
| 574 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test1` | ported | `infinity-storage` `io::tests::byte_slice_writer_tracks_size` |
| 575 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test2` | ported | `infinity-storage` `io::tests::byte_slice_writer_int16_roundtrip` |
| 576 | src/unit_test/storage/io/byte_slice_write_read_ut.cpp | `test3` | ported | `infinity-storage` `highlighter_ut::test3` |
| 577 | src/unit_test/storage/io/byte_slice_write_read_ut.cpp | `test4` | ported | `infinity-storage` `highlighter_ut::test4` |
| 578 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test5` | ported | `infinity-storage` `io::tests::byte_slice_writer_vlong_roundtrip` |
| 579 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test6` | ported | `infinity-storage` `io::tests::byte_slice_writer_dump_and_load` |
| 580 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test7` | ported | `infinity-storage` `io::tests::byte_slice_writer_merge_and_partial_copy` |
| 581 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `TestDataConsistency` | ported | `infinity-storage` `io::tests::byte_slice_reader_writer_data_consistency` |
| 582 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `test1` | ported | `infinity-storage` `io::tests::file_writer_read_prefix` |
| 583 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `test2` | ported | `infinity-storage` `io::tests::file_writer_vint_roundtrip` |
| 584 | src/unit_test/storage/io/file_write_read_ut.cpp | `test3` | ported | `infinity-storage` `highlighter_ut::test3` |
| 585 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestExceedWriterTotalSize` | ported | `infinity-storage` `io::tests::file_writer_exceed_total_size` |
| 586 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestFilePointerSeek` | ported | `infinity-storage` `io::tests::file_reader_file_pointer_seek` |
| 587 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestFileReadOverflowBuffer` | ported | `infinity-storage` `io::tests::file_reader_read_overflow_buffer` |
| 588 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestFileIODataTypes` | ported | `infinity-storage` `io::tests::file_reader_io_data_types` |
| 589 | `src/unit_test/storage/io/local_file_ut.cpp` | `LocalFileTest` | `test1` | ported | `infinity-storage` `io::tests::local_file_read_write_mmap` |
| 590 | `src/unit_test/storage/io/s3_client_minio_ut.cpp` | `S3ClientMinioTest` | `test1` | ported | `infinity-storage` `s3::tests::minio_client_roundtrip` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 591 | `src/unit_test/storage/io/stream_reader_ut.cpp` | `StreamReaderTest` | `TestBasicStreamIO` | ported | `infinity-storage` `io::tests::stream_reader_reads_lines` |
| 592 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestAppend` | ported | `infinity-storage` `virtual_store::tests::append_then_delete` |
| 593 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestDir` | ported | `infinity-storage` `virtual_store::tests::dir_create_and_remove` |
| 594 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestRead` | ported | `infinity-storage` `virtual_store::tests::write_then_read` |
| 595 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestRename` | ported | `infinity-storage` `virtual_store::tests::rename_moves_the_file` |
| 596 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestTruncate` | ported | `infinity-storage` `virtual_store::tests::truncate_cuts_the_tail` |
| 597 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestMerge` | ported | `infinity-storage` `virtual_store::tests::merge_appends_src_to_dst` |
| 598 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestCleanDir` | ported | `infinity-storage` `virtual_store::tests::cleanup_directory_keeps_the_directory` |
| 599 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `minio_upload` | ported | `infinity-storage` `s3::tests::minio_upload_object` (`#[ignore]`: C++ MinIO) |
| 600 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `minio_download` | ported | `infinity-storage` `s3::tests::minio_download_object` (`#[ignore]`: C++ MinIO) |
| 601 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestStorageType` | ported | `infinity-storage` `virtual_store::tests::storage_type_conversions` |
| 602 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestFileCompress` | ported | `infinity-storage` `virtual_store::tests::file_compress_writes_a_record` |
| 603 | `src/unit_test/storage/knnindex/emvb_search/test_emvb_ut.cpp` | `EMVBTest` | `test_fakepq` | ported | `infinity-index` `emvb::tests::fake_pq_ranks_the_matching_centroids_documents` |
| 604 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testsum256` | ported | `infinity-common` `simd::tests::emvb_simd_sum256` |
| 605 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testloadstore` | ported | `infinity-common` `simd::tests::emvb_simd_load_store` |
| 606 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testmask` | ported | `infinity-common` `simd::tests::emvb_simd_mask` |
| 607 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testmax` | ported | `infinity-common` `simd::tests::emvb_simd_max` |
| 608 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testleftpack` | ported | `infinity-common` `simd::tests::emvb_simd_left_pack` |
| 609 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testleftpackf` | ported | `infinity-common` `simd::tests::emvb_simd_left_pack_filter` |
| 610 | `src/unit_test/storage/knnindex/knn_diskann/test_build_diskann_ut_nimpl.cpp` | `DiskAnnTest` | `test1` | excluded |
| 611 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_ip_blas_reservoir_ut.cpp` | `KnnFlatIpBlasReservoirTest` | `test1` | ported | `infinity-index` `knn_flat::tests::ip_top_k_matches_the_reference` |
| 612 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_ip_blas_ut.cpp` | `KnnFlatIpBlasTest` | `test1` | ported | `infinity-index` `knn_flat::tests::ip_top_k_matches_the_reference` |
| 613 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_ip_reservoir_ut.cpp` | `KnnFlatIpReservoirTest` | `test1` | ported | `infinity-index` `knn_flat::tests::ip_top_k_matches_the_reference` |
| 614 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_ip_ut.cpp` | `KnnFlatIpTest` | `test1` | ported | `infinity-index` `knn_flat::tests::ip_top_k_matches_the_reference` |
| 615 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_blas_reservoir_ut.cpp` | `KnnFlatL2BlasReservoirTest` | `test1` | ported | `infinity-index` `knn_flat::tests::l2_top_k_matches_the_reference` |
| 616 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_blas_ut.cpp` | `KnnFlatL2BlasTest` | `test1` | ported | `infinity-index` `knn_flat::tests::l2_top_k_matches_the_reference` |
| 617 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_reservoir_ut.cpp` | `KnnFlatL2ReservoirTest` | `test1` | ported | `infinity-index` `knn_flat::tests::l2_top_k_matches_the_reference` |
| 618 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_top1_blas_ut.cpp` | `KnnFlatL2Top1BlasTest` | `test1` | ported | `infinity-index` `knn_flat::tests::l2_top_1_matches_the_reference` |
| 619 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_top1_ut.cpp` | `KnnFlatL2Top1Test` | `test1` | ported | `infinity-index` `knn_flat::tests::l2_top_1_matches_the_reference` |
| 620 | `src/unit_test/storage/knnindex/knn_flat/knn_flat_l2_ut.cpp` | `KnnFlatL2Test` | `test1` | ported | `infinity-index` `knn_flat::tests::l2_top_k_matches_the_reference` |
| 621 | `src/unit_test/storage/knnindex/knn_hnsw/test_dist_func2_ut.cpp` | `DistFuncTest2` | `test2` | ported | `infinity-index` `lvq::tests::lvq_ip_distance_matches_decompressed_vectors` |
| 622 | `src/unit_test/storage/knnindex/knn_hnsw/test_dist_func_ut.cpp` | `DistFuncTest` | `test1` | ported | `infinity-index` `lvq::tests::i8_ip_residual_matches_the_scalar_dot_product` |
| 623 | `src/unit_test/storage/knnindex/knn_hnsw/test_dist_func_ut.cpp` | `DistFuncTest` | `test2` | ported | `infinity-index` `lvq::tests::lvq_l2_distance_matches_decompressed_vectors` |
| 624 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_bitmask_ut.cpp` | `HnswAlgBitmaskTest` | `test1` | ported | `infinity-index` `vector::reference_suite::filtered_search_honours_the_bitmask` |
| 625 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `test_memory` | ported | `infinity-index` `hnsw_handler::reference_suite::test_memory` |
| 626 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `test_compress` | ported | `infinity-index` `hnsw_handler::reference_suite::test_compress` |
| 627 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `DISABLED_SLOW_test_load` | ported | `infinity-index` `vector::reference_suite::disabled_slow_test_load`（`#[ignore]`） |
| 628 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `DISABLED_RANDOME_test_parallel` | ported | `infinity-index` `vector::reference_suite::disabled_random_test_parallel`（`#[ignore]`） |
| 629 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_sparse_ut.cpp` | `HnswSparseTest` | `test1` | ported | `infinity-index` `vector::reference_suite::sparse_ip_hnsw_roundtrip` |
| 630 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_1` | ported | `infinity-index` `vector::reference_suite::plain_l2_self_recall` |
| 631 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_2` | ported | `infinity-index` `vector::reference_suite::plain_cos_self_recall` |
| 632 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_3` | ported | `infinity-index` `vector::reference_suite::plain_parallel_insert_and_search` |
| 633 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_4` | ported | `infinity-index` `vector::reference_suite::chunk_roundtrip_preserves_recall` |
| 634 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_1` | ported | `infinity-index` `vector::reference_suite::lvq_l2_self_recall` |
| 635 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_2` | ported | `infinity-index` `vector::reference_suite::lvq_parallel_insert_and_search` |
| 636 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_3` | ported | `infinity-index` `vector::reference_suite::lvq_chunk_roundtrip_preserves_recall` |
| 637 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_4` | ported | `infinity-index` `vector::reference_suite::plain_index_compresses_to_lvq` |
| 638 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_rabitq_1` | ported | `infinity-index` `vector::reference_suite::rabitq_l2_self_recall` |
| 639 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_rabitq_2` | ported | `infinity-index` `vector::reference_suite::rabitq_chunk_roundtrip_preserves_recall` |
| 640 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_rabitq_3` | ported | `infinity-index` `vector::reference_suite::rabitq_parallel_insert_and_search` |
| 641 | `src/unit_test/storage/knnindex/knn_hnsw/test_lsg_build_ut.cpp` | `LSGBuildTest` | `test_avg` | ported | `infinity-index` `lsg::tests::avg_is_at_least_the_exhaustive_topk_mean` |
| 642 | `src/unit_test/storage/knnindex/knn_hnsw/test_lsg_build_ut.cpp` | `LSGBuildTest` | `test1` | ported | `infinity-executor` `tests::lsg_hnsw_keeps_self_query_recall` |
| 643 | `src/unit_test/storage/knnindex/knn_hnsw/test_lvq_ut.cpp` | `HnswLVQTest` | `test1` | ported | `infinity-index` `lvq::tests::reconstruction_error_is_below_two_percent` |
| 644 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_flat_lvq` | ported | `infinity-index` `quantizer_perf::disabled_slow_flat_lvq` (`#[ignore]`: C++ DISABLED_SLOW_flat_lvq) |
| 645 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_flat_rabitq` | ported | `infinity-index` `quantizer_perf::disabled_slow_flat_rabitq` (`#[ignore]`: C++ DISABLED_SLOW_flat_rabitq) |
| 646 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_hnsw_lvq` | ported | `infinity-index` `quantizer_perf::disabled_slow_hnsw_lvq` (`#[ignore]`: C++ DISABLED_SLOW_hnsw_lvq) |
| 647 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_hnsw_rabitq` | ported | `infinity-index` `quantizer_perf::disabled_slow_hnsw_rabitq` (`#[ignore]`: C++ DISABLED_SLOW_hnsw_rabitq) |
| 648 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_hnsw_lsg_rabitq` | ported | `infinity-index` `quantizer_perf::disabled_slow_hnsw_lsg_rabitq` (`#[ignore]`: C++ DISABLED_SLOW_hnsw_lsg_rabitq) |
| 649 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_compress_hnsw_to_rabitq` | ported | `infinity-index` `quantizer_perf::disabled_slow_compress_hnsw_to_rabitq` (`#[ignore]`: C++ DISABLED_SLOW_compress_hnsw_to_rabitq) |
| 650 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_compress_lsg_to_rabitq` | ported | `infinity-index` `quantizer_perf::disabled_slow_compress_lsg_to_rabitq` (`#[ignore]`: C++ DISABLED_SLOW_compress_lsg_to_rabitq) |
| 651 | `src/unit_test/storage/knnindex/knn_hnsw/test_rabitq_ut.cpp` | `RabitqTest` | `test_simple` | ported | `infinity-index` `rabitq::tests::centroid_is_the_rotated_mean` |
| 652 | `src/unit_test/storage/knnindex/knn_hnsw/test_rabitq_ut.cpp` | `RabitqTest` | `test_compress` | ported | `infinity-index` `rabitq::tests::test_compress` |
| 653 | `src/unit_test/storage/knnindex/knn_hnsw/test_rabitq_ut.cpp` | `RabitqTest` | `test_distance` | ported | `infinity-index` `rabitq::tests::l2_estimator_keeps_the_true_neighbour` |
| 654 | `src/unit_test/storage/knnindex/knn_ivf/index_ivf_def_ut.cpp` | `IndexIVFTest` | `testplain` | ported | `infinity-index` `ivf_def::tests::testplain` |
| 655 | `src/unit_test/storage/knnindex/knn_ivf/index_ivf_def_ut.cpp` | `IndexIVFTest` | `testsq` | ported | `infinity-index` `ivf_def::tests::testsq` |
| 656 | `src/unit_test/storage/knnindex/knn_ivf/index_ivf_def_ut.cpp` | `IndexIVFTest` | `testpq` | ported | `infinity-index` `ivf_def::tests::testpq` |
| 657 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_fwd_ut.cpp` | `BMPFwdTest` | `test1` | ported | `infinity-index` `sparse::tests::block_fwd_groups_docs_into_blocks` |
| 658 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_index_ut.cpp` | `BMPIndexTest` | `test1` | ported | `infinity-index` `sparse::bmp_reference_suite::bmp_index_accuracy_and_disk_roundtrip` |
| 659 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_index_ut.cpp` | `BMPIndexTest` | `test2` | ported | `infinity-index` `sparse::tests::bmp_index_two_columns_topk` |
| 660 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_ivt_ut.cpp` | `BMPIvtTest` | `test1` | ported | `infinity-index` `sparse::tests::bmp_ivt_keeps_block_maxima_and_kth` |
| 661 | `src/unit_test/storage/knnindex/knn_sparse/test_bp_reordering_ut.cpp` | `BPReorderingTest` | `test1` | ported | `infinity-index` `bp::tests::reordering_does_not_increase_cost` |
| 662 | `src/unit_test/storage/knnindex/knn_sparse/test_bp_reordering_ut.cpp` | `BPReorderingTest` | `test2` | ported | `infinity-index` `bp::tests::reordering_on_the_small_csr_when_present` |
| 663 | `src/unit_test/storage/knnindex/knn_sparse/test_linscan_ut.cpp` | `LinScanAlgTest` | `accurate_scan` | ported | `infinity-index` `linscan::tests::accurate_scan` |
| 664 | `src/unit_test/storage/knnindex/knn_sparse/test_linscan_ut.cpp` | `LinScanAlgTest` | `approximate_scan` | ported | `infinity-index` `linscan::tests::approximate_scan` |
| 665 | `src/unit_test/storage/knnindex/merge_optimize/test_optimize_ut.cpp` | `OptimizeKnnTest` | `test_hnsw_optimize` | ported | `infinity-executor` `tests::test_hnsw_optimize` |
| 666 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidQuantizerTest` | `test_train_quantize_dequantize` | ported | `infinity-index` `plaid::quantizer::tests::train_quantize_dequantize` |
| 667 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidQuantizerTest` | `test_2bit_quantization` | ported | `infinity-index` `plaid::quantizer::tests::two_bit_quantization` |
| 668 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_basic_construction` | ported | `infinity-index` `plaid::index::tests::basic_construction` |
| 669 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_train_and_add` | ported | `infinity-index` `plaid::index::tests::train_and_add` |
| 670 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_search` | ported | `infinity-index` `plaid::index::tests::search_ranks_the_queried_document_first` |
| 671 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_save_load` | ported | `infinity-index` `plaid::index::tests::save_and_load_roundtrip` |
| 672 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_move_semantics` | ported | `infinity-index` `plaid::index::tests::move_moves_the_index` |
| 673 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_mem_usage` | ported | `infinity-index` `plaid::index::tests::mem_usage_matches_calc_size` |
| 674 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_construction` | ported | `infinity-index` `plaid::index::tests::colbertsar_construction` |
| 675 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_train_and_add` | ported | `infinity-index` `plaid::index::tests::colbertsar_mode_trains_and_searches_without_residuals` |
| 676 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_basic_search` | ported | `infinity-index` `plaid::index::tests::colbertsar_mode_trains_and_searches_without_residuals` |
| 677 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_save_load` | ported | `infinity-index` `plaid::index::tests::colbertsar_mode_trains_and_searches_without_residuals` |
| 678 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_move_semantics` | ported | `infinity-index` `plaid::index::tests::colbertsar_move_semantics` |
| 679 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_mem_usage` | ported | `infinity-index` `plaid::index::tests::colbertsar_mem_usage` |
| 680 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_merge` | ported | `infinity-index` `plaid::index::tests::colbertsar_merge` |
| 681 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_size_smaller_than_plaid` | ported | `infinity-index` `plaid::index::tests::colbertsar_size_smaller_than_plaid` |
| 682 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `SaveAndLoad` | ported | `infinity-storage` `mvcc::tests::save_and_load_roundtrip` |
| 683 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `SaveAndLoad2` | ported | `mvcc::tests::reopen_preserves_the_slot_array` |
| 684 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `delete_test` | ported | `mvcc::tests::delete_twice_is_rejected` |
| 685 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `check_delete_test` | ported | `mvcc::tests::check_delete_compares_timestamps` |
| 686 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `get_delete_ts_test` | ported | `mvcc::tests::get_delete_ts_returns_the_slot_range` |
| 687 | src/unit_test/storage/new_catalog/alter_ut.cpp | `add_column0` | ported | `infinity-storage` `alter_ut::add_column0` |
| 688 | src/unit_test/storage/new_catalog/alter_ut.cpp | `drop_column0` | ported | `infinity-storage` `alter_ut::drop_column0` |
| 689 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append0` | ported | `infinity-executor` `catalog_ut::appends_accumulate_across_commits` |
| 690 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append1` | ported | `infinity-executor` `catalog_ut::appends_accumulate_across_commits` |
| 691 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append2` | ported | `infinity-executor` `catalog_ut::large_appends_span_blocks` |
| 692 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_mismatch` | ported | `infinity-executor` `catalog_ut::append_type_mismatch_is_rejected` |
| 693 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_drop_db` | ported | `infinity-executor` `catalog_ut::append_after_drop_is_rejected` |
| 694 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_drop_table` | ported | `infinity-executor` `catalog_ut::append_after_drop_is_rejected` |
| 695 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_add_column` | ported | `infinity-executor` `catalog_ut::append_add_column_fills_old_rows_with_null` |
| 696 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_drop_column` | ported | `infinity-executor` `catalog_ut::append_add_column_fills_old_rows_with_null` |
| 697 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_rename` | ported | `infinity-executor` `catalog_ut::append_uses_the_renamed_table` |
| 698 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_append` | ported | `infinity-executor` `append_delete_ut::append_append_keeps_one_visible_block` |
| 699 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `DISABLED_SLOW_test_append_append_concurrent` | ported | `infinity-executor` `tests::disabled_slow_test_append_append_concurrent` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 700 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_create_index` | ported | `infinity-executor` `catalog_ut::append_around_secondary_index_lifecycle` |
| 701 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_drop_index` | ported | `infinity-executor` `catalog_ut::append_around_secondary_index_lifecycle` |
| 702 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_compact` | ported | `infinity-executor` `catalog_ut::append_then_compact_preserves_rows` |
| 703 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_optimize_index` | ported | `infinity-executor` `append_delete_ut::append_and_optimize_index_keeps_rows_and_index` |
| 704 | `src/unit_test/storage/new_catalog/checkpoint1_ut.cpp` | `TestTxnCheckpointTest` | `checkpoint_and_create_db` | ported | `infinity-executor` `checkpoint_ut::checkpoint_and_create_db` |
| 705 | `src/unit_test/storage/new_catalog/checkpoint1_ut.cpp` | `TestTxnCheckpointTest` | `checkpoint_and_create_table` | ported | `infinity-executor` `checkpoint_ut::checkpoint_and_create_table` |
| 706 | src/unit_test/storage/new_catalog/checkpoint_add_column_ut.cpp | `addcol_checkpoint_insert` | ported | `infinity-storage` `checkpoint_ut::addcol_checkpoint_insert` |
| 707 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `test_checkpoint0` | ported | `infinity-executor` `checkpoint_ut::checkpoint_internal_test_checkpoint0` |
| 708 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `DISABLED_SLOW_test_checkpoint1` | ported | `infinity-executor` `checkpoint_ut::checkpoint_internal_test_checkpoint1` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 709 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `test_checkpoint2` | ported | `infinity-executor` `checkpoint_ut::checkpoint_internal_test_checkpoint2` |
| 710 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `DISABLED_SLOW_test_checkpoint3` | ported | `infinity-executor` `checkpoint_ut::checkpoint_internal_test_checkpoint3` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 711 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `DISABLED_SLOW_test_checkpoint4` | ported | `infinity-executor` `checkpoint_ut::checkpoint_internal_test_checkpoint4` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 712 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `test_checkpoint5` | ported | `infinity-executor` `checkpoint_ut::checkpoint_internal_test_checkpoint5` |
| 713 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_db` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_db` |
| 714 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_table` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_table` |
| 715 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_index` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_index` |
| 716 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_compact` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_compact` |
| 717 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_optimize` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_optimize` |
| 718 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_drop_column` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_drop_column` |
| 719 | src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp | `test_cleanup_drop_index_and_checkpoint_and_restart` | ported | `infinity-storage` `cleanup_internal_ut::test_cleanup_drop_index_and_checkpoint_and_restart` |
| 720 | src/unit_test/storage/new_catalog/cleanup_rollback_txn.cpp | `test_import_with_index_rollback_cleanup` | ported | `infinity-storage` `cleanup_rollback_txn_ut::test_import_with_index_rollback_cleanup` |
| 721 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_with_drop_db` | ported | `infinity-storage` `cleanup_ut::cleanup_with_drop_db` |
| 722 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_drop_table` | ported | `infinity-storage` `cleanup_ut::cleanup_and_drop_table` |
| 723 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_add_columns` | ported | `infinity-storage` `cleanup_ut::cleanup_and_add_columns` |
| 724 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_drop_columns` | ported | `infinity-storage` `cleanup_ut::cleanup_and_drop_columns` |
| 725 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_rename_table` | ported | `infinity-storage` `cleanup_ut::cleanup_and_rename_table` |
| 726 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_compact` | ported | `infinity-storage` `cleanup_ut::cleanup_and_compact` |
| 727 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_drop_index` | ported | `infinity-storage` `cleanup_ut::cleanup_and_drop_index` |
| 728 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_optimize_index` | ported | `infinity-storage` `cleanup_ut::cleanup_and_optimize_index` |
| 729 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_append` | ported | `infinity-storage` `cleanup_ut::cleanup_and_append` |
| 730 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_delete` | ported | `infinity-storage` `cleanup_ut::cleanup_and_delete` |
| 731 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_update` | ported | `infinity-storage` `cleanup_ut::cleanup_and_update` |
| 732 | src/unit_test/storage/new_catalog/cleanup_ut.cpp | `cleanup_and_dump_index` | ported | `infinity-storage` `cleanup_ut::cleanup_and_dump_index` |
| 733 | src/unit_test/storage/new_catalog/column_ut.cpp | `test_add_columns` | ported | `infinity-executor` `new_catalog_column_ut::test_add_columns` |
| 734 | src/unit_test/storage/new_catalog/column_ut.cpp | `alter_column` | ported | `infinity-executor` `new_catalog_column_ut::alter_column` |
| 735 | src/unit_test/storage/new_catalog/column_ut.cpp | `add_column_and_drop_db` | ported | `infinity-executor` `new_catalog_column_ut::add_column_and_drop_db` |
| 736 | src/unit_test/storage/new_catalog/column_ut.cpp | `add_column_and_drop_table` | ported | `infinity-executor` `new_catalog_column_ut::add_column_and_drop_table` |
| 737 | src/unit_test/storage/new_catalog/column_ut.cpp | `add_column_and_add_column` | ported | `infinity-executor` `new_catalog_column_ut::add_column_and_add_column` |
| 738 | src/unit_test/storage/new_catalog/column_ut.cpp | `drop_column_and_drop_db` | ported | `infinity-executor` `new_catalog_column_ut::drop_column_and_drop_db` |
| 739 | src/unit_test/storage/new_catalog/column_ut.cpp | `drop_column_and_drop_table` | ported | `infinity-executor` `new_catalog_column_ut::drop_column_and_drop_table` |
| 740 | src/unit_test/storage/new_catalog/column_ut.cpp | `drop_column_and_add_column` | ported | `infinity-executor` `new_catalog_column_ut::drop_column_and_add_column` |
| 741 | src/unit_test/storage/new_catalog/column_ut.cpp | `drop_column_and_drop_column` | ported | `infinity-executor` `new_catalog_column_ut::drop_column_and_drop_column` |
| 742 | `src/unit_test/storage/new_catalog/compact_internal_ut.cpp` | `TestTxnCompactInternal` | `test_compact` | ported | `infinity-executor` `compact_replay_ut::internal_compact_skips_deleted_offsets` |
| 743 | src/unit_test/storage/new_catalog/compact_internal_ut.cpp | `test_compact_with_index` | ported | `infinity-executor` `compact_replay_ut::disabled_slow_test_compact_with_index` |
| 744 | src/unit_test/storage/new_catalog/compact_ut.cpp | `compact_with_index_commit` | ported | `infinity-executor` `compact_replay_ut::compact_with_index_commit` |
| 745 | src/unit_test/storage/new_catalog/compact_ut.cpp | `compact_with_index_rollback` | ported | `infinity-executor` `compact_replay_ut::compact_with_index_rollback` |
| 746 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_db` | ported | `infinity-executor` `compact_replay_ut::compact_then_drop_database` |
| 747 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_table` | ported | `infinity-executor` `compact_replay_ut::compact_then_drop_table` |
| 748 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_add_columns` | ported | `infinity-executor` `compact_replay_ut::compact_then_add_columns` |
| 749 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_columns` | ported | `infinity-executor` `compact_replay_ut::compact_then_drop_columns` |
| 750 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_rename_table` | ported | `infinity-executor` `compact_replay_ut::compact_then_rename_table` |
| 751 | src/unit_test/storage/new_catalog/compact_ut.cpp | `compact_and_compact` | ported | `infinity-executor` `compact_replay_ut::compact_and_compact` |
| 752 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_create_index` | ported | `infinity-executor` `compact_replay_ut::compact_then_create_index` |
| 753 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_index` | ported | `infinity-executor` `compact_replay_ut::compact_then_drop_index` |
| 754 | src/unit_test/storage/new_catalog/database_ut.cpp | `createdb_createdb_test` | ported | `infinity-executor` `new_catalog_database_ut::createdb_createdb_test` |
| 755 | src/unit_test/storage/new_catalog/database_ut.cpp | `db_test2` | ported | `infinity-executor` `new_catalog_database_ut::db_test2` |
| 756 | src/unit_test/storage/new_catalog/database_ut.cpp | `db_test3` | ported | `infinity-executor` `new_catalog_database_ut::db_test3` |
| 757 | src/unit_test/storage/new_catalog/database_ut.cpp | `createdb_wrong_conflicttype` | ported | `infinity-executor` `new_catalog_database_ut::createdb_wrong_conflicttype` |
| 758 | src/unit_test/storage/new_catalog/database_ut.cpp | `dropdb_wrong_conflicttype` | ported | `infinity-executor` `new_catalog_database_ut::dropdb_wrong_conflicttype` |
| 759 | src/unit_test/storage/new_catalog/database_ut.cpp | `dropdb_dropdb_test` | ported | `infinity-executor` `new_catalog_database_ut::dropdb_dropdb_test` |
| 760 | src/unit_test/storage/new_catalog/database_ut.cpp | `createdb_dropdb_test` | ported | `infinity-executor` `new_catalog_database_ut::createdb_dropdb_test` |
| 761 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete` | ported | `infinity-executor` `catalog_ut::delete_removes_rows_from_scans` |
| 762 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_multiple_blocks` | ported | `infinity-executor` `catalog_ut::delete_across_multiple_blocks` |
| 763 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_db` | ported | `infinity-executor` `append_delete_ut::delete_then_drop_database` |
| 764 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_table` | ported | `infinity-executor` `append_delete_ut::delete_then_drop_table` |
| 765 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_add_column` | ported | `infinity-executor` `append_delete_ut::delete_then_add_column` |
| 766 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_column` | ported | `infinity-executor` `append_delete_ut::delete_then_drop_column` |
| 767 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_rename` | ported | `infinity-executor` `append_delete_ut::delete_then_rename_table` |
| 768 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_create_index` | ported | `infinity-executor` `append_delete_ut::delete_then_create_index` |
| 769 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_index` | ported | `infinity-executor` `append_delete_ut::delete_then_drop_index` |
| 770 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_import` | ported | `infinity-executor` `append_delete_ut::delete_then_import` |
| 771 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_append` | ported | `infinity-executor` `catalog_ut::delete_then_append_and_compact` |
| 772 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_delete` | ported | `infinity-executor` `append_delete_ut::delete_then_delete` |
| 773 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_compact` | ported | `infinity-executor` `append_delete_ut::delete_then_compact` |
| 774 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_optimize_index` | ported | `infinity-executor` `append_delete_ut::delete_then_optimize_index` |
| 775 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_db` | ported | `infinity-executor` `dump_mem_index_ut::dump_and_drop_db` |
| 776 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_table` | ported | `infinity-executor` `dump_mem_index_ut::dump_and_drop_table` |
| 777 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `dump_and_add_column` | ported | `infinity-storage` `dump_mem_index_ut::dump_and_add_column` |
| 778 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `dump_and_drop_column` | ported | `infinity-storage` `dump_mem_index_ut::dump_and_drop_column` |
| 779 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_rename_table` | ported | `infinity-executor` `dump_mem_index_ut::dump_and_rename_table` |
| 780 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `dump_and_create_index` | ported | `infinity-storage` `dump_mem_index_ut::dump_and_create_index` |
| 781 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_index` | ported | `infinity-executor` `dump_mem_index_ut::dump_and_drop_index` |
| 782 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_import` | ported | `infinity-executor` `dump_mem_index_ut::dump_and_import` |
| 783 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `dump_and_append` | ported | `infinity-storage` `dump_mem_index_ut::dump_and_append` |
| 784 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `dump_and_delete` | ported | `infinity-storage` `dump_mem_index_ut::dump_and_delete` |
| 785 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `DISABLED_SLOW_dump_and_dump` | ported | `infinity-storage` `dump_mem_index_ut::dump_and_dump` |
| 786 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `DISABLED_SLOW_test_dump_index_and_optimize_index` | ported | `infinity-executor` `dump_mem_index_ut::dump_index_and_optimize_index` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 787 | src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp | `test_dump_index_and_compact` | ported | `infinity-storage` `dump_mem_index_ut::test_dump_index_and_compact` |
| 788 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import1` | ported | `infinity-executor` `update_import_ut::import1_keeps_every_block` |
| 789 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_with_index` | ported | `infinity-executor` `update_import_ut::import_with_index` |
| 790 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_with_index_rollback` | ported | `infinity-executor` `update_import_ut::import_with_index_rollback` |
| 791 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_insert_and_import` | ported | `infinity-executor` `update_import_ut::insert_and_import` |
| 792 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_drop_db` | ported | `infinity-executor` `update_import_ut::import_then_drop_database` |
| 793 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_drop_table` | ported | `infinity-executor` `update_import_ut::import_then_drop_table` |
| 794 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_add_columns` | ported | `infinity-executor` `update_import_ut::import_then_add_columns` |
| 795 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_drop_columns` | ported | `infinity-executor` `update_import_ut::import_then_drop_columns` |
| 796 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import` | ported | `infinity-executor` `update_import_ut::import_keeps_block_boundaries` |
| 797 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_append_table` | ported | `infinity-executor` `update_import_ut::import_then_append` |
| 798 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_import_table` | ported | `infinity-executor` `update_import_ut::import_then_import` |
| 799 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_rename` | ported | `infinity-executor` `update_import_ut::import_then_rename` |
| 800 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_and_create_index` | ported | `infinity-executor` `update_import_ut::import_then_create_index` |
| 801 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_and_drop_index` | ported | `infinity-executor` `update_import_ut::import_then_drop_index` |
| 802 | src/unit_test/storage/new_catalog/import_ut.cpp | `DISABLED_SLOW_test_import_and_compact` | ported | `infinity-executor` `update_import_ut::disabled_slow_test_import_and_compact` |
| 803 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_and_optimize_index` | ported | `infinity-executor` `update_import_ut::import_then_optimize_index` |
| 804 | src/unit_test/storage/new_catalog/index_internal_ut.cpp | `test_index0` | ported | `infinity-executor` `new_catalog_index_internal_ut::test_index0` |
| 805 | src/unit_test/storage/new_catalog/index_internal_ut.cpp | `test_index` | ported | `infinity-executor` `new_catalog_index_internal_ut::test_index` |
| 806 | src/unit_test/storage/new_catalog/index_internal_ut.cpp | `test_populate_index0` | ported | `infinity-executor` `new_catalog_index_internal_ut::test_populate_index0` |
| 807 | src/unit_test/storage/new_catalog/index_internal_ut.cpp | `DISABLED_SLOW_test_populate_index` | ported | `infinity-executor` `new_catalog_index_internal_ut::disabled_slow_test_populate_index` |
| 808 | src/unit_test/storage/new_catalog/index_internal_ut.cpp | `test_json_index` | ported | `infinity-executor` `new_catalog_index_internal_ut::test_json_index` |
| 809 | src/unit_test/storage/new_catalog/index_ut.cpp | `index_test1` | ported | `infinity-executor` `new_catalog_index_ut::index_test1` |
| 810 | src/unit_test/storage/new_catalog/index_ut.cpp | `index_test2` | ported | `infinity-executor` `new_catalog_index_ut::index_test2` |
| 811 | src/unit_test/storage/new_catalog/index_ut.cpp | `createindex_wrong_conflicttype` | ported | `infinity-executor` `new_catalog_index_ut::createindex_wrong_conflicttype` |
| 812 | src/unit_test/storage/new_catalog/index_ut.cpp | `dropindex_wrong_conflicttype` | ported | `infinity-executor` `new_catalog_index_ut::dropindex_wrong_conflicttype` |
| 813 | src/unit_test/storage/new_catalog/index_ut.cpp | `create_index_and_drop_db` | ported | `infinity-executor` `new_catalog_index_ut::create_index_and_drop_db` |
| 814 | src/unit_test/storage/new_catalog/index_ut.cpp | `create_index_and_drop_table` | ported | `infinity-executor` `new_catalog_index_ut::create_index_and_drop_table` |
| 815 | src/unit_test/storage/new_catalog/index_ut.cpp | `DISABLED_SLOW_create_index_create_index` | ported | `infinity-executor` `new_catalog_index_ut::disabled_slow_create_index_create_index` |
| 816 | src/unit_test/storage/new_catalog/index_ut.cpp | `create_index_add_column` | ported | `infinity-executor` `new_catalog_index_ut::create_index_add_column` |
| 817 | src/unit_test/storage/new_catalog/index_ut.cpp | `create_index_drop_column` | ported | `infinity-executor` `new_catalog_index_ut::create_index_drop_column` |
| 818 | src/unit_test/storage/new_catalog/index_ut.cpp | `create_index_rename_table` | ported | `infinity-executor` `new_catalog_index_ut::create_index_rename_table` |
| 819 | src/unit_test/storage/new_catalog/index_ut.cpp | `drop_index_and_drop_db` | ported | `infinity-executor` `new_catalog_index_ut::drop_index_and_drop_db` |
| 820 | src/unit_test/storage/new_catalog/index_ut.cpp | `drop_index_and_drop_table` | ported | `infinity-executor` `new_catalog_index_ut::drop_index_and_drop_table` |
| 821 | src/unit_test/storage/new_catalog/index_ut.cpp | `create_index_and_drop_index` | ported | `infinity-executor` `new_catalog_index_ut::create_index_and_drop_index` |
| 822 | src/unit_test/storage/new_catalog/index_ut.cpp | `drop_index_and_drop_index` | ported | `infinity-executor` `new_catalog_index_ut::drop_index_and_drop_index` |
| 823 | src/unit_test/storage/new_catalog/index_ut.cpp | `drop_index_add_column` | ported | `infinity-executor` `new_catalog_index_ut::drop_index_add_column` |
| 824 | src/unit_test/storage/new_catalog/index_ut.cpp | `drop_index_drop_column` | ported | `infinity-executor` `new_catalog_index_ut::drop_index_drop_column` |
| 825 | src/unit_test/storage/new_catalog/index_ut.cpp | `drop_index_and_rename_table` | ported | `infinity-executor` `new_catalog_index_ut::drop_index_and_rename_table` |
| 826 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store0` | ported | `infinity-storage` `txn_kv::tests::kv_store0` |
| 827 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `DISABLED_SLOW_kv_store1` | ported | `infinity-storage` `txn_kv::tests::kv_store1_multi_version_transactions` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 828 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store2` | ported | `infinity-storage` `txn_kv::tests::kv_store2_disable_wal` |
| 829 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store3` | ported | `infinity-storage` `txn_kv::tests::kv_store3_snapshot_isolated_iterator` |
| 830 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store4` | ported | `infinity-storage` `txn_kv::tests::kv_store4_backup_restore` |
| 831 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store5` | ported | `infinity-storage` `txn_kv::tests::kv_store5_transaction_snapshot_iterator_bounds` |
| 832 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store6` | ported | `infinity-executor` `kv_ut::kv_store6_prefix_iteration_and_flush` |
| 833 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `wal` | ported | `infinity-storage` `txn_kv::tests::wal_disabled_put_is_lost_without_flush` |
| 834 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `txn_conflict_same_key_snapshot_isolation` | ported | `infinity-storage` `txn_kv::tests::txn_conflict_same_key_snapshot_isolation` |
| 835 | `src/unit_test/storage/new_catalog/kv_utility_ut.cpp` | `KVUtilityTest` | `test1` | ported | `infinity-executor` `kv_ut::kv_utility_catalog_keys_carry_the_timestamp` |
| 836 | src/unit_test/storage/new_catalog/meta_cache_ut.cpp | `test_db` | ported | `infinity-storage` `meta_cache_ut::test_db` |
| 837 | src/unit_test/storage/new_catalog/meta_cache_ut.cpp | `test_table` | ported | `infinity-storage` `meta_cache_ut::test_table` |
| 838 | `src/unit_test/storage/new_catalog/meta_cache_ut.cpp` | `MetaCacheTest` | `test_index` | ported | `infinity-executor` `meta_cache_ut::test_index` |
| 839 | src/unit_test/storage/new_catalog/meta_cache_ut.cpp | `test_mix` | ported | `infinity-storage` `meta_cache_ut::test_mix` |
| 840 | src/unit_test/storage/new_catalog/meta_cache_ut.cpp | `test_erase` | ported | `infinity-storage` `meta_cache_ut::test_erase` |
| 841 | src/unit_test/storage/new_catalog/new_catalog_ut.cpp | `test_block_lock` | ported | `infinity-executor` `new_catalog_meta_ut::test_block_lock` |
| 842 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `test_ts` | ported | `infinity-executor` `replay_misc_ut::txn_manager_timestamps` |
| 843 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `DISABLED_SLOW_test_parallel_ts` | ported | `infinity-executor` `tests::disabled_slow_test_parallel_ts` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 844 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `test_check_txns` | ported | `infinity-executor` `replay_misc_ut::txn_manager_check_txns` |
| 845 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `DISABLED_SLOW_test_parallel_insert_table` | ported | `infinity-executor` `tests::disabled_slow_test_parallel_insert_table` (`#[ignore]`: C++ DISABLED_SLOW_) |
| 846 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_create_db` | ported | `infinity-executor` `replay_txn_ut::replay_create_db` |
| 847 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_create_db2` | ported | `infinity-executor` `replay_txn_ut::replay_create_db2` |
| 848 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_create_table` | ported | `infinity-executor` `replay_txn_ut::replay_create_table` |
| 849 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_drop_table` | ported | `infinity-executor` `replay_txn_ut::replay_drop_table` |
| 850 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_create` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_create` |
| 851 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_drop` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_append_drop` |
| 852 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_drop_column_add_column` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_append_drop_column_add_column` |
| 853 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_compact` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_append_compact` |
| 854 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_delete_compact` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_delete_compact` |
| 855 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_rename` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_append_rename` |
| 856 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_dump_optimize_index` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_append_dump_optimize_index` |
| 857 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_append` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_append_append` |
| 858 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_dump_index` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_dump_index` |
| 859 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_optimize_index` | ported | `infinity-executor` `replay_txn_ut::replay_flush_gap_optimize_index` |
| 860 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_create_db` | ported | `infinity-executor` `replay_txn_ut::ckp_replay_create_db` |
| 861 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_import` | ported | `infinity-executor` `replay_txn_ut::ckp_replay_import` |
| 862 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_compact` | ported | `infinity-executor` `replay_txn_ut::ckp_replay_compact` |
| 863 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_insert` | ported | `infinity-executor` `replay_txn_ut::ckp_replay_insert` |
| 864 | src/unit_test/storage/new_catalog/optimize_index_ut.cpp | `optimize_index_rollback` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_rollback` |
| 865 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_db` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_then_drop_database` |
| 866 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_table` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_then_drop_table` |
| 867 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_index` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_then_drop_index` |
| 868 | src/unit_test/storage/new_catalog/optimize_index_ut.cpp | `DISABLED_SLOW_optimize_index_and_optimize_index` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_optimize_index_and_optimize_index` |
| 869 | src/unit_test/storage/new_catalog/optimize_index_ut.cpp | `DISABLED_SLOW_optimize_index_and_dump_index` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_optimize_index_and_dump_index` |
| 870 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_add_column` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_then_add_column` |
| 871 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_column` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_then_drop_column` |
| 872 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_rename_table` | ported | `infinity-executor` `optimize_replay_ut::optimize_index_then_rename_table` |
| 873 | src/unit_test/storage/new_catalog/optimize_index_ut.cpp | `DISABLED_SLOW_optimize_index_and_compact_table` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_optimize_index_and_compact_table` |
| 874 | `src/unit_test/storage/new_catalog/replay_alter_ut.cpp` | `TestTxnReplayAlter` | `test_add_column` | ported | `infinity-executor` `replay_txn_ut::replay_alter_add_column` |
| 875 | `src/unit_test/storage/new_catalog/replay_alter_ut.cpp` | `TestTxnReplayAlter` | `test_drop_column` | ported | `infinity-executor` `replay_txn_ut::replay_alter_drop_column` |
| 876 | `src/unit_test/storage/new_catalog/replay_alter_ut.cpp` | `TestTxnReplayAlter` | `test_rename` | ported | `infinity-executor` `replay_txn_ut::replay_alter_rename` |
| 877 | `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp` | `TestTxnReplayAppend` | `test_append0` | ported | `infinity-executor` `replay_misc_ut::replay_append0` |
| 878 | `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp` | `TestTxnReplayAppend` | `test_replay_append_delete` | ported | `infinity-executor` `replay_misc_ut::replay_append_delete` |
| 879 | `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp` | `TestTxnReplayAppend` | `test_replay_append_with_index0` | ported | `infinity-executor` `replay_misc_ut::replay_append_with_index0` |
| 880 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact_commit` | ported | `infinity-executor` `compact_replay_ut::replay_compact_commit` |
| 881 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact_rollback` | ported | `infinity-executor` `compact_replay_ut::replay_compact_rollback` |
| 882 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_replay_compact_flush_gap` | ported | `infinity-executor` `compact_replay_ut::replay_compact_flush_gap` |
| 883 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact_interrupt` | ported | `infinity-executor` `compact_replay_ut::replay_compact_interrupt` |
| 884 | src/unit_test/storage/new_catalog/replay_compact_ut.cpp | `DISABLED_SLOW_test_compact_with_index` | ported | `infinity-executor` `compact_replay_ut::disabled_slow_test_compact_with_index` |
| 885 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact1_rollback` | ported | `infinity-executor` `compact_replay_ut::replay_compact1_rollback` |
| 886 | `src/unit_test/storage/new_catalog/replay_import_ut.cpp` | `TestTxnReplayImport` | `test_import0` | ported | `infinity-executor` `replay_misc_ut::replay_import0` |
| 887 | `src/unit_test/storage/new_catalog/replay_import_ut.cpp` | `TestTxnReplayImport` | `test_import_with_index` | ported | `infinity-executor` `replay_misc_ut::replay_import_with_index` |
| 888 | src/unit_test/storage/new_catalog/replay_index_ut.cpp | `DISABLED_SLOW_test_replay_append_with_index` | ported | `infinity-executor` `replay_misc_ut::disabled_slow_test_replay_append_with_index` |
| 889 | src/unit_test/storage/new_catalog/replay_index_ut.cpp | `DISABLED_SLOW_test_populate_index` | ported | `infinity-executor` `replay_misc_ut::disabled_slow_test_populate_index` |
| 890 | src/unit_test/storage/new_catalog/replay_index_ut.cpp | `drop_index` | ported | `infinity-executor` `replay_misc_ut::replay_index_drop_index` |
| 891 | `src/unit_test/storage/new_catalog/replay_index_ut.cpp` | `TestTxnReplayIndex` | `test_json_index_replay` | ported | `infinity-executor` `replay_misc_ut::replay_json_index` |
| 892 | src/unit_test/storage/new_catalog/replay_optimize_ut.cpp | `DISABLED_SLOW_test_optimize_commit` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_test_optimize_commit` |
| 893 | src/unit_test/storage/new_catalog/replay_optimize_ut.cpp | `DISABLED_SLOW_test_optimize_rollback` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_test_optimize_rollback` |
| 894 | src/unit_test/storage/new_catalog/replay_optimize_ut.cpp | `DISABLED_SLOW_test_optimize_interrupt` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_test_optimize_interrupt` |
| 895 | src/unit_test/storage/new_catalog/replay_restore_ut.cpp | `DISABLED_SLOW_test_repaly_restore_table_snapshot` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_test_repaly_restore_table_snapshot` |
| 896 | src/unit_test/storage/new_catalog/replay_restore_ut.cpp | `DISABLED_SLOW_test_repaly_restore_database_snapshot` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_test_repaly_restore_database_snapshot` |
| 897 | src/unit_test/storage/new_catalog/replay_restore_ut.cpp | `DISABLED_SLOW_test_repaly_restore_system_snapshot` | ported | `infinity-executor` `optimize_replay_ut::disabled_slow_test_repaly_restore_system_snapshot` |
| 898 | src/unit_test/storage/new_catalog/table_cache_ut.cpp | `test_table_cache` | ported | `infinity-executor` `new_catalog_meta_ut::test_table_cache` |
| 899 | src/unit_test/storage/new_catalog/table_meta_ut.cpp | `table_meta` | ported | `infinity-executor` `new_catalog_meta_ut::table_meta` |
| 900 | src/unit_test/storage/new_catalog/table_ut.cpp | `table_test1` | ported | `infinity-executor` `new_catalog_table_ut::table_test1` |
| 901 | src/unit_test/storage/new_catalog/table_ut.cpp | `table_test2` | ported | `infinity-executor` `new_catalog_table_ut::table_test2` |
| 902 | src/unit_test/storage/new_catalog/table_ut.cpp | `createtable_wrong_conflicttype` | ported | `infinity-executor` `new_catalog_table_ut::createtable_wrong_conflicttype` |
| 903 | src/unit_test/storage/new_catalog/table_ut.cpp | `droptable_wrong_conflicttype` | ported | `infinity-executor` `new_catalog_table_ut::droptable_wrong_conflicttype` |
| 904 | src/unit_test/storage/new_catalog/table_ut.cpp | `createtable_createtable` | ported | `infinity-executor` `new_catalog_table_ut::createtable_createtable` |
| 905 | src/unit_test/storage/new_catalog/table_ut.cpp | `createdb_createtable_test` | ported | `infinity-executor` `new_catalog_table_ut::createdb_createtable_test` |
| 906 | src/unit_test/storage/new_catalog/table_ut.cpp | `createtable_dropdb_test` | ported | `infinity-executor` `new_catalog_table_ut::createtable_dropdb_test` |
| 907 | src/unit_test/storage/new_catalog/table_ut.cpp | `dropdb_droptable_test` | ported | `infinity-executor` `new_catalog_table_ut::dropdb_droptable_test` |
| 908 | src/unit_test/storage/new_catalog/table_ut.cpp | `createtable_droptable_test` | ported | `infinity-executor` `new_catalog_table_ut::createtable_droptable_test` |
| 909 | src/unit_test/storage/new_catalog/table_ut.cpp | `droptable_droptable_test` | ported | `infinity-executor` `new_catalog_table_ut::droptable_droptable_test` |
| 910 | src/unit_test/storage/new_catalog/table_ut.cpp | `rename_table_test` | ported | `infinity-executor` `new_catalog_table_ut::rename_table_test` |
| 911 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update` | ported | `infinity-executor` `catalog_ut::update_replaces_visible_values` |
| 912 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_multiple_blocks` | ported | `infinity-executor` `update_import_ut::update_multiple_blocks` |
| 913 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_drop_db` | ported | `infinity-executor` `update_import_ut::update_then_drop_database` |
| 914 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_drop_table` | ported | `infinity-executor` `update_import_ut::update_then_drop_table` |
| 915 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_drop_index` | ported | `infinity-executor` `update_import_ut::update_then_drop_index` |
| 916 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_dump_index` | ported | `infinity-executor` `update_import_ut::update_then_dump_index` |
| 917 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_add_drop_column_conflicts` | ported | `infinity-executor` `update_import_ut::update_add_and_drop_column` |
| 918 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_compact_conflicts` | ported | `infinity-executor` `update_import_ut::update_and_compact_compose` |
| 919 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_create_index_conflicts` | ported | `infinity-executor` `update_import_ut::update_and_create_index_compose` |
| 920 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_delete_conflicts` | ported | `infinity-executor` `catalog_ut::update_and_delete_compose` |
| 921 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_delete_no_conflicts` | ported | `infinity-executor` `update_import_ut::update_then_delete_leaves_the_rest` |
| 922 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `create_index_append` | ported | `infinity-executor` `request_ut::conflict_create_index_append` |
| 923 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_append` | ported | `infinity-executor` `request_ut::conflict_add_column_append` |
| 924 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_import` | ported | `infinity-executor` `request_ut::conflict_add_column_import` |
| 925 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_compact` | ported | `infinity-executor` `request_ut::conflict_add_column_compact` |
| 926 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_add_column` | ported | `infinity-executor` `request_ut::conflict_add_column_add_column` |
| 927 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_append` | ported | `infinity-executor` `request_ut::conflict_drop_column_append` |
| 928 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_import` | ported | `infinity-executor` `request_ut::conflict_drop_column_import` |
| 929 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_compact` | ported | `infinity-executor` `request_ut::conflict_drop_column_compact` |
| 930 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_create_index` | exempt | 该用例所在的 `#if 0` 块（691-767 行）被关闭，无编译后可执行行为 
| 931 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `delete_append` | ported | `infinity-executor` `request_ut::conflict_delete_append` |
| 932 | `src/unit_test/storage/new_request/data_request_ut.cpp` | `TestDataRequest` | `test_append` | ported | `infinity-executor` `request_ut::append_into_missing_table_fails` |
| 933 | `src/unit_test/storage/new_request/data_request_ut.cpp` | `TestDataRequest` | `test_select` | ported | `infinity-executor` `request_ut::select_returns_inserted_row` |
| 934 | `src/unit_test/storage/new_request/data_request_ut.cpp` | `TestDataRequest` | `test_delete` | ported | `infinity-executor` `request_ut::delete_removes_matching_row` |
| 935 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_create_db` | ported | `infinity-executor` `request_ut::create_database_succeeds_once` |
| 936 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_db` | ported | `infinity-executor` `request_ut::drop_database_succeeds_once` |
| 937 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_create_table` | ported | `infinity-executor` `request_ut::create_table_succeeds_once` |
| 938 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_table` | ported | `infinity-executor` `request_ut::drop_table_succeeds_once` |
| 939 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_create_index` | ported | `infinity-executor` `request_ut::create_index_succeeds_once` |
| 940 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_index` | ported | `infinity-executor` `request_ut::drop_index_succeeds_once` |
| 941 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_add_column` | ported | `infinity-executor` `request_ut::add_column_succeeds_once` |
| 942 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_column` | ported | `infinity-executor` `request_ut::drop_column_succeeds_once` |
| 943 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_use_schema` | ported | `infinity-executor` `request_ut::use_requires_existing_database` |
| 944 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `index_scan` | ported | `infinity-executor` `request_ut::index_scan_returns_filtered_rows` |
| 945 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `fulltext_index_scan` | ported | `infinity-executor` `request_ut::fulltext_index_scan_returns_matching_row` |
| 946 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `vector_index_scan` | ported | `infinity-executor` `request_ut::vector_index_scan_matches_with_hnsw_and_ivf` |
| 947 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `sparse_index_scan` | ported | `infinity-executor` `request_ut::sparse_index_scan_matches_with_bmp` |
| 948 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `tensor_index_scan` | ported | `infinity-executor` `request_ut::tensor_index_scan_matches_with_emvb` |
| 949 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `test_optimize_index` | ported | `infinity-executor` `request_ut::optimize_index_preserves_vector_search` |
| 950 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `PersistFileBasic` | ported | `infinity-storage` `persistence::tests::persist_file_basic` |
| 951 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `PersistMultiFile` | ported | `infinity-storage` `persistence::tests::persist_multi_file` |
| 952 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `PersistFileMultiThread` | ported | `infinity-storage` `persistence::tests::persist_file_multi_thread` |
| 953 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `CleanupBasic` | ported | `infinity-storage` `persistence::tests::cleanup_basic` |
| 954 | `src/unit_test/storage/result_cache_manager_ut.cpp` | `ResultCacheManagerTest` | `test1` | ported | `infinity-storage` `result_cache::tests::cache_projects_columns_and_merges_new_ones` |
| 955 | `src/unit_test/storage/result_cache_manager_ut.cpp` | `ResultCacheManagerTest` | `test2` | ported | `infinity-storage` `result_cache::tests::cache_evicts_the_least_recently_used_entry` |
| 956 | `src/unit_test/storage/secondary_index/test_high_cardinality_ut.cpp` | `HighCardinalitySecondaryIndexTest` | `TestSaveLoadHighCardinality` | ported | `infinity-storage` `secondary_index_data::tests::save_load_high_cardinality` |
| 957 | `src/unit_test/storage/secondary_index/test_high_cardinality_ut.cpp` | `HighCardinalitySecondaryIndexTest` | `TestTxn` | ported | `infinity-executor` `txn_ut::secondary_index_high_cardinality_txn` |
| 958 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestIntegerLowCardinality` | ported | `infinity-storage` `secondary_lowcard::tests::test_integer_low_cardinality` |
| 959 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestFloatLowCardinality` | ported | `infinity-storage` `secondary_lowcard::tests::test_float_low_cardinality` |
| 960 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestEmptyIndex` | ported | `infinity-storage` `secondary_lowcard::tests::test_empty_low_cardinality_index` |
| 961 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestSingleValueCardinality` | ported | `infinity-storage` `secondary_lowcard::tests::test_single_value_low_cardinality` |
| 962 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestComparisonWithHighCardinality` | ported | `infinity-storage` `secondary_lowcard::tests::test_low_cardinality_vs_high_cardinality` |
| 963 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestSaveLoadLowCardinality` | ported | `infinity-storage` `secondary_lowcard::tests::test_save_load_low_cardinality` |
| 964 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestAllDataType` | ported | `infinity-storage` `secondary_lowcard::tests::test_all_data_types_low_cardinality` |
| 965 | `src/unit_test/storage/tracer/test_tracer_ut.cpp` | `MemIndexTracerTest` | `test1` | ported | `infinity-storage` `memindex_tracer::tests::test1` |
| 966 | `src/unit_test/storage/tracer/test_tracer_ut.cpp` | `MemIndexTracerTest` | `test2` | ported | `infinity-storage` `memindex_tracer::tests::test2` |
| 967 | `src/unit_test/storage/txn/conflict_check_ut.cpp` | `ConflictCheckTest` | `conflict_check_delete` | ported | `infinity-executor` `conflict_check_ut::conflict_check_delete` |
| 968 | `src/unit_test/storage/txn/constants_ut.cpp` | `TxnContants` | `test1` | ported | `infinity-txn` `state::tests::started_state_renders_as_started` |
| 969 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_restore_database_rollback_basic` | ported | `infinity-executor` `txn_ut::database_snapshot_restore_rollback_basic` |
| 970 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_restore_database_create_database_multithreaded` | ported | `infinity-executor` `txn_ut::database_snapshot_restore_create_database_multithreaded` |
| 971 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_create_snapshot_same_name_multithreaded` | ported | `infinity-executor` `txn_ut::database_snapshot_create_same_name_multithreaded` |
| 972 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_sql_parser` | ported | `infinity-executor` `txn_ut::database_snapshot_sql_parser` |
| 973 | `src/unit_test/storage/txn/system_snapshot_ut.cpp` | `SystemSnapshotTest` | `test_restore_system_rollback_basic` | ported | `infinity-executor` `txn_ut::system_snapshot_restore_rollback_basic` |
| 974 | `src/unit_test/storage/txn/system_snapshot_ut.cpp` | `SystemSnapshotTest` | `test_restore_system_create_database_multithreaded` | ported | `infinity-executor` `txn_ut::system_snapshot_restore_create_database_multithreaded` |
| 975 | `src/unit_test/storage/txn/system_snapshot_ut.cpp` | `SystemSnapshotTest` | `test_sql_parser` | ported | `infinity-executor` `txn_ut::system_snapshot_sql_parser` |
| 976 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_restore_table_rollback_basic` | ported | `infinity-executor` `txn_ut::table_snapshot_restore_rollback_basic` |
| 977 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_restore_table_create_table_multithreaded` | ported | `infinity-executor` `txn_ut::table_snapshot_restore_create_table_multithreaded` |
| 978 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_create_snapshot_same_name_multithreaded` | ported | `infinity-executor` `txn_ut::table_snapshot_create_same_name_multithreaded` |
| 979 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_show_snapshot_multithreaded` | ported | `infinity-executor` `txn_ut::table_snapshot_show_multithreaded` |
| 980 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_restore_table_same_snapshot_multithreaded` | ported | `infinity-executor` `txn_ut::table_snapshot_restore_same_snapshot_multithreaded` |
| 981 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_create_snapshot_delete_data` | ported | `infinity-executor` `txn_ut::table_snapshot_create_snapshot_delete_data` |
| 982 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_create_snapshot_insert_data` | ported | `infinity-executor` `txn_ut::table_snapshot_create_snapshot_insert_data` |
| 983 | src/unit_test/storage/wal/recycle_log_ut.cpp | `recycle_wal_after_delta_checkpoint` | ported | `infinity-storage` `wal_replay_ut::recycle_wal_after_delta_checkpoint` |
| 984 | `src/unit_test/storage/wal/repeat_replay_ut.cpp` | `RepeatReplayTest` | `append` | ported | `infinity-executor` `wal_replay_ut::repeat_replay_append` |
| 985 | `src/unit_test/storage/wal/repeat_replay_ut.cpp` | `RepeatReplayTest` | `import` | ported | `infinity-executor` `wal_replay_ut::repeat_replay_import` |
| 986 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `ReadWriteV2` | ported | `infinity-storage` `wal_entry::tests::read_write_v2_roundtrips_every_command` |
| 987 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `WalEntryIterator` | ported | `infinity-storage` `wal_entry::tests::wal_entry_iterator_replay_stops_at_checkpoint` |
| 988 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `WalListIterator` | ported | `infinity-storage` `wal_entry::tests::wal_list_iterator_replay_stops_at_checkpoint` |
| 989 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedEntryBeforeCheckpointIsIgnored` | ported | `infinity-storage` `wal_entry::tests::damaged_entry_before_checkpoint_is_ignored` |
| 990 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedEntryTruncatesWalFile` | ported | `infinity-storage` `wal_entry::tests::damaged_entry_truncates_wal_file` |
| 991 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedFirstEntryDropsWalFile` | ported | `infinity-storage` `wal_entry::tests::damaged_first_entry_drops_wal_file` |
| 992 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedTailAfterCheckpointIsTruncated` | ported | `infinity-storage` `wal_entry::tests::damaged_tail_after_checkpoint_is_truncated` |
| 993 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedOlderFileDropsNewerOnes` | ported | `infinity-storage` `wal_entry::tests::damaged_older_file_drops_newer_ones` |
| 994 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `NoCheckpointStillPurgesDamage` | ported | `infinity-storage` `wal_entry::tests::no_checkpoint_still_purges_damage` |
| 995 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `TornTailStopsIteration` | ported | `infinity-storage` `wal_entry::tests::torn_tail_stops_iteration` |
| 996 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `CorruptedTrailingSizeIsRecoveredByForwardScan` | ported | `infinity-storage` `wal_entry::tests::corrupted_trailing_size_is_recovered_by_forward_scan` |
| 997 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `EmptyWalFileIsNotAnError` | ported | `infinity-storage` `wal_entry::tests::empty_wal_file_is_not_an_error` |
| 998 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `FileShorterThanEntrySize` | ported | `infinity-storage` `wal_entry::tests::file_shorter_than_entry_size_is_empty` |
| 999 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `AllFilesPurgedLeavesEmptyList` | ported | `infinity-storage` `wal_entry::tests::all_files_purged_leaves_empty_list` |
| 1000 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamageAccumulatesToCheckpointFile` | ported | `infinity-storage` `wal_entry::tests::damage_accumulates_to_checkpoint_file` |
| 1001 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_database` | ported | `infinity-storage` `wal_replay_ut::wal_replay_database` |
| 1002 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_tables` | ported | `infinity-storage` `wal_replay_ut::wal_replay_tables` |
| 1003 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_append` | ported | `infinity-storage` `wal_replay_ut::wal_replay_append` |
| 1004 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_import` | ported | `infinity-storage` `wal_replay_ut::wal_replay_import` |
| 1005 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_compact` | ported | `infinity-storage` `wal_replay_ut::wal_replay_compact` |
| 1006 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_create_index_IvfFlat` | ported | `infinity-storage` `wal_replay_ut::wal_replay_create_index_ivf_flat` |
| 1007 | src/unit_test/storage/wal/wal_replay_ut.cpp | `wal_replay_create_index_hnsw` | ported | `infinity-storage` `wal_replay_ut::wal_replay_create_index_hnsw` |
| 1008 | `src/unit_test/third_party/json_ut.cpp` | `JsonTest` | `nlohmann_test` | ported | `infinity-common` `json::tests::nlohmann_test_parses_twitter_metadata` |
| 1009 | `src/unit_test/third_party/json_ut.cpp` | `JsonTest` | `simdjson_test` | ported | `infinity-common` `json::tests::simdjson_test_walks_documents_by_type` |
| 1010 | `src/unit_test/third_party/test_croaring_ut.cpp` | `CRoaringTest` | `test_size` | ported | `infinity-common` `bitmap::third_party_reference_suite::roaring_size_and_optimize` |
| 1011 | `src/unit_test/third_party/test_croaring_ut.cpp` | `CRoaringTest` | `test1` | ported | `infinity-common` `bitmap::third_party_reference_suite::roaring_cardinality_32_and_64` |
| 1012 | `src/unit_test/third_party/test_croaring_ut.cpp` | `CRoaringTest` | `test2` | ported | `infinity-common` `bitmap::third_party_reference_suite::roaring_bitmap_operations` |
