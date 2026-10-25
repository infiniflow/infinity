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

当前没有未实现的功能缺口。原先记在这一节里的接线项——相关子查询、压缩/`IMPORT` 的 WAL 帧、DDL 的 typed 命令、`COMPACT` 的候选集、`IndexScanBuilder`、`ResultCacheGetter`、`MATCH TEXT` 走倒排、`fast_rough_filter`、`compact_interval`、分析器家族、稀疏检索选项、快照——现在各自记录在 [`CLASS_MAP.md`](CLASS_MAP.md)（类/方法级）与 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §6（格式与门禁级），本文不再重复。

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
| 执行器的并行度（D4） | 端口不拆 fragment、没有 tasklet：`run_fragment` 在单线程里跑完整条算子链（`operators/mod.rs:342`），`EXPLAIN FRAGMENT` 里的 `Fragment #N * M Task` 是单测 fixture 的文本（`explain.rs:1805`）。参考的 `FragmentContext::CreateTasks` 按 `min(cpu_count, first_operator->TaskletCount())` 建 tasklet、scan 的 tasklet 是块数（`fragment_context_impl.cpp:1215-1245`），fragment 之间用交换队列；`PhysicalOperator::TaskletCount()` 默认 1，filter/project/limit 透传左侧、match/merge/projection 强制 1。**影响**：结果一致，扫描/连接的并行度不同 | `CLASS_MAP.md`「有意的设计变更」、`operators/mod.rs:342` |
| 事务下半部的执行者与可见顺序 | 端口把冲突检测、WAL 追加、KV 批量写与 `max_applied` 全放在 `commit_at` 的同一把 `commit_lock` 里（`infinity-txn/src/lib.rs:458`），执行顺序 = 抢锁顺序，而 commit ts 在抢锁**之前**分配；索引更新在 `commit_at` 返回、锁释放之后由调用线程逐行做。参考是每个事务在自己的线程里做 bottom（`NewTxn::CommitBottom` → `CommitBottomAppend`，单事务内对 append range 顺序循环，`new_txn_data_impl.cpp:1840`），manager 用 `bottom_txns_`（按 commit_ts 排序）从队首推进 `current_ts_` 并 `NotifyTopHalf`（`new_txn_manager_impl.cpp:410-455`），所以**可见性**按 commit_ts 有序、bottom 工作本身并发。索引 insert 属于 bottom 工作（`AppendIndex` 在 `CommitBottomAppend` 里，`new_txn_data_impl.cpp:1887`），不是上半部。**影响**：并发客户端下端口可能出现 ts 大的事务先对读者可见，且同一张表的索引更新可以在锁外交错 | `infinity-txn/src/lib.rs:458`、`new_txn_manager_impl.cpp:410` |

