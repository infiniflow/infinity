# 现状缺口清单

本文件是**唯一**的缺口汇总：哪些地方与参考不同、为什么、影响是什么。分四类：

- **§1 有意不移植**：已经决定不做，不是待办；
- **§2 未实现 / 未接线**：功能缺失，附证据与影响；
- **§3 不可逐字节比对**：参考自身不确定或依赖实现细节，门禁只能按内容比；
- **§4 覆盖不到的输入空间**：机制有了，门禁还没跑到那个规模/形态。

格式层面的逐文件细节在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5；逐类/逐用例的映射状态在
[`CLASS_AUDIT.md`](CLASS_AUDIT.md)（`partial` / `gap` 行）与 [`TEST_AUDIT.md`](TEST_AUDIT.md)（`gap` 行）。
**已接受的设计变更**（不是待办）登记在 [`CLASS_MAP.md`](CLASS_MAP.md)「有意的设计变更」，本文不重复。

---

## 1 有意不移植

| 项 | 决定 | 影响 | 依据 |
|---|---|---|---|
| 远程对象存储（`obj_key` 打包 + VirtualStore 上传、catalog 的 `meta/` 路径树同步） | 不移植；本地对象目录（`pm\|object\|` 记录 + 打包对象）仍按参考布局写 | 桶内布局与参考不同，跨引擎"同一桶互读"不成立 | `FORMAT_EQUIVALENCE_TESTS.md` §6.5 |
| DiskANN | 不移植 | 该索引类型不可用 | 计划书 §2.6/R14 |
| geo 类型（`Point`/`Line`/`LineSeg`/`Box`/`Circle`） | 不移植；`GeographyTryCastToVarlen` 随 cast 保留 | 这些列类型不可用 | `TEST_AUDIT.md` 的 `excluded` 行 |

## 2 未实现 / 未接线

当前没有未实现的功能缺口。相关子查询的执行方式与门禁如下：

- **相关子查询是 dependent join**（`infinity-executor/src/dependent.rs`）：`QueryBinder::bind_subquery` 的 `correlated_columns` 决定哪些嵌套查询读外层（参考的 `CorrelatedExpressionsDetector`），每个这样的嵌套查询被改写成隐藏扫描列 `#_corr_<n>`；外层关系物化后按 **distinct key** 求值内层（每个 key 一次，缓存复用），再把值拼回每一行。`WHERE`（`EXISTS`/`NOT EXISTS`/`IN`/`NOT IN`/标量比较）、投影（含投影表达式里的子查询）和 `HAVING` 都走普通列路径；`HAVING`/分组查询里要求相关列被 `GROUP BY` 决定，隐藏列随之进 `GROUP BY`（不改变分组）。外层关系可以是基表、CTE 或 JOIN 的 FROM 侧（隐藏列随左侧行进 join 源）。
- **参考侧没有可对齐的行为**：C++ 对 `SELECT (SELECT …) FROM o` 在 `FragmentTask::OnExecute` 里 `std::terminate`/abort（实测整机崩溃），对 `WHERE EXISTS (…)` 回 `Unknown error`；`JoinType::kMark` 只有 planner 生成、`src/executor` 没有对应算子。所以这里按参考 planner 的模型（`SubqueryUnnest` → `DependentJoinFlattener` 的 mark join + 按相关列 join）执行，而不是复刻一次崩溃。
- **测试**：`a_correlated_subquery_works_outside_the_where_clause`（投影/投影表达式/HAVING）、`a_correlated_subquery_runs_once_per_distinct_key`（4 行 2 个 key → 内层只执行 2 次，含 `IN`/`NOT IN`）、`a_correlated_subquery_reads_a_cte_or_joined_relation`（CTE 与 JOIN 左侧）、以及原有的 `a_correlated_subquery_is_evaluated_per_outer_row`、`a_correlated_subquery_sees_each_outer_row`。

压缩与 `IMPORT` 的 WAL 命令形态（`CompactV2` / `ImportV2` vs `[AppendV2, DeleteV2]` / `AppendV2`）是已登记的行为差异，记在 `FORMAT_EQUIVALENCE_TESTS.md` §6.1，不在这里重复。

其余各项的接线位置与门禁：

- **`IndexScanBuilder` 是优化器规则**（`infinity-planner` 的 `IndexScanRule`）：规则重写计划（谓词下推 + 残余过滤）并把候选（含求值器）通过共享槽交给执行器；测试 `the_index_scan_rule_publishes_its_candidate_and_keeps_the_residual`。
- **`ResultCacheGetter` 在生产管线里**：`result_cache` 配置打开时，`run_plan` 在优化之后跑 getter，扫描节点用 `cache_scan_rows` 填充 `SharedCache`，写路径清空；测试 `the_plan_cache_answers_a_repeated_scan_and_a_write_drops_it`。
- **`fast_rough_filter` 用真实 catalog**：`with_catalog` 是生产路径，且扫描按块的 `blk|…|fast_rough_filter` 记录跳过不可能命中的块（`Filters::decode` + `may_contain`）；测试 `the_block_prefilter_skips_blocks_that_cannot_match`、`a_decoded_record_refutes_only_what_it_can_prove`。
- **`compact_interval` 会触发压缩**：`notify_compact` 遍历库/表并调用 `compact_table`（`NewCompactionAlg` 的 DBT 选段）；测试 `notify_compact_merges_the_segments_the_alg_picks`。
- **全文分析器家族已接线**：`index_analyzer` → `AnalyzerPool`，建索引与查询用同一个分析器；测试 `a_fulltext_index_uses_its_declared_analyzer`（`ngram-3` 的 `qui` 命中、`standard` 不命中）。名称语法由 `parse_analyzer_name` 一处解析，DDL 校验与 `get_analyzer` 共用：族名大小写敏感、`chinese`/`ik`/`traditional` 只认精确 `-fine`、`keyword`/`whitespace` 的分隔符逐字节（`keyword-,` 按逗号切，索引侧同样生效）、`ngram` 走 `strtol`（`ngram-3x` 等于 `ngram-3`）、`sparsegram` 校验窗口（空段/非数字/第三个数字/>1024/`min==0`/`max<min` 均拒绝）；单测 `analyzer_name_grammar_matches_the_reference`、`analyzer_suffixes_reach_the_constructed_analyzers`、`a_keyword_fulltext_index_takes_its_delimiters_from_the_name`、`create_index_validates_the_analyzer_name_like_the_reference`，跨引擎门禁 `test_analyzer_names_and_tokenisation_match`。
- **稀疏检索选项**：`alpha`/`beta`/`threshold` 生效（`BmpSearchOptions` 传到磁盘 reader，含 `beta` 截断与 k-th 阈值），`use_tail` 像参考一样被 binder 拒绝、`tail` 被接受但不生效；门禁 `test_the_sparse_search_options_change_the_answer_the_same_way`。

## 3 不可逐字节比对（参考侧不确定）

| 项 | 原因 | 门禁处理 |
|---|---|---|
| HNSW 图区 | `thread_local std::mt19937`，节点层级取决于线程调度；实测同一输入五次跑出五种图 | header/vectors/labels 字节比，图与查询答案按内容比（`FORMAT_EQUIVALENCE_TESTS.md` §6.4） |
| IVF 质心 / 训练 | `std::random_device` 采样，质心个数也不固定 | chunk 按内容比 |
| `catalog/` 的字节 | 带 ts / LSN / session id，参考自己两次跑也不同 | 逻辑内容按键比 |
| NULL 行定长 slot、`VARCHAR` 内联 padding | 参考不写这些字节（未初始化内存，实测随前一次分配变化） | 新进程、该列首次写时逐字节相同；门禁按此安排 |
| 后台 dump 的 chunk 切分 | 配额驱动的队列 + 后台线程，切分是调度结果 | 关闭配额触发，只保留显式 `DumpIndex` 与行容量触发 |

`IVF` 的 `IndexIVFOption` 原先在这里：它作为整块 POD 写进 `CreateIndexV2`，5 个 padding 字节未初始化，参考自己两次运行就不同。现在这两段 padding 在参考里是**明写的字段**（`IndexIVFOption::padding_after_metric_`、`IndexIVFStorageOption::padding_after_storage_enums_`，布局不变、`sizeof` 仍为 32），端口按同一布局写出 0，于是 IVF 的 DDL 命令与其它索引一样进 `test_the_ddl_wal_commands_match_the_reference` 的逐字节比对（plain 与 `scalar_quantization` 各一条）。

## 4 覆盖不到的输入空间

细节（用例、规模、原因）在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5.2，这里只列名字：

| 项 | 为什么没覆盖 |
|---|---|
| 带索引的 8M 行表 | 只测了无索引的容量切分；带索引按 `mem_index_capacity` 切 chunk，分钟级 |
| 三段以上的容量切分 | 只测两段 |
| `COMPACT` 候选集 > 4 段 | 参考只合并 DBT 挑中的 4 个，重写版合并全部存活段 |
| PLAID / EMVB / MULTIVECTOR chunk 字节 | 只有重写版自身单测 |
| 快照（`CREATE … SNAPSHOT`） | 两种机制互不兼容，无跨引擎用例 |
| binary-fuse 半区（`BOOLEAN`/`DECIMAL` 的 `bloom_filter`） | 重写版固定写「存在但未构建」 |
| `IMPORT` 的 WAL 命令形态 | 参考写 `ImportV2`，重写版写等价的 `AppendV2` |
| IVF 的 **product-quantization** chunk | 端口写出的 PQ 码本/码区布局与参考不同：参考读端口目录再跑 `MATCH VECTOR` 会在 `PQ_Code_StorageT::ExtractCodes` 里越界（段错误）。plain 与 scalar-quantization 的 chunk 参考能读；`test_the_quantized_ivf_indexes_survive_a_crash` 因此只在端口自读时探测 `pq` |

## 5 已登记的行为差异（性能/口径，不是待办）

| 项 | 说明 | 出处 |
|---|---|---|
| 相关子查询的作用域边界 | 只支持引用**外层关系（FROM 侧）**的相关子查询：引用 JOIN 右侧表、引用派生表（parser 不保留派生表别名，标准 SQL 也要求限定）、或写在 `SEARCH` 的 `WHERE` filter 里时返回错误；不会静默按内层同名列求值（`substitute_outer_row` 只剥掉内层表自己的限定符） | `dependent.rs`、`substitute_outer_row` |
| 多词 / 短语 `MATCH` | 走「整行重新分词 + 逐行匹配」，结果与参考一致但不用索引（`filter_fulltext` 的索引过滤已构造真实 `Or`/`And` 树） | `CLASS_MAP.md`「有意的设计变更」 |
| `standard-porter` | 用 Porter2（`rust_stemmers::English`），少数词干与参考的 `STEM_LANG_PORT` 不同；其余 17 种语言同族一致 | 同上 |
| `SPARSE` 列的 `SHOW`/catalog 类型文本 | 存储字节一致，但渲染不同：C++ `Sparse(float,int8,0)`，Rust `Sparse(float,int32,8)`（元素类型一致） | `test_cross_engine_compat.py` 只比元素类型 |
