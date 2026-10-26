# 现状缺口清单

> **子目标 S7 · 缺口收口**｜纲领见 [`PORTING.md`](PORTING.md) §5

本文件是**唯一**的缺口汇总：哪些地方与参考不同、为什么、影响是什么。分四类，另有已登记的行为差异（§5，不是待办）：

- **§1 有意不移植**：已经决定不做，不是待办；
- **§2 未实现 / 未接线**：当前为空；出现时附证据与影响；
- **§3 不可逐字节比对**：参考自身不确定或依赖实现细节，门禁只能按内容比；
- **§4 覆盖不到的输入空间**：机制有了，门禁还没跑到那个规模/形态。

格式层面的逐文件细节在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5；逐类/逐用例的映射状态在
[`CLASS_MAP.md`](CLASS_MAP.md) §2（`mapped` / `partial` / `excluded` 行）与 [`TEST_AUDIT.md`](TEST_AUDIT.md)（`excluded` 行，对应本文 §1 的不移植决定）。
**已接受的设计变更**（不是待办）登记在 [`CLASS_MAP.md`](CLASS_MAP.md)「有意的设计变更」，本文不重复。

---

## 1 有意不移植

| 项 | 决定 | 影响 | 依据 |
|---|---|---|---|
| 远程对象存储（`obj_key` 打包 + VirtualStore 上传、catalog 的 `meta/` 路径树同步） | 不移植；本地对象目录（`pm\|object\|` 记录 + 打包对象）仍按参考布局写 | 桶内布局与参考不同，跨引擎"同一桶互读"不成立 | `FORMAT_EQUIVALENCE_TESTS.md` §6.5 |
| DiskANN | 不移植 | 该索引类型不可用 | 计划书 §2.6/R14 |
| geo 类型（`Point`/`Line`/`LineSeg`/`Box`/`Circle`） | 不移植；`GeographyTryCastToVarlen` 随 cast 保留 | 这些列类型不可用 | `TEST_AUDIT.md` 的 `excluded` 行 |

## 2 未实现 / 未接线

**现在有两项（都是并发对齐的待办）**：

| 待办 | 参考的做法 | 端口现在 |
|---|---|---|
| 执行器的多 fragment 与 tasklet（D4） | `FragmentBuilder` 把计划切成并行流 / 串行 materialize / merge 若干 fragment，之间用交换队列；`FragmentContext::CreateTasks` 按 `min(cpu_count, first_operator->TaskletCount())` 建 tasklet，scan 的 tasklet 数就是块数（`fragment_context_impl.cpp:1215-1245`），`TaskletCount()` 默认 1、filter/project/limit/top/sort 透传左侧、其余为 1（`physical_operator_impl.cpp:47` 起）；达到 merge 点时在 `physical_planner` 里换成 merge 算子（`physical_planner_impl.cpp:790/841`） | 已落地：`tasklet_count` / `block_tasklets` 按同一套规则解析整条链（`operators/mod.rs`）；行内算子（filter/project）按 `min(cpu_count, 块数)` 切 tasklet，各用链副本（`PhysicalOperator::fork`）跑连续行区间，输出经**有界交换队列**（每 tasklet 一条通道，容量 2 个 chunk）按 tasklet 序交给 merge 任务，merge 任务边收边算、收够即停（LIMIT 会连带停掉上游 tasklet）；tasklet 的报错也走通道上抛。仍待办：`EXPLAIN FRAGMENT` 除 SEARCH 路径外仍不出 fragment 文本（`explain.rs:1805` 只是对齐用例的 fixture） |
| 事务下半部的执行者与可见顺序 | 一个事务的下半部只由**一个** worker 跑完：`SendToWAL` 把 txn 交给 `BottomExecutor`，按 `crc32(table_id) % n` 分进 `max(2, hardware_concurrency()/2)` 条队列（`bottom_executor_impl.cpp:32-78`、`config_impl.cpp:562-567`），同表严格有序、只有跨表才并行；`CommitBottom` 内部按 WAL 命令 → 块对齐区间 → 列 → 索引逐个串行（`new_txn_impl.cpp:4578-4601`、`new_txn_data_impl.cpp:1840-1907`），上半部在 `commit_cv_` 上等它（`new_txn_impl.cpp:2138-2139`）。下半部里唯一的池是全文内存索引的追加：非实时 `MemIndexAppender::Submit`（`new_txn_index_impl.cpp:891-894`），实时走 `AsyncInsert` 的信号量，`PostCommit` 全部 `acquire` 完才 `SetCompletion` | 可见顺序已按参考落地（`register_bottom` / `finish_bottom`，时钟从队首按 commit_ts 推进，冲突也走同一屏障）。下半部本身同样是在一个线程上跑完行 → 索引 → 块镜像，与参考"每事务一个线程串行"同形；差别只在跨表——参考按表分片各跑各的，端口所有表共用 `Engine::tables` 一把锁（`update_indexes_on_insert_text`），且全文追加在发布前同步做完、没走池。已按参考落地：`BottomExecutor`（`infinity-storage` 的移植）挂在 `Engine` 上，`start_background_tasks` 按 `max(2, hardware_concurrency()/2)` 启动；`Engine::run_commit_bottom` 把每个语句的下半部（INSERT / DELETE·UPDATE / IMPORT / DumpMemIndex / `EngineTxn::commit`）包成 `BottomWork` 按 `crc32(table_id)` 分片投递，调用方等 condvar——即 `SendToWAL → Submit → CommitBottom` + `commit_cv_`；worker 内取该表 gate，同表按提交序、异表并行。无池或引擎不在 `Arc` 里时走内联。仍待办：全文追加的后台队列（`MemIndexAppender`） |

原先记在这一节里的接线项——相关子查询、压缩/`IMPORT` 的 WAL 帧、DDL 的 typed 命令、`COMPACT` 的候选集、`IndexScanBuilder`、`ResultCacheGetter`、`MATCH TEXT` 走倒排、`fast_rough_filter`、`compact_interval`、分析器家族、稀疏检索选项、快照——已经接线，记录在 [`CLASS_MAP.md`](CLASS_MAP.md)（类/方法级）与 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §6（格式与门禁级）。

## 3 不可逐字节比对（参考侧不确定）

| 项 | 原因 | 门禁处理 |
|---|---|---|
| HNSW 图区 | `thread_local std::mt19937`，节点层级取决于线程调度；实测同一输入五次跑出五种图 | header/vectors/labels 字节比，图与查询答案按内容比（`FORMAT_EQUIVALENCE_TESTS.md` §6.4） |
| IVF 质心 / 训练 | `std::random_device` 采样，质心个数也不固定 | chunk 按内容比 |
| `catalog/` 的字节 | 带 ts / LSN / session id，参考自己两次跑也不同 | 逻辑内容按键比 |
| NULL 行定长 slot、`VARCHAR` 内联 padding | 参考不写这些字节（未初始化内存，实测随前一次分配变化） | 新进程、该列首次写时逐字节相同；门禁按此安排 |
| 后台 dump 的 chunk 切分 | 配额驱动的队列 + 后台线程，切分是调度结果 | 关闭配额触发，只保留显式 `DumpIndex` 与行容量触发 |

## 4 覆盖不到的输入空间

细节（用例、规模、原因）在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5.2，这里只列名字：

| 项                     | 为什么没覆盖                                                                                                                                                     |
| --------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| PLAID / EMVB chunk 字节 | 两者的训练都含 `std::random_device`（PLAID 的 k-means 初始化、EMVB 的残差 PQ 训练），只能按内容比而不是逐字节；EMVB 另外要超过 `build_index_threshold_`（8192 个 embedding）才会建内存索引，小表上两引擎都不产 chunk |

SMVE 已经闭口：`test_the_smve_chunk_matches_byte_for_byte`（500 行 `TENSOR(FLOAT,4)`）逐字节相同，前提是参考侧 `SMVEIndexInMem::BuildFromColumn` 把 `GetTensorRaw` 的第二个返回值当**token 数**而不是字节数（参考里原来除错了，chunk 因此是空的）。

## 5 已登记的行为差异（性能/口径，不是待办）

| 项 | 说明 | 出处 |
|---|---|---|
| 相关子查询的作用域边界 | 只支持引用**外层关系（FROM 侧）**的相关子查询：引用 JOIN 右侧表、引用派生表（parser 不保留派生表别名，标准 SQL 也要求限定）、或写在 `SEARCH` 的 `WHERE` filter 里时返回错误；不会静默按内层同名列求值（`substitute_outer_row` 只剥掉内层表自己的限定符） | `dependent.rs`、`substitute_outer_row` |
| `CREATE TABLE … AS SELECT` | 不实现（参考也没有）：`CREATE TABLE t AS SELECT …` 返回参考的 `Status::NoColumnDefined`（`Try to define Table t without any column.`，3048）；带列定义的 `CREATE TABLE t (a INT) AS SELECT …` 在参考是解析错误（`unexpected AS, expecting end of file`），端口同样拒绝 | `select_ddl_ut::create_table_as_select_is_rejected_like_the_reference` |
| `CREATE VIEW` / `DROP VIEW` | 不实现（参考也没有实现）：`PhysicalCreateView::Execute` 是空壳、DDL 分发没有这个分支，两侧都死在 `UnrecoverableException("Unknown DDL type")`（`query_context_impl.cpp:467`/`:489`），视图不会被登记，`FROM v` 随后报表不存在 | `select_ddl_ut::views_are_rejected_like_the_reference` |
| `INSERT … SELECT` | 只支持 select list 全为常量：参考的 `PhysicalInsert` 把投影表达式交给 `ExpressionState::CreateState`，那里没有 `Column` 分支，任何列引用（含 `SELECT *`）都会死在该处（`expression_state_impl.cpp:68`）。端口报同一条 `Unknown expression type: Column. expression name: {col}`，常量情形与参考一样按源行数插入 | `select_ddl_ut::insert_select_is_refused_when_the_select_list_reads_a_column` |
| `CREATE SNAPSHOT … ON TABLE <db>.<tbl>` | 参考的文法是 `IDENTIFIER`，带库名的写法是解析错误（`unexpected '.', expecting end of file`）；端口接受限定名（HTTP 层就按这个拼），无库名时用会话库 | `infinity-parser` 的 `CREATE SNAPSHOT` 分支（`parser.rs:362`） |
| `standard-porter` | 用 Porter2（`rust_stemmers::English`），少数词干与参考的 `STEM_LANG_PORT` 不同；其余 17 种语言同族一致 | `CLASS_MAP.md`「有意的设计变更」 |

