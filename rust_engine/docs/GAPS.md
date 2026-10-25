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

压缩与 `IMPORT` 的 WAL 帧与参考同构：
- 压缩是一帧一条 `CompactV2`（合并段 + 废弃段 id + 表的 index 名/id，不搬行），`IMPORT` 是一帧一条 `ImportV2`（新段信息），两条命令的载荷与参考**逐字节相同**（`wal_shape_ut` 用参考 `wal.log` 里取出的字节做 fixture）；行写进 KV/块，不出现在 WAL 里。
- 带索引的表在同一帧里再追加每个 index 的 `DumpIndexV2`（`chunk_id` 0、`base_row_id` 段基址、`row_count` 段行数、`deprecate_ts` u64::MAX；全文索引的 `base_name` 是 `ft_<base>`、`term_count` 是 `(term, row)` posting 数，其余索引留空）；导入帧的 `table_key` 是表的 catalog 键，压缩帧为空（`CompactTxnStore` 不设它）。
- 一条 `COPY` 是一个事务、一帧（解析仍按 8192 行一批，写入同一个 `Txn`）；只有超过 `IMPORT_TXN_ROWS`（1,048,576 行）的压测导入才会分批提交，那是内存上限而不是格式选择。
- DDL 的 typed 命令在**每次** DDL 上都会写出：原生 catalog 的写和参考的 C++ 布局在同一个 commit 里落盘（`Catalog::capture` 在 `f` 之后重写一次布局，把改写记进捕获的 batch），`WalCmdCreateTableV2`/`CreateIndexV2`/… 就是从那个 batch 派生出来的。在这之前布局只在 checkpoint 时镜像，DDL 事务的 batch 是空的，typed DDL 帧整类丢失（`test_the_ddl_wal_commands_match_the_reference` 会看到 0 帧）。
- 压缩只合并 `NewCompactionAlg` 挑中的段，其余段原地不动：`COMPACT` 走 `Engine::compact_table`，候选由 DBT 规则给出（先剔除未封段的段——`INSERT` 还在追加的那个；同层最少 4 段、取该层前 4 个、行数之和不超过段容量），合并后的段号仍是「全部段的最大号 + 1」，被合并的段进 `drop|seg|` 等 `Cleanup`。实测参考对 6 个段说 `Compact segments 0,1,2,3 into new segment`，端口同样留下 `{4,5,6}`，`seg_6` 含 4 个块、`seg_4`/`seg_5` 各 1 个块，`seg_0…3` 的文件保留（测试 `a_compaction_of_more_than_four_segments_keeps_the_rest`）。

其余各项的接线位置与门禁：

- **`IndexScanBuilder` 是优化器规则**（`infinity-planner` 的 `IndexScanRule`）：规则重写计划（谓词下推 + 残余过滤）并把候选（含求值器）通过共享槽交给执行器；测试 `the_index_scan_rule_publishes_its_candidate_and_keeps_the_residual`。
- **`ResultCacheGetter` 在生产管线里**：`result_cache` 配置打开时，`run_plan` 在优化之后跑 getter，扫描节点用 `cache_scan_rows` 填充 `SharedCache`，写路径清空；测试 `the_plan_cache_answers_a_repeated_scan_and_a_write_drops_it`。
- **`fast_rough_filter` 用真实 catalog**：`with_catalog` 是生产路径，且扫描按块的 `blk|…|fast_rough_filter` 记录跳过不可能命中的块（`Filters::decode` + `may_contain`）；测试 `the_block_prefilter_skips_blocks_that_cannot_match`、`a_decoded_record_refutes_only_what_it_can_prove`。binary-fuse（bloom）半区在参考里**永远不会被构建**：`CREATE TABLE … PROPERTIES (bloom_filter_columns = '…')` 只把 `build_bloom_filter_` 设在内存里的 `ColumnDef` 上（`logical_planner_impl.cpp:690`），而 `ColumnDef::ToJson`（`column_def.cpp:229`）不写这个字段，于是封段事务从 `tbl|col|…` 读回列定义时看到的是 `false`，`BuildOnlyBloomFilter` 不会执行。实测：`BOOLEAN` 列声明了该属性、导入两次，两个段的 `seg|…|fast_rough_filter` 里该列仍是 `1, 0`（`exists=1, built=0`）。端口因此也写 `1, 0`；DDL 层仍然照参考接受/校验这个属性（未知列、不支持的类型、重复属性名的三条报错与 3013），并且把 flag 写进 `CreateTableV2`（`ColumnDef::WriteAdv` 里它是唯一的持久化位置）。测试 `a_bloom_filter_column_is_accepted_but_never_built`、`a_create_table_logs_the_bloom_flag`。
- **`compact_interval` 会触发压缩**：`notify_compact` 遍历库/表并调用 `compact_table`（`NewCompactionAlg` 的 DBT 选段）；测试 `notify_compact_merges_the_segments_the_alg_picks`。
- **全文分析器家族已接线**：`index_analyzer` → `AnalyzerPool`，建索引与查询用同一个分析器；测试 `a_fulltext_index_uses_its_declared_analyzer`（`ngram-3` 的 `qui` 命中、`standard` 不命中）。名称语法由 `parse_analyzer_name` 一处解析，DDL 校验与 `get_analyzer` 共用：族名大小写敏感、`chinese`/`ik`/`traditional` 只认精确 `-fine`、`keyword`/`whitespace` 的分隔符逐字节（`keyword-,` 按逗号切，索引侧同样生效）、`ngram` 走 `strtol`（`ngram-3x` 等于 `ngram-3`）、`sparsegram` 校验窗口（空段/非数字/第三个数字/>1024/`min==0`/`max<min` 均拒绝）；单测 `analyzer_name_grammar_matches_the_reference`、`analyzer_suffixes_reach_the_constructed_analyzers`、`a_keyword_fulltext_index_takes_its_delimiters_from_the_name`、`create_index_validates_the_analyzer_name_like_the_reference`，跨引擎门禁 `test_analyzer_names_and_tokenisation_match`。
- **稀疏检索选项**：`alpha`/`beta`/`threshold` 生效（`BmpSearchOptions` 传到磁盘 reader，含 `beta` 截断与 k-th 阈值），`use_tail` 像参考一样被 binder 拒绝、`tail` 被接受但不生效；门禁 `test_the_sparse_search_options_change_the_answer_the_same_way`。
- **快照的时间戳两侧都记录并展示**：`SnapshotInfo::max_commit_ts_` 写进 `<name>.json`（`SnapshotInfo` 的基类字段，三种 scope 都有），`SnapshotBrief::GetSnapshots` 从 `max_commit_ts` 读回，`SHOW SNAPSHOTS` 的 `commit` 与 `SHOW SNAPSHOT` 的 `commit_timestamp` 于是都有确定值。参考侧为此改了三处：`MapMetaToSnapShotInfo`/`GetDatabaseSnapshotInfo` 填 ts（`new_txn_impl.cpp`）、`DatabaseSnapshotInfo`/`SystemSnapshotInfo::CreateSnapshotMetadataJSON` 输出 `max_commit_ts`、`SnapshotBrief` 读它（`snapshot_brief_impl.cpp:102`）——原来那几行是注释掉的，列里打印的是未初始化内存。
- **快照按参考的目录布局与 WAL 命令实现**（`infinity-executor/src/snapshot.rs`）：`<snapshot_dir>/<name>/<name>.json` + 数据文件副本，`CREATE` 写 110/111/112、`RESTORE` 写 113/114/115（`WalCmd::CreateTableSnapshot` 等六个 typed 命令，replay 与参考一样跳过 create），`SHOW SNAPSHOTS`/`SHOW SNAPSHOT` 读目录（`SnapshotBrief`），`DROP SNAPSHOT` 先 rename 成 `deleted_<name>` 再删；建快照前先跑一次块镜像（参考的 `CheckpointforSnapshot`），恢复时把文件按新 id 复制回去，再用 `restore_cpp_blocks` 从块文件重建行日志——目录里不写任何参考没有的文件。`table_indexes[].segment_indexes` 从 catalog 的 `idx_chunk|…|chunk_info` 记录读（`ChunkIndexMeta::MapMetaToSnapShotInfo`，与参考同一个账本：`base_row_id`/`row_cnt`/`index_size`/全文的 `base_name`），`index_filename` 统一是 `chunk_{id}.idx`、全文分块再由 `full_text_files` 列三份文件；`index_base` 用完整的 `IndexDefValue`（含 SECONDARY 的 `cardinality`/`is_json_index`）。测试 `snapshot_ut::*`（元数据 JSON 逐字节、含索引的表与参考同形、嵌套、删除、恢复行、报错文案）与 `wal_entry::tests::create_snapshot_commands_write_the_reference_layout`。

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
| PLAID / EMVB / MULTIVECTOR chunk 字节 | 只有重写版自身单测 |

## 5 已登记的行为差异（性能/口径，不是待办）

| 项 | 说明 | 出处 |
|---|---|---|
| 相关子查询的作用域边界 | 只支持引用**外层关系（FROM 侧）**的相关子查询：引用 JOIN 右侧表、引用派生表（parser 不保留派生表别名，标准 SQL 也要求限定）、或写在 `SEARCH` 的 `WHERE` filter 里时返回错误；不会静默按内层同名列求值（`substitute_outer_row` 只剥掉内层表自己的限定符） | `dependent.rs`、`substitute_outer_row` |
| `CREATE TABLE … AS SELECT` | 不实现（参考也没有）：`CREATE TABLE t AS SELECT …` 返回参考的 `Status::NoColumnDefined`（`Try to define Table t without any column.`，3048）；带列定义的 `CREATE TABLE t (a INT) AS SELECT …` 在参考是解析错误（`unexpected AS, expecting end of file`），端口同样拒绝 |
| `CREATE VIEW` / `DROP VIEW` | 不实现（参考也没有实现）：`PhysicalCreateView::Execute` 是空壳、DDL 分发没有这个分支，两侧都死在 `UnrecoverableException("Unknown DDL type")`（`query_context_impl.cpp:467`/`:489`），视图不会被登记，`FROM v` 随后报表不存在 |
| `INSERT … SELECT` | 只支持 select list 全为常量：参考的 `PhysicalInsert` 把投影表达式交给 `ExpressionState::CreateState`，那里没有 `Column` 分支，任何列引用（含 `SELECT *`）都会死在该处（`expression_state_impl.cpp:68`）。端口报同一条 `Unknown expression type: Column. expression name: {col}`，常量情形与参考一样按源行数插入 |
| `CREATE SNAPSHOT … ON TABLE <db>.<tbl>` | 参考的文法是 `IDENTIFIER`，带库名的写法是解析错误（`unexpected '.', expecting end of file`）；端口接受限定名（HTTP 层就按这个拼），无库名时用会话库 |
| 多词 / 短语 `MATCH` | 走「整行重新分词 + 逐行匹配」，结果与参考一致但不用索引（`filter_fulltext` 的索引过滤已构造真实 `Or`/`And` 树） | `CLASS_MAP.md`「有意的设计变更」 |
| `standard-porter` | 用 Porter2（`rust_stemmers::English`），少数词干与参考的 `STEM_LANG_PORT` 不同；其余 17 种语言同族一致 | 同上 |
| `SPARSE` 列的 `SHOW`/catalog 类型文本 | 存储字节一致，但渲染不同：C++ `Sparse(float,int8,0)`，Rust `Sparse(float,int32,8)`（元素类型一致） | `test_cross_engine_compat.py` 只比元素类型 |
