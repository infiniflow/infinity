# C++ 单元测试覆盖审计

`src/unit_test/**/*.cpp` 中 `TEST/TEST_F/TEST_P` 共 **1012** 个，状态分布（逐行重算）：`excluded` 47, `exempt` 108, `gap` 99, `gate-a` 42, `integration` 247, `partial` 35, `ported` 434.

- `ported`：Rust 已逐条移植同等场景。
- `partial`：Rust 类已存在、部分场景已覆盖，剩余用例待补（如 `column_vector/*` 的完整 `ColumnVector` API、HNSW 内部类）。
- `gate-a`：parser/planner/executor/function，当前由 Gate A 的 SQL 用例覆盖，单元断言待移植。
- `integration`：catalog/txn/wal/io/bg_task，当前由 Gate C 的 restart/快照集成用例覆盖，单元断言待移植。
- `excluded`：DiskANN（计划 §2.6/R14 排除）以及 geo 类型（`Point/Line/Box/Path/Polygon/Circle`）相关用例——用户决定不移植这些类型。
- `exempt`：SIMD/第三方夹具，以及 `PostingByteSlice`/`Skiplist`/PForDelta 等已登记为设计变更的压缩/字节切片原语。
- `gap`：尚未移植。
- 带 `#[ignore]` 的 `ported` 行：C++ 的 `DISABLED_*` 用例，Rust 用 `#[ignore]` 表达，需 `cargo test -- --ignored` 显式运行。

| # | 文件 | Fixture | 用例 | 状态 |
|---:|---|---|---|---|
| 1 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_default_is_english_and_coarse` | gap |
| 2 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_fine_keeps_english_stemming` | gap |
| 3 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_slovak_folds_and_disables_stemming` | gap |
| 4 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_czech_folds_and_disables_stemming` | gap |
| 5 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_language_and_fine_in_both_orders` | gap |
| 6 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_unknown_segment_falls_back_to_legacy` | gap |
| 7 | `src/unit_test/common/analyzer/analyzer_pool_ut.cpp` | `AnalyzerPoolTest` | `test_rag_analyzers_are_independent` | gap |
| 8 | `src/unit_test/common/analyzer/chinese_analyzer_ut.cpp` | `ChineseAnalyzerTest` | `test1` | gap |
| 9 | `src/unit_test/common/analyzer/ik_analyzer_ut.cpp` | `IKAnalyzerTest` | `test1` | gap |
| 10 | `src/unit_test/common/analyzer/ngram_analyzer_ut.cpp` | `NGramAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::tests::ngram_two_slides_over_words` |
| 11 | `src/unit_test/common/analyzer/ngram_analyzer_ut.cpp` | `NGramAnalyzerTest` | `test2` | ported | `analyzer::tests::ngram_one_emits_each_character` |
| 12 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_analyze_enable_position` | gap |
| 13 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_analyze_enable_position_fine_grained` | gap |
| 14 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_tokenize_consistency_with_posiiton` | gap |
| 15 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_tokenize_consistency_with_python` | gap |
| 16 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_fine_grained_tokenize_consistency_with_python` | gap |
| 17 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_set_language_dutch` | gap |
| 18 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_set_language_slovak` | gap |
| 19 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_set_language_czech` | gap |
| 20 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_slovak_no_stemming` | gap |
| 21 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_diacritics_folding_table` | gap |
| 22 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_out_of_range_diacritics_parity_with_python` | gap |
| 23 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_slovak_tokenize_with_position` | gap |
| 24 | `src/unit_test/common/analyzer/rag_analyzer_ut.cpp` | `RAGAnalyzerTest` | `test_chinese_stopword_filter` | gap |
| 25 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `unicode_replacement_preserves_byte_positions` | gap |
| 26 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `empty_and_unmatched_input` | gap |
| 27 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `invalid_utf8_is_checked_before_any_replacement` | gap |
| 28 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `non_character_offsets_remain_checked` | gap |
| 29 | `src/unit_test/common/analyzer/rag_regex_ut.cpp` | `RAGRegexTest` | `large_html_replacement` | gap |
| 30 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test1` | ported | `infinity-executor` `analyzer::tests::*` |
| 31 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test2` | ported | `infinity-executor` `analyzer::tests::*` |
| 32 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test3` | ported | `infinity-executor` `analyzer::tests::*` |
| 33 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test4` | ported | `infinity-executor` `analyzer::tests::*` |
| 34 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test5` | ported | `infinity-executor` `analyzer::tests::*` |
| 35 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test6` | ported | `infinity-executor` `analyzer::tests::*` |
| 36 | `src/unit_test/common/analyzer/standard_analyzer_ut.cpp` | `StandardAnalyzerTest` | `test7` | ported | `infinity-executor` `analyzer::tests::*` |
| 37 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test1` | exempt |
| 38 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test2` | exempt |
| 39 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test3` | exempt |
| 40 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test4` | exempt |
| 41 | `src/unit_test/common/analyzer/user_defined_analyzer_ut_nimpl.cpp` | `UserDefinedAnalyzerTest` | `test5` | exempt |
| 42 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test1` | exempt |
| 43 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test2` | exempt |
| 44 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test3` | exempt |
| 45 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test4` | exempt |
| 46 | `src/unit_test/common/analyzer/user_defined_term_weight_nimpl.cpp` | `UserDefinedTermWeightTest` | `test5` | exempt |
| 47 | `src/unit_test/common/test_simd_ut.cpp` | `SimdInitTest` | `GetSupportedSimdTypesList` | exempt |
| 48 | `src/unit_test/common/utility/exception_ut.cpp` | `ExceptionTest` | `all_exception_cases` | ported | `infinity-common` `error::tests::exception_messages_match_the_reference` |
| 49 | `src/unit_test/common/utility/trim_ut.cpp` | `TrimTest` | `test1` | ported | `infinity-common` `utility::tests::trim_path_keeps_the_repository_relative_part` |
| 50 | `src/unit_test/common/utility/utility_ut.cpp` | `UtilityTest` | `test1` | ported | `utility::tests::extract_u64_reads_the_key_suffix` |
| 51 | `src/unit_test/executor/expression/expression_executor_select_ut.cpp` | `ExpressionExecutorSelectTest` | `test1` | ported | `infinity-storage` `data_block::reference_suite::expression_select_alternating_input` |
| 52 | `src/unit_test/executor/expression/expression_executor_select_ut.cpp` | `ExpressionExecutorSelectTest` | `test2` | ported | `infinity-storage` `data_block::reference_suite::expression_select_all_true_input` |
| 53 | `src/unit_test/executor/expression/expression_executor_select_ut.cpp` | `ExpressionExecutorSelectTest` | `test3` | ported | `infinity-storage` `data_block::reference_suite::expression_select_all_false_input` |
| 54 | `src/unit_test/executor/expression/new_expression_evaluator_ut.cpp` | `ExpressionEvaluatorTest` | `add_bigint_constant_1` | ported | `infinity-expression` `expression_evaluator_add_bigint_constant_1` |
| 55 | `src/unit_test/executor/expression/new_expression_evaluator_ut.cpp` | `ExpressionEvaluatorTest` | `subtract_constant_8192_bigint` | ported | `infinity-expression` `expression_evaluator_subtract_constant_8192_bigint` |
| 56 | `src/unit_test/executor/fragment/plan_fragment_ut.cpp` | `PlanFragmentTest` | `test1` | gate-a |
| 57 | `src/unit_test/executor/operator/physical_sort_ut_nimpl.cpp` | `PhysicalSortTest` | `test1` | gate-a |
| 58 | `src/unit_test/executor/pipeline/explain_ut.cpp` | `ExplainTest` | `test1` | gate-a |
| 59 | `src/unit_test/executor/pipeline/fragment_ut.cpp` | `FragmentTest` | `test_build_fragment` | gate-a |
| 60 | `src/unit_test/function/aggregate/avg_functions_ut.cpp` | `AvgFunctionTest` | `avg_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 61 | `src/unit_test/function/aggregate/count_functions_ut.cpp` | `CountFunctionTest` | `quarter_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 62 | `src/unit_test/function/aggregate/first_functions_ut.cpp` | `FirstFunctionTest` | `first_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 63 | `src/unit_test/function/aggregate/max_functions_ut.cpp` | `MaxFunctionTest` | `max_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 64 | `src/unit_test/function/aggregate/min_functions_ut.cpp` | `MinFunctionTest` | `min_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 65 | `src/unit_test/function/aggregate/sum_functions_ut.cpp` | `SumFunctionTest` | `sum_func` | ported | `infinity-executor` `tests::aggregate_functions_match_the_reference` |
| 66 | `src/unit_test/function/cast/bitmap_cast_ut_nimpl.cpp` | `BitmapCastTest` | `bitmap_cast0` | gate-a |
| 67 | `src/unit_test/function/cast/bitmap_cast_ut_nimpl.cpp` | `BitmapCastTest` | `bitmap_cast1` | gate-a |
| 68 | `src/unit_test/function/cast/bool_cast_ut.cpp` | `BoolCastTest` | `bool_cast0` | ported | `infinity-common` `bool_cast0` |
| 69 | `src/unit_test/function/cast/bool_cast_ut.cpp` | `BoolCastTest` | `bool_cast1` | ported | `infinity-common` `bool_cast1` |
| 70 | `src/unit_test/function/cast/cast_table_ut.cpp` | `CastTableTest` | `casttable_boolean` | ported | `infinity-common` `casttable_boolean` |
| 71 | `src/unit_test/function/cast/datetime/date_cast_ut.cpp` | `DateCastTest` | `date_cast0` | ported | `infinity-common` `date_cast0` |
| 72 | `src/unit_test/function/cast/datetime/date_cast_ut.cpp` | `DateCastTest` | `date_cast1` | ported | `infinity-common` `date_cast1` |
| 73 | `src/unit_test/function/cast/datetime/datetime_cast_ut.cpp` | `DateTimeCastTest` | `datetime_cast0` | ported | `infinity-common` `datetime_cast0` |
| 74 | `src/unit_test/function/cast/datetime/datetime_cast_ut.cpp` | `DateTimeCastTest` | `datetime_cast1` | ported | `infinity-common` `datetime_cast1` |
| 75 | `src/unit_test/function/cast/datetime/interval_cast_ut.cpp` | `IntervalCastTest` | `date_cast0` | ported | `infinity-common` `interval_cast0` |
| 76 | `src/unit_test/function/cast/datetime/interval_cast_ut.cpp` | `IntervalCastTest` | `date_cast1` | ported | `infinity-common` `interval_cast1` |
| 77 | `src/unit_test/function/cast/datetime/time_cast_ut.cpp` | `TimeCastTest` | `date_cast0` | ported | `infinity-common` `time_cast0` |
| 78 | `src/unit_test/function/cast/datetime/time_cast_ut.cpp` | `TimeCastTest` | `date_cast1` | ported | `infinity-common` `time_cast1` |
| 79 | `src/unit_test/function/cast/datetime/timestamp_cast_ut.cpp` | `TimestampCastTest` | `datetime_cast0` | ported | `infinity-common` `timestamp_cast0` |
| 80 | `src/unit_test/function/cast/datetime/timestamp_cast_ut.cpp` | `TimestampCastTest` | `datetime_cast1` | ported | `infinity-common` `timestamp_cast1` |
| 81 | `src/unit_test/function/cast/decimal/decimal_cast_ut.cpp` | `DecimalCastTest` | `decimal_cast0` | ported | `infinity-common` `decimal_cast0` |
| 82 | `src/unit_test/function/cast/decimal/decimal_cast_ut.cpp` | `DecimalCastTest` | `decimal_cast1` | ported | `infinity-common` `decimal_cast1` |
| 83 | `src/unit_test/function/cast/embedding_cast_ut.cpp` | `EmbeddingCastTest` | `embedding_cast1` | ported | `infinity-common` `embedding_cast1` |
| 84 | `src/unit_test/function/cast/float/double_cast_ut.cpp` | `DoubleCastTest` | `double_cast0` | ported | `infinity-common` `double_cast0` |
| 85 | `src/unit_test/function/cast/float/double_cast_ut.cpp` | `DoubleCastTest` | `double_cast1` | ported | `infinity-common` `double_cast1` |
| 86 | `src/unit_test/function/cast/float/float_cast_ut.cpp` | `FloatCastTest` | `float_cast0` | ported | `infinity-common` `float_cast0` |
| 87 | `src/unit_test/function/cast/float/float_cast_ut.cpp` | `FloatCastTest` | `float_cast1` | ported | `infinity-common` `float_cast1` |
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
| 102 | `src/unit_test/function/cast/integer_cast/bigint_cast_ut.cpp` | `BigIntCastTest` | `bigint_cast0` | ported | `infinity-common` `bigint_cast0` |
| 103 | `src/unit_test/function/cast/integer_cast/bigint_cast_ut.cpp` | `BigIntCastTest` | `bigint_cast1` | ported | `infinity-common` `bigint_cast1` |
| 104 | `src/unit_test/function/cast/integer_cast/hugeint_cast_ut.cpp` | `HugeIntCastTest` | `hugeint_cast0` | ported | `infinity-common` `hugeint_cast0` |
| 105 | `src/unit_test/function/cast/integer_cast/hugeint_cast_ut.cpp` | `HugeIntCastTest` | `hugeint_cast1` | ported | `infinity-common` `hugeint_cast1` |
| 106 | `src/unit_test/function/cast/integer_cast/integer_cast_ut.cpp` | `IntegerCastTest` | `integer_cast0` | ported | `infinity-common` `integer_cast0` |
| 107 | `src/unit_test/function/cast/integer_cast/integer_cast_ut.cpp` | `IntegerCastTest` | `integer_cast1` | ported | `infinity-common` `integer_cast1` |
| 108 | `src/unit_test/function/cast/integer_cast/small_integer_cast_ut.cpp` | `SmallIntegerCastTest` | `small_integer_cast0` | ported | `infinity-common` `small_integer_cast0` |
| 109 | `src/unit_test/function/cast/integer_cast/small_integer_cast_ut.cpp` | `SmallIntegerCastTest` | `small_integer_cast1` | ported | `infinity-common` `small_integer_cast1` |
| 110 | `src/unit_test/function/cast/integer_cast/tiny_integer_cast_ut.cpp` | `TinyIntegerCastTest` | `tiny_integer_cast0` | ported | `infinity-common` `tiny_integer_cast0` |
| 111 | `src/unit_test/function/cast/integer_cast/tiny_integer_cast_ut.cpp` | `TinyIntegerCastTest` | `tiny_integer_cast1` | ported | `infinity-common` `tiny_integer_cast1` |
| 112 | `src/unit_test/function/cast/uuid_cast_ut_nimpl.cpp` | `UuidCastTest` | `uuid_cast0` | gate-a |
| 113 | `src/unit_test/function/cast/uuid_cast_ut_nimpl.cpp` | `UuidCastTest` | `uuid_cast1` | gate-a |
| 114 | `src/unit_test/function/cast/varchar_cast_ut.cpp` | `VarcharCastTest` | `varchar_cast0` | ported | `infinity-common` `varchar_cast0` |
| 115 | `src/unit_test/function/cast/varchar_cast_ut.cpp` | `VarcharCastTest` | `varchar_cast1` | ported | `infinity-common` `varchar_cast1` |
| 116 | `src/unit_test/function/scalar/abs_functions_ut.cpp` | `AbsFunctionsTest` | `abs_func` | ported | `infinity-executor` `tests::abs_function_covers_every_numeric_type` |
| 117 | `src/unit_test/function/scalar/add_functions_ut.cpp` | `AddFunctionsTest` | `add_func` | partial | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` 覆盖常规值；C++ 的按列宽 checked 溢出→NULL 未建模（i64 求值） |
| 118 | `src/unit_test/function/scalar/and_functions_ut.cpp` | `AndFunctionsTest` | `and_func` | ported | `infinity-executor` `tests::arithmetic_and_boolean_operators_match_the_reference` |
| 119 | `src/unit_test/function/scalar/century_functions_ut.cpp` | `CenturyFunctionsTest` | `century_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 120 | `src/unit_test/function/scalar/current_date_functions_ut.cpp` | `CurrentDateFunctionsTest` | `current_date_func` | gate-a |
| 121 | `src/unit_test/function/scalar/current_time_functions_ut.cpp` | `CurrentTimeFunctionsTest` | `current_time_func` | gate-a |
| 122 | `src/unit_test/function/scalar/current_timestamp_functions_ut.cpp` | `CurrentTimestampFunctionsTest` | `current_timestamp_func` | gate-a |
| 123 | `src/unit_test/function/scalar/date_part_functions_ut.cpp` | `DatePartFunctionsTest` | `datepart_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 124 | `src/unit_test/function/scalar/day_functions_ut.cpp` | `DayFunctionsTest` | `day_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 125 | `src/unit_test/function/scalar/day_of_month_functions_ut.cpp` | `DayOfMonthFunctionsTest` | `day_of_month_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 126 | `src/unit_test/function/scalar/day_of_week_ut.cpp` | `DayOfWeekFunctionsTest` | `day_of_week_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 127 | `src/unit_test/function/scalar/day_of_year_functions_ut.cpp` | `DayOfYearFunctionsTest` | `day_of_year_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 128 | `src/unit_test/function/scalar/div_functions_ut.cpp` | `DivFunctionsTest` | `div_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 129 | `src/unit_test/function/scalar/epoch_functions_ut.cpp` | `EpochFunctionsTest` | `epoch_func` | gate-a |
| 130 | `src/unit_test/function/scalar/equals_functions_ut.cpp` | `EqualsFunctionsTest` | `equals_func` | ported | `infinity-executor` `tests::arithmetic_and_equality_follow_the_reference` |
| 131 | `src/unit_test/function/scalar/era_functions_ut.cpp` | `EraFunctionsTest` | `era_func` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 132 | `src/unit_test/function/scalar/extract_function_ut.cpp` | `ExtractFunctionTest` | `extract_year_test` | ported | `infinity-executor` `tests::date_component_functions_match_the_reference` |
| 133 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `config_test` | ported |
| 134 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `tensor_test` | ported |
| 135 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `random_generator_test` | ported |
| 136 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `hash_partitioner_test` | ported |
| 137 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `sparse_projector_test` | ported |
| 138 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `internal_functions_test` | ported |
| 139 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `simple_encoding_test` | ported |
| 140 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `invalid_input_test` | ported |
| 141 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `document_encoding_test` | ported |
| 142 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `simhash_partitioning_test` | ported |
| 143 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `ams_projection_test` | ported |
| 144 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `final_projection_test` | ported |
| 145 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `multiple_repetitions_test` | ported |
| 146 | `src/unit_test/function/scalar/fixed_dimensional_encoding_ut.cpp` | `FixedDimensionalEncodingTest` | `fill_empty_partitions_test` | ported |
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
| 182 | `src/unit_test/infinity_cloud/upload_meta_to_s3_ut.cpp` | `UploadMetaToS3` | `DISABLED_MINIO_test1` | gap |
| 183 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `test1` | ported | `infinity-cli` `config::tests::defaults` |
| 184 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `test2` | ported | `infinity-cli` `config::tests::full_config_file` |
| 185 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestWrongParamNames` | ported | `infinity-cli` `config::tests::wrong_parameter_names` |
| 186 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestConfInvalidValues` | ported | `infinity-cli` `config::tests::invalid_values` |
| 187 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestOutofRangeValues` | ported | `infinity-cli` `config::tests::out_of_range_values` |
| 188 | `src/unit_test/main/config_ut.cpp` | `ConfigTest` | `TestValidValues` | ported | `infinity-cli` `config::tests::valid_values` |
| 189 | `src/unit_test/main/infinity_ut.cpp` | `InfinityTest` | `test1` | ported | `infinity-executor` `tests::catalog_lifecycle_matches_the_reference_test` |
| 190 | `src/unit_test/main/infinity_ut.cpp` | `InfinityTest` | `test2` | ported | `infinity-executor` `tests::show_variable_covers_the_reference_names` + `variables::tests` |
| 191 | `src/unit_test/main/query_profiler_ut.cpp` | `QueryProfilerTest` | `test1` | ported | `infinity-executor` `profiler::tests::query_phase_names_match_the_reference` |
| 192 | `src/unit_test/main/query_profiler_ut.cpp` | `QueryProfilerTest` | `test2` | ported | `profiler::tests::disabled_profiler_ignores_phase_transitions` + `enabled_profiler_enforces_phase_order` |
| 193 | `src/unit_test/main/query_profiler_ut.cpp` | `QueryProfilerTest` | `test3` | ported | `profiler::tests::all_phases_and_optimizer_rules_record_time` |
| 194 | `src/unit_test/main/stats/global_resource_usage_ut.cpp` | `GlobalResourceUsageTest` | `usage_test` | exempt | 用例主体在 `#ifdef INFINITY_STATS` 内，构建系统从未定义该宏 → C++ 侧编译为空 |
| 195 | `src/unit_test/main/table_ut.cpp` | `InfinityTableTest` | `test1` | ported | `infinity-executor` `tests::explain_covers_every_plan_kind`（6 种 EXPLAIN） |
| 196 | `src/unit_test/parallel/chaos_ut.cpp` | `ParallelTest` | `DISABLED_SLOW_ChaosTest` | ported | `infinity-executor` `tests::disabled_slow_chaos_test`（`#[ignore]`，`INFINITY_CHAOS_SECONDS` 可缩短） |
| 197 | `src/unit_test/parser/float_serialize_ut.cpp` | `FloatSerializeTest` | `test_precision` | ported | `infinity-parser` `float_serialize_test_precision` |
| 198 | `src/unit_test/parser/float_serialize_ut.cpp` | `FloatSerializeTest` | `test_json` | ported | `infinity-expression` `float_serialize_test_json` |
| 199 | `src/unit_test/parser/float_serialize_ut.cpp` | `FloatSerializeTest` | `test_json_hex_float` | ported | `infinity-expression` `float_serialize_test_json_hex_float` |
| 200 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `good_test1` | gate-a |
| 201 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `good_test2` | gate-a |
| 202 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `operator_option_test` | gate-a |
| 203 | `src/unit_test/parser/search_driver_ut.cpp` | `SearchDriverTest` | `whitespace_analyzer_test` | gate-a |
| 204 | `src/unit_test/parser/sql_file_parsing_test_ut.cpp` | `SQLFileParsingTest` | `tpch` | gate-a |
| 205 | `src/unit_test/parser/sql_file_parsing_test_ut.cpp` | `SQLFileParsingTest` | `hyrise` | gate-a |
| 206 | `src/unit_test/parser/sql_file_parsing_test_ut.cpp` | `SQLFileParsingTest` | `infinity` | gate-a |
| 207 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `good_test1` | ported | `infinity-parser` `parser::tests::sql_parser_good_test1` |
| 208 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `good_test2` | gate-a |
| 209 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `bad_test1` | gate-a |
| 210 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `good_create_index_1` | ported | `infinity-parser` `sql_parser_good_create_index_1` |
| 211 | `src/unit_test/parser/sql_parser_ut.cpp` | `SQLParserTest` | `bad_create_index_1` | ported | `infinity-parser` `sql_parser_bad_create_index_1` |
| 212 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test1` | ported | `infinity-parser` `select_statement_parsing_good_test1` |
| 213 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test2` | ported | `infinity-parser` `select_statement_parsing_good_test2` |
| 214 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test3` | gate-a |
| 215 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_test4` | ported | `infinity-parser` `parser::tests::cast_date_minus_interval_parses` |
| 216 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `bad_test1` | ported | `infinity-parser` `parser::tests::offset_without_limit_is_rejected` |
| 217 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `bad_knn_test` | ported | `infinity-parser` `parser::tests::vector_metric_type_mismatches_are_rejected` |
| 218 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `bad_search_test` | ported | `infinity-parser` `parser::tests::search_with_order_or_limit_is_rejected` |
| 219 | `src/unit_test/parser/sql_select_statement_ut.cpp` | `SelectStatementParsingTest` | `good_search_test` | ported | `infinity-parser` `select_statement_parsing_good_search_test` |
| 220 | `src/unit_test/parser/sql_statement_ut.cpp` | `StatementParsingTest` | `good_test1` | ported | `infinity-parser` `statement_parsing_good_test1` |
| 221 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap1` | gate-a |
| 222 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap2` | gate-a |
| 223 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap33` | gate-a |
| 224 | `src/unit_test/parser/type/complex/bitmap_type_ut_nimpl.cpp` | `BitmapTypeTest` | `bitmap34` | gate-a |
| 225 | `src/unit_test/parser/type/complex/blob_type_ut_nimpl.cpp` | `BlobTypeTest` | `blob1` | gate-a |
| 226 | `src/unit_test/parser/type/complex/uuid_type_ut.cpp` | `UuidTypeTest` | `Uuid` | ported | `infinity-common` `uuid` |
| 227 | `src/unit_test/parser/type/complex/varchar_ut.cpp` | `VarcharTest` | `structure_check` | gate-a |
| 228 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `GetTypeName` | ported | `infinity-common` `data_type_get_type_name` |
| 229 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `TypeToString` | ported | `infinity-common` `data_type_type_to_string` |
| 230 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `Serialize` | gate-a |
| 231 | `src/unit_test/parser/type/data_type_ut.cpp` | `DataTypeTest` | `ReadWrite` | gate-a |
| 232 | `src/unit_test/parser/type/datetime/date_type_old_ut.cpp` | `DateTypeOldTest` | `test1` | gate-a |
| 233 | `src/unit_test/parser/type/datetime/date_type_old_ut.cpp` | `DateTypeOldTest` | `TestEqStdChronoForward` | gate-a |
| 234 | `src/unit_test/parser/type/datetime/date_type_old_ut.cpp` | `DateTypeOldTest` | `TestEqStdChronoBackward` | gate-a |
| 235 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestSameSize` | gate-a |
| 236 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestFromString` | ported | `infinity-common` `test_from_string` |
| 237 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestAddSubstract` | ported | `infinity-common` `test_add_subtract` |
| 238 | `src/unit_test/parser/type/datetime/date_type_ut.cpp` | `DateTypeTest` | `TestNegativeYears` | ported | `infinity-common` `test_negative_years` |
| 239 | `src/unit_test/parser/type/fp16_ut.cpp` | `FP16TypeTest` | `test1` | gate-a |
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
| 257 | `src/unit_test/parser/type/info/decimal_info_ut.cpp` | `DecimalInfoTest` | `decimal_info_A` | ported | `infinity-common` `decimal_info_a` |
| 258 | `src/unit_test/parser/type/info/embedding_info_ut.cpp` | `EmbeddingInfoTest` | `embedding_info_A` | ported | `infinity-common` `embedding_info_a` |
| 259 | `src/unit_test/parser/type/null_value_ut.cpp` | `NullValueTest` | `null_value_test1` | ported | `infinity-common` `null_value_test1` |
| 260 | `src/unit_test/planner/column_binding_ut.cpp` | `ColumnBindingTest` | `test1` | gate-a |
| 261 | `src/unit_test/planner/explain_basic_ut.cpp` | `ExplainBasicTest` | `test1` | gate-a |
| 262 | `src/unit_test/planner/logical_plan_tpch_ut.cpp` | `LogicalPlannerTpchTest` | `test1` | gate-a |
| 263 | `src/unit_test/planner/logical_planner_ut_nimpl.cpp` | `LogicalPlannerTest` | `test1` | gate-a |
| 264 | `src/unit_test/planner/node/logical_aggregate_ut.cpp` | `LogicalAggregateTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 265 | `src/unit_test/planner/node/logical_check_ut.cpp` | `LogicalCheckTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 266 | `src/unit_test/planner/node/logical_command_ut.cpp` | `LogicalCommandTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 267 | `src/unit_test/planner/node/logical_compact_ut.cpp` | `LogicalCompactTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 268 | `src/unit_test/planner/node/logical_create_collection_ut.cpp` | `LogicalCreateCollectionTest` | `test1` | gate-a |
| 269 | `src/unit_test/planner/node/logical_create_index_ut.cpp` | `LogicalCreateIndexTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 270 | `src/unit_test/planner/node/logical_create_table_ut.cpp` | `LogicalCreateTableTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 271 | `src/unit_test/planner/node/logical_drop_collection_ut.cpp` | `LogicalDropCollectionTest` | `test1` | gate-a |
| 272 | `src/unit_test/planner/node/logical_drop_index_ut.cpp` | `LogicalDropIndexTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 273 | `src/unit_test/planner/node/logical_drop_table_ut.cpp` | `LogicalDropTableTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 274 | `src/unit_test/planner/node/logical_flush_ut.cpp` | `LogicalFlushTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 275 | `src/unit_test/planner/node/logical_insert_ut.cpp` | `LogicalInsertTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 276 | `src/unit_test/planner/node/logical_limit_ut.cpp` | `LogicalLimitTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 277 | `src/unit_test/planner/node/logical_match_scan_base_ut.cpp` | `LogicalMatchScanTest` | `test1` | gate-a |
| 278 | `src/unit_test/planner/node/logical_optimize_ut.cpp` | `LogicalOptimzieTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 279 | `src/unit_test/planner/node/logical_project_ut.cpp` | `LogicalProjectTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 280 | `src/unit_test/planner/node/logical_show_ut.cpp` | `LogicalShowTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 281 | `src/unit_test/planner/node/logical_update_ut.cpp` | `LogicalUpdateTest` | `test1` | ported | `infinity-executor` `tests::logical_planner_statements_succeed` |
| 282 | `src/unit_test/planner/optimizer/regex_literal_extractor_ut.cpp` | `RegexLiteralExtractorTest` | `top_level_alternation_requires_nothing` | ported |
| 283 | `src/unit_test/planner/optimizer/regex_literal_extractor_ut.cpp` | `RegexLiteralExtractorTest` | `scoped_case_insensitive_group_keeps_its_mode` | ported |
| 284 | `src/unit_test/planner/optimizer/regex_literal_extractor_ut.cpp` | `RegexLiteralExtractorTest` | `re2_escapes` | ported |
| 285 | `src/unit_test/storage/bg_task/background_process_ut.cpp` | `BGProcessTest` | `test1` | integration |
| 286 | `src/unit_test/storage/bg_task/cleanup_task_ut.cpp` | `CleanupTaskTest` | `test_delete_db_simple` | integration |
| 287 | `src/unit_test/storage/bg_task/compact_segments_task_ut.cpp` | `CompactTaskTest` | `bg_compact` | integration |
| 288 | `src/unit_test/storage/bg_task/dump_index_task_2_ut.cpp` | `DumpMemIndexTaskTest2` | `DISABLED_SLOW_row_cnt_exceed_memory_quota` | integration |
| 289 | `src/unit_test/storage/bg_task/dump_index_task_ut.cpp` | `DumpMemIndexTaskTest` | `DISABLED_SLOW_row_cnt_exceed_memindex_capacity` | integration |
| 290 | `src/unit_test/storage/binary_fuse_filter/binary_fuse_filter_test_ut.cpp` | `BinaryFuseFilterTest` | `test_2000` | gap |
| 291 | `src/unit_test/storage/buffer/buffer_handle_ut.cpp` | `BufferHandleTest` | `test1` | ported |
| 292 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerTest` | `cleanup_test` | ported |
| 293 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerTest` | `varfile_test` | ported |
| 294 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerParallelTest` | `parallel_test1` | ported |
| 295 | `src/unit_test/storage/buffer/buffer_manager_ut.cpp` | `BufferManagerParallelTest` | `parallel_test2` | ported |
| 296 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `test1` | partial |
| 297 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `test_hnsw_index_buffer_obj_shutdown` | partial |
| 298 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `test_big_with_gc_and_cleanup` | partial |
| 299 | `src/unit_test/storage/buffer/buffer_obj_ut.cpp` | `BufferObjTest` | `DISABLED_SLOW_test_multiple_threads_read` | ported | `infinity-storage` `buffer::tests::disabled_slow_test_multiple_threads_read`（`#[ignore]`） |
| 300 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `flat_boolean` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 301 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `contant_bool` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 302 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `bool_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 303 | `src/unit_test/storage/column_vector/column_vector_bool_ut.cpp` | `ColumnVectorBoolTest` | `bool_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::bool_flat_constant_select_slice` |
| 304 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_date` | ported |
| 305 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_date` | ported |
| 306 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `date_column_vector_select` | ported |
| 307 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `date_column_slice_init` | ported |
| 308 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_time` | ported |
| 309 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_time` | ported |
| 310 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `time_column_vector_select` | ported |
| 311 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `time_column_slice_init` | ported |
| 312 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_datetime` | ported |
| 313 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_datetime` | ported |
| 314 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `datetime_column_vector_select` | ported |
| 315 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `datetime_column_slice_init` | ported |
| 316 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_timestamp` | ported |
| 317 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_timestamp` | ported |
| 318 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `timestamp_column_vector_select` | ported |
| 319 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `timestamp_column_slice_init` | ported |
| 320 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `flat_interval` | ported |
| 321 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `contant_flat` | ported |
| 322 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `interval_column_vector_select` | ported |
| 323 | `src/unit_test/storage/column_vector/column_vector_date_time_ut.cpp` | `ColumnVectorDateTimeTest` | `interval_column_slice_init` | ported |
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
| 368 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_tinyint` | ported |
| 369 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_tinyint` | ported |
| 370 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `tinyint_column_vector_select` | ported |
| 371 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `tinyint_column_slice_init` | ported |
| 372 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_smallint` | ported |
| 373 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_smallint` | ported |
| 374 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `smallint_column_vector_select` | ported |
| 375 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `smallint_column_slice_init` | ported |
| 376 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_int` | ported |
| 377 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_int` | ported |
| 378 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `integer_column_vector_select` | ported |
| 379 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `integer_column_slice_init` | ported |
| 380 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_bigint` | ported |
| 381 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_bigint` | ported |
| 382 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `bigint_column_vector_select` | ported |
| 383 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `bigint_column_slice_init` | ported |
| 384 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `flat_hugeint` | exempt |
| 385 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `contant_hugeint` | exempt |
| 386 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `hugeint_column_vector_select` | exempt |
| 387 | `src/unit_test/storage/column_vector/column_vector_integer_ut.cpp` | `ColumnVectorIntegerTest` | `hugeint_column_slice_init` | exempt |
| 388 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_integer_a` | exempt |
| 389 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_integer_column_vector_select` | exempt |
| 390 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_integer_column_slice_init` | exempt |
| 391 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_float_a` | exempt |
| 392 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_float_column_vector_select` | exempt |
| 393 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_float_column_slice_init` | exempt |
| 394 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_short_str_a` | exempt |
| 395 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_short_str_column_vector_select` | exempt |
| 396 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_short_str_column_slice_init` | exempt |
| 397 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_long_str_a` | exempt |
| 398 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_long_str_column_vector_select` | exempt |
| 399 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_long_str_column_slice_init` | exempt |
| 400 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_null_a` | exempt |
| 401 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_null_column_vector_select` | exempt |
| 402 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_null_column_slice_init` | exempt |
| 403 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_missing_a` | exempt |
| 404 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_missing_column_vector_select` | exempt |
| 405 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_missing_column_slice_init` | exempt |
| 406 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_tuple_a` | exempt |
| 407 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_tuple_column_vector_select` | exempt |
| 408 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_tuple_column_slice_init` | exempt |
| 409 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_array_a` | exempt |
| 410 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_array_column_vector_select` | exempt |
| 411 | `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp` | `ColumnVectorMixedTest` | `mixed_array_column_slice_init` | exempt |
| 412 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `flat_row` | partial |
| 413 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `contant_row` | partial |
| 414 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `row_column_vector_select` | partial |
| 415 | `src/unit_test/storage/column_vector/column_vector_row_ut.cpp` | `ColumnVectorRowTest` | `row_column_slice_init` | partial |
| 416 | `src/unit_test/storage/column_vector/column_vector_ut.cpp` | `ColumnVectorTest` | `ReadWrite` | partial |
| 417 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `flat_uuid` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 418 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `contant_uuid` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 419 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `uuid_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 420 | `src/unit_test/storage/column_vector/column_vector_uuid_ut.cpp` | `ColumnVectorUuidTest` | `uuid_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::uuid_flat_constant_select_slice` |
| 421 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `flat_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 422 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `constant_inline_varchar` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 423 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `varchar_column_vector_select` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 424 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `varchar_column_slice_init` | ported | `infinity-storage` `data_block::reference_suite::varchar_flat_constant_select_slice` |
| 425 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `flat_not_inline_varchar` | partial |
| 426 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `constant_not_inline_varchar` | partial |
| 427 | `src/unit_test/storage/column_vector/column_vector_varchar_ut.cpp` | `ColumnVectorVarcharTest` | `flat_mixed_inline_varchar` | partial |
| 428 | `src/unit_test/storage/column_vector/selection_ut.cpp` | `SelectionTest` | `test1` | ported | `infinity-storage` `selection::tests::selection_matches_the_reference` |
| 429 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_embedding` | exempt | C++ 用例仅调用 `AppendToJson` 构造 JSON，无对 JSON 的断言（`test_basic_type` 的 ASSERT_NEAR 读的是 Value 本身） |
| 430 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_sparse` | exempt | C++ 用例仅调用 `AppendToJson` 构造 JSON，无对 JSON 的断言（`test_basic_type` 的 ASSERT_NEAR 读的是 Value 本身） |
| 431 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_tensor` | exempt | C++ 用例仅调用 `AppendToJson` 构造 JSON，无对 JSON 的断言（`test_basic_type` 的 ASSERT_NEAR 读的是 Value 本身） |
| 432 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_tensor_array` | exempt | C++ 用例仅调用 `AppendToJson` 构造 JSON，无对 JSON 的断言（`test_basic_type` 的 ASSERT_NEAR 读的是 Value 本身） |
| 433 | `src/unit_test/storage/column_vector/value_json_ut.cpp` | `Value2JsonTest` | `test_basic_type` | exempt | C++ 用例仅调用 `AppendToJson` 构造 JSON，无对 JSON 的断言（`test_basic_type` 的 ASSERT_NEAR 读的是 Value 本身） |
| 434 | `src/unit_test/storage/column_vector/value_ut.cpp` | `ValueStringTest` | `test_tostring` | ported | `infinity-storage` `data_block::reference_suite::value_to_string_matches_the_reference`（JSON 以文本形式原样呈现） |
| 435 | `src/unit_test/storage/column_vector/value_ut.cpp` | `ValueStringTest` | `test_stringtovalue` | partial | `data_block::reference_suite::string_to_value_roundtrips_the_scalar_types` 覆盖标量；日期/时间与 JSON 的文本解析未逐项移植 |
| 436 | `src/unit_test/storage/column_vector/var_buffer_ut.cpp` | `VarBufferTest` | `test1` | ported | `infinity-storage` `var_buffer::tests::var_buffer_appends_and_roundtrips` |
| 437 | `src/unit_test/storage/common/kv_code_ut.cpp` | `KVCodeTest` | `kv_code1` | ported | `infinity-storage` `catalog_format::tests::catalog_keys_match_the_reference` |
| 438 | `src/unit_test/storage/common/kv_code_ut.cpp` | `KVCodeTest` | `kv_code2` | ported | `catalog_format::tests::name_keyed_catalog_keys_match_the_reference` |
| 439 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge1` | ported | `infinity-common` `loser_tree::tests::basic_merge_1` |
| 440 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge2` | ported | `infinity-common` `loser_tree::tests::basic_merge_2` |
| 441 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge3` | ported | `infinity-common` `loser_tree::tests::basic_merge_3` |
| 442 | `src/unit_test/storage/common/loser_tree_ut.cpp` | `LoserTreeTest` | `BasicMerge4` | ported | `infinity-common` `loser_tree::tests::basic_merge_4` |
| 443 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestBasicInsertAndGet` | ported |
| 444 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestLowercaseAliases` | ported |
| 445 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestDelete` | ported |
| 446 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestGetAllValuesWithRef` | ported |
| 447 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestGarbageCollection` | ported |
| 448 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestExpiredEntries` | ported |
| 449 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestGetWithRcuTime` | ported |
| 450 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestMultipleValuesPerKey` | ported |
| 451 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestIntegerKeys` | ported |
| 452 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestEmptyMap` | ported |
| 453 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestStressInsertAndRetrieve` | ported |
| 454 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestDeleteFunctionality` | ported |
| 455 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestDifferentValueTypes` | ported |
| 456 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestConcurrentInsertAndRead` | ported |
| 457 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestConcurrentInsertDeleteRead` | ported |
| 458 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestHighVolumeMultiThreading` | ported |
| 459 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestRcuSwapUnderLoad` | ported |
| 460 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMultiMapTest` | `TestRcuBehavior` | ported |
| 461 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestBasicInsertAndGet` | ported |
| 462 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestLowercaseAliases` | ported |
| 463 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestDelete` | ported |
| 464 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGetAllValuesWithRef` | ported |
| 465 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGetAllKeyValuePairs` | ported |
| 466 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGarbageCollection` | ported |
| 467 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestExpiredEntries` | ported |
| 468 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestGetWithRcuTime` | ported |
| 469 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestUniqueKeys` | ported |
| 470 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestIntegerKeys` | ported |
| 471 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestEmptyMap` | ported |
| 472 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestEmplace` | ported |
| 473 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestRange` | ported |
| 474 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestStressInsertAndRetrieve` | ported |
| 475 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestConcurrentInsertAndRead` | ported |
| 476 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestConcurrentInsertDeleteRead` | ported |
| 477 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestRcuSwapUnderLoad` | ported |
| 478 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestRcuBehavior` | ported |
| 479 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapTest` | `TestMapVsMultiMapBehavior` | ported |
| 480 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `ReadHeavyWorkload` | exempt |
| 481 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `WriteHeavyWorkload` | exempt |
| 482 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `ReadOnlyWorkload` | exempt |
| 483 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `HighContentionWorkload` | exempt |
| 484 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `ScalabilityTest` | exempt |
| 485 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `MemoryAndGcPerformance` | exempt |
| 486 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `ComprehensiveBenchmarkSummary` | exempt |
| 487 | `src/unit_test/storage/common/test_rcu_multimap_ut.cpp` | `RcuMapBenchmarkTest` | `QuickPerformanceVerification` | exempt |
| 488 | `src/unit_test/storage/definition/column_def_ut.cpp` | `ColumnDefTest` | `test1` | ported | `infinity-catalog` `value_format::tests::column_def_to_string_renders_constraints_in_set_order` |
| 489 | `src/unit_test/storage/definition/column_def_ut.cpp` | `ColumnDefTest` | `test2` | ported | `value_format::tests::column_def_to_string_renders_a_primary_key` |
| 490 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `test1` | ported |
| 491 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `test2` | ported |
| 492 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `test3` | ported |
| 493 | `src/unit_test/storage/definition/data_block_ut.cpp` | `DataBlockTest` | `ReadWrite` | ported |
| 494 | `src/unit_test/storage/definition/index_base_ut.cpp` | `IndexBaseTest` | `ivf_readwrite` | ported | `value_format::tests::ivf_index_base_roundtrips_through_its_bytes` |
| 495 | `src/unit_test/storage/definition/index_base_ut.cpp` | `IndexBaseTest` | `hnsw_readwrite` | ported | `value_format::tests::hnsw_index_base_roundtrips_through_its_bytes` |
| 496 | `src/unit_test/storage/definition/index_base_ut.cpp` | `IndexBaseTest` | `full_text_readwrite` | ported | `value_format::tests::full_text_index_base_roundtrips_through_its_bytes` |
| 497 | `src/unit_test/storage/definition/table_def_ut.cpp` | `TableDefTest` | `test1` | ported | `infinity-catalog` `value_format::tests::table_def_reports_schema_table_comment_and_columns` |
| 498 | `src/unit_test/storage/definition/table_def_ut.cpp` | `TableDefTest` | `ReadWrite` | ported | `value_format::tests::table_def_roundtrips_through_its_bytes` |
| 499 | `src/unit_test/storage/definition/table_ut.cpp` | `TableTest` | `test1` | gap |
| 500 | `src/unit_test/storage/invertedindex/column_inverter_ut.cpp` | `ColumnInverterTest` | `Invert` | ported |
| 501 | `src/unit_test/storage/invertedindex/common/external_sort_ut.cpp` | `ExternalSortTest` | `test1` | gap |
| 502 | `src/unit_test/storage/invertedindex/common/external_sort_ut.cpp` | `ExternalSortTest` | `test2` | gap |
| 503 | `src/unit_test/storage/invertedindex/common/skiplist_ut_nimpl.cpp` | `SkiplistTest` | `test1` | exempt | Skiplist 容器已登记为设计变更（读路径按 postings 列表解码），不单独移植 |
| 504 | `src/unit_test/storage/invertedindex/common/skiplist_ut_nimpl.cpp` | `SkiplistTest` | `test2` | exempt | Skiplist 容器已登记为设计变更（读路径按 postings 列表解码），不单独移植 |
| 505 | `src/unit_test/storage/invertedindex/common/skiplist_ut_nimpl.cpp` | `SkiplistTest` | `test3` | exempt | Skiplist 容器已登记为设计变更（读路径按 postings 列表解码），不单独移植 |
| 506 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test1` | exempt |
| 507 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test2` | exempt |
| 508 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test3` | exempt |
| 509 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test4` | exempt |
| 510 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test5` | exempt |
| 511 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test6` | exempt |
| 512 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test7` | exempt |
| 513 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test8` | exempt |
| 514 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test9` | exempt |
| 515 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test10` | exempt |
| 516 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test11` | exempt |
| 517 | `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp` | `InMemDocListDecoderTest` | `test12` | exempt |
| 518 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test1` | exempt |
| 519 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test2` | exempt |
| 520 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test3` | exempt |
| 521 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test4` | exempt |
| 522 | `src/unit_test/storage/invertedindex/format/inmem_position_list_decoder_ut.cpp` | `InMemPositionListDecoderTest` | `test5` | exempt |
| 523 | `src/unit_test/storage/invertedindex/format/position_list_decoder_ut.cpp` | `PositionListDecoderTest` | `test1` | exempt |
| 524 | `src/unit_test/storage/invertedindex/format/position_list_encoder_ut.cpp` | `PositionListEncoderTest` | `test1` | exempt |
| 525 | `src/unit_test/storage/invertedindex/format/position_list_encoder_ut.cpp` | `PositionListEncoderTest` | `test2` | exempt |
| 526 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test1` | exempt |
| 527 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test2` | exempt |
| 528 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test3` | exempt |
| 529 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test4` | exempt |
| 530 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test5` | exempt |
| 531 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test6` | exempt |
| 532 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test7` | exempt |
| 533 | `src/unit_test/storage/invertedindex/format/posting_buffer_ut.cpp` | `PostingBufferTest` | `test8` | exempt |
| 534 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_reader_ut.cpp` | `PostingByteSliceReaderTest` | `test1` | exempt |
| 535 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_reader_ut.cpp` | `PostingByteSliceReaderTest` | `test2` | exempt |
| 536 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_reader_ut.cpp` | `PostingByteSliceReaderTest` | `test3` | exempt |
| 537 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_ut.cpp` | `PostingByteSliceTest` | `test1` | exempt |
| 538 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_ut.cpp` | `PostingByteSliceTest` | `test2` | exempt |
| 539 | `src/unit_test/storage/invertedindex/format/posting_byte_slice_ut.cpp` | `PostingByteSliceTest` | `test3` | exempt |
| 540 | `src/unit_test/storage/invertedindex/format/skiplist_reader_ut.cpp` | `SkiplistReaderTest` | `SkipListReaderByteSliceTest` | exempt |
| 541 | `src/unit_test/storage/invertedindex/format/skiplist_reader_ut.cpp` | `SkiplistReaderTest` | `SkipListReaderPostingByteSliceTest` | exempt |
| 542 | `src/unit_test/storage/invertedindex/format/skiplist_writer_ut.cpp` | `SkipListWriterTest` | `test1` | exempt |
| 543 | `src/unit_test/storage/invertedindex/format/skiplist_writer_ut.cpp` | `SkipListWriterTest` | `test2` | exempt |
| 544 | `src/unit_test/storage/invertedindex/format/skiplist_writer_ut.cpp` | `SkipListWriterTest` | `test3` | exempt |
| 545 | `src/unit_test/storage/invertedindex/format/term_meta_ut.cpp` | `TermMetaTest` | `test1` | ported |
| 546 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildMemEmpty` | ported | `infinity-storage` `fst_build_mem_empty` |
| 547 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildFileEmpty` | ported | `infinity-storage` `fst_build_file_empty` |
| 548 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildMem` | ported | `infinity-storage` `fst_build_mem` |
| 549 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `BuildFile` | ported | `infinity-storage` `fst_build_file` |
| 550 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `Get` | ported | `infinity-storage` `fst_get_values` |
| 551 | `src/unit_test/storage/invertedindex/fst/fst_ut.cpp` | `FstTest` | `Iterate` | ported | `infinity-storage` `fst_iterate_with_bounds` |
| 552 | `src/unit_test/storage/invertedindex/fst/registry_ut.cpp` | `RegistryTest` | `InsertAndFind` | ported | `infinity-storage` `insert_and_find` |
| 553 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `Chunk` | ported | `infinity-executor` `tests::memory_indexer_builds_the_reference_postings` |
| 554 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `DISABLED_SLOW_Memory` | ported | `infinity-executor` `tests::disabled_slow_memory`（`#[ignore]`） |
| 555 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `SpillLoadTest` | gap |
| 556 | `src/unit_test/storage/invertedindex/memory_indexer_ut.cpp` | `MemoryIndexerTest` | `DISABLED_SLOW_SeekPosition` | ported | `infinity-executor` `tests::disabled_slow_seek_position`（`#[ignore]`） |
| 557 | `src/unit_test/storage/invertedindex/posting_merger_ut.cpp` | `PostingMergerTest` | `Basic` | gap |
| 558 | `src/unit_test/storage/invertedindex/posting_writer_ut.cpp` | `PostingWriterTest` | `test1` | gap |
| 559 | `src/unit_test/storage/invertedindex/posting_writer_ut.cpp` | `PostingWriterTest` | `tf_after_skip_to_across_segments` | gap |
| 560 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test1` | gap |
| 561 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test2` | gap |
| 562 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test3` | gap |
| 563 | `src/unit_test/storage/invertedindex/search/highlighter_ut.cpp` | `HighlighterTest` | `test4` | gap |
| 564 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_and` | gap |
| 565 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_or` | gap |
| 566 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_and_not` | gap |
| 567 | `src/unit_test/storage/invertedindex/search/query_builder_ut.cpp` | `QueryBuilderTest` | `test_and_not2` | gap |
| 568 | `src/unit_test/storage/invertedindex/search/query_match_ut.cpp` | `QueryMatchTest` | `basic_term` | gap |
| 569 | `src/unit_test/storage/invertedindex/search/query_match_ut.cpp` | `QueryMatchTest` | `phrase` | gap |
| 570 | `src/unit_test/storage/invertedindex/search/query_match_ut.cpp` | `QueryMatchTest` | `bmw_many_terms` | gap |
| 571 | `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp` | `QueryParserAndOptimizerTest` | `test1` | gap |
| 572 | `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp` | `QueryParserAndOptimizerTest` | `operator_option_test_standard` | gap |
| 573 | `src/unit_test/storage/invertedindex/search/query_parser_and_optimizer_ut.cpp` | `QueryParserAndOptimizerTest` | `operator_option_test_chinese` | gap |
| 574 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test1` | integration |
| 575 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test2` | integration |
| 576 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test3` | integration |
| 577 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test4` | integration |
| 578 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test5` | integration |
| 579 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test6` | integration |
| 580 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `test7` | integration |
| 581 | `src/unit_test/storage/io/byte_slice_write_read_ut.cpp` | `ByteSliceReaderWriterTest` | `TestDataConsistency` | integration |
| 582 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `test1` | integration |
| 583 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `test2` | integration |
| 584 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `test3` | integration |
| 585 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestExceedWriterTotalSize` | integration |
| 586 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestFilePointerSeek` | integration |
| 587 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestFileReadOverflowBuffer` | integration |
| 588 | `src/unit_test/storage/io/file_write_read_ut.cpp` | `FileWriteReadTest` | `TestFileIODataTypes` | integration |
| 589 | `src/unit_test/storage/io/local_file_ut.cpp` | `LocalFileTest` | `test1` | integration |
| 590 | `src/unit_test/storage/io/s3_client_minio_ut.cpp` | `S3ClientMinioTest` | `test1` | integration |
| 591 | `src/unit_test/storage/io/stream_reader_ut.cpp` | `StreamReaderTest` | `TestBasicStreamIO` | integration |
| 592 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestAppend` | ported | `infinity-storage` `append_then_delete` |
| 593 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestDir` | ported | `infinity-storage` `dir_create_and_remove` |
| 594 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestRead` | ported | `infinity-storage` `write_then_read` |
| 595 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestRename` | ported | `infinity-storage` `rename_moves_the_file` |
| 596 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestTruncate` | ported | `infinity-storage` `truncate_cuts_the_tail` |
| 597 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestMerge` | ported | `infinity-storage` `merge_appends_src_to_dst` |
| 598 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestCleanDir` | ported | `infinity-storage` `cleanup_directory_keeps_the_directory` |
| 599 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `minio_upload` | integration |
| 600 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `minio_download` | integration |
| 601 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestStorageType` | ported | `infinity-storage` `storage_type_conversions` |
| 602 | `src/unit_test/storage/io/virtual_store_ut.cpp` | `VirtualStoreTest` | `TestFileCompress` | ported | `infinity-storage` `file_compress_writes_a_record` |
| 603 | `src/unit_test/storage/knnindex/emvb_search/test_emvb_ut.cpp` | `EMVBTest` | `test_fakepq` | gap |
| 604 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testsum256` | exempt |
| 605 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testloadstore` | exempt |
| 606 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testmask` | exempt |
| 607 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testmax` | exempt |
| 608 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testleftpack` | exempt |
| 609 | `src/unit_test/storage/knnindex/emvb_search/test_simd_ut.cpp` | `SIMDTest` | `testleftpackf` | exempt |
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
| 622 | `src/unit_test/storage/knnindex/knn_hnsw/test_dist_func_ut.cpp` | `DistFuncTest` | `test1` | exempt | int8 IP 的 SSE 核与标量核对照；属已登记的 SIMD 原语设计变更 |
| 623 | `src/unit_test/storage/knnindex/knn_hnsw/test_dist_func_ut.cpp` | `DistFuncTest` | `test2` | ported | `infinity-index` `lvq::tests::lvq_l2_distance_matches_decompressed_vectors` |
| 624 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_bitmask_ut.cpp` | `HnswAlgBitmaskTest` | `test1` | ported | `infinity-index` `vector::reference_suite::filtered_search_honours_the_bitmask` |
| 625 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `test_memory` | partial |
| 626 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `test_compress` | partial |
| 627 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `DISABLED_SLOW_test_load` | ported | `infinity-index` `vector::reference_suite::disabled_slow_test_load`（`#[ignore]`） |
| 628 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_handler_ut.cpp` | `HnswHandlerTest` | `DISABLED_RANDOME_test_parallel` | ported | `infinity-index` `vector::reference_suite::disabled_random_test_parallel`（`#[ignore]`） |
| 629 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_sparse_ut.cpp` | `HnswSparseTest` | `test1` | partial |
| 630 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_1` | ported |
| 631 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_2` | ported |
| 632 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_3` | ported | `infinity-index` `vector::reference_suite::plain_parallel_insert_and_search` |
| 633 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_plain_4` | ported |
| 634 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_1` | ported |
| 635 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_2` | ported | `infinity-index` `vector::reference_suite::lvq_parallel_insert_and_search` |
| 636 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_3` | ported | `infinity-index` `vector::reference_suite::lvq_chunk_roundtrip_preserves_recall` |
| 637 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_lvq_4` | ported | `infinity-index` `vector::reference_suite::plain_index_compresses_to_lvq` |
| 638 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_rabitq_1` | ported |
| 639 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_rabitq_2` | ported | `infinity-index` `vector::reference_suite::rabitq_chunk_roundtrip_preserves_recall` |
| 640 | `src/unit_test/storage/knnindex/knn_hnsw/test_hnsw_ut.cpp` | `HnswAlgTest` | `test_rabitq_3` | ported | `infinity-index` `vector::reference_suite::rabitq_parallel_insert_and_search` |
| 641 | `src/unit_test/storage/knnindex/knn_hnsw/test_lsg_build_ut.cpp` | `LSGBuildTest` | `test_avg` | ported |
| 642 | `src/unit_test/storage/knnindex/knn_hnsw/test_lsg_build_ut.cpp` | `LSGBuildTest` | `test1` | ported |
| 643 | `src/unit_test/storage/knnindex/knn_hnsw/test_lvq_ut.cpp` | `HnswLVQTest` | `test1` | ported |
| 644 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_flat_lvq` | partial |
| 645 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_flat_rabitq` | partial |
| 646 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_hnsw_lvq` | partial |
| 647 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_hnsw_rabitq` | partial |
| 648 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_hnsw_lsg_rabitq` | partial |
| 649 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_compress_hnsw_to_rabitq` | partial |
| 650 | `src/unit_test/storage/knnindex/knn_hnsw/test_quantizer_performance_ut.cpp` | `QuantizerPerformanceTest` | `DISABLED_SLOW_compress_lsg_to_rabitq` | partial |
| 651 | `src/unit_test/storage/knnindex/knn_hnsw/test_rabitq_ut.cpp` | `RabitqTest` | `test_simple` | ported |
| 652 | `src/unit_test/storage/knnindex/knn_hnsw/test_rabitq_ut.cpp` | `RabitqTest` | `test_compress` | ported |
| 653 | `src/unit_test/storage/knnindex/knn_hnsw/test_rabitq_ut.cpp` | `RabitqTest` | `test_distance` | ported |
| 654 | `src/unit_test/storage/knnindex/knn_ivf/index_ivf_def_ut.cpp` | `IndexIVFTest` | `testplain` | ported | `infinity-index` `ivf_def::tests::testplain` |
| 655 | `src/unit_test/storage/knnindex/knn_ivf/index_ivf_def_ut.cpp` | `IndexIVFTest` | `testsq` | ported | `infinity-index` `ivf_def::tests::testsq` |
| 656 | `src/unit_test/storage/knnindex/knn_ivf/index_ivf_def_ut.cpp` | `IndexIVFTest` | `testpq` | ported | `infinity-index` `ivf_def::tests::testpq` |
| 657 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_fwd_ut.cpp` | `BMPFwdTest` | `test1` | ported |
| 658 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_index_ut.cpp` | `BMPIndexTest` | `test1` | partial |
| 659 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_index_ut.cpp` | `BMPIndexTest` | `test2` | partial |
| 660 | `src/unit_test/storage/knnindex/knn_sparse/test_bmp_ivt_ut.cpp` | `BMPIvtTest` | `test1` | ported |
| 661 | `src/unit_test/storage/knnindex/knn_sparse/test_bp_reordering_ut.cpp` | `BPReorderingTest` | `test1` | ported |
| 662 | `src/unit_test/storage/knnindex/knn_sparse/test_bp_reordering_ut.cpp` | `BPReorderingTest` | `test2` | ported |
| 663 | `src/unit_test/storage/knnindex/knn_sparse/test_linscan_ut.cpp` | `LinScanAlgTest` | `accurate_scan` | ported |
| 664 | `src/unit_test/storage/knnindex/knn_sparse/test_linscan_ut.cpp` | `LinScanAlgTest` | `approximate_scan` | ported |
| 665 | `src/unit_test/storage/knnindex/merge_optimize/test_optimize_ut.cpp` | `OptimizeKnnTest` | `test_hnsw_optimize` | partial |
| 666 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidQuantizerTest` | `test_train_quantize_dequantize` | ported |
| 667 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidQuantizerTest` | `test_2bit_quantization` | ported |
| 668 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_basic_construction` | ported |
| 669 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_train_and_add` | ported |
| 670 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_search` | ported |
| 671 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_save_load` | ported |
| 672 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_move_semantics` | ported |
| 673 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexTest` | `test_mem_usage` | ported |
| 674 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_construction` | ported |
| 675 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_train_and_add` | ported |
| 676 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_basic_search` | ported |
| 677 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_save_load` | ported |
| 678 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_move_semantics` | ported |
| 679 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_mem_usage` | ported |
| 680 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_merge` | ported |
| 681 | `src/unit_test/storage/knnindex/plaid_search/test_plaid_ut.cpp` | `PlaidIndexColBERTSaRTest` | `test_colbertsar_size_smaller_than_plaid` | ported |
| 682 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `SaveAndLoad` | ported | `infinity-storage` `mvcc::tests::save_and_load_roundtrip` |
| 683 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `SaveAndLoad2` | ported | `mvcc::tests::reopen_preserves_the_slot_array` |
| 684 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `delete_test` | ported | `mvcc::tests::delete_twice_is_rejected` |
| 685 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `check_delete_test` | ported | `mvcc::tests::check_delete_compares_timestamps` |
| 686 | `src/unit_test/storage/meta/entry/block_version_ut.cpp` | `BlockVersionTest` | `get_delete_ts_test` | ported | `mvcc::tests::get_delete_ts_returns_the_slot_range` |
| 687 | `src/unit_test/storage/new_catalog/alter_ut.cpp` | `TestTxnAlter` | `add_column0` | integration |
| 688 | `src/unit_test/storage/new_catalog/alter_ut.cpp` | `TestTxnAlter` | `drop_column0` | integration |
| 689 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append0` | ported | `infinity-executor` `appends_accumulate_across_commits` |
| 690 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append1` | integration |
| 691 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append2` | ported | `infinity-executor` `large_appends_span_blocks` |
| 692 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_mismatch` | ported | `infinity-executor` `append_type_mismatch_is_rejected` |
| 693 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_drop_db` | ported | `infinity-executor` `append_after_drop_is_rejected` |
| 694 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_drop_table` | ported | `infinity-executor` `append_after_drop_is_rejected` |
| 695 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_add_column` | ported | `infinity-executor` `append_add_column_fills_old_rows_with_null` |
| 696 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_drop_column` | ported | `infinity-executor` `append_add_column_fills_old_rows_with_null` |
| 697 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_rename` | ported | `infinity-executor` `append_uses_the_renamed_table` |
| 698 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_append` | integration |
| 699 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `DISABLED_SLOW_test_append_append_concurrent` | ported | `infinity-executor` `tests::disabled_slow_test_append_append_concurrent`（`#[ignore]`） |
| 700 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_create_index` | ported | `infinity-executor` `append_around_secondary_index_lifecycle` |
| 701 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_drop_index` | ported | `infinity-executor` `append_around_secondary_index_lifecycle` |
| 702 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_compact` | ported | `infinity-executor` `append_then_compact_preserves_rows` |
| 703 | `src/unit_test/storage/new_catalog/append_ut.cpp` | `TestTxnAppend` | `test_append_and_optimize_index` | integration |
| 704 | `src/unit_test/storage/new_catalog/checkpoint1_ut.cpp` | `TestTxnCheckpointTest` | `checkpoint_and_create_db` | integration |
| 705 | `src/unit_test/storage/new_catalog/checkpoint1_ut.cpp` | `TestTxnCheckpointTest` | `checkpoint_and_create_table` | integration |
| 706 | `src/unit_test/storage/new_catalog/checkpoint_add_column_ut.cpp` | `TestTxnCheckpointAddColumnTest` | `addcol_checkpoint_insert` | integration |
| 707 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `test_checkpoint0` | integration |
| 708 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `DISABLED_SLOW_test_checkpoint1` | integration |
| 709 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `test_checkpoint2` | integration |
| 710 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `DISABLED_SLOW_test_checkpoint3` | integration |
| 711 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `DISABLED_SLOW_test_checkpoint4` | integration |
| 712 | `src/unit_test/storage/new_catalog/checkpoint_internal_ut.cpp` | `TestTxnCheckpointInternalTest` | `test_checkpoint5` | integration |
| 713 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_db` | integration |
| 714 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_table` | integration |
| 715 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_index` | integration |
| 716 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_compact` | integration |
| 717 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_optimize` | integration |
| 718 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_drop_column` | integration |
| 719 | `src/unit_test/storage/new_catalog/cleanup_internal_ut.cpp` | `TestTxnCleanupInternal` | `test_cleanup_drop_index_and_checkpoint_and_restart` | integration |
| 720 | `src/unit_test/storage/new_catalog/cleanup_rollback_txn.cpp` | `TestTxnCleanupRollbackTxn` | `test_import_with_index_rollback_cleanup` | integration |
| 721 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_with_drop_db` | integration |
| 722 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_drop_table` | integration |
| 723 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_add_columns` | integration |
| 724 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_drop_columns` | integration |
| 725 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_rename_table` | integration |
| 726 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_compact` | integration |
| 727 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_drop_index` | integration |
| 728 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_optimize_index` | integration |
| 729 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_append` | integration |
| 730 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_delete` | integration |
| 731 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_update` | integration |
| 732 | `src/unit_test/storage/new_catalog/cleanup_ut.cpp` | `TestTxnCleanup` | `cleanup_and_dump_index` | integration |
| 733 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `test_add_columns` | integration |
| 734 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `alter_column` | integration |
| 735 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `add_column_and_drop_db` | integration |
| 736 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `add_column_and_drop_table` | integration |
| 737 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `add_column_and_add_column` | integration |
| 738 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `drop_column_and_drop_db` | integration |
| 739 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `drop_column_and_drop_table` | integration |
| 740 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `drop_column_and_add_column` | integration |
| 741 | `src/unit_test/storage/new_catalog/column_ut.cpp` | `TestTxnColumn` | `drop_column_and_drop_column` | integration |
| 742 | `src/unit_test/storage/new_catalog/compact_internal_ut.cpp` | `TestTxnCompactInternal` | `test_compact` | integration |
| 743 | `src/unit_test/storage/new_catalog/compact_internal_ut.cpp` | `TestTxnCompactInternal` | `test_compact_with_index` | integration |
| 744 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_with_index_commit` | integration |
| 745 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_with_index_rollback` | integration |
| 746 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_db` | integration |
| 747 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_table` | integration |
| 748 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_add_columns` | ported | `infinity-executor` `compact_then_alter_and_drop_table` |
| 749 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_columns` | integration |
| 750 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_rename_table` | integration |
| 751 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_compact` | ported | `infinity-executor` `compaction_is_idempotent` |
| 752 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_create_index` | integration |
| 753 | `src/unit_test/storage/new_catalog/compact_ut.cpp` | `TestTxnCompact` | `compact_and_drop_index` | integration |
| 754 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `createdb_createdb_test` | integration |
| 755 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `db_test2` | integration |
| 756 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `db_test3` | integration |
| 757 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `createdb_wrong_conflicttype` | integration |
| 758 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `dropdb_wrong_conflicttype` | integration |
| 759 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `dropdb_dropdb_test` | integration |
| 760 | `src/unit_test/storage/new_catalog/database_ut.cpp` | `TestTxnDatabase` | `createdb_dropdb_test` | integration |
| 761 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete` | ported | `infinity-executor` `delete_removes_rows_from_scans` |
| 762 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_multiple_blocks` | ported | `infinity-executor` `delete_across_multiple_blocks` |
| 763 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_db` | integration |
| 764 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_table` | integration |
| 765 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_add_column` | integration |
| 766 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_column` | integration |
| 767 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_rename` | integration |
| 768 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_create_index` | integration |
| 769 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_drop_index` | integration |
| 770 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_import` | integration |
| 771 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_append` | ported | `infinity-executor` `delete_then_append_and_compact` |
| 772 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_delete` | integration |
| 773 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_compact` | ported | `infinity-executor` `delete_then_append_and_compact` |
| 774 | `src/unit_test/storage/new_catalog/delete_ut.cpp` | `TestTxnDelete` | `test_delete_and_optimize_index` | integration |
| 775 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_db` | integration |
| 776 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_table` | integration |
| 777 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_add_column` | integration |
| 778 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_column` | integration |
| 779 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_rename_table` | integration |
| 780 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_create_index` | integration |
| 781 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_drop_index` | integration |
| 782 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_import` | integration |
| 783 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_append` | integration |
| 784 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `dump_and_delete` | integration |
| 785 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `DISABLED_SLOW_dump_and_dump` | integration |
| 786 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `DISABLED_SLOW_test_dump_index_and_optimize_index` | integration |
| 787 | `src/unit_test/storage/new_catalog/dump_mem_index_ut.cpp` | `TestTxnDumpMemIndex` | `test_dump_index_and_compact` | integration |
| 788 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import1` | integration |
| 789 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_with_index` | integration |
| 790 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_with_index_rollback` | integration |
| 791 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_insert_and_import` | integration |
| 792 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_drop_db` | integration |
| 793 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_drop_table` | integration |
| 794 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_add_columns` | integration |
| 795 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_drop_columns` | integration |
| 796 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import` | integration |
| 797 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_append_table` | integration |
| 798 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_import_table` | integration |
| 799 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_rename` | integration |
| 800 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_and_create_index` | integration |
| 801 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_and_drop_index` | integration |
| 802 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `DISABLED_SLOW_test_import_and_compact` | integration |
| 803 | `src/unit_test/storage/new_catalog/import_ut.cpp` | `TestTxnImport` | `test_import_and_optimize_index` | integration |
| 804 | `src/unit_test/storage/new_catalog/index_internal_ut.cpp` | `TestTxnIndexInternal` | `test_index0` | integration |
| 805 | `src/unit_test/storage/new_catalog/index_internal_ut.cpp` | `TestTxnIndexInternal` | `test_index` | integration |
| 806 | `src/unit_test/storage/new_catalog/index_internal_ut.cpp` | `TestTxnIndexInternal` | `test_populate_index0` | integration |
| 807 | `src/unit_test/storage/new_catalog/index_internal_ut.cpp` | `TestTxnIndexInternal` | `DISABLED_SLOW_test_populate_index` | integration |
| 808 | `src/unit_test/storage/new_catalog/index_internal_ut.cpp` | `TestTxnIndexInternal` | `test_json_index` | integration |
| 809 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `index_test1` | integration |
| 810 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `index_test2` | integration |
| 811 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `createindex_wrong_conflicttype` | integration |
| 812 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `dropindex_wrong_conflicttype` | integration |
| 813 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `create_index_and_drop_db` | integration |
| 814 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `create_index_and_drop_table` | integration |
| 815 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `DISABLED_SLOW_create_index_create_index` | integration |
| 816 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `create_index_add_column` | integration |
| 817 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `create_index_drop_column` | integration |
| 818 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `create_index_rename_table` | integration |
| 819 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `drop_index_and_drop_db` | integration |
| 820 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `drop_index_and_drop_table` | integration |
| 821 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `create_index_and_drop_index` | integration |
| 822 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `drop_index_and_drop_index` | integration |
| 823 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `drop_index_add_column` | integration |
| 824 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `drop_index_drop_column` | integration |
| 825 | `src/unit_test/storage/new_catalog/index_ut.cpp` | `TestTxnIndex` | `drop_index_and_rename_table` | integration |
| 826 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store0` | integration |
| 827 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `DISABLED_SLOW_kv_store1` | integration |
| 828 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store2` | integration |
| 829 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store3` | integration |
| 830 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store4` | integration |
| 831 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store5` | integration |
| 832 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `kv_store6` | integration |
| 833 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `wal` | integration |
| 834 | `src/unit_test/storage/new_catalog/kv_store_ut.cpp` | `TestTxnKVStoreTest` | `txn_conflict_same_key_snapshot_isolation` | integration |
| 835 | `src/unit_test/storage/new_catalog/kv_utility_ut.cpp` | `KVUtilityTest` | `test1` | integration |
| 836 | `src/unit_test/storage/new_catalog/meta_cache_ut.cpp` | `MetaCacheTest` | `test_db` | integration |
| 837 | `src/unit_test/storage/new_catalog/meta_cache_ut.cpp` | `MetaCacheTest` | `test_table` | integration |
| 838 | `src/unit_test/storage/new_catalog/meta_cache_ut.cpp` | `MetaCacheTest` | `test_index` | integration |
| 839 | `src/unit_test/storage/new_catalog/meta_cache_ut.cpp` | `MetaCacheTest` | `test_mix` | integration |
| 840 | `src/unit_test/storage/new_catalog/meta_cache_ut.cpp` | `MetaCacheTest` | `test_erase` | integration |
| 841 | `src/unit_test/storage/new_catalog/new_catalog_ut.cpp` | `TestTxnNewCatalog` | `test_block_lock` | integration |
| 842 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `test_ts` | integration |
| 843 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `DISABLED_SLOW_test_parallel_ts` | ported | `infinity-executor` `tests::disabled_slow_test_parallel_ts`（`#[ignore]`） |
| 844 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `test_check_txns` | integration |
| 845 | `src/unit_test/storage/new_catalog/new_txn_mgr_ut.cpp` | `TestTxnManagerTest` | `DISABLED_SLOW_test_parallel_insert_table` | ported | `infinity-executor` `tests::disabled_slow_test_parallel_insert_table`（`#[ignore]`） |
| 846 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_create_db` | integration |
| 847 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_create_db2` | integration |
| 848 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_create_table` | integration |
| 849 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_drop_table` | integration |
| 850 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_create` | integration |
| 851 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_drop` | integration |
| 852 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_drop_column_add_column` | integration |
| 853 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_compact` | integration |
| 854 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_delete_compact` | integration |
| 855 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_rename` | integration |
| 856 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_dump_optimize_index` | integration |
| 857 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_append_append` | integration |
| 858 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_dump_index` | integration |
| 859 | `src/unit_test/storage/new_catalog/new_txn_replay_ut.cpp` | `TestTxnReplayTest` | `test_replay_flush_gap_optimize_index` | integration |
| 860 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_create_db` | integration |
| 861 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_import` | integration |
| 862 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_compact` | integration |
| 863 | `src/unit_test/storage/new_catalog/new_txn_replay_with_ckp_ut.cpp` | `TxnReplayExceptionTest` | `test_replay_insert` | integration |
| 864 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_rollback` | integration |
| 865 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_db` | integration |
| 866 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_table` | integration |
| 867 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_index` | integration |
| 868 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `DISABLED_SLOW_optimize_index_and_optimize_index` | integration |
| 869 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `DISABLED_SLOW_optimize_index_and_dump_index` | integration |
| 870 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_add_column` | integration |
| 871 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_drop_column` | integration |
| 872 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `optimize_index_and_rename_table` | integration |
| 873 | `src/unit_test/storage/new_catalog/optimize_index_ut.cpp` | `TestTxnOptimizeIndex` | `DISABLED_SLOW_optimize_index_and_compact_table` | integration |
| 874 | `src/unit_test/storage/new_catalog/replay_alter_ut.cpp` | `TestTxnReplayAlter` | `test_add_column` | integration |
| 875 | `src/unit_test/storage/new_catalog/replay_alter_ut.cpp` | `TestTxnReplayAlter` | `test_drop_column` | integration |
| 876 | `src/unit_test/storage/new_catalog/replay_alter_ut.cpp` | `TestTxnReplayAlter` | `test_rename` | integration |
| 877 | `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp` | `TestTxnReplayAppend` | `test_append0` | integration |
| 878 | `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp` | `TestTxnReplayAppend` | `test_replay_append_delete` | integration |
| 879 | `src/unit_test/storage/new_catalog/replay_append_delete_ut.cpp` | `TestTxnReplayAppend` | `test_replay_append_with_index0` | integration |
| 880 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact_commit` | integration |
| 881 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact_rollback` | integration |
| 882 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_replay_compact_flush_gap` | integration |
| 883 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact_interrupt` | integration |
| 884 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `DISABLED_SLOW_test_compact_with_index` | integration |
| 885 | `src/unit_test/storage/new_catalog/replay_compact_ut.cpp` | `TestTxnReplayCompact` | `test_compact1_rollback` | integration |
| 886 | `src/unit_test/storage/new_catalog/replay_import_ut.cpp` | `TestTxnReplayImport` | `test_import0` | integration |
| 887 | `src/unit_test/storage/new_catalog/replay_import_ut.cpp` | `TestTxnReplayImport` | `test_import_with_index` | integration |
| 888 | `src/unit_test/storage/new_catalog/replay_index_ut.cpp` | `TestTxnReplayIndex` | `DISABLED_SLOW_test_replay_append_with_index` | integration |
| 889 | `src/unit_test/storage/new_catalog/replay_index_ut.cpp` | `TestTxnReplayIndex` | `DISABLED_SLOW_test_populate_index` | integration |
| 890 | `src/unit_test/storage/new_catalog/replay_index_ut.cpp` | `TestTxnReplayIndex` | `drop_index` | integration |
| 891 | `src/unit_test/storage/new_catalog/replay_index_ut.cpp` | `TestTxnReplayIndex` | `test_json_index_replay` | integration |
| 892 | `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp` | `TestTxnReplayOptimize` | `DISABLED_SLOW_test_optimize_commit` | integration |
| 893 | `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp` | `TestTxnReplayOptimize` | `DISABLED_SLOW_test_optimize_rollback` | integration |
| 894 | `src/unit_test/storage/new_catalog/replay_optimize_ut.cpp` | `TestTxnReplayOptimize` | `DISABLED_SLOW_test_optimize_interrupt` | integration |
| 895 | `src/unit_test/storage/new_catalog/replay_restore_ut.cpp` | `TestTxnReplayRestore` | `DISABLED_SLOW_test_repaly_restore_table_snapshot` | integration |
| 896 | `src/unit_test/storage/new_catalog/replay_restore_ut.cpp` | `TestTxnReplayRestore` | `DISABLED_SLOW_test_repaly_restore_database_snapshot` | integration |
| 897 | `src/unit_test/storage/new_catalog/replay_restore_ut.cpp` | `TestTxnReplayRestore` | `DISABLED_SLOW_test_repaly_restore_system_snapshot` | integration |
| 898 | `src/unit_test/storage/new_catalog/table_cache_ut.cpp` | `TestTableCache` | `test_table_cache` | integration |
| 899 | `src/unit_test/storage/new_catalog/table_meta_ut.cpp` | `TestTxnTableMeta` | `table_meta` | integration |
| 900 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `table_test1` | ported | `infinity-executor` `create_and_drop_table_conflicts` |
| 901 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `table_test2` | integration |
| 902 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `createtable_wrong_conflicttype` | integration |
| 903 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `droptable_wrong_conflicttype` | integration |
| 904 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `createtable_createtable` | integration |
| 905 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `createdb_createtable_test` | integration |
| 906 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `createtable_dropdb_test` | integration |
| 907 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `dropdb_droptable_test` | integration |
| 908 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `createtable_droptable_test` | integration |
| 909 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `droptable_droptable_test` | integration |
| 910 | `src/unit_test/storage/new_catalog/table_ut.cpp` | `TestTxnTable` | `rename_table_test` | integration |
| 911 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update` | ported | `infinity-executor` `update_replaces_visible_values` |
| 912 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_multiple_blocks` | integration |
| 913 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_drop_db` | integration |
| 914 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_drop_table` | integration |
| 915 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_drop_index` | integration |
| 916 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_dump_index` | integration |
| 917 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_add_drop_column_conflicts` | integration |
| 918 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_compact_conflicts` | integration |
| 919 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_create_index_conflicts` | integration |
| 920 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_delete_conflicts` | ported | `infinity-executor` `update_and_delete_compose` |
| 921 | `src/unit_test/storage/new_catalog/update_ut.cpp` | `TestTxnUpdate` | `test_update_and_delete_no_conflicts` | integration |
| 922 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `create_index_append` | gap |
| 923 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_append` | gap |
| 924 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_import` | gap |
| 925 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_compact` | gap |
| 926 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `add_column_add_column` | gap |
| 927 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_append` | gap |
| 928 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_import` | gap |
| 929 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_compact` | gap |
| 930 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `drop_column_create_index` | exempt |
| 931 | `src/unit_test/storage/new_request/conflict_request_ut.cpp` | `TestConflictRequestTest` | `delete_append` | gap |
| 932 | `src/unit_test/storage/new_request/data_request_ut.cpp` | `TestDataRequest` | `test_append` | gap |
| 933 | `src/unit_test/storage/new_request/data_request_ut.cpp` | `TestDataRequest` | `test_select` | gap |
| 934 | `src/unit_test/storage/new_request/data_request_ut.cpp` | `TestDataRequest` | `test_delete` | gap |
| 935 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_create_db` | gap |
| 936 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_db` | gap |
| 937 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_create_table` | gap |
| 938 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_table` | gap |
| 939 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_create_index` | gap |
| 940 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_index` | gap |
| 941 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_add_column` | gap |
| 942 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_drop_column` | gap |
| 943 | `src/unit_test/storage/new_request/ddl_request_ut.cpp` | `TestDDLRequest` | `test_use_schema` | gap |
| 944 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `index_scan` | gap |
| 945 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `fulltext_index_scan` | gap |
| 946 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `vector_index_scan` | gap |
| 947 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `sparse_index_scan` | gap |
| 948 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `tensor_index_scan` | gap |
| 949 | `src/unit_test/storage/new_request/index_request_ut.cpp` | `TestIndexRequest` | `test_optimize_index` | gap |
| 950 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `PersistFileBasic` | gap |
| 951 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `PersistMultiFile` | gap |
| 952 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `PersistFileMultiThread` | gap |
| 953 | `src/unit_test/storage/persistence/persistence_manager_ut.cpp` | `PersistenceManagerTest` | `CleanupBasic` | gap |
| 954 | `src/unit_test/storage/result_cache_manager_ut.cpp` | `ResultCacheManagerTest` | `test1` | ported | `infinity-storage` `result_cache::tests::cache_projects_columns_and_merges_new_ones` |
| 955 | `src/unit_test/storage/result_cache_manager_ut.cpp` | `ResultCacheManagerTest` | `test2` | ported | `infinity-storage` `result_cache::tests::cache_evicts_the_least_recently_used_entry` |
| 956 | `src/unit_test/storage/secondary_index/test_high_cardinality_ut.cpp` | `HighCardinalitySecondaryIndexTest` | `TestSaveLoadHighCardinality` | partial |
| 957 | `src/unit_test/storage/secondary_index/test_high_cardinality_ut.cpp` | `HighCardinalitySecondaryIndexTest` | `TestTxn` | partial |
| 958 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestIntegerLowCardinality` | partial |
| 959 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestFloatLowCardinality` | partial |
| 960 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestEmptyIndex` | partial |
| 961 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestSingleValueCardinality` | partial |
| 962 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestComparisonWithHighCardinality` | partial |
| 963 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestSaveLoadLowCardinality` | partial |
| 964 | `src/unit_test/storage/secondary_index/test_low_cardinality_ut.cpp` | `LowCardinalitySecondaryIndexTest` | `TestAllDataType` | partial |
| 965 | `src/unit_test/storage/tracer/test_tracer_ut.cpp` | `MemIndexTracerTest` | `test1` | gap |
| 966 | `src/unit_test/storage/tracer/test_tracer_ut.cpp` | `MemIndexTracerTest` | `test2` | gap |
| 967 | `src/unit_test/storage/txn/conflict_check_ut.cpp` | `ConflictCheckTest` | `conflict_check_delete` | gap |
| 968 | `src/unit_test/storage/txn/constants_ut.cpp` | `TxnContants` | `test1` | ported | `infinity-txn` `state::tests::started_state_renders_as_started` |
| 969 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_restore_database_rollback_basic` | gap |
| 970 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_restore_database_create_database_multithreaded` | gap |
| 971 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_create_snapshot_same_name_multithreaded` | gap |
| 972 | `src/unit_test/storage/txn/database_snapshot_ut.cpp` | `DatabaseSnapshotTest` | `test_sql_parser` | gap |
| 973 | `src/unit_test/storage/txn/system_snapshot_ut.cpp` | `SystemSnapshotTest` | `test_restore_system_rollback_basic` | gap |
| 974 | `src/unit_test/storage/txn/system_snapshot_ut.cpp` | `SystemSnapshotTest` | `test_restore_system_create_database_multithreaded` | gap |
| 975 | `src/unit_test/storage/txn/system_snapshot_ut.cpp` | `SystemSnapshotTest` | `test_sql_parser` | gap |
| 976 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_restore_table_rollback_basic` | gap |
| 977 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_restore_table_create_table_multithreaded` | gap |
| 978 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_create_snapshot_same_name_multithreaded` | gap |
| 979 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_show_snapshot_multithreaded` | gap |
| 980 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_restore_table_same_snapshot_multithreaded` | gap |
| 981 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_create_snapshot_delete_data` | gap |
| 982 | `src/unit_test/storage/txn/table_snapshot_ut.cpp` | `TableSnapshotTest` | `test_create_snapshot_insert_data` | gap |
| 983 | `src/unit_test/storage/wal/recycle_log_ut.cpp` | `RecycleLogTest` | `recycle_wal_after_delta_checkpoint` | integration |
| 984 | `src/unit_test/storage/wal/repeat_replay_ut.cpp` | `RepeatReplayTest` | `append` | integration |
| 985 | `src/unit_test/storage/wal/repeat_replay_ut.cpp` | `RepeatReplayTest` | `import` | integration |
| 986 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `ReadWriteV2` | ported | `infinity-storage` `read_write_v2_roundtrips_every_command` |
| 987 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `WalEntryIterator` | ported | `infinity-storage` `wal_entry_iterator_replay_stops_at_checkpoint` |
| 988 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `WalListIterator` | ported | `infinity-storage` `wal_list_iterator_replay_stops_at_checkpoint` |
| 989 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedEntryBeforeCheckpointIsIgnored` | ported | `infinity-storage` `damaged_entry_before_checkpoint_is_ignored` |
| 990 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedEntryTruncatesWalFile` | ported | `infinity-storage` `damaged_entry_truncates_wal_file` |
| 991 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedFirstEntryDropsWalFile` | ported | `infinity-storage` `damaged_first_entry_drops_wal_file` |
| 992 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedTailAfterCheckpointIsTruncated` | ported | `infinity-storage` `damaged_tail_after_checkpoint_is_truncated` |
| 993 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamagedOlderFileDropsNewerOnes` | ported | `infinity-storage` `damaged_older_file_drops_newer_ones` |
| 994 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `NoCheckpointStillPurgesDamage` | ported | `infinity-storage` `no_checkpoint_still_purges_damage` |
| 995 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `TornTailStopsIteration` | ported | `infinity-storage` `torn_tail_stops_iteration` |
| 996 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `CorruptedTrailingSizeIsRecoveredByForwardScan` | ported | `infinity-storage` `corrupted_trailing_size_is_recovered_by_forward_scan` |
| 997 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `EmptyWalFileIsNotAnError` | ported | `infinity-storage` `empty_wal_file_is_not_an_error` |
| 998 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `FileShorterThanEntrySize` | ported | `infinity-storage` `file_shorter_than_entry_size_is_empty` |
| 999 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `AllFilesPurgedLeavesEmptyList` | ported | `infinity-storage` `all_files_purged_leaves_empty_list` |
| 1000 | `src/unit_test/storage/wal/wal_entry_ut.cpp` | `WalEntryTest` | `DamageAccumulatesToCheckpointFile` | ported | `infinity-storage` `damage_accumulates_to_checkpoint_file` |
| 1001 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_database` | integration |
| 1002 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_tables` | integration |
| 1003 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_append` | integration |
| 1004 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_import` | integration |
| 1005 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_compact` | integration |
| 1006 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_create_index_IvfFlat` | integration |
| 1007 | `src/unit_test/storage/wal/wal_replay_ut.cpp` | `WalReplayTest` | `wal_replay_create_index_hnsw` | integration |
| 1008 | `src/unit_test/third_party/json_ut.cpp` | `JsonTest` | `nlohmann_test` | exempt |
| 1009 | `src/unit_test/third_party/json_ut.cpp` | `JsonTest` | `simdjson_test` | exempt |
| 1010 | `src/unit_test/third_party/test_croaring_ut.cpp` | `CRoaringTest` | `test_size` | exempt |
| 1011 | `src/unit_test/third_party/test_croaring_ut.cpp` | `CRoaringTest` | `test1` | exempt |
| 1012 | `src/unit_test/third_party/test_croaring_ut.cpp` | `CRoaringTest` | `test2` | exempt |
